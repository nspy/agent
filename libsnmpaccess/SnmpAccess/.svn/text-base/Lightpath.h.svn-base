
#ifndef __LIGHTPATH_H__
#define __LIGHTPATH_H__

#include <SnmpAccess/Interface.h>
#include <SnmpAccess/Worker.h>
#include <arpa/inet.h>
//#include <openflow/openflow.h>

/**
 * GMPLS Control Library.
 *
 * @TODO: fix memory leaks, fix infinite polling, check sec. states(?)
 * add error codes
 *
 *
 * Usage:
 *
 * GMPLS_Controller c;
 *
 * Part A. Intialization.
 *
 * 1. Initliaze internals.
 * GMPLS_Controller_Init(&c);
 *
 * 2. Register callback
 * GMPLS_Controller_AddCalback(&c, my_callback);
 *
 * 3. Start processing.
 * GMPLS_Controller_Start(&c);
 *
 *
 * 4. Put a request.
 *
 * Part B. Requests
 *
 * 1. Create a request
 * my_req = GMPLS_Request_Create(...);
 *
 * 2. Send the request
 * GMPLS_Controller_Put(&c, my_req);
 *
 * 3. Wait for callback function to return a result.
 */

/******************************************************************************/

struct _GMPLS_Controller;

typedef struct _GMPLS_Controller* GMPLS_ControllerPtr;

/******************************************************************************/

// polling timeout in ms
#define GMPLS_REQ_POLL_TIMEOUT (100)

/** total time for a request processing **/
#define GMPLS_REQ_MAX_TIME (180000)

#define GMPLS_LIB_WORKERS (4)

/**
 * Result returned to the client.
 */
typedef enum
{
    GMPLS_REQ_RESULT_SUCCESS,

    GMPLS_REQ_RESULT_NO_CONNECTION,

    GMPLS_REQ_RESULT_TIMEOUT,

    GMPLS_REQ_RESULT_NOT_FOUND,

    GMPLS_REQ_RESULT_OTHER_ERROR,

} GMPLS_Request_Result_t;

/** Type of the request **/
typedef enum
{
    GMPLS_REQ_TYPE_SETUP = 0,           ///< Lightpath setup

    GMPLS_REQ_TYPE_TEAR_DOWN,           ///< Lightpath tear down

    GMPLS_REQ_TYPE_PATH_COMPUTATION,   ///< path computation (no tunnel setup)
} GMPLS_Request_Type_t;

/** State of the request **/
typedef enum
{
    GMPLS_REQ_STATE_NOT_INITIALIZED = 0,        ///< processing not started

    GMPLS_REQ_STATE_IN_PROGRESS,                ///< procssing in progress

    GMPLS_REQ_STATE_EQ_IN_PROGRESS,             ///< equalization (for tunnel setup)

    GMPLS_REQ_STATE_FINALIZED,                  ///< done
} GMPLS_Request_State_t;

/** Request for a lightpath setup **/
typedef struct _GMPLS_Request
{
    /** Unique identifier for the request **/
    int                 Id;

    /** Pointer to the controller instance **/
    GMPLS_ControllerPtr Controller;

    SNMP_ErrCode        SNMPErr;

    /** SNMP session handle **/
    netsnmp_session     *Session;

    X_Timer             Timer;

    /** Timer for measuring setup (without eq.) and tear down time **/
    X_Timer             ProcessingTimer;

    /** For tunnel setup - timer till service is equalized **/
    X_Timer             EqualizationTimer;

    /** Timer to measure total processing time **/
    X_Timer             GuardTimer;

    unsigned int        ProcessingTime;

    unsigned int        EqualizationTime;

    unsigned int        PwrEqState;

    GMPLS_Request_State_t State;

    GMPLS_Request_Type_t Type;

    GMPLS_Request_Result_t Result;

    /** SNMP agent **/
    char                *Agent;
    /** SNMP community **/
    char                *Community;

    /** Ingress node IP **/
    char                FromIP[32];
    /** Egress node IP **/
    char                ToIP[32];
    /** Originating endpoint **/
    long                FromAid;
    /** Terminating  endpoint **/
    long                ToAid;
    /** Tunnel index **/
    long                Index;
    /** Id of the tunnel **/
    char                TunnelId[32];
    /** Path index **/
    long                PathIndex;

    /** Index of a CNX_WDM **/
    long                CnxIndex;

    /** Path description **/
    X_ListPtr           Path;

    /** Private user data **/
    void*               UserData;

    /** Aux. data **/
    unsigned long long  Dpid[2];
    unsigned short      Port[2];

} GMPLS_Request;

