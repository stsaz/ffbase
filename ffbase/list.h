/** ffbase: doubly-linked list
2020, Simon Zolin
*/

#pragma once

#ifndef _FFBASE_BASE_H
#include <ffbase/base.h>
#endif
#include <ffbase/chain.h>


/** Container - root list item used as a sentinel */
typedef struct fflist {
	ffchain_item root;
	ffsize len;
} fflist;

/** Initialize container before use */
static inline void fflist_init(fflist *list)
{
	list->root.next = list->root.prev = &list->root;
	list->len = 0;
}

#define fflist_first(list)  ((list)->root.next)
#define fflist_last(list)  ((list)->root.prev)

/** Return TRUE if list is empty */
#define fflist_empty(list)  ((list)->root.next == &(list)->root)

/** Add item to the end */
static inline void fflist_add(fflist *list, ffchain_item *item)
{
	ffchain_item_append(item, list->root.prev);
	list->len++;
}

/** Add item to the beginning */
static inline void fflist_addfront(fflist *list, ffchain_item *item)
{
	ffchain_item_prepend(item, list->root.next);
	list->len++;
}

/** Remove item */
static inline void fflist_rm(fflist *list, ffchain_item *item)
{
	FF_ASSERT(list->len != 0);
	ffchain_item_unlink(item);
	list->len--;
}

/** Move the item to the beginning */
static inline void fflist_movefront(fflist *list, ffchain_item *item)
{
	ffchain_item_unlink(item);
	ffchain_item_prepend(item, list->root.next);
}

/** Move the item to the end */
static inline void fflist_moveback(fflist *list, ffchain_item *item)
{
	ffchain_item_unlink(item);
	ffchain_item_append(item, list->root.prev);
}

#define FFLIST_WALK(list, it)  FFCHAIN_WALK(&(list)->root, it)
#define FFLIST_RWALK(list, it)  FFCHAIN_RWALK(&(list)->root, it)
