/** ffbase/ring.h tester
2022, Simon Zolin
*/

#include <ffbase/ring.h>
#include <ffbase/ringqueue.h>
#include <test/test.h>

void test_ring_io_some(ffring *rb)
{
	ffstr s1;
	ffring_head h, h1, h2;
	ffsize free, used;

	// ""
	h = ffring_read_begin(rb, 6, &s1, &used);
	x(s1.len == 0);
	x(used == 0);

	// "" -> "12345"
	h = ffring_write_begin(rb, 5, &s1, &free);
	x(s1.len == 5);
	x(free == 3);
	ffmem_copy(s1.ptr, "12345", 5);
	ffring_write_finish(rb, h, &used);
	x(used == 0);

	// "12345" -> "123456" -> "1234567"
	h1 = ffring_write_begin(rb, 1, &s1, &free);
	x(s1.len == 1);
	x(free == 2);
	ffmem_copy(s1.ptr, "6", 1);
	h2 = ffring_write_begin(rb, 1, &s1, &free);
	x(s1.len == 1);
	x(free == 1);
	ffmem_copy(s1.ptr, "7", 1);
	ffring_write_finish(rb, h1, NULL);
	ffring_write_finish(rb, h2, NULL);

	// "1234567" -> "...4567" -> ".....67"
	h1 = ffring_read_begin(rb, 3, &s1, &used);
	xseq(&s1, "123");
	x(used == 4);
	h2 = ffring_read_begin(rb, 2, &s1, &used);
	xseq(&s1, "45");
	x(used == 2);
	ffring_read_finish(rb, h1);
	ffring_read_finish(rb, h2);

	// ".....67" -> ".....678" -> "90...678"
	h = ffring_write_begin(rb, 3, &s1, &free);
	x(s1.len == 1);
	x(free == 5);
	ffmem_copy(s1.ptr, "8", 1);
	ffring_write_finish(rb, h, &used);
	x(used == 2);
	h = ffring_write_begin(rb, 2, &s1, &free);
	x(s1.len == 2);
	x(free == 3);
	ffmem_copy(s1.ptr, "90", 2);
	ffring_write_finish(rb, h, &used);
	x(used == 3);

	// "90...678" -> "90" -> ".."
	h = ffring_read_begin(rb, 5, &s1, &used);
	xseq(&s1, "678");
	x(used == 2);
	ffring_read_finish(rb, h);
	h = ffring_read_begin(rb, 5, &s1, &used);
	xseq(&s1, "90");
	x(used == 0);
	ffring_read_finish(rb, h);

	// ".." -> "..xxxxxx" -> "xxxxxxxx"
	h1 = ffring_write_begin(rb, 9, &s1, &free);
	x(s1.len == 6);
	x(free == 2);
	h2 = ffring_write_begin(rb, 8, &s1, &free);
	x(s1.len == 2);
	x(free == 0);
	ffring_write_finish(rb, h1, NULL);
	ffring_write_finish(rb, h2, NULL);

	// "xxxxxxxx"
	h = ffring_write_begin(rb, 1, &s1, &free);
	x(s1.len == 0);
	x(free == 0);

	ffring_reset(rb);

	// "" -> "12345" -> "12345678"
	x(5 == ffring_write(rb, "12345", 5));
	x(3 == ffring_write(rb, "6789", 4));
	x(0 == ffring_write(rb, "9", 1));

	// "12345678" -> ""
	h = ffring_read_begin(rb, 8, &s1, &used);
	xseq(&s1, "12345678");
	x(used == 0);
}

