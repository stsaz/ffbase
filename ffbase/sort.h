/** ffbase: sort arrays
2020, Simon Zolin
*/

#pragma once
#define _FFBASE_SORT_H

#ifndef _FFBASE_BASE_H
#include <ffbase/base.h>
#endif


/** Return 0: (a == b);
 -1: (a < b);
  1: (a > b) */
typedef int (*ffsortcmp)(const void *a, const void *b, void *udata);

#include <ffbase/sort-merge.h>

/** Sort array elements.
Uses merge-sort with insertion-sort.
n: number of elements
elsize: size of 1 element
*/
static inline int ffsort(void *data, ffsize n, ffsize elsize, ffsortcmp cmp, void *udata)
{
	void *tmp;
	if (NULL == (tmp = _ffmem_alloc_stackorheap(n * elsize)))
		return -1;

	_ffsort_merge(tmp, data, n, elsize, cmp, udata);

	_ffmem_free_stackorheap(tmp, n * elsize);
	return 0;
}
