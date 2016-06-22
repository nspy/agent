
#ifndef __CHANNEL_H__
#define __CHANNEL_H__

/**
 * Implementation of the SNMP_Channel object.
 */

#include <SnmpAccess/Common.h>
#include <SnmpAccess/Inventory.h>
#include <SnmpAccess/Interface.h>
#include <SnmpAccess/Queue.h>
#include <SnmpAccess/Timer.h>
#include <SnmpAccess/Types.h>
#include <SnmpAccess/Log.h>
#include <SnmpAccess/Worker.h>
#include <arpa/inet.h>

struct _SNMP_Channel;

typedef struct _SNMP_Channel* SNMP_ChannelPtr;

/******************************************************************************/

/** SNMP channel state **/
enum
{
    SNMP_CHANNEL_STATE_NOT_INIT,        ///< Not initialized
    SNMP_CHANNEL_STATE_DISCONNECTED,    ///< Disconnected
    SNMP_CHANNEL_STATE_SYNCH,           ///< Synchronizing with NE
    SNMP_CHANNEL_STATE_READY,           ///< Ready, synchronized with NE

    SNMP_CHANNEL_STATE_MAX,
};

/** SNMP channel event type **/
enum
{
    SNMP_CHANNEL_EV_INIT_REQ,           ///< Initliazation request
    SNMP_CHANNEL_EV_SYNCH_REQ,          ///< Synchronization request

    SNMP_CHANNEL_EV_RELOAD_CP,

    SNMP_CHANNEL_EV_LOAD_INVENTORY,

    SNMP_CHANNEL_EV_TIMER_POLL,
    SNMP_CHANNEL_EV_TIMER_RECONNECT,
    SNMP_CHANNEL_EV_TIMER_TRAP,
    SNMP_CHANNEL_EV_TIMER_CP_RELOAD,
    SNMP_CHANNEL_EV_TIMER_RESYNC,

    SNMP_CHANNEL_EV_TIMER_CRS_RELOAD,

    SNMP_CHANNEL_EV_MAX,
};

/** SNMP channel timers **/
enum
{
    SNMP_CHANNEL_TIMER_POLL,
    SNMP_CHANNEL_TIMER_RECONNECT,
    SNMP_CHANNEL_TIMER_TRAP,
    SNMP_CHANNEL_TIMER_CP_RELOAD,
    SNMP_CHANNEL_TIMER_RESYNC,

    SNMP_CHANNEL_TIMER_CRS_RELOAD,

    SNMP_CHANNEL_TIMER_MAX
};

/** SNMP channel callback type **/
enum
{
    SNMP_CHANNEL_CB_READY = 0,          ///< synchronized
    SNMP_CHANNEL_CB_DISCONNECTED,       ///< NE disconnected

    SNMP_CHANNEL_CB_SHELF_CREATED,      ///< SHELF created
    SNMP_CHANNEL_CB_SHELF_DESTROYED,    ///< SHELF destroyed

    SNMP_CHANNEL_CB_MOD_CREATED,        ///< MOD created
    SNMP_CHANNEL_CB_MOD_UPDATED,        ///< MOD entity updated
    SNMP_CHANNEL_CB_MOD_DESTROYED,      ///< MOD entity destroyed

    SNMP_CHANNEL_CB_OL_CREATED,         ///< OL created
    SNMP_CHANNEL_CB_OL_DESTROYED,

    SNMP_CHANNEL_CB_CONN_CREATED,
    SNMP_CHANNEL_CB_CONN_DESTROYED,

    SNMP_CHANNEL_CB_TNL_WDM_CREATED,
    SNMP_CHANNEL_CB_TNL_WDM_DESTROYED,
    SNMP_CHANNEL_CB_TNL_WDM_ADMIN_CHANGED,

    SNMP_CHANNEL_CB_CNX_WDM_CREATED,
    SNMP_CHANNEL_CB_CNX_WDM_DESTROYED,


    SNMP_CHANNEL_CB_CRS_CREATED,
    SNMP_CHANNEL_CB_CRS_DESTROYED,

    SNMP_CHANNEL_CB_MAX,
};

/** SNMP callback function type **/
typedef void (*SNMP_Channel_Callback_t)(SNMP_ChannelPtr, int, void*);

/******************************************************************************/

/** (Hardware) Inventory data **/
typedef struct
{
    EntityNE    NE;
    X_ListPtr   ShelfList;
    X_ListPtr   ModuleList;
    X_ListPtr   ExtObjList;
    X_ListPtr   PlugList;
    X_ListPtr   ConnectionList;
    X_ListPtr   OLList;

    X_ListPtr   CrsList;
} SNMP_Channel_Inventory;

/** Initialize structure (create lists) **/
void
SNMP_Channel_Inventory_Init(SNMP_Channel_Inventory*);

/** Clear structure (clear lists) **/
void
SNMP_Channel_Inventory_Clear(SNMP_Channel_Inventory*);

/** Clear structure (destroy lists) **/
void
SNMP_Channel_Inventory_Deinit(SNMP_Channel_Inventory*);

