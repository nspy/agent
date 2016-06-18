
#include "Lightpath.h"

#if 1
#define DBG(a, b...) fprintf(stderr, "LIGTHPATH-DBG: "a"\r\n", ##b);
#else
#define DBG(a, b...)
#endif

/******************************************************************************/

static int REQ_ID = 0;

/******************************************************************************/

const char*
GMPLS_Request_Result_Description(GMPLS_Request_Result_t result)
{
    switch (result)
    {
    case GMPLS_REQ_RESULT_SUCCESS:
        return "Success";

    case GMPLS_REQ_RESULT_NO_CONNECTION:
        return "No connection";

    case GMPLS_REQ_RESULT_NOT_FOUND:
        return "Not found";

    case GMPLS_REQ_RESULT_TIMEOUT:
        return "Timeout";

    case GMPLS_REQ_RESULT_OTHER_ERROR:
        return "Other error";

    default:
        break;
    }

    return "???";
}

GMPLS_RequestPtr
GMPLS_Request_New(void)
{
    GMPLS_RequestPtr pReq = (GMPLS_RequestPtr)malloc(sizeof(GMPLS_Request));

    if (pReq != NULL)
    {
        _GMPLS_Request_Init(pReq);
    }

    return pReq;
}

GMPLS_RequestPtr
GMPLS_Request_Create(GMPLS_Request_Type_t type, const char *ToIP, long FromAid, long ToAid)
{
    GMPLS_RequestPtr pReq = GMPLS_Request_New();

    if (pReq == NULL)
    {
        return NULL;
    }

    pReq->Type = type;

    strncpy(pReq->ToIP, ToIP, sizeof(pReq->ToIP));

    pReq->FromAid = FromAid;

    pReq->ToAid = ToAid;

    return pReq;
}

GMPLS_RequestPtr
GMPLS_Request_Create2(GMPLS_Request_Type_t type,
        const char *agent,
        const char *community,
        const char *FromIP,
        const char *ToIP,
        long FromAid,
        long ToAid)
{
    GMPLS_RequestPtr pReq = GMPLS_Request_Create(type,ToIP,FromAid,ToAid);
    if (pReq != NULL)
        GMPLS_Request_Set_SNMP(pReq, agent, community);
    return pReq;
}

void
GMPLS_Request_Set_Aux_Data(GMPLS_RequestPtr pReq,
        unsigned long long dpid1, unsigned short port1,
        unsigned long long dpid2, unsigned short port2)
{
    if (pReq == NULL)
        return;

    pReq->Dpid[0] = dpid1;
    pReq->Dpid[1] = dpid2;
    pReq->Port[0] = port1;
    pReq->Port[1] = port2;
}

int
GMPLS_Request_Clear_Path(GMPLS_RequestPtr pReq)
{
    if (pReq == NULL)
        return -1;

    if (pReq->Path != NULL)
    {
        X_List_Clear(pReq->Path);
    }

    return 0;
}

int
GMPLS_Request_Add_Path_Object(GMPLS_RequestPtr pReq, const CrossConnect *Crs)
{
    if (pReq == NULL || Crs == NULL)
        return -1;

    if (pReq->Path == NULL)
    {
        pReq->Path = X_List_New((X_ListMemFree_t)CrossConnect_Free);
    }

    X_List_Append(pReq->Path, CrossConnect_Dup(Crs));

    return 0;
}

void
GMPLS_Request_Set_SNMP(GMPLS_RequestPtr pReq, const char *agent, const char *community)
{
    if (pReq == NULL || agent == NULL || community == NULL)
    {
        return;
    }

    if (pReq->Agent != NULL)
    {
        free(pReq->Agent);
    }
    pReq->Agent = strdup(agent);

    if (pReq->Community != NULL)
    {
        free(pReq->Community);
    }
    pReq->Community = strdup(community);
}

/**
 * Set user data.
 */
void
GMPLS_Request_Set_User(GMPLS_RequestPtr pReq, void *data)
{
    if (pReq != NULL)
        pReq->UserData = data;
}

void*
GMPLS_Request_Get_User(GMPLS_RequestPtr pReq)
{
    return (pReq != NULL) ? pReq->UserData : NULL;
}

