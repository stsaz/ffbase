/** ffbase: ring buffer
2020, Simon Zolin
*/

#pragma once

#include <ffbase/string.h>
#include <ffbase/atomic.h>


/** Ring buffer for at most 1 reader & 1 writer in parallel */
typedef struct ffring {
	void *buf;
	ffsize r, w;
	ffatomic len;
	ffsize cap;
} ffring;

/** Allocate buffer
Return NULL on error */
static inline void* ffring_create(ffring *r, ffsize cap)
{
	r->buf = ffmem_alloc(cap);
	r->cap = cap;
	return r->buf;
}

/** Free buffer */
static inline void ffring_free(ffring *r)
{
	ffmem_free(r->buf);
	r->buf = NULL;
}

/** Clear buffer */
static inline void ffring_clear(ffring *r)
{
	r->r = r->w = 0;
	ffatomic_store(&r->len, 0);
}

/** Get the number of bytes available for reading */
static inline ffsize ffring_filled(ffring *r)
{
	return ffatomic_load(&r->len);
}

/** Write data to a free sequential data block
Return N of bytes written */
static inline ffsize ffring_write(ffring *r, const void *data, ffsize n)
{
	ffsize R = FFINT_READONCE(r->r);
	ffsize canwrite = (R > r->w) ? R - r->w // xx(w)..(r)xx
		: (R < r->w) ? r->cap - r->w // ..(r)xx(w)..
		: (ffatomic_load(&r->len) == 0) ? r->cap - r->w // ..(rw)....
		: 0; // xx(rw)xxxx
	n = ffmin(n, canwrite);
	ffmem_copy((char*)r->buf + r->w, data, n);
	r->w = (r->w + n) % r->cap;
	ffatomic_fetch_add(&r->len, n);
	return n;
}

static inline ffsize ffring_writestr(ffring *r, ffstr data)
{
	return ffring_write(r, data.ptr, data.len);
}

/** Get buffer available for sequential reading */
static inline ffstr ffring_acquire_read(ffring *r)
{
	ffsize w = FFINT_READONCE(r->w);
	ffsize canread = (r->r > w) ? r->cap - r->r // xx(w)..(r)xx
		: (r->r < w) ? w - r->r // ..(r)xx(w)..
		: (ffatomic_load(&r->len) == 0) ? 0 // ..(rw)....
		: r->cap - r->r; // xx(rw)xxxx
	ffstr s;
	ffstr_set(&s, (char*)r->buf + r->r, canread);
	return s;
}

/** Shift reading position */
static inline void ffring_release_read(ffring *r, ffsize n)
{
	r->r = (r->r + n) % r->cap;
	ffatomic_fetch_add(&r->len, -(ffssize)n);
}
