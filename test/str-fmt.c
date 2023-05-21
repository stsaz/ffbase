/** ffbase/str-format.h tester
2020, Simon Zolin
*/

#include <ffbase/string.h>
#include <ffbase/stringz.h>
#include <test/test.h>

void test_ffstr_fmtcap()
{
	xieq(-6-1, ffs_format(NULL, 0, "%s", NULL));
	xieq(-6-1, ffs_format(NULL, 0, "%s", "abcdef"));
	xieq(-7-1, ffs_format(NULL, 0, "%7s", "abcdef"));
	xieq(-5-1, ffs_format(NULL, 0, "%*s", (ffsize)5, "abcdef"));
	xieq(-1-1, ffs_format(NULL, 0, "%*s1", (ffsize)0, "abcdef"));
	xieq(-1-1, ffs_format(NULL, 0, "%%"));
	xieq(-1-1, ffs_format(NULL, 0, "%c", 'C'));
	xieq(-5-1, ffs_format(NULL, 0, "%5c", 'C'));
	xieq(-5-1, ffs_format(NULL, 0, "%*c", (ffsize)5, 'C'));
	xieq(-4*2-1, ffs_format(NULL, 0, "%*xb", (ffsize)4, "0123"));
	xieq(-1-1, ffs_format(NULL, 0, "%Z"));

	xieq(-32-1, ffs_format(NULL, 0, "%U", (ffint64)123456789));
	xieq(-32-1, ffs_format(NULL, 0, "%D", (ffint64)-123456789));
	xieq(-32-1, ffs_format(NULL, 0, "%u", (int)123456789));
	xieq(-32-1, ffs_format(NULL, 0, "%d", (int)-123456789));
	xieq(-32-1, ffs_format(NULL, 0, "%L", (ffsize)123456789));
	xieq(-32-1, ffs_format(NULL, 0, "%xU", (ffint64)0x1234abcd));
	xieq(-32-1, ffs_format(NULL, 0, "%XU", (ffint64)0x1234abcd));
	xieq(-32-1, ffs_format(NULL, 0, "%10xU", (ffint64)0x1234abcd));
	xieq(-32-1, ffs_format(NULL, 0, "%010xU", (ffint64)0x1234abcd));
	xieq(-32-1, ffs_format(NULL, 0, "%,U", (ffint64)123456789));
	xieq(-32-1, ffs_format(NULL, 0, "%p", (ffsize)0x1234af));

	xieq(-32-1-32-1, ffs_format(NULL, 0, "%f", (double)123.456));
}

void test_ffs_format_r0()
{
	char buf[4+1];
	x(4 == ffs_format_r0(buf, sizeof(buf), "%u", 1234));
	x(0 == ffs_format_r0(buf, sizeof(buf), "%u", 12345));
}

void test_ffsz_allocfmt()
{
	char *s = ffsz_allocfmt("");
	x(ffsz_eq(s, ""));
	ffmem_free(s);

	s = ffsz_allocfmt("a%5cz", '!');
	x(ffsz_eq(s, "a!!!!!z"));
	ffmem_free(s);

	s = ffsz_allocfmt("a%80cz", '!');
	x(ffsz_eq(s, "a!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!z"));
	ffmem_free(s);
}

