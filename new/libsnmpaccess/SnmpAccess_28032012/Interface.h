
#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <SnmpAccess/Common.h>
#include <SnmpAccess/Inventory.h>
#include <SnmpAccess/List.h>
#include <SnmpAccess/Vector.h>
#include <SnmpAccess/Log.h>
#include <SnmpAccess/Utils.h>

#ifndef IMPORT_MIB
#include <SnmpAccess/MIB.h>
#endif

/**
 * Collection of functions to interact via SNMP interface.
 */

/******************************************************************************/

/**
 * Retrieve system uptime info.
 */
ErrCode
SNMP_GetSysUptime(netsnmp_session*, SNMP_ErrCodePtr, unsigned int*);

/******************************************************************************/


#if 0
/**
 * Retrieve per NE information.
 * @param   err - SNMP error code
 * @return  STAT_SUCCESS (0)    - success
 *          STAT_ERROR (1)      - internal error (?)
 *          STAT_TIMEOUT (2)    - timeout (disconnected)
 */
int
GetEntityNE(netsnmp_session*, int *err, EntityNE*);

/******************************************************************************/

/**
 * Retrieve per SHELF information.
 * @param   err - SNMP error code
 * @return  STAT_SUCCESS (0)    - success
 *          STAT_ERROR (1)      - internal error (?)
 *          STAT_TIMEOUT (2)    - timeout (disconnected)
 */
int
GetEntityShelf(netsnmp_session*, int *err, unsigned long index, EntityShelf*);

/******************************************************************************/

/**
 * Retrieve list of SHELFs.
 * @return  STAT_SUCCESS (0)    - success
 *          STAT_ERROR (1)      - internal error (?)
 *          STAT_TIMEOUT (2)    - timeout (disconnected)
 */
int
GetEntityShelfList(netsnmp_session*, int *err, X_ListPtr);
#endif

#if 0
int
GetEntityModule(struct snmp_session*,
        int *err,
        unsigned long index,
        EntityModule*);

int
GetEntityModuleList(struct snmp_session*,
        int *err,
        X_ListPtr);
#endif

int
GetEntityPlug(struct snmp_session*,
        int *err,
        unsigned long index,
        EntityPlug*);

int
GetEntityPlugList(netsnmp_session*, int *err, X_ListPtr);

#if 0
int
GetEntityConnection(netsnmp_session*,int *err, unsigned long index, EntityConnection*);

int
GetEntityConnectionList(netsnmp_session*, int *err, X_ListPtr);
#endif

#if 0
int
GetEntityPTP(netsnmp_session*, int *err, unsigned long index, EntityPTP*);

int
GetEntityPTPList(struct snmp_session*,
        int *err,
        X_ListPtr);

int
GetEntityOL(struct snmp_session*,
        int *err,
        unsigned long index,
        EntityOL*);

int
GetEntityOLList(struct snmp_session*,
        int *err,
        X_ListPtr);
#endif

#if 0
int
GetEntityTunnelWDM(struct snmp_session*,
        int *err,
        unsigned long index,
        EntityTunnel*);

int
GetEntityTunnelWDMList(struct snmp_session*,
        int *err,
        X_ListPtr);

int
GetEntityCnxWDM(struct snmp_session*,
        int *err,
        unsigned long index,
        EntityCNX*);

int
GetEntityCnxWDMList(struct snmp_session*,
        int *err,
        X_ListPtr);

#endif

int
GetEntityTEUnnumLink(struct snmp_session*,
        int *err,
        unsigned int ip,
        unsigned int id,
        EntityTEUnnumLink*);

int
GetEntityTEUnnumLinkList(struct snmp_session*,
        int *err,
        X_ListPtr);

int
GetUnusedWDMTunnelIndex(struct snmp_session*,
        int *err,
        unsigned long *index);

int
GetEntityContainedIn(struct snmp_session*,
        unsigned long,
        unsigned long*);

///////

#if 0

int
SNMP_CreateWDMTunnel(netsnmp_session*, int *err, EntityTunnel*);

#endif
///////////
/******************************************************************************/

