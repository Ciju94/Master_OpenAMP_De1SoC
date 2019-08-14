/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2017 Xilinx, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**************************************************************************
 * FILE NAME
 *
 *       platform_info.c
 *
 * DESCRIPTION
 *
 *       This file define platform specific data and implements APIs to set
 *       platform specific information for OpenAMP.
 *
 **************************************************************************/

#include <stdio.h>

#include <atomic.h>
#include <metal_assert.h>
#include <device.h>
#include <irq.h>
#include <utilities.h>

#include <lib/include/openamp/rpmsg_virtio.h>

#include "platform_info.h"
#include "rsc_table.h"

#include "atomic_mutex.h"

#define PROC_ID				ALT_CPU_CPU_ID_VALUE
#define RSC_ID				ALT_CPU_CPU_ID_VALUE

#define IPI_DEV_NAME        "ipi_dev"
#define IPI_BUS_NAME        "generic"
//#define IPI_BASE_ADDR        XPAR_XIPIPSU_0_BASE_ADDRESS /* IPI base address*/
#define IPI_CHN_BITMASK     0x01000000 /* IPI channel bit mask for IPI from/to APU */

#define SHARED_MEM_PA  		0x01C00000 // 0x0100.0000 + 0x0080.0000 + 0x0040.0000
#define SHARED_MEM_SIZE 	0x00400000 //0x100000UL
#define SHARED_BUF_OFFSET 	0x8000UL

#define _rproc_wait() asm volatile("nop")

/* IPI information used by remoteproc operations.
 */
static metal_phys_addr_t ipi_phys_addr = 0; //IPI_BASE_ADDR;

struct metal_device ipi_device = {
	.name = IPI_DEV_NAME,
	.bus = NULL,
	.num_regions = 1,
	.regions = {
		{
			.virt = (void*)NULL,//IPI_BASE_ADDR, SMATRAMO DA NIJE BITNA
			.physmap = &ipi_phys_addr,
			.size = 0x1000,
			.page_shift = -1UL,
			.page_mask = -1UL,
			.mem_flags = DEVICE_NONSHARED | PRIV_RW_USER_RW,
			.ops = {NULL},
		}
	},
	.node = {NULL},
	.irq_num = 1,
	.irq_info = (void *)IPI_IRQ_VECT_ID,
};

static struct remoteproc_priv rproc_priv = {
	.ipi_name = IPI_DEV_NAME,
	.ipi_bus_name = IPI_BUS_NAME,
	.ipi_chn_mask = IPI_CHN_BITMASK,
};

static struct remoteproc rproc_inst;

/* External functions */
extern int init_system(void);
extern void cleanup_system(void);

/* processor operations from r5 to a53. It defines
 * notification operation and remote processor management operations. */
extern struct remoteproc_ops zynqmp_r5_a53_proc_ops;

/* RPMsg virtio shared buffer pool */
static struct rpmsg_virtio_shm_pool shpool;

static struct remoteproc *
platform_create_proc(int proc_index, int rsc_index)
{
	void *rsc_table;
	int rsc_size;
	int ret;
	metal_phys_addr_t pa;

	(void) proc_index;
	rsc_table = get_resource_table(rsc_index, &rsc_size);

	/* Register IPI device */
	(void)metal_register_generic_device(&ipi_device);

	/* Initialize remoteproc instance */
	if (!remoteproc_init(&rproc_inst, &zynqmp_r5_a53_proc_ops, &rproc_priv))
		return NULL;

	/*
	 * Mmap shared memories
	 * Or shall we constraint that they will be set as carved out
	 * in the resource table?
	 */
	/* mmap resource table */
	pa = (metal_phys_addr_t)rsc_table;
	(void *)remoteproc_mmap(&rproc_inst, &pa,
				NULL, rsc_size,
				NORM_NSHARED_NCACHE|PRIV_RW_USER_RW,
				&rproc_inst.rsc_io);

	/* mmap shared memory */
	pa = SHARED_MEM_PA;
	(void *)remoteproc_mmap(&rproc_inst, &pa,
				NULL, SHARED_MEM_SIZE,
				NORM_NSHARED_NCACHE|PRIV_RW_USER_RW,
				NULL);

	/* parse resource table to remoteproc */
	ret = remoteproc_set_rsc_table(&rproc_inst, rsc_table, rsc_size);
	if (ret) {
		printf("Failed to intialize remoteproc\r\n");
		remoteproc_remove(&rproc_inst);
		return NULL;
	}

	printf("Initialize remoteproc successfully.\r\n");

	return &rproc_inst;
}

