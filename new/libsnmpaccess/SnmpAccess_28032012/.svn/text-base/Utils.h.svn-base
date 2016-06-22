
#ifndef __UTILS_H__
#define __UTILS_H__

#include <SnmpAccess/Common.h>
#include <SnmpAccess/List.h>
#include <SnmpAccess/MIB.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>


struct variable_list*
snmp_add_null_var_index(struct snmp_pdu *, const oid *, size_t, oid);

struct variable_list*
snmp_add_null_var_index2(struct snmp_pdu *, const oid *, size_t, oid, oid);

struct variable_list*
snmp_add_null_var_mindex(struct snmp_pdu *, const oid *, size_t, oid[], size_t);

int
snmp_var_get_int(const struct variable_list*, int*);

int
set_string(char *, size_t, const struct variable_list*);

int
snmp_add_int(struct snmp_pdu*, const oid*, size_t, oid, int);

size_t
oid_copy(oid *to, size_t to_len, const oid *from, size_t from_len);

int
oid_append(oid *, size_t, oid);

/******************************************************************************/

/**
 * Return the last oid (if empty then 0 is returned).
 */
oid
SNMP_Var_Index(const netsnmp_variable_list*);

int
SNMP_Var_Set_Integer(const netsnmp_variable_list*, int*);

int
SNMP_Var_Set_String(const netsnmp_variable_list*, char*, size_t);

int
SNMP_Var_Set_Bits(const netsnmp_variable_list *pVar, char *b, size_t s);

int
SNMP_Var_Init_Oid(oid**, const char*, size_t);

int
SNMP_Var_Init_Oid_Raw(oid**, const oid*, size_t, size_t);

int
SNMP_Var_Get_Type(const oid[], size_t);

/******************************************************************************/

/**
 * Auxillary structure for keeping OIDs.
 */
typedef struct _SNMP_Oid
{
    /** The OID **/
    oid         *Name;
    /** Real length of the OID**/
    size_t      Length;
    /** Extra space allocated (the real size of Name = Length + Extra) **/
    size_t      Extra;

    /** The variable type **/
    int         Type;

    union
    {
        int     *IntVar;
        char    *ByteVar;

        void    *Void;
    };
    size_t      Size;


    void        *VarStorage;
    size_t      VarSize;
    size_t      VarCapacity;

} SNMP_Oid;

typedef struct _SNMP_Oid* SNMP_OidPtr;

/**
 * Creates new SNMP_Oid structure based on MIB data.
 */
SNMP_OidPtr
SNMP_Oid_New(const char *objid, size_t extra, void *ptr, size_t size);

SNMP_OidPtr
SNMP_Oid_New_Raw(const oid name[], size_t length, size_t extra, int type, void *ptr, size_t size);

SNMP_OidPtr
SNMP_Oid_New_Empty(size_t);

SNMP_OidPtr
SNMP_Oid_New_From_Node(OID_Node*, void *ptr, size_t, size_t extra);

SNMP_OidPtr
SNMP_Oid_Set_From_Node(SNMP_OidPtr, OID_Node*, void *ptr, size_t, size_t extra);

SNMP_OidPtr
SNMP_Oid_Copy(const SNMP_OidPtr);

int
SNMP_Oid_Append(SNMP_OidPtr, oid);

oid
SNMP_Oid_PopIndex(SNMP_OidPtr);

int
SNMP_Oid_UpdateIndex(SNMP_OidPtr, oid);

oid
SNMP_Oid_Index(SNMP_OidPtr);

int
SNMP_Oid_Assign(SNMP_OidPtr, const netsnmp_variable_list*);

int
SNMP_Oid_AssignTo(const SNMP_OidPtr,netsnmp_variable_list*);

int
SNMP_Oid_Write(const SNMP_OidPtr, netsnmp_pdu*);

void
SNMP_Oid_Storage_Alloc(SNMP_OidPtr, size_t);

void
SNMP_Oid_Dump(SNMP_OidPtr);

void
SNMP_Oid_Free(SNMP_OidPtr);

/******************************************************************************/

typedef struct _SNMP_ErrCode
{
    /** SNMP status: STAT_SUCCESS, STAT_TIMEOUT, STAT_ERROR. **/
    int Status;
    /** SNMP error codes **/
    int Err;
} SNMP_ErrCode;

typedef SNMP_ErrCode* SNMP_ErrCodePtr;

/**
 * Set default values for SNMP_ErrCode:
 *      Status = STAT_SUCCESS
 *      Err = SNMP_ERR_NOERROR
 */
void
SNMP_ErrCode_Init(SNMP_ErrCodePtr);

/**
 * Translate SNMP status + SNMP error code into a single ErrCode type.
 */
ErrCode
SNMP_ErrCode_Decode(SNMP_ErrCodePtr);


/**
 * Translate SNMP status and errcode into a single number.
 */
ErrCode
SNMP_GetErrCode(const SNMP_ErrCodePtr);

/******************************************************************************/

/*
 * Issue a GETNEXT query.
 * @param Session (in) net-snmp session pointer
 * @param Err (out)
 * @param Oid (inout) The oid.
 * @param RootOid (in) Optional parameter (can be NULL), if resulting Oid
 *      is not in the subtree of RootOid NOERROR_END should be returned.
 *
 * @return ERRCODE_{NOERROR, NOERROR_END} on success
 */
ErrCode
SNMP_GetNextOid(netsnmp_session *Session, SNMP_ErrCodePtr Err,
        SNMP_OidPtr Oid, SNMP_OidPtr RootOid);

/*
 * Issue a GETNEXT query for multiple oids.
 */
ErrCode
SNMP_GetNext(netsnmp_session*, SNMP_ErrCodePtr, X_ListPtr);

/*
 * Issue a GET query for a single oid.
 * @return ERRCODE_NOERROR on success.
 */
ErrCode
SNMP_GetOid(netsnmp_session*, SNMP_ErrCodePtr, SNMP_OidPtr);


ErrCode
SNMP_Get(netsnmp_session*, SNMP_ErrCodePtr, X_ListPtr);

ErrCode
SNMP_SetOid(netsnmp_session*, SNMP_ErrCodePtr, SNMP_OidPtr);

ErrCode
SNMP_Set(netsnmp_session*, SNMP_ErrCodePtr, X_ListPtr);

ErrCode
SNMP_Walk(netsnmp_session*, SNMP_ErrCodePtr, X_ListPtr, X_ListPtr);

#if 0
ErrCode
SNMP_SmartWalk(netsnmp_session*, SNMP_ErrCodePtr, SNMP_OidPtr, X_ListPtr);


ErrCode
SNMP_WalkOids(netsnmp_session *pSess, SNMP_ErrCodePtr pErr,
        SNMP_OidPtr pRootOid, SNMP_OidPtr pOid, X_ListPtr pOidList);
#endif

#endif
