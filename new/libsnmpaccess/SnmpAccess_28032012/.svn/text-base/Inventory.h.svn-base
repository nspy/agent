
#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <SnmpAccess/List.h>
#include <SnmpAccess/Types.h>

/******************************************************************************/

/** **/
#define EXTERNAL_CHANNEL        (0x10000000)

/*
 * Maximum length of the AID's textual representation
 */
#define MAX_AID_LEN             (32)

/*
 * Maximum length of the TNL_xxx identifier
 */
#define MAX_TUNNEL_ID           (32)

/** (Default) Memory free function **/
#define EntityFree  (free)

/** Default function for copying EntityXXX objects **/
#define EntityCopy (memcpy)

/** Type on an Entity object (internal type, not an ADVA's F7 type!) **/
typedef enum
{
    ENTITY_TYPE_UNKNOWN = 0,
    ENTITY_TYPE_GENERIC = 1,
    ENTITY_TYPE_NE,
    ENTITY_TYPE_SHELF,
    ENTITY_TYPE_MOD,
    ENTITY_TYPE_PLUG,
    ENTITY_TYPE_CONN,
    ENTITY_TYPE_PTP,
    ENTITY_TYPE_OL,
    ENTITY_TYPE_TUNNEL,
    ENTITY_TYPE_CNX, // 10
    ENTITY_TYPE_TE_NUM_LINK,
    ENTITY_TYPE_TE_UNNUM_LINK,
    ENTITY_TYPE_PATH,
    ENTITY_TYPE_PATH_ELEMENT,
    ENTITY_TYPE_MAX,
} EntityType_t;

typedef void*   (*EntityNew_t)(void);
typedef void*   (*EntityClear_t)(void*);
typedef void*   (*EntityDup_t)(void*);
typedef void*   (*EntityCopy_t)(void*,const void*,size_t);
typedef size_t  (*EntityDump_t)(void*,char*,size_t);
typedef void    (*EntityFree_t)(void*);

/******************************************************************************/

/**
 * Generic Entity object.
 */
typedef struct
{
    /** Index to the SNMP table (usually 0 means invalid/undefined) **/
    unsigned long       Index;

    /** For CRS_CH entities **/
    unsigned long       Index2;

    /** ADVA's F7 specific class of an entity ( 0 - undefined )**/
    unsigned int        Class;

    /** ADVA's F7 specific class of a Control Plane entity ( 0 - undefined ) **/
    unsigned int        CPClass;

    /** Size of the Entity object **/
    unsigned int        Size;

    /** Type of an Entity object (ENTITY_TYPE_XXX) **/
    EntityType_t        Type;

} Entity;

typedef Entity* EntityPtr;

/**
 * Create a new (generic) Entity object.
 */
EntityPtr
Entity_New(void);

/**
 * Initialize Entity object.
 */
void
Entity_Init(EntityPtr);

/**
 * Create a duplicate of a an Entity object.
 */
EntityPtr
Entity_Dup(const EntityPtr);

/**
 * Retrieve a SNMP index from an entity object.
 */
#define Entity_Get_Index(e) \
    ((e != NULL) ? (((EntityPtr)(e))->Index) : (0))

/**
 * Set a SNMP index.
 */
#define Entity_Set_Index(e, idx) \
    if (e != NULL) { ((EntityPtr)(e))->Index = idx; }

/**
 * Create an Entity object of a given class.
 */
EntityPtr
Entity_Create(EntityType_t);

#define Entity_Dump(e,b,s) \
        __Entity_Dump((EntityPtr)e, b, s)

size_t
__Entity_Dump(EntityPtr, char*, size_t);


/******************************************************************************/

/** Global, per NE data **/
typedef struct
{
    Entity              Generic;
    unsigned int        SysIP;
    unsigned int        SysIPMask;
} EntityNE;

void
EntityNE_Init(EntityNE*);

size_t
EntityNE_Dump(EntityNE*,char*,size_t);

/******************************************************************************/

/** SHELF description **/
typedef struct
{
    Entity              Generic;
    unsigned int        Type;
    unsigned int        Admin;
    unsigned int        Oper;
    char                Aid[MAX_AID_LEN];
} EntityShelf;

EntityShelf*
EntityShelf_New(void);

void
EntityShelf_Init(EntityShelf*);

EntityShelf*
EntityShelf_Dup(const EntityShelf*);

size_t
EntityShelf_Dump(const EntityShelf*,char*,size_t);

/******************************************************************************/

/** MOD description **/
typedef struct
{
    Entity          Generic;
    unsigned int    Type;
    unsigned int    Mode;
    unsigned int    Admin;
    unsigned int    Oper;
    unsigned int    RoadmNo;
    unsigned int    Band;
    unsigned int    Channel;
    char            Aid[MAX_AID_LEN];
} EntityModule;

EntityModule*
EntityModule_New(void);

void
EntityModule_Init(EntityModule*);

EntityModule*
EntityModule_Dup(const EntityModule*);

size_t
EntityModule_Dump(const EntityModule*,char*,size_t);

/******************************************************************************/

