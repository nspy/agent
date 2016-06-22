
#include "Channel.h"

/******************************************************************************/

/** Forward declarations for internal functions**/

/**
 * Internal processing of TRAP list.
 */
void
_SNMP_Channel_Process_Traps(SNMP_Channel*);

ErrCode
_SNMP_Channel_Process_Traps_Ext(SNMP_Channel*, Entity*, SNMP_ErrCodePtr);

ErrCode
_SNMP_Channel_Process_Traps_Ext_CP(SNMP_Channel*, Entity*, SNMP_ErrCodePtr);


/******************************************************************************/

static SNMP_OidPtr EntityStateAdminOid = NULL;
static SNMP_OidPtr EntityStateOperOid = NULL;
static SNMP_OidPtr EntityClassOid = NULL;
static SNMP_OidPtr EntityIndexOid = NULL;
static SNMP_OidPtr CPWDMEntityClassOid = NULL;

void
InitCommonOids(void)
{
    if (EntityStateAdminOid == NULL)
    {
        EntityStateAdminOid = SNMP_Oid_New_From_Node(
                &FspR7_MIB_entityStateAdmin,
                NULL, 0, 1);
    }

    if (EntityStateOperOid == NULL)
    {
        EntityStateOperOid = SNMP_Oid_New_From_Node(
                &FspR7_MIB_entityStateOper,
                NULL, 0, 1);
    }

    if (EntityClassOid == NULL)
    {
        EntityClassOid = SNMP_Oid_New_From_Node(
                &ADVA_MIB_entityClass,
                NULL, 0, 1);
    }

    if (CPWDMEntityClassOid == NULL)
    {
        CPWDMEntityClassOid = SNMP_Oid_New("ADVA-MIB::controlPlaneWdmEntityClass",
                1, NULL, 0);
    }

    if (EntityIndexOid == NULL)
    {
        EntityIndexOid = SNMP_Oid_New("ADVA-MIB::entityIndex",
                1, NULL, 0);
    }
}

static const char *__EventName[] =
{
        "EV_INIT_REQ",
        "EV_SYNCH_REQ",

        "EV_RELOAD_CP",

        "EV_LOAD_INVENTORY",

        "EV_TIMER_POLL",
        "EV_TIMER_RECONNECT",
        "EV_TIMER_TRAP",
        "EV_TIMER_CP_RELOAD",
        "EV_TIMER_RESYNC",

        "EV_TIMER_CRS_RELOAD",

};

/******************************************************************************/

// milliseconds
long SNMP_Channel_Timeouts[SNMP_CHANNEL_TIMER_MAX] =
{
        10000,
        5000,
        3000,
        10000,
        0,
        10000,
};

/******************************************************************************/

void
SNMP_Channel_Inventory_Init(SNMP_Channel_Inventory *inv)
{
    if (inv == NULL) return;
    EntityNE_Init(&inv->NE);
    inv->ShelfList      = X_List_New(EntityFree);
    inv->ModuleList     = X_List_New(EntityFree);
    inv->ExtObjList     = X_List_New(EntityFree);
    inv->PlugList       = X_List_New(EntityFree);
    inv->ConnectionList = X_List_New(EntityFree);
    inv->OLList         = X_List_New(EntityFree);
    inv->CrsList        = X_List_New(EntityFree);
}

void
SNMP_Channel_Inventory_Clear(SNMP_Channel_Inventory *inv)
{
    if (inv == NULL) return;
    EntityNE_Init(&inv->NE);
    if (inv->ShelfList) X_List_Clear(inv->ShelfList);
    if (inv->ModuleList) X_List_Clear(inv->ModuleList);
    if (inv->ExtObjList) X_List_Clear(inv->ExtObjList);
    if (inv->PlugList) X_List_Clear(inv->PlugList);
    if (inv->ConnectionList) X_List_Clear(inv->ConnectionList);
    if (inv->OLList) X_List_Clear(inv->OLList);
    if (inv->CrsList) X_List_Clear(inv->CrsList);
}

void
SNMP_Channel_Inventory_Deinit(SNMP_Channel_Inventory *inv)
{
    if (inv == NULL) return;
    EntityNE_Init(&inv->NE);
    if (inv->ShelfList) X_List_Free(inv->ShelfList);
    if (inv->ModuleList) X_List_Free(inv->ModuleList);
    if (inv->ExtObjList) X_List_Free(inv->ExtObjList);
    if (inv->PlugList) X_List_Free(inv->PlugList);
    if (inv->ConnectionList) X_List_Free(inv->ConnectionList);
    if (inv->OLList) X_List_Free(inv->OLList);
    if (inv->CrsList) X_List_Free(inv->CrsList);
}

void
SNMP_Channel_Inventory_Dump(SNMP_Channel_Inventory *inv, FILE *fp)
{
    char tmp[256];
    X_ListNodePtr node;

    if (inv == NULL || fp == NULL) return;

    X_LOGINFO(0,"==============================");
    X_LOGINFO(0,"SHELF TOTAL = %d", inv->ShelfList->Size);
    for(node = X_List_Head(inv->ShelfList); node; node = X_List_Next(node))
    {
        Entity_Dump(node->Data, tmp, sizeof(tmp));
        X_LOGINFO(X_LOG_CHAN_ID, " %s", tmp);
    }

    X_LOGINFO(0,"==============================");
    X_LOGINFO(0,"MOD TOTAL = %d", inv->ModuleList->Size);
    for(node = X_List_Head(inv->ModuleList); node; node = X_List_Next(node))
    {
        Entity_Dump(node->Data, tmp, sizeof(tmp));
        X_LOGINFO(X_LOG_CHAN_ID, " %s", tmp);
    }

    X_LOGINFO(0,"==============================");
    X_LOGINFO(0,"ECH TOTAL = %d", inv->ExtObjList->Size);
    for(node = X_List_Head(inv->ExtObjList); node; node = X_List_Next(node))
    {
        Entity_Dump(node->Data, tmp, sizeof(tmp));
        X_LOGINFO(X_LOG_CHAN_ID, " %s", tmp);
    }

    X_LOGINFO(0, "==============================");
    X_LOGINFO(0,"OL TOTAL = %d", inv->OLList->Size);
    for(node = X_List_Head(inv->OLList); node; node = X_List_Next(node))
    {
        Entity_Dump(node->Data, tmp, sizeof(tmp));
        X_LOGINFO(X_LOG_CHAN_ID, " %s", tmp);
    }

    X_LOGINFO(0, "==============================");
    X_LOGINFO(0,"CONN TOTAL = %d", inv->ConnectionList->Size);
    for(node = X_List_Head(inv->ConnectionList); node; node = X_List_Next(node))
    {
        Entity_Dump(node->Data, tmp, sizeof(tmp));
        X_LOGINFO(X_LOG_CHAN_ID, " %s", tmp);
    }

    X_LOGINFO(0, "==============================");
    X_LOGINFO(0,"CRS TOTAL = %d", inv->CrsList->Size);
    for(node = X_List_Head(inv->CrsList); node; node = X_List_Next(node))
    {
        EntityCrs_Dump(node->Data, tmp, sizeof(tmp));
        X_LOGINFO(X_LOG_CHAN_ID, " %s", tmp);
    }
}

/******************************************************************************/

void
SNMP_Channel_InventoryCP_Init(SNMP_Channel_InventoryCP *inv)
{
    if (inv == NULL) return;
    inv->TunnelWDMList = X_List_New(EntityFree);
    inv->CnxWDMList = X_List_New(EntityFree);
    inv->TEUnnumLinks = X_List_New(EntityFree);
    inv->TENumLinks = X_List_New(EntityFree);
}

void
SNMP_Channel_InventoryCP_Clear(SNMP_Channel_InventoryCP *inv)
{
    if (inv == NULL) return;
    if(inv->TunnelWDMList) X_List_Clear(inv->TunnelWDMList);
    if(inv->CnxWDMList) X_List_Clear(inv->CnxWDMList);
    if(inv->TEUnnumLinks) X_List_Clear(inv->TEUnnumLinks);
    if(inv->TENumLinks) X_List_Clear(inv->TENumLinks);
}