void
SNMP_Channel_Inventory_Dump(SNMP_Channel_Inventory*,FILE*);

/******************************************************************************/

typedef struct
{
    X_ListPtr TunnelWDMList;
    X_ListPtr CnxWDMList;
    X_ListPtr TEUnnumLinks;
    X_ListPtr TENumLinks;
} SNMP_Channel_InventoryCP;

void
SNMP_Channel_InventoryCP_Init(SNMP_Channel_InventoryCP*);

void
SNMP_Channel_InventoryCP_Clear(SNMP_Channel_InventoryCP*);

void
SNMP_Channel_InventoryCP_Deinit(SNMP_Channel_InventoryCP*);

void
SNMP_Channel_InventoryCP_Dump(SNMP_Channel_InventoryCP*);

/******************************************************************************/

/** SNMP channel event **/
typedef struct
{
    int     Type;

    int     TimerId;

    // specific data, interpret according to event type

    SNMP_Channel_Inventory      Inventory;

    SNMP_Channel_InventoryCP    InventoryCP;

} SNMP_Channel_Event;

SNMP_Channel_Event*
SNMP_Channel_Event_New(int Type);

void
SNMP_Channel_Event_Free(SNMP_Channel_Event*);

size_t
SNMP_Channel_Event_Dump(SNMP_Channel_Event*, char*, size_t);

const char*
SNMP_Channel_Event_GetName(int);

/******************************************************************************/

/** SNMP_Channel configuration parameters **/
typedef struct
{
    /** Address of the SNMP agent **/
    char        Host[64];

    /** SNMP community string **/
    char        Community[64];

    /** SNMP version: SNMP_VERSION_1 is currently supported, v2c to be added... **/
    int         Version;

    /** TODO: add more paramters (e.g. timeouts) **/

    /** Polling timeout (polls for sys uptime) in seconds (set 0 to disable) **/
    long        PollTimeout;

    /** CP inventory data reload in seconds (set 0 to disable) **/
    long        CPReloadTimeout;

    /** Timeout to reconnect with NE in seconds **/
    long        ResyncTimeout;
}
SNMP_Channel_Conf;

/** Initializes structure with default values **/
void
SNMP_Channel_Conf_Init(SNMP_Channel_Conf*);

void
SNMP_Channel_Conf_Set_Host(SNMP_Channel_Conf*, const char*);

void
SNMP_Channel_Conf_Set_Community(SNMP_Channel_Conf*, const char*);

SNMP_Channel_Conf*
SNMP_Channel_Conf_Copy(SNMP_Channel_Conf*,const SNMP_Channel_Conf*);

/******************************************************************************/

#define SNMP_CHANNEL(p) (SNMP_ChannelPtr)(p)

/** SNMP channel structure **/
typedef struct _SNMP_Channel
{
    /** Configuration structure **/
    SNMP_Channel_Conf       Config;

    /** Handle to the net-snmp session **/
    struct snmp_session*    Session;

    X_MutexPtr              Mutex;

    X_MutexPtr              IntMutex;

    /** Current state of the SNMP channel **/
    int                     State;

    /** User defined context **/
    void*                   Context;

    /** Internal timers **/
    X_Timer                 Timers[SNMP_CHANNEL_TIMER_MAX];

    /** Callbacks **/
    SNMP_Channel_Callback_t Callbacks[SNMP_CHANNEL_CB_MAX];

    /** Internal event queue **/
    X_QueuePtr                  EvQueue;

    /** Cached traps **/
    X_ListPtr                   TrapEvList;

    /////

    SNMP_Channel_Inventory      Inventory;

    SNMP_Channel_InventoryCP    InventoryCP;

    /////////////////////////////////////

    int                     Run;

    X_ThreadPtr             Thread;


    ///////// API for Worker/Task //////////

    X_QueuePtr  TaskQueue;

    X_Ticker    Ticker;


} SNMP_Channel;

/** public API **/


/** Instantiate a new SNMP_Channel object
 * @param Config    Configuration structure
 * @return Valid pointer to a SNMP_Channel instance
 */
SNMP_ChannelPtr
SNMP_Channel_New(SNMP_Channel_Conf *Config);

/** Destroy SNMP_Channel object **/
void
SNMP_Channel_Free(SNMP_ChannelPtr);

/** Start processing **/
ErrCode
SNMP_Channel_Init(SNMP_ChannelPtr);

/** Set user context
 * @param Context User specific data
 */
void
SNMP_Channel_Set_Context(SNMP_ChannelPtr, void *Context);

void
SNMP_Channel_Execute(SNMP_ChannelPtr);

/**
 * Process a SNMP TRAP.
 * Verifies if TRAP originates from connected NE.
 */
void
SNMP_Channel_On_Trap(SNMP_Channel*, netsnmp_pdu*);

ErrCode
SNMP_Channel_Dispatch(SNMP_Channel*, SNMP_Channel_Event*);

/**
 * Lock SNMP_Channel object.
 */
void
SNMP_Channel_Lock(SNMP_ChannelPtr);

/*
 * Unlock SNMP_Channel object.
 */
