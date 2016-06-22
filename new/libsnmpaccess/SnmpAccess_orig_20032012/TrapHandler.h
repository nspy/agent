
#ifndef __TRAP_HANDLER_H__
#define __TRAP_HANDLER_H__

#include <SnmpAccess/Log.h>
#include <SnmpAccess/List.h>
#include <SnmpAccess/Threads.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

struct _SNMP_Channel;
struct _SNMP_TrapHandler;

typedef struct _SNMP_TrapHandler* SNMP_TrapHandlerPtr;

/******************************************************************************/

/** Callback function type for receiving traps **/
typedef void (*SNMP_TrapHandler_Callback_t)(void*,struct snmp_pdu*);

/******************************************************************************/

typedef struct _SNMP_TrapHandler
{
    /** Local address to bind TrapHandler **/
    char                        LocalAddr[64];

    /** Callback for incoming traps **/
    SNMP_TrapHandler_Callback_t Callback;

    /** SNMP session handle **/
    struct snmp_session*        Session;

    /** List of SNMP_Channels **/
    X_ListPtr                   Listeners;

    X_MutexPtr                  Mutex;

    ///

    int                         Run;

    X_ThreadPtr                 Thread;

} SNMP_TrapHandler;


/****************** public API ************************************************/


SNMP_TrapHandlerPtr
SNMP_TrapHandler_New(const char *addr);

SNMP_TrapHandlerPtr
SNMP_TrapHandler_Init(SNMP_TrapHandlerPtr, const char*);

void
SNMP_TrapHandler_Free(SNMP_TrapHandlerPtr);

void
SNMP_TrapHandler_SetCallback(SNMP_TrapHandler*,SNMP_TrapHandler_Callback_t);

void
SNMP_TrapHandler_Execute(SNMP_TrapHandler*);

void
SNMP_TrapHandler_Add(SNMP_TrapHandler*, struct _SNMP_Channel*);

void
SNMP_TrapHandler_Remove(SNMP_TrapHandler*, struct _SNMP_Channel*);

void
SNMP_TrapHandler_Start(SNMP_TrapHandler*);

void
SNMP_TrapHandler_Run(SNMP_TrapHandler*);

/********** private API **********/

/**
 * Initialize snmp_session handle.
 * @return 0 on success, != 0 otherwise
 */
int
_SNMP_TrapHandler_InitSession(SNMP_TrapHandler*);

/**
 * Internal callback for receiving traps from net-snmp lib.
 */
int
_SNMP_TrapHandler_Callback(int, netsnmp_session*, int, netsnmp_pdu*, void*);

/**
 * Internal callback for receiving traps from net-snmp lib.
 */
int
_SNMP_TrapHandler_Callback(int, netsnmp_session*, int, netsnmp_pdu*, void*);

void
_SNMP_TrapHandler_Lock(SNMP_TrapHandler*);

void
_SNMP_TrapHandler_Unlock(SNMP_TrapHandler*);

void
SNMP_TrapHandler_ThreadProc(void*);

#endif
