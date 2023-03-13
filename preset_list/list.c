/*************************************
 *
 * 循环双向链表
 *
 *
 *************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"


/*
struct ListD_T {
	ListDNode_T head;
	int count;
};
*/


/*
 * description: create a new node of list
 * return value: return the pointer to ListDNode_T
 * args: x: user private data pointer
 */
ListDNode_T ListDNode_new(void *x)
{
//	assert(x);

	ListDNode_T node = (ListDNode_T )calloc(1, sizeof(*node));
	if (node)
		node->priv = x;

	return node;
}

/*
 * description: free the node $p
 * return value: return void
 * args: @p: pointer to ListDNode_T
 */
void ListDNode_free(ListDNode_T *p)
{
	assert(p);
	assert(*p);

	free(*p);
	*p = NULL;
}

ListD_T  ListD_new(void)
{
	ListD_T list = (ListD_T )calloc(1, sizeof(*list));
	if (list)
	{
		list->head = NULL;
		list->count = 0;
	}

	return list;
}

void ListD_free(ListD_T *list)
{
	ListDNode_T node = NULL;

	assert(list);
	assert(*list);

	while (node = ListD_head(*list))
	{
		ListD_remove(*list, node);
		ListDNode_free(&node);
	}
}

/*
 * description: insert @new to @list's tail,
 * return value: @new
 * args: @list: the pointer to list
 * 		@new: new node to insert
 */
ListDNode_T ListD_insert(ListD_T list, ListDNode_T new)
{
	assert(list);
	assert(new);
	if (NULL == list->head)
	{
		list->head = new;
		new->prev = new;
		new->next = new;
		list->count++;
	}
	else
	{
		ListD_insertBefore(list, list->head, new);
	}

	return new;
}

/*
 * description: insert @new before @cur,
 * return value: @new
 * args: @list: the pointer to list
 * 		@cur: current node 
 * 		@new: new node to insert
 */
ListDNode_T ListD_insertBefore(ListD_T list, ListDNode_T cur, ListDNode_T new)
{
	assert(list);
	assert(cur);
	assert(new);

	new->prev = cur->prev;
	new->next = cur;
	cur->prev->next = new;
	cur->prev = new;

	list->count++;

	return new;
}

/*
 * description: insert @new after @cur,
 * return value: @new
 * args: @list: the pointer to list
 * 		@cur: current node 
 * 		@new: new node to insert
 */
ListDNode_T ListD_insertAfter(ListD_T list, ListDNode_T cur, ListDNode_T new)
{
	assert(list);
	assert(cur);
	assert(new);

	new->prev = cur;
	new->next = cur->next;
	cur->next->prev = new;
	cur->next = new;

	list->count++;

	return new;
}

/*
 * description: remove @old from @list,
 * return value: @old
 * args: @list: the pointer to list
 * 		@old: node to remove 
 */
ListDNode_T ListD_remove(ListD_T list, ListDNode_T old)
{
	assert(list);
	assert(list->count > 0);
	assert(old);

	if (1 == list->count)
	{
		assert(list->head == old);
		list->head = NULL;
	}
	else
	{
		if (list->head == old)
			list->head = old->next;

		old->prev->next = old->next;
		old->next->prev = old->prev;
	}

	old->next = NULL;
	old->prev = NULL;

	list->count--;

	return old;
}

/*
 * description: return the prev node of @x,
 * return value: node prev @x
 * args: @x: the pointer to node
 */
ListDNode_T ListD_prev(ListDNode_T x)
{
	assert(x);

	return x->prev;
}

/*
 * description: return the next node of @x,
 * return value: node next @x
 * args: @x: the pointer to node
 */
ListDNode_T ListD_next(ListDNode_T x)
{
	assert(x);

	return x->next;
}

/*
 * description: return the head node of list ,
 * return value: node of list
 * args: @list: the pointer to list
 */
ListDNode_T ListD_head(ListD_T list)
{
	assert(list);

	return list->head;
}

/*
 * description: return the tail node of list ,
 * return value: node of list
 * args: @list: the pointer to list
 */
ListDNode_T ListD_tail(ListD_T list)
{
	assert(list);

	return list->head ? list->head->prev : NULL;
}

/*
 * 根据index返回链表中相应序号的元素
 * index超过count，返回NULL
 */
ListDNode_T ListD_get(ListD_T list, int index)
{
	assert(list);

	ListDNode_T node = NULL;

	if (index < 0 || index > list->count - 1)
	{
		return NULL;
	}

	node = ListD_head(list);
	for (; index > 0; index--)
	{
		node = ListD_next(node);
	}

	return node;
}

/*
 * 根据key在链表中查找元素，找到返回指针，没有返回NULL
 */
ListDNode_T ListD_search(ListD_T list, int (*cmp)(void *, void *), void *key)
{
	assert(list && cmp);

	int i;
	ListDNode_T node = NULL;

	node = ListD_head(list);
	for (i = 0; i < list->count; i++)
	{
		if (cmp(node->priv, key) == 0)
		{
			break;
		}
		node = ListD_next(node);
	}

	/*
	 * 链表为循环链表
	 */
	if (i < list->count)
	{
		return node;
	}
	else
	{
		return NULL;
	}
}

/*
 * description: return the count of list ,
 * return value: count of nodes in list >= 0
 * 				-1  error
 * args: @list: the pointer to list
 */
int ListD_count(ListD_T list)
{
	assert(list);

	return list->count;
}

/*
 * description: is list empty,
 * return value: 1: list is empty
 * 				0: list is not empty
 * args: @list: the pointer to list
 */
int ListD_isEmpty(ListD_T list)
{
	assert(list);

	return list->count == 0 ? 1 : 0;
}


