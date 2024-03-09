/** ffbase/string.h tester
2020, Simon Zolin
*/

#include <ffbase/stringz.h>
#include <test/test.h>

static void charr_find()
{
	const char sss[][4] = {
		"a",
		"b",
		"c",
		"c0",
		"c1",
		"c11",
		"c111",
		"d",
		"e",
	};
	xieq(-1, ffcharr_findsorted(sss, FF_COUNT(sss), sizeof(sss[0]), "z", 1));
	for (ffuint i = 0;  i != FF_COUNT(sss);  i++) {
		xieq(i, ffcharr_findsorted(sss, FF_COUNT(sss), sizeof(sss[0]), sss[i], ffmin(ffsz_len(sss[i]), 4)));
	}

	static const struct {
		char name[4];
		uint val;
	} arr[] = {
		{"aa", 4},
		{"bb", 3},
		{"cc", 2},
		{"dd", 1},
	};
	for (ffuint i = 0;  i != FF_COUNT(arr);  i++) {
		xieq(i, ffcharr_find_sorted_padding(arr, FF_COUNT(arr), 4, 4, arr[i].name, 2));
	}
}

static void szarr_find()
{
	const char *const ss[] = {
		"a",
		"b",
		"c",
		"d",
		"e",
	};

	xieq(-1, ffszarr_find(ss, FF_COUNT(ss), "z", 1));
	xieq(1, ffszarr_find(ss, FF_COUNT(ss), "b", 1));

	xieq(-1, ffszarr_ifind(ss, FF_COUNT(ss), "z", 1));
	xieq(4, ffszarr_ifind(ss, FF_COUNT(ss), "E", 1));

	xieq(-1, ffszarr_findsorted(ss, FF_COUNT(ss), "z", 1));
	xieq(1, ffszarr_findsorted(ss, FF_COUNT(ss), "b", 1));

	xieq(-1, ffszarr_ifindsorted(ss, FF_COUNT(ss), "z", 1));
	xieq(4, ffszarr_ifindsorted(ss, FF_COUNT(ss), "E", 1));
}

void test_strz()
{
	char *s;
	s = ffsz_dup("123");
	x(ffsz_eq(s, "123"));
	ffmem_free(s);

	s = ffsz_dupn("123", 2);
	x(ffsz_eq(s, "12"));
	ffmem_free(s);

	x(ffsz_eq("s123", "s123"));
	x(0 == ffsz_cmp("s123", "s123"));
	x(0 == ffsz_icmp("A123b", "a123B"));
	x(ffsz_icmp("A123c", "a123B") > 0);
	x(ffsz_icmp("A123b", "a123C") < 0);

	x(ffsz_match("key=val", "key", 3));
	x(ffsz_match("key=val", "key=val", 7));
	x(!ffsz_match("key=val", "key1", 4));
	x(!ffsz_match("key=val", "keykeykeykey", 12));

	x(ffsz_matchz("key=val", "key"));
	x(ffsz_matchz("key=val", "key=val"));
	x(!ffsz_matchz("key=val", "key1"));
	x(!ffsz_matchz("key=val", "keykeykeykey"));

	szarr_find();
	charr_find();

	char buf[10];
	xieq(4, ffsz_copyz(buf, 4, "12345"));
	x(ffsz_eq(buf, "123"));

	ffmem_zero_obj(buf);
	ffsz_copyn(buf, 4, "123456", 2);
	x(ffsz_eq(buf, "12"));

	ffmem_zero_obj(buf);
	ffsz_copyn(buf, 4, "123456", 6);
	x(ffsz_eq(buf, "123"));

	xieq(4, ffsz_findchar("asdf1234", '1'));
	xieq(-1, ffsz_findchar("asdf1234", '\0'));
	xieq(-1, ffsz_findchar("asdf1234", '5'));
	xieq(-1, ffsz_findchar("", '5'));
}
