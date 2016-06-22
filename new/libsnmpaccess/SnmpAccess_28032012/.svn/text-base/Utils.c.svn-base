
#include "Utils.h"

struct variable_list*
snmp_add_null_var_index(struct snmp_pdu *pdu, const oid *name, size_t length, oid index)
{

    struct variable_list *var;

    var = snmp_add_null_var(pdu, name, length);

    if (var != NULL)
    {
        var->name[var->name_length - 1] = index;
    }

    return var;
}

struct variable_list*
snmp_add_null_var_index2(struct snmp_pdu *pdu, const oid *name, size_t length, oid index1, oid index2)
{
    struct variable_list *var;

    var = snmp_add_null_var(pdu, name, length);

    if (var != NULL)
    {
        var->name[var->name_length - 2] = index1;
        var->name[var->name_length - 1] = index2;
    }

    return var;
}

struct variable_list*
snmp_add_null_var_mindex(struct snmp_pdu *pdu, const oid *name, size_t length, oid oids[], size_t s)
{
    struct variable_list *var;
    int i;

    var = snmp_add_null_var(pdu, name, length);

    if (var != NULL)
    {
        for(i = 0; i < s; ++i)
        {
            var->name[var->name_length - s + i] = oids[i];
        }
    }

    return var;
}

int
snmp_var_get_int(const struct variable_list *var, int *i)
{
    if (var && i)
    {
        *i = *(var->val.integer);
        return 0;
    }
    return -1;
}


int set_string(char *b, size_t s, const struct variable_list *v)
{
    if (b != NULL && v != NULL)
    {
        s = (v->val_len <= s - 2) ? (v->val_len) : (s - 2);
        memcpy(b, v->val.string, s);
        b[s] = '\0';
        return (int)s;
    }
    return 0;
}

size_t
oid_copy(oid *to, size_t to_len, const oid *from, size_t from_len)
{
    int i = 0;
    size_t s = (to_len < from_len) ? to_len : from_len;
    if (to == NULL || from == NULL) return (0);
    for(i = 0; i < s; ++i) to[i] = from[i];
    return i;
}

int
oid_append(oid *name, size_t length, oid index)
{
    return 0;
}

/******************************************************************************/

oid
SNMP_Var_Index(const netsnmp_variable_list *pVar)
{
    if (pVar == NULL || pVar->name_length == 0) return (0);
    return (pVar->name[pVar->name_length - 1]);
}


int
SNMP_Var_Set_Integer(const netsnmp_variable_list *pVar, int *pInt)
{
    if (pVar == NULL || pInt == NULL)
    {
        return (-1);
    }
    *pInt = *(pVar->val.integer);
    return (0);
}

int
SNMP_Var_Set_String(const netsnmp_variable_list *pVar, char *b, size_t s)
{
    if (pVar == NULL || b == NULL)
    {
        return (-1);
    }
    s = (pVar->val_len <= s - 2) ? (pVar->val_len) : (s - 2);
    memcpy(b, pVar->val.string, s);
    b[s] = '\0';
    return (int)s;
}

int
SNMP_Var_Set_Bits(const netsnmp_variable_list *pVar, char *b, size_t s)
{
    if (pVar == NULL || b == NULL)
    {
        return (-1);
    }
    memcpy(b, pVar->val.bitstring, s);
    return (int)s;
}

int
SNMP_Var_Init_Oid(oid **o, const char *objid, size_t extra)
{
    //struct tree* tp;
    oid         name[MAX_OID_LEN];
    size_t      length = MAX_OID_LEN;
    int         ret;

    if (o == NULL || objid == NULL)
    {
        return (-1);
    }

    ret = read_objid(objid, name, &length);

    if (ret == 0)
    {
        return (-1);
    }

    /*
    tp = get_tree(name,length,get_tree_head());

    u_char asn = mib_to_asn_type(tp->type);
    */

//    printf("ret = %d length = %d\n", ret, length);

    *o = (oid*)malloc((length + extra) * sizeof(oid));

    if(*o == NULL)
    {
        return (-1);
    }

    memcpy(*o, name, length * sizeof(oid));

    return (length);
}

