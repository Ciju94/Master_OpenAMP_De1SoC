/*
 * Copyright (c) 2015, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	atomic.h
 * @brief	Atomic primitives for libmetal.
 */

#ifndef __METAL_ATOMIC__H__
#define __METAL_ATOMIC__H__

#if defined(HAVE_STDATOMIC_H) && !defined(__STDC_NO_ATOMICS__) && \
	!defined(__cplusplus)
# include <stdint.h>
# include <stdatomic.h>
#elif defined(__cplusplus)
# include <atomic>
#elif defined(__GNUC__)
# include <compiler/gcc/atomic.h>
#else
# include <processor/nios2/atomic.h>
#endif

#endif /* __METAL_ATOMIC__H__ */