ErrCode
SNMP_GetEntityNE(netsnmp_session*, SNMP_ErrCodePtr, EntityNE*);

ErrCode
SNMP_GetEntityShelf(netsnmp_session*, SNMP_ErrCodePtr, EntityShelf*);

ErrCode
SNMP_GetEntityShelfList(netsnmp_session*, SNMP_ErrCodePtr, X_ListPtr);

ErrCode
SNMP_GetEntityModule(netsnmp_session*, SNMP_ErrCodePtr, EntityModule*);

ErrCode
SNMP_GetEntityModuleList(netsnmp_session*, SNMP_ErrCodePtr, X_ListPtr);

ErrCode
SNMP_GetEntityPlug(netsnmp_session*, SNMP_ErrCodePtr, EntityPlug*);

ErrCode
SNMP_GetEntityPlugList(netsnmp_session*, SNMP_ErrCodePtr, X_ListPtr);

ErrCode
SNMP_GetEntityConnection(netsnmp_session*, SNMP_ErrCodePtr, EntityConnection*);

ErrCode
SNMP_GetEntityConnectionList(netsnmp_session*,SNMP_ErrCodePtr, X_ListPtr);

ErrCode
SNMP_GetEntityOL(netsnmp_session*,SNMP_ErrCodePtr, EntityOL*);

ErrCode
SNMP_GetEntityOLList(netsnmp_session*,SNMP_ErrCodePtr, X_ListPtr);

ErrCode
SNMP_GetEntityCnxWDM(netsnmp_session*, SNMP_ErrCodePtr, EntityCNX*);

ErrCode
SNMP_GetEntityCnxWDMList(netsnmp_session*,SNMP_ErrCodePtr,X_ListPtr);

ErrCode
SNMP_GetEntityTunnelWDM(netsnmp_session*, SNMP_ErrCodePtr, EntityTunnel*);

ErrCode
SNMP_GetEntityTunnelWDMList(netsnmp_session*,SNMP_ErrCodePtr, X_ListPtr);

ErrCode
SNMP_GetEntityTEUnnumLinkList(netsnmp_session*,SNMP_ErrCodePtr, X_ListPtr);

ErrCode
SNMP_GetEntityTENumLinkList(netsnmp_session*,SNMP_ErrCodePtr, X_ListPtr);

///

/**
 * Find and return first matching CNX for tunnel.
 */
ErrCode
SNMP_GetCnxWDMForTunnel(netsnmp_session*, SNMP_ErrCodePtr, const EntityTunnel*, EntityCNX*);

ErrCode
SNMP_GetPathWdm(netsnmp_session*, SNMP_ErrCodePtr, EntityPath*);

ErrCode
SNMP_GetPEL(netsnmp_session*, SNMP_ErrCodePtr, EntityPathElement*);

ErrCode
SNMP_GetPELs(netsnmp_session*, SNMP_ErrCodePtr, const EntityPath*, X_ListPtr);

ErrCode
SNMP_GetSignalledPath(netsnmp_session*,SNMP_ErrCodePtr,const EntityCNX*,X_ListPtr);

/*
 * Get the computed path for a tunnel.
 */
ErrCode
SNMP_GetComputedPath(netsnmp_session*,SNMP_ErrCodePtr, unsigned long idx, X_ListPtr);

///
ErrCode
SNMP_GetUnusedWDMTunnelIndex(netsnmp_session*, SNMP_ErrCodePtr, unsigned long*);

ErrCode
SNMP_GetEntityClass(netsnmp_session*, SNMP_ErrCodePtr, unsigned long index, unsigned int*);

ErrCode
SNMP_GetCPWDMEntityClass(netsnmp_session*, SNMP_ErrCodePtr, unsigned long index, unsigned int*);

ErrCode
SNMP_CreateTunnel(netsnmp_session*, SNMP_ErrCodePtr, EntityTunnel*);

ErrCode
SNMP_SetTunnelAdmin(netsnmp_session*, SNMP_ErrCodePtr, EntityTunnel*, unsigned int admin);

