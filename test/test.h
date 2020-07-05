/** ffbase: testing helpers
2020, Simon Zolin
*/

#include <ffbase/string.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


static inline void test_check(int ok, const char *expr, const char *file, ffuint line, const char *func)
{
	if (ok) {
		return;
	}

	fprintf(stderr, "FAIL: %s:%u: %s: %s\n"
		, file, line, func, expr);
	abort();
}

static inline void test_check_int_int(int ok, ffint64 i1, ffint64 i2, const char *file, ffuint line, const char *func)
{
	if (ok) {
		return;
	}

#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
	fprintf(stderr, "FAIL: %s:%u: %s: %d != %d\n"
		, file, line, func
		, (int)i1, (int)i2);
#else
	fprintf(stderr, "FAIL: %s:%u: %s: %lld != %lld\n"
		, file, line, func
		, i1, i2);
#endif
	abort();
}

static inline void test_check_str_sz(int ok, ffsize slen, const char *s, const char *sz, const char *file, ffuint line, const char *func)
{
	if (ok) {
		return;
	}

	fprintf(stderr, "FAIL: %s:%u: %s: %.*s != %s\n"
		, file, line, func
		, (int)slen, s, sz);
	abort();
}

#define x(expr) \
	test_check(expr, #expr, __FILE__, __LINE__, __func__)

#define xieq(i1, i2) \
({ \
	ffint64 __i1 = (i1); \
	ffint64 __i2 = (i2); \
	test_check_int_int(__i1 == __i2, __i1, __i2, __FILE__, __LINE__, __func__); \
})

#define xseq(s, sz) \
({ \
	ffstr __s = *(s); \
	test_check_str_sz(ffstr_eqz(&__s, sz), __s.len, __s.ptr, sz, __FILE__, __LINE__, __func__); \
})

/** Read 4k file into a new buffer */
static inline int file_readall(ffstr *a, const char *fn)
{
	int f = open(fn, O_RDONLY);
	if (f < 0)
		return -1;
	ffstr_alloc(a, 4*1024);
	a->len = read(f, a->ptr, 4*1024);
	close(f);
	return 0;
}

#define FFARR_WALK(static_array, it) \
	for (it = static_array;  it != static_array + FF_COUNT(static_array);  it++)
