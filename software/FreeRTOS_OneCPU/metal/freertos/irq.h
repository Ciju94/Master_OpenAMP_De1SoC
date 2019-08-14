/*
 * Copyright (c) 2016, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	freertos/irq.c
 * @brief	FreeRTOS libmetal irq definitions.
 */

#ifndef __METAL_IRQ__H__
#error "Include irq.h instead of freertos/irq.h"
#endif

#ifndef __METAL_FREERTOS_IRQ__H__
#define __METAL_FREERTOS_IRQ__H__

/* We need to find the internal MAX_IRQS limit */
/* Could be retrieved from platform specific files in the future */
#define METAL_INTERNAL		ALT_NIRQ

#endif /* __METAL_FREERTOS_IRQ__H__ */
