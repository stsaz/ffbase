/** ffbase: stream.h tester
2022, Simon Zolin */

#include <ffbase/stream.h>
#include <test/test.h>

void test_stream()
{
	ffstream s = {};
	ffstream_realloc(&s, 8);
	int r;
	ffstr in, v;

	// empty -> pointer
	ffstr_setz(&in, "12");
	r = ffstream_gather(&s, in, 4, &v);
	x(r == 2);
	xseq(&v, "12");

	// pointer -> (copy) half
	ffstr_setz(&in, "345");
	r = ffstream_gather(&s, in, 4, &v);
	x(r == 2);
	xseq(&v, "1234");
	ffstream_consume(&s, 2); // "..34"

	// half -> full
	ffstr_setz(&in, "5678");
	r = ffstream_gather(&s, in, 6, &v);
	x(r == 4);
	xseq(&v, "345678");
	ffstream_consume(&s, 6); // "........"

	// empty -> pointer
	ffstr_setz(&in, "1234");
	r = ffstream_gather(&s, in, 3, &v);
	x(r == 4);
	xseq(&v, "1234");
	x(v.ptr == in.ptr);
	ffstream_consume(&s, 2); // "..34"

	// pointer -> (copy) half
	ffstr_setz(&in, "56789abc");
	r = ffstream_gather(&s, in, 7, &v);
	x(r == 5);
	xseq(&v, "3456789");
	x(v.ptr == s.buf);
	ffstream_consume(&s, 1); // ".456789"

	// half -> (move) full
	ffstr_setz(&in, "abc");
	r = ffstream_gather(&s, in, 8, &v);
	x(r == 2);
	xseq(&v, "456789ab");
	x(v.ptr == s.buf);

	x(0 == ffstream_realloc(&s, 12));
	x(s.cap == 16);
	v = ffstream_view(&s);
	xseq(&v, "456789ab");

	ffstream_free(&s);
}
