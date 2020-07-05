/** ffbase/lock.h tester
2020, Simon Zolin
*/

#include <ffbase/lock.h>
#include <test/test.h>


void test_lock()
{
	fflock l = {};
	fflock_init(&l);
	x(fflock_trylock(&l));
	x(!fflock_trylock(&l));
	fflock_unlock(&l);
	fflock_lock(&l);
	fflock_unlock(&l);
}