void
GMPLS_Request_Free(GMPLS_RequestPtr pReq)
{
    if (pReq == NULL)
    {
        return;
    }

    if (pReq->Session != NULL)
    {
        snmp_sess_close(pReq->Session);
        pReq->Session = NULL;
    }

    if (pReq->Agent != NULL)
    {
        free(pReq->Agent);
    }

    if (pReq->Community != NULL)
    {
        free(pReq->Community);
    }

    if (pReq->Path != NULL)
    {
        X_List_Free(pReq->Path);
    }

    free (pReq);
}

int
_GMPLS_Request_Init(GMPLS_RequestPtr pReq)
{
    if (pReq == NULL)
    {
        return (-1);
    }

    pReq->Id = ++REQ_ID;

    if (REQ_ID >= 10000) REQ_ID = 0;

    pReq->Controller = NULL;

    pReq->Session = NULL;

    X_Timer_Init(&pReq->Timer);

    X_Timer_Init(&pReq->ProcessingTimer);

    X_Timer_Init(&pReq->EqualizationTimer);

    X_Timer_Init(&pReq->GuardTimer);

    pReq->ProcessingTime = 0;

    pReq->EqualizationTime = 0;

    pReq->PwrEqState = 0;

    pReq->State = GMPLS_REQ_STATE_NOT_INITIALIZED;

    pReq->Type = GMPLS_REQ_TYPE_TEAR_DOWN;

    pReq->Result = GMPLS_REQ_RESULT_SUCCESS;

    pReq->Agent = NULL;

    pReq->Community = NULL;

    pReq->FromIP[0] = '\0';

    pReq->ToIP[0] = '\0';

    pReq->FromAid = pReq->ToAid = 0;

    pReq->Index = 0;

    pReq->TunnelId[0] = '\0';

    pReq->PathIndex = 0;

    pReq->CnxIndex = 0;

    pReq->Path = NULL;

    pReq->UserData = NULL;

    pReq->Dpid[0] = pReq->Dpid[1] = 0;
    pReq->Port[0] = pReq->Port[1] = 0;

    return (0);
}

int
_GMPLS_Request_Create_Session(GMPLS_RequestPtr pReq)
{
    netsnmp_session Session;

    if (pReq == NULL)
    {
        return (-1);
    }

    DBG("agent: %s, community: %s", pReq->Agent, pReq->Community);

    snmp_sess_init(&Session);

    Session.version = SNMP_VERSION_1;

    Session.peername = pReq->Agent;

    Session.community = (u_char*) pReq->Community;

    Session.community_len = strlen(pReq->Community);

    pReq->Session = (netsnmp_session*)snmp_sess_open(&Session);

    return (pReq->Session != NULL);
}

int
_GMPLS_Request_Process(GMPLS_RequestPtr pReq)
{
    assert (pReq != NULL);

    if (pReq == NULL)
    {
        return (-1);
    }

    // !!!
    if ( X_Timer_Update(&pReq->GuardTimer) != 0 )
    {
        DBG("Total processing time for this request has been exceeded!!!");
        X_Timer_Stop(&pReq->GuardTimer);
        pReq->Result = GMPLS_REQ_RESULT_TIMEOUT;
        pReq->State = GMPLS_REQ_STATE_FINALIZED;
        return 0;
    }

    DBG("Processing GMPLS_Request id = %d", pReq->Id);

    if (pReq->Type == GMPLS_REQ_TYPE_SETUP)
    {
        _GMPLS_Request_Process_Setup(pReq);
    }
    else if (pReq->Type == GMPLS_REQ_TYPE_TEAR_DOWN)
    {
        _GMPLS_Request_Process_Teardown(pReq);
    }
    else if (pReq->Type == GMPLS_REQ_TYPE_PATH_COMPUTATION)
    {

    }
    else
    {
         /// ?
    }

    return (0);
}

void
_GMPLS_Request_Schedule_Task(GMPLS_RequestPtr pReq)
{
    X_TaskPtr pTask;

    if (pReq == NULL || pReq->Controller == NULL)
    {
        return;
    }

    pTask = X_Task_New();

    pTask->Dispatch = GMPLS_Controller_Dispatch;

    pTask->Data = pReq;

    X_Queue_Put(pReq->Controller->Queue, pTask);
}