int
SNMP_Var_Init_Oid_Raw(oid **o, const oid name[], size_t length, size_t extra)
{
    if (o == NULL || name == NULL)
    {
        return (-1);
    }

    *o = (oid*)malloc(sizeof(oid) * (length + extra));

    if (*o == NULL)
    {
        return (-1);
    }

    memcpy(*o, name, length * sizeof(oid));

    return (length);
}

int
SNMP_Var_Get_Type(const oid name[], size_t length)
{
    struct tree *pTree = get_tree_head();
    if (pTree != NULL)
    {
        pTree = get_tree(name, length, pTree);
        if (pTree != NULL)
        {
            return mib_to_asn_type(pTree->type);
        }
    }
    return 0;
}

/******************************************************************************/


/******************************************************************************/

SNMP_OidPtr
SNMP_Oid_New(const char *objid, size_t extra, void *ptr, size_t size)
{
    SNMP_OidPtr pOid = (SNMP_OidPtr)malloc(sizeof(SNMP_Oid));
    int ret;

#ifndef IMPORT_MIB
    oid *_oid;
#endif


    if (pOid == NULL)
    {
        return NULL;
    }

    pOid->Name = NULL;
    pOid->Length = 0;
    pOid->Type = 0;
    pOid->Extra = extra;

#ifdef IMPORT_MIB
    if ((ret = SNMP_Var_Init_Oid(&pOid->Name, objid, extra)) <= 0)
    {
        free(pOid);
        return NULL;
    }

    pOid->Length = ret;
    pOid->Type = SNMP_Var_Get_Type(pOid->Name, pOid->Length);
#else
    ret = MIB_Lookup(objid, &_oid, &pOid->Length, (unsigned char*)&pOid->Type);
    if (ret < 0)
    {
        free (pOid);
        return NULL;
    }
    SNMP_Var_Init_Oid_Raw(&pOid->Name, _oid, pOid->Length, extra);
#endif

    pOid->Void = ptr;
    pOid->Size = size;



    pOid->VarStorage = NULL;
    pOid->VarSize = pOid->VarCapacity = 0;

    return pOid;
}

SNMP_OidPtr
SNMP_Oid_New_Raw(const oid name[], size_t length, size_t extra, int type, void *ptr, size_t size)
{
    SNMP_OidPtr pOid = (SNMP_OidPtr)malloc(sizeof(SNMP_Oid));
    int ret;

    if (pOid == NULL)
    {
        return NULL;
    }

    pOid->Name = NULL;
    pOid->Length = length;
    pOid->Type = type;
    pOid->Extra = extra;

    ret = SNMP_Var_Init_Oid_Raw(&pOid->Name, name, length, extra);

    if (ret <= 0)
    {
        free(pOid);
        return NULL;
    }

    pOid->Void = ptr;
    pOid->Size = size;

    pOid->VarStorage = NULL;
    pOid->VarSize = pOid->VarCapacity = 0;

    return pOid;
}

SNMP_OidPtr
SNMP_Oid_New_Empty(size_t length)
{
    SNMP_OidPtr pOid = (SNMP_OidPtr) malloc(sizeof(SNMP_Oid));
//    int ret;

    if (pOid == NULL)
    {
        return NULL;
    }

    pOid->Name = (oid*)malloc(sizeof(oid) * length);

    if (pOid->Name == NULL)
    {
        free(pOid);
        return NULL;
    }

    pOid->Length = length;
    pOid->Type = 0;
    pOid->Extra = 0;


    pOid->Void = NULL;
    pOid->Size = 0;

    pOid->VarStorage = NULL;
    pOid->VarSize = pOid->VarCapacity = 0;

    return pOid;
}

SNMP_OidPtr
SNMP_Oid_New_From_Node(OID_Node *pNode, void *ptr, size_t size, size_t extra)
{
    assert (pNode != NULL);
    if (pNode == NULL) return NULL;
    return SNMP_Oid_New_Raw(pNode->Oid, pNode->Size, extra, pNode->Type, ptr, size);
}

