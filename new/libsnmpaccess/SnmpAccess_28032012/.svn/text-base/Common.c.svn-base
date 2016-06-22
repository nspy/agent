
#include "Common.h"

/** Textual representation of common ErrCodes **/
static const char *const ErrCodeMsg[] =
{
        "NO ERROR",
        "NO ERROR END",
        "INVALID PARAMS",
        "TIMEOUT",
        "INVALID STATE",
        "NO MEMORY",
        "INTERNAL ERROR",
        "NO OBJECT",

        "OTHER ERROR"
};

const char *GetErrMsg(int err)
{
    if (err >= 0 && err < (int)(sizeof(ErrCodeMsg)/sizeof(char*)))
    {
        return ErrCodeMsg[err];
    }
    return ERRCODE_UNKNOWN_MSG;
}



uint64_t
_htonll(uint64_t n)
{
    return htonl(1) == 1 ? n : ((uint64_t) htonl(n) << 32) | htonl(n >> 32);
}

uint64_t
_ntohll(uint64_t n)
{
    return htonl(1) == 1 ? n : ((uint64_t) ntohl(n) << 32) | ntohl(n >> 32);
}