void
_GMPLS_Request_Tick(GMPLS_RequestPtr pReq)
{
    if (X_Timer_Update(&pReq->Timer))
    {
        X_Timer_Stop(&pReq->Timer);
        _GMPLS_Request_Schedule_Task(pReq);
    }
}

/** Internal processing of the SETUP request **/
void
_GMPLS_Request_Process_Setup(GMPLS_RequestPtr pReq)
{
    EntityTunnel        Tnl;
    EntityTunnelResult  TnlResult;
    EntityPath          PathEntry;
    ErrCode             Err;
    EntityCNX           Cnx;

    //X_ListNodePtr       pNode;
    X_ListPtr           NumLinks;
    X_ListPtr           UnnumLinks;
    X_ListPtr           Path;
    //char                Tmp[128];

    assert (pReq != NULL);
    if (pReq == NULL) return;

    switch (pReq->State)
    {
    case GMPLS_REQ_STATE_NOT_INITIALIZED:

        _GMPLS_Request_Create_Session(pReq);

        X_Timer_Start(&pReq->ProcessingTimer, 1000);
        X_Timer_Start(&pReq->EqualizationTimer, 1000);

        EntityTunnel_Init(&Tnl);

        inet_aton(pReq->ToIP,(struct in_addr*) &Tnl.ToIP);

        Tnl.ToAid = pReq->ToAid;
        Tnl.FromAid = pReq->FromAid;

        // store tunnel id
        snprintf(pReq->TunnelId,sizeof(pReq->TunnelId),"T_%d",(int) time(NULL));
        strncpy(Tnl.Id,pReq->TunnelId,sizeof(Tnl.Id));

        //////

        if ((Tnl.FromAid & 0xff000000) >> 24 == 0x45)
        {
            Tnl.Facility = FacilityType_IFTYPE10GBE;
        }

        /////

        Err = SNMP_CreateTunnel(pReq->Session,&pReq->SNMPErr,&Tnl);

        if (Err != ERRCODE_NOERROR ||
                pReq->SNMPErr.Status != STAT_SUCCESS ||
                pReq->SNMPErr.Err != SNMP_ERR_NOERROR)
        {
            DBG("Can't create TNL_WDM entity...");
            _GMPLS_Request_Stop_Processing_Timer(pReq);
            pReq->Result = GMPLS_REQ_RESULT_OTHER_ERROR;
            pReq->State = GMPLS_REQ_STATE_FINALIZED;
            return;
        }

        //////
        if (pReq->Path != NULL && pReq->Path->Size > 0)
        {
            NumLinks = X_List_New(EntityFree);
            UnnumLinks = X_List_New(EntityFree);
            Path = X_List_New(EntityFree);

            SNMP_GetEntityTENumLinkList(pReq->Session, &pReq->SNMPErr, NumLinks);
            SNMP_GetEntityTEUnnumLinkList(pReq->Session, &pReq->SNMPErr, UnnumLinks);

            ConvertPathDescriptionToGMPLS(NumLinks, UnnumLinks, pReq->Path, Path);

            EntityPath_Init(&PathEntry);
            strncpy(PathEntry.PathID, Tnl.Id, sizeof(PathEntry.PathID));

            SNMP_CreatePath(pReq->Session, &pReq->SNMPErr, &PathEntry, Path);

            SNMP_AttachPath(pReq->Session, &pReq->SNMPErr,
                    Entity_Get_Index(&Tnl), Entity_Get_Index(&PathEntry), 0);

            X_List_Free(Path);
            X_List_Free(NumLinks);
            X_List_Free(UnnumLinks);
        }

        //////

        Err = SNMP_SetTunnelAdmin(pReq->Session, &pReq->SNMPErr, &Tnl, AdminState_IS);

        if (Err != ERRCODE_NOERROR ||
                pReq->SNMPErr.Status != STAT_SUCCESS ||
                pReq->SNMPErr.Err != SNMP_ERR_NOERROR)
        {
            DBG("Can't change TNL_WDM entity admin state to IS...");
            _GMPLS_Request_Stop_Processing_Timer(pReq);
            // try to remove tunnel entry
            if (pReq->SNMPErr.Status != STAT_TIMEOUT)
            {
                SNMP_DestroyTunnel(pReq->Session, &pReq->SNMPErr, Entity_Get_Index(&Tnl));
            }
            // failure when setting IS on tunnel
            pReq->Result = GMPLS_REQ_RESULT_OTHER_ERROR;
            pReq->State = GMPLS_REQ_STATE_FINALIZED;
            return;
        }

        pReq->Index = Entity_Get_Index(&Tnl);
        pReq->State = GMPLS_REQ_STATE_IN_PROGRESS;
        X_Timer_Start(&pReq->Timer, 500);
        break;

    case GMPLS_REQ_STATE_IN_PROGRESS:
        EntityTunnel_Init(&Tnl);
        Entity_Set_Index(&Tnl, pReq->Index);
        strcpy(Tnl.Id, pReq->TunnelId);

        Err = SNMP_GetEntityTunnelWDM(pReq->Session, &pReq->SNMPErr, &Tnl);

        if (Err != ERRCODE_NOERROR ||
                pReq->SNMPErr.Status != STAT_SUCCESS ||
                pReq->SNMPErr.Err != SNMP_ERR_NOERROR)
        {
            DBG("Can't retrieve TNL_WDM entity admin state...");
            _GMPLS_Request_Stop_Processing_Timer(pReq);
            pReq->State = GMPLS_REQ_STATE_FINALIZED;
            pReq->Result = GMPLS_REQ_RESULT_OTHER_ERROR;
            return;
        }

        if (Tnl.Admin == AdminState_IS)
        {
            //_GMPLS_Request_Stop_Processing_Timer(pReq);
            X_Timer_Update(&pReq->ProcessingTimer);
            X_Timer_Stop(&pReq->ProcessingTimer);
            pReq->ProcessingTime = (unsigned int)pReq->ProcessingTimer.Elapsed;

            _GMPLS_Fetch_Path(pReq);

            EntityCNX_Init(&Cnx);
            Err = SNMP_GetCnxWDMForTunnel(pReq->Session, &pReq->SNMPErr, &Tnl, &Cnx);

            if (Err != ERRCODE_NOERROR ||
                    pReq->SNMPErr.Status != STAT_SUCCESS)
            {
                DBG("Can't get CNX_WDM for tunnel...");
                _GMPLS_Request_Stop_Processing_Timer(pReq);
                pReq->Result = GMPLS_REQ_RESULT_OTHER_ERROR;
                pReq->State = GMPLS_REQ_STATE_FINALIZED;
                return;
            }

            pReq->CnxIndex = Entity_Get_Index(&Cnx);
            pReq->State = GMPLS_REQ_STATE_EQ_IN_PROGRESS;
            X_Timer_Start(&pReq->Timer,GMPLS_REQ_POLL_TIMEOUT);
            /**
            pReq->State = GMPLS_REQ_STATE_FINALIZED;
            pReq->Result = GMPLS_REQ_RESULT_SUCCESS;
            **/
            return;
        }

        memset(&TnlResult, 0, sizeof(TnlResult));

        TnlResult.Index = pReq->Index;

        // check errors for tunnel setup
        Err = SNMP_GetTunnelResult(pReq->Session,&pReq->SNMPErr,&TnlResult);

        if (Err != ERRCODE_NOERROR ||
                pReq->SNMPErr.Status != STAT_SUCCESS ||
                pReq->SNMPErr.Err != SNMP_ERR_NOERROR)
        {
            DBG("Can't retrieve TNL_WDM entity service setup states...");
            _GMPLS_Request_Stop_Processing_Timer(pReq);
            pReq->Result = GMPLS_REQ_RESULT_OTHER_ERROR;
            pReq->State = GMPLS_REQ_STATE_FINALIZED;
            return;
        }

        DBG("Result: admin=%d sec. states =%08x comp. state=%08x errCat=%d",
                TnlResult.Admin,
                TnlResult.SecondayStates,
                TnlResult.ComputationState,
                TnlResult.ErrCategory);

        if (TnlResult.ErrCategory != ErrorCategory_SUCCESS)
        {
            DBG("TNL_WDM entity service setup failed...");
            _GMPLS_Request_Stop_Processing_Timer(pReq);
            SNMP_DestroyTunnel(pReq->Session,&pReq->SNMPErr,pReq->Index);
            pReq->State = GMPLS_REQ_STATE_FINALIZED;
            pReq->Result = GMPLS_REQ_RESULT_OTHER_ERROR;
            return;
        }
        X_Timer_Start(&pReq->Timer,GMPLS_REQ_POLL_TIMEOUT);
        break;

    case GMPLS_REQ_STATE_EQ_IN_PROGRESS:

        DBG("TNL_WDM entity equalization in progress state...(CNX_WDM index = %d)",
                (int)pReq->CnxIndex);

        EntityCNX_Init(&Cnx);
        Entity_Set_Index(&Cnx, pReq->CnxIndex);

        if (pReq->CnxIndex == 0)
        {
            EntityTunnel_Init(&Tnl);
            Entity_Set_Index(&Tnl, pReq->Index);
            strcpy(Tnl.Id, pReq->TunnelId);

            DBG("Trying to find matching CNX_WDM...");

            Err = SNMP_GetCnxWDMForTunnel(pReq->Session, &pReq->SNMPErr, &Tnl, &Cnx);

            if (Err != ERRCODE_NOERROR ||
                    pReq->SNMPErr.Status != STAT_SUCCESS ||
                    pReq->SNMPErr.Err != SNMP_ERR_NOERROR)
            {
                DBG("Unable to find CNX_WDM...");
                _GMPLS_Request_Stop_Processing_Timer(pReq);
                pReq->Result = GMPLS_REQ_RESULT_OTHER_ERROR;
                pReq->State = GMPLS_REQ_STATE_FINALIZED;
                return;
            }
            pReq->CnxIndex = Entity_Get_Index(&Cnx);
        }
        else
        {
            Err = SNMP_GetCnxWdmPwrEqState(pReq->Session, &pReq->SNMPErr,
                    pReq->CnxIndex, &Cnx.EqState);

            if (Err != ERRCODE_NOERROR ||
                    pReq->SNMPErr.Status != STAT_SUCCESS ||
                    pReq->SNMPErr.Err != SNMP_ERR_NOERROR)
            {
                DBG("Unable to read equalization state.... (%s, stat=%d, err= %d)",
                        GetErrMsg(Err), pReq->SNMPErr.Status, pReq->SNMPErr.Err);
                _GMPLS_Request_Stop_Processing_Timer(pReq);
                pReq->Result = GMPLS_REQ_RESULT_OTHER_ERROR;
                pReq->State = GMPLS_REQ_STATE_FINALIZED;
                return;
            }
        }

        pReq->PwrEqState = Cnx.EqState;

        if (pReq->PwrEqState == 3) // inprogress
        {
            X_Timer_Start(&pReq->Timer,GMPLS_REQ_POLL_TIMEOUT);
            return;
        }

        _GMPLS_Request_Stop_Processing_Timer(pReq);
        pReq->State = GMPLS_REQ_STATE_FINALIZED;
        pReq->Result = GMPLS_REQ_RESULT_SUCCESS;

        break;

    default:
        break;
    }
}

