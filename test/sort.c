/** ffbase/sort.h tester
2020, Simon Zolin
*/

#include <ffbase/base.h>
#include <ffbase/sort.h>
#include <test/test.h>


#ifdef LIBC
static int intcmp_libc(const void *a, const void *b)
{
	ncmp++;
	ffint64 i1 = *(ffint64*)a;
	ffint64 i2 = *(ffint64*)b;
	return ffint_cmp(i1, i2);
}
qsort(a->ptr, a->len, sizeof(ffint64), &intcmp_libc);
#endif

int int64cmp(const void *a, const void *b, void *udata)
{
	(void) udata;
	ffint64 aa = *(ffint64*)a, bb = *(ffint64*)b;
	return ((aa == bb) ? 0 : (aa < bb) ? -1 : 1);
}

void test_sort_inc()
{
	ffsize cap = 1 << 16;
	void *buf = ffmem_alloc(cap * sizeof(ffuint64));
	ffuint64 *arr = buf;
	ffuint n = 0;
	for (ffuint i = 0;  i != cap;  i++) {
		arr[n++] = i;
	}

	ffsort(buf, cap, sizeof(ffuint64), &int64cmp, NULL);

	ffint64 prev = -1;
	for (ffuint i = 0;  i != cap;  i++) {
		x((ffint64)arr[i] >= prev);
		prev = arr[i];
	}

	ffmem_free(buf);
}

void test_sort_dec()
{
	ffsize cap = 1 << 16;
	void *buf = ffmem_alloc(cap * sizeof(ffuint64));
	ffuint64 *arr = buf;
	ffuint n = 0;
	for (int i = cap - 1;  i >= 0;  i--) {
		arr[n++] = i;
	}

	ffsort(buf, cap, sizeof(ffuint64), &int64cmp, NULL);

	ffint64 prev = -1;
	for (ffuint i = 0;  i != cap;  i++) {
		x((ffint64)arr[i] >= prev);
		prev = arr[i];
	}

	ffmem_free(buf);
}

#ifdef FF_UNIX
#include <stdlib.h>
#include <time.h>
#define ffrnd_seed  srandom
#define ffrnd_get  random

#else //FF_WIN:
#define ffrnd_seed  srand
#define ffrnd_get  rand
#endif

void test_sort_rnd()
{
	ffsize cap = 1 << 16;
	void *buf = ffmem_alloc(cap * sizeof(ffuint64));
	ffuint64 *arr = buf;
	ffuint n = 0;
	for (ffuint i = 0;  i != cap;  i++) {
		arr[n++] = ffrnd_get();
	}

	ffsort(buf, cap, sizeof(ffuint64), &int64cmp, NULL);

	ffint64 prev = -1;
	for (ffuint i = 0;  i != cap;  i++) {
		x((ffint64)arr[i] >= prev);
		prev = arr[i];
	}

	ffmem_free(buf);
}

void test_sort()
{
#ifdef FF_UNIX
	ffrnd_seed(time(NULL));
#else
	ffrnd_seed(GetTickCount());
#endif
	test_sort_inc();
	test_sort_dec();
	test_sort_rnd();
}