SNMP_OidPtr
SNMP_Oid_Set_From_Node(SNMP_OidPtr pOid, OID_Node *pNode, void *ptr, size_t size, size_t extra)
{
    assert(pOid != NULL);
    assert(pNode != NULL);

    if (pOid == NULL || pNode == NULL)
        return NULL;

//    pOid->Type = pNode->
    return NULL;

}


SNMP_OidPtr
SNMP_Oid_Copy(const SNMP_OidPtr pOid)
{
    SNMP_OidPtr pCopy;

    if (pOid == NULL)
    {
        return NULL;
    }

    pCopy = SNMP_Oid_New_Raw(pOid->Name, pOid->Length, pOid->Extra,
            pOid->Type, pOid->Void, pOid->Size);

    return pCopy;
}


int
SNMP_Oid_Append(SNMP_OidPtr pOid, oid o)
{
    if (pOid == NULL) return (-1);

    if (pOid->Extra > 0)
    {
        pOid->Name[pOid->Length] = o;
        pOid->Length += 1;
        pOid->Extra -= 1;
        return (0);
    }
    else
    {
        oid *tmp = (oid*)malloc(sizeof(oid) * (pOid->Length + 1));
        if (tmp == NULL) return -1;
        memcpy(tmp, pOid->Name, sizeof(oid) * (pOid->Length));
        pOid->Name = tmp;
        pOid->Name[pOid->Length] = o;
        pOid->Length += 1;
        return (0);
    }

    return (-1);
}

oid
SNMP_Oid_PopIndex(SNMP_OidPtr pOid)
{
    if (pOid == NULL || pOid->Name == NULL || pOid->Length <= 0)
    {
        return 0;
    }
    pOid->Length -= 1;
    pOid->Extra += 1;
    return pOid->Name[pOid->Length];
}

int
SNMP_Oid_UpdateIndex(SNMP_OidPtr pOid, oid o)
{
    if (pOid == NULL || pOid->Name == NULL)
    {
        return (-1);
    }
    pOid->Name[pOid->Length - 1] = o;
    return (0);
}

oid
SNMP_Oid_Index(SNMP_OidPtr pOid)
{
    if (pOid == NULL || pOid->Name == NULL || pOid->Length <= 0)
    {
        return (-1);
    }
    return pOid->Name[pOid->Length - 1];
}

int
SNMP_Oid_Assign(SNMP_OidPtr pOid, const netsnmp_variable_list *pVar)
{
    size_t size;
    oid *pTemp;
    int i;

    if (pOid == NULL || pVar == NULL || pOid->Void == NULL)
    {
        return (-1);
    }

    size = pOid->Length + pOid->Extra;

    // reallocate mem.
    if (pVar->name_length > size)
    {
        pTemp = (oid*)malloc(sizeof(oid) * pVar->name_length);
        if(pTemp == NULL)
        {
            return (-1);
        }
        free(pOid->Name);
        pOid->Name = pTemp;
        size = pVar->name_length;
    }

    for (i = 0; i < pVar->name_length; ++i)
    {
        pOid->Name[i] = pVar->name[i];
    }

    pOid->Length = pVar->name_length;
    pOid->Extra = size - pOid->Length;
    pOid->Type = pVar->type;


    switch (pOid->Type)
    {
    case ASN_INTEGER:
    case ASN_UNSIGNED:
    case ASN_BOOLEAN:
    case ASN_IPADDRESS:
        return SNMP_Var_Set_Integer(pVar,pOid->IntVar);
        // return (0);

    case ASN_OCTET_STR:
        return SNMP_Var_Set_String(pVar,pOid->ByteVar,pOid->Size);
        //return (0);

    case ASN_BIT_STR:
        return SNMP_Var_Set_Bits(pVar, pOid->ByteVar, pOid->Size);

    default:
        fprintf(stderr, "SNMP_Oid_Assign: unknown variable type\n");
        assert(0);
    }
    return (-1);
}