typedef struct
{
    Entity          Generic;
    unsigned long   ContainedIn;
    unsigned int    Type;
    unsigned int    Admin;
    unsigned int    Oper;
    char            Aid[MAX_AID_LEN];
} EntityPlug;

EntityPlug*
EntityPlug_New(void);

void
EntityPlug_Init(EntityPlug*);

EntityPlug*
EntityPlug_Dup(const EntityPlug*);

/******************************************************************************/

/** CONN description **/
typedef struct
{
    Entity          Generic;
    unsigned int    From;
    unsigned int    To;
    unsigned int    Type;
    char            FromAid[MAX_AID_LEN];
    char            ToAid[MAX_AID_LEN];
} EntityConnection;

EntityConnection*
EntityConnection_New(void);

void
EntityConnection_Init(EntityConnection*);

EntityConnection*
EntityConnection_Dup(const EntityConnection*);

size_t
EntityConnection_Dump(const EntityConnection*, char *b, size_t);

/******************************************************************************/

typedef struct
{
    Entity          Generic;
    unsigned long   ContainedIn;
    char            Aid[MAX_AID_LEN];
} EntityPTP;

EntityPTP*
EntityPTP_New(void);

void
EntityPTP_Clear(EntityPTP*);

EntityPTP*
EntityPTP_Copy(const EntityPTP*);

size_t
EntityPTP_Dump(const EntityPTP*, char*, size_t);

/******************************************************************************/

typedef struct
{

    Entity      Generic;

    char        Aid[MAX_AID_LEN];

    /** List of WCHs available on the OL **/
   // X_ListPtr   Wch;
} EntityOL;

void
EntityOL_Init(EntityOL*);

EntityOL*
EntityOL_Copy(const EntityOL*);

size_t
EntityOL_Dump(const EntityOL*, char*, size_t);

/******************************************************************************/

typedef struct
{
    Entity          Generic;
    unsigned int    Admin;
    unsigned int    ToIP;
    unsigned int    FromAid;
    unsigned int    ToAid;
    char            Id[MAX_TUNNEL_ID];
    unsigned int    Facility;
} EntityTunnel;

EntityTunnel*
EntityTunnel_New(void);

void
EntityTunnel_Init(EntityTunnel*);

EntityTunnel*
EntityTunnel_Dup(const EntityTunnel*);

size_t
EntityTunnel_Dump(EntityTunnel*,char*,size_t);

void
EntityTunnel_Set_Id(EntityTunnel*, const char*);

/******************************************************************************/

typedef struct
{
    unsigned long       Index;
    unsigned int        Admin;
    unsigned int        SecondayStates;
    unsigned int        ComputationState;
    unsigned int        ErrCategory;
    char                ErrMsg[128];
} EntityTunnelResult;

/******************************************************************************/

typedef struct
{
    Entity          Generic;
    unsigned int    Oper;
    unsigned int    TunNo;
    unsigned int    FromNodeIP;
    unsigned int    ToNodeIP;
    unsigned int    EqState;
    char            Id[MAX_TUNNEL_ID];
} EntityCNX;

void
EntityCNX_Init(EntityCNX*);

EntityCNX*
EntityCNX_Copy(const EntityCNX*);

size_t
EntityCNX_Dump(const EntityCNX*,char*,size_t);

/******************************************************************************/

typedef struct
{
    Entity       Generic;
    unsigned int NodeIP;
    unsigned int LinkId;
    unsigned int PhyLink;
    unsigned int Synch;
    unsigned int FendNodeIP;
    unsigned int FendLinkId;
} EntityTEUnnumLink;

EntityTEUnnumLink*
EntityTEUnnumLink_New(void);

void
EntityTEUnnumLink_Init(EntityTEUnnumLink*);

EntityTEUnnumLink*
EntityTEUnnumLink_Copy(const EntityTEUnnumLink*);

size_t
EntityTEUnnumLink_Dump(EntityTEUnnumLink*,char*,size_t);

/******************************************************************************/

typedef EntityTEUnnumLink EntityTENumLink;

EntityTENumLink*
EntityTENumLink_New(void);

#define EntityTENumLink_Init   EntityTEUnnumLink_Init

#define EntityTENumLink_Copy   EntityTEUnnumLink_Copy


#if 0
EntityTENumLink*
EntityTENumLink_New(void);

void
EntityTENumLink_Init(EntityTENumLink*);

EntityTENumLink*
EntityTENumLink_Copy(const EntityTENumLink*);
#endif

size_t
EntityTENumLink_Dump(EntityTENumLink*,char*,size_t);

/******************************************************************************/

typedef struct
{
    Entity              Generic;
    char                PathID[32];
} EntityPath;

void
EntityPath_Init(EntityPath*);

/******************************************************************************/

typedef struct
{
    Entity              Generic;
    unsigned int        Type; // TE type
    unsigned int        NodeIP;
    unsigned int        LifIP;
    unsigned int        LifID;
    unsigned int        Seq;
    unsigned int        Follow; // Strict/Loose
    unsigned int        UpChannel;
    unsigned int        DownChannel;
    char                PathID[32];
} EntityPathElement;

