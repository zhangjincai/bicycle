#ifndef __LIB_LIST_H__
#define __LIB_LIST_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/

#include <stddef.h>

#define container_of(ptr, type, member) ({	\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

#if 0

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("lib__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

struct list_head 
{
	struct list_head *next, *prev;
};
typedef struct list_head lib_list_head_t;


/**
 * lib_list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define lib_list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * lib_list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define lib_list_first_entry(ptr, type, member) \
	lib_list_entry((ptr)->next, type, member)

/**
 * lib_list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define lib_list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * __list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 *
 * This variant doesn't differ from list_for_each() any more.
 * We don't do prefetching in either case.
 */
#define lib_list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * lib_list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define lib_list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * lib_list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define lib_list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * lib_list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define lib_list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     pos != (head); \
	     pos = n, n = pos->prev)

/**
 * lib_list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define lib_list_for_each_entry(pos, head, member)				\
	for (pos = lib_list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = lib_list_entry(pos->member.next, typeof(*pos), member))

/**
 * lib_list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define lib_list_for_each_entry_reverse(pos, head, member)			\
	for (pos = lib_list_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = lib_list_entry(pos->member.prev, typeof(*pos), member))

/**
 * lib_list_prepare_entry - prepare a pos entry for use in list_for_each_entry_continue()
 * @pos:	the type * to use as a start point
 * @head:	the head of the list
 * @member:	the name of the list_struct within the struct.
 *
 * Prepares a pos entry for use as a start point in list_for_each_entry_continue().
 */
#define lib_list_prepare_entry(pos, head, member) \
	((pos) ? : lib_list_entry(head, typeof(*pos), member))

/**
 * lib_list_for_each_entry_continue - continue iteration over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define lib_list_for_each_entry_continue(pos, head, member) 		\
	for (pos = lib_list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head);	\
	     pos = lib_list_entry(pos->member.next, typeof(*pos), member))

/**
 * lib_list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define lib_list_for_each_entry_continue_reverse(pos, head, member)		\
	for (pos = lib_list_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head);	\
	     pos = lib_list_entry(pos->member.prev, typeof(*pos), member))

/**
 * lib_list_for_each_entry_from - iterate over list of given type from the current point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define lib_list_for_each_entry_from(pos, head, member) 			\
	for (; &pos->member != (head);	\
	     pos = lib_list_entry(pos->member.next, typeof(*pos), member))

/**
 * lib_list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define lib_list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = lib_list_entry((head)->next, typeof(*pos), member),	\
		n = lib_list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = lib_list_entry(n->member.next, typeof(*n), member))

/**
 * lib_list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define lib_list_for_each_entry_safe_continue(pos, n, head, member) 		\
	for (pos = lib_list_entry(pos->member.next, typeof(*pos), member), 		\
		n = lib_list_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = lib_list_entry(n->member.next, typeof(*n), member))

/**
 * lib_list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define lib_list_for_each_entry_safe_from(pos, n, head, member) 			\
	for (n = lib_list_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = lib_list_entry(n->member.next, typeof(*n), member))

/**
 * lib_list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define lib_list_for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = lib_list_entry((head)->prev, typeof(*pos), member),	\
		n = lib_list_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = lib_list_entry(n->member.prev, typeof(*n), member))

/**
 * lib_list_safe_reset_next - reset a stale list_for_each_entry_safe loop
 * @pos:	the loop cursor used in the list_for_each_entry_safe loop
 * @n:		temporary storage used in list_for_each_entry_safe
 * @member:	the name of the list_struct within the struct.
 *
 * list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define lib_list_safe_reset_next(pos, n, member)				\
	n = lib_list_entry(pos->member.next, typeof(*pos), member)



#define LIB_LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIB_LIST_HEAD(name) \
	struct list_head name = LIB_LIST_HEAD_INIT(name)

void lib_init_list_head(lib_list_head_t *list);
void lib_list_add(lib_list_head_t *new, lib_list_head_t *head);
void lib_list_add_tail(lib_list_head_t *new, lib_list_head_t *head);
void lib_list_del(lib_list_head_t *entry);
void lib_list_replace(lib_list_head_t *old, lib_list_head_t *new);
void lib_list_replace_init(lib_list_head_t *old, lib_list_head_t *new);
void lib_list_move(lib_list_head_t *list, lib_list_head_t *head);
void lib_list_move_tail(lib_list_head_t *list, lib_list_head_t *head);
int lib_list_is_last(const lib_list_head_t *list, const lib_list_head_t *head);
int lib_list_empty(const lib_list_head_t *head);
int lib_list_empty_careful(const lib_list_head_t *head);
void lib_list_rotate_left(lib_list_head_t *head);
int lib_list_is_singular(const lib_list_head_t *head);
void lib_list_cut_position(lib_list_head_t *list, lib_list_head_t *head, lib_list_head_t *entry);
void lib_list_splice(const lib_list_head_t *list, lib_list_head_t *head);
void lib_list_splice_tail(lib_list_head_t *list, lib_list_head_t *head);
void lib_list_splice_init(lib_list_head_t *list,  lib_list_head_t *head);
void lib_list_splice_tail_init(lib_list_head_t *list, lib_list_head_t *head);
#endif


/*
 * Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is too wasteful.
 * You lose the ability to access the tail in O(1).
 */