void
SNMP_Channel_InventoryCP_Deinit(SNMP_Channel_InventoryCP *inv)
{
    if(inv == NULL) return;
    if(inv->TunnelWDMList) X_List_Free(inv->TunnelWDMList);
    if(inv->CnxWDMList) X_List_Free(inv->CnxWDMList);
    if(inv->TEUnnumLinks) X_List_Free(inv->TEUnnumLinks);
    if(inv->TENumLinks) X_List_Free(inv->TENumLinks);
}

void
SNMP_Channel_InventoryCP_Dump(SNMP_Channel_InventoryCP *inv)
{
    char tmp[256];
    X_ListNodePtr node;

    if (inv == NULL) return;

    X_LOGINFO(X_LOG_CHAN_ID, "==============================");
    X_LOGINFO(X_LOG_CHAN_ID, "TUNNEL WDM TOTAL = %d", inv->TunnelWDMList->Size);
    for(node = X_List_Head(inv->TunnelWDMList); node; node = X_List_Next(node))
    {
        Entity_Dump(node->Data, tmp, sizeof(tmp));
        X_LOGINFO(0, tmp);
    }

    X_LOGINFO(X_LOG_CHAN_ID, "==============================");
    X_LOGINFO(X_LOG_CHAN_ID, "CNX WDM TOTAL = %d", inv->CnxWDMList->Size);
    for(node = X_List_Head(inv->CnxWDMList); node; node = X_List_Next(node))
    {
        Entity_Dump((EntityCNX*)node->Data,tmp,sizeof(tmp));
        X_LOGINFO(0, tmp);
    }

    X_LOGINFO(X_LOG_CHAN_ID, "==============================");
    X_LOGINFO(X_LOG_CHAN_ID, "TE UNNUM. LINKS TOTAL = %d", inv->TEUnnumLinks->Size);
    X_List_ForEach(inv->TEUnnumLinks, node)
    {
        EntityTEUnnumLink_Dump((EntityTEUnnumLink*)node->Data,tmp,sizeof(tmp));
        X_LOGINFO(0, tmp);
    }

    X_LOGINFO(X_LOG_CHAN_ID, "==============================");
    X_LOGINFO(X_LOG_CHAN_ID, "TE NUM. LINKS TOTAL = %d", inv->TENumLinks->Size);
    X_List_ForEach(inv->TENumLinks, node)
    {
        EntityTENumLink_Dump((EntityTENumLink*)node->Data,tmp,sizeof(tmp));
        X_LOGINFO(X_LOG_CHAN_ID, tmp);
    }
}


/******************************************************************************/

SNMP_Channel_Event*
SNMP_Channel_Event_New(int Type)
{
    SNMP_Channel_Event *e = (SNMP_Channel_Event*)malloc(sizeof(SNMP_Channel_Event));
    if (e != NULL)
    {
        memset(e, 0, sizeof(SNMP_Channel_Event));
        e->Type = Type;
    }
    return e;
}

void
SNMP_Channel_Event_Free(SNMP_Channel_Event *e)
{
    if (e != NULL)
    {
        free(e);
    }
}

size_t
SNMP_Channel_Event_Dump(SNMP_Channel_Event *e, char *b, size_t s)
{
    if (e == NULL || b == NULL) return 0;
    return snprintf(b, s, "%s (%d)",
            SNMP_Channel_Event_GetName(e->Type), e->Type);
}

const char*
SNMP_Channel_Event_GetName(int type)
{
    if (type < 0 || type >= SNMP_CHANNEL_EV_MAX) return "EV_???";
    return __EventName[type];
}

/******************************************************************************/

void
SNMP_Channel_Conf_Init(SNMP_Channel_Conf *conf)
{
    if (conf != NULL)
    {
        strcpy(conf->Host, "192.168.0.1");
        strcpy(conf->Community, "public");
        conf->Version = SNMP_VERSION_1;
        conf->PollTimeout = 30;
        conf->CPReloadTimeout = 0;
        conf->ResyncTimeout = 10;
    }
}

void
SNMP_Channel_Conf_Set_Host(SNMP_Channel_Conf *pConf, const char *host)
{
    if (pConf != NULL)
    {
        strncpy(pConf->Host, host, sizeof(pConf->Host));
    }
}

void
SNMP_Channel_Conf_Set_Community(SNMP_Channel_Conf *pConf, const char *comm)
{
    if (pConf != NULL)
    {
        strncpy(pConf->Community, comm, sizeof(pConf->Community));
    }
}

SNMP_Channel_Conf*
SNMP_Channel_Conf_Copy(SNMP_Channel_Conf *copy, const SNMP_Channel_Conf *orig)
{
    memcpy(copy, orig, sizeof(SNMP_Channel_Conf));
    return copy;
}

/******************************************************************************/

SNMP_ChannelPtr
SNMP_Channel_New(SNMP_Channel_Conf *pConf)
{
    ErrCode err;
    SNMP_ChannelPtr pCh;
    int i;

    pCh = (SNMP_ChannelPtr)malloc(sizeof(SNMP_Channel));

    if (pCh == NULL)
    {
        X_LOGERR(X_LOG_CHAN_ID, "SNMP_Channel_New: failed to allocate memory");
        return NULL;
    }

    memset(pCh, 0, sizeof(SNMP_Channel));

    /** Init config with default values **/
    SNMP_Channel_Conf_Init(&pCh->Config);

    /** Update config **/
    if (pConf != NULL)
    {
        SNMP_Channel_Conf_Copy(&pCh->Config, pConf);
    }
    else
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_New: using the default configuration");
    }

    /** Create net-snmp session handler **/
    if (ERRCODE_NOERROR != (err = _SNMP_Channel_Create_Session(pCh, &pCh->Config)))
    {
        X_LOGERR(X_LOG_CHAN_ID, "SNMP_Channel_New: failed to create session (%s)", GetErrMsg(err));
        free(pCh);
        return NULL;
    }

    //pthread_mutex_init(&ch->Mutex, NULL);
    pCh->Mutex = X_Mutex_New();

    pCh->IntMutex = X_Mutex_New();

#if 0
    pCh->EvList = X_List_New(free);
#endif

    pCh->EvQueue = X_Queue_New((X_ListMemFree_t)free);

    pCh->TrapEvList = X_List_New(EntityFree);

    pCh->State = SNMP_CHANNEL_STATE_NOT_INIT;

    for(i = 0; i < SNMP_CHANNEL_TIMER_MAX; ++i)
    {
        X_Timer_Init(&pCh->Timers[i]);
    }

    pCh->Context = NULL;

    /** Init inventory list **/
    SNMP_Channel_Inventory_Init(&pCh->Inventory);

    SNMP_Channel_InventoryCP_Init(&pCh->InventoryCP);

    pCh->Run = 1;

    pCh->Thread = X_Thread_New();

    X_LOGDBG(X_LOG_CHAN_ID, "SNMP_Channel_New: new session %08x created!", pCh);

    InitCommonOids();

    //// Worker/Tasks API ///

    pCh->TaskQueue = NULL;

    X_Ticker_Init(&pCh->Ticker,
            (X_Ticker_Tick_t)(_SNMP_Channel_ProcessTimers), pCh);

    ///////

    return pCh;
}

void SNMP_Channel_Free(SNMP_Channel *pCh)
{
    if (pCh != NULL)
    {
        SNMP_Channel_InventoryCP_Deinit(&pCh->InventoryCP);

        SNMP_Channel_Inventory_Deinit(&pCh->Inventory);

        X_Thread_Free(pCh->Thread);

        if (pCh->TrapEvList != NULL)
        {
            X_List_Free(pCh->TrapEvList);
            pCh->TrapEvList = NULL;
        }

        if (pCh->EvQueue != NULL)
        {
            X_Queue_Free(pCh->EvQueue);
            pCh->EvQueue = NULL;
        }

#if 0
        if (pCh->EvList != NULL)
        {
            X_List_Free(pCh->EvList);
            pCh->EvList = NULL;
        }
#endif

        if (pCh->Mutex)
        {
            X_Mutex_Free(pCh->Mutex);
            pCh->Mutex = NULL;
        }

        if (pCh->IntMutex != NULL)
        {
            X_Mutex_Free(pCh->IntMutex);
            pCh->IntMutex = NULL;
        }

        if (pCh->Session != NULL)
        {
            snmp_sess_close(pCh->Session);
            pCh->Session = NULL;
        }
    }
}