typedef GMPLS_Request* GMPLS_RequestPtr;

const char*
GMPLS_Request_Result_Description(GMPLS_Request_Result_t);

GMPLS_RequestPtr
GMPLS_Request_New(void);

/**
 * Create new GMPLS request
 * @param type          setup/teardown
 * @param ToIP          dst. IP
 * @param FromAid       src. AID
 * @param ToAid         dst. AID
 */
GMPLS_RequestPtr
GMPLS_Request_Create(GMPLS_Request_Type_t type,
        const char *ToIP,
        long FromAid, long ToAid);

GMPLS_RequestPtr
GMPLS_Request_Create2(GMPLS_Request_Type_t type,
        const char *agent,
        const char *community,
        const char *FromIP,
        const char *ToIP,
        long FromAid,
        long ToAid);

void
GMPLS_Request_Set_Aux_Data(GMPLS_RequestPtr,
        unsigned long long dpid1, unsigned short port1,
        unsigned long long dpid2, unsigned short port2);


int
GMPLS_Request_Clear_Path(GMPLS_RequestPtr);

int
GMPLS_Request_Add_Path_Object(GMPLS_RequestPtr, const CrossConnect*);

/**
 * Set SNMP agent info.
 */
void
GMPLS_Request_Set_SNMP(GMPLS_RequestPtr, const char *agent, const char *community);

/**
 * Set user data.
 */
void
GMPLS_Request_Set_User(GMPLS_RequestPtr, void*);

/**
 * Retrieve user data.
 */
void*
GMPLS_Request_Get_User(GMPLS_RequestPtr);

/**
 * Release request.
 */
void
GMPLS_Request_Free(GMPLS_RequestPtr);

/** Initialize request (internal) **/
int
_GMPLS_Request_Init(GMPLS_RequestPtr);

int
_GMPLS_Request_Create_Session(GMPLS_RequestPtr);

int
_GMPLS_Request_Process(GMPLS_RequestPtr);

void
_GMPLS_Request_Schedule_Task(GMPLS_RequestPtr);

void
_GMPLS_Request_Tick(GMPLS_RequestPtr);

/** Internal processing of the SETUP request **/
void
_GMPLS_Request_Process_Setup(GMPLS_RequestPtr);

/** Internal processing of the TEARDOWN request **/
void
_GMPLS_Request_Process_Teardown(GMPLS_RequestPtr);

void
_GMPLS_Request_Stop_Processing_Timer(GMPLS_RequestPtr);

/** Fetch signalled path from the NE **/
int
_GMPLS_Fetch_Path(GMPLS_RequestPtr);

/******************************************************************************/

#if 0
/**
 * API for the OpenFlow Controller
 */
GMPLS_RequestPtr
GMPLS_Request_Create_OpenFlow(GMPLS_Request_Type_t type,
        struct ofp_phy_port *in,
        uint64_t in_dpid,
        struct ofp_phy_port *out,
        uint64_t out_dpid);
#endif


/******************************************************************************/

/** Request finalized callback **/
typedef void (*GMPLS_Request_Finalize_t)(int id, GMPLS_RequestPtr, GMPLS_Request_Result_t);

/** **/
typedef struct _GMPLS_Controller
{
    X_Ticker            Ticker;
    X_QueuePtr          Queue;
    X_WorkerPtr         Timer;
    X_ListPtr           Workers;
    X_ListPtr           Requests;
    X_MutexPtr          Mutex;
    X_ListPtr           Callbacks;
} GMPLS_Controller;

/** Initialize structure **/
int
GMPLS_Controller_Init(GMPLS_ControllerPtr);

void
GMPLS_Controller_Uninit(GMPLS_ControllerPtr);

/** Start processing (threads) **/
int
GMPLS_Controller_Start(GMPLS_ControllerPtr);

void
GMPLS_Controller_Stop(GMPLS_ControllerPtr);

void
GMPLS_Controller_Add_Callback(GMPLS_ControllerPtr, GMPLS_Request_Finalize_t);

/**
 * Ticker callback.
 */
void
GMPLS_Controller_Tick(GMPLS_ControllerPtr);

/**
 * Put a GMPLS request.
 * @return -1 on error, request id (>0) on success.
 */
int
GMPLS_Controller_Put(GMPLS_ControllerPtr, GMPLS_RequestPtr);

/** Task dispatch callback **/
void
GMPLS_Controller_Dispatch(X_TaskPtr);


#endif
