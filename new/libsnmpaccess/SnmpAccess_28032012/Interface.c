
#include "Interface.h"
//#include "Oids.h"
#include "Utils.h"
#include "Types.h"
#include <assert.h>

/******************************************************************************/

ErrCode
SNMP_GetSysUptime(netsnmp_session *sess, SNMP_ErrCodePtr err, unsigned int *t)
{
    netsnmp_pdu *pdu = NULL, *response = NULL;
    netsnmp_variable_list *var;

    oid sysUptime_oid[] = {1, 3, 6, 1, 2, 1, 1, 3};
    size_t sysUptime_oid_size = 8;

    if (sess == NULL || err == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    if (pdu == NULL)
    {
        return ERRCODE_NOMEM;
    }

    snmp_add_null_var(pdu, sysUptime_oid, sysUptime_oid_size);

    err->Status = snmp_sess_synch_response(sess, pdu, &response);

    if (err->Status == STAT_SUCCESS && response != NULL && response->errstat == SNMP_ERR_NOERROR)
    {
        for(var = response->variables; var; var = var->next_variable)
        {
            if (!netsnmp_oid_is_subtree(sysUptime_oid,sysUptime_oid_size,
                    var->name, var->name_length))
            {
                if (t != NULL) SNMP_Var_Set_Integer(var, (int*)t);
            }
        }
    }

    if (response != NULL)
    {
        err->Err = response->errstat;
        snmp_free_pdu(response);
    }

    return ERRCODE_NOERROR;
}

/******************************************************************************/

#if 0
int
GetEntityNE(netsnmp_session *sess, int *err, EntityNE *e)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    struct variable_list *var;
    int status;

    assert(e != NULL);

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    if (pdu == NULL)
    {
        return -1;
    }


    snmp_add_null_var(pdu,neSystemIp_oid,neSystemIp_oid_size);

    snmp_add_null_var(pdu,neSystemIpMask_oid,neSystemIpMask_oid_size);

    status = snmp_sess_synch_response(sess, pdu, &response);

    if (status == STAT_SUCCESS && response != NULL && response->errstat == SNMP_ERR_NOERROR)
    {
        for(var = response->variables; var; var = var->next_variable)
        {
            if (!netsnmp_oid_is_subtree(neSystemIp_oid, neSystemIp_oid_size,
                    var->name, var->name_length))
            {
                e->SysIP = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(neSystemIpMask_oid, neSystemIpMask_oid_size,
                    var->name, var->name_length))
            {
                e->SysIPMask = *(var->val.integer);
            }
        }

        if (response != NULL)
        {
            if (err != NULL)
            {
                *err = response->errstat;
            }
            snmp_free_pdu(response);
        }
    }

    return status;
}

int
GetEntityShelf(struct snmp_session *sess, int *err, unsigned long index, EntityShelf *e)
{
    netsnmp_pdu *pdu = NULL, *response = NULL;
    netsnmp_variable_list *var;
    int status;

    if (sess == NULL || err == NULL || e == NULL)
    {
        X_LOGERR(1, "GetEntityShelf: invalid arguments");
        return -1;
    }

    pdu = snmp_pdu_create(SNMP_MSG_GET);

    if (pdu == NULL)
    {
        return -1;
    }
    snmp_add_null_var_index(pdu,deployProvShelfType_oid,
            deployProvShelfType_oid_size + 1, index);

    snmp_add_null_var_index(pdu,entityStateAdmin_oid,
            entityStateAdmin_oid_size + 1, index);

    snmp_add_null_var_index(pdu,entityStateOper_oid,
            entityStateOper_oid_size + 1, index);

    snmp_add_null_var_index(pdu,entityIndexAid_oid,
            entityIndexAid_oid_size + 1, index);

    status = snmp_sess_synch_response(sess, pdu, &response);

    if (status == STAT_SUCCESS && response != NULL && response->errstat == SNMP_ERR_NOERROR)
    {
        e->GeIndex = index;

        for(var = response->variables; var; var = var->next_variable)
        {
            if (!netsnmp_oid_is_subtree(deployProvShelfType_oid, deployProvShelfType_oid_size,
                    var->name, var->name_length))
            {
                SNMP_Var_Set_Integer(var, (int*)&e->Type);
            }
            else if (!netsnmp_oid_is_subtree(entityStateAdmin_oid, entityStateAdmin_oid_size,
                    var->name, var->name_length))
            {
                SNMP_Var_Set_Integer(var, (int*)&e->Admin);
            }
            else if (!netsnmp_oid_is_subtree(entityStateOper_oid,entityStateOper_oid_size,
                    var->name,var->name_length))
            {
                SNMP_Var_Set_Integer(var, (int*)&e->Oper);
            }
            else if (!netsnmp_oid_is_subtree(entityIndexAid_oid,entityIndexAid_oid_size,
                    var->name,var->name_length))
            {
                SNMP_Var_Set_String(var, e->Aid,sizeof(e->Aid));
            }
        }
    }

    if (response != NULL)
    {
        if (err != NULL)
        {
            *err = response->errstat;
        }
        snmp_free_pdu(response);
    }

    return status;
}

int GetEntityShelfList(netsnmp_session *sess, int *err, X_ListPtr list)
{
    netsnmp_pdu *pdu = NULL, *response = NULL;
    unsigned long index;
    int status, RowStatus;
    EntityShelf e;

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    if (pdu == NULL)
    {
        return (-1);
    }

    snmp_add_null_var(pdu, deployProvShelfRowStatus_oid,
            deployProvShelfRowStatus_oid_size);

    while ( 1 )
    {
        status = snmp_sess_synch_response(sess, pdu, &response);

        // if timeout or error do not continue...
        if (status != STAT_SUCCESS)
        {
            return status;
        }

        // if not in the subtree then break;
        if (netsnmp_oid_is_subtree(deployProvShelfRowStatus_oid, deployProvShelfRowStatus_oid_size,
                response->variables->name, response->variables->name_length))
        {
            break;
        }

        SNMP_Var_Set_Integer(response->variables, (int*)&RowStatus);

        index = SNMP_Var_Index(response->variables);

        if (RowStatus == SNMP_ROW_ACTIVE) // retrieve only active entries!
        {
            EntityShelf_Init(&e);

            status = GetEntityShelf(sess,err,index,&e);

            if (status != STAT_SUCCESS)
            {
                return status;
            }
            else if (status == STAT_SUCCESS && *err == SNMP_ERR_NOERROR)
            {
                if (list != NULL)
                {
                    X_List_Append(list, EntityShelf_Dup(&e));
                }
            }
        }

        if (response != NULL)
            snmp_free_pdu(response);

        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

        snmp_add_null_var_index(pdu, deployProvShelfRowStatus_oid, deployProvShelfRowStatus_oid_size + 1, index);
    }

    return status;
}
#endif

#if 0
int GetEntityModule(struct snmp_session *sess, int *err, unsigned long index, EntityModule *e)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    struct variable_list *var;
    int status;

    pdu = snmp_pdu_create(SNMP_MSG_GET);

    if (pdu == NULL)
    {
        return -1;
    }

    snmp_add_null_var_index(pdu,deployProvModuleType_oid,deployProvModuleType_oid_size+1,index);

    snmp_add_null_var_index(pdu,entityStateAdmin_oid, entityStateAdmin_oid_size+1,index);

    snmp_add_null_var_index(pdu,entityStateOper_oid, entityStateOper_oid_size+1,index);

    snmp_add_null_var_index(pdu,entityIndexAid_oid, entityIndexAid_oid_size+1,index);

    status = snmp_sess_synch_response(sess, pdu, &response);

    if (status == STAT_SUCCESS && response != NULL && response->errstat == SNMP_ERR_NOERROR)
    {
        e->Index = index;

        for(var = response->variables; var; var = var->next_variable)
        {
            if (!netsnmp_oid_is_subtree(deployProvModuleType_oid, deployProvModuleType_oid_size,
                    var->name, var->name_length))
            {
                e->Type = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(entityStateAdmin_oid, entityStateAdmin_oid_size,
                    var->name, var->name_length))
            {
                e->Admin = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(entityStateOper_oid,entityStateOper_oid_size,
                    var->name,var->name_length))
            {
                e->Oper = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(entityIndexAid_oid,entityIndexAid_oid_size,
                    var->name,var->name_length))
            {
                set_string(e->Aid, sizeof(e->Aid), var);
            }
        }
    }

    if (response != NULL)
    {
        if (err != NULL)
        {
            *err = response->errstat;
        }
        snmp_free_pdu(response);
    }

    return status;
}

int GetEntityModuleList(struct snmp_session *sess, int *err, X_ListPtr list)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    unsigned long index;
    int status;
    EntityModule e;

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    snmp_add_null_var(pdu, deployProvModuleRowStatus_oid, deployProvModuleRowStatus_oid_size);

    while ( 1 )
    {
        status = snmp_sess_synch_response(sess, pdu, &response);

        if (status != STAT_SUCCESS)
        {
            return status;
        }

        if (netsnmp_oid_is_subtree(deployProvModuleRowStatus_oid, deployProvModuleRowStatus_oid_size,
                response->variables->name, response->variables->name_length))
        {
            break;
        }

        index = response->variables->name[response->variables->name_length - 1];

        EntityModule_Init(&e);

        GetEntityModule(sess, err, index, &e);

        if (status != STAT_SUCCESS)
        {
            return status;
        }
        else if(status == STAT_SUCCESS && *err == SNMP_ERR_NOERROR)
        {
            if (list != NULL)
            {
                X_List_Append(list, EntityModule_Dup(&e));
            }
        }

        snmp_free_pdu(response);

        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

        snmp_add_null_var_index(pdu, deployProvModuleRowStatus_oid, deployProvModuleRowStatus_oid_size + 1, index);
    }

    return 0;
}
#endif

int GetEntityPlug(struct snmp_session *sess, int *err, unsigned long index, EntityPlug *e)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    struct variable_list *var;
    int stat;

    pdu = snmp_pdu_create(SNMP_MSG_GET);

    if (pdu == NULL)
    {
        return -1;
    }

    snmp_add_null_var_index(pdu,deployProvPlugType_oid,
            deployProvPlugType_oid_size + 1,index);

    snmp_add_null_var_index(pdu,entityStateAdmin_oid,
            entityStateAdmin_oid_size + 1,index);

    snmp_add_null_var_index(pdu,entityStateOper_oid,
            entityStateOper_oid_size + 1,index);

    snmp_add_null_var_index(pdu,entityIndexAid_oid,
            entityIndexAid_oid_size + 1,index);

    snmp_add_null_var_index(pdu, entityContainedIn_oid,
            entityContainedIn_oid_size + 1, index);

    stat = snmp_sess_synch_response(sess,pdu,&response);

    if (response != NULL)
    {
        *err = response->errstat;
    }

    if (stat == STAT_SUCCESS && err == SNMP_ERR_NOERROR && response != NULL)
    {
        Entity_Set_Index(e, index);

        for (var = response->variables; var; var = var->next_variable)
        {
            if (!netsnmp_oid_is_subtree(deployProvPlugType_oid,deployProvPlugType_oid_size,
                    var->name,var->name_length))
            {
                e->Type = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(entityStateAdmin_oid,entityStateAdmin_oid_size,
                    var->name,var->name_length))
            {
                e->Admin = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(entityStateOper_oid,entityStateOper_oid_size,
                    var->name,var->name_length))
            {
                printf("oper state: ");
                print_variable(var->name,var->name_length,var);
                e->Oper = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(entityIndexAid_oid,entityIndexAid_oid_size,
                    var->name,var->name_length))
            {
                printf("aid: ");
                print_variable(var->name,var->name_length,var);
                size_t len = (var->val_len >= MAX_AID_LEN - 1) ? (MAX_AID_LEN - 2) : (var->val_len);
                memcpy(e->Aid,var->val.string,len);
                e->Aid[len + 1] = 0;
            }
            else if (!netsnmp_oid_is_subtree(entityContainedIn_oid, entityContainedIn_oid_size,
                    var->name, var->name_length))
            {
                e->ContainedIn = *(var->val.integer);
            }
        }
    }

    if (response != NULL)
    {
        snmp_free_pdu(response);
    }

    if (stat == STAT_SUCCESS && err == SNMP_ERR_NOERROR)
    {
        GetEntityContainedIn(sess, e->ContainedIn, &e->ContainedIn);
    }

    return stat;
}

int
GetEntityPlugList(struct snmp_session *sess, int *err, X_ListPtr list)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    unsigned long index;
    int status;
    EntityPlug e;

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    snmp_add_null_var(pdu,deployProvPlugRowStatus_oid,deployProvPlugRowStatus_oid_size);

    while (1)
    {
        status = snmp_sess_synch_response(sess,pdu,&response);

        if (status != STAT_SUCCESS)
            return status;

        if (netsnmp_oid_is_subtree(deployProvPlugRowStatus_oid,deployProvPlugRowStatus_oid_size,
                response->variables->name,response->variables->name_length))
        {
            break;
        }

        index = response->variables->name[response->variables->name_length - 1];

        EntityPlug_Init(&e);

        status = GetEntityPlug(sess,err,index,&e);

        if (status != STAT_SUCCESS)
        {
            return status;
        }

        if (status == STAT_SUCCESS && *err == SNMP_ERR_NOERROR)
        {
            if (list) X_List_Append(list, EntityPlug_Dup(&e));
        }

        snmp_free_pdu(response);

        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

        snmp_add_null_var_index(pdu,deployProvPlugRowStatus_oid,
                deployProvPlugRowStatus_oid_size + 1,index);
    }

    return 0;
}

#if 0
int GetEntityConnection(struct snmp_session *sess, int *err, unsigned long index, EntityConnection *e)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    struct variable_list *var;
    int status;

    pdu = snmp_pdu_create(SNMP_MSG_GET);

    if (pdu == NULL)
    {
        return -1;
    }

    snmp_add_null_var_index(pdu,intraNeConnectionsEndpointA_oid, intraNeConnectionsEndpointA_oid_size + 1,index);

    snmp_add_null_var_index(pdu,intraNeConnectionsEndpointB_oid, intraNeConnectionsEndpointB_oid_size + 1,index);

    snmp_add_null_var_index(pdu,intraNeConnectionsType_oid, intraNeConnectionsType_oid_size + 1,index);

    status = snmp_sess_synch_response(sess, pdu, &response);

    if (status == STAT_SUCCESS && response != NULL && response->errstat == SNMP_ERR_NOERROR)
    {
        e->Index = index;

        for(var = response->variables; var; var = var->next_variable)
        {
            if (!netsnmp_oid_is_subtree(intraNeConnectionsEndpointA_oid, intraNeConnectionsEndpointA_oid_size,
                    var->name, var->name_length))
            {
                e->From = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(intraNeConnectionsEndpointB_oid, intraNeConnectionsEndpointB_oid_size,
                    var->name, var->name_length))
            {
                e->To = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(intraNeConnectionsType_oid, intraNeConnectionsType_oid_size,
                    var->name,var->name_length))
            {
                e->Type = *(var->val.integer);
            }
        }
    }

    if (response != NULL)
    {
        if (err != NULL) *err = response->errstat;
        snmp_free_pdu(response);
    }

    return status;
}

int GetEntityConnectionList(struct snmp_session *sess, int *err, X_ListPtr list)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    unsigned long index;
    int status;
    EntityConnection e;

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    snmp_add_null_var(pdu,intraNeConnectionsType_oid, intraNeConnectionsType_oid_size);

    while (1)
    {
        status = snmp_sess_synch_response(sess,pdu,&response);

        if (status != STAT_SUCCESS)
            return status;

        if (netsnmp_oid_is_subtree(intraNeConnectionsType_oid,intraNeConnectionsType_oid_size,
                response->variables->name,response->variables->name_length))
        {
            break;
        }

        index = response->variables->name[response->variables->name_length - 1];

        EntityConnection_Init(&e);

        status = GetEntityConnection(sess,err,index,&e);

        if (status != STAT_SUCCESS)
        {
            return status;
        }
        else if (status == STAT_SUCCESS && *err == SNMP_ERR_NOERROR)
        {
            if (list != NULL) X_List_Append(list, EntityConnection_Dup(&e));
        }

        snmp_free_pdu(response);

        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

        snmp_add_null_var_index(pdu,intraNeConnectionsType_oid,intraNeConnectionsType_oid_size + 1,index);
    }

    return status;
}
#endif

#if 0