ErrCode
SNMP_Channel_Init(SNMP_Channel *pCh)
{
    SNMP_Channel_Event *e;

    if (pCh == NULL)
    {
        X_LOGERR(X_LOG_CHAN_ID, "SNMP_Channel_Init: invalid pointer");
        return ERRCODE_INVALID_PARAMS;
    }

    //SNMP_Channel_Schedule_Task(pCh, 0, SNMP_CHANNEL_EV_INIT_REQ);

    e = SNMP_Channel_Event_New(SNMP_CHANNEL_EV_INIT_REQ);

    _SNMP_Channel_PutEvent(pCh, e);

    return ERRCODE_NOERROR;
}

void
SNMP_Channel_Set_Context(SNMP_Channel *ch, void *c)
{
    if (ch != NULL)
    {
        ch->Context = c;
    }
}

void
SNMP_Channel_Execute(SNMP_Channel *ch)
{
    SNMP_Channel_Event *e = _SNMP_Channel_GetEvent(ch);

    _SNMP_Channel_ProcessTimers(ch);

    if (e != NULL)
    {
        SNMP_Channel_Dispatch(ch, e);
        SNMP_Channel_Event_Free(e);
    }
}

void
SNMP_Channel_On_Trap(SNMP_Channel *pCh, netsnmp_pdu *pdu)
{
    struct variable_list *v;
    unsigned int        Addr;
    unsigned long       Idx; ///< Index of the entity that TRAP refers to
    unsigned int        Class; ///< Class of the entity
    unsigned int        CP_Class; ///< Class of the CP entity
    int i;
    Entity En;

    if (pCh == NULL || pdu == NULL)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_On_Trap(%08x): invalid arguments", pCh);
        return;
    }

    if (pCh->State != SNMP_CHANNEL_STATE_READY)
    {
        X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_On_Trap(%08x): not synchronized", pCh);
    }

    Addr = pCh->Inventory.NE.SysIP;


    // check if the TRAP originates from the NE that we're connected to
    // TODO: verify this for le/be issues !
    for(i = 0 ; i < 4; ++i)
    {
        if (((Addr >> (8*i)) & 0xff) != pdu->agent_addr[i])
        {
            // addr doesn't match
            return;
        }
    }

    Idx = 0;
    Class = CP_Class = 0;

    X_LOGINFO(X_LOG_CHAN_ID,"SNMP_Channel_ProcessTrap(%08x): TRAP received from %08x", pCh, Addr);

    for(v = pdu->variables; v; v = v->next_variable)
    {
        print_variable(v->name, v->name_length, v);

        if (
                !netsnmp_oid_is_subtree(EntityStateAdminOid->Name,
                        EntityStateAdminOid->Length,
                        v->name,v->name_length) ||
                !netsnmp_oid_is_subtree(EntityStateOperOid->Name,
                        EntityStateOperOid->Length,
                        v->name,v->name_length) ||
                !netsnmp_oid_is_subtree(EntityIndexOid->Name,
                        EntityIndexOid->Length,
                        v->name, v->name_length)
                        )
        {
            Idx = SNMP_Var_Index(v);
        }
        else if(!netsnmp_oid_is_subtree(EntityClassOid->Name,
                EntityClassOid->Length,
                v->name, v->name_length))
        {
            Idx = SNMP_Var_Index(v);
            SNMP_Var_Set_Integer(v, (int*)&Class);
        }
        else if(!netsnmp_oid_is_subtree(CPWDMEntityClassOid->Name,
                  CPWDMEntityClassOid->Length,
                  v->name, v->name_length))
        {
            Idx = SNMP_Var_Index(v);
            SNMP_Var_Set_Integer(v, (int*)&CP_Class);
        }
    }

    // If Index was retrieved, add this to pending TRAPs
    if (Idx != 0)
    {
        //X_LOGINFO(0, "<<< TRAP EVENT >>>");
        Entity_Init(&En);

        En.Index        = Idx;
        En.Class        = Class;
        En.CPClass      = CP_Class;

        X_Mutex_Lock(pCh->IntMutex);

        if (NULL == __FindByIndex(&En, pCh->TrapEvList, NULL))
        {
            X_List_Append(pCh->TrapEvList, Entity_Dup(&En));
            if (!pCh->Timers[SNMP_CHANNEL_TIMER_TRAP].Enabled)
            {
                _SNMP_Channel_Timer(pCh, SNMP_CHANNEL_TIMER_TRAP, 1);
            }
        }
        //X_LOGINFO(0, "<<< TRAP EVENT (%d) >>>", X_List_Size(pCh->TrapEvList));
        X_Mutex_Unlock(pCh->IntMutex);
    }
}


ErrCode
SNMP_Channel_Dispatch(SNMP_Channel *ch, SNMP_Channel_Event *e)
{
    char tmp[128];

    assert (ch != NULL);

    if (ch == NULL || e == NULL)
    {
        X_LOGERR(X_LOG_CHAN_ID, "SNMP_Channel_Dispatch(%08x): invalid arguments!", ch);
        return ERRCODE_INVALID_PARAMS;
    }

    SNMP_Channel_Event_Dump(e, tmp, sizeof(tmp));

    X_LOGDBG(X_LOG_CHAN_ID, "SNMP_Channel_Dispatch(%08x): dispatch event - %s @ %s",
            ch, tmp,
            SNMP_Channel_GetStateName(ch->State));

    switch (ch->State)
    {
    case SNMP_CHANNEL_STATE_NOT_INIT:
        _SNMP_Channel_HandleNotInit(ch, e);
        break;

    case SNMP_CHANNEL_STATE_DISCONNECTED:
        _SNMP_Channel_HandleDisconnected(ch, e);
        break;

    case SNMP_CHANNEL_STATE_SYNCH:
        _SNMP_Channel_HandleSynch(ch, e);
        break;

    case SNMP_CHANNEL_STATE_READY:
        _SNMP_Channel_HandleReady(ch, e);
        break;

    default:
        X_LOGERR(X_LOG_CHAN_ID, "SNMP_Channel_Dispatch(%08x): invalid state (%d)!",
                ch, ch->State);
        return ERRCODE_INVALID_STATE;
    }

    return ERRCODE_NOERROR;
}

void
SNMP_Channel_Lock(SNMP_Channel *pCh)
{
    assert(pCh != NULL);
    if (pCh != NULL)
    {
        X_Mutex_Lock(pCh->Mutex);
    }
}

void
SNMP_Channel_Unlock(SNMP_Channel *pCh)
{
    assert(pCh != NULL);
    if (pCh != NULL)
    {
        X_Mutex_Unlock(pCh->Mutex);
    }
}


ErrCode
SNMP_Channel_Start(SNMP_Channel *pCh)
{
    int ret;

    assert(pCh != NULL);

    ret = X_Thread_Start(pCh->Thread, SNMP_Channel_ThreadProc, pCh, 1);

    if (ret != 0)
    {
        X_LOGERR(X_LOG_CHAN_ID, "SNMP_Channel_Start: failed to create the thread");
        return ERRCODE_OTHER;
    }

    return ERRCODE_NOERROR;
}

void
SNMP_Channel_Run(SNMP_Channel *pCh)
{
    SNMP_Channel_Event *e;
    struct timespec ts;

    assert(pCh != NULL);

    while (pCh->Run)
    {
        X_TimeGet(&ts);
        X_TimeAdd(&ts, 200);

        _SNMP_Channel_ProcessTimers(pCh);

        while(0 == X_Queue_Get(pCh->EvQueue, &ts, (void**)&e))
        {
            SNMP_Channel_Dispatch(pCh, e);
            _SNMP_Channel_ProcessTimers(pCh);
        }
    }
}

void
SNMP_Channel_Stop(SNMP_ChannelPtr pCh)
{
    assert (pCh != NULL);

    pCh->Run = 0;

    X_Thread_Join(pCh->Thread);
}

ErrCode
SNMP_Channel_SetCallback(SNMP_ChannelPtr pCh, int type, SNMP_Channel_Callback_t cb)
{
    if (pCh == NULL)
    {
        X_LOGERR(X_LOG_CHAN_ID, "SNMP_Channel_SetCallback: invalid channel pointer");
        return ERRCODE_INVALID_PARAMS;
    }

    if (type < 0 || type >= SNMP_CHANNEL_CB_MAX)
    {
        X_LOGERR(X_LOG_CHAN_ID, "SNMP_Channel_SetCallback: invalid callback type");
        return ERRCODE_INVALID_PARAMS;
    }

    pCh->Callbacks[type] = cb;

    return ERRCODE_NOERROR;
}