/** Internal processing of the TEARDOWN request **/
void
_GMPLS_Request_Process_Teardown(GMPLS_RequestPtr pReq)
{
    EntityTunnel        Tnl;
    EntityTunnel*       PtrTnl;
    EntityTunnelResult  TnlResult;
    X_ListPtr           pTnlList;
    X_ListNodePtr       pNode;
    ErrCode             Err;
    unsigned long       Path1, Path2;

    assert (pReq != NULL);
    if (pReq == NULL) return;

    switch (pReq->State)
    {
    case GMPLS_REQ_STATE_NOT_INITIALIZED:

        _GMPLS_Request_Create_Session(pReq);

        X_Timer_Start(&pReq->ProcessingTimer, 1000);

        EntityTunnel_Init(&Tnl);

        inet_aton(pReq->ToIP,(struct in_addr*) &Tnl.ToIP);

        Tnl.ToAid = pReq->ToAid;
        Tnl.FromAid = pReq->FromAid;

        pTnlList = X_List_New((EntityFree));

        pReq->Index = 0;

        // find matching tunnel

        Err = SNMP_GetEntityTunnelWDMList(pReq->Session,&pReq->SNMPErr,pTnlList);

        if (pReq->SNMPErr.Status != STAT_SUCCESS ||
                pReq->SNMPErr.Err != SNMP_ERR_NOERROR)
        {
            _GMPLS_Request_Stop_Processing_Timer(pReq);
            // failed to obtain list of tunnels...
            pReq->State = GMPLS_REQ_STATE_FINALIZED;
            pReq->Result = GMPLS_REQ_RESULT_OTHER_ERROR;
            return;
        }

        X_List_ForEach(pTnlList, pNode)
        {
            PtrTnl = (EntityTunnel*) pNode->Data;

            if (PtrTnl->Admin == AdminState_IS && PtrTnl->FromAid == Tnl.FromAid
                    && PtrTnl->ToAid == Tnl.ToAid && PtrTnl->ToIP == Tnl.ToIP)
            {
                pReq->Index = Entity_Get_Index(PtrTnl);
                break;
            }
        }

        X_List_Free(pTnlList);

        // no tunnel found...
        if (pReq->Index == 0)
        {
            _GMPLS_Request_Stop_Processing_Timer(pReq);
            pReq->State = GMPLS_REQ_STATE_FINALIZED;
            pReq->Result = GMPLS_REQ_RESULT_NOT_FOUND;
            return;
        }

        Entity_Set_Index(&Tnl, pReq->Index);
        TnlResult.Index = pReq->Index;

        Err = SNMP_GetTunnelResult(pReq->Session, &pReq->SNMPErr, &TnlResult);

        if (Err != ERRCODE_NOERROR ||
                pReq->SNMPErr.Status != STAT_SUCCESS ||
                pReq->SNMPErr.Err != SNMP_ERR_NOERROR)
        {
            _GMPLS_Request_Stop_Processing_Timer(pReq);
            pReq->State = GMPLS_REQ_STATE_FINALIZED;
            pReq->Result = GMPLS_REQ_RESULT_OTHER_ERROR;
            return;
        }

        DBG("Result: admin=%d sec. states =%08x comp. state=%08x errCat=%d",
                TnlResult.Admin,
                TnlResult.SecondayStates,
                TnlResult.ComputationState,
                TnlResult.ErrCategory);

        Err = SNMP_SetTunnelAdmin(pReq->Session,&pReq->SNMPErr,&Tnl,AdminState_PPS);

        if (Err != ERRCODE_NOERROR ||
                pReq->SNMPErr.Status != STAT_SUCCESS ||
                pReq->SNMPErr.Err != SNMP_ERR_NOERROR)
        {
            _GMPLS_Request_Stop_Processing_Timer(pReq);
            pReq->State = GMPLS_REQ_STATE_FINALIZED;
            pReq->Result = GMPLS_REQ_RESULT_OTHER_ERROR;
            return;
        }

        X_Timer_Start(&pReq->Timer,1000);

        pReq->State = GMPLS_REQ_STATE_IN_PROGRESS;

        break;

    case GMPLS_REQ_STATE_IN_PROGRESS:

        memset(&TnlResult,0,sizeof(TnlResult));

        TnlResult.Index = pReq->Index;

        Err = SNMP_GetTunnelResult(pReq->Session,&pReq->SNMPErr,&TnlResult);

        if (pReq->SNMPErr.Status != STAT_SUCCESS ||
                pReq->SNMPErr.Err != SNMP_ERR_NOERROR)
        {
            _GMPLS_Request_Stop_Processing_Timer(pReq);
            pReq->State = GMPLS_REQ_STATE_FINALIZED;
            pReq->Result = GMPLS_REQ_RESULT_OTHER_ERROR;
            return;
        }

        DBG("Result: admin=%d sec. states =%08x comp. state=%08x errCat=%d",
                TnlResult.Admin,
                TnlResult.SecondayStates,
                TnlResult.ComputationState,
                TnlResult.ErrCategory);

        if (TnlResult.Admin == AdminState_PPS)
        {
            _GMPLS_Request_Stop_Processing_Timer(pReq);

            // get prov. paths
            Path1 = Path2 = 0;
            Err = SNMP_GetProvPath(pReq->Session,&pReq->SNMPErr,pReq->Index,&Path1,&Path2);

            // delete paths
            Err = SNMP_AttachPath(pReq->Session,&pReq->SNMPErr,pReq->Index,0,0);

            if (Path1 != 0)
            {
                SNMP_DeletePath(pReq->Session,&pReq->SNMPErr,Path1);
            }

            if (Path2 != 0)
            {
                SNMP_DeletePath(pReq->Session,&pReq->SNMPErr,Path2);
            }

            SNMP_DestroyTunnel(pReq->Session,&pReq->SNMPErr,pReq->Index);

            pReq->State = GMPLS_REQ_STATE_FINALIZED;
            pReq->Result = GMPLS_REQ_RESULT_SUCCESS;
            return;
        }

        X_Timer_Start(&pReq->Timer,GMPLS_REQ_POLL_TIMEOUT);
        break;

    default:
        break;
    }
}

