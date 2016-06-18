
#include "Queue.h"
#include <assert.h>

X_QueuePtr
X_Queue_New(X_ListMemFree_t cb)
{
    X_Queue *q = (X_Queue*)malloc(sizeof(X_Queue));
    if (q != NULL)
    {
        X_Queue_Init(q, cb);
    }
    return q;
}

X_QueuePtr
X_Queue_Init(X_QueuePtr pQueue, X_ListMemFree_t Cb)
{
    if (pQueue != NULL)
    {
        pQueue->Queue = X_List_New(Cb);
        pQueue->Mutex = X_Mutex_New();
        pQueue->Cond = X_Cond_New();
    }
    return pQueue;
}

void
X_Queue_Deinit(X_QueuePtr pQueue)
{
    if (pQueue != NULL)
    {
        X_Cond_Free(pQueue->Cond);
        X_Mutex_Free(pQueue->Mutex);
        X_List_Free(pQueue->Queue);
    }
}

void
X_Queue_Free(X_QueuePtr q)
{
    if (q != NULL)
    {
        X_Queue_Deinit(q);
        free(q);
    }
}

int
X_Queue_Put(X_QueuePtr q, void *data)
{
    if (q != NULL)
    {
        X_Mutex_Lock(q->Mutex);
        X_List_Append(q->Queue, data);
        X_Mutex_Unlock(q->Mutex);
        X_Cond_Signal(q->Cond);
    }
    return (0);
}

int
X_Queue_Get(X_QueuePtr q, const struct timespec *abstime, void **data)
{
    int ret;

    if (q == NULL)
    {
        return (-1);
    }

    X_Mutex_Lock(q->Mutex);

    while (X_List_Empty(q->Queue))
    {
        ret = X_Cond_TimedWait(q->Cond, q->Mutex, abstime);

        if (ret != 0)
        {
            break;
        }

    }

    if(!X_List_Empty(q->Queue))
    {
        ret = X_List_Remove(q->Queue, q->Queue->Head, data);
    }

    X_Mutex_Unlock(q->Mutex);

    return ret;
}

void
GetTime(struct timespec *ts)
{
    assert(ts != NULL);
    clock_gettime(CLOCK_REALTIME, ts);
}

void
AddTime(struct timespec *ts, long ms)
{
    assert(ts != NULL);
    ts->tv_sec   += (ms / 1000);
    ts->tv_nsec  += (ms - (ms / 1000) * 1000) * 1000000;
    if (ts->tv_nsec > 1000000000)
    {
        ts->tv_sec += 1;
        ts->tv_nsec -= 1000000000;
    }
}