void
SNMP_Channel_ClearAllCallbacks(SNMP_Channel *ch)
{
    int i;
    if (ch != NULL)
    {
        for(i = 0; i < SNMP_CHANNEL_CB_MAX; ++i)
        {
            ch->Callbacks[i] = NULL;
        }
    }
}


netsnmp_session*
SNMP_Channel_Duplicate_Session(SNMP_ChannelPtr pCh)
{
    netsnmp_session Session, *pSession;

    if (pCh == NULL || pCh->Session == NULL)
    {
        return NULL;
    }

    snmp_sess_init(&Session);

    Session.version = pCh->Config.Version;
    Session.peername = pCh->Config.Host;
    Session.community = (u_char*)pCh->Config.Community;
    Session.community_len = strlen(pCh->Config.Community);

    pSession = snmp_sess_open(&Session);

    return pSession;
}

void
SNMP_Channel_Destroy_Session(SNMP_ChannelPtr pCh, netsnmp_session *pSession)
{
    if (pSession != NULL)
    {
        snmp_sess_close((void*)pSession);
    }
}

ErrCode
_SNMP_Channel_Create_Session(SNMP_Channel *ch, SNMP_Channel_Conf *conf)
{
    netsnmp_session session;

    if (ch == NULL || conf == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    snmp_sess_init(&session);
    session.version = conf->Version;
    session.peername = conf->Host;
    session.community = (u_char*) conf->Community;
    session.community_len = strlen(conf->Community);

    ch->Session = (netsnmp_session*)snmp_sess_open(&session);

    return (ch->Session != NULL) ? ERRCODE_NOERROR : ERRCODE_OTHER;
}

void
_SNMP_Channel_State_Change(SNMP_Channel *chan, int state)
{
    if (chan == NULL)
    {
        X_LOGERR(X_LOG_CHAN_ID, "_SNMP_Channel_StateChange: invalid pointer!");
        return;
    }

    if (chan->State == state)
    {
        return;
    }

    X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel(%08x) state change %s -> %s",
            chan,
            SNMP_Channel_GetStateName(chan->State),
            SNMP_Channel_GetStateName(state));

    switch (chan->State)
    {
    case SNMP_CHANNEL_STATE_READY:
        _SNMP_Channel_Timer(chan, SNMP_CHANNEL_TIMER_POLL, 0); // disable polling timer
        _SNMP_Channel_Timer(chan, SNMP_CHANNEL_TIMER_CP_RELOAD, 0); // disbale CP reload timer
        break;
    }

    chan->State = state;

    switch (chan->State)
    {
    case SNMP_CHANNEL_STATE_READY:
        _SNMP_Channel_Timer(chan, SNMP_CHANNEL_TIMER_POLL, 1);
        _SNMP_Channel_Timer(chan, SNMP_CHANNEL_TIMER_CP_RELOAD, 1);
        _SNMP_Channel_Callback(chan, SNMP_CHANNEL_CB_READY, NULL);
        break;

    case SNMP_CHANNEL_STATE_NOT_INIT:
    case SNMP_CHANNEL_STATE_DISCONNECTED:
        _SNMP_Channel_Callback(chan, SNMP_CHANNEL_CB_DISCONNECTED, NULL);

        if (chan->State == SNMP_CHANNEL_STATE_DISCONNECTED)
        {
            _SNMP_Channel_Timer(chan, SNMP_CHANNEL_TIMER_RESYNC, 1);
        }

        break;

    default: break;
    }
}

