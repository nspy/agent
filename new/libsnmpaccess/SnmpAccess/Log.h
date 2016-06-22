
#ifndef __LOG_H__
#define __LOG_H__

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

/******************************************************************************/

#define X_LOG_CONTEXT_MAX         (8)

#define X_LOG_LEVEL_ERR         (0)
#define X_LOG_LEVEL_WARN        (1)
#define X_LOG_LEVEL_INFO        (2)
#define X_LOG_LEVEL_DBG         (3)
#define X_LOG_LEVEL_MAX         (4)

#define X_LOG_MASK_STDERR         (1 << 1)
#define X_LOG_MASK_STDOUT         (1 << 2)
#define X_LOG_MASK_FILE           (1 << 3)
#define X_LOG_MASK_LOGFUNC        (1 << 4)
#define X_LOG_MASK_NO_TIMESTAMP   (1 << 5)

typedef void (*X_Log_Callback_t)(int, int, const char*);

#define X_LOG(id, level, fmt, str...) X_Log_Printf(id, level, fmt, str)
#define X_VLOG(id, level, fmt, str...) X_Log_VPrintf(id, level, fmt, str)

#define X_LOGERR(id, fmt, str...)     X_Log_Printf(id, 0, fmt, ##str)
#define X_LOGWARN(id, fmt, str...)    X_Log_Printf(id, 1, fmt, ##str)
#define X_LOGINFO(id, fmt, str...)    X_Log_Printf(id, 2, fmt, ##str)
#define X_LOGDBG(id, fmt, str...)     X_Log_Printf(id, 3, fmt, ##str)



/******************************************************************************/

void
X_Log_Open(int mask);

void
X_Log_Close(void);

int
X_Log_SetFile(const char*);

void
X_Log_SetCallback(X_Log_Callback_t);

int
X_Log_SetMask(int);

void
X_Log_SetLevel(int id, int level);

size_t
X_Log_Printf(int id, int level, const char *fmt, ...);

size_t
X_Log_VPrintf(int id, int level, const char *fmt, va_list ap);

#endif
