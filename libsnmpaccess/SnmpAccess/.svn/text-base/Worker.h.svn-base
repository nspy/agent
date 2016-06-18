
#ifndef __X_MSG_HANDLER_H__
#define __X_MSG_HANDLER_H__

#include <SnmpAccess/Queue.h>
#include <SnmpAccess/Timer.h>
#include <assert.h>

/******************************************************************************/

struct _X_Task;
struct _X_TaskQueue;
struct _X_Worker;
struct _X_Ticker;

typedef struct _X_Task*         X_TaskPtr;
typedef struct _X_TaskQueue*    X_TaskQueuePtr;
typedef struct _X_Worker*       X_WorkerPtr;
typedef struct _X_Ticker*       X_TickerPtr;

/******************************************************************************/

/** Function to relase memory allocated for a task structure **/
typedef void (*X_Task_Free_t)(void*);

typedef void (*X_Task_Dispatch_t)(X_TaskPtr);


/** Task to be executed by a Worker **/
typedef struct _X_Task
{
    /** Use this to differentiate between various tasks (0 - default) **/
    unsigned int        Key;

    /** Pointer to a task processing function **/
    X_Task_Dispatch_t   Dispatch;

    /** Pointer to a 'destructor', if NULL then use free() **/
    X_Task_Free_t       Free;

    /** User defined pointer **/
    void*               Data;

} X_Task;

X_TaskPtr
X_Task_New(void);

void
X_Task_Free(X_TaskPtr);

/******************************************************************************/

/** Worker thread **/
typedef struct _X_Worker
{
    /** Pointer to monitored queue **/
    X_QueuePtr          Queue;

    /** Thread associated with a worker **/
    X_ThreadPtr         Thread;

    /** Callback to dispatch Tasks -> moved to the X_Task structure (more flexible) **/
    //X_Task_Dispatch_t   Dispatch;

    /** Work flag **/
    int                 Run;

    /** List of Tickers **/
    X_ListPtr           Tickers;

    /** Mutex to protect internal data **/
    X_MutexPtr          Mutex;
} X_Worker;

/**
 * Create a new Worker, note that the thread is not started yet!
 */
X_WorkerPtr
X_Worker_New(X_QueuePtr); //, X_Task_Dispatch_t);

void
X_Worker_Init(X_WorkerPtr, X_QueuePtr); //, X_Task_Dispatch_t);

void
X_Worker_Add_Ticker(X_WorkerPtr, X_TickerPtr);

void
X_Worker_Remove_Ticker(X_WorkerPtr, X_TickerPtr);

void
X_Worker_Free(X_WorkerPtr);

int
X_Worker_Start(X_WorkerPtr, int Detach);

void
X_Worker_Stop(X_WorkerPtr);

void
X_Worker_Wait(X_WorkerPtr);

/******************************************************************************/

/** X_Ticker callback function type. **/
typedef void (*X_Ticker_Tick_t)(void*);

/**
 * Ticker object can be registered in the Worker thread to recieve periodic ticks.
 */
typedef struct _X_Ticker
{
    /** Callback function **/
    X_Ticker_Tick_t     Tick;

    /** User data **/
    void*               Data;
} X_Ticker;

/** Initialize X_Ticker **/
void
X_Ticker_Init(X_TickerPtr, X_Ticker_Tick_t, void*);

#endif
