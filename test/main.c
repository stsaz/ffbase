/** ffbase tester
2020, Simon Zolin
*/

#include <ffbase/base.h>
#include <ffbase/stringz.h>
#include <test/test.h>
#include <stdio.h>


void test_base();
void test_mem();
void test_slice();
void test_str();
void test_strz();
void test_sort();
void test_assert();
void test_unicode();
void test_list();
void test_rbtree();
void test_vec();
void test_map();
void test_ring();
void test_json();
void test_lock();
void test_atomic();
void test_write_bitmap();
void test_write_charmap();


void test_base()
{
	x(1 == ffmin(1, 2));
	x(2 == ffmin(-1, 2));
	x(2 == ffmax(1, 2));
	x(2 == ffmax(-1, 2));
	x(1 == ffint_abs(-1));
	x(4+1 == FF_COUNT("asdf"));
	x(4 == FFS_LEN("asdf"));

	struct t {
		ffuint i;
		ffuint i2;
	};
	x(4 == FF_OFF(struct t, i2));
	struct t t;
	x(&t.i2 == FF_PTR(&t, 4));
	x(&t == FF_STRUCTPTR(struct t, i2, &t + 4));

	x(0 == ffbit_find64(0));
	x(63 == ffbit_find64(1) - 1);
	x(4 == ffbit_find64(0x0880000000000000ULL) - 1);
	x(0 == ffbit_find32(0));
	x(4 == ffbit_find32(0x08800000) - 1);
	x(31 == ffbit_find32(0x00000001) - 1);

#ifdef FF_LITTLE_ENDIAN
	x(0x1234 == ffint_be_cpu16(0x3412));
	x(0x1234 == ffint_le_cpu16(0x1234));
#else
	x(0x1234 == ffint_le_cpu16(0x3412));
	x(0x1234 == ffint_be_cpu16(0x1234));
#endif

	x(!ffint_ispower2(0));
	x(!ffint_ispower2(1));
	x(ffint_ispower2(2));
	x(!ffint_ispower2(63));
	x(ffint_ispower2(64));
	x(!ffint_ispower2(65));

	x(0x8000000000000000ULL == ffint_align_power2(0x8000000000000000ULL));
	x(32 == ffint_align_power2(17));
	x(16 == ffint_align_power2(16));
	x(16 == ffint_align_power2(15));
	x(2 == ffint_align_power2(2));
	x(2 == ffint_align_power2(1));
	x(2 == ffint_align_power2(0));
}

void test_assert()
{
	int i = 0;
	FF_ASSERT(i == 1);
}

struct test {
	const char *name;
	void (*func)();
};
#define T(nm) { #nm, &test_ ## nm }
static const struct test atests[] = {
	T(base), T(mem),
	T(slice), T(vec), T(sort),
	T(str), T(strz),
	T(unicode),
	T(list), T(rbtree), T(map), T(ring),
	T(json),
	T(atomic), T(lock),
};
// tests not included in 'all'
static const struct test natests[] = {
	T(assert),
};
#undef T

int main(int argc, const char **argv)
{
	const struct test *t;

	if (argc == 1) {
		printf("Supported tests: all ");
		FFARR_WALK(atests, t) {
			printf("%s ", t->name);
		}
		FFARR_WALK(natests, t) {
			printf("%s ", t->name);
		}
		printf("\n");
		return 0;
	}

	if (ffsz_eq(argv[1], "all")) {
		//run all tests
		FFARR_WALK(atests, t) {
			printf("%s\n", t->name);
			t->func();
			printf("  OK\n");
		}

	} else {
		//run the specified tests only

		for (ffuint n = 1;  n < (ffuint)argc;  n++) {
			const struct test *sel = NULL;

			FFARR_WALK(atests, t) {
				if (ffsz_eq(argv[n], t->name)) {
					sel = t;
					goto call;
				}
			}

			FFARR_WALK(natests, t) {
				if (ffsz_eq(argv[n], t->name)) {
					sel = t;
					goto call;
				}
			}

			if (sel == NULL) {
				printf("unknown test: %s\n", argv[n]);
				return 1;
			}

call:
			printf("%s\n", sel->name);
			sel->func();
			printf("  OK\n");
		}
	}
	return 0;
}