ErrCode
_SNMP_Channel_LoadConfig(SNMP_Channel *pCh)
{
    SNMP_ErrCode Err;
    ErrCode ret;
    int status, err;

    assert(pCh != NULL);

    if (pCh == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }



    X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig: loading NE");

    SNMP_ErrCode_Init(&Err);

    ret = SNMP_GetEntityNE(pCh->Session, &Err, &pCh->Inventory.NE);

    if (ret != ERRCODE_NOERROR || Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig(): errcode(%s), status(%d)",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    // SHELF

    X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig: loading SHELF");
    ret = SNMP_GetEntityShelfList(pCh->Session, &Err, pCh->Inventory.ShelfList);

    if (ret != ERRCODE_NOERROR || Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig(): errcode(%s), status(%d)",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    // MOD
    X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig: loading MOD");
    ret = SNMP_GetEntityModuleList(pCh->Session, &Err, pCh->Inventory.ModuleList);

    if (ret != ERRCODE_NOERROR || Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig(): errcode(%s), status(%d)",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    // Ext Obj
    X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig: loading ECH");
    ret = SNMP_GetEntityExternalChannelList(pCh->Session, &Err, pCh->Inventory.ExtObjList);

    if (ret != ERRCODE_NOERROR || Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig(): errcode(%s), status(%d)",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    status = GetEntityPlugList(pCh->Session,&err,pCh->Inventory.PlugList);

    if (status != STAT_SUCCESS)
    {
        return status;
    }

    X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig: loading CONN");

    SNMP_ErrCode_Init(&Err);

    ret = SNMP_GetEntityConnectionList(pCh->Session, &Err, pCh->Inventory.ConnectionList);

    if (ret != ERRCODE_NOERROR || Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig(): errcode(%s), status(%d)",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    /*
    status = GetEntityPTPList(pCh->Session,&err,pCh->Inventory.PTPList);

    if (status != STAT_SUCCESS)
    {
        return status;
    }
    */

    X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig: loading OL");

    ret = SNMP_GetEntityOLList(pCh->Session,&Err,pCh->Inventory.OLList);

    if (ret != ERRCODE_NOERROR || Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig(): errcode(%s), status(%d)",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }


    // Crs

    X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig: loading CRS_CH");

    ret = SNMP_GetCrsList(pCh->Session,&Err,pCh->Inventory.CrsList);

    if (ret != ERRCODE_NOERROR || Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig(): errcode(%s), status(%d)",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }


    status = _SNMP_Channel_LoadConfigCP(pCh);

    if (status != STAT_SUCCESS)
    {
        return ERRCODE_INVALID_STATE;
    }

    X_LOGINFO(X_LOG_CHAN_ID,"_SNMP_Channel_LoadConfig(%08x): confguration loaded successfully from NE %s",
            pCh, inet_ntoa(*(struct in_addr*)(&pCh->Inventory.NE.SysIP)));

    SNMP_Channel_Inventory_Dump(&pCh->Inventory, stderr);
    SNMP_Channel_InventoryCP_Dump(&pCh->InventoryCP);

    return ERRCODE_NOERROR;
}


ErrCode
_SNMP_Channel_Load_Inventory(SNMP_Channel *pCh, SNMP_Channel_Inventory *pInv)
{
    ErrCode ret;
    SNMP_ErrCode Err;
    int status, err;

    if (pCh == NULL || pInv == NULL)
    {
        X_LOGERR(X_LOG_CHAN_ID, "_SNMP_Channel_LoadInventory: invalid arguments");
        return ERRCODE_INVALID_PARAMS;
    }

    // NE

    ret = SNMP_GetEntityNE(pCh->Session, &Err, &pInv->NE);

    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig: ret = %s status = %d",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    // SHELF

    ret = SNMP_GetEntityShelfList(pCh->Session, &Err, pInv->ShelfList);
    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig: ret = %s status = %d",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    // MOD

    ret = SNMP_GetEntityModuleList(pCh->Session, &Err, pInv->ModuleList);
    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig: ret = %s status = %d",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    // PL

    status = GetEntityPlugList(pCh->Session,&err,pInv->PlugList);

    if (status != STAT_SUCCESS)
    {
        return status;
    }

    // CONN

    ret = SNMP_GetEntityConnectionList(pCh->Session, &Err, pInv->ConnectionList);

    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig: ret = %s status = %d",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    // OL

    ret = SNMP_GetEntityOLList(pCh->Session,&Err,pCh->Inventory.OLList);

    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig: ret = %s status = %d",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    // Crs

    ret = SNMP_GetCrsList(pCh->Session,&Err,pCh->Inventory.CrsList);

    if (ret != ERRCODE_NOERROR || Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfig: ret = %s status = %d",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    X_LOGINFO(X_LOG_CHAN_ID,"_SNMP_Channel_LoadInventory(%08x): confguration loaded successfully from NE %s",
            pCh, inet_ntoa(*(struct in_addr*)(&pInv->NE.SysIP)));

    return ERRCODE_NOERROR;
}

int
_SNMP_Channel_LoadConfigCP(SNMP_Channel *pCh)
{
    ErrCode ret;
    SNMP_ErrCode Err;

    assert(pCh != NULL);

    X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfigCP(): loading TNL_WDMs");

    ret = SNMP_GetEntityTunnelWDMList(pCh->Session, &Err, pCh->InventoryCP.TunnelWDMList);

    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfigCP: ret = %s status = %d",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfigCP(): loading CNX_WDMs");

    ret = SNMP_GetEntityCnxWDMList(pCh->Session, &Err, pCh->InventoryCP.CnxWDMList);

    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfigCP: ret = %s status = %d",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfigCP(): loading unnumbered TE links");

    ret = SNMP_GetEntityTEUnnumLinkList(pCh->Session, &Err, pCh->InventoryCP.TEUnnumLinks);

    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfigCP: ret = %s status = %d",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfigCP(): loading numbered TE links");

    ret = SNMP_GetEntityTENumLinkList(pCh->Session, &Err, pCh->InventoryCP.TENumLinks);

    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfigCP: ret = %s status = %d",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    return (0);
}

ErrCode
_SNMP_Channel_Load_InventoryCP(SNMP_Channel *pCh, SNMP_Channel_InventoryCP *pInv)
{
    ErrCode ret;
    SNMP_ErrCode Err;

    if (pCh == NULL || pInv == NULL)
    {
        X_LOGERR(X_LOG_CHAN_ID, "_SNMP_Channel_LoadInventoryCP: invalid arguments");
        return ERRCODE_INVALID_PARAMS;
    }

    ret = SNMP_GetEntityTunnelWDMList(pCh->Session, &Err, pInv->TunnelWDMList);

    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "_SNMP_Channel_LoadInventoryCP: ret = %s status = %d",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    ret = SNMP_GetEntityCnxWDMList(pCh->Session, &Err, pInv->CnxWDMList);

    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "_SNMP_Channel_LoadInventoryCP: ret = %s status = %d",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    ret = SNMP_GetEntityTEUnnumLinkList(pCh->Session, &Err, pInv->TEUnnumLinks);

    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "_SNMP_Channel_LoadInventoryCP: ret = %s status = %d",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    ret = SNMP_GetEntityTENumLinkList(pCh->Session, &Err, pInv->TENumLinks);

    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_CHAN_ID, "SNMP_Channel_LoadConfigCP: ret = %s status = %d",
                GetErrMsg(ret), Err.Status);
        return ERRCODE_INVALID_STATE;
    }

    return (ERRCODE_NOERROR);
}

/** Reload HW related data **/
ErrCode
_SNMP_Channel_Reload_HW_Config(SNMP_ChannelPtr pCh)
{
    ErrCode ret;
    SNMP_ErrCode Err;
    X_ListPtr pList;

    if (pCh == NULL || pCh->Session == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pList = X_List_New(EntityFree);

    if (pList == NULL)
    {
        return ERRCODE_NOMEM;
    }

    // SHELF

    ret = SNMP_GetEntityShelfList(pCh->Session, &Err, pList);

    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_List_Free(pList);
        return ERRCODE_INVALID_STATE;
    }

    _SNMP_Channel_Merge_Shelves(pCh, pList);

    X_List_Clear(pList);

    // MOD

    ret = SNMP_GetEntityModuleList(pCh->Session, &Err, pList);

    if ((ret == ERRCODE_NOERROR || ret == ERRCODE_NOERROR_END) &&
            Err.Status != STAT_SUCCESS)
    {
        X_List_Free(pList);
        return ERRCODE_INVALID_STATE;
    }

    //_SNMP_Channel_Merge_Shelves(pCh, pList);

    X_List_Clear(pList);


    X_List_Free(pList);

    return ERRCODE_NOERROR;
}

void
_SNMP_Channel_Merge_Shelves(SNMP_ChannelPtr pCh, X_ListPtr pList)
{
    X_ListNodePtr pNode, pNode2;
    EntityShelf *pShelf, *pShelf2;

    if (pCh == NULL || pList == NULL) return;

    X_List_ForEach(pCh->Inventory.ShelfList, pNode)
    {
        pShelf = (EntityShelf*)pNode->Data;

        pNode2 = FindByIndex(pShelf, pList, &pShelf2);

        if (pNode2 == NULL)
        {
            _SNMP_Channel_Callback(pCh, SNMP_CHANNEL_CB_SHELF_DESTROYED, pShelf);
            pNode = X_List_Destroy(pCh->Inventory.ShelfList, pNode);
        }
        else
        {
            // ???
            // @TODO: ?
            X_List_Destroy(pList, pNode2);
        }
    }


    while (X_List_Size(pList) > 0)
    {
        X_List_Remove(pList, X_List_Head(pList), (void**)&pShelf);
        _SNMP_Channel_Callback(pCh, SNMP_CHANNEL_CB_SHELF_CREATED, pShelf);
        X_List_Append(pCh->Inventory.ShelfList, pShelf);
    }
}

int
_SNMP_Channel_ReloadConfigCP(SNMP_ChannelPtr pCh)
{
    X_ListPtr pTnlList, pCnxList;
    SNMP_ErrCode err;
    ErrCode ret;
    netsnmp_session *pSession;

    assert (pCh != NULL);

    if (pCh->Session == NULL)
    {
        return (-1);
    }

    pSession = pCh->Session;

    pTnlList = X_List_New((EntityFree));

    pCnxList = X_List_New((EntityFree));

    ret = SNMP_GetEntityTunnelWDMList(pSession, &err, pTnlList);

    if (ret != ERRCODE_NOERROR && ret != ERRCODE_NOERROR_END)
    {
        return (-1);
    }

    ret = SNMP_GetEntityCnxWDMList(pSession, &err, pCnxList);

    if (ret != ERRCODE_NOERROR && ret != ERRCODE_NOERROR_END)
    {
        return (-1);
    }

    _SNMP_Channel_Merge_Tunnels(pCh, pTnlList);
    _SNMP_Channel_Merge_CNXs(pCh, pCnxList);

    return (0);

}


void
_SNMP_Channel_Merge_Tunnels(SNMP_ChannelPtr pCh, X_ListPtr pList)
{
    X_ListNodePtr pNode, pNode2;
    EntityTunnel *pTnlEnt, *pTnlEnt2;

    assert (pCh != NULL && pList != NULL);

    X_List_ForEach(pCh->InventoryCP.TunnelWDMList, pNode)
    {
        pTnlEnt = (EntityTunnel*)(pNode->Data);

        pNode2 = FindByIndex(pTnlEnt, pList, &pTnlEnt2);

        if (pNode2 == NULL)
        {
            pNode2 = pNode;
            pNode = pNode->Next;
            _SNMP_Channel_Callback(pCh,
                    SNMP_CHANNEL_CB_TNL_WDM_DESTROYED,
                    pTnlEnt);
            X_List_Destroy(pCh->InventoryCP.TunnelWDMList, pNode2);
        }
        else
        {
            if (pTnlEnt->Admin != pTnlEnt2->Admin)
            {
                pTnlEnt->Admin = pTnlEnt2->Admin;

                _SNMP_Channel_Callback(pCh,
                        SNMP_CHANNEL_CB_TNL_WDM_ADMIN_CHANGED,
                        pTnlEnt);

            }
            X_List_Destroy(pList, pNode2);
        }
    }

    while (X_List_Size(pList) > 0)
    {
        X_List_Remove(pList, X_List_Head(pList), (void**)&pTnlEnt);
        _SNMP_Channel_Callback(pCh,
                SNMP_CHANNEL_CB_TNL_WDM_CREATED,
                pTnlEnt);
        X_List_Append(pCh->InventoryCP.TunnelWDMList, pTnlEnt);
    }
}

void
_SNMP_Channel_Merge_CNXs(SNMP_ChannelPtr pCh, X_ListPtr pList)
{
    X_ListNodePtr pNode, pNode2;
    EntityCNX *pCnxEnt, *pCnxEnt2;

    assert (pCh != NULL && pList != NULL);

    X_List_ForEach(pCh->InventoryCP.CnxWDMList, pNode)
    {
        pCnxEnt = (EntityCNX*)(pNode->Data);

        pNode2 = FindByIndex(pCnxEnt, pList, &pCnxEnt2);

        if (pNode2 == NULL)
        {
            pNode2 = pNode;
            pNode = pNode->Next;
            // callback
            _SNMP_Channel_Callback(pCh, SNMP_CHANNEL_CB_CNX_WDM_DESTROYED, pCnxEnt);
            X_List_Destroy(pCh->InventoryCP.CnxWDMList, pNode2);
        }
        else
        {
            // callback
            X_List_Destroy(pList, pNode2);
        }
      }

      while (X_List_Size(pList) > 0)
      {
          X_List_Remove(pList, X_List_Head(pList), (void**)&pCnxEnt);
          _SNMP_Channel_Callback(pCh, SNMP_CHANNEL_CB_CNX_WDM_CREATED, pCnxEnt);
          X_List_Append(pCh->InventoryCP.CnxWDMList, pCnxEnt);
      }
}

int
_SNMP_Channel_Poll(SNMP_Channel *pCh)
{
    SNMP_ErrCode err;

    assert(pCh != NULL);

    SNMP_GetSysUptime(pCh->Session, &err, NULL);

    return (err.Status);
}


SNMP_Channel_Event*
_SNMP_Channel_GetEvent(SNMP_Channel *ch)
{
    SNMP_Channel_Event *e = NULL;
    struct timespec ts;
    if (ch != NULL)
    {
        X_TimeGet(&ts);
        //pthread_mutex_lock(&ch->Mutex);
        //List_Get(ch->EvList, ch->EvList->head,(void**)&e);
        //pthread_mutex_unlock(&ch->Mutex);
        X_Queue_Get(ch->EvQueue,&ts,(void**)&e);
    }
    return e;
}


void
_SNMP_Channel_PutEvent(SNMP_Channel *ch, SNMP_Channel_Event *e)
{
    if (ch != NULL && e != NULL)
    {
        X_Queue_Put(ch->EvQueue, e);
    }
}

void
_SNMP_Channel_Timer(SNMP_Channel *pCh, int Id, int Start)
{
    long T = 0;

    assert(pCh != NULL);

    if (Id >= 0 && Id < SNMP_CHANNEL_TIMER_MAX)
    {
        if (Start)
        {
            switch (Id)
            {
            case SNMP_CHANNEL_TIMER_POLL:
                T = pCh->Config.PollTimeout * 1000;
                break;

            case SNMP_CHANNEL_TIMER_CP_RELOAD:
                T = pCh->Config.CPReloadTimeout * 1000;
                break;

            case SNMP_CHANNEL_TIMER_RESYNC:
                T = pCh->Config.ResyncTimeout * 1000;
                break;

            default:
                T = SNMP_Channel_Timeouts[Id];
                break;
            }

            if (T != 0)
                X_Timer_Start(&pCh->Timers[Id], T);
        }
        else
        {
            X_Timer_Stop(&pCh->Timers[Id]);
        }
    }
}

void
_SNMP_Channel_ProcessTimers(SNMP_Channel *ch)
{
    SNMP_Channel_Event *e;
    int i;

    //X_LOGDBG(0, "Tick");

    assert(ch != NULL);

    for(i = 0; i < SNMP_CHANNEL_TIMER_MAX; ++i)
    {
        if (X_Timer_Update(&ch->Timers[i]))
        {
            X_Timer_Stop(&ch->Timers[i]);

            e = SNMP_Channel_Event_New(SNMP_CHANNEL_EV_TIMER_POLL + i);

            if (e != NULL)
            {
                e->TimerId = i;
                _SNMP_Channel_PutEvent(ch, e);
            }
        }
    }
}

void
_SNMP_Channel_HandleNotInit(SNMP_Channel *ch, SNMP_Channel_Event *e)
{
    SNMP_Channel_Event *ev;

    assert(ch != NULL);

    switch (e->Type)
    {
    case SNMP_CHANNEL_EV_INIT_REQ:
        _SNMP_Channel_State_Change(ch, SNMP_CHANNEL_STATE_SYNCH);
        ev = SNMP_Channel_Event_New(SNMP_CHANNEL_EV_SYNCH_REQ);
        _SNMP_Channel_PutEvent(ch, ev);
        break;

    default: break;
    }
}

void
_SNMP_Channel_HandleDisconnected(SNMP_Channel *ch, SNMP_Channel_Event *e)
{
    assert(ch != NULL);

    SNMP_Channel_Event *ev;

    switch (e->Type)
    {
    case SNMP_CHANNEL_EV_TIMER_RESYNC:
        _SNMP_Channel_State_Change(ch, SNMP_CHANNEL_STATE_SYNCH);
        ev = SNMP_Channel_Event_New(SNMP_CHANNEL_EV_SYNCH_REQ);
        _SNMP_Channel_PutEvent(ch, ev);
        break;

    default:
        break;
    }
}

void
_SNMP_Channel_HandleSynch(SNMP_Channel *ch, SNMP_Channel_Event *e)
{
    int ret;

    assert(ch != NULL);

    switch(e->Type)
    {
    case SNMP_CHANNEL_EV_SYNCH_REQ:
        ret = _SNMP_Channel_LoadConfig(ch);

        if (ret == 0)
        {
            _SNMP_Channel_State_Change(ch, SNMP_CHANNEL_STATE_READY);
        }
        else
        {
            // disconnected !!!
            _SNMP_Channel_State_Change(ch, SNMP_CHANNEL_STATE_DISCONNECTED);
        }

        break;
    }
}

void
_SNMP_Channel_HandleReady(SNMP_Channel *ch, SNMP_Channel_Event *e)
{
    int ret;

    switch (e->Type)
    {
    case SNMP_CHANNEL_EV_TIMER_POLL:
        ret = _SNMP_Channel_Poll(ch);
        if (ret != 0)
        {
            _SNMP_Channel_State_Change(ch, SNMP_CHANNEL_STATE_DISCONNECTED);
        }
        else
        {
            _SNMP_Channel_Timer(ch, SNMP_CHANNEL_TIMER_POLL, 1);
        }
        break;

    case SNMP_CHANNEL_EV_TIMER_CP_RELOAD:
        _SNMP_Channel_ReloadConfigCP(ch);
        _SNMP_Channel_Timer(ch, SNMP_CHANNEL_TIMER_CP_RELOAD, 1);
        break;

    case SNMP_CHANNEL_EV_TIMER_TRAP:
        _SNMP_Channel_Process_Traps(ch);
        break;

    default: break;
    }
}

void
_SNMP_Channel_Callback(SNMP_Channel *ch, int cb, void *data)
{
    if (ch == NULL || cb < 0 || cb >= SNMP_CHANNEL_CB_MAX)
    {
       return;
    }

    if (ch->Callbacks[cb] != NULL)
    {
        ch->Callbacks[cb](ch, cb, data);
    }
}

void
_SNMP_Channel_Process_Traps(SNMP_Channel *pCh)
{
    SNMP_ErrCode        Err;
    ErrCode             Ret;
    EntityPtr           pEnt;

    assert(pCh != NULL);

    if (pCh == NULL)
    {
        return;
    }

    // process each entry from the list
    while (1)
    {
        pEnt = NULL;

        X_Mutex_Lock(pCh->IntMutex);

        if (X_List_Size(pCh->TrapEvList) > 0)
        {
            X_List_Remove(pCh->TrapEvList, X_List_Head(pCh->TrapEvList), (void**)&pEnt);
        }
        else
        {
            X_Mutex_Unlock(pCh->IntMutex);
            break;
        }

        X_Mutex_Unlock(pCh->IntMutex);

        Ret = _SNMP_Channel_Process_Traps_Ext(pCh, pEnt, &Err);

        if (Ret != ERRCODE_NOERROR || Err.Status != STAT_SUCCESS)
        {
           // break;
        }
    }
}

ErrCode
_SNMP_Channel_Process_Traps_Ext(SNMP_Channel *pCh, Entity *pEn, SNMP_ErrCodePtr pErr)
{
    ErrCode ret;
    EntityShelf EnShelf;
    EntityModule EnMod;
    EntityConnection EnConn;
    EntityOL EnOL;
    X_ListNodePtr pNode;

    assert(pCh != NULL && pEn != NULL && pErr != NULL);

    X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_Process_Traps_Ext(): processing trap event idx = x%08x (%d), class = %d cpclass=%d",
            pEn->Index, pEn->Index, pEn->Class, pEn->CPClass);


    /** If CP class is assigned, continue processing in dedicated function **/
    if (pEn->CPClass != 0)
    {
        return _SNMP_Channel_Process_Traps_Ext_CP(pCh, pEn, pErr);
    }

    /** TODO: EntityClass_CONTAINER? **/
    if (pEn->Class == 0) // || pEn->Class == EntityClass_CONTAINER)
    {
        // if Class field is not assigned try to retrieve class information
        if (pEn->Class == 0)
        {
            ret = SNMP_GetEntityClass(pCh->Session, pErr, pEn->Index, &pEn->Class);

            if (ret == ERRCODE_NOOBJECT)
            {
                ret = SNMP_GetCPWDMEntityClass(pCh->Session, pErr, pEn->Index, &pEn->CPClass);

                if (ret != ERRCODE_NOERROR ||
                        pErr->Status != STAT_SUCCESS ||
                        pErr->Err != SNMP_ERR_NOERROR)
                {
                    X_LOGWARN(X_LOG_CHAN_ID,
                            "SNMP_Channel_Process_Traps_Ext(): failed to retrieve CP class info (%s)",
                            GetErrMsg(ret));
                    return ret;
                }

                X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_Process_Traps_Ext(): Class retrieved: idx = x%08x (%d), class = %d cpclass=%d",
                        pEn->Index, pEn->Index, pEn->Class, pEn->CPClass);

                return _SNMP_Channel_Process_Traps_Ext_CP(pCh, pEn, pErr);
            }

            if (ret != ERRCODE_NOERROR)
            {
                X_LOGWARN(X_LOG_CHAN_ID,
                        "SNMP_Channel_Process_Traps_Ext(): failed to retrieve class info (%s)",
                        GetErrMsg(ret));
                return ret;
            }
        }

        X_LOGINFO(X_LOG_CHAN_ID, "SNMP_Channel_Process_Traps_Ext(): class retrieved: idx = x%08x (%d), class = %d cpclass=%d",
                                pEn->Index, pEn->Index, pEn->Class, pEn->CPClass);
    }

    /** Continue with processing the normal entity class **/
    switch (pEn->Class)
    {

    case EntityClass_CHASSIS:   // SHELF
        EntityShelf_Init(&EnShelf);
        Entity_Set_Index(&EnShelf, Entity_Get_Index(pEn));
        ret = SNMP_GetEntityShelf(pCh->Session, pErr, &EnShelf);
        break;

    case EntityClass_MODULE:    // MOD
    case EntityClass_FAN:       // FCU
        EntityModule_Init(&EnMod);
        Entity_Set_Index(&EnMod, Entity_Get_Index(pEn));
        ret = SNMP_GetEntityModule(pCh->Session, pErr, &EnMod);
        break;

    case EntityClass_CONNECTION: // CONN
        EntityConnection_Init(&EnConn);
        Entity_Set_Index(&EnConn, Entity_Get_Index(pEn));
        ret = SNMP_GetEntityConnection(pCh->Session, pErr, &EnConn);
        break;

    case EntityClass_OPTICALLINK: // OL
        EntityOL_Init(&EnOL);
        Entity_Set_Index(&EnOL, Entity_Get_Index(pEn));
        ret = SNMP_GetEntityOL(pCh->Session, pErr, &EnOL);
        break;


    default:
        // ignore other types
        return ERRCODE_NOERROR;
    }

    if (ret != ERRCODE_NOERROR ||
            pErr->Status != STAT_SUCCESS)
    {
        X_LOGINFO(X_LOG_CHAN_ID, "Failed to load entity data");
        return ret;
    }

    if (!(pErr->Err == SNMP_ERR_NOSUCHNAME || pErr->Err == SNMP_ERR_NOERROR))
    {
        X_LOGERR(X_LOG_CHAN_ID, "SNMP error occured");
        return ret;
    }

    if (pErr->Err == SNMP_ERR_NOSUCHNAME)
    {
        switch (pEn->Class)
        {
        case EntityClass_CHASSIS:
            pNode = X_List_Find(pCh->Inventory.ShelfList, pEn,
                    (X_ListCompare_t)CompareByIndex);
            if (pNode != NULL)
            {
                X_List_Destroy(pCh->Inventory.ShelfList, pNode);
            }
            break;

        case EntityClass_MODULE:
        case EntityClass_FAN: // FCU
            X_LOGINFO(0, "MOD destroyed");
            pNode = X_List_Find(pCh->Inventory.ModuleList, pEn, (X_ListCompare_t)CompareByIndex);
            if (pNode != NULL)
            {
                _SNMP_Channel_Callback(pCh, SNMP_CHANNEL_CB_MOD_DESTROYED, pNode->Data);
                X_List_Destroy(pCh->Inventory.ModuleList, pNode);
            }
            break;

        case EntityClass_CONNECTION:
            X_LOGINFO(0, "CONN destroyed");
            pNode = X_List_Find(pCh->Inventory.ConnectionList, pEn, (X_ListCompare_t)CompareByIndex);
            if (pNode != NULL)
            {
                _SNMP_Channel_Callback(pCh, SNMP_CHANNEL_CB_CONN_DESTROYED, pNode->Data);
                X_List_Destroy(pCh->Inventory.ConnectionList, pNode);
            }
            break;

        case EntityClass_OPTICALLINK:
            pNode = X_List_Find(pCh->Inventory.OLList, pEn, (X_ListCompare_t)CompareByIndex);
            if (pNode != NULL)
            {
                _SNMP_Channel_Callback(pCh, SNMP_CHANNEL_CB_OL_DESTROYED, pNode->Data);
                X_List_Destroy(pCh->Inventory.OLList, pNode);
            }
            break;
        }
    }
    else
    {
        switch (pEn->Class)
        {
        case EntityClass_CHASSIS:
            pNode = X_List_Find(pCh->Inventory.ShelfList, pEn, (X_ListCompare_t)CompareByIndex);
            if (pNode == NULL)
            {
                X_List_Append(pCh->Inventory.ShelfList, EntityShelf_Dup(&EnShelf));
            }
            break;

        case EntityClass_MODULE:
        case EntityClass_FAN: // FCU
            pNode = X_List_Find(pCh->Inventory.ModuleList, pEn, (X_ListCompare_t)CompareByIndex);
            if (pNode == NULL)
            {
                _SNMP_Channel_Callback(pCh, SNMP_CHANNEL_CB_MOD_CREATED, &EnMod);
                X_List_Append(pCh->Inventory.ModuleList, EntityModule_Dup(&EnMod));
            }
            else
            {
                _SNMP_Channel_Callback(pCh, SNMP_CHANNEL_CB_MOD_UPDATED, &EnMod);
            }
            break;

        case EntityClass_CONNECTION:
            pNode = X_List_Find(pCh->Inventory.ConnectionList, pEn, (X_ListCompare_t)CompareByIndex);
            if (pNode == NULL)
            {
                X_LOGINFO(0, "CONN created!");
                _SNMP_Channel_Callback(pCh, SNMP_CHANNEL_CB_CONN_CREATED, &EnConn);
                X_List_Append(pCh->Inventory.ConnectionList, EntityConnection_Dup(&EnConn));
            }
            break;

        case EntityClass_OPTICALLINK:
            pNode = X_List_Find(pCh->Inventory.OLList, pEn, (X_ListCompare_t)CompareByIndex);
            if (pNode == NULL)
            {
                _SNMP_Channel_Callback(pCh, SNMP_CHANNEL_CB_OL_CREATED, &EnOL);
                X_List_Append(pCh->Inventory.OLList, EntityOL_Copy(&EnOL));
            }
            break;
        }
    }

    return ret;
}

