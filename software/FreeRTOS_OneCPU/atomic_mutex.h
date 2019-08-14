#ifndef __ATOMIC_MUTEX__H__
#define __ATOMIC_MUTEX__H__

void atomic_mutex_init(void);
void atomic_mutex_deinit(void);
void atomic_mutex_acquire(void);
void atomic_mutex_release(void);

#endif /* __ATOMIC_MUTEX__H__ */
