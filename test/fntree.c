/** ffbase: fntree.h tester
2022, Simon Zolin */

#include <ffbase/fntree.h>
#include <test/test.h>

static void test_fntree_cmp()
{
	fntree_block *l, *r, *d2, *d4;
	fntree_entry *e;
	ffstr s;

	/*
	a1     a1
	       a1r
	d2     d2
	d2/f21
	       d2/z2
	f3     f3
	       d4
	       d4/z31
	f4
	*/

	ffstr_setz(&s, "/lpath"); x(NULL != (l = fntree_create(s)));
	fntree_addz(&l, "a1", 0);
	e = fntree_addz(&l, "d2", 0);
		ffstr_setz(&s, "/path/d2"); d2 = fntree_create(s);
		fntree_attach(e, d2);
		fntree_addz(&d2, "f21", 0);
	fntree_addz(&l, "f3", 0);
	fntree_addz(&l, "f4", 0);

	ffstr_setz(&s, "/rpath"); r = fntree_create(s);
	fntree_addz(&r, "a1", 0);
	fntree_addz(&r, "a1r", 0);
	e = fntree_addz(&r, "d2", 0);
		ffstr_setz(&s, "/rpath/d2"); d2 = fntree_create(s);
		fntree_attach(e, d2);
		fntree_addz(&d2, "z2", 0);
	fntree_addz(&r, "f3", 0);
	e = fntree_addz(&r, "d4", 0);
		ffstr_setz(&s, "/rpath/d4"); d4 = fntree_create(s);
		fntree_attach(e, d4);
		fntree_addz(&d4, "z31", 0);

	fntree_cmp c = {};
	fntree_cmp_init(&c, l, r, _fntree_cmp_default, NULL);
	fntree_entry *le, *re;
	fntree_block *lb, *rb;
	int i;

	i = fntree_cmp_next(&c, &le, &re, &lb, &rb);
	xlog("%s -- %s", (le)?le->name:NULL, (re)?re->name:NULL);
	xieq(i, FNTREE_CMP_EQ);
	// a1

	i = fntree_cmp_next(&c, &le, &re, &lb, &rb);
	xlog("%s -- %s", (le)?le->name:NULL, (re)?re->name:NULL);
	xieq(i, FNTREE_CMP_RIGHT);
	// . a1r

	i = fntree_cmp_next(&c, &le, &re, &lb, &rb);
	xlog("%s -- %s", (le)?le->name:NULL, (re)?re->name:NULL);
	xieq(i, FNTREE_CMP_EQ);
	// d2

	i = fntree_cmp_next(&c, &le, &re, &lb, &rb);
	xlog("%s -- %s", (le)?le->name:NULL, (re)?re->name:NULL);
	xieq(i, FNTREE_CMP_LEFT);
	// d2/f21 .

	i = fntree_cmp_next(&c, &le, &re, &lb, &rb);
	xlog("%s -- %s", (le)?le->name:NULL, (re)?re->name:NULL);
	xieq(i, FNTREE_CMP_RIGHT);
	// . d2/z2

	i = fntree_cmp_next(&c, &le, &re, &lb, &rb);
	xlog("%s -- %s", (le)?le->name:NULL, (re)?re->name:NULL);
	xieq(i, FNTREE_CMP_EQ);
	// f3

	i = fntree_cmp_next(&c, &le, &re, &lb, &rb);
	xlog("%s -- %s", (le)?le->name:NULL, (re)?re->name:NULL);
	xieq(i, FNTREE_CMP_RIGHT);
	// . d4

	i = fntree_cmp_next(&c, &le, &re, &lb, &rb);
	xlog("%s -- %s", (le)?le->name:NULL, (re)?re->name:NULL);
	xieq(i, FNTREE_CMP_RIGHT);
	// . d4/z31

	i = fntree_cmp_next(&c, &le, &re, &lb, &rb);
	xlog("%s -- %s", (le)?le->name:NULL, (re)?re->name:NULL);
	xieq(i, FNTREE_CMP_LEFT);
	// . f4

	i = fntree_cmp_next(&c, &le, &re, &lb, &rb);
	xlog("%s -- %s", (le)?le->name:NULL, (re)?re->name:NULL);
	xieq(i, -1);

	fntree_free_all(l);
	fntree_free_all(r);
}

void test_fntree()
{
	fntree_block *b, *b2, *pb;
	fntree_entry *e;
	ffstr s;

	ffstr_setz(&s, "/path"); x(NULL != (b = fntree_create(s)));
	fntree_addz(&b, "a1", 0);
	e = fntree_addz(&b, "d2", 0);
		ffstr_setz(&s, "/path/d2"); b2 = fntree_create(s);
		fntree_attach(e, b2);
		fntree_addz(&b2, "f21", 0);
	fntree_addz(&b, "f3", 0);

// fntree_cur_next
	fntree_cursor c = {};
	x(NULL != (e = fntree_cur_next(&c, b)));
	s = fntree_name(e); xseq(&s, "a1");

	x(NULL != (e = fntree_cur_next(&c, b)));
	s = fntree_name(e); xseq(&s, "d2");

	x(NULL != (e = fntree_cur_next(&c, b)));
	s = fntree_name(e); xseq(&s, "f3");

	x(NULL == fntree_cur_next(&c, b));

	s = fntree_path(b2);
	xseq(&s, "/path/d2");

// fntree_cur_next_r
	ffmem_zero_obj(&c);

	pb = b;
	x(NULL != (e = fntree_cur_next_r(&c, &pb)));
	x(pb == b);
	s = fntree_name(e); xseq(&s, "a1");

	x(NULL != (e = fntree_cur_next_r(&c, &pb)));
	s = fntree_name(e); xseq(&s, "d2");

	x(NULL != (e = fntree_cur_next_r(&c, &pb)));
	x(pb == b2);
	s = fntree_name(e); xseq(&s, "f21");
	pb = b;

	x(NULL != (e = fntree_cur_next_r(&c, &pb)));
	s = fntree_name(e); xseq(&s, "f3");

	x(NULL == fntree_cur_next_r(&c, &pb));

// fntree_cur_next_r_ctx
	ffmem_zero_obj(&c);

	pb = b;
	x(NULL != (e = fntree_cur_next_r_ctx(&c, &pb)));
	x(pb == b);
	s = fntree_name(e); xseq(&s, "a1");

	x(NULL != (e = fntree_cur_next_r_ctx(&c, &pb)));
	x(pb == b);
	s = fntree_name(e); xseq(&s, "d2");

	x(NULL != (e = fntree_cur_next_r_ctx(&c, &pb)));
	x(pb == b);
	s = fntree_name(e); xseq(&s, "f3");

	x(NULL != (e = fntree_cur_next_r_ctx(&c, &pb)));
	x(pb == b2);
	s = fntree_name(e); xseq(&s, "f21");

	x(NULL == fntree_cur_next_r_ctx(&c, &pb));

	fntree_free_all(b);

	test_fntree_cmp();
}