ErrCode
_SNMP_Channel_Process_Traps_Ext_CP(SNMP_Channel *pCh, Entity *pEn, SNMP_ErrCodePtr pErr)
{
    ErrCode ret;
    EntityTunnel EntTnl;
    EntityCNX EntCNX;
    X_ListNodePtr pNode;

    if (pCh == NULL || pEn == NULL)
    {
        //?
        return ERRCODE_NOERROR;
    }

    X_LOGDBG(X_LOG_CHAN_ID, "_SNMP_Channe_ProcessTrapsExtCP(): Index=%08x Class = %d CP-Class=%d",
            pEn->Index, pEn->Class, pEn->CPClass);

    switch (pEn->CPClass)
    {
    case CPEntityClass_TUNNEL:
        EntityTunnel_Init(&EntTnl);
        Entity_Set_Index(&EntTnl, pEn->Index);
        ret = SNMP_GetEntityTunnelWDM(pCh->Session, pErr, &EntTnl);
        break;

    case CPEntityClass_CONNECTION:
        EntityCNX_Init(&EntCNX);
        Entity_Set_Index(&EntCNX, pEn->Index);
        ret = SNMP_GetEntityCnxWDM(pCh->Session, pErr, &EntCNX);
        break;

    default:
        // for now ignore other CP entities
        return ERRCODE_NOERROR;
    }

    if (ret != ERRCODE_NOERROR || pErr->Status != STAT_SUCCESS)
    {
        return ret;
    }

    if (pErr->Err == SNMP_ERR_NOSUCHNAME)
    {
        switch (pEn->CPClass)
        {
        case CPEntityClass_TUNNEL:
            pNode = X_List_Find(pCh->InventoryCP.TunnelWDMList,&EntTnl,(X_ListCompare_t)CompareByIndex);
            if (pNode != NULL)
            {
                //X_LOGINFO(0, "TNL_WDM destroyed");

                // remove CNXs....

#if 0
                pNodeCnx = NULL;
                while (0 == FindCNX(pCh->InventoryCP.CnxWDMList, pNode->Data, &pNodeCnx))
                {
                    X_List_Destroy(pCh->InventoryCP.CnxWDMList, pNodeCnx);
                    pNodeCnx = NULL;
                }
#endif
                //

                ///
                _SNMP_Channel_Callback(pCh,SNMP_CHANNEL_CB_TNL_WDM_DESTROYED,pNode->Data);
                X_List_Destroy(pCh->InventoryCP.TunnelWDMList, pNode);


            }
            break;

        case CPEntityClass_CONNECTION:
            pNode = X_List_Find(pCh->InventoryCP.CnxWDMList,&EntCNX,(X_ListCompare_t)CompareByIndex);
            if (pNode != NULL)
            {
                //X_LOGINFO(0, "CNX_WDM removed");
                _SNMP_Channel_Callback(pCh,SNMP_CHANNEL_CB_CNX_WDM_DESTROYED,pNode->Data);
                X_List_Destroy(pCh->InventoryCP.CnxWDMList, pNode);
            }
            break;

        default:
            break;
        }
    }
    else if (pErr->Err == SNMP_ERR_NOERROR)
    {
        switch (pEn->CPClass)
        {
        case CPEntityClass_TUNNEL:
            pNode = X_List_Find(pCh->InventoryCP.TunnelWDMList,&EntTnl,(X_ListCompare_t)CompareByIndex);
            if (pNode == NULL)
            {
                X_List_Append(pCh->InventoryCP.TunnelWDMList,EntityTunnel_Dup(&EntTnl));
                _SNMP_Channel_Callback(pCh, SNMP_CHANNEL_CB_TNL_WDM_CREATED,&EntTnl);
            }
            else
            {
                X_LOGINFO(0, "<<< TNL_WDM updated >>>");
                /// TODO: fix it
                ((EntityTunnel*)pNode->Data)->Admin = EntTnl.Admin;
                ///...

                _SNMP_Channel_Callback(pCh, SNMP_CHANNEL_CB_TNL_WDM_ADMIN_CHANGED,pNode->Data);
            }
            break;

        case CPEntityClass_CONNECTION:
            pNode = X_List_Find(pCh->InventoryCP.CnxWDMList,&EntCNX,(X_ListCompare_t)CompareByIndex);
            if (pNode == NULL)
            {
                X_List_Append(pCh->InventoryCP.CnxWDMList,EntityCNX_Copy(&EntCNX));
                _SNMP_Channel_Callback(pCh, SNMP_CHANNEL_CB_CNX_WDM_CREATED, &EntCNX);
            }
            else
            {
                ///...
            }
            break;
            default: break;
        }
    }

    return ERRCODE_NOERROR;
}

