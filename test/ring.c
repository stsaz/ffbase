/** ffbase/ringf.h tester
2020, Simon Zolin
*/

#include <ffbase/ring.h>
#include <test/test.h>


void test_ring()
{
	ffring r = {};
	x(NULL != ffring_create(&r, 8));

	xieq(0, ffring_filled(&r));
	xieq(4, ffring_write(&r, "data", 4));
	xieq(4, ffring_filled(&r));
	xieq(4, ffring_write(&r, "12345", 5));
	xieq(8, ffring_filled(&r));

	ffstr s = ffring_acquire_read(&r);
	xieq(8, s.len);
	s.len = 4;
	xseq(&s, "data");
	ffring_release_read(&r, 4);

	s = ffring_acquire_read(&r);
	xieq(4, s.len);
	xseq(&s, "1234");
	ffring_release_read(&r, 3);

	ffring_clear(&r);
	xieq(0, ffring_filled(&r));

	// "(rw)........"
	s = ffring_acquire_read(&r);
	xseq(&s, "");
	xieq(7, ffring_write(&r, "1234567", 7)); // "(r)1234567(w)."
	s = ffring_acquire_read(&r);
	xseq(&s, "1234567");
	ffring_release_read(&r, 2); // "..(r)34567(w)."
	xieq(1, ffring_write(&r, "abcdef", 5)); // "(w)..(r)34567a"
	xieq(2, ffring_write(&r, "bcdef", 4)); // "bc(rw)34567a"
	s = ffring_acquire_read(&r);
	xieq(6, s.len);
	xseq(&s, "34567a");
	ffring_release_read(&r, s.len); // "(r)bc(rw)......"
	s = ffring_acquire_read(&r);
	xseq(&s, "bc");
	ffring_release_read(&r, 2);

	ffring_free(&r);
}
