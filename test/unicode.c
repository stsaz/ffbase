/** ffbase/unicode.h tester
2020, Simon Zolin
*/

#include <ffbase/unicode.h>
#include <test/test.h>


void test_utf8()
{
	xieq(1, ffutf8_size(0x00));
	xieq(1, ffutf8_size(L'A'));
	xieq(2, ffutf8_size(0x044f));
	xieq(3, ffutf8_size(0xFFFD));

	char utf8[6];

	xieq(1, ffutf8_encode(utf8, 1, L'A'));
	x(!memcmp(utf8, "A", 1));

	xieq(2, ffutf8_encode(utf8, 2, 0x0444 /*CYRILLIC SMALL LETTER EF*/));
	x(!memcmp(utf8, "\xD1\x84", 2));

	xieq(0, ffutf8_encode(utf8, 2, 0x0E1D));
	xieq(3, ffutf8_encode(utf8, 3, 0x0E1D /*THAI CHARACTER FO FA*/));
	x(!memcmp(utf8, "\xE0\xB8\x9D", 3));

	xieq(4, ffutf8_encode(utf8, 4, 0x1F600 /*GRINNING FACE*/));
	x(!memcmp(utf8, "\xF0\x9F\x98\x80", 4));


	ffuint n;
	xieq(1, ffutf8_decode("A", 1, &n));
	xieq('A', n);

	xieq(2, ffutf8_decode("\xD1\x84", 2, &n));
	xieq(0x0444, n);

	xieq(3, ffutf8_decode("\xE0\xB8\x9D", 3, &n));
	xieq(0x0E1D, n);

	xieq(4, ffutf8_decode("\xF0\x9F\x98\x80", 4, &n));
	xieq(0x1F600, n);

	// incomplete
	xieq(-1, ffutf8_decode("", 0, &n));
	xieq(-2, ffutf8_decode("\xD1", 1, &n));
	xieq(-3, ffutf8_decode("\xE0", 1, &n));
	xieq(-4, ffutf8_decode("\xF0", 1, &n));
	xieq(-4, ffutf8_decode("\xF0\x9F\x98\x80", 3, &n));

	// invalid
	xieq(0, ffutf8_decode("\x80", 1, &n));
	xieq(0, ffutf8_decode("\xD1\x04", 2, &n));

	// utf-8 -> utf-8
	xieq(5, ffutf8_from_utf8(NULL, 0, "a\x80""b", 3, 0));
	xieq(5, ffutf8_from_utf8(utf8, sizeof(utf8), "a\x80""b", 3, 0));
	x(!memcmp(utf8, "a\xEF\xBF\xBD" "b", 5));
}

void test_utf16()
{
	char utf8[6];

	// get cap (incomplete)
	xieq(3, ffutf8_from_utf16(NULL, 0, "\x0E", 1, FFUNICODE_UTF16LE)); // incomplete code
	xieq(3, ffutf8_from_utf16(NULL, 0, "\x3D\xD8", 2, FFUNICODE_UTF16LE)); // incomplete pair
	xieq(3, ffutf8_from_utf16(NULL, 0, "\x3D\xD8\x00", 3, FFUNICODE_UTF16LE)); // incomplete code in pair
	xieq(4, ffutf8_from_utf16(NULL, 0, "\x3D\xD8\x00\xDE", 4, FFUNICODE_UTF16LE));

	// incomplete
	xieq(-1, ffutf8_from_utf16(utf8, 2, "\x0E", 1, FFUNICODE_UTF16LE)); // incomplete code
	xieq(-1, ffutf8_from_utf16(utf8, 2, "\x3D\xD8", 2, FFUNICODE_UTF16LE)); // incomplete pair
	xieq(-1, ffutf8_from_utf16(utf8, 2, "\x3D\xD8\x00", 3, FFUNICODE_UTF16LE)); // incomplete code in pair

	// decode incomplete
	xieq(3, ffutf8_from_utf16(utf8, 3, "\x0E", 1, FFUNICODE_UTF16LE)); // incomplete code
	x(!memcmp(utf8, _FFUTF8_REPLCHAR, 3));
	xieq(3, ffutf8_from_utf16(utf8, 3, "\x3D\xD8", 2, FFUNICODE_UTF16LE)); // incomplete pair
	x(!memcmp(utf8, _FFUTF8_REPLCHAR, 3));
	xieq(3, ffutf8_from_utf16(utf8, 3, "\x3D\xD8\x00", 3, FFUNICODE_UTF16LE)); // incomplete code in pair
	x(!memcmp(utf8, _FFUTF8_REPLCHAR, 3));

	xieq(4, ffutf8_from_utf16(utf8, 4, "\x3D\xD8\x00\xDE" /*0xD83D 0xDE00*/, 4, FFUNICODE_UTF16LE));
	x(!memcmp(utf8, "\xF0\x9F\x98\x80", 4));

	xieq(4, ffutf8_from_utf16(utf8, 4, "\xD8\x3D\xDE\x00" /*0xD83D 0xDE00*/, 4, FFUNICODE_UTF16BE));
	x(!memcmp(utf8, "\xF0\x9F\x98\x80", 4));

	ffushort utf16[2];
	xieq(2, ffutf8_to_utf16((char*)utf16, 1*2, "A", 1, FFUNICODE_UTF16LE));
	xieq('A', utf16[0]);

	xieq(2, ffutf8_to_utf16((char*)utf16, 1*2, "\xD1\x84", 2, FFUNICODE_UTF16LE));
	xieq(0x0444, utf16[0]);

	xieq(2, ffutf8_to_utf16((char*)utf16, 1*2, "\xD1\x84", 2, FFUNICODE_UTF16BE));
	xieq(0x4404, utf16[0]);

	xieq(2, ffutf8_to_utf16((char*)utf16, 1*2, "\xE0\xB8\x9D", 3, FFUNICODE_UTF16LE));
	xieq(0x0E1D, utf16[0]);

	// xieq(4, ffutf8_to_utf16(utf16, 2*2, "\xF0\x9F\x98\x80", 4, FFUNICODE_UTF16LE));
}

