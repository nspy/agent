
#include "OpenFlow.h"

#if 1
#define DBG(a, b...)    fprintf(stderr,"DBG-OF-DRV: "a"\n", ##b)
#else
#define DBG(a, b...)
#endif

/******************************************************************************/

OpenFlow_Driver_Event*
OpenFlow_Driver_Event_New(OpenFlow_Drvier_Event_Type Type)
{
    OpenFlow_Driver_Event *pEvent = (OpenFlow_Driver_Event*)(malloc(sizeof(OpenFlow_Driver_Event)));
    if (pEvent != NULL)
    {
        pEvent->Type = Type;
    }
    return pEvent;
}

void
OpenFlow_Driver_Event_Free(OpenFlow_Driver_Event *pEvent)
{
    if (pEvent != NULL)
    {
        if (pEvent->Connection != NULL)
        {
            free(pEvent->Connection);
        }
        free(pEvent);
    }
}

/******************************************************************************/

/** Create new driver object **/
OpenFlow_DriverPtr
OpenFlow_Driver_New(void)
{
    OpenFlow_DriverPtr pDrv = (OpenFlow_DriverPtr)malloc(sizeof(OpenFlow_Driver));
    if(pDrv != NULL)
        _OpenFlow_Driver_Init(pDrv);
    return pDrv;
}

/** Initialize with default values **/
void
_OpenFlow_Driver_Init(OpenFlow_DriverPtr pDrv)
{
    int i;
    assert (pDrv != NULL);
    pDrv->Connection = NULL;
    pDrv->VConn = NULL;
    pDrv->PVConn = NULL;
    pDrv->MsgQueue = X_Queue_New((X_ListMemFree_t)(OpenFlow_Driver_Event_Free));
    pDrv->UserData = NULL;
    pDrv->Connected = 0;
    pDrv->Run = 1;
    pDrv->Thread = X_Thread_New();

    X_Timer_Init(&pDrv->ReconnectTimer);
    X_Timer_Init(&pDrv->IdleTimer);

    for (i = 0; i < OPENFLOW_DRV_CB_MAX; ++i)
        pDrv->Callbacks[i] = NULL;
}

void
OpenFlow_Driver_Main(OpenFlow_DriverPtr pDrv)
{
    struct timespec ts;
    OpenFlow_Driver_Event *pEvent;

    DBG("Starting OpenFlow_Driver_Main()...");

    while (pDrv->Run != 0)
    {
        X_TimeGet(&ts);
        X_TimeAdd(&ts, 10);

        while(0 == X_Queue_Get(pDrv->MsgQueue, &ts, (void**)&pEvent))
        {
            DBG("Event received...");
            if (pEvent != NULL)
            {
                ///
                switch (pEvent->Type)
                {
                case OPENFLOW_DRV_EV_CONNECT:
                    if (pDrv->Connected == 0)
                    {
                        if (pDrv->Connection) free(pDrv->Connection);
                        pDrv->Connection = strdup(pEvent->Connection);
                    }
                    _OpenFlow_Driver_Connect(pDrv);
                    break;

                case OPENFLOW_DRV_EV_DISCONNECT:
                    _OpenFlow_Driver_Disconnect(pDrv);
                    break;

                default:
                    break;
                }
                OpenFlow_Driver_Event_Free(pEvent);
            }
        }

        if (pDrv->VConn != NULL)
            _OpenFlow_Driver_Handle_Connections(pDrv);

        if ( X_Timer_Update(&pDrv->ReconnectTimer) != 0)
        {
            X_Timer_Stop(&pDrv->ReconnectTimer);
            _OpenFlow_Driver_Connect(pDrv);
        }

        if ( X_Timer_Update(&pDrv->IdleTimer) != 0)
        {
            X_Timer_Stop(&pDrv->IdleTimer);
            X_Timer_Start(&pDrv->IdleTimer, 5000);
            _OpenFlow_Driver_Send_Probe(pDrv);
        }

        X_Sleep(10);
    }

    DBG("Closing OpenFlow_Driver_Main()...");
}

void
OpenFlow_Driver_Connect(OpenFlow_DriverPtr pDrv, const char *Connection)
{
    OpenFlow_Driver_Event *Event;

    if (pDrv == NULL || Connection == NULL)
        return;

    Event = OpenFlow_Driver_Event_New(OPENFLOW_DRV_EV_CONNECT);

    if (Event != NULL)
    {
        Event->Connection = strdup(Connection);
        X_Queue_Put(pDrv->MsgQueue, Event);
    }
}

void
OpenFlow_Driver_Start(OpenFlow_DriverPtr pDrv)
{
    assert(pDrv != NULL);
    pDrv->Run = 1;
    X_Thread_Start(pDrv->Thread, (X_ThreadProc_t)OpenFlow_Driver_Main, pDrv, 0);
}

void
OpenFlow_Driver_Stop(OpenFlow_DriverPtr pDrv)
{
    assert(pDrv != NULL);
    pDrv->Run = 0;
    X_Thread_Join(pDrv->Thread);
}

int
OpenFlow_Driver_Set_Callback(OpenFlow_DriverPtr pDrv, int type, OpenFlow_Driver_Callback_t f)
{
    if (pDrv == NULL || type < 0 || type >= OPENFLOW_DRV_CB_MAX)
    {
        return -1;
    }

    pDrv->Callbacks[type] = f;

    return 0;
}

