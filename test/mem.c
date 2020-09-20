/** ffbase: ffmem tester
2020, Simon Zolin
*/

#include <ffbase/base.h>

#define x  assert


void test_mem()
{
	char ss10[10] = { 0,1,2,3,4,5,6,7,8,9 };
	char ss0[10] = {};
	char *d;

	x(NULL != (d = ffmem_alloc(10)));
	ffmem_copy(d, ss10, 10);
	x(NULL != (d = ffmem_realloc(d, 2000)));
	x(!ffmem_cmp(d, ss10, 10));
	x(NULL != (d = ffmem_realloc(d, 10)));
	x(!ffmem_cmp(d, ss10, 10));
	ffmem_zero(d, 10);
	x(!ffmem_cmp(d, ss0, 10));
	ffmem_free(d);

	x(NULL != (d = ffmem_zalloc(10)));
	x(!ffmem_cmp(d, ss0, 10));
	ffmem_free(d);

	x(NULL != (d = ffmem_realloc(NULL, 10)));
	ffmem_free(d);

	x(NULL != (d = ffmem_new(char)));
	ffmem_free(d);

	x(NULL != (d = ffmem_align(4, 16)));
	x(((ffsize)d % 16) == 0);
	ffmem_alignfree(d);

	x(NULL != (d = ffmem_align(999, 512)));
	x(((ffsize)d % 512) == 0);
	ffmem_alignfree(d);
}
