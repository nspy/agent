
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <SnmpAccess/List.h>
#include <SnmpAccess/Threads.h>
#include <time.h>
#include <sys/time.h>

/** Monitored queue **/
typedef struct _X_Queue
{
    X_ListPtr           Queue;
    X_MutexPtr          Mutex;
    X_CondPtr           Cond;
} X_Queue;

typedef struct _X_Queue* X_QueuePtr;

/** Create a new Queue object **/
X_QueuePtr
X_Queue_New(X_ListMemFree_t);

X_QueuePtr
X_Queue_Init(X_QueuePtr, X_ListMemFree_t);

void
X_Queue_Deinit(X_QueuePtr);

/** Destroy queue **/
void
X_Queue_Free(X_QueuePtr);

/**
 * Put an item into the queue
 * @return 0 on success
 */
int
X_Queue_Put(X_QueuePtr,void*);

/**
 * Retrieve item from the queue
 * @return 0 on success
 */
int
X_Queue_Get(X_QueuePtr, const struct timespec*, void**);

#endif