void
_GMPLS_Request_Stop_Processing_Timer(GMPLS_RequestPtr pReq)
{
    if (pReq == NULL)
        return;

    X_Timer_Update(&pReq->ProcessingTimer);
    X_Timer_Stop(&pReq->ProcessingTimer);
    pReq->ProcessingTime = (unsigned int)pReq->ProcessingTimer.Elapsed;

    X_Timer_Update(&pReq->EqualizationTimer);
    X_Timer_Stop(&pReq->EqualizationTimer);
    pReq->EqualizationTime = (unsigned int)pReq->EqualizationTimer.Elapsed;
}

/** Fetch signalled path from the NE **/
int
_GMPLS_Fetch_Path(GMPLS_RequestPtr pReq)
{
    EntityTunnel        EntTnl;
    EntityCNX           EntCnx;
    netsnmp_session*    Session;
    X_ListPtr           PathList;
    X_ListPtr           TENumLinks, TEUnnumLinks;

    X_ListNodePtr       pNode;
    EntityPathElement   *pel;
    char                buf[128];

    if (pReq == NULL)
    {
        return (-1);
    }


    Session = pReq->Session;

    EntityTunnel_Init(&EntTnl);
    EntityCNX_Init(&EntCnx);

    PathList = X_List_New(EntityFree);

    if (pReq->Path == NULL)
    {
        pReq->Path = X_List_New(CrossConnect_Free);
    }
    else
    {
        X_List_Clear(pReq->Path);
    }

    TENumLinks = X_List_New(EntityFree);
    TEUnnumLinks = X_List_New(EntityFree);

    Entity_Set_Index(&EntTnl, pReq->Index);

    EntityTunnel_Set_Id(&EntTnl, pReq->TunnelId);

    SNMP_GetCnxWDMForTunnel(Session, &pReq->SNMPErr, &EntTnl, &EntCnx);

    SNMP_GetSignalledPath(Session,&pReq->SNMPErr, &EntCnx, PathList);

    X_List_ForEach(PathList, pNode)
    {
        pel = (EntityPathElement*)pNode->Data;

        EntityPathElement_Dump(pel, buf, sizeof(buf));
        DBG("\t%s", buf);
    }


    SNMP_GetEntityTENumLinkList(Session, &pReq->SNMPErr, TENumLinks);
    SNMP_GetEntityTEUnnumLinkList(Session, &pReq->SNMPErr, TEUnnumLinks);

    ConvertPathDescriptionFromGMPLS(TENumLinks, TEUnnumLinks, PathList, pReq->Path);

    X_List_Free(PathList);

    return (0);
}

