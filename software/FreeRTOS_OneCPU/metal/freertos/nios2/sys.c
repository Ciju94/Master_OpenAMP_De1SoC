/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * Copyright (c) 2016, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	freertos/nios2/sys.c
 * @brief	machine specific system primitives implementation.
 */

//////////////////////////
/* NEEDS TO BE EDITED! */
/////////////////////////


#include <metal_io.h>
#include <sys.h>

#include <stdint.h>

#include "sys/alt_cache.h"
#include "sys/alt_irq.h"

//#include "xil_exception.h"
//#include "xil_mmu.h"
//#include "xscugic.h"

/* Translation table is 16K in size */
//#define     ARM_AR_MEM_TTB_SIZE                    16*1024

/* Each TTB descriptor covers a 1MB region */
//#define     ARM_AR_MEM_TTB_SECT_SIZE               1024*1024

/* Mask off lower bits of addr */
//#define     ARM_AR_MEM_TTB_SECT_SIZE_MASK          (~(ARM_AR_MEM_TTB_SECT_SIZE-1UL))

void sys_irq_restore_enable(unsigned int flags)
{
	alt_irq_enable_all(~flags);
}

unsigned int sys_irq_save_disable(void)
{
	unsigned int state;

	state = alt_irq_disable_all();

	return state;
}

void metal_machine_cache_flush(void *addr, unsigned int len)
{
	if (!addr && !len)
		alt_dcache_flush_all();
	else
		alt_dcache_flush(addr, len);
}

void metal_machine_cache_invalidate(void *addr, unsigned int len)
{
	/*if (!addr && !len)
		Xil_DCacheInvalidate();
	else
		Xil_DCacheInvalidateRange((intptr_t)addr, len);*/
}

/**
 * @brief poll function until some event happens
 */
void metal_weak metal_generic_default_poll(void)
{
	asm volatile("nop");
}

void *metal_machine_io_mem_map(void *va, metal_phys_addr_t pa,
				      size_t size, unsigned int flags)
{
	(void)pa;
	(void)size;
	(void)flags;

	return va;
}