void test_ring_io_all(ffring *rb)
{
	ffstr s1, s2;
	ffring_head h, h1, h2;
	ffsize free, used;

	// ""
	h = ffring_read_all_begin(rb, 6, &s1, &s2, &used);
	x(s1.len == 0);
	x(s2.len == 0);
	x(used == 0);

	// "" -> "12345"
	h = ffring_write_all_begin(rb, 5, &s1, &s2, &free);
	x(s1.len == 5);
	x(s2.len == 0);
	x(free == 3);
	ffmem_copy(s1.ptr, "12345", 5);
	ffring_write_finish(rb, h, &used);
	x(used == 0);

	// "12345" -> "123456" -> "1234567"
	h1 = ffring_write_all_begin(rb, 1, &s1, &s2, &free);
	x(s1.len == 1);
	x(s2.len == 0);
	x(free == 2);
	ffmem_copy(s1.ptr, "6", 1);
	h2 = ffring_write_all_begin(rb, 1, &s1, &s2, &free);
	x(s1.len == 1);
	x(s2.len == 0);
	x(free == 1);
	ffmem_copy(s1.ptr, "7", 1);
	ffring_write_finish(rb, h1, NULL);
	ffring_write_finish(rb, h2, NULL);

	// "1234567" -> "...4567" -> ".....67"
	h1 = ffring_read_all_begin(rb, 3, &s1, &s2, &used);
	xseq(&s1, "123");
	x(s2.len == 0);
	x(used == 4);
	h2 = ffring_read_all_begin(rb, 2, &s1, &s2, &used);
	xseq(&s1, "45");
	x(s2.len == 0);
	x(used == 2);
	ffring_read_finish(rb, h1);
	ffring_read_finish(rb, h2);

	// ".....67" -> "90...678"
	h = ffring_write_all_begin(rb, 3, &s1, &s2, &free);
	x(s1.len == 1);
	x(s2.len == 2);
	x(free == 3);
	ffmem_copy(s1.ptr, "8", 1);
	ffmem_copy(s2.ptr, "90", 2);
	ffring_write_finish(rb, h, &used);
	x(used == 2);

	// "90...678"
	h = ffring_read_all_begin(rb, 6, &s1, &s2, &used);
	x(s1.len == 0);
	x(s2.len == 0);
	x(used == 5);

	// "90...678" -> ".."
	h = ffring_read_all_begin(rb, 5, &s1, &s2, &used);
	xseq(&s1, "678");
	xseq(&s2, "90");
	x(used == 0);
	ffring_read_finish(rb, h);

	// ".."
	h = ffring_write_all_begin(rb, 9, &s1, &s2, &free);
	x(s1.len == 0);
	x(s2.len == 0);
	x(free == 8);

	// ".." -> "xxxxxxxx"
	h = ffring_write_all_begin(rb, 8, &s1, &s2, &free);
	x(s1.len == 6);
	x(s2.len == 2);
	x(free == 0);
	ffring_write_finish(rb, h, &used);
	x(used == 0);

	// "xxxxxxxx"
	h = ffring_write_all_begin(rb, 1, &s1, &s2, &free);
	x(s1.len == 0);
	x(s2.len == 0);
	x(free == 0);

	ffring_reset(rb);

	// "" -> "12345" -> "12345678"
	x(5 == ffring_write_all(rb, "12345", 5));
	x(0 == ffring_write_all(rb, "asdf", 4));
	x(3 == ffring_write_all(rb, "678", 3));
	x(0 == ffring_write_all(rb, "9", 1));

	// "12345678" -> ""
	h = ffring_read_all_begin(rb, 8, &s1, &s2, &used);
	xseq(&s1, "12345678");
	x(s2.len == 0);
	x(used == 0);
}

void test_rq()
{
	ffringqueue *q = ffrq_alloc(10);
	void *ptr;
	ffuint used;

	for (ffsize i = 0;  i < 16;  i++) {
		x(!ffrq_add(q, (void*)i, &used));
		x(used == i);
	}
	x(!!ffrq_add(q, (void*)0x1, &used)); // full

	for (ffsize i = 0;  i < 16;  i++) {
		x(!ffrq_fetch(q, &ptr, &used));
		x(used == 16 - i);
	}

	x(!!ffrq_fetch(q, &ptr, &used)); // empty

	ffrq_free(q);
}

void test_ring()
{
	ffring *rb = ffring_alloc(7, FFRING_1_READER | FFRING_1_WRITER);

	test_ring_io_all(rb);
	ffring_reset(rb);
	test_ring_io_some(rb);

	rb->flags = FFRING_1_WRITER;
	ffring_reset(rb);
	test_ring_io_all(rb);
	ffring_reset(rb);
	test_ring_io_some(rb);

	rb->flags = FFRING_1_READER;
	ffring_reset(rb);
	test_ring_io_all(rb);
	ffring_reset(rb);
	test_ring_io_some(rb);

	rb->flags = 0;
	ffring_reset(rb);
	test_ring_io_all(rb);
	ffring_reset(rb);
	test_ring_io_some(rb);

	ffring_free(rb);

	test_rq();
}
