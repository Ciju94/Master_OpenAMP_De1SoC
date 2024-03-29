/*
 * Copyright (c) 2016, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <errno.h>

#include "../metal-test.h"
#include <atomic.h>
#include <log.h>
#include <sys.h>

#include "../atomic_mutex.h"

static const int atomic_test_count = 1000;

static void *atomic_thread(void *arg)
{
	atomic_int *c = arg;
	int i;

	for (i = 0; i < atomic_test_count; i++) {
		atomic_mutex_acquire();
		atomic_fetch_add(c, 1);
		atomic_mutex_release();
	}

	return NULL;
}

static int atomic(void)
{
	const int threads = 10;
	atomic_int counter = ATOMIC_VAR_INIT(0);
	int value, error;

	error = metal_run(threads, atomic_thread, &counter);
	if (!error) {
		value = atomic_load(&counter);
		value -= atomic_test_count * threads;
		if (value) {
			metal_log(METAL_LOG_DEBUG, "counter mismatch, delta = %d\n",
				  value);
			error = -EINVAL;
		}
	}

	return error;
}
METAL_ADD_TEST(atomic);