int platform_init(void **platform)
{
	struct remoteproc *rproc;

	if (!platform) {
		printf("Failed to initialize platform,"
			   "NULL pointer to store platform data.\r\n");
		return -EINVAL;
	}
	/* Initialize HW system components */
	init_system();

	rproc = platform_create_proc(PROC_ID, RSC_ID);
	if (!rproc) {
		printf("Failed to create remoteproc device.\r\n");
		return -EINVAL;
	}

	*platform = rproc;
	return 0;
}

struct  rpmsg_device *
platform_create_rpmsg_vdev(void *platform, unsigned int vdev_index,
			   unsigned int role,
			   void (*rst_cb)(struct virtio_device *vdev),
			   rpmsg_ns_bind_cb ns_bind_cb)
{
	struct remoteproc *rproc = platform;
	struct rpmsg_virtio_device *rpmsg_vdev;
	struct virtio_device *vdev;
	void *shbuf;
	struct metal_io_region *shbuf_io;
	int ret;

	rpmsg_vdev = metal_allocate_memory(sizeof(*rpmsg_vdev));
	if (!rpmsg_vdev)
		return NULL;

	shbuf_io = remoteproc_get_io_with_pa(rproc, SHARED_MEM_PA);
	if (!shbuf_io)
		return NULL;

	shbuf = metal_io_phys_to_virt(shbuf_io,
				      SHARED_MEM_PA + SHARED_BUF_OFFSET);

	printf("creating remoteproc virtio\r\n");
	/* TODO: can we have a wrapper for the following two functions? */
	vdev = remoteproc_create_virtio(rproc, vdev_index, role, rst_cb);
	if (!vdev) {
		printf("failed remoteproc_create_virtio\r\n");
		goto err1;
	}

	printf("initializing rpmsg shared buffer pool\r\n");
	/* Only RPMsg virtio master needs to initialize the shared buffers pool */
	rpmsg_virtio_init_shm_pool(&shpool, shbuf,
				   (SHARED_MEM_SIZE - SHARED_BUF_OFFSET));

	printf("initializing rpmsg vdev\r\n");
	/* RPMsg virtio slave can set shared buffers pool argument to NULL */
	ret =  rpmsg_init_vdev(rpmsg_vdev, vdev, ns_bind_cb,
			       shbuf_io,
			       &shpool);
	if (ret) {
		printf("failed rpmsg_init_vdev\r\n");
		goto err2;
	}
	printf("initializing rpmsg vdev\r\n");
	return rpmsg_virtio_get_rpmsg_device(rpmsg_vdev);
err2:
	remoteproc_remove_virtio(rproc, vdev);
err1:
	metal_free_memory(rpmsg_vdev);
	return NULL;
}

int platform_poll(void *priv)
{
	struct remoteproc *rproc = priv;
	struct remoteproc_priv *prproc;
	unsigned int flags;

	prproc = rproc->priv;

	while(1) {
		flags = metal_irq_save_disable();

		atomic_int val;

		atomic_mutex_acquire();
		val = atomic_flag_test_and_set(&prproc->ipi_nokick);
		atomic_mutex_release();

		if (!val) {
			metal_irq_restore_enable(flags);
			remoteproc_get_notification(rproc, RSC_NOTIFY_ID_ANY);
			break;
		}

		_rproc_wait();
		metal_irq_restore_enable(flags);
	}
	return 0;
}

void platform_release_rpmsg_vdev(struct rpmsg_device *rpdev)
{
	(void)rpdev;
}

void platform_cleanup(void *platform)
{
	struct remoteproc *rproc = platform;

	if (rproc)
		remoteproc_remove(rproc);
	cleanup_system();
}