/** ffbase: fixed-size lockless ring buffer, single-producer/consumer
2022, Simon Zolin */

/*
ffring_alloc ffring_free
ffring_reset
ffring_write ffring_write_all
ffring_write_begin ffring_write_all_begin
ffring_write_finish
ffring_read_begin ffring_read_all_begin
ffring_read_finish
*/

#pragma once
#include <ffbase/lock.h>
#include <ffbase/string.h>

typedef struct ffring {
	union {
		struct {
			ffsize cap;
			ffsize mask;
			ffuint flags;
		};
		char align0[64];
	};
	union {
		struct {
			ffsize whead, wtail;
		};
		char align1[64]; // try to gain performance by placing read-write cursors on separate cache lines
	};
	union {
		struct {
			ffsize rhead, rtail;
		};
		char align2[64];
	};
	char data[0];
} ffring;

typedef struct ffring_head {
	ffsize old, nu;
} ffring_head;

enum FFRING_FLAGS {
	FFRING_1_READER = 1, // optimize for single reader
	FFRING_1_WRITER = 2, // optimize for single writer
};

/** Allocate buffer
cap: max size; automatically aligned to the power of 2
flags: enum FFRING_FLAGS
Return NULL on error */
static inline ffring* ffring_alloc(ffsize cap, ffuint flags)
{
	if (flags != (FFRING_1_READER | FFRING_1_WRITER))
		return NULL;

	cap = ffint_align_power2(cap);
	ffring *b = (ffring*)ffmem_align(sizeof(ffring) + cap, 64);
	if (b == NULL)
		return NULL;

	b->flags = flags;
	b->whead = b->wtail = 0;
	b->rhead = b->rtail = 0;
	b->cap = cap;
	b->mask = cap - 1;
	return b;
}

static inline void ffring_free(ffring *b)
{
	ffmem_alignfree(b);
}

static inline void ffring_reset(ffring *b)
{
	b->whead = b->wtail = 0;
	b->rhead = b->rtail = 0;
}

/** Reserve contiguous free space region with the maximum size of 'n' bytes.
free: (output) amount of free space after the operation
Return value for ffring_write_finish() */
static inline ffring_head ffring_write_begin(ffring *b, ffsize n, ffstr *dst, ffsize *free)
{
	ffring_head wh;
	wh.old = b->whead;
	ffsize _free = b->cap + FFINT_READONCE(b->rtail) - wh.old;
	ffcpu_fence_acquire();

	ffsize i = wh.old & b->mask;
	if (n > _free)
		n = _free;
	if (i + n > b->cap)
		n = b->cap - i;

	wh.nu = wh.old + n;
	b->whead = wh.nu;

	dst->ptr = b->data + i;
	dst->len = n;

	if (free != NULL)
		*free = _free - n;
	return wh;
}

/** Reserve free space for 'n' bytes.
free: (output) amount of free space after the operation
Return value for ffring_write_finish() */
static inline ffring_head ffring_write_all_begin(ffring *b, ffsize n, ffstr *d1, ffstr *d2, ffsize *free)
{
	ffsize i;
	ffring_head wh;
	wh.old = b->whead;
	ffsize _free = b->cap + FFINT_READONCE(b->rtail) - wh.old;
	ffcpu_fence_acquire();

	if (n > _free) {
		// Not enough space
		d1->len = d2->len = 0;
		n = 0;
		wh.nu = wh.old; // allow ffring_write_finish()
		goto end;
	}

	wh.nu = wh.old + n;
	b->whead = wh.nu;

	i = wh.old & b->mask;
	d1->ptr = b->data + i;
	d2->ptr = b->data;
	d1->len = n;
	d2->len = 0;
	if (i + n > b->cap) {
		d1->len = b->cap - i;
		d2->len = i + n - b->cap;
	}

end:
	if (free != NULL)
		*free = _free - n;
	return wh;
}

/** Commit reserved data.
wh: return value from ffring_write*_begin() */
static inline void ffring_write_finish(ffring *b, ffring_head wh)
{
	ffcpu_fence_release();
	FFINT_WRITEONCE(b->wtail, wh.nu);
}

/** Write some data
Return N of bytes written */
static inline ffsize ffring_write(ffring *b, const void *src, ffsize n)
{
	ffstr d;
	ffring_head wh = ffring_write_begin(b, n, &d, NULL);
	if (d.len == 0)
		return 0;

	ffmem_copy(d.ptr, src, d.len);
	ffring_write_finish(b, wh);
	return d.len;
}

/** Write whole data
Return N of bytes written */
static inline ffsize ffring_write_all(ffring *b, const void *src, ffsize n)
{
	ffstr d1, d2;
	ffring_head wh = ffring_write_all_begin(b, n, &d1, &d2, NULL);
	if (d1.len + d2.len == 0)
		return 0;

	ffmem_copy(d1.ptr, src, d1.len);
	if (d2.len != 0)
		ffmem_copy(d2.ptr, (char*)src + d1.len, d2.len);
	ffring_write_finish(b, wh);
	return n;
}

/** Lock contiguous data region with the maximum size of 'n' bytes.
used: (output) amount of used space after the operation
Return value for ffring_read_finish() */
static inline ffring_head ffring_read_begin(ffring *b, ffsize n, ffstr *dst, ffsize *used)
{
	ffring_head rh;
	rh.old = b->rhead;
	ffsize _used = FFINT_READONCE(b->wtail) - rh.old;
	ffcpu_fence_acquire();

	ffsize i = rh.old & b->mask;
	if (n > _used)
		n = _used;
	if (i + n > b->cap)
		n = b->cap - i;

	rh.nu = rh.old + n;
	b->rhead = rh.nu;

	dst->ptr = b->data + i;
	dst->len = n;

	if (used != NULL)
		*used = _used - n;
	return rh;
}

/** Lock data region of exactly 'n' bytes.
used: (output) amount of used space after the operation
Return value for ffring_read_finish() */
static inline ffring_head ffring_read_all_begin(ffring *b, ffsize n, ffstr *d1, ffstr *d2, ffsize *used)
{
	ffsize i;
	ffring_head rh;
	rh.old = b->rhead;
	ffsize _used = FFINT_READONCE(b->wtail) - rh.old;
	ffcpu_fence_acquire();

	if (n > _used) {
		// Not enough data
		d1->len = d2->len = 0;
		n = 0;
		rh.nu = rh.old; // allow ffring_read_finish()
		goto end;
	}

	rh.nu = rh.old + n;
	b->rhead = rh.nu;

	i = rh.old & b->mask;
	d1->ptr = b->data + i;
	d2->ptr = b->data;
	d1->len = n;
	d2->len = 0;
	if (i + n > b->cap) {
		d1->len = b->cap - i;
		d2->len = i + n - b->cap;
	}

end:
	if (used != NULL)
		*used = _used - n;
	return rh;
}

/** Discard the locked data region.
rh: return value from ffring_read*_begin() */
static inline void ffring_read_finish(ffring *b, ffring_head rh)
{
	ffcpu_fence_release();
	FFINT_WRITEONCE(b->rtail, rh.nu);
}
