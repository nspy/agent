

#include <SnmpAccess/SnmpAccess.h>
#include <SnmpAccess/Worker.h>

X_QueuePtr      Queue;

X_ListPtr       Workers;

X_Ticker        Ticker;

void Dispatch(X_TaskPtr pTask)
{
    printf("*\n");
}

void Tick(void *arg)
{
    printf(".\n");
}

int main(int argc, char *argv[])
{
    X_WorkerPtr pWorker;
    X_ListNodePtr pNode;
    int N;
    char Cmd[128], *p;
    X_TaskPtr pTask;

    if (argc < 2)
    {
        printf("usage: %s <n>\r\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);

    Queue = X_Queue_New((X_ListMemFree_t)X_Task_Free);

    Workers = X_List_New(NULL);

    Ticker.Tick = Tick;

    while (N > 0)
    {
        pWorker = X_Worker_New(Queue);

        if (pWorker != NULL)
        {
            X_List_Append(Workers, pWorker);
        }

        X_Worker_Start(pWorker, 0);

        N -= 1;
    }

    pWorker = (X_WorkerPtr)(X_List_Head(Workers)->Data);

    X_Worker_Add_Ticker(pWorker, &Ticker);


    while (fgets(Cmd, sizeof(Cmd), stdin))
    {
        p = strchr(Cmd, '\n');

        if (p != NULL)
        {
            *p = '\0';
        }

        if (!strcmp(Cmd, "quit"))
        {
            break;
        }

        pTask = X_Task_New();

        pTask->Dispatch = Dispatch;

        X_Queue_Put(Queue, (void*)pTask);
    }

    X_List_ForEach(Workers, pNode)
    {
        pWorker = (X_WorkerPtr)pNode->Data;
        pWorker->Run = 0;
    }

    while (X_List_Size(Workers) > 0)
    {
        X_List_Remove(Workers, X_List_Head(Workers), (void**)&pWorker);

        X_Thread_Join(pWorker->Thread);

        X_Worker_Free(pWorker);
    }

    X_List_Free(Workers);

    X_Queue_Free(Queue);

    return (0);
}
