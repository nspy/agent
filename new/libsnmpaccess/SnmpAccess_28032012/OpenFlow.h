
#ifndef __OPENFLOW_H__
#define __OPENFLOW_H__

#include <SnmpAccess/Threads.h>
#include <SnmpAccess/Queue.h>
#include <SnmpAccess/List.h>
#include <SnmpAccess/Timer.h>
#include <openflow/include/openflow/openflow.h>
#include <openflow/include/openflow/optical-ext.h>
#include <openflow/lib/vconn.h>
#include <openflow/lib/ofpbuf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

/******************************************************************************/

typedef enum
{
    OPENFLOW_DRV_EV_CONNECT,
    OPENFLOW_DRV_EV_DISCONNECT,
} OpenFlow_Drvier_Event_Type;

typedef struct
{
    OpenFlow_Drvier_Event_Type  Type;
    char*                       Connection;
} OpenFlow_Driver_Event;

OpenFlow_Driver_Event*
OpenFlow_Driver_Event_New(OpenFlow_Drvier_Event_Type);

void
OpenFlow_Driver_Event_Free(OpenFlow_Driver_Event*);


typedef enum
{
    OPENFLOW_DRV_CB_FEATURES_REQ,
    OPENFLOW_DRV_CB_MESSAGE,
    OPENFLOW_DRV_CB_DISCONNECTED,
    OPENFLOW_DRV_CB_MAX,
} OpenFlow_Driver_Callback_Type;

struct _OpenFlow_Driver;

typedef struct _OpenFlow_Driver* OpenFlow_DriverPtr;

typedef int (*OpenFlow_Driver_Callback_t)(OpenFlow_DriverPtr, OpenFlow_Driver_Callback_Type, struct ofp_header*);

////////

typedef struct _OpenFlow_Driver
{
    /** Connection desription **/
    char                *Connection;
    /** Client connection **/
    struct vconn        *VConn;
    /** Server connection **/
    struct pvconn       *PVConn;

    void                *UserData;

    int                 Connected;

    int                 Run;

    X_ThreadPtr         Thread;

    X_QueuePtr          MsgQueue;

    X_Timer             ReconnectTimer;

    X_Timer             IdleTimer;

    OpenFlow_Driver_Callback_t Callbacks[OPENFLOW_DRV_CB_MAX];

} OpenFlow_Driver;

/** Create new driver object **/
OpenFlow_DriverPtr
OpenFlow_Driver_New(void);

/** Initialize with default values **/
void
_OpenFlow_Driver_Init(OpenFlow_DriverPtr);

/** Main loop **/
void
OpenFlow_Driver_Main(OpenFlow_DriverPtr);

void
OpenFlow_Driver_Connect(OpenFlow_DriverPtr,const char*);

/** Start thread **/
void
OpenFlow_Driver_Start(OpenFlow_DriverPtr);

/** Stop thread **/
void
OpenFlow_Driver_Stop(OpenFlow_DriverPtr);

int
OpenFlow_Driver_Set_Callback(OpenFlow_DriverPtr, int, OpenFlow_Driver_Callback_t);

int
OpenFlow_Driver_Set_UserData(OpenFlow_DriverPtr, void*);

int
OpenFlow_Driver_Get_UserData(OpenFlow_DriverPtr, void**);

void
_OpenFlow_Driver_Handle_Connections(OpenFlow_DriverPtr);

int
_OpenFlow_Driver_Run_Callback(OpenFlow_DriverPtr, OpenFlow_Driver_Callback_Type, struct ofp_header*);

void
_OpenFlow_Driver_Handle_Message(OpenFlow_DriverPtr, struct vconn*,struct ofp_header*);

void
_OpenFlow_Driver_Send_Raw(OpenFlow_DriverPtr, char*, size_t);

void
_OpenFlow_Driver_Connect(OpenFlow_DriverPtr);

void
_OpenFlow_Driver_Disconnect(OpenFlow_DriverPtr);

void
_OpenFlow_Driver_Send_Probe(OpenFlow_DriverPtr);

#endif
