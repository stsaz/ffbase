/** ffbase/array.h tester
2020, Simon Zolin
*/

#include <ffbase/slice.h>
#include <test/test.h>


void test_ffslice_alloc_add()
{
	const char *data = "0123456789";
	ffslice a = {};

#ifdef FF_64
	x(NULL == ffslice_allocT(&a, 0xffffffffffffffff / 8 + 1, void*)); // multiply overflow
#endif

#ifdef FFBASE_DEBUG
	x(NULL != ffslice_alloc_stack(&a, 10, 1));
	ffslice_null(&a);
#endif

	x(NULL != ffslice_zallocT(&a, 10, char));
	static char empty[10];
	x(!memcmp(a.ptr, empty, 10));
	ffslice_free(&a);

	x(NULL != ffslice_allocT(&a, 10, char));
	*ffslice_pushT(&a, 10, char) = '0';
	*ffslice_pushT(&a, 10, char) = '1';
	x(3 == ffslice_addT(&a, 5, data+2, 10-2, char));
	x(NULL != ffslice_growT(&a, 5, char));
	x(5 == ffslice_addT(&a, 10, data+2+3, 10-2-3, char));
	x(ffslice_eqT(&a, data, 10, char));
	x(ffslice_eq2T(&a, &a, char));

	ffslice a2;
	ffslice_set(&a2, "6789", 4);
	a.len = 6;
	x(4 == ffslice_add2T(&a, 10, &a2, char));
	x(ffslice_eqT(&a, data, 10, char));

	x(NULL != ffslice_reallocT(&a, 9, char));
	x(a.len == 9);
	x(NULL != ffslice_reallocT(&a, 10, char));
	x(ffslice_eqT(&a, data, 9, char));

	ffslice_free(&a);
}

int sort_func(const void *a, const void *b, void *udata)
{
	(void) udata;
	char aa = *(char*)a, bb = *(char*)b;
	return ((aa == bb) ? 0 : (aa < bb) ? -1 : 1);
}

int cmp_func(const void *a, const void *b)
{
	char aa = *(char*)a, bb = *(char*)b;
	return ((aa == bb) ? 0 : (aa < bb) ? -1 : 1);
}

void freeptr(void **pp)
{
	ffmem_free(*pp);
}

void test_ffslice_move()
{
	ffslice a = {};
	ffslice_dupT(&a, "0123456789", 10, char);

	void *p = ffslice_moveT(&a, 3, 7, 2, char);
	x((char*)p == ((char*)a.ptr)+3);
	x(ffslice_eqT(&a, "0123456349", 10, char));

	ffslice_moveT(&a, 0, 8, 2, char);
	x(ffslice_eqT(&a, "0123456301", 10, char));

	// ffslice_moveT(&a, 3, 8, 3, char); // assert

	ffslice_free(&a);
}

void test_ffslice_misc()
{
	ffslice a = {};

	x(NULL != ffslice_dupT(&a, "0123456789", 10, char));
	ffslice_rmswapT(&a, 5, 2, char);
	x(ffslice_eqT(&a, "01234897", 8, char));
	ffslice_free(&a);

	x(NULL != ffslice_dupT(&a, "0123456789", 10, char));
	ffslice_rmswapT(&a, 7, 2, char);
	x(ffslice_eqT(&a, "01234569", 8, char));
	ffslice_free(&a);

	x(NULL != ffslice_dupT(&a, "0123456789", 10, char));
	ffslice_rmT(&a, 6, 2, char);
	x(ffslice_eqT(&a, "01234589", 8, char));
	ffslice_free(&a);

	x(NULL != ffslice_dupT(&a, "0987654321", 10, char));
	ffslice_sortT(&a, &sort_func, NULL, char);
	x(ffslice_eqT(&a, "0123456789", 10, char));

	char *it;
	int i = 0;
	FFSLICE_WALK_T(&a, it, char) {
		x(i == ffslice_findT(&a, it, &cmp_func, char));
		i++;
	}
	x(-1 == ffslice_findT(&a, "!", &cmp_func, char));

	i = 0;
	FFSLICE_WALK_T(&a, it, char) {
		xieq(i, ffslice_binfindT(&a, it, &cmp_func, char));
		i++;
	}
	x(-1 == ffslice_binfindT(&a, "!", &cmp_func, char));

	ffslice_free(&a);

	ffslice_allocT(&a, 2, void*);
	*ffslice_pushT(&a, 2, void*) = ffmem_alloc(1);
	*ffslice_pushT(&a, 2, void*) = ffmem_alloc(1);
	x(NULL == ffslice_pushT(&a, 2, void*));
	FFSLICE_FOREACH_PTR_T(&a, ffmem_free, void*);
	ffslice_free(&a);

	ffslice_allocT(&a, 2, void*);
	*ffslice_pushT(&a, 2, void*) = ffmem_alloc(1);
	*ffslice_pushT(&a, 2, void*) = ffmem_alloc(1);
	FFSLICE_FOREACH_T(&a, freeptr, void*);
	ffslice_free(&a);
}

void test_arr()
{
	ffuint a[] = {10, 5, 1, 3};
	ffuint asorted[] = {1, 3, 5, 10};
	ffarrint32_sort(a, FF_COUNT(a));
	x(!ffmem_cmp(a, asorted, sizeof(a)));

	x(-1 == ffarrint32_binfind(a, FF_COUNT(a), 2));
	x(1 == ffarrint32_binfind(a, FF_COUNT(a), 3));
	x(3 == ffarrint32_binfind(a, FF_COUNT(a), 10));

	x(2 == ffarrint32_find(a, FF_COUNT(a), 5));
}

void test_slice()
{
	const char *data = "0123456789";
	ffslice a = {};

	ffslice_null(&a);
	ffslice_set(&a, data, 10);
	ffslice_set2(&a, &a);

	x(*ffslice_itemT(&a, 8, char) == '8');
	x(*ffslice_lastT(&a, char) == '9');
	x(ffslice_endT(&a, char) == data + 10);

	char *it;
	int i = 0;
	FFSLICE_WALK_T(&a, it, char) {
		x(it == data + i++);
	}
	x(i == 10);

	i = 10;
	FFSLICE_RWALK_T(&a, it, char) {
		x(it == data + --i);
	}
	x(i == 0);

	ffslice_null(&a);

	test_ffslice_alloc_add();
	test_ffslice_move();
	test_ffslice_misc();
	test_arr();
}