ErrCode
SNMP_GetTunnelResult(netsnmp_session*, SNMP_ErrCodePtr, EntityTunnelResult*);

ErrCode
SNMP_DestroyTunnel(netsnmp_session*, SNMP_ErrCodePtr, long);


/**
 * Fetch auxilary data associated with a regular entity.
 */
ErrCode
SNMP_GetAuxData(netsnmp_session*, SNMP_ErrCodePtr, AuxEntityData*);

/**
 * Fetch auxialary data associated with a CP entity.
 */
ErrCode
SNMP_GetAuxData_CP_WDM(netsnmp_session*, SNMP_ErrCodePtr, AuxEntityData*);

/**
 * Returns a list of AuxEntityData structures describing contained entities.
 */
ErrCode
SNMP_Contains(netsnmp_session*, SNMP_ErrCodePtr, unsigned long Index, X_ListPtr);

/**
 * Returns a list of AuxEntityData structures describing contained entities for CP-WDM entities.
 */
ErrCode
SNMP_Contains_CP_WDM(netsnmp_session*, SNMP_ErrCodePtr, unsigned long idx, X_ListPtr);

ErrCode
SNMP_Create_OM_Facility(netsnmp_session*, SNMP_ErrCodePtr, unsigned long idx);

/**
 * Create CH entity.
 * @param Index - index of the entity
 * @param FacType - facility type (if 0 then do not set)
 * @param Channel -
 *
 */
ErrCode
SNMP_Create_CH_Facility(netsnmp_session*, SNMP_ErrCodePtr,
        unsigned long Index, unsigned int FacType, unsigned int Channel);

/**
 * Create a VCH entity.
 * @param Index Index of an entity
 */
ErrCode
SNMP_Create_VCH_Facility(netsnmp_session*, SNMP_ErrCodePtr,
        unsigned long Index);

/**
 * Create WCH entity.
 */
ErrCode
SNMP_Create_WCH(netsnmp_session*, SNMP_ErrCodePtr, unsigned long Index);

/**
 * Destroy WCH entity.
 */
ErrCode
SNMP_Destroy_WCH(netsnmp_session*, SNMP_ErrCodePtr, unsigned long Index);

/**
 * Destroy "interface" entity (e.g. CH).
 */
ErrCode
SNMP_Destroy_If(netsnmp_session*, SNMP_ErrCodePtr, unsigned long Index);

/*
 * Destroy VCH entity.
 * @param Index Index of the VCH entity.
 */
ErrCode
SNMP_Destroy_VCH(netsnmp_session*, SNMP_ErrCodePtr, unsigned long Index);

/**
 * Create CRS entity
 */
ErrCode
SNMP_Create_Crs(netsnmp_session*, SNMP_ErrCodePtr,
        unsigned long From, unsigned long To, unsigned int FacType, unsigned int PathNode);

ErrCode
SNMP_Get_Crs(netsnmp_session*, SNMP_ErrCodePtr,
        unsigned int From, unsigned int *To);

//

/**
 * Create a PATH_WDM and associated PEL_WDMs
 */
ErrCode
SNMP_CreatePath(netsnmp_session*, SNMP_ErrCodePtr,
        EntityPath *pPath, X_ListPtr pPathElementList);

ErrCode
SNMP_DeletePath(netsnmp_session*, SNMP_ErrCodePtr, unsigned long);

/**
 * Create PEL_WDM
 */
ErrCode
SNMP_CreatePathElement(netsnmp_session*, SNMP_ErrCodePtr, EntityPathElement *pPEL);

/**
 * Attach paths to a tunnel.
 * @param TnlIndex      Index of a TNL_WDM
 * @param Path1Index    Index of the working path
 * @param Path2Index    Index of the prot. path (or 0 if none)
 */
ErrCode
SNMP_AttachPath(netsnmp_session*, SNMP_ErrCodePtr,
        unsigned long TnlIndex,
        unsigned long Path1Index, unsigned long Path2Index);

/**
 * Get provisioned paths.
 */
ErrCode
SNMP_GetProvPath(netsnmp_session*, SNMP_ErrCodePtr,
        unsigned long TnlIndex,
        unsigned long *Path1Index, unsigned long *Path2Index);


