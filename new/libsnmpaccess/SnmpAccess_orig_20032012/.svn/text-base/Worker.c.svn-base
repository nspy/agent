
#include "Worker.h"

#define DBG(a, b...) fprintf(stderr, "DBG: "a"\r\n", ##b)

/******************************************************************************/

X_TaskPtr
X_Task_New(void)
{
    X_TaskPtr pTask = (X_TaskPtr)malloc(sizeof(X_Task));
    if (pTask != NULL)
    {
        pTask->Key = 0;
        pTask->Dispatch = NULL;
        pTask->Free = NULL;
    }
    return pTask;
}

void
X_Task_Free(X_TaskPtr pTask)
{
    if (pTask != NULL)
    {
        if (pTask->Free != NULL)
        {
            pTask->Free(pTask);
        }
        else
        {
            free(pTask);
        }
    }
}

/******************************************************************************/

static void
X_Worker_ThreadProc(X_WorkerPtr);

X_WorkerPtr
X_Worker_New(X_QueuePtr pQueue) //, X_Task_Dispatch_t f)
{
    X_WorkerPtr pWorker;

    pWorker = (X_WorkerPtr)malloc(sizeof(X_Worker));

    if (pWorker == NULL)
    {
        return NULL;
    }

    X_Worker_Init(pWorker, pQueue); //, f);

    return pWorker;
}

void
X_Worker_Init(X_WorkerPtr pWorker, X_QueuePtr pQueue) //, X_Task_Dispatch_t f)
{
    assert(pWorker != NULL);

    if (pWorker == NULL)
    {
        return;
    }

    pWorker->Queue = pQueue;
    pWorker->Run = 0;
    pWorker->Thread = X_Thread_New();
    //pWorker->Dispatch = f;
    pWorker->Mutex = X_Mutex_New();
    pWorker->Tickers = NULL;
}

void
X_Worker_Add_Ticker(X_WorkerPtr pWorker, X_TickerPtr pTicker)
{
    if (pWorker == NULL || pTicker == NULL)
    {
        return;
    }

    X_Mutex_Lock(pWorker->Mutex);

    if (pWorker->Tickers == NULL)
    {
        pWorker->Tickers = X_List_New(NULL);
    }

    X_List_Append(pWorker->Tickers, pTicker);

    X_Mutex_Unlock(pWorker->Mutex);
}

void
X_Worker_Remove_Ticker(X_WorkerPtr pWorker, X_TickerPtr pTicker)
{
    X_ListNodePtr pNode;

    if (pWorker == NULL || pTicker == NULL)
    {
        return;
    }

    X_Mutex_Lock(pWorker->Mutex);

    if (pWorker->Tickers != NULL)
    {
        pNode = X_List_Find(pWorker->Tickers, pTicker, NULL);

        if (pNode != NULL)
        {
            X_List_Destroy(pWorker->Tickers, pNode);
        }

    }

    X_Mutex_Unlock(pWorker->Mutex);
}

void
X_Worker_Free(X_WorkerPtr pWorker)
{
    if (pWorker != NULL)
    {

        X_Thread_Free(pWorker->Thread);
        X_Mutex_Free(pWorker->Mutex);

        if (pWorker->Tickers)
        {
            X_List_Free(pWorker->Tickers);
        }

        free (pWorker);
    }
}

int
X_Worker_Start(X_WorkerPtr pWorker, int Detach)
{
    if (pWorker == NULL)
    {
        return -1;
    }

    pWorker->Run = 1;

    return X_Thread_Start(pWorker->Thread,
            (X_ThreadProc_t)X_Worker_ThreadProc, pWorker, Detach);
}

void
X_Worker_Stop(X_WorkerPtr pWorker)
{
    if (pWorker != NULL)
    {
        pWorker->Run = 0;
    }
}

void
X_Worker_Wait(X_WorkerPtr pWorker)
{
    if (pWorker != NULL)
    {
        pWorker->Run = 0;
        X_Thread_Join(pWorker->Thread);
    }
}

void
X_Worker_ThreadProc(X_WorkerPtr pWorker)
{
    struct timespec ts1, ts2;
    X_ListNodePtr pNode;
    X_TaskPtr pTask;
    X_TickerPtr pTicker;
    long Tick = 100, Delay;

    DBG("Starting X_Worker thread...");

    while (pWorker->Run)
    {
        X_TimeGet(&ts1);

        X_Mutex_Lock(pWorker->Mutex);

        if (pWorker->Tickers != NULL)
        {
            X_List_ForEach(pWorker->Tickers, pNode)
            {
                pTicker = (X_TickerPtr)(pNode->Data);

                if (pTicker != NULL && pTicker->Tick != NULL)
                {
                    pTicker->Tick(pTicker->Data);
                }
            }
        }

        X_Mutex_Unlock(pWorker->Mutex);

        X_TimeGet(&ts2);

        Delay = Tick - X_TimeDiff(&ts1, &ts2);

        if (Delay < 0) Delay = 0;
        else if (Delay > 100) Delay = 100;

        if (pWorker->Queue == NULL)
        {
            X_Sleep(Delay);
        }
        else
        {
            X_TimeAdd(&ts1, Delay);

            while (0 == X_Queue_Get(pWorker->Queue,&ts1,(void**) &pTask)
                    && pWorker->Run)
            {
                DBG("X_Worker thread processing task");
                if (pTask != NULL)
                {
                    if (pTask->Dispatch != NULL)
                    {
                        pTask->Dispatch(pTask);
                    }

//                    if (pWorker->Dispatch != NULL) pWorker->Dispatch(pTask);

                    X_Task_Free(pTask);
                }
                DBG("X_Worker thread processing task - done");
            }
        }

    }

    DBG("Stopping X_Worker thread...");
}

void
X_Ticker_Init(X_TickerPtr pTicker, X_Ticker_Tick_t f, void *data)
{
    assert (pTicker != NULL);
    if (pTicker != NULL)
    {
        pTicker->Tick = f;
        pTicker->Data = data;
    }
}
