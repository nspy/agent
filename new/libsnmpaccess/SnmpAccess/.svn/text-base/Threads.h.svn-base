
#ifndef __THREADS_H__
#define __THREADS_H__

#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef pthread_t           X_ThreadHandle_t;
typedef pthread_mutex_t     X_MutexHandle_t;
typedef pthread_cond_t      X_CondHandle_t;
typedef void (*X_ThreadProc_t)(void*);

struct _X_Thread;
struct _X_Mutex;
struct _X_Cond;

typedef struct _X_Thread*       X_ThreadPtr;
typedef struct _X_Mutex*        X_MutexPtr;
typedef struct _X_Cond*         X_CondPtr;

/**
 * Create new thread object.
 */
X_ThreadPtr
X_Thread_New(void);

/**
 * Destroy thread object.
 */
void
X_Thread_Free(X_ThreadPtr);

/**
 * Initialize structire
 */
int
X_Thread_Init(X_ThreadPtr);

/**
 * Deinitialize structure.
 */
void
X_Thread_Deinit(X_ThreadPtr);

/**
 * Start thread.
 */
int
X_Thread_Start(X_ThreadPtr, X_ThreadProc_t, void* arg, int detach);

void
X_Thread_Join(X_ThreadPtr);

/******************************************************************************/

/**
 * Create new mutex object.
 */
X_MutexPtr
X_Mutex_New(void);

/**
 * Destroy mutex.
 */
void
X_Mutex_Free(X_MutexPtr);

/*
 * Initialize (internal)
 */
int
X_Mutex_Init(X_MutexPtr);

/*
 * DeInitialize (internal)
 */
void
X_Mutex_Deinit(X_MutexPtr);

int
X_Mutex_Lock(X_MutexPtr);

int
X_Mutex_Unlock(X_MutexPtr);

/******************************************************************************/

X_CondPtr
X_Cond_New(void);

int
X_Cond_Init(X_CondPtr);

void
X_Cond_Deinit(X_CondPtr);

void
X_Cond_Free(X_CondPtr);

int
X_Cond_Wait(X_CondPtr, X_MutexPtr);

int
X_Cond_TimedWait(X_CondPtr, X_MutexPtr, const struct timespec*);

void
X_Cond_Signal(X_CondPtr);

void
X_Cond_Broadcast(X_CondPtr);

/******************************************************************************/

void
X_TimeGet(struct timespec*);

struct timespec*
X_TimeAdd(struct timespec*, long ms);

int
X_TimeCompare(const struct timespec*, const struct timespec*);

/**
 * Returns the time difference in ms.
 */
long
X_TimeDiff(const struct timespec* prev, const struct timespec* now);

#endif
