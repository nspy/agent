
#include "TrapHandler.h"
#include "Channel.h"
#include "Log.h"
#include <assert.h>

SNMP_TrapHandler*
SNMP_TrapHandler_New(const char *addr)
{
    SNMP_TrapHandler *th;

    if (addr == NULL) return NULL;

    th = (SNMP_TrapHandler*)malloc(sizeof(SNMP_TrapHandler));

    if (th == NULL) return th;

    th = SNMP_TrapHandler_Init(th, addr);

    th->Mutex = X_Mutex_New();

    th->Listeners = X_List_New(NULL);

    th->Run = 1;

    th->Thread = X_Thread_New();

    return th;
}

void
SNMP_TrapHandler_Free(SNMP_TrapHandler *th)
{
    if (th == NULL) return;

    X_Thread_Free(th->Thread);

    if (th->Session != NULL)
    {
        snmp_sess_close(th->Session);
        th->Session = NULL;
    }

    if (th->Listeners)
    {
        X_List_Free(th->Listeners);
        th->Listeners = NULL;
    }

    X_Mutex_Free(th->Mutex);

    free(th);
}

SNMP_TrapHandler*
SNMP_TrapHandler_Init(SNMP_TrapHandler *th, const char *addr)
{
    if (th == NULL || addr == NULL) return NULL;

    strncpy(th->LocalAddr, addr, sizeof(th->LocalAddr));

    th->Callback = NULL;

    if(_SNMP_TrapHandler_InitSession(th) != 0)
    {
        X_LOGERR(2, "_SNMP_TrapHandler_InitSession() failed!");
    }

    return th;
}

void
SNMP_TrapHandler_Execute(SNMP_TrapHandler *th)
{
    int retval, fds = 0, block = 1;
    fd_set fdset;
    struct timeval timeout;

    if (th == NULL) return;

    FD_ZERO(&fdset);

    snmp_sess_select_info(th->Session, &fds, &fdset, &timeout, &block);

    block = 0;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    retval = select(fds, &fdset, NULL, NULL, block ? NULL : &timeout);

    if (retval < 0)
    {

    }
    else if (retval == 0)
    {
        snmp_sess_timeout(th->Session);
    }
    else
    {
        snmp_sess_read(th->Session, &fdset);
    }
}

void
SNMP_TrapHandler_Add(SNMP_TrapHandler *th, struct _SNMP_Channel* ch)
{
    X_ListNodePtr node;
    if (th == NULL || ch == NULL) return;

    _SNMP_TrapHandler_Lock(th);

    for(node = th->Listeners->Head; node; node = node->Next)
    {
        if (node->Data == ch)
        {
            _SNMP_TrapHandler_Unlock(th);
            return;
        }
    }

    X_List_Append(th->Listeners, ch);

    _SNMP_TrapHandler_Unlock(th);
}

void
SNMP_TrapHandler_Remove(SNMP_TrapHandler *th, struct _SNMP_Channel *ch)
{
    X_ListNodePtr node;

    if (th == NULL) return;

    _SNMP_TrapHandler_Lock(th);

    for (node = th->Listeners->Head; node; node = node->Next)
    {
        if (node->Data == ch)
        {
            X_List_Destroy(th->Listeners, node);
            _SNMP_TrapHandler_Unlock(th);
            return;
        }
    }

    _SNMP_TrapHandler_Unlock(th);
}


void
SNMP_TrapHandler_Start(SNMP_TrapHandler *th)
{
    assert(th != NULL);
    X_Thread_Start(th->Thread, SNMP_TrapHandler_ThreadProc, th, 1);
}

void
SNMP_TrapHandler_Run(SNMP_TrapHandler *th)
{
    int retval, fds = 0, block = 1;
    fd_set fdset;
    struct timeval timeout;

    assert(th != NULL);

    if (th->Session == NULL)
    {
        return;
    }

    while (th->Run)
    {
        FD_ZERO(&fdset);

        snmp_sess_select_info(th->Session,&fds,&fdset,&timeout,&block);

        block = 1;
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000;

        retval = select(fds,&fdset,NULL,NULL,block ? NULL : &timeout);

        if (retval < 0)
        {

        }
        else if (retval == 0)
        {
            snmp_sess_timeout(th->Session);
        }
        else
        {
            snmp_sess_read(th->Session,&fdset);
        }
    }
}

void
SNMP_TrapHandler_SetCallback(SNMP_TrapHandler *th, SNMP_TrapHandler_Callback_t cb)
{
    if (th != NULL)
    {
        th->Callback = cb;
    }
}

int
_SNMP_TrapHandler_InitSession(SNMP_TrapHandler *th)
{
    struct snmp_session session;
    netsnmp_transport *tr;

    assert(th != NULL);

    snmp_sess_init(&session);

    session.version = SNMP_VERSION_1;
    session.peername = SNMP_DEFAULT_PEERNAME;
    session.community_len = SNMP_DEFAULT_COMMUNITY_LEN;
    session.callback = _SNMP_TrapHandler_Callback;
    session.callback_magic = (void*)(th);

    //session.local_port = htons(1620);
    //th->Session = snmp_sess_open(&session);

    tr = netsnmp_transport_open_server("", th->LocalAddr);

    if (tr == NULL)
    {
        return -1;
    }

    th->Session = (struct snmp_session*)snmp_sess_add(&session,tr,NULL,NULL);

    if (th->Session == NULL)
    {
        return 1;
    }

    return 0;
}



int
_SNMP_TrapHandler_Callback(int oper, netsnmp_session *sess, int reqid, netsnmp_pdu *pdu, void *user)
{
    SNMP_TrapHandler *th;
    X_ListPtr pList;
    X_ListNodePtr node;

    if (user == NULL) return 1;

    th = (SNMP_TrapHandler*)(user);

    _SNMP_TrapHandler_Lock(th);

    pList = th->Listeners;

    if (oper == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE)
    {
        if (reqid == 0)  // trap
        {
            if (th->Callback) th->Callback(th, pdu);

            for (node = pList->Head; node; node = node->Next)
            {
                SNMP_Channel *ch = SNMP_CHANNEL(node->Data);

                if (ch != NULL)
                {
                    SNMP_Channel_On_Trap(ch, pdu);
                }
            }
        }
    }

    _SNMP_TrapHandler_Unlock(th);

    return 1;
}

void
_SNMP_TrapHandler_Lock(SNMP_TrapHandler *th)
{
    X_Mutex_Lock(th->Mutex);
}

void
_SNMP_TrapHandler_Unlock(SNMP_TrapHandler *th)
{
    X_Mutex_Unlock(th->Mutex);
}

void
SNMP_TrapHandler_ThreadProc(void *arg)
{
    SNMP_TrapHandler *pTh = (SNMP_TrapHandler*)(arg);

    if (pTh == NULL) return;

    SNMP_TrapHandler_Run(pTh);
}