//
ErrCode
SNMP_Destroy_Crs_From(netsnmp_session*, SNMP_ErrCodePtr, unsigned int from);

/**
 * Retrieve ECH parameters.
 */
ErrCode
SNMP_GetEntityExternalChannel(netsnmp_session*, SNMP_ErrCodePtr, EntityModule*);

/**
 * Get list of ECH objects.
 */
ErrCode
SNMP_GetEntityExternalChannelList(netsnmp_session*, SNMP_ErrCodePtr, X_ListPtr);

/**
 * Retrieve textual representation of an AID for a PTP or VTP entity.
 */
ErrCode
SNMP_GetPhysicalTermPointAid(netsnmp_session*,
        SNMP_ErrCodePtr, unsigned long Index, char *Buffer, size_t Size);

/**
 * Get ADMIN state capabilities from FspR7-MIB::entityStateCapAdmin.
 * @param Index - index of an entity
 * @param Caps - bitmap of settable capabilities.
 */
ErrCode
SNMP_GetEntityAdminCap(netsnmp_session*, SNMP_ErrCodePtr,
        unsigned long Index, unsigned int *Caps);

/**
 * Set ADMIN state of an entity using FspR7-MIB::entityStateCapAdmin.
 * @param Index Index
 * @param Admin Admin state
 */
ErrCode
SNMP_SetEntityAdmin(netsnmp_session*, SNMP_ErrCodePtr,
        unsigned long Index, unsigned int Admin);

/**
 * Get ADMIN state capabilities from FspR7-MIB::crsConnDeployProvCapAdmin.
 * @param From - index of a 'From' entity
 * @param To - index of a 'To' entity
 * @param Caps - bitmap of settable capabilities.
 */
ErrCode
SNMP_GetCrsAdminCap(netsnmp_session*, SNMP_ErrCodePtr,
        unsigned long From, unsigned long To, unsigned int *Caps);

/**
 * Set ADMIN state capabilities FspR7-MIB::crsConnDeployProvAdmin.
 * @param From - index of a 'From' entity
 * @param To - index of a 'To' entity
 * @param Admin - admin state
 */
ErrCode
SNMP_SetCrsAdmin(netsnmp_session*, SNMP_ErrCodePtr,
        unsigned long From, unsigned long To, unsigned int Admin);

/**
 * Set highest/lowest ADMIN state (FspR7-MIB::entityStateAdmin).
 * @param Index         Index of an entity.
 * @param Enable        If !=0 then set the highest state, if == 0 set the lowest one.
 */
ErrCode
SNMP_SetAdminEnable(netsnmp_session*, SNMP_ErrCodePtr,
        unsigned long Index, int Enable);

/**
 * Set highest/lowest ADMIN state (FspR7-MIB::crsConnDeployProvAdmin).
 * @param Enable If !=0 then set the highest state, if == 0 set the lowest one.
 */
ErrCode
SNMP_SetCrsAdminEnable(netsnmp_session*, SNMP_ErrCodePtr,
        unsigned long From, unsigned long To, int Enable);


/**
 * Fetch all CRS_CH entities.
 */
ErrCode
SNMP_GetCrsList(netsnmp_session*, SNMP_ErrCodePtr, X_ListPtr);

/**
 * Load CRS_CH entity detailed data.
 */
ErrCode
SNMP_GetEntityCrs(netsnmp_session*, SNMP_ErrCodePtr, EntityCrs*);


ErrCode
SNMP_GetCnxWdmPwrEqState(netsnmp_session*, SNMP_ErrCodePtr,
        unsigned long Index, unsigned int *State);


ErrCode
SNMP_InitiatePowerEqualization(netsnmp_session*, SNMP_ErrCodePtr, unsigned long Index);

/**
 * Load WCH entities for an OL.
 * @param Index Index of an OL entity.
 */
ErrCode
SNMP_GetWch(netsnmp_session*, SNMP_ErrCodePtr, unsigned long Index, X_VectorPtr);




#endif
