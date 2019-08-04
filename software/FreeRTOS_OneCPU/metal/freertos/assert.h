/*
 * Copyright (c) 2018, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	assert.h
 * @brief	FreeRTOS assertion support.
 */
#ifndef __METAL_ASSERT__H__
#error "Include assert.h instead of freertos/assert.h"
#endif

#ifndef __METAL_FREERTOS_ASSERT__H__
#define __METAL_FREERTOS_ASSERT__H__

#include <C:/intelFPGA_lite/17.0/nios2eds/bin/gnu/H-x86_64-mingw32/nios2-elf/include/assert.h>

/**
 * @brief Assertion macro for FreeRTOS applications.
 * @param cond Condition to evaluate.
 */
#define metal_sys_assert(cond) assert(cond)

#endif /* __METAL_FREERTOS_ASSERT__H__ */