int
SNMP_Oid_AssignTo(const SNMP_OidPtr pOid, netsnmp_variable_list *pVar)
{
    if (pOid == NULL || pVar == NULL)
    {
        return (-1);
    }

    snmp_varlist_add_variable(&pVar, pOid->Name, pOid->Length, pOid->Type,
            (u_char*)pOid->ByteVar, pOid->Size);

    return (0);
}

int
SNMP_Oid_Write(const SNMP_OidPtr pOid, netsnmp_pdu *pdu)
{
    if (pOid == NULL || pdu == NULL)
    {
        return (-1);
    }

    switch (pOid->Type)
    {
    case ASN_INTEGER:
    case ASN_UNSIGNED:
    case ASN_BOOLEAN:
    case ASN_IPADDRESS:
        snmp_pdu_add_variable(pdu,pOid->Name,pOid->Length,
                pOid->Type,(u_char*)pOid->ByteVar,pOid->Size);
        return (0);

    case ASN_OCTET_STR:
        snmp_pdu_add_variable(pdu,pOid->Name,pOid->Length,
                pOid->Type,(u_char*)pOid->ByteVar,strlen(pOid->ByteVar));
        return (0);

    default:
        fprintf(stderr,"SNMP_Oid_Assign: unknown variable type\n");
    }

    return (-1);
}

void
SNMP_Oid_Storage_Alloc(SNMP_OidPtr pOid, size_t s)
{
    void *pTemp;

    if (pOid == NULL)
    {
        return;
    }

    if (pOid->VarStorage == NULL)
    {
        pOid->VarStorage = malloc(s);
        pOid->VarCapacity = s;
    }
    else if (pOid->VarStorage != NULL || s < pOid->VarCapacity)
    {
        pTemp = malloc(s);
        memcpy(pTemp, pOid->VarStorage, pOid->VarCapacity);
        free (pOid->VarStorage);
        pOid->VarStorage = pTemp;
        pOid->VarCapacity = s;
    }
}


void
SNMP_Oid_Dump(SNMP_OidPtr pOid)
{
    if (pOid == NULL) return;
    print_objid(pOid->Name, pOid->Length);
}

void
SNMP_Oid_Free(SNMP_OidPtr pOid)
{
    if (pOid != NULL)
    {
        if (pOid->Name != NULL)
        {
            free(pOid->Name);
        }

        if (pOid->VarStorage != NULL)
        {
            free(pOid->VarStorage);
        }
        free(pOid);
    }
}

/******************************************************************************/

void
SNMP_ErrCode_Init(SNMP_ErrCodePtr pErr)
{
    if (pErr != NULL)
    {
        pErr->Status = STAT_SUCCESS;
        pErr->Err = SNMP_ERR_NOERROR;
    }
}

ErrCode
SNMP_ErrCode_Decode(SNMP_ErrCodePtr pErr)
{
    assert (pErr != NULL);

    if (pErr->Status != STAT_SUCCESS)
    {
        if (pErr->Status == STAT_TIMEOUT)
        {
            return ERRCODE_TIMEOUT;
        }

        if (pErr->Status == STAT_ERROR)
        {
            return ERRCODE_INTERNAL_ERROR;
        }

        // should not happen ?
        return ERRCODE_INTERNAL_ERROR;
    }

    switch (pErr->Err)
    {

    case SNMP_ERR_NOERROR:
        return ERRCODE_NOERROR;

    case SNMP_ERR_NOSUCHNAME:
        return ERRCODE_NOOBJECT;

    default:
        break;
    }

    return ERRCODE_OTHER;
}

/**
 * Translate SNMP status and errcode into a single number.
 */