/******************************************************************************/

int
GMPLS_Controller_Init(GMPLS_ControllerPtr pCtrl)
{
    X_WorkerPtr pWorker;
    int i;

    if (pCtrl == NULL)
    {
        return (-1);
    }

    pCtrl->Queue = X_Queue_New(NULL);

    pCtrl->Requests = X_List_New(NULL);

    pCtrl->Workers = X_List_New(NULL);

    for(i = 0; i < GMPLS_LIB_WORKERS; ++i)
    {
        pWorker = X_Worker_New(pCtrl->Queue);

        X_List_Append(pCtrl->Workers, pWorker);
    }

    pCtrl->Timer = X_Worker_New(NULL);

    X_Ticker_Init(&pCtrl->Ticker, (X_Ticker_Tick_t)GMPLS_Controller_Tick, pCtrl);

    X_Worker_Add_Ticker(pCtrl->Timer, &pCtrl->Ticker);

    pCtrl->Mutex = X_Mutex_New();

    pCtrl->Callbacks = X_List_New(NULL);

    return (0);
}

void
GMPLS_Controller_Uninit(GMPLS_ControllerPtr pCtrl)
{
    X_WorkerPtr pWorker;

    if (pCtrl == NULL)
    {
        return;
    }

    X_List_Free(pCtrl->Requests);

    while (X_List_Size(pCtrl->Workers) > 0)
    {
        X_List_Remove(pCtrl->Workers, X_List_Head(pCtrl->Workers), (void**)&pWorker);

        X_Worker_Free(pWorker);
    }
    X_List_Free(pCtrl->Workers);

    X_Worker_Free(pCtrl->Timer);

    X_Mutex_Free(pCtrl->Mutex);

    X_List_Free(pCtrl->Callbacks);
}

