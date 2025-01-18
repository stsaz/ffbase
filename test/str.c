/** ffbase/string.h tester
2020, Simon Zolin
*/

#include <ffbase/string.h>
#include <ffbase/slice.h>
#include <test/test.h>

#define STR(s)  (char*)(s), FFS_LEN(s)

void test_str_conv();
void test_ffstr_fmt();


void test_ffstr_rm()
{
	ffstr s;
	const char *data = "01234";
	ffstr_setz(&s, data);
	x(ffstr_popfront(&s) == '0');
	xseq(&s, data+1);

	ffstr_setz(&s, "  0  ");
	xieq(2, ffstr_skipchar(&s, ' '));
	xseq(&s, "0  ");

	ffstr_setz(&s, " ,. 0 ,. ");
	ffstr skip = FFSTR_INIT(" .,");
	ffstr_skipany(&s, &skip);
	xseq(&s, "0 ,. ");

	ffstr_setz(&s, "  0  ");
	ffstr_rskipchar(&s, ' ');
	xseq(&s, "  0");

	ffstr_setz(&s, "  0  ");
	ffstr_rskipchar1(&s, ' ');
	xseq(&s, "  0 ");

	ffstr_setz(&s, "  0 ,. ");
	ffstr_setz(&skip, " .,");
	ffstr_rskipany(&s, &skip);
	xseq(&s, "  0");

	ffstr_setz(&s, " \n 0  \t ");
	ffstr_trimwhite(&s);
	xseq(&s, "0");

	char d[8];
	ffmem_copy(d, "1234567", 7);
	ffstr_set(&s, d, 7);
	ffstr_erase_left(&s, 3);
	xseq(&s, "4567");
}

void test_ffstr_alloc()
{
	const char *data = "0123456789";
	ffstr s = {};

#ifdef FFBASE_DEBUG
	x(NULL != ffstr_alloc_stack(&s, 10));
	ffstr_null(&s);
#endif

	x(NULL != ffstr_alloc(&s, 5));
	*ffstr_push(&s) = '0';
	*ffstr_push(&s) = '1';
	x(3 == ffstr_add(&s, 5, data+2, 10-2));
	ffsize cap = 5;
	x(NULL != ffstr_grow(&s, &cap, 5));
	x(5 == ffstr_addz(&s, 10, data+2+3));
	xseq(&s, "0123456789");

	s.len = 6;
	ffstr s2 = FFSTR_INIT("6789");
	x(4 == ffstr_add2(&s, 10, &s2));
	xseq(&s, "0123456789");

	x(NULL != ffstr_realloc(&s, 9));
	x(s.len == 9);
	x(NULL != ffstr_realloc(&s, 10));
	x(ffstr_eq(&s, data, 9));

	ffstr_free(&s);

	x(NULL != ffstr_dup(&s, data, 10));
	xseq(&s, data);
	s.len = 2;
	x(3 == ffstr_addfill(&s, 10, '!', 3));
	xseq(&s, "01!!!");
	ffstr_free(&s);

	x(NULL != ffstr_dupz(&s, data));
	xseq(&s, data);
	ffstr_free(&s);

	cap = 0;
	xieq(4, ffstr_growadd(&s, &cap, "0123", 4));
	xseq(&s, "0123");
	xieq(4, cap);

	xieq(2, ffstr_growaddz(&s, &cap, "45"));
	xseq(&s, "012345");
	xieq(6, cap);

	ffstr_setz(&s2, "abcdef");
	xieq(6, ffstr_growadd2(&s, &cap, &s2));
	xseq(&s, "012345abcdef");
	xieq(12, cap);
	ffstr_free(&s);

	cap = 0;
	xieq(1, ffstr_growaddchar(&s, &cap, 'A'));
	xseq(&s, "A");
	xieq(1, cap);
	xieq(0, ffstr_addchar(&s, cap, 'A'));
	x(NULL != ffstr_grow(&s, &cap, 1));
	cap = 2;
	xieq(1, ffstr_addchar(&s, cap, 'B'));
	xseq(&s, "AB");
	ffstr_free(&s);

	cap = 0;
	xieq(5, ffstr_growaddfill(&s, &cap, 'A', 5));
	xseq(&s, "AAAAA");
	xieq(5, cap);
	ffstr_free(&s);

	cap = 0;
	xieq(12, ffstr_growfmt(&s, &cap, ".%s.", "0123456789"));
	xseq(&s, ".0123456789.");
	xieq(12+1, cap);
	ffstr_free(&s);

	cap = 0;
	x(NULL != ffstr_growtwice(&s, &cap, 4));
	xieq(4, cap);
	s.len = 4;
	x(NULL != ffstr_growtwice(&s, &cap, 2));
	xieq(8, cap);
	s.len = 8;
	x(NULL != ffstr_growtwice(&s, &cap, 10));
	xieq(18, cap);
	ffstr_free(&s);
}

