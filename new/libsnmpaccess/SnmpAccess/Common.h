
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <arpa/inet.h>
#include <sys/types.h>

#ifdef BYTE_ORDER_64
#define htobe64(x) (x)
#define be64toh(x) (x)
#define htole64(x) (x)
#define le64toh(x) (x)
#endif

/******************************************************************************/

typedef int ErrCode;

/** Common error codes **/
enum
{
    ERRCODE_NOERROR = 0,
    ERRCODE_NOERROR_END,
    ERRCODE_INVALID_PARAMS,
    ERRCODE_TIMEOUT,
    ERRCODE_INVALID_STATE,
    ERRCODE_NOMEM,
    ERRCODE_INTERNAL_ERROR,
    ERRCODE_NOOBJECT,

    ERRCODE_OTHER,
};


#define ERRCODE_UNKNOWN_MSG "ERRCODE_???"

/** Return textual representation of an error code **/
const char *GetErrMsg(int);

/******************************************************************************/

/** Identifiers for logging functionality **/

///< SNMP Channel
#define X_LOG_CHAN_ID           (0)

///< SNMP Interface
#define X_LOG_INTF_ID           (1)

/// X_Resource
#define X_LOG_RES_ID            (2)

///< GMPLS Control Library
#define X_LOG_GMPLS_LIB_ID      (5)


uint64_t
_htonll(uint64_t n);

uint64_t
_ntohll(uint64_t n);

#endif