int
GetEntityPTP(netsnmp_session *sess, int *err, unsigned long index, EntityPTP *e)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    struct variable_list *var;
    int status;

    pdu = snmp_pdu_create(SNMP_MSG_GET);

    if (pdu == NULL)
    {
        X_LOGERR(1, "GetEntityPTP: snmp_pdu_create() failed");
        return -1;
    }

    snmp_add_null_var_index(pdu, ptpEntityContainedIn_oid, ptpEntityContainedIn_oid_size + 1, index);

    snmp_add_null_var_index(pdu, ptpEntityIndexAid_oid, ptpEntityIndexAid_oid_size + 1, index);

    status = snmp_sess_synch_response(sess, pdu, &response);

    if (status == STAT_SUCCESS && response != NULL && response->errstat == SNMP_ERR_NOERROR)
    {
        e->Index = index;

        for(var = response->variables; var; var = var->next_variable)
        {
            if (!netsnmp_oid_is_subtree(ptpEntityContainedIn_oid, ptpEntityContainedIn_oid_size,
                    var->name, var->name_length))
            {
                SNMP_Var_Set_Integer(var, (int*)&e->ContainedIn);
            }
            else if (!netsnmp_oid_is_subtree(ptpEntityIndexAid_oid, ptpEntityIndexAid_oid_size,
                    var->name, var->name_length))
            {
                SNMP_Var_Set_String(var, e->Aid, sizeof(e->Aid));
            }
        }
    }

    if (response != NULL)
    {
           if (err != NULL) *err = response->errstat;
           snmp_free_pdu(response);
       }

       return status;
}


int GetEntityPTPList(struct snmp_session *sess, int *err, X_ListPtr list)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    unsigned long index;
    int status;
    EntityPTP e;

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    snmp_add_null_var(pdu, ptpEntityContainedIn_oid, ptpEntityContainedIn_oid_size);

    while (1)
    {
        status = snmp_sess_synch_response(sess,pdu,&response);

        if (status != STAT_SUCCESS)
            return status;

        if (netsnmp_oid_is_subtree(ptpEntityContainedIn_oid,ptpEntityContainedIn_oid_size,
                response->variables->name,response->variables->name_length))
        {
            break;
        }


        index = response->variables->name[response->variables->name_length - 1];

        EntityPTP_Clear(&e);

        status = GetEntityPTP(sess, err, index, &e);

        if (status != STAT_SUCCESS)
        {
            return status;
        }
        else if (status == STAT_SUCCESS)
        {
            if (list)
            {
                X_List_Append(list, EntityPTP_Copy(&e));
            }
        }

        if (response != NULL)
            snmp_free_pdu(response);


        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

        snmp_add_null_var_index(pdu,ptpEntityContainedIn_oid,
                ptpEntityContainedIn_oid_size + 1,index);
    }

    return 0;
}

int GetEntityOL(struct snmp_session *sess, int *err, unsigned long index, EntityOL *e)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    struct variable_list *var;
    int status;

    pdu = snmp_pdu_create(SNMP_MSG_GET);

    if (pdu == NULL)
    {
        return -1;
    }

    snmp_add_null_var_index(pdu,entityIndexAid_oid,entityIndex_oid_size+1,index);

    status = snmp_sess_synch_response(sess,pdu,&response);

    if (status == STAT_SUCCESS && response != NULL && response->errstat == SNMP_ERR_NOERROR)
    {
        e->Index = index;

        for (var = response->variables; var; var = var->next_variable)
        {
            print_variable(var->name, var->name_length, var);
            if (!netsnmp_oid_is_subtree(entityIndexAid_oid,entityIndexAid_oid_size,
                    var->name,var->name_length))
            {
                set_string(e->Aid,sizeof(e->Aid),var);
            }
        }
    }

    if (response != NULL)
    {
        if (err != NULL)
        {
            *err = response->errstat;
        }
        snmp_free_pdu(response);
    }

    return status;
}

int
GetEntityOLList(struct snmp_session *sess,
        int *err,
        X_ListPtr list)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    unsigned long index;
    int status;
    EntityOL e;

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    snmp_add_null_var(pdu,deployProvOpticalLinkRowStatus_oid,
            deployProvOpticalLinkRowStatus_oid_size);

    while (1)
    {
        status = snmp_sess_synch_response(sess,pdu,&response);

        if (status != STAT_SUCCESS)
            return status;

        if (netsnmp_oid_is_subtree(deployProvOpticalLinkRowStatus_oid,
                deployProvOpticalLinkRowStatus_oid_size,
                response->variables->name,
                response->variables->name_length))
        {
            break;
        }

        index = response->variables->name[response->variables->name_length - 1];

        EntityOL_Init(&e);

        status = GetEntityOL(sess,err,index,&e);

        if (status != STAT_SUCCESS)
        {
            return status;
        }
        else if (status == STAT_SUCCESS && *err == SNMP_ERR_NOERROR)
        {
            if (list != NULL)
                X_List_Append(list, EntityOL_Copy(&e));
        }

        snmp_free_pdu(response);

        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

        snmp_add_null_var_index(pdu,deployProvOpticalLinkRowStatus_oid,
                deployProvOpticalLinkRowStatus_oid_size+1,index);
    }

    return status;
}

#endif

#if 0
int
GetEntityTunnelWDM(struct snmp_session *sess,
        int *err,
        unsigned long index,
        EntityTunnel *e)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    struct variable_list *var;
    int status;

    pdu = snmp_pdu_create(SNMP_MSG_GET);

    if (pdu == NULL)
    {
        return -1;
    }

    snmp_add_null_var_index(pdu,controlPlaneWdmEntityStateAdmin_oid,
            controlPlaneWdmEntityStateAdmin_oid_size+1,index);

    snmp_add_null_var_index(pdu,tunnelWdmDataTunnelId_oid,
            tunnelWdmDataTunnelId_oid_size+1,index);

    snmp_add_null_var_index(pdu,tunnelWdmDataFromAid_oid,
            tunnelWdmDataFromAid_oid_size+1,index);

    snmp_add_null_var_index(pdu,tunnelWdmDataToAid_oid,
                tunnelWdmDataToAid_oid_size+1,index);

    snmp_add_null_var_index(pdu,tunnelWdmDataToNodeIp_oid,
                    tunnelWdmDataToNodeIp_oid_size+1,index);

    /*
    snmp_add_null_var_index(pdu,entityIndexAid_oid,
            entityIndex_oid_size+1,index);
            */

    status = snmp_sess_synch_response(sess,pdu,&response);

    if (status == STAT_SUCCESS && response != NULL && response->errstat
            == SNMP_ERR_NOERROR)
    {
        e->Index = index;

        for (var = response->variables; var; var = var->next_variable)
        {
            if (!netsnmp_oid_is_subtree(controlPlaneWdmEntityStateAdmin_oid,
                    controlPlaneWdmEntityStateAdmin_oid_size,
                    var->name,var->name_length))
            {
                e->Admin = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(tunnelWdmDataTunnelId_oid,
                    tunnelWdmDataTunnelId_oid_size,
                    var->name,var->name_length))
            {
                set_string(e->Id, sizeof(e->Id), var);
            }
            else if (!netsnmp_oid_is_subtree(tunnelWdmDataFromAid_oid,
                                tunnelWdmDataFromAid_oid_size,
                                var->name,var->name_length))
            {
                e->FromAid = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(tunnelWdmDataToAid_oid,
                    tunnelWdmDataToAid_oid_size,
                    var->name,var->name_length))
            {
                e->ToAid = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(tunnelWdmDataToNodeIp_oid,
                    tunnelWdmDataToNodeIp_oid_size,
                    var->name,var->name_length))
            {
                e->ToIP = *(var->val.integer);
            }
        }
    }

    if (response != NULL)
    {
        if (err != NULL)
        {
            *err = response->errstat;
        }
        snmp_free_pdu(response);
    }

    return status;
}

int GetEntityTunnelWDMList(struct snmp_session *sess, int *err, X_ListPtr list)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    unsigned long index;
    int status;
    EntityTunnel e;

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    snmp_add_null_var(pdu,deployProvTunnelWdmRowStatus_oid,
            deployProvTunnelWdmRowStatus_oid_size);

    while (1)
    {
        status = snmp_sess_synch_response(sess,pdu,&response);

        if (status != STAT_SUCCESS)
            return status;

        if (netsnmp_oid_is_subtree(deployProvTunnelWdmRowStatus_oid,
                deployProvTunnelWdmRowStatus_oid_size,
                response->variables->name,response->variables->name_length))
        {
            break;
        }

        index = response->variables->name[response->variables->name_length - 1];

        EntityTunnel_Init(&e);

        status = GetEntityTunnelWDM(sess,err,index,&e);

        if (status != STAT_SUCCESS)
        {
            return status;
        }
        else if (status == STAT_SUCCESS && *err == SNMP_ERR_NOERROR)
        {
            if (list != NULL)
                X_List_Append(list, EntityTunnel_Dup(&e));
        }

        snmp_free_pdu(response);

        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

        snmp_add_null_var_index(pdu,
                deployProvTunnelWdmRowStatus_oid,
                deployProvTunnelWdmRowStatus_oid_size + 1,index);
    }

    return status;
}

int
GetEntityCnxWDM(struct snmp_session *sess,
        int *err,
        unsigned long index,
        EntityCNX *e)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    struct variable_list *var;
    int status;

    assert(e != NULL);

    pdu = snmp_pdu_create(SNMP_MSG_GET);

    if (pdu == NULL)
    {
        return -1;
    }

    snmp_add_null_var_index(pdu,controlPlaneWdmEntityStateOper_oid,
            controlPlaneWdmEntityStateOper_oid_size + 1,index);

    snmp_add_null_var_index(pdu,connectionWdmDataFromNodeIp_oid,
            connectionWdmDataFromNodeIp_oid_size + 1, index);

    snmp_add_null_var_index(pdu,connectionWdmDataToNodeIp_oid,
            connectionWdmDataToNodeIp_oid_size + 1, index);

    snmp_add_null_var_index(pdu,connectionWdmDataTunnelId_oid,
            connectionWdmDataTunnelId_oid_size + 1, index);

    status = snmp_sess_synch_response(sess,pdu,&response);

    if (status == STAT_SUCCESS && response != NULL &&
            response->errstat == SNMP_ERR_NOERROR)
    {
        e->Index = index;

        for (var = response->variables; var; var = var->next_variable)
        {
            if (!netsnmp_oid_is_subtree(controlPlaneWdmEntityStateOper_oid,
                    controlPlaneWdmEntityStateOper_oid_size,
                    var->name,var->name_length))
            {
                e->Oper = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(connectionWdmDataFromNodeIp_oid,
                    connectionWdmDataFromNodeIp_oid_size,var->name,var->name_length))
            {
                e->FromNodeIP = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(connectionWdmDataToNodeIp_oid,
                    connectionWdmDataToNodeIp_oid_size,var->name,var->name_length))
            {
                e->ToNodeIP = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(connectionWdmDataTunnelId_oid,
                    connectionWdmDataTunnelId_oid_size,var->name,var->name_length))
            {
                set_string(e->Id,sizeof(e->Id),var);
            }
        }
    }

    if (response != NULL)
    {
        if (err != NULL)
        {
            *err = response->errstat;
        }
        snmp_free_pdu(response);
    }

    return status;
}

int
GetEntityCnxWDMList(struct snmp_session *sess,
        int *err,
        X_ListPtr list)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    unsigned long index;
    int status;
    EntityCNX e;

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    snmp_add_null_var(pdu,controlPlaneWdmEntityStateOper_oid,
            controlPlaneWdmEntityStateOper_oid_size);

    while (1)
    {
        status = snmp_sess_synch_response(sess,pdu,&response);

        if (status != STAT_SUCCESS)
            return status;

        if (netsnmp_oid_is_subtree(controlPlaneWdmEntityStateOper_oid,
                controlPlaneWdmEntityStateOper_oid_size,
                response->variables->name,response->variables->name_length))
        {
            break;
        }

        index = response->variables->name[response->variables->name_length - 1];

        EntityCNX_Init(&e);

        status = GetEntityCnxWDM(sess,err,index,&e);

        if (status != STAT_SUCCESS)
        {
            return status;
        }
        else if (status == STAT_SUCCESS && *err == SNMP_ERR_NOERROR)
        {
            printf("%s\n",e.Id);
            if (list != NULL)
                X_List_Append(list, EntityCNX_Copy(&e));
        }

        snmp_free_pdu(response);

        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

        snmp_add_null_var_index(pdu,controlPlaneWdmEntityStateOper_oid,
                controlPlaneWdmEntityStateOper_oid_size + 1,index);
    }

    return status;
}

#endif

int
GetEntityTEUnnumLink(struct snmp_session *sess,
        int *err,
        unsigned int ip, unsigned int id, EntityTEUnnumLink *e)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    struct variable_list *var;
    int status;
    oid index[5] = { 0 };

    pdu = snmp_pdu_create(SNMP_MSG_GET);

    if (pdu == NULL)
    {
        return -1;
    }

    index[0] = (ip >> 24) & 0xff;
    index[1] = (ip >> 16) & 0xff;
    index[2] = (ip >> 8) & 0xff;
    index[3] = (ip) & 0xff;
    index[4] = id;


    snmp_add_null_var_mindex(pdu,teUnnumLinkWdmDataNodeIp_oid,
            teUnnumLinkWdmDataNodeIp_oid_size + 5, index, 5);

    snmp_add_null_var_mindex(pdu,teUnnumLinkWdmDataTeLinkId_oid,
            teUnnumLinkWdmDataTeLinkId_oid_size + 5, index, 5);

    snmp_add_null_var_mindex(pdu,teUnnumLinkWdmDataPhysicalLink_oid,
            teUnnumLinkWdmDataPhysicalLink_oid_size + 5, index, 5);

    snmp_add_null_var_mindex(pdu,teUnnumLinkWdmDataSynchronization_oid,
            teUnnumLinkWdmDataSynchronization_oid_size + 5, index, 5);

    snmp_add_null_var_mindex(pdu,teUnnumLinkWdmDataFendNodeIp_oid,
            teUnnumLinkWdmDataFendNodeIp_oid_size + 5, index, 5);

    snmp_add_null_var_mindex(pdu,teUnnumLinkWdmDataFendTeLinkId_oid,
            teUnnumLinkWdmDataFendTeLinkId_oid_size + 5, index, 5);

    status = snmp_sess_synch_response(sess, pdu, &response);

    if (status == STAT_SUCCESS && response != NULL && response->errstat == SNMP_ERR_NOERROR)
    {
        for(var = response->variables; var; var = var->next_variable)
        {
            if (!netsnmp_oid_is_subtree(teUnnumLinkWdmDataNodeIp_oid,
                    teUnnumLinkWdmDataNodeIp_oid_size,
                    var->name, var->name_length))
            {
                e->NodeIP = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(teUnnumLinkWdmDataTeLinkId_oid,
                    teUnnumLinkWdmDataTeLinkId_oid_size,
                    var->name, var->name_length))
            {
                e->LinkId = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(teUnnumLinkWdmDataPhysicalLink_oid,
                    teUnnumLinkWdmDataPhysicalLink_oid_size,
                    var->name,var->name_length))
            {
                e->PhyLink = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(teUnnumLinkWdmDataSynchronization_oid,
                    teUnnumLinkWdmDataSynchronization_oid_size,
                    var->name,var->name_length))
            {
                e->Synch = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(teUnnumLinkWdmDataFendNodeIp_oid,
                    teUnnumLinkWdmDataFendNodeIp_oid_size,
                    var->name,var->name_length))
            {
                e->FendNodeIP = *(var->val.integer);
            }
            else if (!netsnmp_oid_is_subtree(teUnnumLinkWdmDataFendTeLinkId_oid,
                    teUnnumLinkWdmDataFendTeLinkId_oid_size,
                    var->name,var->name_length))
            {
                e->FendLinkId = *(var->val.integer);
            }
        }
    }

    if (response != NULL)
    {
        if (err != NULL) *err = response->errstat;
        snmp_free_pdu(response);
    }

    return status;
}

int
GetEntityTEUnnumLinkList(struct snmp_session *sess, int *err, X_ListPtr list)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    struct variable_list *var;
    unsigned int ip, id;
    int status;
    EntityTEUnnumLink e;

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    snmp_add_null_var(pdu,teUnnumLinkWdmDataNodeIp_oid,
            teUnnumLinkWdmDataNodeIp_oid_size);

    while (1)
    {

        status = snmp_sess_synch_response(sess, pdu, &response);

        if (status != STAT_SUCCESS)
            return status;

        print_objid(response->variables->name, response->variables->name_length);

        if (netsnmp_oid_is_subtree(teUnnumLinkWdmDataNodeIp_oid,
                teUnnumLinkWdmDataNodeIp_oid_size,
                response->variables->name,
                response->variables->name_length))
        {
            break;
        }

        var = response->variables;

        id = var->name[var->name_length - 1];

        ip = (0xff & var->name[var->name_length - 2]) |
                ((0xff & var->name[var->name_length - 3]) << 8) |
                ((0xff & var->name[var->name_length - 4]) << 16) |
                ((0xff & var->name[var->name_length - 5]) << 24);

        EntityTEUnnumLink_Init(&e);

        status = GetEntityTEUnnumLink(sess,err,ip,id,&e);

        if (status != STAT_SUCCESS)
        {
            return status;
        }
        else if (status == STAT_SUCCESS && *err == SNMP_ERR_NOERROR)
        {
            if (list != NULL)
            {
                X_List_Append(list, EntityTEUnnumLink_Copy(&e));
            }
        }

        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

        snmp_add_null_var(pdu, response->variables->name, response->variables->name_length);

        snmp_free_pdu(response);
    }

    return 0;
}