void test_ffstr_gather()
{
	ffsize cap = 0;
	ffstr s = {}, out;

	x(4 == ffstr_gather(&s, &cap, "abcd", 4, 7, &out));
	xseq(&s, "abcd");
	xseq(&out, "");

	x(3 == ffstr_gather(&s, &cap, "1234", 4, 7, &out));
	xseq(&s, "abcd123");
	xseq(&out, "abcd123");

	x(0 == ffstr_gather(&s, &cap, "1234", 4, 6, &out));
	xseq(&out, "abcd12");

	ffstr_free(&s);

	cap = 0;
	x(3 == ffstr_gather(&s, &cap, "1234", 4, 3, &out));
	x(cap == 0);
	x(s.len == 0);
	xseq(&out, "123");
}

void test_ffstr_cmp()
{
	const char *data = "abcdeFGHIJ";
	ffstr s = FFSTR_INIT(data);

	x(!ffstr_eq(&s, data, 9));
	x(!ffstr_eq(&s, "abcdeFGHI", 10));
	x(ffstr_eq(&s, data, 10));
	x(ffstr_eq2(&s, &s));
	x(!ffstr_eqz(&s, "abcdeFGHI"));
	x(ffstr_eqz(&s, data));
	x(ffstr_eqcz(&s, "abcdeFGHIJ"));

	x(!ffstr_ieq(&s, data, 9));
	x(!ffstr_ieq(&s, "abcdeFGHI", 10));
	x(ffstr_ieq(&s, data, 10));
	x(ffstr_ieq2(&s, &s));
	x(!ffstr_ieqz(&s, "abcdeFGHI"));
	x(ffstr_ieqz(&s, data));
	x(ffstr_ieqz(&s, "abcdefghij"));
	x(ffstr_ieqz(&s, "ABCDEFGHIJ"));
	x(ffstr_ieqcz(&s, "abcdefghij"));
	x(ffstr_ieqcz(&s, "ABCDEFGHIJ"));

	x(0 == ffstr_cmp(&s, data, 10));
	x(0 == ffstr_cmp2(&s, &s));
	x(0 == ffstr_icmp(&s, "abcdefghij", 10));
	x(0 == ffstr_icmp(&s, "ABCDEFGHIJ", 10));
	x(0 == ffstr_icmp2(&s, &s));

	x(ffstr_cmp(&s, "ac", 2) < 0);
	x(ffstr_cmp(&s, "aa", 2) > 0);
	x(ffstr_cmp(&s, "Ac", 2) > 0);
	x(ffstr_icmp(&s, "Ac", 2) < 0);
	x(ffstr_icmp(&s, "AA", 2) > 0);
}

#if 0
void bench()
{
	static ffstr longurl = FFSTR_INITZ("http://hosthosthosthosthosthosthosthosthosthost:8080/path/my%20file/path/my%20file/path/my%20file/path/my%20file/path/my%20file/path/my%20file/path/my%20file/path/my%20file?query%20stringquery%20stringquery%20stringquery%20stringquery%20stringquery%20stringquery%20stringquery%20stringquery%20stringquery%20stringquery%20stringquery%20stringquery%20stringquery%20stringquery%20string#sharpsharpsharpsharpsharp");
	for (int i = 0;  i != 5*1000*1000;  i++) {
		static int r;
		r = ffs_ifindstr(longurl.ptr, longurl.len, "SHarp", 5);
		if (r < 0)
			_exit(0);
	}
}
#endif