void test_unicode_w_u()
{
#ifdef FF_WIN
	char s[8];

	xieq(3, ffs_wtou(NULL, 0, (wchar_t*)"\x1D\x0E", 1));
	xieq(-1, ffs_wtou(s, 2, (wchar_t*)"\x1D\x0E", 1));
	xieq(3, ffs_wtou(s, 3, (wchar_t*)"\x1D\x0E", 1));
	x(!memcmp(s, "\xE0\xB8\x9D", 3));

	xieq(3, ffs_wtouz(NULL, 0, (wchar_t*)"\x1D\x0E\x00"));
	xieq(-1, ffs_wtouz(s, 2, (wchar_t*)"\x1D\x0E\x00"));
	xieq(3, ffs_wtouz(s, 3, (wchar_t*)"\x1D\x0E\x00"));
	x(!memcmp(s, "\xE0\xB8\x9D", 3));

	xieq(4, ffsz_wtou(NULL, 0, (wchar_t*)"\x1D\x0E\x00"));
	xieq(-1, ffsz_wtou(s, 3, (wchar_t*)"\x1D\x0E\x00"));
	x(s[0] == '\0');
	s[3] = 'z';
	xieq(4, ffsz_wtou(s, 4, (wchar_t*)"\x1D\x0E\x00"));
	x(!memcmp(s, "\xE0\xB8\x9D\x00", 4));

	xieq(2, ffsz_utow(NULL, 0, "\xE0\xB8\x9D"));
	xieq(-1, ffsz_utow((wchar_t*)s, 1, "\xE0\xB8\x9D"));
	x(!memcmp(s, "\x00\x00", 2));
	s[2] = 'z';
	s[3] = 'z';
	xieq(2, ffsz_utow((wchar_t*)s, 2, "\xE0\xB8\x9D"));
	x(!memcmp(s, "\x1D\x0E\x00\x00", 4));

	char *sz = ffsz_alloc_wtou((wchar_t*)"\x1D\x0E\x00\x00");
	x(!memcmp(sz, "\xE0\xB8\x9D\x00", 4));
	ffmem_free(sz);

	wchar_t *wz = ffsz_alloc_utow("\xE0\xB8\x9D\x00");
	x(!memcmp(wz, "\x1D\x0E\x00\x00", 4));
	ffmem_free(wz);
#endif
}

void test_utf_cp()
{
	ffstr s = {};
	ffsize cap = 0;
	xieq(2, ffstr_growadd_codepage(&s, &cap, "\xff", 1, FFUNICODE_WIN1251));
	xseq(&s, "я");
	ffstr_free(&s);
}

void test_unicode()
{
	ffsize n = 2;
	xieq(-1, ffutf_bom("\xef\xbb\xbf\x00", &n));

	n = 4;
	xieq(FFUNICODE_UTF8, ffutf_bom("\xef\xbb\xbf\x00", &n));
	xieq(3, n);

	test_utf8();
	test_utf16();
	test_unicode_w_u();
	test_utf_cp();
}