int
GetUnusedWDMTunnelIndex(struct snmp_session *sess, int *err, unsigned long *index)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    int status;

    assert(sess != NULL);

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    if (pdu == NULL)
    {
        return -1;
    }

    snmp_add_null_var(pdu,unusedWdmTunnelIndex_oid,
            unusedWdmTunnelIndex_oid_size);

    status = snmp_sess_synch_response(sess, pdu, &response);

    if (status == STAT_SUCCESS && response && response->errstat == SNMP_ERR_NOERROR)
    {
        *err = response->errstat;
        if (index != NULL)
            *index = *(response->variables->val.integer);
    }

    if (response != NULL)
    {
        snmp_free_pdu(response);
    }

    return status;
}


int
GetEntityContainedIn(struct snmp_session *sess, unsigned long index, unsigned long *contained)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    int stat, err;

    pdu = snmp_pdu_create(SNMP_MSG_GET);

    if (pdu == NULL)
    {
        return -1;
    }

    snmp_add_null_var_index(pdu, entityContainedIn_oid,
            entityContainedIn_oid_size + 1, index);

    stat = snmp_sess_synch_response(sess, pdu, &response);

    if(response != NULL)
    {
        err = response->errstat;
    }

    if (stat == STAT_SUCCESS && err == SNMP_ERR_NOERROR && response != NULL)
    {
        if (response->variables != NULL)
        {
            *contained = *(response->variables->val.integer);
        }
    }

    if (response != NULL)
    {
        snmp_free_pdu(response);
    }

    return stat;
}


#if 0
int
SNMP_CreateWDMTunnel(netsnmp_session *sess, int *err, EntityTunnel *e)
{
    struct snmp_pdu *pdu = NULL, *response = NULL;
    struct variable_list *vars;
    int status, RowStatus;

    oid temp[MAX_OID_LEN];
    size_t temp_size = MAX_OID_LEN;

    status = GetUnusedWDMTunnelIndex(sess, err, &e->Index);

    if (status != STAT_SUCCESS)
    {
        X_LOGWARN(1, "SNMP_CreateWDMTunnel: failed to retrieve unused WDM tunnel index");
        return status;
    }

    X_LOGDBG(1, "SNMP_CreateWDMTunnel: creating a WDM tunnel, index = %08x",
            e->Index);

    pdu = snmp_pdu_create(SNMP_MSG_SET);

    if (pdu == NULL)
    {
        return (-1);
    }

    RowStatus = SNMP_ROW_CREATEANDWAIT;

    vars = snmp_pdu_add_variable(pdu,deployProvTunnelWdmRowStatus_oid,
            deployProvTunnelWdmRowStatus_oid_size + 1,
            ASN_INTEGER, &RowStatus, sizeof(int));
    vars->name[vars->name_length - 1] = e->Index;

    status = snmp_sess_synch_response(sess, pdu, &response);

    if (status != STAT_SUCCESS)
    {
        X_LOGERR(1, "SNMP_CreateWDMTunnel: failed to set CREATEANDWAIT!");
        return status;
    }

    if (response != NULL)
    {
        snmp_free_pdu(response);
    }

    pdu = snmp_pdu_create(SNMP_MSG_SET);

    vars = snmp_pdu_add_variable(pdu,deployProvTunnelWdmFromAid_oid,
            deployProvTunnelWdmFromAid_oid_size + 1,
            ASN_INTEGER, &(e->FromAid), sizeof(int));
    vars->name[vars->name_length - 1] = e->Index;

    vars = snmp_pdu_add_variable(pdu,deployProvTunnelWdmToAid_oid,
            deployProvTunnelWdmToAid_oid_size + 1,
            ASN_INTEGER, &(e->ToAid), sizeof(int));
    vars->name[vars->name_length - 1] = e->Index;

    vars = snmp_pdu_add_variable(pdu,deployProvTunnelWdmToNodeIp_oid,
            deployProvTunnelWdmToNodeIp_oid_size + 1,
            ASN_IPADDRESS, &(e->ToIP), sizeof(int));
    vars->name[vars->name_length - 1] = e->Index;

    vars = snmp_pdu_add_variable(pdu,deployProvTunnelWdmTunnelId_oid,
            deployProvTunnelWdmTunnelId_oid_size + 1,
            ASN_OCTET_STR, &(e->Id), strlen(e->Id));
    vars->name[vars->name_length - 1] = e->Index;

    status = snmp_sess_synch_response(sess, pdu, &response);

    if (status != STAT_SUCCESS)
    {
        X_LOGERR(1, "SNMP_CreateWDMTunnel: failed to create TNL_WDM entity!");
        return status;
    }
    else if (status == STAT_SUCCESS && response->errstat != SNMP_ERR_NOERROR)
    {
        X_LOGERR(1, "SNMP_CreateWDMTunnel: failed to create TNL_WDM entity error %s!",
                snmp_errstring(response->errstat));
    }

    if (response != NULL)
    {
        snmp_free_pdu(response);
    }

    pdu = snmp_pdu_create(SNMP_MSG_SET);

    if (pdu == NULL)
    {
        return (-1);
    }

    RowStatus = SNMP_ROW_ACTIVE;

    vars = snmp_pdu_add_variable(pdu,deployProvTunnelWdmRowStatus_oid,
            deployProvTunnelWdmRowStatus_oid_size + 1,
            ASN_INTEGER, &RowStatus, sizeof(int));
    vars->name[vars->name_length - 1] = e->Index;

    status = snmp_sess_synch_response(sess, pdu, &response);

    if (status != STAT_SUCCESS)
    {
        X_LOGERR(1, "SNMP_CreateWDMTunnel: failed to set ACTIVE!");
        return status;
    }

    if (response != NULL)
    {
        snmp_free_pdu(response);
    }

    return status;
}

#endif


/******************************************************************************/


/******************************************************************************/

ErrCode
SNMP_GetEntityNE(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, EntityNE *pEnt)
{
    X_ListPtr pOidList;
    SNMP_OidPtr pOid;
    ErrCode ret;

    if (pSess == NULL || pErr == NULL || pEnt == NULL)
    {
        X_LOGERR(X_LOG_INTF_ID, "SNMP_GetEntityNE: invalid parameters");
        return (ERRCODE_INVALID_PARAMS);
    }

    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

    if (pOidList == NULL)
    {
        return (ERRCODE_NOMEM);
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_neSystemIp,
            &pEnt->SysIP, sizeof(pEnt->SysIP), 1);
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_neSystemIpMask,
            &pEnt->SysIPMask, sizeof(pEnt->SysIPMask), 1);
    X_List_Append(pOidList, pOid);

    ret = SNMP_GetNext(pSess, pErr, pOidList);

    X_List_Free(pOidList);

    return ret;
}

ErrCode
SNMP_GetEntityShelf(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, EntityShelf *pEnt)
{
    X_ListPtr pOidList;
    X_ListNodePtr pNode;
    SNMP_OidPtr pOid;
    ErrCode ret;

    if (pSess == NULL || pErr == NULL || pEnt == NULL)
    {
        X_LOGERR(X_LOG_INTF_ID, "SNMP_GetEntityShelf(): invalid parameters");
        return ERRCODE_INVALID_PARAMS;
    }

    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

    if (pOidList == NULL)
    {
        return ERRCODE_NOMEM;
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvShelfType,
            &pEnt->Type, sizeof(pEnt->Type), 1);
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_entityStateAdmin,
            &pEnt->Admin, sizeof(pEnt->Admin), 1);
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_entityStateOper,
            &pEnt->Oper, sizeof(pEnt->Oper), 1);
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_entityIndexAid,
                pEnt->Aid, sizeof(pEnt->Aid), 1);
    X_List_Append(pOidList, pOid);

    for(pNode = X_List_Head(pOidList); pNode; pNode = X_List_Next(pNode))
    {
        SNMP_OidPtr pOid = (SNMP_OidPtr)pNode->Data;
        SNMP_Oid_Append(pOid, Entity_Get_Index(pEnt));
    }

    ret = SNMP_Get(pSess, pErr, pOidList);

    X_List_Free(pOidList);

    return ret;
}

ErrCode
SNMP_GetEntityShelfList(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, X_ListPtr pList)
{
    SNMP_OidPtr pOid, pOidRoot;
    ErrCode ret = ERRCODE_NOERROR;
    EntityShelf e;
    unsigned int RowStatus;

    assert(pSess && pErr && pList);

    if (pSess == NULL || pErr == NULL || pList == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvShelfRowStatus,
            &RowStatus, sizeof(RowStatus), 1);

    if (pOid == NULL)
    {
        return ERRCODE_NOMEM;
    }

    pOidRoot = SNMP_Oid_Copy(pOid);

    while ( ret == ERRCODE_NOERROR )
    {
        ret = SNMP_GetNextOid(pSess, pErr, pOid, pOidRoot);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        /*
        if (RowStatus != SNMP_ROW_ACTIVE)
            continue;
            */

        EntityShelf_Init(&e);

        Entity_Set_Index(&e, SNMP_Oid_Index(pOid));

        ret = SNMP_GetEntityShelf(pSess, pErr, &e);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        X_List_Append(pList, EntityShelf_Dup(&e));
    }

    SNMP_Oid_Free(pOid);
    SNMP_Oid_Free(pOidRoot);

    return (ret == ERRCODE_NOERROR_END || ret == ERRCODE_NOERROR) ? (ERRCODE_NOERROR) : (ret);
}

