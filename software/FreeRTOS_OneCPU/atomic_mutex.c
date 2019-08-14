#include "atomic_mutex.h"

#include <mutex.h>

metal_mutex_t *atomic_mutex;

void atomic_mutex_init(void)
{
	metal_mutex_init(atomic_mutex);
}

void atomic_mutex_deinit(void)
{
	metal_mutex_release(atomic_mutex);
}

void atomic_mutex_acquire(void)
{
	metal_mutex_acquire(atomic_mutex);
}

void atomic_mutex_release(void)
{
	metal_mutex_deinit(atomic_mutex);
}