void test_ffstr_fmt()
{
	ffstr s = {}, s1;
	ffstr_alloc(&s, 64);

	x(0 == ffstr_addfmt(&s, 5, "%s", NULL));
	x(0 == ffstr_addfmt(&s, 9, "%10s", NULL));
	x(ffstr_addfmt(&s, 64, "%s", NULL));
	xseq(&s, "(null)");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%s", "abcdef"));
	xseq(&s, "abcdef");
	x(ffstr_addfmt(&s, 64, "%S", &s));
	xseq(&s, "abcdefabcdef");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%5s", "abcdef"));
	xseq(&s, "abcde");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%7s", "abcdef"));
	x(ffstr_eq(&s, "abcdef\0", 7));
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%*s", (ffsize)5, "abcdef"));
	xseq(&s, "abcde");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%*s1", (ffsize)0, "abcdef"));
	xseq(&s, "1");
	s.len = 0;

	ffstr_setz(&s1, "abcdef");
	x(ffstr_addfmt(&s, 64, "%*S", (ffsize)3, &s1));
	xseq(&s, "abcdef");
	s.len = 0;

	ffstr_setz(&s1, "abcdef");
	x(ffstr_addfmt(&s, 64, "%*S", (ffsize)10, &s1));
	xseq(&s, "abcdef    ");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%10S", &s1));
	xseq(&s, "abcdef    ");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%%"));
	xseq(&s, "%");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%c", 'C'));
	xseq(&s, "C");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%5c", 'C'));
	xseq(&s, "CCCCC");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%*c", (ffsize)5, 'C'));
	xseq(&s, "CCCCC");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%*xb", (ffsize)4, "0123"));
	xseq(&s, "30313233");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%Z"));
	x(ffstr_eq(&s, "\0", 1));
	s.len = 0;


	x(ffstr_addfmt(&s, 64, "%U", (ffint64)123456789));
	xseq(&s, "123456789");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%D", (ffint64)-123456789));
	xseq(&s, "-123456789");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%u", (int)123456789));
	xseq(&s, "123456789");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%d", (int)-123456789));
	xseq(&s, "-123456789");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%L", (ffsize)123456789));
	xseq(&s, "123456789");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%xU", (ffint64)0x1234abcd));
	xseq(&s, "1234abcd");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%XU", (ffint64)0x1234abcd));
	xseq(&s, "1234ABCD");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%10xU", (ffint64)0x1234abcd));
	xseq(&s, "  1234abcd");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%010xU", (ffint64)0x1234abcd));
	xseq(&s, "001234abcd");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%,U", (ffint64)123456789));
	xseq(&s, "123,456,789");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%p", (ffsize)0x1234af));
#if defined __LP64__ || defined _WIN64
	xseq(&s, "00000000001234af");
#else
	xseq(&s, "001234af");
#endif
	s.len = 0;


	x(ffstr_addfmt(&s, 64, "%f", (double)123.456));
	xseq(&s, "123.456000");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%5f", (double)123.456));
	xseq(&s, "  123.456000");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%05f", (double)123.456));
	xseq(&s, "00123.456000");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%5.5f", (double)123.456));
	xseq(&s, "  123.45600");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%5.5f", (double)-123.456));
	xseq(&s, " -123.45600");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%05.5f", (double)-123.456));
	xseq(&s, "-0123.45600");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%05.5f", (double)-0.456));
	xseq(&s, "-0000.45600");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%5.5f", (double)-0.456));
	xseq(&s, "   -0.45600");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%.5f", (double)-0.456));
	xseq(&s, "-0.45600");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%5.0f", (double)123.456));
	xseq(&s, "  123");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%5.3f", (double)123.0));
	xseq(&s, "  123");
	s.len = 0;

#ifdef FF_UNIX
	x(ffstr_addfmt(&s, 64, "%q", "abcdef"));
	xseq(&s, "abcdef");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%*q", (ffsize)5, "abcdef"));
	xseq(&s, "abcde");
	s.len = 0;

#else
	x(ffstr_addfmt(&s, 64, "%q", L"abcdeф"));
	xseq(&s, "abcdeф");
	s.len = 0;

	x(ffstr_addfmt(&s, 64, "%*q", (ffsize)5, L"abcdef"));
	xseq(&s, "abcde");
	s.len = 0;
#endif

	// x(ffstr_addfmt(&s, 64, "%E", FF_EINVAL));
	// xseq(&s, "(22) Invalid argument");
	// s.len = 0;

	ffstr_free(&s);

	test_ffstr_fmtcap();
	test_ffsz_allocfmt();
	test_ffs_format_r0();
}