void
SNMP_Channel_Unlock(SNMP_ChannelPtr);

/**
 * Start processing.
 */
ErrCode
SNMP_Channel_Start(SNMP_ChannelPtr);

/*
void
SNMP_Channel_Run(SNMP_ChannelPtr);
*/

void
SNMP_Channel_Stop(SNMP_ChannelPtr);

/**
 * Assign callback function
 * @param pCh   Pointer to SNMP_Channel
 * @param type  SNMP_Channel callback type
 */
ErrCode
SNMP_Channel_SetCallback(SNMP_ChannelPtr pCh, int type, SNMP_Channel_Callback_t f);

#define SNMP_Channel_ClearCallback(ch,type) \
    SNMP_Channel_SetCallback(ch, type, NULL)

void
SNMP_Channel_ClearAllCallbacks(SNMP_ChannelPtr);

/**
 * Create a netsnmp_session duplicate.
 */
netsnmp_session*
SNMP_Channel_Duplicate_Session(SNMP_ChannelPtr);

/**
 * Destroy netsnmp_session.
 */
void
SNMP_Channel_Destroy_Session(SNMP_ChannelPtr, netsnmp_session*);

/** internal API **/

ErrCode
_SNMP_Channel_Create_Session(SNMP_Channel*, SNMP_Channel_Conf*);

void
_SNMP_Channel_State_Change(SNMP_Channel*,int);

ErrCode
_SNMP_Channel_LoadConfig(SNMP_Channel*);

ErrCode
_SNMP_Channel_Load_Inventory(SNMP_Channel*, SNMP_Channel_Inventory*);

int
_SNMP_Channel_LoadConfigCP(SNMP_Channel*);

ErrCode
_SNMP_Channel_Load_InventoryCP(SNMP_Channel*, SNMP_Channel_InventoryCP*);

/** Reload HW related data **/
ErrCode
_SNMP_Channel_Reload_HW_Config(SNMP_ChannelPtr);

/**
 * Merge SHELF entities.
 */
void
_SNMP_Channel_Merge_Shelves(SNMP_ChannelPtr, X_ListPtr);

/**
 * Reload and update CP inventory (tunnels + CNXs).
 */
int
_SNMP_Channel_ReloadConfigCP(SNMP_ChannelPtr);

/**
 * Merge TNL_WDM data.
 */
void
_SNMP_Channel_Merge_Tunnels(SNMP_ChannelPtr, X_ListPtr);

/**
 * Merge CNX_WDM data.
 */
void
_SNMP_Channel_Merge_CNXs(SNMP_ChannelPtr, X_ListPtr);

int
_SNMP_Channel_Poll(SNMP_Channel*);

SNMP_Channel_Event*
_SNMP_Channel_GetEvent(SNMP_Channel*);

void
_SNMP_Channel_PutEvent(SNMP_Channel*, SNMP_Channel_Event*);

void
_SNMP_Channel_Timer(SNMP_Channel*, int timer, int start);

void
_SNMP_Channel_ProcessTimers(SNMP_Channel*);

void
_SNMP_Channel_HandleNotInit(SNMP_Channel*,SNMP_Channel_Event*);

void
_SNMP_Channel_HandleDisconnected(SNMP_Channel*,SNMP_Channel_Event*);

void
_SNMP_Channel_HandleSynch(SNMP_Channel*,SNMP_Channel_Event*);

void
_SNMP_Channel_HandleReady(SNMP_Channel*,SNMP_Channel_Event*);

void
_SNMP_Channel_Callback(SNMP_Channel*,int cb,void*);

/***********/

const char*
SNMP_Channel_GetStateName(int);

void
SNMP_Channel_ThreadProc(void*);


/******************************************************************************/

/** API for Workers & Tasks **/

typedef enum
{
    SNMP_CHANNEL_TASK_LOAD_CONFIG,

    SNMP_CHANNEL_TASK_LOAD_CONFIG_HW,

    SNMP_CHANNEL_TASK_LOAD_CONFIG_CP,

} SNMP_Channel_Task_t;

typedef struct _SNMP_Channel_Task
{
    X_Task              Task;

    SNMP_Channel_Task_t Type;

    SNMP_Channel_Event  Event;

    SNMP_ChannelPtr     Channel;

} SNMP_Channel_Task;

typedef SNMP_Channel_Task* SNMP_Channel_TaskPtr;

/** Create new task **/
SNMP_Channel_TaskPtr
SNMP_Channel_Task_New(SNMP_Channel_Task_t, SNMP_ChannelPtr);

/** Destroy task **/
void
SNMP_Channel_Task_Free(SNMP_Channel_TaskPtr);

/** Put task into a task queue **/
ErrCode
SNMP_Channel_Schedule_Task(SNMP_ChannelPtr, SNMP_Channel_Task_t, int evtype);

/** Remove all pending tasks from a task queue **/
void
SNMP_Channel_Remove_Pending_Tasks(SNMP_ChannelPtr);

/** Task dispatch callback function **/
void
SNMP_Channel_Task_Dispatch(SNMP_Channel_TaskPtr);

#endif
