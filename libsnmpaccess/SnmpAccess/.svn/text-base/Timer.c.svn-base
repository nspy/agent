
#include "Timer.h"

void X_Timer_Init(X_Timer *timer)
{
    if (timer != NULL)
    {
        timer->Elapsed = 0;
        timer->Enabled = 0;
        timer->Timeout = 0;
    }
}

void X_Timer_Start(X_Timer *timer, long timeout)
{
    if (timer != NULL && timeout > 0)
    {
        timer->Timeout = timeout;
        timer->Elapsed = 0;
        timer->Enabled = 1;
        gettimeofday(&timer->Tick, NULL);
    }
}

void X_Timer_Stop(X_Timer *timer)
{
    if (timer != NULL)
    {
        timer->Enabled = 0;
    }
}

int X_Timer_Update(X_Timer *timer)
{
    struct timeval tick;

    if (timer != NULL)
    {
        if (timer->Enabled)
        {
            gettimeofday(&tick, NULL);
            timer->Elapsed += (tick.tv_sec * 1000 + tick.tv_usec / 1000) -
                    (timer->Tick.tv_sec * 1000 + timer->Tick.tv_usec / 1000);
            timer->Tick.tv_sec = tick.tv_sec;
            timer->Tick.tv_usec = tick.tv_usec;

            if (timer->Elapsed >= timer->Timeout)
            {
                return 1;
            }
        }
    }

    return 0;
}


void
X_Sleep(long ms)
{
    usleep(ms*1000);
}