void test_ffs_findany()
{
	static char d[] = "012345678905cdef""yz";
	ffuint dn = sizeof(d)-1;
	static char dz[] = "012345678905\x00""cde""yz";
	ffuint dzn = sizeof(dz)-1;

	x(5 == ffs_findany(d, dn, "75", 2));
	x(16-3 == ffs_findany(d+3, dn-3, "yz", 2));
	x(-1 == ffs_findany(d, dn, "gh", 2));
	x(0 == ffs_findany(dz, dzn, "70", 2));
	x(12 == ffs_findany(dz, dzn, "c\x00", 2));

#ifdef __SSE4_2__
	x(_ffcpu_features & (1<<20));
	if (_ffcpu_features & (1<<20)) {
		// aligned
		x(d == _ffmem_findany_sse42(d, dn, "70", 2));
		x(d+5 == _ffmem_findany_sse42(d, dn, "75", 2));
		x(d+16 == _ffmem_findany_sse42(d, dn, "yz", 2));
		x(NULL == _ffmem_findany_sse42(d, dn, "gh", 2));
		x(NULL == _ffmem_findany_sse42(d, dn-1, "zz", 2));

		// NUL byte
		x(dz == _ffmem_findany_sse42(dz, dzn, "70", 2));
		x(dz+12 == _ffmem_findany_sse42(dz, dzn, "c\0", 2));
		x(NULL == _ffmem_findany_sse42(dz, dzn, "gh", 2));
		x(NULL == _ffmem_findany_sse42(dz, dzn-1, "zz", 2));

		// unaligned
		x(d+7 == _ffmem_findany_sse42(d+1, dn-1, "70", 2));
		x(d+5 == _ffmem_findany_sse42(d+1, dn-1, "75", 2));
		x(d+16 == _ffmem_findany_sse42(d+1, dn-1, "yz", 2));
		x(NULL == _ffmem_findany_sse42(d+1, dn-1, "gh", 2));

		// NUL byte
		x(dz+7 == _ffmem_findany_sse42(dz+1, dzn-1, "70", 2));
		x(dz+12 == _ffmem_findany_sse42(dz+1, dzn-1, "c\0", 2));
		x(NULL == _ffmem_findany_sse42(dz+1, dzn-1, "gh", 2));
	}
#endif
}

void test_ffstr_find()
{
	ffstr s = FFSTR_INITZ("abcdeFGHIJ");
	x(4 == ffstr_findchar(&s, 'e'));
	x(4 == ffstr_rfindchar(&s, 'e'));

	ffstr sempty = {};
	x(-1 == ffstr_find(&sempty, "", 0));
	x(0 == ffstr_find(&s, "", 0));
	x(4 == ffstr_find(&s, "eFG", 3));
	x(4 == ffstr_findz(&s, "eFG"));
	x(0 == ffstr_find2(&s, &s));
	x(-1 == ffstr_ifindz(&s, "GHI#"));

	x(-1 == ffstr_ifind(&sempty, "", 0));
	x(0 == ffstr_ifind(&s, "", 0));
	x(4 == ffstr_ifind(&s, "eFG", 3));
	x(4 == ffstr_ifindz(&s, "eFG"));
	x(0 == ffstr_ifind2(&s, &s));
	x(-1 == ffstr_ifindz(&s, "GHI#"));
	xieq(4, ffs_ifindstr("asdf_qwer", 9, "_", 1));

	xieq(7, ffstr_findany(&s, "JH", 2));
	xieq(7, ffstr_findanyz(&s, "JHx"));
	xieq(7, ffstr_findanyz(&s, "xJH"));
	xieq(8, ffstr_rfindany(&s, "FI", 2));

	xieq(6, ffs_rfindstr("abcdefab", 8, "ab", 2));
	xieq(0, ffs_rfindstr("abcdefbc", 8, "abc", 3));
	xieq(-1, ffs_rfindstr("abcdefab", 8, "ac", 2));

	xieq(-1, ffs_findchar("abcdefab", 8, 'z'));
	xieq(0, ffs_findchar("abcdefab", 8, 'a'));

	xieq(-1, ffs_rfindchar("abcdefab", 8, 'z'));
	xieq(6, ffs_rfindchar("abcdefab", 8, 'a'));

	xieq(-1, ffs_findany("abcdefab", 8, "zxv", 3));
	xieq(0, ffs_findany("abcdefab", 8, "zba", 3));
	xieq(5, ffs_rfindany("abcdefab", 8, "df", 2));
}