/********/

const char*
SNMP_Channel_GetStateName(int state)
{
    switch (state)
    {
    case SNMP_CHANNEL_STATE_NOT_INIT:
        return "STATE_NOT_INIT";
    case SNMP_CHANNEL_STATE_DISCONNECTED:
        return "STATE_DISCONNECTED";
    case SNMP_CHANNEL_STATE_SYNCH:
        return "STATE_SYNCH";
    case SNMP_CHANNEL_STATE_READY:
        return "STATE_READY";
    default: break;
    }
    return "STATE_???";
}

void
SNMP_Channel_ThreadProc(void *arg)
{
    SNMP_Channel *pCh = (SNMP_Channel*)arg;

    if (pCh == NULL)
    {
        X_LOGWARN(0, "SNMP_Channel_ThreadProc: invalid pointer!");
        return;
    }

    SNMP_Channel_Run(pCh);
}


/******************************************************************************/

#define SNMP_CHANNEL_TASK_KEY (1)

SNMP_Channel_TaskPtr
SNMP_Channel_Task_New(SNMP_Channel_Task_t Type, SNMP_ChannelPtr pChannel)
{
    SNMP_Channel_TaskPtr pTask = (SNMP_Channel_TaskPtr)(malloc(sizeof(SNMP_Channel_Task)));

    if (pTask == NULL)
    {
        return NULL;
    }

    pTask->Task.Key = SNMP_CHANNEL_TASK_KEY;
    pTask->Task.Free = (X_Task_Free_t)SNMP_Channel_Task_Free;
    pTask->Task.Dispatch = (X_Task_Dispatch_t)SNMP_Channel_Task_Dispatch;
    pTask->Type = Type;
    pTask->Channel = pChannel;

    return pTask;
}