/** Start processing (threads) **/
int
GMPLS_Controller_Start(GMPLS_ControllerPtr pCtrl)
{
    X_ListNodePtr pNode;
    X_WorkerPtr pWorker;

    if (pCtrl == NULL)
    {
        return (-1);
    }

    X_List_ForEach(pCtrl->Workers, pNode)
    {
        pWorker = (X_WorkerPtr)(pNode->Data);
        X_Worker_Start(pWorker, 0);
    }

    X_Worker_Start(pCtrl->Timer, 0);

    return (0);
}

void
GMPLS_Controller_Stop(GMPLS_ControllerPtr pCtrl)
{
    X_ListNodePtr pNode;
    X_WorkerPtr pWorker;

    if (pCtrl == NULL)
    {
        return;
    }

    X_List_ForEach(pCtrl->Workers, pNode)
    {
        pWorker = (X_WorkerPtr)pNode->Data;
        X_Worker_Wait(pWorker);
    }

    X_Worker_Wait(pCtrl->Timer);
}

void
GMPLS_Controller_Add_Callback(GMPLS_ControllerPtr pCtrl, GMPLS_Request_Finalize_t f)
{
    if (pCtrl == NULL || f == NULL)
    {
        return;
    }
    X_Mutex_Lock(pCtrl->Mutex);
    X_List_Append(pCtrl->Callbacks, f);
    X_Mutex_Unlock(pCtrl->Mutex);
}

