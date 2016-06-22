
#include "Threads.h"
#include <assert.h>

#if 1
#define XDBG(a, b...)    fprintf(stderr,"DBG: "a"\n", ##b)
#else
#define XDBG(a, b...)
#endif

typedef struct _X_Thread
{
    X_ThreadHandle_t      Handle;
    int                   Started;
    int                   Detached;
    X_ThreadProc_t        Proc;
} X_Thread;

typedef struct _X_Mutex
{
    X_MutexHandle_t     Handle;
    int                 Initialized;
} X_Mutex;

typedef struct _X_Cond
{
    X_CondHandle_t      Handle;
    int                 Initialized;
} X_Cond;


X_Thread*
X_Thread_New(void)
{
    X_Thread *pThread = (X_Thread*)malloc(sizeof(X_Thread));

    if (pThread == NULL)
    {
        XDBG("X_Thread_New(): invalid pointer");
        return NULL;
    }

    X_Thread_Init(pThread);

    return pThread;
}

void
X_Thread_Free(X_Thread *pThread)
{
    if (pThread == NULL)
    {
        XDBG("X_Thread_Free(): invalid pointer");
        return;
    }
    free(pThread);
}

int
X_Thread_Init(X_Thread *pThread)
{
    if (pThread == NULL)
    {
        XDBG("X_Thread_Init(): invalid pointer");
        return -1;
    }
    memset(pThread, sizeof(X_Thread), 0);
    pThread->Started = 0;
    pThread->Detached = 0;
    return 0;
}

void
X_Thread_Deinit(X_Thread *pThread)
{
    if (pThread == NULL)
    {
        return;
    }
}

int
X_Thread_Start(X_Thread *pThread, X_ThreadProc_t proc, void *arg, int detach)
{
    int ret;

    if (pThread == NULL)
    {
        XDBG("X_Thread_Start(): invalid pointer");
        return -1;
    }

    if(pThread->Started != 0)
    {
        XDBG("X_Thread_Start(): thread already started");
        return -1;
    }

    pThread->Proc = proc;

    ret = pthread_create(&pThread->Handle, NULL, (void* (*)(void*))pThread->Proc, arg);

    if (ret != 0)
    {
        XDBG("X_Thread_Start(): error occured while creating a thread");
        return ret;
    }

    pThread->Started = 1;

    if (detach)
    {
        ret = pthread_detach(pThread->Handle);

        if (ret != 0)
        {
            XDBG("X_Thread_Start(): error occured while detaching thread");
        }

        pThread->Detached = (ret == 0) ? 1 : 0;

    }

    return ret;
}

void
X_Thread_Join(X_Thread *pThread)
{
    if (pThread == NULL || !pThread->Started)
    {
        return;
    }

    if (0 != pThread->Detached)
    {
        return;
    }

    pthread_join(pThread->Handle, NULL);
}


X_MutexPtr
X_Mutex_New(void)
{
    X_MutexPtr pMutex = (X_MutexPtr)malloc(sizeof(X_Mutex));

    if (pMutex == NULL)
    {
        return NULL;
    }

    if (X_Mutex_Init(pMutex) != 0)
    {
        XDBG("X_Mutex_New(): failed to initialize mutex");
        free(pMutex);
        return NULL;
    }

    return pMutex;
}

void
X_Mutex_Free(X_MutexPtr pMutex)
{
    if (pMutex != NULL)
    {
        X_Mutex_Deinit(pMutex);
        free(pMutex);
    }
}

int
X_Mutex_Init(X_MutexPtr pMutex)
{
    int ret;

    if (pMutex == NULL)
    {
        return (-1);
    }

    ret = pthread_mutex_init(&pMutex->Handle, NULL);

    if (ret == 0)
    {
        pMutex->Initialized = 1;
    }

    return ret;
}

void
X_Mutex_Deinit(X_MutexPtr pMutex)
{
    if (pMutex != NULL)
    {
        pthread_mutex_destroy(&pMutex->Handle);
        pMutex->Initialized = 0;
    }
}

int
X_Mutex_Lock(X_MutexPtr pMutex)
{
    if (pMutex == NULL)
    {
        XDBG("X_Mutex_Lock(): invalid pointer");
        return (-1);
    }

    if (0 == pMutex->Initialized)
    {
        XDBG("X_Mutex_Lock(): not initialized");
        return (-1);
    }

    return (pthread_mutex_lock(&pMutex->Handle));
}

