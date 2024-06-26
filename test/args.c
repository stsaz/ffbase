/** ffbase: args.h tester
2023, Simon Zolin
*/

#include <ffbase/args.h>
#include <test/test.h>

struct com {
	ffbyte sw;
	ffuint64 _uint64;
	ffuint _uint;
	int _int;
	double _double;
	ffstr str;
	char *sz;
	char *sz_copy;
	ffuint done :1;
};

int f_sw(struct com *com) {
	com->sw = 1;
	return 0;
}
int f_str(struct com *com, ffstr v) {
	com->str = v;
	return 0;
}
int f_sz(struct com *com, const char *v) {
	com->sz = (char*)v;
	return 0;
}
int f_sz_copy(struct com *com, const char *v) {
	com->sz_copy = (char*)v;
	return 0;
}
int f_size(struct com *com, ffuint64 v) {
	com->_uint64 = v;
	return 0;
}
int f_uint64(struct com *com, ffuint64 v) {
	com->_uint = v;
	return 0;
}
int f_int64(struct com *com, ffuint64 v) {
	com->_int = v;
	return 0;
}
int f_float(struct com *com, double v) {
	com->_double = v;
	return 0;
}
int f_done(struct com *com) {
	com->done = 1;
	return 0;
}

#define O(m)  (void*)(ffsize)FF_OFF(struct com, m)
const struct ffarg scheme_obj[] = {
	{ "-float",	'F',	O(_double) },
	{ "-int",	'd',	O(_int) },
	{ "-size",	'Z',	O(_uint64) },
	{ "-str",	'S',	O(str) },
	{ "-strcp",	'=s',	O(sz_copy) },
	{ "-strz",	's',	O(sz) },
	{ "-sw",	'1',	O(sw) },
	{ "-uint",	'u',	O(_uint) },
	{ "",		0,		f_done }
};

const struct ffarg scheme_func[] = {
	{ "-float",	'F',	f_float },
	{ "-int",	'd',	f_int64 },
	{ "-size",	'Z',	f_size },
	{ "-str",	'S',	f_str },
	{ "-strcp",	'=s',	f_sz_copy },
	{ "-strz",	's',	f_sz },
	{ "-sw",	'0',	f_sw },
	{ "-uint",	'u',	f_uint64 },
	{ "",		0,		f_done }
};

int f_any(struct com *com, ffstr v) {
	com->str = v;
	return 0;
}
const struct ffarg scheme_any[] = {
	{ "-uint",	'u',	O(_uint) },
	{ "\0\1",	'S',	f_any },
	{}
};
const struct ffarg scheme_any_redirect[] = {
	{ "\0\1",	'>',	scheme_any },
	{}
};

const struct ffarg scheme_inner[] = {
	{ "val",	'u',	O(_uint) },
	{}
};
const struct ffarg scheme_outer[] = {
	{ "a",		'>',	scheme_inner },
	{}
};

struct ffarg_ctx f_ctx(struct com *com) {
	struct ffarg_ctx ax = { scheme_inner, com };
	return ax;
}
const struct ffarg scheme_outer_func[] = {
	{ "a",		'{',	f_ctx },
	{}
};

void com_check(struct com *com)
{
	xieq(com->_uint64, 123*1024*1024);
	xieq(com->_uint, 123);
	xieq(com->_int, -123);
	x(com->_double == -123.1);
	xseq(&com->str, "str str");
	x(com->done == 1);
	x(ffsz_eq(com->sz, "strz"));
	x(ffsz_eq(com->sz_copy, "szcopy"));
	ffmem_free(com->sz_copy);  com->sz_copy = NULL;
}

static void test_args_merge()
{
	static const struct ffarg a1[] = {
		{ "123",	'1', NULL },
		{ "456",	'1', NULL },
		{ "\0\1",	'1', NULL },
	};
	static const struct ffarg a2[] = {
		{ "234",	'1', NULL },
		{ "345",	'1', NULL },
		{ "567",	'1', NULL },
		{ "\0",		'1', NULL },
	};
	struct ffarg a[FF_COUNT(a1) + FF_COUNT(a2)] = {};
	xieq(FF_COUNT(a), ffarg_merge(a, FF_COUNT(a), a1, FF_COUNT(a1), a2, FF_COUNT(a2), 0));
	static const char ar[][4] = {
		"123",
		"234",
		"345",
		"456",
		"567",
		"\0\1",
		""
	};
	for (ffuint i = 0;  i < FF_COUNT(ar);  i++) {
		x(!ffmem_cmp(a[i].name, ar[i], 4));
	}
}

void test_args()
{
	struct com com = {};
	struct ffargs a = {};

	char* argv[] = { "-str", "str str",  "-strz", "strz",  "-strcp", "szcopy",
		"-uint", "123",  "-int", "-123",  "-size", "123m",
		"-float", "-123.1",  "-sw"
	};
	xieq(ffargs_process_argv(&a, scheme_obj, &com, 0, argv, FF_COUNT(argv)), 0);
	com_check(&com);

	ffmem_zero_obj(&a);  ffmem_zero_obj(&com);
	xieq(ffargs_process_argv(&a, scheme_func, &com, 0, argv, FF_COUNT(argv)), 0);
	com_check(&com);

	ffmem_zero_obj(&a);  ffmem_zero_obj(&com);
	char line[] = " -str \"str str\"  -strz strz  -strcp szcopy  "
		"-uint 123  -int -123  -size 123m  "
		"-float -123.1  -sw ";
	xieq(ffargs_process_line(&a, scheme_obj, &com, 0, line), 0);
	com_check(&com);

	ffmem_zero_obj(&a);  ffmem_zero_obj(&com);
	xieq(ffargs_process_line(&a, scheme_outer, &com, 0, "a val 123"), 0);
	xieq(com._uint, 123);

	ffmem_zero_obj(&a);  ffmem_zero_obj(&com);
	xieq(ffargs_process_line(&a, scheme_outer_func, &com, 0, "a val 123"), 0);
	xieq(com._uint, 123);

	ffmem_zero_obj(&a);  ffmem_zero_obj(&com);
	xieq(ffargs_process_line(&a, scheme_any, &com, 0, "-uint 123  any"), 0);
	xieq(com._uint, 123);
	xseq(&com.str, "any");

	ffmem_zero_obj(&a);  ffmem_zero_obj(&com);
	xieq(ffargs_process_line(&a, scheme_any_redirect, &com, 0, "-uint 123  any"), 0);
	xieq(com._uint, 123);
	xseq(&com.str, "any");

	ffmem_zero_obj(&a);  ffmem_zero_obj(&com);
	xieq(ffargs_process_line(&a, scheme_obj, &com, FFARGS_O_PARTIAL, "-u 123 -s str"), -FFARGS_E_ARG);
	xieq(com._uint, 123);

	test_args_merge();
}
