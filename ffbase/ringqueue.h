/** ffbase: fixed-size lockless ring queue, multi-producer, multi-consumer
2022, Simon Zolin */

/*
ffrq_alloc
ffrq_free
ffrq_add
ffrq_fetch ffrq_fetch_sr
*/

#pragma once
#include <ffbase/lock.h>

typedef struct ffringqueue {
	union {
		struct {
			ffuint cap;
			ffuint mask;
		};
		char align0[64];
	};
	union {
		struct {
			ffuint whead, wtail;
		};
		char align1[64]; // try to gain performance by placing read-write cursors on separate cache lines
	};
	union {
		struct {
			ffuint rhead, rtail;
		};
		char align2[64];
	};
	void *data[0];
} ffringqueue;

/** Allocate buffer for a queue
cap: max N of elements; automatically aligned to the power of 2
Return NULL on error */
static inline ffringqueue* ffrq_alloc(ffsize cap)
{
	cap = ffint_align_power2(cap);
	ffringqueue *q = (ffringqueue*)ffmem_align(sizeof(ffringqueue) + cap * sizeof(void*), 64);
	if (q == NULL)
		return NULL;
	q->cap = cap;
	q->mask = cap - 1;
	return q;
}

static inline void ffrq_free(ffringqueue *q)
{
	ffmem_alignfree(q);
}

/** Add an element
unused: (output) N of unused (free) elements before this operation
Return 0 on success */
static inline int ffrq_add(ffringqueue *q, void *it, ffuint *unused)
{
	ffuint wh, nwh;

	// reserve space for new data
	for (;;) {
		wh = FFINT_READONCE(q->whead);
		ffcpu_fence_acquire(); // read 'whead' before 'rtail'
		*unused = q->cap + FFINT_READONCE(q->rtail) - wh;
		if (ff_unlikely(1 > *unused))
			return -1;

		nwh = wh + 1;
		if (ff_likely(wh == ffint_cmpxchg(&q->whead, wh, nwh)))
			break;
		// another writer has just added an element
	}

	ffuint i = wh & q->mask;
	q->data[i] = it;

	// wait until previous writers finish their work
	ffint_wait_until_equal(&q->wtail, wh);

	ffcpu_fence_release();
	FFINT_WRITEONCE(q->wtail, nwh);
	return 0;
}

static inline int _ffrq_fetch(ffringqueue *q, void **item, ffuint sr, ffuint *used)
{
	ffuint rh, nrh;

	// reserve items
	for (;;) {
		rh = FFINT_READONCE(q->rhead);
		ffcpu_fence_acquire(); // read 'rhead' before 'wtail'
		*used = FFINT_READONCE(q->wtail) - rh;
		if (1 > *used)
			return -1;

		nrh = rh + 1;
		if (sr) {
			q->rhead = nrh;
			ffcpu_fence_acquire();
			break;
		}
		if (ff_likely(rh == ffint_cmpxchg(&q->rhead, rh, nrh)))
			break;
		// another reader has just read this element
	}

	ffuint i = rh & q->mask;
	*item = q->data[i];

	// wait until previous readers finish their work
	ffint_wait_until_equal(&q->rtail, rh);

	ffcpu_fence_release();
	FFINT_WRITEONCE(q->rtail, nrh);
	return 0;
}

/** Fetch and remove element
Return 0 on success */
static inline int ffrq_fetch(ffringqueue *q, void **item)
{
	ffuint used;
	return _ffrq_fetch(q, item, 0, &used);
}

/** Fetch and remove element (single reader) */
static inline int ffrq_fetch_sr(ffringqueue *q, void **item)
{
	ffuint used;
	return _ffrq_fetch(q, item, 1, &used);
}
