
#ifndef __LIST_H__
#define __LIST_H__

#include <stdlib.h>

struct _X_ListNode;

typedef struct _X_ListNode
{
    struct _X_ListNode *Prev;
    struct _X_ListNode *Next;
    void	       *Data;
} X_ListNode;

typedef struct _X_ListNode* X_ListNodePtr;

typedef void (*X_ListMemFree_t)(void*);

typedef void* (*X_ListCopy_t)(const void*);

typedef int (*X_ListCompare_t)(const void*, const void*);

struct _X_List;

typedef struct _X_List
{
	X_ListNodePtr           Head;
	X_ListNodePtr	        Tail;
	X_ListMemFree_t         Free;
	unsigned int            Size;
} X_List;

typedef struct _X_List* X_ListPtr;

/**
 * Create new list.
 */
X_ListPtr
X_List_New(X_ListMemFree_t);

X_ListPtr
X_List_Init(X_ListPtr, X_ListMemFree_t);

/**
 * Make a copy of the list. Copy user data using pointer to a copy function.
 */
X_ListPtr
X_List_Copy(const X_ListPtr, X_ListCopy_t);

void
X_List_Free(X_ListPtr);

/*
 * Insert element to a list after the 'next'. If next == NULL then element is
 * prepended.
 */
int
X_List_Insert(X_ListPtr, X_ListNodePtr next, void *data);

/**
 * Destroy a node and the user data.
 * @return Next node
 */
X_ListNodePtr
X_List_Destroy(X_ListPtr, X_ListNodePtr);

/**
 * Remove node, but do not destroy user data.
 */
int
X_List_Remove(X_ListPtr, X_ListNodePtr, void **);

/**
 * Destroy all nodes.
 */
void
X_List_Clear(X_ListPtr);

/**
 * Destroy list (and nodes).
 */
X_ListNodePtr
X_List_Head(X_ListPtr);

X_ListNodePtr
X_List_Tail(X_ListPtr);

X_ListNodePtr
X_List_Find(X_ListPtr,const void*,X_ListCompare_t);

#define X_List_Prepend(list, data) \
    X_List_Insert(list, X_List_Head(list), data)

#define X_List_Append(list, data) \
    X_List_Insert(list, NULL, data)

#define X_List_Empty(list) \
    (list->Size == 0)

#define X_List_Next(node) \
    ((node == NULL) ? NULL : (node->Next))

#define X_List_Prev(node) \
    ((node == NULL) ? NULL : (node->Prev))

#define X_List_Size(list) \
    ((list != NULL) ? list->Size : 0)

#define X_List_ForEach(list, node) \
    for(node = X_List_Head(list); node; node = X_List_Next(node))

#define X_List_RForEach(list, node) \
    for(node = X_List_Tail(list); node; node = X_List_Prev(node))

#endif