void test_ffstr_match()
{
	const char *data = "key=val";
	ffstr s = FFSTR_INIT(data);

	x(ffstr_match(&s, "key=", 4));
	x(ffstr_match2(&s, &s));
	x(ffstr_matchz(&s, data));
	x(ffstr_matchz(&s, "key=val"));
	x(ffstr_matchcz(&s, "key="));
	x(!ffstr_matchcz(&s, "key1"));

	x(ffstr_imatch(&s, "KEY=", 4));
	x(ffstr_imatch2(&s, &s));
	x(ffstr_imatchcz(&s, data));
	x(ffstr_imatchcz(&s, "KEY=val"));
	x(!ffstr_imatchcz(&s, "KEY1"));

	x(ffstr_irmatch(&s, "=val", 4));
	x(ffstr_irmatch2(&s, &s));
	x(ffstr_irmatchcz(&s, "=val"));
	x(!ffstr_irmatchcz(&s, "=val2"));
	x(!ffstr_irmatchcz(&s, ".=val"));
}

void test_ffstr_split()
{
	ffstr s = FFSTR_INIT("abcdeFGHIJ");
	ffstr sl, sr;
	x(4 == ffstr_splitby(&s, 'e', &sl, &sr));
	xseq(&sl, "abcd");
	xseq(&sr, "FGHIJ");
	x(4 == ffstr_rsplitby(&s, 'e', &sl, &sr));
	xseq(&sl, "abcd");
	xseq(&sr, "FGHIJ");

	ffstr_setz(&s, "a \tb");
	xieq(1, ffstr_splitbyany(&s, "\n\t ", &sl, &sr));
	xseq(&sl, "a");
	xseq(&sr, "\tb");
}

void test_str_case()
{
	char buf[32];
	ffstr s;
	s.ptr = buf;

	{
	ffmem_copy(buf, "ASDFqwer", 8);
	ffstr s1 = FFSTR_INITN(buf, 8);
	ffstr_lower(&s1);
	xseq(&s1, "asdfqwer");
	}

	{
	ffstr s1 = FFSTR_INITZ("ASDFqwer");
	ffstr s2 = {};
	ffstr_dupstr_lower(&s2, &s1);
	xseq(&s2, "asdfqwer");
	ffstr_free(&s2);
	}

	{
	ffstr s2 = {};
	ffstr_dup_lower(&s2, "ASDFqwer", 5);
	xseq(&s2, "asdfq");
	ffstr_free(&s2);
	}

	s.len = ffs_lower(buf, sizeof(buf), STR("ASDFqwer"));
	xseq(&s, "asdfqwer");

	s.len = ffs_upper(buf, sizeof(buf), STR("ASDFqwer"));
	xseq(&s, "ASDFQWER");

	s.len = ffs_titlecase(buf, sizeof(buf), STR("#it's ASDF qwer-ty"));
	xseq(&s, "#it's Asdf Qwer-ty");
}

void test_ffstr_array()
{
	static const ffstr ss[] = {
		FFSTR_INIT("abc"),
		FFSTR_INIT("1234567890"),
		FFSTR_INIT("zxcv"),
	};
	ffslice a = {};
	ffslice_set(&a, ss, FF_COUNT(ss));
	ffstr search = FFSTR_INIT("1234567890");
	xieq(1, ffslice_findT(&a, &search, (ffslice_cmp_func)ffstr_cmp2, ffstr));
	xieq(2, ffslice_findT(&a, "ZXCv", (ffslice_cmp_func)ffstr_icmpz, ffstr));

	ffstr_setz(&search, "abc");
	xieq(0, ffslicestr_find(&a, &search));
	xieq(2, ffslicestr_findz(&a, "zxcv"));

	ffstr_setz(&search, "ABC");
	xieq(0, ffslicestr_ifind(&a, &search));
	xieq(2, ffslicestr_ifindz(&a, "ZXCv"));
}