ErrCode
SNMP_GetEntityModule(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, EntityModule *e)
{
    SNMP_OidPtr pOid;
    X_ListNodePtr pNode;
    X_ListPtr pOidList;
    ErrCode ret;

    if (pSess == NULL || pErr == NULL || e == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

    if (pOidList == NULL)
    {
        return ERRCODE_NOMEM;
    }

#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::deployProvModuleType",1,&e->Type,sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::deployProvModuleMode",1,&e->Mode,sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::deployProvModuleBand",1,&e->Band,sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::entityStateAdmin",1,&e->Admin,sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::entityStateOper",1,&e->Oper,sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("ADVA-MIB::entityIndexAid",1,e->Aid,sizeof(e->Aid));
    X_List_Append(pOidList, pOid);
#else
    pOid = SNMP_Oid_New_Raw(deployProvModuleType_oid,
            deployProvModuleType_oid_size,
            1, ASN_INTEGER, &e->Type, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(deployProvModuleMode_oid,
            deployProvModuleMode_oid_size,
            1, ASN_INTEGER, &e->Mode, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(deployProvModuleBand_oid,
            deployProvModuleBand_oid_size,
            1, ASN_INTEGER, &e->Band, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(deployProvModuleChannel_oid,
            deployProvModuleChannel_oid_size,
            1, ASN_INTEGER, &e->Channel, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(entityStateAdmin_oid,
            entityStateAdmin_oid_size,
            1, ASN_INTEGER, &e->Admin, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(entityStateOper_oid,
            entityStateOper_oid_size,
            1, ASN_INTEGER, &e->Oper, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(entityIndexAid_oid,
            entityIndexAid_oid_size,
            1, ASN_OCTET_STR, e->Aid, sizeof(e->Aid));
    X_List_Append(pOidList, pOid);
#endif

    // append the index
    for (pNode = X_List_Head(pOidList); pNode; pNode = X_List_Next(pNode))
    {
        pOid = (SNMP_OidPtr) pNode->Data;
        SNMP_Oid_Append(pOid, Entity_Get_Index(e));
    }

    ret = SNMP_Get(pSess, pErr, pOidList);

    X_List_Free(pOidList);

    return ret;
}

ErrCode
SNMP_GetEntityModuleList(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, X_ListPtr pList)
{
    X_ListPtr pOidList;
    X_ListNodePtr pNode;
    SNMP_OidPtr pOid, pOidRoot, pTempOid;
    ErrCode ret = ERRCODE_NOERROR;
    EntityModule e;
    int RowStatus;

    assert(pSess && pErr && pList);

    if (pSess == NULL || pErr == NULL || pList == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOidList = X_List_New((X_ListMemFree_t) SNMP_Oid_Free);

    if (pOidList == NULL)
    {
        return ERRCODE_NOMEM;
    }

#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::deployProvModuleType",1,&e.Type,sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::deployProvModuleMode",1,&e.Mode,sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::deployProvModuleBand",1,&e.Band,sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::entityStateAdmin",1,&e.Admin,sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::entityStateOper",1,&e.Oper,sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("ADVA-MIB::entityIndexAid",1,e.Aid,sizeof(e.Aid));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::deployProvModuleRoadmNumber", 1, &e.RoadmNo, sizeof(int));
    X_List_Append(pOidList, pOid);
#else
    pOid = SNMP_Oid_New_Raw(deployProvModuleType_oid,
            deployProvModuleType_oid_size,
            1, ASN_INTEGER, &e.Type, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(deployProvModuleMode_oid,
            deployProvModuleMode_oid_size,
            1, ASN_INTEGER, &e.Mode, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(deployProvModuleBand_oid,
            deployProvModuleBand_oid_size,
            1, ASN_INTEGER, &e.Band, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(deployProvModuleChannel_oid,
            deployProvModuleChannel_oid_size,
            1, ASN_INTEGER, &e.Channel, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(entityStateAdmin_oid,
            entityStateAdmin_oid_size,
            1, ASN_INTEGER, &e.Admin, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(entityStateOper_oid,
            entityStateOper_oid_size,
            1, ASN_INTEGER, &e.Oper, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(entityIndexAid_oid,
            entityIndexAid_oid_size,
            1, ASN_OCTET_STR, e.Aid, sizeof(e.Aid));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(deployProvModuleRoadmNumber_oid,
            deployProvModuleRoadmNumber_oid_size,
            1, ASN_INTEGER, &e.RoadmNo, sizeof(int));
    X_List_Append(pOidList, pOid);
#endif


#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::deployProvModuleRowStatus",1,&RowStatus,sizeof(int));
#else
    pOid = SNMP_Oid_New_Raw(deployProvModuleRowStatus_oid,
            deployProvModuleRowStatus_oid_size,
            1, ASN_INTEGER, &RowStatus, sizeof(int));
#endif

    pOidRoot = SNMP_Oid_Copy(pOid);

    // append the 0 index
    X_List_ForEach(pOidList, pNode)
    {
        SNMP_Oid_Append((SNMP_OidPtr) pNode->Data, 0);
    }

    while (ret == ERRCODE_NOERROR)
    {
        ret = SNMP_GetNextOid(pSess,pErr,pOid,pOidRoot);

        if (!(ret == ERRCODE_NOERROR &&
                pErr->Status == STAT_SUCCESS &&
                pErr->Err == SNMP_ERR_NOERROR))
        {
            break;
        }

        EntityModule_Init(&e);

        Entity_Set_Index(&e, SNMP_Oid_Index(pOid));

        X_List_ForEach(pOidList, pNode)
        {
            pTempOid = (SNMP_OidPtr) pNode->Data;
            SNMP_Oid_UpdateIndex((SNMP_OidPtr)pNode->Data,
                    Entity_Get_Index(&e));
        }

        ret = SNMP_Get(pSess, pErr, pOidList);

        if (!(ret == ERRCODE_NOERROR &&
                pErr->Status == STAT_SUCCESS &&
                pErr->Err == SNMP_ERR_NOERROR))
        {
            break;
        }

        X_List_Append(pList, EntityModule_Dup(&e));
    }

    SNMP_Oid_Free(pOidRoot);
    X_List_Free(pOidList);

    return (ret == ERRCODE_NOERROR_END || ret == ERRCODE_NOERROR) ? (ERRCODE_NOERROR) : (ret);
}

ErrCode
SNMP_GetEntityPlug(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, EntityPlug *pEntity)
{
    X_ListPtr           pOidList;
    X_ListNodePtr       pNode;
    SNMP_OidPtr         pOid;
    ErrCode             ret;

    if (pSess == NULL || pErr == NULL || pEntity == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvPlugType,
            &pEntity->Type, sizeof(pEntity->Type), 1);
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_entityIndexAid,
            pEntity->Aid, sizeof(pEntity->Aid), 1);
    X_List_Append(pOidList, pOid);

    X_List_ForEach(pOidList, pNode)
    {
        SNMP_Oid_Append((SNMP_OidPtr)pNode->Data, Entity_Get_Index(pEntity));
    }

    ret = SNMP_Get(pSess, pErr, pOidList);

    X_List_Free(pOidList);

    return ret;
}

ErrCode
SNMP_GetEntityPlugList(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, X_ListPtr pList)
{
    SNMP_OidPtr pOid, pRootOid;
    unsigned int RowStatus;
    ErrCode ret = ERRCODE_NOERROR;
    EntityPlug e;

    if (pSess == NULL || pErr == NULL || pList == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::deployProvPlugRowStatus",
            1, &RowStatus, sizeof(RowStatus));
#else
    pOid = SNMP_Oid_New_Raw(deployProvPlugRowStatus_oid,
            deployProvPlugRowStatus_oid_size,
            1, ASN_INTEGER, &RowStatus, sizeof(RowStatus));
#endif

    pRootOid = SNMP_Oid_Copy(pOid);

    while (ret == ERRCODE_NOERROR)
    {
        ret = SNMP_GetNextOid(pSess, pErr, pOid, pRootOid);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        EntityPlug_Init(&e);

        Entity_Set_Index(&e, SNMP_Oid_Index(pOid));

        ret = SNMP_GetEntityPlug(pSess, pErr, &e);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        X_List_Append(pList, EntityPlug_Dup(&e));
    }

    SNMP_Oid_Free(pOid);

    SNMP_Oid_Free(pRootOid);

    return ret;
}


ErrCode
SNMP_GetEntityConnection(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, EntityConnection *e)
{
    SNMP_OidPtr         pOid;
    X_ListNodePtr       pNode;
    X_ListPtr           pOidList;
    ErrCode             ret;

    if (pSess == NULL || pErr == NULL || e == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

    if (pOidList == NULL)
    {
        return ERRCODE_NOMEM;
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_intraNeConnectionsType,
            &e->Type, sizeof(e->Type), 1);
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_intraNeConnectionsEndpointA,
                &e->From, sizeof(e->From), 1);
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_intraNeConnectionsEndpointB,
            &e->To, sizeof(e->To), 1);
    X_List_Append(pOidList, pOid);

    X_List_ForEach(pOidList, pNode)
    {
        pOid = (SNMP_OidPtr)(pNode->Data);
        SNMP_Oid_Append(pOid, Entity_Get_Index(e));
    }

    ret = SNMP_Get(pSess, pErr, pOidList);

    if (ret != ERRCODE_NOERROR ||
            pErr->Status != STAT_SUCCESS ||
            pErr->Err != SNMP_ERR_NOERROR)
    {
        X_LOGERR(X_LOG_INTF_ID, "SNMP_GetEntityConnection(): failed to load connection data");
        X_List_Free(pOidList);
        return ret;
    }

    X_List_Clear(pOidList);

/// TODO: handle VTPs

    //ret = SNMP_GetEntityClass(pSess, pErr, e->From, &FromClass);
/*
    pOid = SNMP_Oid_New("ADVA-MIB::ptpEntityIndexAid",1,e->FromAid,sizeof(e->FromAid));
    SNMP_Oid_Append(pOid, e->From);
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("ADVA-MIB::ptpEntityIndexAid",1,e->ToAid,sizeof(e->ToAid));
    SNMP_Oid_Append(pOid, e->To);
    X_List_Append(pOidList, pOid);
*/

    ret = SNMP_GetPhysicalTermPointAid(pSess, pErr, e->From, e->FromAid, sizeof(e->FromAid));

    if (ret != ERRCODE_NOERROR ||
            pErr->Status != STAT_SUCCESS)
    {
        X_LOGERR(X_LOG_INTF_ID, "SNMP_GetEntityConnection(): failed to load 'from' PTP/VTP data");
        X_List_Free(pOidList);
        return ret;
    }

    ret = SNMP_GetPhysicalTermPointAid(pSess, pErr, e->To, e->ToAid, sizeof(e->ToAid));

    if (ret != ERRCODE_NOERROR ||
            pErr->Status != STAT_SUCCESS)
    {
        X_LOGERR(X_LOG_INTF_ID, "SNMP_GetEntityConnection(): failed to load 'to' PTP/VTP data");
        X_List_Free(pOidList);
        return ret;
    }

    X_List_Free(pOidList);

    return ret;
}

ErrCode
SNMP_GetEntityConnectionList(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, X_ListPtr pList)
{
    SNMP_OidPtr pOid, pOidRoot;
    ErrCode ret = ERRCODE_NOERROR;
    EntityConnection e;

    assert(pSess && pErr && pList);

    if (pSess == NULL || pErr == NULL || pList == NULL)
    {
        X_LOGERR(X_LOG_INTF_ID, "SNMP_GetEntityConnectionList: invalid arguments");
        return ERRCODE_INVALID_PARAMS;
    }

#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::intraNeConnectionsType",1,&e.Type,sizeof(int));
#else
    pOid = SNMP_Oid_New_Raw(intraNeConnectionsType_oid,
            intraNeConnectionsType_oid_size,
            1, ASN_INTEGER, &e.Type, sizeof(int));
#endif

    pOidRoot = SNMP_Oid_Copy(pOid);

    if (pOidRoot == NULL)
    {
        ret = ERRCODE_NOMEM;
    }

    while (ret == ERRCODE_NOERROR)
    {
        ret = SNMP_GetNextOid(pSess, pErr, pOid, pOidRoot);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        EntityConnection_Init(&e);

        Entity_Set_Index(&e, SNMP_Oid_Index(pOid));

        ret = SNMP_GetEntityConnection(pSess, pErr, &e);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

      //  ret = SNMP_GetNextOid(pSess, pErr, pOid, pOidRoot);

        X_List_Append(pList, EntityConnection_Dup(&e));
    }

    SNMP_Oid_Free(pOid);
    SNMP_Oid_Free(pOidRoot);

    return (ret == ERRCODE_NOERROR_END || ret == ERRCODE_NOERROR) ? (ERRCODE_NOERROR) : (ret);
}

ErrCode
SNMP_GetEntityOL(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, EntityOL *pEn)
{
    X_ListPtr pOidList;
    X_ListNodePtr pNode;
    SNMP_OidPtr pOid;
    ErrCode ret;
    unsigned int RowStatus;

    if (pSess == NULL || pErr == NULL || pEn == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

    if (pOidList == NULL)
    {
        return ERRCODE_NOMEM;
    }

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_entityIndexAid,
            pEn->Aid, sizeof(pEn->Aid), 1);
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvOpticalLinkRowStatus,
            &RowStatus, sizeof(RowStatus), 1);
    X_List_Append(pOidList, pOid);

    X_List_ForEach(pOidList, pNode)
    {
        SNMP_Oid_Append((SNMP_OidPtr)pNode->Data, Entity_Get_Index(pEn));
    }

    ret = SNMP_Get(pSess, pErr, pOidList);

    X_List_Free(pOidList);

    return ret;
}

ErrCode
SNMP_GetEntityOLList(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, X_ListPtr pList)
{
    SNMP_OidPtr pOid, pRootOid;
    unsigned int RowStatus;
    EntityOL e;
    ErrCode ret = ERRCODE_NOERROR;

    if (pSess == NULL || pErr == NULL || pList == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvOpticalLinkRowStatus,
            &RowStatus, sizeof(RowStatus), 1);

    if (pOid == NULL)
    {
        return ERRCODE_NOMEM;
    }

    pRootOid = SNMP_Oid_Copy(pOid);

    if (pRootOid == NULL)
    {
        ret = ERRCODE_NOMEM;
    }

    while (ret == ERRCODE_NOERROR)
    {
        ret = SNMP_GetNextOid(pSess, pErr, pOid, pRootOid);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        EntityOL_Init(&e);

        Entity_Set_Index(&e, SNMP_Oid_Index(pOid));

        ret = SNMP_GetEntityOL(pSess, pErr, &e);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        X_List_Append(pList, EntityOL_Copy(&e));
    }

    SNMP_Oid_Free(pRootOid);
    SNMP_Oid_Free(pOid);

    return (ret == ERRCODE_NOERROR_END || ret == ERRCODE_NOERROR) ? (ERRCODE_NOERROR) : (ret);
}

ErrCode
SNMP_GetEntityCnxWDM(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, EntityCNX *pEn)
{
    X_ListPtr pOidList;
    X_ListNodePtr pNode;
    SNMP_OidPtr pOid;
    ErrCode ret;

    if (pSess == NULL || pErr == NULL || pEn == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::connectionWdmDataFromNodeIp",
            1, &pEn->FromNodeIP, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::connectionWdmDataToNodeIp",
            1, &pEn->ToNodeIP, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::connectionWdmDataTunnelId",
                1, &pEn->Id, sizeof(pEn->Id));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::controlPlaneWdmEntityStateOper",
            1, &pEn->Oper, sizeof(int));
    X_List_Append(pOidList, pOid);
#else
    pOid = SNMP_Oid_New_Raw(connectionWdmDataFromNodeIp_oid,
            connectionWdmDataFromNodeIp_oid_size,
            1, ASN_IPADDRESS, &pEn->FromNodeIP, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(connectionWdmDataToNodeIp_oid,
            connectionWdmDataToNodeIp_oid_size,
            1, ASN_IPADDRESS, &pEn->ToNodeIP, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(connectionWdmDataTunnelId_oid,
            connectionWdmDataTunnelId_oid_size,
            1, ASN_OCTET_STR, &pEn->Id, sizeof(pEn->Id));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(controlPlaneWdmEntityStateOper_oid,
            controlPlaneWdmEntityStateOper_oid_size,
            1, ASN_INTEGER, &pEn->Oper, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_connectionWdmDataEqualizationState,
            &pEn->EqState, sizeof(pEn->EqState), 1);
    X_List_Append(pOidList, pOid);
#endif

    X_List_ForEach(pOidList, pNode)
    {
        SNMP_Oid_Append((SNMP_OidPtr)(pNode->Data), Entity_Get_Index(pEn));
    }

    ret = SNMP_Get(pSess, pErr, pOidList);

    X_List_Free(pOidList);

    return ret;
}

ErrCode
SNMP_GetEntityCnxWDMList(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, X_ListPtr pList)
{
    SNMP_OidPtr pOid, pRootOid;
    EntityCNX e;
    ErrCode ret = ERRCODE_NOERROR;

#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::connectionWdmDataFromNodeIp",
            1, &e.FromNodeIP, sizeof(int));
#else
    pOid = SNMP_Oid_New_Raw(connectionWdmDataFromNodeIp_oid,
            connectionWdmDataFromNodeIp_oid_size,
            1, ASN_IPADDRESS, &e.FromNodeIP, sizeof(int));
#endif

    pRootOid = SNMP_Oid_Copy(pOid);

    while (ret == ERRCODE_NOERROR)
    {
        ret = SNMP_GetNextOid(pSess, pErr, pOid, pRootOid);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR
                )
        {
            break;
        }

        EntityCNX_Init(&e);

        Entity_Set_Index(&e, SNMP_Oid_Index(pOid));

        ret = SNMP_GetEntityCnxWDM(pSess, pErr, &e);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR
                )
        {
            break;
        }

        X_List_Append(pList, EntityCNX_Copy(&e));
    }

    SNMP_Oid_Free(pOid);
    SNMP_Oid_Free(pRootOid);

    return ret;
}

ErrCode
SNMP_GetEntityTunnelWDM(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, EntityTunnel *pEn)
{
    SNMP_OidPtr pOid;
    X_ListPtr pOidList;
    X_ListNodePtr pNode;
    ErrCode ret;

    if(pSess == NULL || pErr == NULL || pEn == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

#if IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::deployProvTunnelWdmTunnelId", 1, &pEn->Id, sizeof(pEn->Id));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::deployProvTunnelWdmToNodeIp", 1, &pEn->ToIP, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::deployProvTunnelWdmFromAid", 1, &pEn->FromAid, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::deployProvTunnelWdmToAid", 1, &pEn->ToAid, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::controlPlaneWdmEntityStateAdmin", 1, &pEn->Admin, sizeof(int));
    X_List_Append(pOidList, pOid);
#else
    pOid = SNMP_Oid_New_Raw(deployProvTunnelWdmTunnelId_oid,
            deployProvTunnelWdmTunnelId_oid_size,
            1, ASN_OCTET_STR, pEn->Id, sizeof(pEn->Id));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(deployProvTunnelWdmToNodeIp_oid,
            deployProvTunnelWdmToNodeIp_oid_size,
            1, ASN_IPADDRESS, &pEn->ToIP, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(deployProvTunnelWdmFromAid_oid,
            deployProvTunnelWdmFromAid_oid_size,
            1, ASN_INTEGER, &pEn->FromAid, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(deployProvTunnelWdmToAid_oid,
            deployProvTunnelWdmToAid_oid_size,
            1,ASN_INTEGER, &pEn->ToAid,sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(controlPlaneWdmEntityStateAdmin_oid,
            controlPlaneWdmEntityStateAdmin_oid_size,
            1,ASN_INTEGER, &pEn->Admin,sizeof(int));
    X_List_Append(pOidList, pOid);
#endif

    X_List_ForEach(pOidList, pNode)
    {
        SNMP_Oid_Append((SNMP_OidPtr)pNode->Data, Entity_Get_Index(pEn));
    }

    ret = SNMP_Get(pSess, pErr, pOidList);

    X_List_Free(pOidList);

    return ret;
}

ErrCode
SNMP_GetEntityTunnelWDMList(netsnmp_session *pSess,SNMP_ErrCodePtr pErr, X_ListPtr pList)
{
    SNMP_OidPtr pOid, pRootOid;
    EntityTunnel e;
    unsigned int RowStatus;
    ErrCode ret = ERRCODE_NOERROR;

    if (pSess == NULL || pErr == NULL || pList == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::deployProvTunnelWdmRowStatus",
            1, &RowStatus, sizeof(int));
#else
    pOid = SNMP_Oid_New_Raw(deployProvTunnelWdmRowStatus_oid,
            deployProvTunnelWdmRowStatus_oid_size,
            1, ASN_INTEGER, &RowStatus, sizeof(int));
#endif

    pRootOid = SNMP_Oid_Copy(pOid);

    while (ret == ERRCODE_NOERROR)
    {
        ret = SNMP_GetNextOid(pSess, pErr, pOid, pRootOid);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        EntityTunnel_Init(&e);

        Entity_Set_Index(&e, SNMP_Oid_Index(pOid));

        ret = SNMP_GetEntityTunnelWDM(pSess, pErr, &e);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        X_List_Append(pList, EntityTunnel_Dup(&e));
    }

    SNMP_Oid_Free(pRootOid);
    SNMP_Oid_Free(pOid);

    return ret;
}

ErrCode
SNMP_GetEntityTEUnnumLinkList(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, X_ListPtr pList)
{
    X_ListPtr pOidList, pOidRootList;
    SNMP_OidPtr pOid;
    ErrCode ret = ERRCODE_NOERROR;
    EntityTEUnnumLink e;

    assert(pSess && pErr && pList);

    if (pSess == NULL || pErr == NULL || pList == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

    if (pOidList == NULL)
    {
        return ERRCODE_NOMEM;
    }

#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::teUnnumLinkWdmDataNodeIp",
            5, &e.NodeIP, sizeof(int));
    X_List_Append(pOidList, pOid);
    pOid = SNMP_Oid_New("FspR7-MIB::teUnnumLinkWdmDataTeLinkId",
                5, &e.LinkId, sizeof(int));
    X_List_Append(pOidList, pOid);
    pOid = SNMP_Oid_New("FspR7-MIB::teUnnumLinkWdmDataSynchronization",
            5, &e.LinkId, sizeof(int));
    X_List_Append(pOidList, pOid);
#else
    pOid = SNMP_Oid_New_Raw(teUnnumLinkWdmDataNodeIp_oid,
            teUnnumLinkWdmDataNodeIp_oid_size,
            5, ASN_IPADDRESS, &e.NodeIP, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(teUnnumLinkWdmDataTeLinkId_oid,
            teUnnumLinkWdmDataTeLinkId_oid_size,
            5, ASN_INTEGER, &e.LinkId, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(teUnnumLinkWdmDataFendNodeIp_oid,
            teUnnumLinkWdmDataFendNodeIp_oid_size,
            5, ASN_INTEGER, &e.FendNodeIP, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(teUnnumLinkWdmDataFendTeLinkId_oid,
            teUnnumLinkWdmDataFendTeLinkId_oid_size,
            5, ASN_INTEGER, &e.FendLinkId, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(teUnnumLinkWdmDataSynchronization_oid,
            teUnnumLinkWdmDataSynchronization_oid_size,
            5, ASN_INTEGER, &e.Synch, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(teUnnumLinkWdmDataPhysicalLink_oid,
            teUnnumLinkWdmDataPhysicalLink_oid_size,
            5, ASN_INTEGER, &e.PhyLink, sizeof(int));
    X_List_Append(pOidList, pOid);
#endif

    pOidRootList = X_List_Copy(pOidList, (X_ListCopy_t)SNMP_Oid_Copy);

    if (pOidRootList == NULL)
    {
        ret = ERRCODE_NOMEM;
    }

    while (ret == ERRCODE_NOERROR)
    {
        EntityTEUnnumLink_Init(&e);

        ret = SNMP_Walk(pSess, pErr, pOidList, pOidRootList);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        X_List_Append(pList, EntityTEUnnumLink_Copy(&e));
    }

    X_List_Free(pOidList);
    X_List_Free(pOidRootList);

    return ret;
}


ErrCode
SNMP_GetEntityTENumLinkList(netsnmp_session *pSess,SNMP_ErrCodePtr pErr, X_ListPtr pList)
{
    X_ListPtr pOidList, pOidRootList;
    SNMP_OidPtr pOid;
    ErrCode ret = ERRCODE_NOERROR;
    EntityTENumLink e;

    assert(pSess && pErr && pList);

    if (pSess == NULL || pErr == NULL || pList == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

    if (pOidList == NULL)
    {
        return ERRCODE_NOMEM;
    }

#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::teNumLinkWdmDataNodeIp",
              5, &e.NodeIP, sizeof(int));
      X_List_Append(pOidList, pOid);
      pOid = SNMP_Oid_New("FspR7-MIB::teNumLinkWdmDataTeLinkIp",
                  5, &e.LinkId, sizeof(int));
      X_List_Append(pOidList, pOid);
      pOid = SNMP_Oid_New("FspR7-MIB::teNumLinkWdmDataSynchronization",
              5, &e.LinkId, sizeof(int));
      X_List_Append(pOidList, pOid);
  #else
      pOid = SNMP_Oid_New_Raw(teNumLinkWdmDataNodeIp_oid,
              teNumLinkWdmDataNodeIp_oid_size,
              5, ASN_IPADDRESS, &e.NodeIP, sizeof(int));
      X_List_Append(pOidList, pOid);

      pOid = SNMP_Oid_New_Raw(teNumLinkWdmDataTeLinkIp_oid,
              teNumLinkWdmDataTeLinkIp_oid_size,
              5, ASN_INTEGER, &e.LinkId, sizeof(int));
      X_List_Append(pOidList, pOid);

      pOid = SNMP_Oid_New_Raw(teNumLinkWdmDataSynchronization_oid,
              teNumLinkWdmDataSynchronization_oid_size,
              5, ASN_INTEGER, &e.Synch, sizeof(int));
      X_List_Append(pOidList, pOid);

      pOid = SNMP_Oid_New_Raw(teNumLinkWdmDataPhysicalLink_oid,
              teNumLinkWdmDataPhysicalLink_oid_size,
              5, ASN_INTEGER, &e.PhyLink, sizeof(int));
      X_List_Append(pOidList, pOid);

      pOid = SNMP_Oid_New_Raw(teNumLinkWdmDataFendTeLinkIp_oid,
              teNumLinkWdmDataFendTeLinkIp_oid_size,
              5, ASN_INTEGER, &e.FendLinkId, sizeof(int));
      X_List_Append(pOidList, pOid);

      pOid = SNMP_Oid_New_Raw(teNumLinkWdmDataFendNodeIp_oid,
              teNumLinkWdmDataFendNodeIp_oid_size,
              5, ASN_INTEGER, &e.FendNodeIP, sizeof(int));
      X_List_Append(pOidList, pOid);
      #endif

      pOidRootList = X_List_Copy(pOidList, (X_ListCopy_t)SNMP_Oid_Copy);

      if (pOidRootList == NULL)
      {
          ret = ERRCODE_NOMEM;
      }

      while (ret == ERRCODE_NOERROR)
      {
          EntityTENumLink_Init(&e);

          ret = SNMP_Walk(pSess, pErr, pOidList, pOidRootList);

          if (ret != ERRCODE_NOERROR ||
                  pErr->Status != STAT_SUCCESS ||
                  pErr->Err != SNMP_ERR_NOERROR)
          {
              break;
          }

          X_List_Append(pList, EntityTENumLink_Copy(&e));
      }

      X_List_Free(pOidList);
      X_List_Free(pOidRootList);

      return ret;
}

///

ErrCode
SNMP_GetPathWdm(netsnmp_session *pSession, SNMP_ErrCodePtr pErr, EntityPath *pEntity)
{
    SNMP_OidPtr         pOid;
    ErrCode             ret;

    if (pSession == NULL || pErr == NULL || pEntity == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_pathWdmDataPathId,
            pEntity->PathID, sizeof(pEntity->PathID), 1);

    SNMP_Oid_Append(pOid, Entity_Get_Index(pEntity));

    ret = SNMP_GetOid(pSession, pErr, pOid);

    SNMP_Oid_Free(pOid);

    return ret;
}

ErrCode
SNMP_GetPEL(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, EntityPathElement *pEn)
{
    ErrCode ret;
    SNMP_OidPtr pOid;
    X_ListPtr pOidList;
    X_ListNodePtr pNode;

    if (pSess == NULL || pErr == NULL || pEn == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

    pOid = SNMP_Oid_New_Raw(pathElementWdmDataTrafficEngineeringType_oid,
            pathElementWdmDataTrafficEngineeringType_oid_size,
            1, ASN_INTEGER, &pEn->Type, sizeof(pEn->Type));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(pathElementWdmDataLifIp_oid,
            pathElementWdmDataLifIp_oid_size,
            1, ASN_IPADDRESS, &pEn->LifIP, sizeof(pEn->LifIP));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(pathElementWdmDataLogicalInterfaceId_oid,
            pathElementWdmDataLogicalInterfaceId_oid_size,
            1, ASN_IPADDRESS, &pEn->LifID, sizeof(pEn->LifID));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(pathElementWdmDataNodeIp_oid,
            pathElementWdmDataNodeIp_oid_size,
            1, ASN_IPADDRESS, &pEn->NodeIP, sizeof(pEn->NodeIP));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(pathElementWdmDataSequenceNumber_oid,
            pathElementWdmDataSequenceNumber_oid_size,
            1, ASN_INTEGER, &pEn->Seq, sizeof(pEn->Seq));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(pathElementWdmDataPathId_oid,
            pathElementWdmDataPathId_oid_size,
            1, ASN_OCTET_STR, pEn->PathID, sizeof(pEn->PathID));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(pathElementWdmDataChannelUp_oid,
            pathElementWdmDataChannelUp_oid_size,
            1, ASN_INTEGER, &pEn->UpChannel, sizeof(pEn->UpChannel));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(pathElementWdmDataChannelDown_oid,
            pathElementWdmDataChannelDown_oid_size,
            1, ASN_INTEGER, &pEn->DownChannel, sizeof(pEn->DownChannel));
    X_List_Append(pOidList, pOid);

    X_List_ForEach(pOidList, pNode)
    {
        SNMP_Oid_Append((SNMP_OidPtr)pNode->Data, Entity_Get_Index(pEn));
    }

    ret = SNMP_Get(pSess, pErr, pOidList);

    X_List_Free(pOidList);

    return ret;
}

ErrCode
SNMP_GetPELs(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, const EntityPath *pEn, X_ListPtr pList)
{
    ErrCode ret = ERRCODE_NOERROR;
    SNMP_OidPtr pOid, pOidRoot;
    EntityPathElement PEL;
    char PathId[32] = { '\0', };

    if (pSess == NULL || pErr == NULL || pEn == NULL || pList == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_Raw(pathElementWdmDataPathId_oid,
            pathElementWdmDataPathId_oid_size,
            1, ASN_OCTET_STR, PathId, sizeof(PathId));

    pOidRoot = SNMP_Oid_Copy(pOid);

    while (ret == ERRCODE_NOERROR)
    {
        ret = SNMP_GetNextOid(pSess, pErr, pOid, pOidRoot);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        if (strcmp(PathId, pEn->PathID))
        {
            continue;
        }

        EntityPathElement_Init(&PEL);

        Entity_Set_Index(&PEL, SNMP_Oid_Index(pOid));

        ret = SNMP_GetPEL(pSess, pErr, &PEL);

        if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        X_List_Append(pList, EntityPathElement_Dup(&PEL));
    }

    if (ret == ERRCODE_NOERROR_END) ret = ERRCODE_NOERROR;
    return ret;
}

/**
 * Find and return first matching CNX for tunnel.
 */
ErrCode
SNMP_GetCnxWDMForTunnel(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, const EntityTunnel *pTnl, EntityCNX *pCnx)
{
    SNMP_OidPtr pOid, pRootOid;
    ErrCode err = ERRCODE_NOERROR;
    char Id[32];

    if (pSess == NULL || pErr == NULL || pTnl == NULL || pCnx == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_Raw(connectionWdmDataTunnelId_oid,
            connectionWdmDataTunnelId_oid_size,
            1, ASN_OCTET_STR, Id, sizeof(Id));

    pRootOid = SNMP_Oid_Copy(pOid);

    while (err == ERRCODE_NOERROR)
    {
        err = SNMP_GetNextOid(pSess, pErr, pOid, pRootOid);

        if (err != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        if (!strcmp(Id, pTnl->Id))
        {
            break;
        }
    }

    if (err == ERRCODE_NOERROR)
    {
        EntityCNX_Init(pCnx);
        Entity_Set_Index(pCnx, SNMP_Oid_Index(pOid));
        err = SNMP_GetEntityCnxWDM(pSess, pErr, pCnx);
    }

    SNMP_Oid_Free(pOid);
    SNMP_Oid_Free(pRootOid);

    return (err == ERRCODE_NOERROR || err == ERRCODE_NOERROR_END) ? ERRCODE_NOERROR : err;
}

ErrCode
SNMP_GetSignalledPath(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, const EntityCNX *pCnx, X_ListPtr pList)
{
    SNMP_OidPtr pOid;
    ErrCode ret;
    char Idx[32];
    EntityPath Path;

    if (pSess == NULL || pErr == NULL || pCnx == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    EntityPath_Init(&Path);

    pOid = SNMP_Oid_New_Raw(connectionWdmDataSignalledPaths_oid,
            connectionWdmDataSignalledPaths_oid_size,
            1, ASN_OCTET_STR, Idx, sizeof(Idx));

    SNMP_Oid_Append(pOid, Entity_Get_Index(pCnx));

    ret = SNMP_GetOid(pSess, pErr, pOid);

    if (ret != ERRCODE_NOERROR ||
            pErr->Status != STAT_SUCCESS ||
            pErr->Err != SNMP_ERR_NOERROR)
    {
        SNMP_Oid_Free(pOid);
        return ret;
    }

    Entity_Set_Index(&Path, atoi(Idx));

    X_LOGINFO(1, "Path index = %d (%08x)",
            (int)Entity_Get_Index(&Path),(int)Entity_Get_Index(&Path));

    ret = SNMP_GetPathWdm(pSess, pErr, &Path);

    if (ret != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
    {
        SNMP_Oid_Free(pOid);
        return ret;
    }

    ret = SNMP_GetPELs(pSess, pErr, &Path, pList);

    if (ret != ERRCODE_NOERROR ||
                   pErr->Status != STAT_SUCCESS ||
                   pErr->Err != SNMP_ERR_NOERROR)
    {
        SNMP_Oid_Free(pOid);
        return ret;
    }

    X_LOGINFO(X_LOG_INTF_ID, "PELs: %d", X_List_Size(pList));

    SNMP_Oid_Free(pOid);
    return ret;
}

/*
 * Get the computed path for a tunnel.
 */
ErrCode
SNMP_GetComputedPath(netsnmp_session *pSession, SNMP_ErrCodePtr pErr, unsigned long idx, X_ListPtr pList)
{
    SNMP_OidPtr         pOid;
    unsigned int        PathIdx;
    ErrCode             err;
    EntityPath          EntPath;

    if (pSession == NULL || pErr == NULL || pList == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New("FspR7-MIB::tunnelWdmDataComputedPaths", 1, &PathIdx, sizeof(PathIdx));
    SNMP_Oid_Append(pOid, idx);

    err = SNMP_GetOid(pSession, pErr, pOid);

    SNMP_Oid_Free(pOid);

    if (err != ERRCODE_NOERROR ||
            pErr->Status != STAT_SUCCESS)
    {
        return err;
    }

    if (pErr->Err != SNMP_ERR_NOERROR)
    {
        return err;
    }

    if (PathIdx == 0)
    {
        return err;
    }

    EntityPath_Init(&EntPath);

    Entity_Set_Index(&EntPath, PathIdx);

    err = SNMP_GetPELs(pSession, pErr, &EntPath, pList);

    return err;
}

ErrCode
SNMP_GetUnusedWDMTunnelIndex(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, unsigned long *idx)
{
    ErrCode ret;
    SNMP_OidPtr pOid;

    if (pSess == NULL || pErr == NULL || idx == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::unusedWdmTunnelIndex",1,idx,sizeof(idx));
#else
    pOid = SNMP_Oid_New_Raw(unusedWdmTunnelIndex_oid,
            unusedWdmTunnelIndex_oid_size,
            1, ASN_INTEGER, idx, sizeof(idx));
#endif

    if (pOid == NULL)
    {
        return ERRCODE_NOMEM;
    }

    ret = SNMP_GetNextOid(pSess, pErr, pOid, NULL);

    SNMP_Oid_Free(pOid);

    return ret;
}

ErrCode
SNMP_GetEntityClass(netsnmp_session *pSess, SNMP_ErrCodePtr pErr,
        unsigned long idx, unsigned int *pClass)
{
    SNMP_OidPtr pOid;
    ErrCode ret;

    if (pSess == NULL || pErr == NULL || idx == 0 || pClass == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_entityClass,
            pClass, sizeof(*pClass), 1);

    SNMP_Oid_Append(pOid, idx);

    ret = SNMP_GetOid(pSess, pErr, pOid);

    SNMP_Oid_Free(pOid);

    return ret;
}

ErrCode
SNMP_GetCPWDMEntityClass(netsnmp_session *pSess, SNMP_ErrCodePtr pErr,
        unsigned long idx, unsigned int *pClass)
{
    SNMP_OidPtr pOid;
    ErrCode ret;

    if (pSess == NULL || pErr == NULL || pClass == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New_Raw("ADVA-MIB::controlPlaneWdmEntityClass", 1, pClass, sizeof(int));
#else
    pOid = SNMP_Oid_New_Raw(controlPlaneWdmEntityClass_oid,
            controlPlaneWdmEntityClass_oid_size,
            1, ASN_INTEGER, pClass, sizeof(int));
#endif

    SNMP_Oid_Append(pOid, idx);

    ret = SNMP_GetOid(pSess, pErr, pOid);

    SNMP_Oid_Free(pOid);

    return ret;
}

ErrCode
SNMP_CreateTunnel(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, EntityTunnel *pEn)
{
    ErrCode ret;
    SNMP_OidPtr pOid, pRowStatusOid;
    X_ListPtr pOidList;
    unsigned int RowStatus;
    X_ListNodePtr pNode;

    if (pSess == NULL || pErr == NULL || pEn == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    ret = SNMP_GetUnusedWDMTunnelIndex(pSess, pErr, &pEn->Generic.Index);

    if (ret != ERRCODE_NOERROR ||
            pErr->Status != STAT_SUCCESS ||
            pErr->Err != SNMP_ERR_NOERROR)
    {
        X_LOGERR(X_LOG_INTF_ID, "SNMP_CreateWDMTunnel: unable to get unused WDM tunnel index");
        return ret;
    }

    X_LOGDBG(X_LOG_INTF_ID, "SNMP_CreateWDMTunnel: unused WDM tunnel index = %08x",
            (int)pEn->Generic.Index);

#ifdef IMPORT_MIB
    pRowStatusOid = SNMP_Oid_New("FspR7-MIB::deployProvTunnelWdmRowStatus",
            1, &RowStatus, sizeof(int));
#else
    pRowStatusOid = SNMP_Oid_New_Raw(deployProvTunnelWdmRowStatus_oid,
            deployProvTunnelWdmRowStatus_oid_size,
            1, ASN_INTEGER, &RowStatus, sizeof(int));
#endif

    SNMP_Oid_Append(pRowStatusOid, pEn->Generic.Index);

    RowStatus = SNMP_ROW_CREATEANDWAIT;

    ret = SNMP_SetOid(pSess, pErr, pRowStatusOid);

    if (ret != ERRCODE_NOERROR ||
              pErr->Status != STAT_SUCCESS ||
              pErr->Err != SNMP_ERR_NOERROR)
    {
        X_LOGERR(X_LOG_INTF_ID, "SNMP_CreateWDMTunnel: error occured while setting CREATEANDWAIT");
       // SNMP_Oid_Free(pRowStatusOid);
        ///return ret;
    }

    X_LOGINFO(X_LOG_INTF_ID, "SNMP_CreateWDMTunnel: id=%s toip = %08x from=%08x to=%08x",
            pEn->Id, pEn->ToIP, pEn->FromAid, pEn->ToAid);


    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

#if IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::deployProvTunnelWdmTunnelId", 1, &pEn->Id, sizeof(pEn->Id));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::deployProvTunnelWdmToNodeIp", 1, &pEn->ToIP, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::deployProvTunnelWdmFromAid", 1, &pEn->FromAid, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::deployProvTunnelWdmToAid", 1, &pEn->ToAid, sizeof(int));
    X_List_Append(pOidList, pOid);
#else
    pOid = SNMP_Oid_New_Raw(deployProvTunnelWdmTunnelId_oid,
            deployProvTunnelWdmTunnelId_oid_size,
            1, ASN_OCTET_STR, pEn->Id, sizeof(pEn->Id));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(deployProvTunnelWdmToNodeIp_oid,
            deployProvTunnelWdmToNodeIp_oid_size,
            1, ASN_IPADDRESS, &pEn->ToIP, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(deployProvTunnelWdmFromAid_oid,
            deployProvTunnelWdmFromAid_oid_size,
            1, ASN_INTEGER, &pEn->FromAid, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(deployProvTunnelWdmToAid_oid,
            deployProvTunnelWdmToAid_oid_size,
            1,ASN_INTEGER, &pEn->ToAid,sizeof(int));
    X_List_Append(pOidList, pOid);

    if (pEn->Facility != 0)
    {
        pOid = SNMP_Oid_New_Raw(deployProvTunnelWdmFacilityType_oid,
                deployProvTunnelWdmFacilityType_oid_size,
                1,ASN_INTEGER, &pEn->Facility,sizeof(int));
        X_List_Append(pOidList, pOid);
    }

#endif

    X_List_ForEach(pOidList, pNode)
    {
        SNMP_Oid_Append((SNMP_OidPtr)pNode->Data, Entity_Get_Index(pEn));
    }

    ret = SNMP_Set(pSess, pErr, pOidList);

    if (ret != ERRCODE_NOERROR ||
                 pErr->Status != STAT_SUCCESS ||
                 pErr->Err != SNMP_ERR_NOERROR)
    {
        X_LOGERR(X_LOG_INTF_ID, "SNMP_CreateWDMTunnel: unable to create tunnel");
        X_List_Free(pOidList);
        SNMP_Oid_Free(pRowStatusOid);
        return ret;
    }

    RowStatus = SNMP_ROW_ACTIVE;

    ret = SNMP_SetOid(pSess, pErr, pRowStatusOid);

    if (ret != ERRCODE_NOERROR ||
            pErr->Status != STAT_SUCCESS ||
            pErr->Err != SNMP_ERR_NOERROR)
    {
        X_LOGERR(X_LOG_INTF_ID, "SNMP_CreateWDMTunnel: unable to activate tunnel");
        X_List_Free(pOidList);
        SNMP_Oid_Free(pRowStatusOid);
        return ret;
    }


    ret = SNMP_GetEntityTunnelWDM(pSess, pErr, pEn);

    X_List_Free(pOidList);
    SNMP_Oid_Free(pRowStatusOid);
    return ret;
}

ErrCode
SNMP_SetTunnelAdmin(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, EntityTunnel *pEn, unsigned int admin)
{
    ErrCode ret;
    SNMP_OidPtr pOid;

    if (pSess == NULL || pEn == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::controlPlaneWdmEntityStateAdmin", 1, &admin, sizeof(int));
#else
    pOid = SNMP_Oid_New_Raw(controlPlaneWdmEntityStateAdmin_oid,
            controlPlaneWdmEntityStateAdmin_oid_size,
            1,ASN_INTEGER, &admin,sizeof(int));
#endif

    SNMP_Oid_Append(pOid, Entity_Get_Index(pEn));

    ret = SNMP_SetOid(pSess, pErr, pOid);

    SNMP_Oid_Free(pOid);

    return ret;
}

ErrCode
SNMP_GetTunnelResult(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, EntityTunnelResult *pResult)
{
    X_ListNodePtr pNode;
    X_ListPtr pOidList;
    SNMP_OidPtr pOid;
    ErrCode ret;

    if (pSess == NULL || pErr == NULL || pResult == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::tunnelWdmDataComputationState",
            1, &pResult->ComputationState, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::tunnelWdmDataErrorCategory",
            1, &pResult->ErrCategory, sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New("FspR7-MIB::controlPlaneWdmEntityStateAdmin",
            1, &pResult->Admin, sizeof(int));
    X_List_Append(pOidList, pOid);
#else
    pOid = SNMP_Oid_New_Raw(tunnelWdmDataComputationState_oid,
            tunnelWdmDataComputationState_oid_size,
            1,ASN_INTEGER,&pResult->ComputationState,sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(tunnelWdmDataErrorCategory_oid,
            tunnelWdmDataErrorCategory_oid_size,
            1,ASN_INTEGER,&pResult->ErrCategory,sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(controlPlaneWdmEntityStateAdmin_oid,
            controlPlaneWdmEntityStateAdmin_oid_size,
            1,ASN_INTEGER, &pResult->Admin,sizeof(int));
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_Raw(controlPlaneWdmEntityStateSecondaryStates_oid,
            controlPlaneWdmEntityStateSecondaryStates_oid_size,
            1,ASN_BIT_STR, &pResult->SecondayStates,sizeof(int));
    X_List_Append(pOidList, pOid);
#endif

    X_List_ForEach(pOidList, pNode)
    {
        SNMP_Oid_Append((SNMP_OidPtr)pNode->Data, pResult->Index);
    }

    ret = SNMP_Get(pSess, pErr, pOidList);

    X_List_Free(pOidList);

    return ret;
}

ErrCode
SNMP_DestroyTunnel(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, long idx)
{
    ErrCode ret;
    SNMP_OidPtr pOid;
    int CMD;

    if (pSess == NULL || pErr == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    CMD = SNMP_ROW_DESTROY;

#ifdef IMPORT_MIB
    pOid = SNMP_Oid_New("FspR7-MIB::deployProvTunnelWdmRowStatus",
            1, &CMD, sizeof(CMD));
#else
    pOid = SNMP_Oid_New_Raw(deployProvTunnelWdmRowStatus_oid,
            deployProvTunnelWdmRowStatus_oid_size,
            1, ASN_INTEGER,&CMD,sizeof(CMD));
#endif

    SNMP_Oid_Append(pOid, idx);

    ret = SNMP_SetOid(pSess, pErr, pOid);

    SNMP_Oid_Free(pOid);

    return ret;
}

ErrCode
SNMP_GetAuxData(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, AuxEntityData *pData)
{
    SNMP_OidPtr pOid;
    X_ListPtr pList;
    X_ListNodePtr pNode;
    ErrCode err;

    if (pSess == NULL || pErr == NULL || pData == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_entityIndexAid,
            pData->Aid, sizeof(pData->Aid), 1);
    X_List_Append(pList, pOid);

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_entityClass,
            &pData->Class, sizeof(pData->Class), 1);
    X_List_Append(pList, pOid);

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_entityAssignmentState,
            &pData->Assignment, sizeof(pData->Assignment), 1);
    X_List_Append(pList, pOid);

    X_List_ForEach(pList, pNode)
    {
        pOid = (SNMP_OidPtr)(pNode->Data);
        SNMP_Oid_Append(pOid, pData->Index);
    }

    err = SNMP_Get(pSess, pErr, pList);

    X_List_Free(pList);

    return err;
}

ErrCode
SNMP_GetAuxData_CP_WDM(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, AuxEntityData *pData)
{
    SNMP_OidPtr         pOid;
    X_ListPtr           pList;
    X_ListNodePtr       pNode;
    ErrCode             err;

    if (pSess == NULL || pErr == NULL || pData == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

    if (pList == NULL)
    {
        return ERRCODE_NOMEM;
    }

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_controlPlaneWdmEntityIndexAid,
            pData->Aid, sizeof(pData->Aid), 1);
    if (pOid != NULL)
        X_List_Append(pList, pOid);

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_controlPlaneWdmEntityClass,
                &pData->Class, sizeof(pData->Class), 1);
    if (pOid != NULL)
        X_List_Append(pList, pOid);

    X_List_ForEach(pList, pNode)
    {
        pOid = (SNMP_OidPtr)(pNode->Data);
        SNMP_Oid_Append(pOid, pData->Index);
    }

    err = SNMP_Get(pSess, pErr, pList);

    X_List_Free(pList);

    return err;
}

ErrCode
SNMP_Contains(netsnmp_session *pSession, SNMP_ErrCodePtr pErr,
        unsigned long idx, X_ListPtr pList)
{
    SNMP_OidPtr         pOid, pRoot;
    unsigned long       oid;
    ErrCode             err = ERRCODE_NOERROR;
    AuxEntityData       aux;

    if (pSession == NULL || pErr == NULL || pList == NULL || idx == 0)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_containsIndex,
            &oid, sizeof(oid), 1);
    SNMP_Oid_Append(pOid, idx);

    pRoot = SNMP_Oid_Copy(pOid);

    while (err == ERRCODE_NOERROR)
    {
        err = SNMP_GetNextOid(pSession, pErr, pOid, pRoot);

        if (err != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        AuxEntityData_Init(&aux);

        aux.Index = SNMP_Oid_Index(pOid);

        err = SNMP_GetAuxData(pSession, pErr, &aux);

        if (err != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        X_List_Append(pList, AuxEntityData_Dup(&aux));
    }

    SNMP_Oid_Free(pOid);
    SNMP_Oid_Free(pRoot);

    return (err == ERRCODE_NOERROR || err == ERRCODE_NOERROR_END) ? ERRCODE_NOERROR : err;
}

ErrCode
SNMP_Contains_CP_WDM(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, unsigned long idx, X_ListPtr pList)
{
    SNMP_OidPtr pOid, pRoot;
    unsigned long oid;
    ErrCode err = ERRCODE_NOERROR;
    AuxEntityData aux;

    if (pSess == NULL || pErr == NULL || pList == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_controlPlaneWdmContainsIndex,
            &oid, sizeof(oid), 1);

    if (pOid == NULL)
    {
        return ERRCODE_INTERNAL_ERROR;
    }

    SNMP_Oid_Append(pOid, idx);

    pRoot = SNMP_Oid_Copy(pOid);

    while (err == ERRCODE_NOERROR)
    {
        err = SNMP_GetNextOid(pSess, pErr, pOid, pRoot);

        if (err != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        AuxEntityData_Init(&aux);

        aux.Index = SNMP_Oid_Index(pOid);

        err = SNMP_GetAuxData_CP_WDM(pSess, pErr, &aux);

        if (err != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS ||
                pErr->Err != SNMP_ERR_NOERROR)
        {
            break;
        }

        X_List_Append(pList, AuxEntityData_Dup(&aux));
    }

    SNMP_Oid_Free(pOid);
    SNMP_Oid_Free(pRoot);

    return (err == ERRCODE_NOERROR || err == ERRCODE_NOERROR_END) ? ERRCODE_NOERROR : err;
}

ErrCode
SNMP_Create_OM_Facility(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, unsigned long idx)
{
    ErrCode err;
    SNMP_OidPtr pOid, pOidType;
    unsigned int rowStatus;
    unsigned int facType;

    if (pSess == NULL || pErr == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    rowStatus = SNMP_ROW_CREATEANDWAIT;

    pOid = SNMP_Oid_New_Raw(deployProvIfRowStatus_oid,
            deployProvIfRowStatus_oid_size,
            1, ASN_INTEGER, &rowStatus, sizeof(rowStatus));

    SNMP_Oid_Append(pOid, idx);

    err = SNMP_SetOid(pSess, pErr, pOid);

    facType = FacilityType_IFTYPEOM;

    pOidType = SNMP_Oid_New_Raw(deployProvIfType_oid,
            deployProvIfType_oid_size,
            1, ASN_INTEGER, &facType, sizeof(facType));
    SNMP_Oid_Append(pOidType, idx);

    err = SNMP_SetOid(pSess, pErr, pOidType);

    rowStatus = SNMP_ROW_ACTIVE;

    err = SNMP_SetOid(pSess, pErr, pOid);

    return err;
}

ErrCode
SNMP_Create_CH_Facility(netsnmp_session *pSess, SNMP_ErrCodePtr pErr,
        unsigned long Index, unsigned int FacType, unsigned int Channel)
{
    ErrCode             Err;
    SNMP_OidPtr         pOid, pOidFacType, pOidCh;
    unsigned int        RowStatus;

    if (pSess == NULL || pErr == NULL || Index == 0)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    RowStatus = SNMP_ROW_CREATEANDWAIT;

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvIfRowStatus,
            &RowStatus, sizeof(RowStatus), 1);
    SNMP_Oid_Append(pOid, Index);

    Err = SNMP_SetOid(pSess, pErr, pOid);

    if (Err != ERRCODE_NOERROR)
    {
        SNMP_Oid_Free(pOid);
        return Err;
    }

    if (Err == ERRCODE_NOERROR && FacType != 0)
    {
        pOidFacType = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvIfType,
                &FacType, sizeof(FacType), 1);
        SNMP_Oid_Append(pOidFacType, Index);
        Err = SNMP_SetOid(pSess, pErr, pOidFacType);
        SNMP_Oid_Free(pOidFacType);
    }

    if (Err == ERRCODE_NOERROR && Channel != 0)
    {
        pOidCh = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvIfChannelProvisioning,
                &Channel, sizeof(Channel), 1);
        SNMP_Oid_Append(pOidCh, Index);
        Err = SNMP_SetOid(pSess, pErr, pOidCh);
        SNMP_Oid_Free(pOidCh);
    }

    if (Err == ERRCODE_NOERROR)
    {
        RowStatus = SNMP_ROW_ACTIVE;

        Err = SNMP_SetOid(pSess, pErr, pOid);
    }

    SNMP_Oid_Free(pOid);

    return Err;
}

ErrCode
SNMP_Create_VCH_Facility(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, unsigned long Index)
{
    ErrCode             err;
    SNMP_OidPtr         pOid;
    unsigned int        RowStatus;

    if (pSess == NULL || pErr == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    RowStatus = SNMP_ROW_CREATEANDWAIT;

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvVchChannelRowStatus,
            &RowStatus, sizeof(RowStatus), 1);
    SNMP_Oid_Append(pOid, Index);

    err = SNMP_SetOid(pSess, pErr, pOid);

    if (err != ERRCODE_NOERROR)
    {
        SNMP_Oid_Free(pOid);
        return err;
    }

    RowStatus = SNMP_ROW_ACTIVE;

    err = SNMP_SetOid(pSess, pErr, pOid);

    SNMP_Oid_Free(pOid);

    return err;
}

/**
 * Create WCH entity.
 */
ErrCode
SNMP_Create_WCH(netsnmp_session *pSession, SNMP_ErrCodePtr pErr, unsigned long Index)
{
    SNMP_OidPtr         pOid;
    unsigned int        RowStatus;
    ErrCode             Err;

    if (pSession == NULL || pErr == NULL || Index == 0)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvVirtualOpticalChannelRowStatus,
            &RowStatus, sizeof(RowStatus), 1);
    SNMP_Oid_Append(pOid, Index);

    RowStatus = SNMP_ROW_CREATEANDWAIT;

    Err = SNMP_SetOid(pSession, pErr, pOid);

    RowStatus = SNMP_ROW_ACTIVE;

    Err = SNMP_SetOid(pSession, pErr, pOid);

    SNMP_Oid_Free(pOid);

    return Err;
}

/**
 * Destroy WCH entity.
 */
ErrCode
SNMP_Destroy_WCH(netsnmp_session *pSession, SNMP_ErrCodePtr pErr, unsigned long Index)
{
    SNMP_OidPtr         pOid;
    unsigned int        RowStatus;
    ErrCode             Err;

    if (pSession == NULL || pErr == NULL || Index == 0)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvVirtualOpticalChannelRowStatus,
            &RowStatus, sizeof(RowStatus), 1);
    SNMP_Oid_Append(pOid, Index);

    RowStatus = SNMP_ROW_DESTROY;

    Err = SNMP_SetOid(pSession, pErr, pOid);

    SNMP_Oid_Free(pOid);

    if (Err == ERRCODE_NOOBJECT)
        Err = ERRCODE_NOERROR;

    return Err;
}

ErrCode
SNMP_Destroy_If(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, unsigned long Index)
{
    ErrCode             Err;
    SNMP_OidPtr         pOid;
    unsigned int        AdminCap;
    unsigned int        Admin;
    unsigned int        RowStatus;

    if (pSess == NULL || pErr == NULL || Index == 0)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    Err = SNMP_GetEntityAdminCap(pSess, pErr, Index, &AdminCap);

    if (Err != ERRCODE_NOERROR || pErr->Status == STAT_TIMEOUT)
    {
        return Err;
    }

    if (pErr->Err == SNMP_ERR_NOSUCHNAME)
    {
        return ERRCODE_NOERROR;
    }

    Admin = GetAdminState(AdminCap, 1);

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvIfAdmin,
            &Admin, sizeof(Admin), 1);
    SNMP_Oid_Append(pOid, (oid)Index);

    Err = SNMP_SetOid(pSess, pErr, pOid);

    SNMP_Oid_Free(pOid);

    if (Err != ERRCODE_NOERROR || pErr->Status == STAT_TIMEOUT)
    {
        return Err;
    }

    RowStatus = SNMP_ROW_DESTROY;

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvIfRowStatus,
            &RowStatus, sizeof(RowStatus), 1);
    SNMP_Oid_Append(pOid, (oid)Index);

    Err = SNMP_SetOid(pSess, pErr, pOid);

    SNMP_Oid_Free(pOid);

    return Err;
}

ErrCode
SNMP_Destroy_VCH(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, unsigned long Index)
{
    ErrCode             Err;
    SNMP_OidPtr         pOid;
    //unsigned int        Cmd = 2;
    unsigned int        Admin;
    unsigned int        AdminCaps;
    unsigned int        RowStatus;

    if (pSess == NULL || pErr == NULL || Index == 0)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    Err = SNMP_GetEntityAdminCap(pSess, pErr, Index, &AdminCaps);

    if (Err != ERRCODE_NOERROR || pErr->Status == STAT_TIMEOUT)
    {
        return Err;
    }

    if (pErr->Err == SNMP_ERR_NOSUCHNAME)
    {
        return ERRCODE_NOERROR;
    }

    Admin = GetAdminState(AdminCaps, 1);

#if 1
    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvVchChannelAdmin,
            &Admin, sizeof(Admin), 1);
    SNMP_Oid_Append(pOid, (oid)Index);

    Err = SNMP_SetOid(pSess, pErr, pOid);

    SNMP_Oid_Free(pOid);

    if (Err != ERRCODE_NOERROR ||
            pErr->Status == STAT_TIMEOUT)
    {
        return Err;
    }

    if (pErr->Err == SNMP_ERR_NOSUCHNAME)
    {
        return ERRCODE_NOERROR;
    }
#endif

    RowStatus = SNMP_ROW_DESTROY;

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvVchChannelRowStatus,
            &RowStatus, sizeof(RowStatus), 1);
    SNMP_Oid_Append(pOid, (oid)Index);

    Err = SNMP_SetOid(pSess, pErr, pOid);

    SNMP_Oid_Free(pOid);

    return Err;
}

/**
 * Create CRS entity
 */
ErrCode
SNMP_Create_Crs(netsnmp_session *pSess, SNMP_ErrCodePtr pErr,
        unsigned long From, unsigned long To, unsigned int FacType, unsigned int PathNode)
{
    ErrCode             Err;
    unsigned int        RowStatus;
    unsigned int        Conn;
    SNMP_OidPtr         pOid, pOidConn, pOidFacType, pOidPathNode;

    if (pErr == NULL || pSess == NULL || From == 0 || To == 0)
    {
        X_LOGERR(X_LOG_INTF_ID, "SNMP_Create_Crs: invalid arguments");
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_crsConnDeployProvRowStatus,
            &RowStatus, sizeof(RowStatus), 2);
    SNMP_Oid_Append(pOid, (oid)From);
    SNMP_Oid_Append(pOid, (oid)To);

    RowStatus = SNMP_ROW_CREATEANDWAIT;

    Err = SNMP_SetOid(pSess, pErr, pOid);

    if (Err != ERRCODE_NOERROR)
    {
        SNMP_Oid_Free(pOid);
        return Err;
    }

    // bi-direction
    Conn = 1;

    pOidConn = SNMP_Oid_New_From_Node(&FspR7_MIB_crsConnDeployProvConn,
            &Conn, sizeof(Conn), 2);
    SNMP_Oid_Append(pOidConn, (oid)From);
    SNMP_Oid_Append(pOidConn, (oid)To);

    Err = SNMP_SetOid(pSess, pErr, pOidConn);

    if (FacType != 0)
    {
        pOidFacType = SNMP_Oid_New_From_Node(&FspR7_MIB_crsConnDeployProvType,
                &FacType, sizeof(FacType), 2);
        SNMP_Oid_Append(pOidFacType, (oid)From);
        SNMP_Oid_Append(pOidFacType, (oid)To);

        Err = SNMP_SetOid(pSess, pErr, pOidFacType);

        SNMP_Oid_Free(pOidFacType);
    }

    if (PathNode != 0)
    {
        pOidPathNode = SNMP_Oid_New_From_Node(&FspR7_MIB_crsConnDeployProvPathNode,
                &PathNode,sizeof(PathNode),2);
        SNMP_Oid_Append(pOidPathNode,(oid) From);
        SNMP_Oid_Append(pOidPathNode,(oid) To);

        Err = SNMP_SetOid(pSess,pErr,pOidPathNode);

        SNMP_Oid_Free(pOidPathNode);
    }

    RowStatus = SNMP_ROW_ACTIVE;

    Err = SNMP_SetOid(pSess, pErr, pOid);

    SNMP_Oid_Free(pOid);
    SNMP_Oid_Free(pOidConn);

    return Err;
}

ErrCode
SNMP_Get_Crs(netsnmp_session *pSess, SNMP_ErrCodePtr pErr,
        unsigned int From, unsigned int *To)
{
    SNMP_OidPtr         pOid, pOidRoot;
    ErrCode             err;
    unsigned int        Tmp;


    if (pSess == NULL || pErr == NULL || From == 0 || To == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_crsConnDeployProvRowStatus,
            &Tmp, sizeof(Tmp), 2);
    SNMP_Oid_Append(pOid, (oid)From);

    pOidRoot = SNMP_Oid_Copy(pOid);

    err = SNMP_GetNextOid(pSess, pErr, pOid, pOidRoot);

    if (err == ERRCODE_NOERROR && pErr->Status == STAT_SUCCESS)
    {
        *To = SNMP_Oid_Index(pOid);
    }

    SNMP_Oid_Free(pOid);
    SNMP_Oid_Free(pOidRoot);

    return err;
}


ErrCode
SNMP_CreatePath(netsnmp_session *pSess, SNMP_ErrCodePtr pErr,
        EntityPath *pPath, X_ListPtr pPathElementList)
{
    SNMP_OidPtr pOid, pPathRowStatusOid, pPathIdOid;
    unsigned int RowStatus;
    ErrCode ret;
    X_ListPtr pAuxDataList;
    X_ListNodePtr pNode1, pNode2;
    EntityPathElement *pPEL;
    AuxEntityData *pAuxData;
    char *p;
    unsigned int Seq;

    if (pSess == NULL || pErr == NULL || pPath == NULL || pPathElementList == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    // check if invalid path name
    if (pPath->PathID[0] == '\0')
    {
        return ERRCODE_INVALID_PARAMS;
    }

    // 1. Get unused WDM path index

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_unusedWdmPathIndex,
            &pPath->Generic.Index, sizeof(pPath->Generic.Index), 1);

    if (pOid == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    ret = SNMP_GetNextOid(pSess, pErr, pOid, NULL);

    SNMP_Oid_Free(pOid);

    if (ret != ERRCODE_NOERROR)
    {
        return ret;
    }

    // 2. Create new PATH entity
    pPathRowStatusOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvPathWdmRowStatus,
            &RowStatus, sizeof(RowStatus), 1);

    pPathIdOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvPathWdmPathId,
            pPath->PathID, strlen(pPath->PathID), 1);

    if (pPathRowStatusOid == NULL || pPathIdOid == NULL)
    {
        if (pPathRowStatusOid != NULL)
            SNMP_Oid_Free(pPathRowStatusOid);

        if (pPathIdOid != NULL)
            SNMP_Oid_Free(pPathIdOid);

        return ERRCODE_INVALID_PARAMS;
    }

    SNMP_Oid_Append(pPathRowStatusOid, pPath->Generic.Index);
    SNMP_Oid_Append(pPathIdOid, pPath->Generic.Index);

    RowStatus = SNMP_ROW_CREATEANDWAIT;

    ret = SNMP_SetOid(pSess, pErr, pPathRowStatusOid);

    if (ret != ERRCODE_NOERROR)
    {
        SNMP_Oid_Free(pPathRowStatusOid);
        SNMP_Oid_Free(pPathIdOid);
        return ret;
    }

    ret = SNMP_SetOid(pSess,pErr,pPathIdOid);

    if (ret != ERRCODE_NOERROR)
    {
        SNMP_Oid_Free(pPathRowStatusOid);
        SNMP_Oid_Free(pPathIdOid);
        return ret;
    }

    RowStatus = SNMP_ROW_ACTIVE;

    ret = SNMP_SetOid(pSess, pErr, pPathRowStatusOid);

    SNMP_Oid_Free(pPathRowStatusOid);
    SNMP_Oid_Free(pPathIdOid);

    if (ret != ERRCODE_NOERROR)
    {
        return ret;
    }

    // 3. PATH is ready, now assign PELs

    pAuxDataList = X_List_New((X_ListMemFree_t)AuxEntityData_Free);

    ret = SNMP_Contains_CP_WDM(pSess, pErr, pPath->Generic.Index, pAuxDataList);

    if (ret != ERRCODE_NOERROR)
    {
        X_List_Free(pAuxDataList);
        return ret;
    }

    pNode2 = X_List_Head(pAuxDataList);

    X_List_ForEach(pPathElementList, pNode1)
    {
        pPEL = (EntityPathElement*)(pNode1->Data);

        while (pNode2 != NULL)
        {
            pAuxData = (AuxEntityData*)pNode2->Data;

            fprintf(stderr, "%s\r\n", pAuxData->Aid);

            /**
             * PELs for WDM paths have the following format:
             * PEL_WDM-<index>-<seq>, where index matches PATH_WDM index
             */
            p = strrchr(pAuxData->Aid, '-');

            if (p != NULL && ++p)
            {
                Seq = atoi(p);

                if (Seq == pPEL->Seq)
                {
                    // now we have the index of the right PEL
                    Entity_Set_Index(pPEL, pAuxData->Index);

                    ret = SNMP_CreatePathElement(pSess, pErr, pPEL);

                    if (ret != ERRCODE_NOERROR)
                    {
                        pNode2 = NULL;
                        break;
                    }

                    pNode2 = pNode2->Next;
                    break;
                }
            }

            pNode2 = pNode2->Next;
        }
    }

    X_List_Free(pAuxDataList);

    return ret;
}

ErrCode
SNMP_DeletePath(netsnmp_session *pSession, SNMP_ErrCodePtr pErr, unsigned long Index)
{
    SNMP_OidPtr pOid;
    unsigned int RowStatus;
    ErrCode ret;

    if (pSession == NULL || pErr == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    if (Index == 0)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    RowStatus = SNMP_ROW_DESTROY;

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvPathWdmRowStatus,
            &RowStatus, sizeof(RowStatus), 1);
    SNMP_Oid_Append(pOid, Index);

    ret = SNMP_SetOid(pSession, pErr, pOid);

    SNMP_Oid_Free(pOid);

    return ret;
}

ErrCode
SNMP_CreatePathElement(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, EntityPathElement *pPEL)
{
    SNMP_OidPtr pRowStatusOid, pOid, pTeTypeOid;
    X_ListPtr pOidList;
    ErrCode ret;
    unsigned int RowStatus;
    X_ListNodePtr pNode;

    if (pSess == NULL || pErr == NULL || pPEL == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pRowStatusOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvPathElementWdmRowStatus,
            &RowStatus, sizeof(RowStatus), 1);

    if (pRowStatusOid == NULL)
    {
        return ERRCODE_INTERNAL_ERROR;
    }
    SNMP_Oid_Append(pRowStatusOid, pPEL->Generic.Index);

    pTeTypeOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvPathElementWdmTrafficEngineeringType,
            &pPEL->Type, sizeof(pPEL->Type), 1);
    if (pTeTypeOid == NULL)
    {
        return ERRCODE_INTERNAL_ERROR;
    }
    SNMP_Oid_Append(pTeTypeOid, pPEL->Generic.Index);

    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

    if (pOidList == NULL)
    {
        return ERRCODE_NOMEM;
    }

    if (pPEL->Follow != 0)
    {
        pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvPathElementWdmTie,
                &pPEL->Follow, sizeof(pPEL->Follow), 1);
        if (pOid != NULL)
            X_List_Append(pOidList, pOid);
    }

    switch (pPEL->Type)
    {
    case TeType_LINKUN:
        pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvPathElementWdmNodeIp,
                 &pPEL->NodeIP, sizeof(pPEL->NodeIP), 1);
        if (pOid != NULL)
            X_List_Append(pOidList, pOid);

        pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvPathElementWdmLogicalInterfaceId,
                &pPEL->LifID, sizeof(pPEL->LifID), 1);
        if (pOid != NULL)
            X_List_Append(pOidList, pOid);
        break;

    case TeType_LINKNR:
        pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvPathElementWdmLifIp,
                &pPEL->LifIP, sizeof(pPEL->LifIP), 1);
        //pOid = SNMP_Oid_New_Raw(deployProvPathElementWdmLifIp_oid,
                //deployProvPathElementWdmLifIp_oid_size, 1, ASN_IPADDRESS, &pPEL->LifIP, sizeof(pPEL->LifIP));
        if (pOid != NULL)
            X_List_Append(pOidList, pOid);
        break;

    case TeType_NODE:
        pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_deployProvPathElementWdmNodeIp,
                &pPEL->NodeIP, sizeof(pPEL->NodeIP), 1);
        if (pOid != NULL)
            X_List_Append(pOidList, pOid);
        break;

    default:
        /// ??? ///
        SNMP_Oid_Free(pRowStatusOid);
        X_List_Free(pOidList);
        return ERRCODE_INVALID_PARAMS;
    }

    // skip it for now....
#if 0
    if (pPEL->Type == TeType_LINKUN || pPEL->Type == TeType_LINKNR)
    {
        if (pPEL->DownChannel != 0)
        {
            pOid = SNMP_Oid_New("FspR7-MIB::deployProvPathElementWdmChannelDown",
                    1, &pPEL->DownChannel, sizeof(pPEL->DownChannel));
            if (pOid != NULL)
                X_List_Append(pOidList, pOid);
        }

        if (pPEL->UpChannel != 0)
        {
            pOid = SNMP_Oid_New("FspR7-MIB::deployProvPathElementWdmChannelUp",
                    1, &pPEL->UpChannel, sizeof(pPEL->UpChannel));
            if (pOid != NULL)
                X_List_Append(pOidList, pOid);
        }
    }
#endif

    // append index
    X_List_ForEach(pOidList, pNode)
    {
        SNMP_Oid_Append((SNMP_OidPtr)pNode->Data, pPEL->Generic.Index);
    }

    RowStatus = SNMP_ROW_CREATEANDWAIT;

    ret = SNMP_SetOid(pSess, pErr, pRowStatusOid);

    if (ret == ERRCODE_NOERROR)
    {

        fprintf(stderr, "PEL create & wait - ok\r\n");

        ret = SNMP_SetOid(pSess, pErr, pTeTypeOid);

        if (ret != ERRCODE_NOERROR)
        {
            fprintf(stderr, "Te type set failed\r\n");
        }

        ret = SNMP_Set(pSess, pErr, pOidList);

        if (ret == ERRCODE_NOERROR)
        {
            fprintf(stderr, "PEL set params - ok\r\n");
            RowStatus = SNMP_ROW_ACTIVE;
            ret = SNMP_SetOid(pSess, pErr, pRowStatusOid);
        }
    }

    X_List_Free(pOidList);
    SNMP_Oid_Free(pRowStatusOid);

    return ret;
}

ErrCode
SNMP_AttachPath(netsnmp_session *pSession, SNMP_ErrCodePtr pErr,
        unsigned long TnlIndex,
        unsigned long Path1Index, unsigned long Path2Index)
{
    SNMP_OidPtr pOid;
    ErrCode Ret;
    char Tmp[64] = { 0 };
    int Len = 0;

    if (pSession == NULL || pErr == NULL || TnlIndex == 0)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    if (Path1Index != 0)
    {
        Len += snprintf(Tmp + Len, sizeof(Tmp) - Len, "%d", (int)Path1Index);

        if (Path2Index != 0)
        {
            Len += snprintf(Tmp + Len, sizeof(Tmp) - Len, "&%d", (int)Path2Index);
        }
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_tunnelWdmConfigProvisionedPaths,
            Tmp, Len, 1);
    SNMP_Oid_Append(pOid, TnlIndex);

    Ret = SNMP_SetOid(pSession, pErr, pOid);

    SNMP_Oid_Free(pOid);

    return Ret;
}

ErrCode
SNMP_GetProvPath(netsnmp_session *pSession, SNMP_ErrCodePtr pErr,
        unsigned long TnlIndex,
        unsigned long *Path1Index, unsigned long *Path2Index)
{
    SNMP_OidPtr pOid;
    ErrCode Ret;
    char Tmp[64] = { 0 }, *p;


    if (pSession == NULL || pErr == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    if (Path1Index == NULL || Path2Index == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_tunnelWdmConfigProvisionedPaths,
            Tmp, sizeof(Tmp), 1);
    SNMP_Oid_Append(pOid, TnlIndex);

    Ret = SNMP_GetOid(pSession, pErr, pOid);

    if (Ret == ERRCODE_NOERROR)
    {
        if (Tmp[0] == '\0')
        {
            *Path1Index = 0;
            *Path2Index = 0;
        }
        else
        {
            p = strchr(Tmp, '&');

            if (p == NULL)
            {
                *Path1Index = atoi(Tmp);
                *Path2Index = 0;
            }
            else
            {
                *p = '\0';
                p++;
                *Path1Index = atoi(Tmp);
                *Path2Index = atoi(p);
            }
        }
    }

    SNMP_Oid_Free(pOid);

    return Ret;
}


ErrCode
SNMP_Destroy_Crs_From(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, unsigned int Idx)
{
    unsigned int        IdxTo = 0;
    unsigned int        RowStatus;
    unsigned int        AdminCap;
    unsigned int        Admin;
    ErrCode             Err;
    SNMP_OidPtr         pOid;

    Err = SNMP_Get_Crs(pSess, pErr, Idx, &IdxTo);

    if (Err != ERRCODE_NOERROR || pErr->Status != STAT_SUCCESS)
    {
        return Err;
    }

    if (pErr->Err == SNMP_ERR_NOSUCHNAME)
    {
        return ERRCODE_NOERROR;
    }

    AdminCap = 0;

    Err = SNMP_GetCrsAdminCap(pSess, pErr, Idx, IdxTo, &AdminCap);

    Admin = GetAdminState(AdminCap, 1);

    Err = SNMP_SetCrsAdmin(pSess, pErr, Idx, IdxTo, Admin);

    RowStatus = SNMP_ROW_DESTROY;

    pOid = SNMP_Oid_New_Raw(crsConnDeployProvRowStatus_oid,
            crsConnDeployProvRowStatus_oid_size,
            2, ASN_INTEGER, &RowStatus, sizeof(RowStatus));
    SNMP_Oid_Append(pOid, (oid)Idx);
    SNMP_Oid_Append(pOid, (oid)IdxTo);

    Err = SNMP_SetOid(pSess, pErr, pOid);

    SNMP_Oid_Free(pOid);

    return Err;
}

/////////

ErrCode
SNMP_GetEntityExternalChannel(netsnmp_session *pSession, SNMP_ErrCodePtr pErr, EntityModule *pEntMod)
{
    ErrCode             err;
    X_ListPtr           pList;
    X_ListNodePtr       pNode;
    SNMP_OidPtr         pOid;

    if (pSession == NULL || pErr == NULL || pEntMod == NULL)
    {
        X_LOGERR(X_LOG_INTF_ID, "SNMP_GetEntityExternalChannel(): invalid arguments");
        return ERRCODE_INVALID_PARAMS;
    }

    pList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

    pEntMod->Type = EXTERNAL_CHANNEL;

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_exChannelDataTransmitChannel,
            &pEntMod->Channel, sizeof(pEntMod->Channel), 1);
    X_List_Append(pList, pOid);

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_entityIndexAid,
            &pEntMod->Aid, sizeof(pEntMod->Aid), 1);
    X_List_Append(pList, pOid);

    X_List_ForEach(pList, pNode)
    {
        pOid = (SNMP_OidPtr)(pNode->Data);
        SNMP_Oid_Append(pOid, Entity_Get_Index(pEntMod));
    }

    err = SNMP_Get(pSession, pErr, pList);

    X_List_Free(pList);

    return err;
}

ErrCode
SNMP_GetEntityExternalChannelList(netsnmp_session *pSession, SNMP_ErrCodePtr pErr, X_ListPtr pList)
{
    ErrCode             err = ERRCODE_NOERROR;
    SNMP_OidPtr         pOid, pRootOid;
    EntityModule        EntMod;

    if (pSession == NULL || pErr == NULL || pList == NULL)
    {
        X_LOGERR(X_LOG_INTF_ID,
                "SNMP_GetEntityExternalChannelList(): invalid arguments");
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_exChannelDataTransmitChannel,
            &EntMod.Channel, sizeof(EntMod), 1);

    pRootOid = SNMP_Oid_Copy(pOid);

    while (err == ERRCODE_NOERROR)
    {
        err = SNMP_GetNextOid(pSession, pErr, pOid, pRootOid);

        if (err != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS)
        {
            break;
        }

        EntityModule_Init(&EntMod);

        Entity_Set_Index(&EntMod, SNMP_Oid_Index(pOid));

        err = SNMP_GetEntityExternalChannel(pSession, pErr, &EntMod);

        if (err != ERRCODE_NOERROR ||
                pErr->Status != STAT_SUCCESS)
        {
            break;
        }

        X_List_Append(pList, EntityModule_Dup(&EntMod));
    }

    SNMP_Oid_Free(pOid);
    SNMP_Oid_Free(pRootOid);

    return (err == ERRCODE_NOERROR || err == ERRCODE_NOERROR_END) ? ERRCODE_NOERROR : err;
}


ErrCode
SNMP_GetPhysicalTermPointAid(netsnmp_session *pSession, SNMP_ErrCodePtr pErr,
        unsigned long Index, char *Buffer, size_t Size)
{
    SNMP_OidPtr         pOid;
    ErrCode             Err;

    if (pSession == NULL || pErr == NULL || Buffer == NULL || Size == 0)
    {
        X_LOGWARN(X_LOG_INTF_ID, "SNMP_GetPhysicalTermPointAid(): invalid agruments");
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_ptpEntityIndexAid, Buffer, Size, 1);
    SNMP_Oid_Append(pOid, Index);

    Err = SNMP_GetOid(pSession, pErr, pOid);

    SNMP_Oid_Free(pOid);

    if (Err == ERRCODE_NOERROR)
    {
        return Err;
    }

    if (Err != ERRCODE_NOOBJECT)
    {
        X_LOGWARN(X_LOG_INTF_ID,"SNMP_GetPhysicalTermPointAid(): unable to get VTP entity AID (Index=%d)",
                (int)Index);
        return Err;
    }

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_vtpEntityIndexAid, Buffer, Size, 1);
    SNMP_Oid_Append(pOid, Index);

    Err = SNMP_GetOid(pSession, pErr, pOid);

    if (Err != ERRCODE_NOERROR ||
            pErr->Status != STAT_SUCCESS)
    {
        X_LOGWARN(X_LOG_INTF_ID,"SNMP_GetPhysicalTermPointAid(): unable to get VTP entity AID (Index=%d)",
                (int)Index);
        return Err;
    }

    SNMP_Oid_Free(pOid);

    return Err;
}

ErrCode
SNMP_GetEntityAdminCap(netsnmp_session *pSession, SNMP_ErrCodePtr pErr,
        unsigned long Index, unsigned int *Caps)
{
    ErrCode     Err;
    SNMP_OidPtr pOid;

    if (pSession == NULL || pErr == NULL || Index == 0 || Caps == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    *Caps = 0;

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_entityStateCapAdmin,
            Caps, sizeof(Caps), 1);
    SNMP_Oid_Append(pOid, Index);

    Err = SNMP_GetOid(pSession, pErr, pOid);
    SNMP_Oid_Free(pOid);

    return Err;
}

ErrCode
SNMP_SetEntityAdmin(netsnmp_session *pSession, SNMP_ErrCodePtr pErr,
        unsigned long Index, unsigned int Admin)
{
    ErrCode     Err;
    SNMP_OidPtr pOid;

    if (pSession == NULL || pErr == NULL || Index == 0)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_entityStateAdmin,
            &Admin, sizeof(Admin), 1);
    SNMP_Oid_Append(pOid, Index);

    Err = SNMP_SetOid(pSession, pErr, pOid);

    SNMP_Oid_Free(pOid);

    return Err;
}

ErrCode
SNMP_GetCrsAdminCap(netsnmp_session *pSession, SNMP_ErrCodePtr pErr, unsigned long From,
        unsigned long To, unsigned int *Caps)
{
    ErrCode     Err;
    SNMP_OidPtr pOid;

    if (pSession == NULL || pErr == NULL || From == 0 || To == 0 || Caps == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    *Caps = 0;

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_crsConnDeployProvCapAdmin,
            Caps, sizeof(*Caps), 2);
    SNMP_Oid_Append(pOid, From);
    SNMP_Oid_Append(pOid, To);

    Err = SNMP_GetOid(pSession, pErr, pOid);
    SNMP_Oid_Free(pOid);

    return Err;
}

ErrCode
SNMP_SetCrsAdmin(netsnmp_session *pSession, SNMP_ErrCodePtr pErr,
        unsigned long From, unsigned long To, unsigned int Admin)
{
    ErrCode     Err;
    SNMP_OidPtr pOid;

    if (pSession == NULL || pErr == NULL || From == 0 || To == 0)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_crsConnDeployProvAdmin,
            &Admin, sizeof(Admin), 2);
    SNMP_Oid_Append(pOid, From);
    SNMP_Oid_Append(pOid, To);

    Err = SNMP_SetOid(pSession, pErr, pOid);
    SNMP_Oid_Free(pOid);
    return Err;
}

ErrCode
SNMP_SetAdminEnable(netsnmp_session *pSession, SNMP_ErrCodePtr pErr,
        unsigned long Index, int Enable)
{
    ErrCode             Err;
    unsigned int        AdminCap;
    unsigned int        Admin;

    if (pSession == NULL || pErr == NULL || Index == 0)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    Err = SNMP_GetEntityAdminCap(pSession, pErr, Index, &AdminCap);

    if (Err != ERRCODE_NOERROR || pErr->Status != STAT_SUCCESS)
    {
        return Err;
    }

    if (pErr->Err != SNMP_ERR_NOERROR)
    {
        return Err;
    }

    Admin = GetAdminState(AdminCap, (Enable) ? 0 : 1);

    Err = SNMP_SetEntityAdmin(pSession, pErr, Index, Admin);

    return Err;
}

ErrCode
SNMP_SetCrsAdminEnable(netsnmp_session *pSession, SNMP_ErrCodePtr pErr,
        unsigned long From, unsigned long To, int Enable)
{
    ErrCode             Err;
    unsigned int        AdminCap;
    unsigned int        Admin;

    if (pSession == NULL || pErr == NULL || From == 0 || To == 0)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    Err = SNMP_GetCrsAdminCap(pSession, pErr, From, To, &AdminCap);

    if (Err != ERRCODE_NOERROR ||
            pErr->Status != STAT_SUCCESS ||
            pErr->Err != SNMP_ERR_NOERROR)
    {
        return Err;
    }

    Admin = GetAdminState(AdminCap, (Enable) ? 0 : 1);

    Err = SNMP_SetCrsAdmin(pSession, pErr, From, To, Admin);

    return Err;
}

ErrCode
SNMP_GetCrsList(netsnmp_session *pSession, SNMP_ErrCodePtr pErr, X_ListPtr pList)
{
    ErrCode             Err = ERRCODE_NOERROR;
    SNMP_OidPtr         pOid, pRoot;
    unsigned int        RowStatus;
    EntityCrs           EntCrs;

    if (pSession == NULL || pErr == NULL || pList == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_crsConnDeployProvRowStatus,
            &RowStatus, sizeof(RowStatus), 2);

    pRoot = SNMP_Oid_Copy(pOid);

    while (Err == ERRCODE_NOERROR)
    {
        Err = SNMP_GetNextOid(pSession, pErr, pOid, pRoot);

        if (Err != ERRCODE_NOERROR || pErr->Status != STAT_SUCCESS)
            break;

        EntityCrs_Init(&EntCrs);

        EntCrs.From = pOid->Name[pOid->Length - 2];
        EntCrs.To = pOid->Name[pOid->Length - 1];

        Err = SNMP_GetEntityCrs(pSession, pErr, &EntCrs);

        if (Err != ERRCODE_NOERROR || pErr->Status != STAT_SUCCESS)
            break;

        X_List_Append(pList, EntityCrs_Dup(&EntCrs));
    }

    SNMP_Oid_Free(pOid);
    SNMP_Oid_Free(pRoot);

    return (Err == ERRCODE_NOERROR || Err == ERRCODE_NOERROR_END) ? ERRCODE_NOERROR : Err;
}

ErrCode
SNMP_GetEntityCrs(netsnmp_session *pSession, SNMP_ErrCodePtr pErr, EntityCrs *pCrs)
{
    SNMP_OidPtr         pOid;
    X_ListPtr           pOidList;
    //X_ListNodePtr       pNode;
    ErrCode             Ret;

    if (pSession == NULL || pErr == NULL || pCrs == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pOidList = X_List_New((X_ListMemFree_t)SNMP_Oid_Free);

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_entityIndexAid,
            pCrs->FromAid, sizeof(pCrs->FromAid), 2);
    SNMP_Oid_Append(pOid, pCrs->From);
    X_List_Append(pOidList, pOid);

    pOid = SNMP_Oid_New_From_Node(&ADVA_MIB_entityIndexAid,
            pCrs->ToAid, sizeof(pCrs->ToAid), 2);
    SNMP_Oid_Append(pOid, pCrs->To);
    X_List_Append(pOidList, pOid);


    Ret = SNMP_Get(pSession, pErr, pOidList);

    X_List_Free(pOidList);

    return Ret;
}

ErrCode
SNMP_GetCnxWdmPwrEqState(netsnmp_session *pSession, SNMP_ErrCodePtr pErr,
        unsigned long Index, unsigned int *State)
{
    ErrCode             Err;
    SNMP_OidPtr         pOid;
    if (pSession == NULL || pErr == NULL || Index == 0 || State == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }
    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_connectionWdmDataEqualizationState,
            State, sizeof(*State), 1);
    SNMP_Oid_Append(pOid, Index);
    Err = SNMP_GetOid(pSession, pErr, pOid);
    SNMP_Oid_Free(pOid);
    return Err;
}

ErrCode
SNMP_InitiatePowerEqualization(netsnmp_session *pSession, SNMP_ErrCodePtr pErr,
        unsigned long Index)
{
    SNMP_OidPtr         pOid;
    ErrCode             Err;
    unsigned int        Cmd;

    if (pSession == NULL || pErr == NULL || Index == 0)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    Cmd = 2; // opr

    pOid = SNMP_Oid_New_From_Node(&FspR7_MIB_interfaceConfigInitiateEqualization,
            &Cmd, sizeof(Cmd), 1);
    SNMP_Oid_Append(pOid, Index);

    Err = SNMP_SetOid(pSession, pErr, pOid);

    SNMP_Oid_Free(pOid);

    return Err;
}


ErrCode
SNMP_GetWch(netsnmp_session *pSession, SNMP_ErrCodePtr pErr, unsigned long Index, X_VectorPtr pVec)
{
    X_ListPtr           pList;
    ErrCode             Err;
    AuxEntityData*      pAuxData;
    void*               Data;

    if (pSession == NULL || pErr == NULL || Index == 0 || pVec == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pList = X_List_New((X_ListMemFree_t)AuxEntityData_Free);

    Err = SNMP_Contains(pSession, pErr, Index, pList);

    while (!X_List_Empty(pList))
    {
        X_List_Remove(pList, X_List_Head(pList), &Data);

        pAuxData = (AuxEntityData*)(Data);

        if (pAuxData->Class != EntityClass_VIRTUALOPTICALCHANNEL)
        {
            AuxEntityData_Free(pAuxData);
            continue;
        }

        X_Vector_Append(pVec, Data);
    }

    X_List_Free(pList);

    return Err;
}
