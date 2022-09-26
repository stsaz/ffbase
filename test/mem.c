/** ffbase: ffmem tester
2020, Simon Zolin
*/

#include <ffbase/base.h>
#include <ffbase/mem-print.h>

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

void test_mem_print()
{
	char ss[] = "asdfghjkzxcv12345678";
	ffstr s;
	s = ffmem_alprint(ss, sizeof(ss), 0);
	printf("%.*s\n", (int)s.len, s.ptr);
	ffstr_free(&s);
	s = ffmem_alprint(ss, sizeof(ss), FFMEM_PRINT_NO_TEXT);
	printf("%.*s\n", (int)s.len, s.ptr);
	ffstr_free(&s);
	s = ffmem_alprint(ss, sizeof(ss), FFMEM_PRINT_ZEROSPACE);
	printf("%.*s\n", (int)s.len, s.ptr);
	ffstr_free(&s);
}
