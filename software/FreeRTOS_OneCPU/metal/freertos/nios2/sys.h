/*
 * Copyright (c) 2016, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	freertos/nios2/sys.h
 * @brief	freertos nios2 system primitives for libmetal.
 */

#ifndef __METAL_FREERTOS_SYS__H__
#error "Include freertos/sys.h instead of freertos/nios2/sys.h"
#endif

#include <irq.h>
#include <freertos/nios2/common/sys.h>

#include "sys/alt_irq.h"

#ifndef __METAL_FREERTOS_NIOS_II_SYS__H__
#define __METAL_FREERTOS_NIOS_II_SYS__H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef METAL_INTERNAL

#define NIOS_MAXIRQS 	NIOS2_NIRQ
#define	IC_ID_IGNORE 	0

static inline void sys_irq_enable(unsigned int vector)
{
	alt_ic_irq_enable(IC_ID_IGNORE, (alt_u32)vector);
}

static inline void sys_irq_disable(unsigned int vector)
{
	alt_ic_irq_disable(IC_ID_IGNORE, (alt_u32)vector);
}

#endif /* METAL_INTERNAL */

#ifdef __cplusplus
}
#endif

#endif /* __METAL_FREERTOS_NIOS_II_SYS__H__ */
