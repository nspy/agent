
#ifndef __AGENT_H__
#define __AGENT_H__

#include <SnmpAccess/SnmpAccess.h>
#include <SnmpAccess/OpenFlow.h>
#include "openflow.h"
#include <SnmpAccess/Vector.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
//#include <sstream>
//#include <fstream>
#include <string.h>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

//using namespace std;

/** The core structure of the Agent **/
typedef struct
{
    /***********************************************************/

    /** SNMP channel configuration **/
    SNMP_Channel_Conf   SnmpConf;

    /** Address of the SNMP trap listener **/
    char                SnmpTrap[64];

    /** Address of the OpenFlow controller **/
    char                OpenFlowAddr[64];

    /** SNMP channel **/
    SNMP_ChannelPtr     SnmpChannel;

    /** SNMP trap handler **/
    SNMP_TrapHandlerPtr SnmpTrapHandler;

    /** Resource database **/
    X_ResourcePtr       Resources;

    /** OpenFlow communication channel **/
    OpenFlow_DriverPtr  OpenFlowDrv;

    /***********************************************************/


    /** System IP / router ID of the Network Element **/
    unsigned int        SysIP;

    /** List of OpenFlow ports **/
    X_ListPtr           Ports;

    //VectorPtr           Constraints;

} X_Core;

typedef X_Core* X_CorePtr;

/**
 * Create X_Core from the configuration stored in a XML file.
 */
X_CorePtr
X_Core_Create_From_File(const char *path);

void
X_Core_Free(X_CorePtr);

void
X_Core_Generate_OpenFlow_Ports(X_CorePtr);

void
X_Core_Handle_Features_Request(X_CorePtr);

void
X_Core_Switching_Constraints(X_CorePtr);

void
X_Core_Handle_OpenFlow_Message(X_CorePtr, struct ofp_header*);

void
X_Core_cport_status(X_CorePtr pCore, uint8_t reason);

struct ofp_phy_cport*
X_Core_Find_Port(X_CorePtr, X_ModulePtr);

/** Process OOE_MGMT_INFO_REQUEST message **/
void
X_Core_Process_Mgmt_Info_Request(X_CorePtr);

/** Process OOE_POWER_EQ_REQUEST message **/
void
X_Core_Process_Power_Equalization_Request(X_CorePtr, struct ooe_power_eq*);

int
sendCommand (char * hostName, int portNum ,char * pMsg);

#define BUF_SIZE 2056



#endif