int
OpenFlow_Driver_Set_UserData(OpenFlow_DriverPtr pDrv, void *data)
{
    if (pDrv == NULL)
        return -1;
    pDrv->UserData = data;
    return 0;
}

int
OpenFlow_Driver_Get_UserData(OpenFlow_DriverPtr pDrv, void** data)
{
    if (pDrv == NULL || data == NULL)
        return -1;
    *data = pDrv->UserData;
    return 0;
}

void
_OpenFlow_Driver_Handle_Connections(OpenFlow_DriverPtr pDrv)
{
    struct ofpbuf *pMsg = NULL;
    int ret;

    assert (pDrv != NULL);

    do
    {
        ret = vconn_recv(pDrv->VConn, &pMsg);

        if (ret == 0)
        {
            _OpenFlow_Driver_Handle_Message(pDrv, pDrv->VConn, (struct ofp_header*)pMsg->base);
            ofpbuf_delete(pMsg);
        }
    } while (ret == 0);

    if (ret != 0)
    {
        if (ret == EOF)
        {
            DBG("Connection %s closed", vconn_get_name(pDrv->VConn));
            vconn_close(pDrv->VConn);
            pDrv->VConn = NULL;
            _OpenFlow_Driver_Run_Callback(pDrv, OPENFLOW_DRV_CB_DISCONNECTED, NULL);
            pDrv->Connected = 0;
            X_Timer_Start(&pDrv->ReconnectTimer, 5000);
        }
    }
}

int
_OpenFlow_Driver_Run_Callback(OpenFlow_DriverPtr pDrv, OpenFlow_Driver_Callback_Type type, struct ofp_header *pMsg)
{
    assert (pDrv != NULL);
    assert (type >= 0 && type < OPENFLOW_DRV_CB_MAX);
    if (pDrv->Callbacks[type] != NULL)
        return pDrv->Callbacks[type](pDrv, type, pMsg);
    return 0;
}

void
_OpenFlow_Driver_Handle_Message(OpenFlow_DriverPtr pDrv, struct vconn *pConn, struct ofp_header *pMsg)
{
    DBG("OpenFlow message received!");

    assert (pDrv != NULL && pConn != NULL && pMsg != NULL);

    if (pMsg->type == OFPT_HELLO)
    {
        pMsg->type = OFPT_HELLO;
        _OpenFlow_Driver_Send_Raw(pDrv, (void*)pMsg, (size_t)ntohs(pMsg->length));
    }

    if (pMsg->type == OFPT_ECHO_REQUEST)
    {
        // dispatch OFPT_ECHO_REQUEST locally
        pMsg->type = OFPT_ECHO_REPLY;
        _OpenFlow_Driver_Send_Raw(pDrv, (void*)pMsg, (size_t)ntohs(pMsg->length));
    }
    else if (pMsg->type == OFPT_FEATURES_REQUEST)
    {
        _OpenFlow_Driver_Run_Callback(pDrv, OPENFLOW_DRV_CB_FEATURES_REQ, pMsg);
    }
    else
    {
        _OpenFlow_Driver_Run_Callback(pDrv, OPENFLOW_DRV_CB_MESSAGE, pMsg);
    }
}

void
_OpenFlow_Driver_Send_Raw(OpenFlow_DriverPtr pDrv, char *b, size_t s)
{
    int ret;
    struct ofpbuf *msg;

    if (pDrv->VConn == NULL)
        return;

    msg = ofpbuf_new(s);
    ofpbuf_put(msg, b, s);

    ret = vconn_send_block(pDrv->VConn, msg);

    if (ret != 0)
    {
        DBG("Error while sending message (%s)", strerror(errno));
        ofpbuf_delete(msg);


        _OpenFlow_Driver_Disconnect(pDrv);
    }
}

void
_OpenFlow_Driver_Connect(OpenFlow_DriverPtr pDrv)
{
    int ret;

    DBG("_OpenFlow_Driver_Connect(): Connection = %s", pDrv->Connection);

    ret = vconn_open_block(pDrv->Connection, 0, &pDrv->VConn);

    DBG("vconn_open() = %d (%s)", ret, strerror(ret));

    if (ret != 0)
    {
        DBG("Unable to open connection (%s)", strerror(errno));
        pDrv->Connected = 0;
        X_Timer_Start(&pDrv->ReconnectTimer, 5000);

    }
    else
    {
        DBG("Connection established (%s)", vconn_get_name(pDrv->VConn));
        pDrv->Connected = 1;
        X_Timer_Stop(&pDrv->IdleTimer);
        X_Timer_Start(&pDrv->IdleTimer, 5000);
    }
}

void
_OpenFlow_Driver_Disconnect(OpenFlow_DriverPtr pDrv)
{
    if (pDrv->VConn != NULL)
    {
        vconn_close(pDrv->VConn);
        pDrv->VConn = NULL;
    }
    pDrv->Connected = 0;
    X_Timer_Stop(&pDrv->IdleTimer);

    X_Timer_Stop(&pDrv->ReconnectTimer);
    X_Timer_Start(&pDrv->ReconnectTimer, 5000);
}

void
_OpenFlow_Driver_Send_Probe(OpenFlow_DriverPtr pDrv)
{
    struct ofp_header msg;

    msg.length = htons(sizeof(msg));
    msg.version = OFP_VERSION;
    msg.type = OFPT_ECHO_REQUEST;
    msg.xid = 0;

    _OpenFlow_Driver_Send_Raw(pDrv, (char*)&msg, sizeof(msg));
}
