
#include "Log.h"
//#include <pthread.h>
#include "Threads.h"

//static pthread_mutex_t      _Log_Mutex = PTHREAD_MUTEX_INITIALIZER;
static X_MutexPtr           _Log_Mutex = NULL;
static int                  _Log_Mask = 0;
static FILE*                _Log_File = NULL;
static int                  _Log_Level[X_LOG_CONTEXT_MAX];
static X_Log_Callback_t     _Log_Cb = NULL;
static char                 _Log_TmpBuf[1024];

/*
#define LOG_LOCK            pthread_mutex_lock(&_Log_Mutex)
#define LOG_UNLOCK          pthread_mutex_unlock(&_Log_Mutex)
*/

#define LOG_LOCK        if (_Log_Mutex != NULL) { X_Mutex_Lock(_Log_Mutex); }
#define LOG_UNLOCK      if (_Log_Mutex != NULL) { X_Mutex_Unlock(_Log_Mutex); }

void
X_Log_Open(int mask)
{
    int i = 0;

    for(; i < X_LOG_LEVEL_MAX; ++i)
    {
        _Log_Level[i] = -1;
    }

    _Log_Mask = mask;

    if (_Log_Mutex != NULL)
    {
        _Log_Mutex = X_Mutex_New();
    }
}

void
X_Log_Close(void)
{
    LOG_LOCK;

    _Log_Mask = 0;

    if (_Log_File != NULL)
    {
        fclose(_Log_File);
        _Log_File = NULL;
    }

    _Log_Cb = NULL;

    LOG_UNLOCK;

    X_Mutex_Free(_Log_Mutex);

    _Log_Mutex = NULL;
}

int
X_Log_SetFile(const char *path)
{
    int ret;
    LOG_LOCK;
    if (_Log_File != NULL)
    {
        fclose(_Log_File);
    }
    _Log_File = fopen(path, "w");
    ret = (_Log_File != NULL) ? 0 : 1;
    LOG_UNLOCK;
    return ret;
}

int
X_Log_SetMask(int mask)
{
    int ret;
    LOG_LOCK;
    ret = _Log_Mask;
    _Log_Mask = mask;
    LOG_UNLOCK;
    return ret;
}

void
X_Log_SetCallback(X_Log_Callback_t cb)
{
    LOG_LOCK;
    _Log_Cb = cb;
    LOG_UNLOCK;
}

void
X_Log_SetLevel(int context, int level)
{
    if (context < 0 || context >= X_LOG_CONTEXT_MAX) return;

    if (level >= X_LOG_LEVEL_MAX) level = X_LOG_LEVEL_MAX - 1;
    else if (level < 0) level = 0;

    LOG_LOCK;
    _Log_Level[context] = level;
    LOG_UNLOCK;
}

size_t
X_Log_Printf(int id, int level, const char *fmt, ...)
{
    va_list ap;
    size_t pos = 0;
    time_t t;
    struct tm tmtime;

    if (id < 0 || id >= X_LOG_CONTEXT_MAX) return 0;

    LOG_LOCK;

    if (level < 0 || level > _Log_Level[id])
    {
        LOG_UNLOCK;
        return 0;
    }

    if (!(_Log_Mask & X_LOG_MASK_NO_TIMESTAMP))
    {
        t = time(NULL);
        localtime_r(&t, &tmtime);

        pos += snprintf(_Log_TmpBuf + pos,
                sizeof(_Log_TmpBuf) - pos,
                "%d-%02d-%02d %02d:%02d:%02d | ",
                tmtime.tm_year+1900,tmtime.tm_mon+1,tmtime.tm_mday,
                tmtime.tm_hour,tmtime.tm_min,tmtime.tm_sec);
    }

    /*
    pos += snprintf(_Log_TmpBuf + pos,
            sizeof(_Log_TmpBuf) - pos,
            "<%d> | ", id);
    */

    va_start(ap,fmt);
    pos += vsnprintf(_Log_TmpBuf + pos,
            sizeof(_Log_TmpBuf) - pos,
            fmt, ap);
    va_end(ap);

    if (_Log_Mask & X_LOG_MASK_STDOUT)
    {
        fprintf(stdout, "%s\r\n", _Log_TmpBuf);
    }

    if (_Log_Mask & X_LOG_MASK_STDERR)
    {
        fprintf(stderr, "%s\r\n", _Log_TmpBuf);
    }

    if (_Log_Mask & X_LOG_MASK_FILE)
    {
        if (_Log_File != NULL)
            fprintf(_Log_File, "%s\r\n", _Log_TmpBuf);
    }

    if (_Log_Mask & X_LOG_MASK_LOGFUNC)
    {
        if (_Log_Cb != NULL)
            _Log_Cb(id, level, _Log_TmpBuf);
    }

    LOG_UNLOCK;

    return pos;
}

size_t
X_Log_VPrintf(int id, int level, const char *fmt, va_list ap)
{
    size_t pos = 0;
    time_t t;
    struct tm tmtime;

    if (id < 0 || id >= X_LOG_CONTEXT_MAX)
        return 0;

    LOG_LOCK;

    if (level < 0 || level > _Log_Level[id])
    {
        LOG_UNLOCK;
        return 0;
    }

    if (!(_Log_Mask & X_LOG_MASK_NO_TIMESTAMP))
    {
        t = time(NULL);
        localtime_r(&t,&tmtime);

        pos += snprintf(_Log_TmpBuf + pos,sizeof(_Log_TmpBuf) - pos,"%d-%02d-%02d %02d:%02d:%02d | ",
                tmtime.tm_year + 1900, tmtime.tm_mon + 1, tmtime.tm_mday,
                tmtime.tm_hour,tmtime.tm_min,tmtime.tm_sec);
    }

    pos += vsnprintf(_Log_TmpBuf + pos,sizeof(_Log_TmpBuf) - pos,fmt,ap);

    if (_Log_Mask & X_LOG_MASK_STDOUT)
    {
        fprintf(stdout,"%s\r\n",_Log_TmpBuf);
    }

    if (_Log_Mask & X_LOG_MASK_STDERR)
    {
        fprintf(stderr,"%s\r\n",_Log_TmpBuf);
    }

    if (_Log_Mask & X_LOG_MASK_FILE)
    {
        if (_Log_File != NULL)
            fprintf(_Log_File,"%s\r\n",_Log_TmpBuf);
    }

    if (_Log_Mask & X_LOG_MASK_LOGFUNC)
    {
        if (_Log_Cb != NULL)
            _Log_Cb(id,level,_Log_TmpBuf);
    }

    LOG_UNLOCK;

    return pos;
}
