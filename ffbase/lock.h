/** ffbase: spinlock
2020, Simon Zolin
*/

#pragma once

#include <ffbase/atomic.h>

/*
fflock_init
fflock_trylock fflock_lock
fflock_unlock
*/

#ifdef FF_WIN
#define ffthread_yield  SwitchToThread

#else
#include <sched.h>
#define ffthread_yield  sched_yield
#endif

#define ffcpu_pause()  __asm volatile("pause")


#define FFLOCK_SPIN  2048

typedef struct fflock {
	ffuint lock;
} fflock;

static inline void fflock_init(fflock *lk)
{
	lk->lock = 0;
}

/** Try to acquire lock
Return FALSE if lock couldn't be acquired */
static inline int fflock_trylock(fflock *lk)
{
	return 0 == ffint_cmpxchg(&lk->lock, 0, 1);
}

/** Acquire lock */
static inline void fflock_lock(fflock *lk)
{
	for (;;) {

		if (fflock_trylock(lk))
			return;

		for (ffuint n = 0;  n != FFLOCK_SPIN;  n++) {
			ffcpu_pause();
			if (fflock_trylock(lk))
				return;
		}

		ffthread_yield();
	}
}

/** Release lock */
static inline void fflock_unlock(fflock *lk)
{
	ffcpu_fence_release();
	FFINT_WRITEONCE(lk->lock, 0);
}
