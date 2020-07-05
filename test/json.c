/** ffbase: json.h, json-writer.h tester
2020, Simon Zolin
*/

#include <ffbase/base.h>
#include <ffbase/json.h>
#include <ffbase/json-writer.h>
#include <ffbase/json-scheme.h>
#include <test/test.h>

#define STR(s)  (char*)(s), FFS_LEN(s)


void test_json_err()
{
	ffjson j;

	ffjson_init(&j);
	xieq(-FFJSON_EBADCHAR, ffjson_validate(&j, STR("{,")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EBADCHAR, ffjson_validate(&j, STR("[123;")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EBADSTR, ffjson_validate(&j, STR("\"val\n")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EFIN, ffjson_validate(&j, STR("\"val\"1")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EFIN, ffjson_validate(&j, STR("\"val\",")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EFIN, ffjson_validate(&j, STR("\"val\"]")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EFIN, ffjson_validate(&j, STR("123,")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EFIN, ffjson_validate(&j, STR("123]")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EFIN, ffjson_validate(&j, STR("[123],")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EFIN, ffjson_validate(&j, STR("[123]]")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EBADCHAR, ffjson_validate(&j, STR("{\"key\",")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EBADCHAR, ffjson_validate(&j, STR("{\"key\":,")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EBADBOOL, ffjson_validate(&j, STR("truE")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EBADBOOL, ffjson_validate(&j, STR("falsE")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EBADNULL, ffjson_validate(&j, STR("nulL")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EINCOMPLETE, ffjson_validate(&j, STR(".")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_ELARGE, ffjson_validate(&j, STR("123456789123456789123456789123456789123456789123456789123456789123456789")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EBADESCAPE, ffjson_validate(&j, STR("\"\\1\"")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EBADCTX, ffjson_validate(&j, STR("[123}")));
	ffjson_fin(&j);

	ffjson_init(&j);
	xieq(-FFJSON_EBADCOMMENT, ffjson_validate(&j, STR(" /")));
	ffjson_fin(&j);

	ffjson_init(&j);
	j.flags |= FFJSON_FCOMMENTS;
	xieq(-FFJSON_EBADCOMMENT, ffjson_validate(&j, STR(" /z")));
	ffjson_fin(&j);
}

void test_json_unescape()
{
	ffstr s = {};
	const char *ss = "my \\u0001 \\b \\t \\f \\r\\n \\\\ \\/ \\\" \\u0444 \\uD83D\\uDE02 string";
	const char *ss2 = "my \x01 \b \t \f \r\n \\ / \" ф 😂 string";
	ffsize cap = ffsz_len(ss2);
	ffstr_alloc(&s, cap);
	s.len = ffjson_unescape(s.ptr, cap, ss, ffsz_len(ss));
	x((ffssize)s.len > 0);
	xseq(&s, ss2);

	xieq(0, ffjson_unescape(s.ptr, cap - 1, ss, ffsz_len(ss)));
	xieq(-1, ffjson_unescape(s.ptr, cap, ss, ffsz_len(ss) - FFS_LEN("2 string")));

	ffstr_free(&s);
}

void test_json_rw()
{
	ffjson j = {};

	ffjson_init(&j);
	j.flags |= FFJSON_FCOMMENTS;

	ffjsonw jw = {};
	ffjsonw_init(&jw);

	ffjsonw jwp = {};
	ffjsonw_init(&jwp);
	jwp.flags |= FFJSONW_FPRETTY_TABS;

	ffstr s = {}, s2, c;
	x(0 == file_readall(&s, "./data/test.json"));
	x(s.len != 0);
	s2 = s;

	for (;;) {
		c = s;
		c.len = 1;
		int r = ffjson_parse(&j, &c);
		if (c.len == 0)
			ffstr_shift(&s, 1);

		if (r < 0) {
			ffstr ps = {};
			ffsize cap = 0;
			ffstr_growfmt(&ps, &cap, "ffjson_parse: %u:%u at \"%*s\" %d %s\n"
				, j.line, j.linechar
				, ffmin(s.len, 16), s.ptr
				, r, ffjson_errstr(r));
			write(2, ps.ptr, ps.len);
			ffstr_free(&ps);
		}

		x(r >= 0);

		switch (r) {
		case 0:
			break;

		case FFJSON_TNULL:
			x(0 < ffjsonw_addnull(&jw));
			x(0 < ffjsonw_addnull(&jwp));
			break;

		case FFJSON_TSTR:
		case FFJSON_TOBJ_KEY:
			x(0 < ffjsonw_add(&jw, r, &j.val));
			x(0 < ffjsonw_add(&jwp, r, &j.val));
			break;

		case FFJSON_TINT:
			x(0 < ffjsonw_addint(&jw, j.intval));
			x(0 < ffjsonw_addint(&jwp, j.intval));
			break;

		case FFJSON_TNUM:
			x(0 < ffjsonw_addnum(&jw, j.fltval));
			x(0 < ffjsonw_addnum(&jwp, j.fltval));
			break;

		case FFJSON_TBOOL:
			x(0 < ffjsonw_addbool(&jw, j.intval));
			x(0 < ffjsonw_addbool(&jwp, j.intval));
			break;

		case FFJSON_TOBJ_OPEN:
		case FFJSON_TOBJ_CLOSE:
		case FFJSON_TARR_OPEN:
		case FFJSON_TARR_CLOSE:
			x(0 < ffjsonw_add(&jw, r, NULL));
			x(0 < ffjsonw_add(&jwp, r, NULL));
			break;

		default:
			x(0);
		}

		if (s.len == 0)
			break;
	}

	xieq(0, ffjson_fin(&j));
	ffstr_free(&s2);

	ffstr tc = {};
	x(0 == file_readall(&tc, "./data/test-compressed.json"));
	if (!ffslice_eq2T((ffslice*)&jw.buf, &tc, char)) {
		write(2, jw.buf.ptr, jw.buf.len);
		x(0);
	}
	ffstr_free(&tc);

	ffstr tp = {};
	x(0 == file_readall(&tp, "./data/test-pretty.json"));
	if (!ffslice_eq2T((ffslice*)&jwp.buf, &tp, char)) {
		write(2, jwp.buf.ptr, jwp.buf.len);
		x(0);
	}
	ffstr_free(&tp);

	ffjsonw_close(&jw);
	ffjsonw_close(&jwp);
}


typedef struct jstruct jstruct;
struct jstruct {
	ffstr s;
	ffint64 n;
	double flt;
	ffbyte b;

	ffbyte bnull;
	ffint64 inull;
	double fnull;
	ffstr snull;

	jstruct *obj;
	ffvec arr; // int64[]
	jstruct *arr_obj;
};

static int js_obj(ffjson_scheme *js, jstruct *o);
static int js_arr(ffjson_scheme *js, jstruct *o);
static int js_arr_el(ffjson_scheme *js, jstruct *o);

static const ffjson_arg jargs_arr[] = {
	{ "*",	FFJSON_TANY,	(ffsize)js_arr_el },
	{},
};

#define OFF(m)  FF_OFF(jstruct, m)
static const ffjson_arg jargs[] = {
	{ "str",	FFJSON_TSTR,	OFF(s) },
	{ "flt",	FFJSON_TNUM,	OFF(flt) },
	{ "int",	FFJSON_TINT,	OFF(n) },
	{ "bool",	FFJSON_TBOOL,	OFF(b) },

	{ "bnull",	FFJSON_TBOOL,	OFF(bnull) },
	{ "inull",	FFJSON_TINT,	OFF(inull) },
	{ "fnull",	FFJSON_TNUM,	OFF(fnull) },
	{ "snull",	FFJSON_TSTR,	OFF(snull) },

	{ "obj",	FFJSON_TOBJ_OPEN,	(ffsize)js_obj },
	{ "arr",	FFJSON_TARR_OPEN,	(ffsize)js_arr },
	{},
};
#undef OFF

static int js_obj(ffjson_scheme *js, jstruct *o)
{
	o->obj = ffmem_new(jstruct);
	ffjson_scheme_addctx(js, jargs, o->obj);
	return 0;
}

static int js_arr(ffjson_scheme *js, jstruct *o)
{
	ffjson_scheme_addctx(js, jargs_arr, o);
	return 0;
}

static int js_arr_el(ffjson_scheme *js, jstruct *o)
{
	if (js->jtype == FFJSON_TOBJ_OPEN) {
		o->arr_obj = ffmem_new(jstruct);
		ffjson_scheme_addctx(js, jargs, o->arr_obj);
		return 0;
	}

	if (js->jtype != FFJSON_TINT)
		return FFJSON_EBADINT;
	*ffvec_pushT(&o->arr, ffint64) = js->parser->intval;
	return 0;
}


void test_json_scheme()
{
	ffstr err = {};
	ffstr d = {};
	x(0 == file_readall(&d, "./data/object.json"));
	ffstr s = d;

	jstruct o = {};
	o.snull.len = 1;
	o.bnull = 1;
	o.inull = 1;
	o.fnull = 1;

	int r = ffjson_parse_object(jargs, &o, &s, 0, FFJSON_FCOMMENTS, &err);
	if (r != 0)
		fprintf(stderr, "%d %.*s\n", r, (int)err.len, err.ptr);
	ffstr_free(&err);
	xieq(0, r);
	xseq(&o.s, "string");
	x(o.flt == 123.456);
	x(o.n == 1234);
	x(o.b == 1);
	x(o.bnull == 0);
	x(o.inull == 0);
	x(o.fnull == 0);
	x(o.snull.len == 0);
	ffstr_free(&o.s);

	xseq(&o.obj->s, "objstring");
	x(o.obj->flt == 123.456);
	x(o.obj->n == 1234);
	x(o.obj->b == 1);
	x(o.obj->obj != NULL);
	x(*ffslice_itemT(&o.obj->arr, 0, ffint64) == 1);
	x(*ffslice_itemT(&o.obj->arr, 1, ffint64) == 2);
	xieq(12345, o.obj->arr_obj->n);
	ffstr_free(&o.obj->s);
	ffmem_free(o.obj->obj);
	ffmem_free(o.obj);

	ffstr_free(&d);
}


void test_json()
{
	test_json_err();
	test_json_unescape();
	test_json_rw();
	test_json_scheme();
}
