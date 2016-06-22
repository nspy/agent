
#include <SnmpAccess/SnmpAccess.h>
#include <SnmpAccess/Worker.h>

X_QueuePtr      TaskQueue;

X_WorkerPtr     Worker;

X_WorkerPtr     Ticker;

SNMP_ChannelPtr Channel;

SNMP_Channel_Conf Config;

int main(int argc, char *argv[])
{
    char Cmd[128];

    X_TaskPtr pTask;

    X_Log_Open(X_LOG_MASK_STDERR);
    X_Log_SetLevel(0, 3);
    X_Log_SetLevel(1, 3);

    TaskQueue = X_Queue_New((X_ListMemFree_t)X_Task_Free);

    Worker = X_Worker_New(TaskQueue);

    Ticker = X_Worker_New(NULL);



    X_Worker_Start(Worker, 0);

    X_Worker_Start(Ticker, 0);



    SNMP_Channel_Conf_Init(&Config);

    strcpy(Config.Host, "10.12.72.3");

    strcpy(Config.Community, "private");

    Channel = SNMP_Channel_New(&Config);

    Channel->TaskQueue = TaskQueue;

    X_Worker_Add_Ticker(Ticker, &Channel->Ticker);

    SNMP_Channel_Init(Channel);

    while(fgets(Cmd, sizeof(Cmd), stdin))
    {
        pTask = X_Task_New();

        X_Queue_Put(TaskQueue, (void*)pTask);

    }

    X_Worker_Wait(Worker);
    X_Worker_Wait(Ticker);

    X_Worker_Free(Worker);
    X_Worker_Free(Ticker);

    return (0);

}
