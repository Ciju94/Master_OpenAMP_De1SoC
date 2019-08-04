/*
 * Copyright (c) 2016, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include <string.h>
#include "../../Source/include/FreeRTOS.h"
#include "task.h"

#include "../metal-test.h"
#include <time.h>
#include <sleep.h>
#include <log.h>
#include <sys.h>

static int sleep(void)
{
	int rc;
	unsigned int usec = 3;
	unsigned long long tstart, tend, tdelayed;

	tstart = metal_get_timestamp();
	metal_sleep_usec((usec/portTICK_PERIOD_MS));
	tend = metal_get_timestamp();
	tdelayed = tend - tstart;
	if (tdelayed > (usec/portTICK_PERIOD_MS))
		rc = -1;
	else
		rc = 0;

	return rc;
}
METAL_ADD_TEST(sleep);
