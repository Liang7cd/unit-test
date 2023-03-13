/*************************************
 *
 * filename:	list.h
 * description:	implement the new list operations
 * 循环双向链表
 * author:
 * date:		2021-11-03
 *
 *
 *************************************/

#ifndef DLIST_H__
#define DLIST_H__

#ifdef __cplusplus 
extern "C"
{
#endif


typedef struct ListD *ListD_T;

typedef struct ListDNode *ListDNode_T;

struct ListD {
	ListDNode_T head;
	int count;
};

struct ListDNode {
	ListDNode_T prev;
	ListDNode_T next;
	void *priv; // user's data
};


/*
 * description: create a new node of list
 * return value: return the pointer to ListDNode_T
 * args: x: user private data pointer
 */
ListDNode_T ListDNode_new(void *x);

/*
 * description: free the node $p
 * return value: return void
 * args: @p: pointer to ListDNode_T
 */
void ListDNode_free(ListDNode_T *p);

/*
 * description: create a new emtpy list
 * return value: return the pointer to list
 * args: void
 */
ListD_T  ListD_new(void);

/*
 * description: free list,
 * return value: void
 * args: the pointer to list
 */
void ListD_free(ListD_T *listp);

/*
 * description: insert @new to @list's tail,
 * return value: @new
 * args: @list: the pointer to list
 * 		@new: new node to insert
 */
ListDNode_T ListD_insert(ListD_T list, ListDNode_T node);

/*
 * description: insert @new before @cur,
 * return value: @new
 * args: @list: the pointer to list
 * 		@cur: current node 
 * 		@new: new node to insert
 */
ListDNode_T ListD_insertBefore(ListD_T list, ListDNode_T cur, ListDNode_T node);

/*
 * description: insert @new after @cur,
 * return value: @new
 * args: @list: the pointer to list
 * 		@cur: current node 
 * 		@new: new node to insert
 */
ListDNode_T ListD_insertAfter(ListD_T list, ListDNode_T cur, ListDNode_T node);

/*
 * description: remove @old from @list,
 * return value: @old
 * args: @list: the pointer to list
 * 		@old: node to remove 
 */
ListDNode_T ListD_remove(ListD_T list, ListDNode_T old);

/*
 * description: return the prev node of @x,
 * return value: node prev @x
 * args: @x: the pointer to node
 */
ListDNode_T ListD_prev(ListDNode_T x);

/*
 * description: return the next node of @x,
 * return value: node next @x
 * args: @x: the pointer to node
 */
ListDNode_T ListD_next(ListDNode_T x);

/*
 * description: return the head node of list ,
 * return value: node of list
 * args: @list: the pointer to list
 */
ListDNode_T ListD_head(ListD_T list);

/*
 * description: return the tail node of list ,
 * return value: node of list
 * args: @list: the pointer to list
 */
ListDNode_T ListD_tail(ListD_T list);

/*
 * 根据index返回链表中相应序号的元素
 * index超过count，返回NULL
 */
ListDNode_T ListD_get(ListD_T list, int index);

/*
 * 根据key在链表中查找元素，找到返回指针，没有返回NULL
 */
ListDNode_T ListD_search(ListD_T list, int (*cmp)(void *, void *), void *key);

/*
 * description: return the count of list ,
 * return value: count of nodes in list >= 0
 * 				-1  error
 * args: @list: the pointer to list
 */
int ListD_count(ListD_T list);

/*
 * description: is list empty,
 * return value: 1: list is empty
 * 				0: list is not empty
 * args: @list: the pointer to list
 */
int ListD_isEmpty(ListD_T list);


#ifdef __cplusplus
}
#endif


#endif	// end of DLIST_H__


