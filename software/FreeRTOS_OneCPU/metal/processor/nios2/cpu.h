/*
 * Copyright (c) 2017, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	cpu.h
 * @brief	CPU specific primatives on microblaze platform.
 */

//////////////////////////
/* NEEDS TO BE EDITED! */
/////////////////////////

#ifndef __METAL_NIOS_II__H__
#define __METAL_NIOS_II__H__

#include <stdint.h>

#include <atomic.h>

#define metal_cpu_yield()			asm("nop") // !!

#endif /* __METAL_NIOS_II__H__ */