int
X_Mutex_Unlock(X_MutexPtr pMutex)
{
    if (pMutex == NULL)
    {
        XDBG("X_Mutex_Unlock(): invalid pointer");
        return (-1);
    }

    if (0 == pMutex->Initialized)
    {
        XDBG("X_Mutex_Unlock(): not initialized");
        return (-1);
    }

    return (pthread_mutex_unlock(&pMutex->Handle));
}

X_CondPtr
X_Cond_New(void)
{
    X_CondPtr pCond = (X_CondPtr)malloc(sizeof(X_Cond));

    if (pCond == NULL)
    {
        XDBG("X_Cond_New(): invalid pointer");
        return NULL;
    }

    if (0 != X_Cond_Init(pCond))
    {
        XDBG("X_Cond_New(): failed to initiazlie conditon var.");
        free(pCond);
        return NULL;
    }

    return pCond;
}

int
X_Cond_Init(X_CondPtr pCond)
{
    if (pCond == NULL)
    {
        return -1;
    }
    return (pthread_cond_init(&pCond->Handle, NULL));
}

void
X_Cond_Deinit(X_CondPtr pCond)
{
    if (pCond != NULL)
    {
        pthread_cond_destroy(&pCond->Handle);
    }
}

void
X_Cond_Free(X_CondPtr pCond)
{
    if (pCond != NULL)
    {
        X_Cond_Deinit(pCond);
        free(pCond);
    }
}

int
X_Cond_Wait(X_CondPtr pCond, X_MutexPtr pMutex)
{
    if (pCond == NULL || pMutex == NULL)
    {
        XDBG("X_Cond_Wait(): invalid pointer");
        return (-1);
    }
    return (pthread_cond_wait(&pCond->Handle,&pMutex->Handle));
}

int
X_Cond_TimedWait(X_CondPtr pCond, X_MutexPtr pMutex, const struct timespec *pTs)
{
    if (pCond == NULL || pMutex == NULL || pTs == NULL)
    {
        XDBG("X_Cond_TimedWait(): invalid pointer");
        return (-1);
    }

    return (pthread_cond_timedwait(&pCond->Handle,&pMutex->Handle,pTs));
}

void
X_Cond_Signal(X_CondPtr pCond)
{
    int ret;

    if (pCond == NULL)
    {
        XDBG("X_Cond_Signal(): invalid pointer");
        return;
    }

    ret = pthread_cond_signal(&pCond->Handle);
}

void
X_Cond_Broadcast(X_CondPtr pCond)
{
    int ret;

    if (pCond == NULL)
    {
        XDBG("X_Cond_Broadcast(): invalid pointer");
        return;
    }

    ret = pthread_cond_broadcast(&pCond->Handle);
}

void
X_TimeGet(struct timespec *pTs)
{
    assert (pTs != NULL);
    clock_gettime(CLOCK_REALTIME, pTs);
}

struct timespec*
X_TimeAdd(struct timespec *pTs, long ms)
{
    assert (pTs != NULL);
    pTs->tv_sec   += (ms / 1000);
    pTs->tv_nsec  += (ms - (ms / 1000) * 1000) * 1000000;
    if (pTs->tv_nsec > 1000000000)
    {
        pTs->tv_sec += 1;
        pTs->tv_nsec -= 1000000000;
    }
    return pTs;
}

int
X_TimeCompare(const struct timespec *pTs1, const struct timespec *pTs2)
{
    assert(pTs1 != NULL && pTs2 != NULL);

    if (pTs1->tv_sec < pTs2->tv_sec)
    {
        return -1;
    }
    else if (pTs1->tv_sec > pTs2->tv_sec)
    {
        return 1;
    }

    if (pTs1->tv_nsec < pTs2->tv_nsec)
    {
        return -1;
    }
    else if (pTs1->tv_nsec > pTs2->tv_sec)
    {
        return 1;
    }

    return 0;
}

long
X_TimeDiff(const struct timespec* prev, const struct timespec* now)
{
    assert(prev != NULL && now != NULL);

    return (now->tv_sec * 1000 + now->tv_nsec / 1000000) -
            (prev->tv_sec * 1000 + prev->tv_nsec / 1000000);
}
