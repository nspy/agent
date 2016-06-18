
#ifndef __TIMER_H__
#define __TIMER_H__

#include <unistd.h>
#include <sys/time.h>
#include <time.h>

typedef struct
{
    int     Enabled;
    long    Elapsed;
    long    Timeout;
    struct timeval  Tick;
} X_Timer;

void
X_Timer_Init(X_Timer*);

void
X_Timer_Start(X_Timer*,long);

void
X_Timer_Stop(X_Timer*);

int
X_Timer_Update(X_Timer*);

#define X_Timer_Enabled(timer) ((timer) ? (timer->Enabled) : (0))

void
X_Sleep(long ms);

#endif
