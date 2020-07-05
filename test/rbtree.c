/** ffbase/rbtree.h tester
2020, Simon Zolin
*/

#include <ffbase/base.h>
#include <ffbase/rbtree.h>
#include <test/test.h>


void test_rbtree()
{
	ffrbtree tr;
	enum { NUM = 1000, OFF = 100 };
	ffuint i;
	int n;
	ffrbt_node *ar;
	ffrbt_node *nod;

	ffrbt_init(&tr);
	ar = (ffrbt_node*)ffmem_zalloc(NUM * sizeof(ffrbt_node));
	x(ar != NULL);

// insert
	n = 0;
	for (ffuint i = OFF; i < NUM; i++) {
		ar[n].key = i;
		ffrbt_insert(&tr, &ar[n++], NULL);
	}
	for (ffuint i = 0; i < OFF; i++) {
		ar[n].key = i;
		ffrbt_insert(&tr, &ar[n++], NULL);
	}

	x(tr.len == NUM);

	x(0 == ffrbt_first(&tr)->key);

// walk
	i = 0;
	FFRBT_WALK(&tr, nod) {
		x(i++ == nod->key);
	}
	x(i == NUM);

// find
	{
		ffrbt_node *found;

		found = ffrbt_find(&tr, NUM / 3);
		x(found->key == NUM / 3);

		x(NULL == ffrbt_find(&tr, NUM));

		found = ffrbt_node_locate(NUM, tr.root, &tr.sentl);
		x(found->key == NUM - 1);
	}

// remove
	for (ffuint i = (NUM / 2);  i != (NUM / 2 + OFF);  i++) {
		ffrbt_rm(&tr, &ar[i]);
	}

// walk & remove
	ffrbt_node *nod2, *next;
	FFRBT_FOR(&tr, nod2) {
		next = ffrbt_node_successor(nod2, &tr.sentl);
		ffrbt_rm(&tr, (void*)nod2);
		nod2 = next;
	}
	x(tr.len == 0);

	ffmem_free(ar);
}