void
SNMP_Channel_Task_Free(SNMP_Channel_TaskPtr pTask)
{
    if (pTask != NULL)
        free(pTask);
}

ErrCode
SNMP_Channel_Schedule_Task(SNMP_ChannelPtr pCh, SNMP_Channel_Task_t type, int ev)
{
    SNMP_Channel_TaskPtr pTask;

    if (pCh == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pTask = SNMP_Channel_Task_New(type, pCh);

    if (pTask == NULL)
    {
        return ERRCODE_NOMEM;
    }

    pTask->Event.Type = ev;

    if (pCh->TaskQueue != NULL)
    {
        X_Queue_Put(pCh->TaskQueue, (void*)pTask);
    }

    return ERRCODE_NOERROR;
}

void
SNMP_Channel_Remove_Pending_Tasks(SNMP_ChannelPtr pCh)
{
    X_QueuePtr pTq;
    X_ListNodePtr pNode;
    SNMP_Channel_TaskPtr pTask;

    if (pCh == NULL || pCh->TaskQueue == NULL) return;

    pTq = pCh->TaskQueue;

    X_Mutex_Lock(pTq->Mutex);

    pNode = X_List_Head(pTq->Queue);

    while (pNode != NULL)
    {
        pTask = (SNMP_Channel_TaskPtr)pNode->Data;

        if (pTask->Task.Key == SNMP_CHANNEL_TASK_KEY && pTask->Channel == pCh)
        {
            pNode = X_List_Destroy(pTq->Queue, pNode);
            continue;
        }

        pNode = pNode->Next;
    }

    X_Mutex_Unlock(pTq->Mutex);
}

void
SNMP_Channel_Task_Dispatch(SNMP_Channel_TaskPtr pTask)
{
    SNMP_ChannelPtr pChannel;

    if(pTask == NULL || pTask->Channel == NULL)
    {
        //// ?
        return;
    }

    pChannel = pTask->Channel;

    SNMP_Channel_Lock(pChannel);

    SNMP_Channel_Dispatch(pChannel, &pTask->Event);

    SNMP_Channel_Unlock(pChannel);
}