static void test_str_wildcard()
{
	x(0 == ffs_wildcard(NULL, 0, NULL, 0, 0));
	x(0 == ffs_wildcard(STR("*"), NULL, 0, 0));
	x(0 != ffs_wildcard(STR("?"), NULL, 0, 0));
	x(0 != ffs_wildcard(NULL, 0, STR("a"), 0));
	x(0 == ffs_wildcard(STR("aa"), STR("aa"), 0));
	x(0 != ffs_wildcard(STR("aa"), STR("ba"), 0));
	x(0 == ffs_wildcard(STR("*"), STR("aa"), 0));
	x(0 == ffs_wildcard(STR("?b?"), STR("abc"), 0));

	x(0 == ffs_wildcard(STR("*c"), STR("abc"), 0));
	x(0 != ffs_wildcard(STR("*c"), STR("ab!"), 0));
	x(0 == ffs_wildcard(STR("a*"), STR("abc"), 0));
	x(0 == ffs_wildcard(STR("a*c"), STR("abbc"), 0));

	x(0 == ffs_wildcard(STR("*aB*"), STR("ac.Abc"), FFS_WC_ICASE));
	x(0 != ffs_wildcard(STR("*aB*"), STR("ac.Abc"), 0));
	x(0 == ffs_wildcard(STR("*ab*"), STR("ac.abc"), 0));
	x(0 == ffs_wildcard(STR("a*a*bb*c"), STR("aabcabbc"), 0));
	x(0 != ffs_wildcard(STR("a*a*bbc*c"), STR("aabcabbc"), 0));
	x(0 != ffs_wildcard(STR("*ab*"), STR("ac.ac"), 0));
	x(0 != ffs_wildcard(STR("b*"), STR("abc"), 0));
}

void test_ffstr_replace()
{
	ffstr in, out = {}, sch, repl;
	ffsize cap = 0;

	ffstr_setz(&in, "abcabc");
	ffstr_setz(&sch, "bcb");
	ffstr_setz(&repl, "123");
	xieq(0, ffstr_growadd_replace(&out, &cap, &in, &sch, &repl, 0));
	xseq(&out, "abcabc");

	ffstr_setz(&in, "abcabc");
	ffstr_setz(&sch, "bc");
	ffstr_setz(&repl, "123");
	out.len = 0;
	xieq(1, ffstr_growadd_replace(&out, &cap, &in, &sch, &repl, 0));
	xseq(&out, "a123abc");

	ffstr_setz(&in, "abcabc");
	ffstr_setz(&sch, "bc");
	ffstr_setz(&repl, "123");
	out.len = 0;
	xieq(2, ffstr_growadd_replace(&out, &cap, &in, &sch, &repl, FFSTR_REPLACE_ALL));
	xseq(&out, "a123a123");

	ffstr_setz(&in, "aBCabc");
	ffstr_setz(&sch, "bC");
	ffstr_setz(&repl, "123");
	out.len = 0;
	xieq(2, ffstr_growadd_replace(&out, &cap, &in, &sch, &repl, FFSTR_REPLACE_ICASE | FFSTR_REPLACE_ALL));
	xseq(&out, "a123a123");

	ffstr_free(&out);
}

void test_ffs_skipany()
{
	xieq(12, ffs_skipany(" \t\r\n \t\r\n \t\r\n", 12, "\t\n\r ", 4));
	xieq(12, ffs_skipany(" \t\r\n \t\r\n \t\r\nkey", 12+3, "\t\n\r ", 4));
	xieq(12, ffs_rskipany(" \t\r\n \t\r\n \t\r\n", 12, "\t\n\r ", 4));
	xieq(12, ffs_rskipany("key \t\r\n \t\r\n \t\r\n", 12+3, "\t\n\r ", 4));
}

void test_ffs_skip_ranges()
{
	xieq(3, ffs_skip_ranges("key val", 7, "01\x21\x7f", 4));
	xieq(0, ffs_skip_ranges(" key val", 8, "01\x21\x7f", 4));
	xieq(-1, ffs_skip_ranges("keyval", 6, "01\x21\x7f", 4));

	xieq(3, ffs_skip_ranges("key val", 7, "0123456789\x21\x7f" "012345", 18));
	xieq(0, ffs_skip_ranges(" key val", 8, "0123456789\x21\x7f" "012345", 18));
	xieq(-1, ffs_skip_ranges("keyval", 6, "0123456789\x21\x7f" "012345", 18));
}

