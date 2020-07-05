/** ffbase: list.h & chain.h tester
2020, Simon Zolin
*/

#include <ffbase/base.h>
#include <ffbase/vector.h>
#include <test/test.h>


void test_vec()
{
	ffvec v = {};
	ffvec_null(&v);

	x(NULL != ffvec_allocT(&v, 5, char));
	*ffvec_pushT(&v, char) = '0';

	x(9 == ffvec_addT(&v, "123456789", 9, char));
	xieq(10, v.len);
	x(!memcmp(v.ptr, "0123456789", 10));

	ffvec_set(&v, "abcdef", 6); // free buffer and set static
	ffvec_shiftT(&v, 1, char);
	x(ffvec_eqT(&v, "bcdef", 5, char));

	*ffvec_pushT(&v, char) = 'g'; // allocate, copy, push
	x(!memcmp(v.ptr, "bcdefg", 6));

	ffvec_free(&v);

	x(NULL != ffvec_growT(&v, 4, char)); // allocate
	xieq(4, v.cap);
	v.len = 3;
	x(NULL != ffvec_growtwiceT(&v, 1, char)); // nothing
	xieq(4, v.cap);
	v.len = 4;
	x(NULL != ffvec_growtwiceT(&v, 1, char)); // grow
	xieq(8, v.cap);
	ffvec_free(&v);

	x(NULL != ffvec_growtwiceT(&v, 6, char)); // allocate
	xieq(6, v.cap);
	ffvec_free(&v);

	x(NULL != ffvec_allocT(&v, 4, char));
	v.len = 4;
	x(NULL != ffvec_growtwiceT(&v, 6, char));
	xieq(10, v.cap);
	ffvec_free(&v);
}