ErrCode
SNMP_GetErrCode(const SNMP_ErrCodePtr pSnmpErr)
{
    assert (pSnmpErr != NULL);

    if (pSnmpErr == NULL)
        return ERRCODE_INVALID_PARAMS;

    if (pSnmpErr->Status == STAT_SUCCESS)
    {
        if (pSnmpErr->Err == SNMP_ERR_NOERROR)
            return ERRCODE_NOERROR;

        if (pSnmpErr->Err == SNMP_ERR_NOSUCHNAME)
            return ERRCODE_NOOBJECT;

    }
    else if (pSnmpErr->Status == STAT_TIMEOUT)
    {
        return ERRCODE_TIMEOUT;
    }

    return ERRCODE_OTHER;
}


/******************************************************************************/

ErrCode
SNMP_GetNextOid(netsnmp_session *sess, SNMP_ErrCodePtr err, SNMP_OidPtr pOid, SNMP_OidPtr pRoot)
{
    ErrCode ret = ERRCODE_NOERROR;
    netsnmp_pdu *pdu = NULL, *response = NULL;
    netsnmp_variable_list *var;

    assert (sess != NULL && err != NULL && pOid != NULL);

    if (sess == NULL || err == NULL || pOid == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    if (pdu == NULL)
    {
        return ERRCODE_NOMEM;
    }

    snmp_add_null_var(pdu, pOid->Name, pOid->Length);

    err->Status = snmp_sess_synch_response(sess, pdu, &response);

    if (err->Status == STAT_SUCCESS)
    {
        if (response != NULL && response->errstat == SNMP_ERR_NOERROR)
        {
            var = response->variables;

            SNMP_Oid_Assign(pOid, var);

            if (pRoot != NULL)
            {
                if(netsnmp_oid_is_subtree(pRoot->Name, pRoot->Length,
                        var->name, var->name_length))
                {
                    ret = ERRCODE_NOERROR_END;
                }
            }
        }
    }

    if (response != NULL)
    {
        err->Err = response->errstat;
        snmp_free_pdu(response);
    }

    if (ret != ERRCODE_NOERROR_END)
        ret = SNMP_GetErrCode(err);

    return ret;
}

ErrCode
SNMP_GetNext(netsnmp_session *sess, SNMP_ErrCodePtr err, X_ListPtr list)
{
    ErrCode ret = ERRCODE_NOERROR;
    netsnmp_pdu *pdu = NULL, *response = NULL;
    netsnmp_variable_list *var;
    X_ListNodePtr node;
    //int match;

    assert (sess != NULL && err != NULL && list != NULL);

    if (sess == NULL || err == NULL || list == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    if (X_List_Empty(list))
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    if (pdu == NULL)
    {
        return ERRCODE_NOMEM;
    }

    for (node = X_List_Head(list); node; node = X_List_Next(node))
    {
        SNMP_OidPtr pOid = (SNMP_OidPtr)node->Data;

        if (pOid == NULL)
        {
            /// ???
            continue;
        }

        snmp_add_null_var(pdu, pOid->Name, pOid->Length);
    }

    err->Status = snmp_sess_synch_response(sess, pdu, &response);

    if (err->Status == STAT_SUCCESS)
    {
        if (response != NULL && response->errstat == SNMP_ERR_NOERROR)
        {
            node = X_List_Head(list);
            var = response->variables;

            while (node && var)
            {
                SNMP_Oid_Assign((SNMP_OidPtr)node->Data, var);
                node = X_List_Next(node);
                var = var->next_variable;
            }

            /*
            for (var = response->variables; var; var = var->next_variable)
            {
                match = 0;
                for (node = X_List_Head(list); node; node = X_List_Next(node))
                {
                    SNMP_OidPtr pOid = (SNMP_OidPtr)node->Data;

                    if (pOid == NULL)
                    {
                        /// ???
                        continue;
                    }

                    if (!netsnmp_oid_is_subtree(pOid->Name, (var->name_length == pOid->Length) ? pOid->Length - 1 : pOid->Length,
                            var->name, var->name_length))
                    {
                        SNMP_Oid_Assign(pOid, var);
                        match = 1;
                    }
                }

                // if no match then return end
                if (match == 0)
                    ret = ERRCODE_NOERROR_END;
            }
            */
        }
    }

    if (response != NULL)
    {
        err->Err = response->errstat;
        snmp_free_pdu(response);
    }

    return ret;
}

ErrCode
SNMP_GetOid(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, SNMP_OidPtr pOid)
{
    netsnmp_pdu *pdu = NULL, *response = NULL;
    netsnmp_variable_list *var;

    assert (pSess != NULL && pErr != NULL && pOid != NULL);

    if (pSess == NULL || pErr == NULL || pOid == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    SNMP_ErrCode_Init(pErr);

    pdu = snmp_pdu_create(SNMP_MSG_GET);

    if (pdu == NULL)
    {
        return ERRCODE_NOMEM;
    }

    snmp_add_null_var(pdu,pOid->Name,pOid->Length);

    pErr->Status = snmp_sess_synch_response(pSess,pdu,&response);

    if (pErr->Status == STAT_SUCCESS)
    {
        if (response != NULL && response->errstat == SNMP_ERR_NOERROR)
        {
            var = response->variables;
            SNMP_Oid_Assign(pOid, var);
        }
    }

    if (response != NULL)
    {
        pErr->Err = response->errstat;
        snmp_free_pdu(response);
    }

   // fprintf(stderr, "SNMP_GetOid(): status=%d, err=%d\r\n", pErr->Status, pErr->Err);

    return SNMP_GetErrCode(pErr);
}

ErrCode
SNMP_Get(netsnmp_session *sess, SNMP_ErrCodePtr err, X_ListPtr list)
{
    netsnmp_pdu *pdu = NULL, *response = NULL;
    netsnmp_variable_list *var;
    X_ListNodePtr node;

    assert (sess != NULL && err != NULL && list != NULL);

    if (sess == NULL || err == NULL || list == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pdu = snmp_pdu_create(SNMP_MSG_GET);

    if (pdu == NULL)
    {
        return ERRCODE_NOMEM;
    }

    for (node = X_List_Head(list); node; node = X_List_Next(node))
    {
        SNMP_OidPtr pOid = (SNMP_OidPtr) node->Data;

        if (pOid == NULL)
        {
            /// ???
            continue;
        }

        snmp_add_null_var(pdu,pOid->Name,pOid->Length);
    }

    err->Status = snmp_sess_synch_response(sess,pdu,&response);

    if (err->Status == STAT_SUCCESS)
    {
        if (response != NULL && response->errstat == SNMP_ERR_NOERROR)
        {
            for (var = response->variables; var; var = var->next_variable)
            {
                for (node = X_List_Head(list); node; node = X_List_Next(node))
                {
                    SNMP_OidPtr pOid = (SNMP_OidPtr) node->Data;

                    if (pOid == NULL)
                    {
                        /// ???
                        continue;
                    }

                    if (!netsnmp_oid_equals(pOid->Name, pOid->Length,var->name,var->name_length))
                    {
                        SNMP_Oid_Assign(pOid,var);
                    }

                }
            }
        }
    }

    if (response != NULL)
    {
        err->Err = response->errstat;
        snmp_free_pdu(response);
    }

    return SNMP_GetErrCode(err);
}

ErrCode
SNMP_SetOid(netsnmp_session *sess, SNMP_ErrCodePtr err, SNMP_OidPtr pOid)
{
    netsnmp_pdu *pdu = NULL, *response = NULL;
    netsnmp_variable_list *var;

    assert (sess != NULL && err != NULL && pOid != NULL);

    if (sess == NULL || err == NULL || pOid == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pdu = snmp_pdu_create(SNMP_MSG_SET);

    if (pdu == NULL)
    {
        return ERRCODE_NOMEM;
    }

    snmp_pdu_add_variable(pdu, pOid->Name, pOid->Length, pOid->Type,
            (u_char*)pOid->ByteVar, pOid->Size);

    err->Status = snmp_sess_synch_response(sess,pdu,&response);

    if (err->Status == STAT_SUCCESS && response != NULL)
    {
        var = response->variables;
        SNMP_Oid_Assign(pOid,var);
    }

    if (response != NULL)
    {
        err->Err = response->errstat;
        snmp_free_pdu(response);
    }

    return SNMP_GetErrCode(err);
}

ErrCode
SNMP_Set(netsnmp_session *sess, SNMP_ErrCodePtr err, X_ListPtr list)
{
    netsnmp_pdu *pdu = NULL, *response = NULL;
    netsnmp_variable_list *var;
    X_ListNodePtr node;

    assert (sess != NULL && err != NULL && list != NULL);

    if (sess == NULL || err == NULL || list == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pdu = snmp_pdu_create(SNMP_MSG_SET);

    if (pdu == NULL)
    {
        return ERRCODE_NOMEM;
    }

    for (node = X_List_Head(list); node; node = X_List_Next(node))
    {
        SNMP_OidPtr pOid = (SNMP_OidPtr) node->Data;
        if (pOid == NULL)
        {
            /// ???
            continue;
        }

        SNMP_Oid_Write(pOid, pdu);

        //snmp_pdu_add_variable(pdu, pOid->Name, pOid->Length, pOid->Type,
          //      (u_char*)pOid->ByteVar, pOid->Size);
    }

    err->Status = snmp_sess_synch_response(sess,pdu,&response);

    if (err->Status == STAT_SUCCESS)
    {
        if (response != NULL && response->errstat == SNMP_ERR_NOERROR)
        {
            for (var = response->variables; var; var = var->next_variable)
            {
                for (node = X_List_Head(list); node; node = X_List_Next(node))
                {
                    SNMP_OidPtr pOid = (SNMP_OidPtr) node->Data;
                    if (pOid == NULL)
                    {
                        /// ???
                        continue;
                    }

                    if (!netsnmp_oid_equals(pOid->Name, pOid->Length,var->name,var->name_length))
                    {
                        SNMP_Oid_Assign(pOid,var);
                    }
                }
            }
        }
    }

    if (response != NULL)
    {
        err->Err = response->errstat;
        snmp_free_pdu(response);
    }

    return SNMP_GetErrCode(err);
}

ErrCode
SNMP_Walk(netsnmp_session *sess, SNMP_ErrCodePtr err, X_ListPtr list, X_ListPtr rootList)
{
    netsnmp_pdu *pdu = NULL, *response = NULL;
    netsnmp_variable_list *var;
    X_ListNodePtr node, rootNode;
    SNMP_OidPtr pOid, pRoot;
    ErrCode ret = ERRCODE_NOERROR;

    assert (sess != NULL && err != NULL && list != NULL && rootList != NULL);

    if (sess == NULL || err == NULL || list == NULL || rootList == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    if (X_List_Empty(list))
    {
        return ERRCODE_INVALID_PARAMS;
    }

    if (X_List_Size(rootList) != X_List_Size(list))
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    if (pdu == NULL)
    {
        return ERRCODE_NOMEM;
    }

    for (node = X_List_Head(list); node; node = X_List_Next(node))
    {
        pOid = (SNMP_OidPtr) node->Data;

        if (pOid == NULL)
        {
            /// ???
            continue;
        }

        snmp_add_null_var(pdu,pOid->Name,pOid->Length);
    }

    err->Status = snmp_sess_synch_response(sess,pdu,&response);

    if (err->Status == STAT_SUCCESS)
    {
        if (response != NULL && response->errstat == SNMP_ERR_NOERROR)
        {
            node = X_List_Head(list);
            rootNode = X_List_Head(rootList);
            var = response->variables;

            while (node && rootNode && var)
            {
                pOid = (SNMP_OidPtr)(node->Data);
                pRoot = (SNMP_OidPtr)(rootNode->Data);

               // SNMP_Oid_Dump(pRoot);

                if (netsnmp_oid_is_subtree(pRoot->Name, pRoot->Length,
                        var->name, var->name_length))
                {
                    ret = ERRCODE_NOERROR_END;
                }

                SNMP_Oid_Assign(pOid, var);

                node = X_List_Next(node);
                rootNode = X_List_Next(rootNode);
                var = var->next_variable;
            }
            /*
            for (var = response->variables; var; var = var->next_variable)
            {
                for (node = X_List_Head(list); node; node = X_List_Next(node))
                {
                    SNMP_OidPtr pOid = (SNMP_OidPtr) node->Data;

                    if (pOid == NULL)
                    {
                        /// ???
                        continue;
                    }

                    if (!netsnmp_oid_equals(pOid->Name,pOid->Length,var->name,var->name_length))
                    {
                        SNMP_Oid_Assign(pOid,var);
                    }

                }
            }
            */
        }
    }

    if (response != NULL)
    {
        err->Err = response->errstat;
        snmp_free_pdu(response);
    }

    return (ret);
}

#if 0
ErrCode
SNMP_SmartWalk(netsnmp_session *sess, SNMP_ErrCodePtr err, SNMP_OidPtr pRootOid, X_ListPtr list)
{
    netsnmp_pdu *pdu = NULL, *response = NULL;
    netsnmp_variable_list *var;
    X_ListNodePtr pNode;
    SNMP_OidPtr pOid;
    ErrCode ret = ERRCODE_NOERROR;

    if (sess == NULL || err == NULL || pRootOid == NULL || list == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    if (X_List_Empty(list))
    {
        return ERRCODE_INVALID_PARAMS;
    }

    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);

    if (pdu == NULL)
    {
        return ERRCODE_NOMEM;
    }

    for (pNode = X_List_Head(list); pNode; pNode = X_List_Next(pNode))
    {
        pOid = (SNMP_OidPtr) pNode->Data;

        if (pOid == NULL)
        {
            /// ???
            continue;
        }
        snmp_add_null_var(pdu,pOid->Name,pOid->Length);
    }

    err->Status = snmp_sess_synch_response(sess,pdu,&response);

    if (err->Status == STAT_SUCCESS)
    {
        if (response != NULL && response->errstat == SNMP_ERR_NOERROR)
        {
            pNode = X_List_Head(list);
            var = response->variables;

            /***/
            if (var)
            {
                if (netsnmp_oid_is_subtree(pRootOid->Name,pRootOid->Length,
                        var->name,var->name_length))
                {
                    ret = ERRCODE_NOERROR_END;
                }
            }

            while (pNode && var)
            {
                pOid = (SNMP_OidPtr) (pNode->Data);

                SNMP_Oid_Assign(pOid, var);

                pNode = X_List_Next(pNode);
                var = var->next_variable;
            }
        }
    }

    if (response != NULL)
    {
        err->Err = response->errstat;
        snmp_free_pdu(response);
    }

    return (ret);
}


ErrCode
SNMP_WalkOids(netsnmp_session *pSess, SNMP_ErrCodePtr pErr, SNMP_OidPtr pOidRoot,
        SNMP_OidPtr pOid, X_ListPtr pList)
{
    X_ListNodePtr pNode;
    ErrCode ret = ERRCODE_NOERROR;
    //netsnmp_pdu *pdu, *response;

    if (pSess == NULL || pErr == NULL || pOidRoot == NULL || pOid == NULL || pList == NULL)
    {
        return ERRCODE_INVALID_PARAMS;
    }

    ret = SNMP_GetNextOid(pSess, pErr, pOid, pOidRoot);

    if (ret != ERRCODE_NOERROR && ret != ERRCODE_NOERROR_END)
    {
        return ret;
    }
    else if (ret == ERRCODE_NOERROR && pErr->Status != STAT_SUCCESS)
    {
        return ret;
    }

    X_List_ForEach(pList, pNode)
    {
        SNMP_Oid_UpdateIndex((SNMP_OidPtr)pNode->Data, SNMP_Oid_Index(pOid));
    }

    ret = SNMP_Get(pSess, pErr, pList);

    return ret;
}
#endif