void test_str()
{
	const char *data = "0123456789";
	ffstr s = FFSTR_INIT(data);

	{
		ffstr s1 = FFSTR_INITZ("1234");
		xseq(&s1, "1234");
	}
	{
		ffstr s1 = FFSTR_INITN("1234", 3);
		xseq(&s1, "123");
	}
	{
		ffstr s1 = FFSTR_INITSTR(&s);
		xseq(&s1, data);
	}

	ffstr_null(&s);
	ffstr_set(&s, data, 10);
	ffstr_set2(&s, &s);
	ffstr_setz(&s, data);
	ffstr_setz(&s, "0123456789");
	ffstr_setnz(&s, data, 5);
	x(s.len == 5);
	ffstr_setnz(&s, data, 15);
	x(s.len == 10);

	ffstr_shift(&s, 2);
	x(s.ptr == data + 2);
	s.ptr -= 2;
	s.len += 2;

	x(*ffstr_last(&s) == '9');
	x(ffstr_end(&s) == data + 10);

	test_ffstr_rm();
	test_ffstr_alloc();
	test_ffstr_cmp();
	test_ffs_findany();
	test_ffstr_find();
	test_ffstr_match();
	test_ffstr_split();
	test_str_conv();
	test_ffstr_fmt();
	test_str_case();
	test_ffstr_array();
	test_ffstr_gather();
	test_str_wildcard();
	test_ffstr_replace();
	test_ffs_skipany();
	test_ffs_skip_ranges();
}

void test_write_bitmap()
{
	ffstr s = {};
	ffsize cap = 0;
	ffuint k = 0;
	ffstr_growaddz(&s, &cap, "static const ffuint array[] = {\n");
	for (int i = 0x00;  i <= 0xff;  i += 32) {

		ffuint b = 0;
		for (int j = i + 32-1;  j >= i;  j--) {
			b <<= 1;
			// if (j >= 0x20 && j != 0x7f)
			if (NULL != ffmem_findbyte("\"\\\b\f\r\n\t", 7, j))
				b |= 1;
		}
		ffstr_growfmt(&s, &cap, "\t0x%08xu, /* ", b);

		for (int j = i + 32-1;  j >= i;  j--) {

			if (b & (1 << j)) {
				// bit set
				if (j >= 0x20 && j < 0x7f) {
					ffstr_growaddchar(&s, &cap, j);
				}
				else {
					// non-printable
					ffssize pos = ffs_findchar("\b\f\r\n\t", 5, j);
					if (pos >= 0)
						ffstr_growaddchar(&s, &cap, "bfrnt"[pos]);
					else
						ffstr_growaddchar(&s, &cap, 'x');
				}

			} else {
				// bit not set
				ffstr_growaddchar(&s, &cap, '.');
			}

			k++;
			if (k % 4 == 0) {
				ffstr_growaddchar(&s, &cap, ' ');
			}
			if (k % 16 == 0) {
				ffstr_growaddchar(&s, &cap, ' ');
			}
		}
		s.len -= 2;
		k = 0;
		ffstr_growaddz(&s, &cap, " */\n");
	}
	ffstr_growaddz(&s, &cap, "};\n");
	printf("%.*s\n", (int)s.len, s.ptr);
	x(0);
}

void test_write_charmap()
{
	ffstr s = {};
	ffsize cap = 0;
	ffstr_growaddz(&s, &cap, "static const char array[256] = {\n");
	for (int i = 0x00;  i <= 0xff;  i += 32) {

		ffstr_growaddz(&s, &cap, "\t");

		for (int j = i;  j != i + 32;  j++) {
			ffssize pos = ffs_findchar("\"\\\b\f\r\n\t", 7, j);
			if (pos >= 0)
				ffstr_growfmt(&s, &cap, "'%c',", "\"\\bfrnt"[pos]);
			else {
				if (j >= 0x20 && j != 0x7f)
					ffstr_growfmt(&s, &cap, "0,");
				else
					ffstr_growfmt(&s, &cap, "1,");
			}
		}

		ffstr_growaddz(&s, &cap, "\n");
	}
	ffstr_growaddz(&s, &cap, "};\n");
	printf("%.*s\n", (int)s.len, s.ptr);
	x(0);
}
