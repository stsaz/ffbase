/** ffbase: list.h & chain.h tester
2020, Simon Zolin
*/

#include <ffbase/base.h>
#include <ffbase/list.h>
#include <test/test.h>


void test_list()
{
	fflist ls;
	ffchain_item i1, i2, i3;
	ffchain_item *it;
	int n;

	fflist_init(&ls);
	x(fflist_empty(&ls));
	fflist_add(&ls, &i1);
	x(!fflist_empty(&ls));
	fflist_rm(&ls, &i1);
	x(fflist_empty(&ls));

	fflist_add(&ls, &i1);
	x(!fflist_empty(&ls));
	// sentl <-> i1 <->

	ffchain_item_append(&i2, &i1);
	// sentl <-> i1 <-> i2 <->

	ffchain_item_prepend(&i3, &i1);
	// sentl <-> i3 <-> i1 <-> i2 <->

	fflist_movefront(&ls, &i1);
	// sentl <-> i1 <-> i3 <-> i2 <->

	fflist_moveback(&ls, &i3);
	// sentl <-> i1 <-> i2 <-> i3 <->

	n = 0;
	FFLIST_WALK(&ls, it) {
		switch (n++) {
		case 0:
			x(it == &i1); break;
		case 1:
			x(it == &i2); break;
		case 2:
			x(it == &i3); break;
		case 3:
			x(0);
		}
	}

	n = 0;
	FFLIST_RWALK(&ls, it) {
		switch (n++) {
		case 0:
			x(it == &i3); break;
		case 1:
			x(it == &i2); break;
		case 2:
			x(it == &i1); break;
		case 3:
			x(0);
		}
	}

	ffstr s = fflist_print(&ls);
	ff_printf("%.*s\n", (int)s.len, s.ptr);
	ffstr_free(&s);
}