typedef struct hlist_node
{
	struct hlist_node *next, **pprev;	
}lib_hlist_node_t;

typedef struct hlist_head
{
	struct hlist_node *first;	
}lib_hlist_head_t;


#define lib_hlist_entry(ptr, type, member) 			container_of(ptr,type,member)

#define lib_hlist_for_each(pos, head)  \
		for(pos = (head)->first; pos ; pos = pos->next)
			
#define lib_hlist_for_each_safe(pos, n, head) \
	for(pos = (head)->first; pos && ({ n = pos->next; 1; }); \
		pos = n)

/**
 * lib_hlist_for_each_entry	- iterate over list of given type
 * @tpos:	the type * to use as a loop cursor.
 * @pos:	the &struct hlist_node to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define lib_hlist_for_each_entry(tpos, pos, head, member)	\
	for (pos = (head)->first;			\
		pos &&					\
		({ tpos = lib_hlist_entry(pos, typeof(*tpos), member); 1;}); \
		pos = pos->next)

/**
 * lib_hlist_for_each_entry_continue - iterate over a hlist continuing after current point
 * @tpos:	the type * to use as a loop cursor.
 * @pos:	the &struct hlist_node to use as a loop cursor.
 * @member:	the name of the hlist_node within the struct.
 */
#define lib_hlist_for_each_entry_continue(tpos, pos, member)		 \
	for (pos = (pos)->next;						 \
	     pos &&							 \
		({ tpos = lib_hlist_entry(pos, typeof(*tpos), member); 1;}); \
	     pos = pos->next)

/**
 * lib_hlist_for_each_entry_from - iterate over a hlist continuing from current point
 * @tpos:	the type * to use as a loop cursor.
 * @pos:	the &struct hlist_node to use as a loop cursor.
 * @member:	the name of the hlist_node within the struct.
 */
#define lib_hlist_for_each_entry_from(tpos, pos, member)			 \
	for (; pos &&							 \
		({ tpos = lib_hlist_entry(pos, typeof(*tpos), member); 1;}); \
	     pos = pos->next)

/**
 * lib_hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @tpos:	the type * to use as a loop cursor.
 * @pos:	the &struct hlist_node to use as a loop cursor.
 * @n:		another &struct hlist_node to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define lib_hlist_for_each_entry_safe(tpos, pos, n, head, member) 		 \
	for (pos = (head)->first;					 \
	     pos && ({ n = pos->next; 1; }) && 				 \
		({ tpos = lib_hlist_entry(pos, typeof(*tpos), member); 1;}); \
	     pos = n)	
	


#define GOLDEN_RATIO_PRIME_32 		0x9e370001UL  
#define HASH_SHIFT_32				(11)  //HASH_SHIFT_32为散列值最大数,2的11次方2048
static inline unsigned int hash_32(unsigned int val, unsigned int bits)  
{
	unsigned int hash = val * GOLDEN_RATIO_PRIME_32; 
	return hash >> (32 - bits); 
}

#define hash_long(val, bits) 			hash_32(val, bits)  
#define lib_hashfn(nr, ns)				hash_long((unsigned long)nr + (unsigned long)ns, HASH_SHIFT_32)   //散列函数
#define lib_hashfn_shift(nr, ns, shift)	hash_long((unsigned long)nr + (unsigned long)ns, shift) 

#define LIB_HLIST_HEAD_INIT 			{ .first = NULL }
#define LIB_HLIST_HEAD(name) 		struct hlist_head name = {  .first = NULL }
#define LIB_INIT_HLIST_HEAD(ptr) 	((ptr)->first = NULL)

void lib_hlist_node_init(lib_hlist_node_t *h);
void lib_hlist_del(lib_hlist_node_t *n);
int lib_hlist_empty(lib_hlist_head_t *h);
int lib_hlist_unhashed(lib_hlist_node_t *h);
void lib_hlist_del_init(lib_hlist_node_t *n);
void lib_hlist_add_head(lib_hlist_node_t *n, lib_hlist_head_t *h);
/* 
 * next must be != NULL 
 */
void lib_hlist_add_before(lib_hlist_node_t *n, lib_hlist_node_t *next);
void lib_hlist_add_after(lib_hlist_node_t *n, lib_hlist_node_t *next);
void lib_hlist_add_behind(lib_hlist_node_t *n, lib_hlist_node_t *prev);
/* 
 * after that we'll appear to be on some hlist and hlist_del will work 
 */
void lib_hlist_add_fake(lib_hlist_node_t *n);
/*
 * Move a list from one list head to another. Fixup the pprev
 * reference of the first entry if it exists.
 */
void lib_hlist_move_list(lib_hlist_head_t *old, lib_hlist_head_t *new);



/*@*/
#ifdef __cplusplus
}
#endif
#endif