EntityPathElement*
EntityPathElement_New(void);

void
EntityPathElement_Init(EntityPathElement*);

EntityPathElement*
EntityPathElement_Dup(EntityPathElement*);

size_t
EntityPathElement_Dump(EntityPathElement*,char*,size_t);

/******************************************************************************/

typedef struct
{
    Entity              Generic;
    char                Aid[MAX_AID_LEN];
} EntityFacility;

/******************************************************************************/


typedef struct
{
    /** SNMP index **/
    unsigned long       Index;
    /** AID **/
    char                Aid[MAX_AID_LEN];
    /** entity class **/
    unsigned int        Class;
    /** assignment state **/
    unsigned int        Assignment;
} AuxEntityData;

void
AuxEntityData_Init(AuxEntityData*);

AuxEntityData*
AuxEntityData_Dup(const AuxEntityData*);

void
AuxEntityData_Free(AuxEntityData*);

/******************************************************************************/

typedef struct
{
    unsigned long       From;
    unsigned long       To;
    char                FromAid[MAX_AID_LEN];
    char                ToAid[MAX_AID_LEN];
} EntityCrs;

void
EntityCrs_Init(EntityCrs*);

EntityCrs*
EntityCrs_Dup(const EntityCrs*);

size_t
EntityCrs_Dump(const EntityCrs*, char*, size_t);

/******************************************************************************/

EntityPTP*
Find(unsigned long, X_ListPtr);

X_ListNodePtr
__FindByIndex(const Entity*, X_ListPtr, Entity**);

#define FindByIndex(e,l,ee) \
    __FindByIndex((const Entity*)(e), (l), (Entity**)(ee))

X_ListNodePtr
FindTELink(X_ListPtr, unsigned int NodeIP, unsigned int LinkID, EntityTEUnnumLink**);

/**
 * Find CNX that matches given tunnel.
 * @param pList List of CNXs
 * @param pTnl  Tunnel description
 * @param pCnx  (out) CNX or NULL
 */
int
FindCNX(X_ListPtr pList, const EntityTunnel *pTnl, X_ListNodePtr *pCnx);

/**
 * Compare by index value.
 */
int
CompareByIndex(const Entity*, const Entity*);

void SetAid(char*, const char *, size_t);

const char *PrintAdmin(unsigned int);

/******************************************************************************/

/**
 * Convert OL's number into ADVA's GMPLS physical link id.
 */
int
OL_To_PhyLink(int ol_num);

/**
 * Convert OL's number into ADVA's GMPLS physical link id.
 */
int
OL_To_LogicalInterfaceIndex(int ol_num);

/**
 * Convert phyiscal link id to OL number.
 */
int
PhyLink_To_OL(int id);

/**
 * Parse TE links and locate peering NE and OL.
 */
int
FindPeer(X_ListPtr Num, X_ListPtr Unnum,
        unsigned int IP, int OL,
        unsigned int *PeerIP, int *PeerOL);

int
FindPeerLink(X_ListPtr links, const EntityTEUnnumLink*, EntityTEUnnumLink**);

/******************************************************************************/

/** NE-wide cross-connect description.
 * Should be used for the OpenFlow interface.
 **/
typedef struct
{
    /** IP address of the NE **/
    unsigned int NE;
    /** Input port **/
    unsigned int Input;
    /** Output port **/
    unsigned int Output;
    /** Input channel **/
    unsigned int InChannel;
    /** Output channel **/
    unsigned int OutChannel;
} CrossConnect;

/** Initialize with default values **/
void
CrossConnect_Init(CrossConnect*);

/** Duplicate CrossConnect structure **/
CrossConnect*
CrossConnect_Dup(const CrossConnect*);

/** Free memory assigned to CrossConnect **/
#define CrossConnect_Free \
    free

/**
 * Converts path description from ADVA's syntax to a series of CrossConnect structures.
 * @param NumLinks      (in) List of numbered TE links.
 * @param UnnumLinks    (in) List of unnumbered TE links.
 * @param PELs          (in) List of EntityPathElement
 * @param crs           (out) List of CrossConnects
 */
int
ConvertPathDescriptionFromGMPLS(X_ListPtr NumLinks, X_ListPtr UnnumLinks,
        X_ListPtr PELs, X_ListPtr crs);

int
ConvertPathDescriptionToGMPLS(X_ListPtr NumLinks, X_ListPtr UnnumLinks,
        X_ListPtr CrsList, X_ListPtr PELList);

/******************************************************************************/

/**
 * Convert ADVA's channel type to frequency expressed in GHz.
 * @return Frequency in GHz or (-1) on failure.
 */
int
ADVA_Channel_To_Freq(Channel_t);

Channel_t
Freq_To_ADVA_Channel(int);

/******************************************************************************/

/**
 * Return the highest/lowest ADMIN state.
 * @param Caps - Bitmap of supported ADMIN states
 * @param Dsbld - if 0 then return highest else lowest
 */
unsigned int
GetAdminState(unsigned int Caps, int Dsbld);


#endif
