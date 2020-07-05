/** ffbase/atomic.h tester
2020, Simon Zolin
*/

#include <ffbase/atomic.h>
#include <test/test.h>


void test_atomic()
{
	ffatomic a;
	ffatomic_store(&a, 1);
	xieq(1, ffatomic_load(&a));
	xieq(1, ffatomic_fetch_add(&a, 2));
	xieq(3, ffatomic_load(&a));
	xieq(3, ffatomic_cmpxchg(&a, 2, 5));
	xieq(3, ffatomic_cmpxchg(&a, 3, 5));
	xieq(5, ffatomic_load(&a));
}