void
GMPLS_Controller_Tick(GMPLS_ControllerPtr pCtrl)
{
    X_ListNodePtr pNode, pNode2, pNode3;
    GMPLS_RequestPtr pReq;
//    GMPLS_Request_Finalize_t Cb;

    X_Mutex_Lock(pCtrl->Mutex);


    // update all requests
    X_List_ForEach(pCtrl->Requests, pNode)
    {
        _GMPLS_Request_Tick((GMPLS_RequestPtr) pNode->Data);
    }

    // check if there'are finalized requests

    pNode = X_List_Head(pCtrl->Requests);

    while(pNode != NULL)
    {
        pReq = (GMPLS_RequestPtr)pNode->Data;

        if (pReq->State == GMPLS_REQ_STATE_FINALIZED)
        {
            pNode2 = pNode;
            pNode = pNode2->Next;
            X_List_Remove(pCtrl->Requests, pNode2, (void**)&pReq);

            X_List_ForEach(pCtrl->Callbacks, pNode3)
            {
                if (pNode3->Data != NULL)
                {
                    ((GMPLS_Request_Finalize_t)(pNode3->Data))(pReq->Id, pReq, pReq->Result);
                }
            }

            GMPLS_Request_Free(pReq);

            continue;
        }

        pNode = pNode->Next;
    }



    X_Mutex_Unlock(pCtrl->Mutex);
}

int
GMPLS_Controller_Put(GMPLS_ControllerPtr pCtrl, GMPLS_RequestPtr pReq)
{
    if (pCtrl == NULL || pReq == NULL)
    {
        return (-1);
    }

    X_Mutex_Lock(pCtrl->Mutex);

    pReq->Controller = pCtrl;

    X_List_Append(pCtrl->Requests, pReq);

    X_Mutex_Unlock(pCtrl->Mutex);

    /** Start the guard timer **/
    X_Timer_Start(&pReq->GuardTimer, GMPLS_REQ_MAX_TIME);

    _GMPLS_Request_Schedule_Task(pReq);

    return pReq->Id;
}

void
GMPLS_Controller_Dispatch(X_TaskPtr pTask)
{
    GMPLS_RequestPtr pReq = (GMPLS_RequestPtr)pTask->Data;
    _GMPLS_Request_Process(pReq);
}
