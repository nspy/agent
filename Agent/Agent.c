
#include "Agent.h"
//#include "openflow.h"


#define DBG(a, b...) fprintf(stderr, "AGENT: "a"\r\n", ##b)

/******************************************************************************/

void
SNMP_Channel_Callback(SNMP_ChannelPtr,int, void*);

int
OpenFlow_Driver_Callback(OpenFlow_DriverPtr, OpenFlow_Driver_Callback_Type, struct ofp_header*);

X_CorePtr
X_Core_New(void);

X_CorePtr
X_Core_New_From(X_CorePtr);

void
X_Core_Pre_Init(X_CorePtr pCore);

int
X_Core_Load_From_File(X_CorePtr, const char *);

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int
sendCommand (char * hostName, int portNum ,char * pMsg);

/******************************************************************************/

int sendCommand (char * hostName, int portNum ,char * pMsg)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUF_SIZE];
    DBG("host:%s sending string: %s", hostName, pMsg);
    if (hostName == NULL ) {
       fprintf(stderr,"usage %s hostname port\n", hostName);
       return;
    }
    portno = portNum;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(hostName);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    serv_addr.sin_port = htons(portno);


    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
       {  DBG("error connecting server %s", hostName); return(0);}


    bzero(buffer,BUF_SIZE);
    //fgets(buffer,255,stdin);
    strcpy (buffer, pMsg);
    DBG("sending command %s", buffer);
    n = write(sockfd,buffer,(BUF_SIZE - 1));

    if (n < 0)
         error("ERROR writing to socket");

    bzero(buffer,BUF_SIZE);
    n = read(sockfd,buffer,(BUF_SIZE - 1));

    if (n < 0)
         error("ERROR reading from socket");

    printf("receving from Waveshaper: %s\n",buffer);
    close(sockfd);

    return 0;
}


int
X_Core_Init(X_CorePtr pCore)
{
    if (pCore == NULL)
        return 1;

    pCore->SnmpChannel = SNMP_Channel_New(&pCore->SnmpConf);

    if (pCore->SnmpChannel == NULL)
        return 1;

    pCore->SnmpTrapHandler = SNMP_TrapHandler_New(pCore->SnmpTrap);

    if (pCore->SnmpTrapHandler == NULL)
        return 1;

    pCore->Resources = X_Resource_New();

    pCore->Ports = X_List_New((X_ListMemFree_t)(free));

    pCore->OpenFlowDrv = OpenFlow_Driver_New();

    OpenFlow_Driver_Set_UserData(pCore->OpenFlowDrv, pCore);
    OpenFlow_Driver_Set_Callback(pCore->OpenFlowDrv, OPENFLOW_DRV_CB_FEATURES_REQ, OpenFlow_Driver_Callback);
    OpenFlow_Driver_Set_Callback(pCore->OpenFlowDrv, OPENFLOW_DRV_CB_MESSAGE, OpenFlow_Driver_Callback);
    OpenFlow_Driver_Set_Callback(pCore->OpenFlowDrv, OPENFLOW_DRV_CB_DISCONNECTED, OpenFlow_Driver_Callback);

    X_Resource_Set_User_Data(pCore->Resources, pCore);

    X_Resource_Set_SNMP_Channel(pCore->Resources, pCore->SnmpChannel);

    SNMP_Channel_SetCallback(pCore->SnmpChannel, SNMP_CHANNEL_CB_READY, SNMP_Channel_Callback);
    SNMP_Channel_SetCallback(pCore->SnmpChannel, SNMP_CHANNEL_CB_CONN_CREATED, SNMP_Channel_Callback);
    SNMP_Channel_SetCallback(pCore->SnmpChannel, SNMP_CHANNEL_CB_CONN_DESTROYED, SNMP_Channel_Callback);
    SNMP_Channel_SetCallback(pCore->SnmpChannel, SNMP_CHANNEL_CB_MOD_CREATED, SNMP_Channel_Callback);
    SNMP_Channel_SetCallback(pCore->SnmpChannel, SNMP_CHANNEL_CB_MOD_DESTROYED, SNMP_Channel_Callback);
    SNMP_Channel_SetCallback(pCore->SnmpChannel, SNMP_CHANNEL_CB_OL_CREATED, SNMP_Channel_Callback);
    SNMP_Channel_SetCallback(pCore->SnmpChannel, SNMP_CHANNEL_CB_OL_DESTROYED, SNMP_Channel_Callback);

    SNMP_Channel_Set_Context(pCore->SnmpChannel,(void*)pCore);

    OpenFlow_Driver_Start(pCore->OpenFlowDrv);
    OpenFlow_Driver_Connect(pCore->OpenFlowDrv, pCore->OpenFlowAddr);

    //SNMP_TrapHandler_Add(pCore->SnmpTrapHandler, pCore->SnmpChannel);

    //SNMP_TrapHandler_Start(pCore->SnmpTrapHandler);

    //SNMP_Channel_Start(pCore->SnmpChannel);

    //SNMP_Channel_Init(pCore->SnmpChannel);

    return (0);
}

X_CorePtr
X_Core_New(void)
{
    X_CorePtr pCore = (X_CorePtr)(malloc(sizeof(X_Core)));
    if (pCore != NULL)
    {
        X_Core_Pre_Init(pCore);
    }
    return pCore;
}

X_CorePtr
X_Core_New_From(X_CorePtr pCoreConf)
{
    X_CorePtr pCore = X_Core_New();

    /** Copy configuration **/
    SNMP_Channel_Conf_Copy(&pCore->SnmpConf, &pCoreConf->SnmpConf);

    memcpy(pCore->SnmpTrap, pCoreConf->SnmpTrap, sizeof(pCore->SnmpTrap));

    memcpy(pCore->OpenFlowAddr, pCoreConf->OpenFlowAddr, sizeof(pCore->OpenFlowAddr));

    /** Now initialize **/
    X_Core_Init(pCore);

    return pCore;
}

void
X_Core_Pre_Init(X_CorePtr pCore)
{
    if (pCore == NULL)
        return;

    SNMP_Channel_Conf_Init(&pCore->SnmpConf);
    strncpy(pCore->SnmpTrap, "0.0.0.0:1620", sizeof(pCore->SnmpTrap));
    strncpy(pCore->OpenFlowAddr, "192.168.1.1:6633",sizeof(pCore->OpenFlowAddr));

    pCore->SnmpChannel = NULL;
    pCore->Resources = NULL;
    pCore->SnmpTrapHandler = NULL;
    pCore->OpenFlowDrv = NULL;

    pCore->SysIP = 0;
    pCore->Ports = NULL;
}

int
X_Core_Load_From_File(X_CorePtr pCore, const char *path)
{
    xmlNodePtr root, node;
    xmlDocPtr doc;
    xmlChar *s;

    if (pCore == NULL || path == NULL)
    {
        return -1;
    }

    ////
    doc = xmlParseFile(path);

    if (doc == NULL)
    {
        return (-1);
    }

    root = xmlDocGetRootElement(doc);

    if (root == NULL)
    {
        xmlFreeDoc(doc);
        return (-1);
    }

    if (xmlStrcasecmp((xmlChar*)"config",root->name))
    {
        xmlFreeDoc(doc);
        return (-1);
    }

    for (node = root->children; node; node = node->next)
    {
        if (node->type != XML_ELEMENT_NODE)
        {
            continue;
        }

        if (!xmlStrcasecmp((xmlChar*) "host",node->name))
        {
            s = xmlNodeGetContent(node);
            SNMP_Channel_Conf_Set_Host(&pCore->SnmpConf,(char*) s);
            xmlFree(s);
        }
        else if (!xmlStrcasecmp((xmlChar*) "community",node->name))
        {
            s = xmlNodeGetContent(node);
            SNMP_Channel_Conf_Set_Community(&pCore->SnmpConf,(char*) s);
            xmlFree(s);
        }
        else if (!xmlStrcasecmp((xmlChar*) "trap",node->name))
        {
            s = xmlNodeGetContent(node);
            strncpy(pCore->SnmpTrap,(char*)s,sizeof(pCore->SnmpTrap));
            xmlFree(s);
        }
        else if (!xmlStrcasecmp((xmlChar*) "poll",node->name))
        {
            s = xmlNodeGetContent(node);
            pCore->SnmpConf.PollTimeout = atoi((char*) s);
            xmlFree(s);
        }
        else if (!xmlStrcasecmp((xmlChar*) "cpreload",node->name))
        {
            s = xmlNodeGetContent(node);
            pCore->SnmpConf.CPReloadTimeout = atoi((char*) s);
            xmlFree(s);
        }
        else if (!xmlStrcasecmp((xmlChar*)"openflow", node->name))
        {
            s = xmlNodeGetContent(node);
            strncpy(pCore->OpenFlowAddr,(char*)s,sizeof(pCore->OpenFlowAddr));
            xmlFree(s);
        }
    }

    xmlFreeDoc(doc);

    return (0);
}

/**
 * Create X_Core from the configuration stored in a XML file.
 */
X_CorePtr
X_Core_Create_From_File(const char *path)
{
    X_Core core;
    X_CorePtr pCore;

    X_Core_Pre_Init(&core);

    X_Core_Load_From_File(&core, path);

    pCore = X_Core_New_From(&core);

    return pCore;
}

void
X_Core_Free(X_CorePtr pCore)
{
    if (pCore != NULL)
    {
        return;
    }

    if (pCore->SnmpTrapHandler)
    {
        SNMP_TrapHandler_Free(pCore->SnmpTrapHandler);
    }

    if (pCore->SnmpChannel)
    {
        SNMP_Channel_Stop(pCore->SnmpChannel);

        SNMP_Channel_Free(pCore->SnmpChannel);
    }

    if (pCore->Resources)
    {
        X_Resource_Free(pCore->Resources);
    }

    free (pCore);
}

void
X_Core_Generate_OpenFlow_Ports(X_CorePtr pCore)
{
    struct ofp_phy_cport *port;
    X_ListNodePtr pNode;
    X_ModulePtr pMod;
    X_ResourcePtr pRes;
    int PortBase = 1000;
    unsigned int PeerIP;
    int PeerOL;

    assert (pCore != NULL);

    pRes = pCore->Resources;

    X_List_ForEach((&pRes->Modules), pNode)
    {
        pMod = (X_ModulePtr)(pNode->Data);

        // generate port description only for OLs and ADAPT modules
        if (pMod->ModType != X_MOD_TYPE_TRANSPDR && pMod->ModType != X_MOD_TYPE_OL)
        {
            continue;
        }

        port = malloc(sizeof(struct ofp_phy_cport));

        if (port == NULL)
            continue;

		// TTT
        //memset(port, 0, sizeof(struct ofp_phy_cport));
        memset(port, 0, sizeof(struct ofp_phy_cport) + 8);

        if (pMod->ModType == X_MOD_TYPE_TRANSPDR)
        {
            port->port_no = htons(++PortBase);
        }
        else
        {
            port->port_no = htons(pMod->Addr.Shelf - X_RES_OL_SHELF_BASE);

            if (pCore->SnmpChannel)
            {
                if (0 == FindPeer(pCore->SnmpChannel->InventoryCP.TENumLinks,
                        pCore->SnmpChannel->InventoryCP.TEUnnumLinks,
                        pCore->SnmpChannel->Inventory.NE.SysIP,
                        pMod->Addr.Shelf - X_RES_OL_SHELF_BASE,
                        &PeerIP, &PeerOL))
                {
                    X_LOGINFO(X_LOG_RES_ID, "Found peer for OL-%d: %s / OL-%d",
                            pMod->Addr.Shelf - X_RES_OL_SHELF_BASE,
                            inet_ntoa(*((struct in_addr*)&PeerIP)),
                            PeerOL);
                    // ??? //

                    port->peer_datapath_id = _htonll(PeerIP);
                    port->peer_port_no = htons(PeerOL);
                }
            }
        }

        *(unsigned int*)(&port->hw_addr) = pMod->Index;

        strcpy((char*)port->name, pMod->Aid);

        port->config = htonl((OFPPC_NO_FLOOD | OFPPC_NO_STP | OFPPC_NO_PACKET_IN));
        port->supp_swtype = htons(OFPST_WAVE);

        // TODO:
        // assign bandwidth according to constraints bitmaps from the line port


        X_List_Append(pCore->Ports, port);
    }


    X_List_ForEach(pCore->Ports, pNode)
    {
        port = (struct ofp_phy_cport*)(pNode->Data);

        X_Resource_Printf("Port: %d name = %s", ntohs(port->port_no), (char*)port->name);
        X_Resource_Printf("HW: %x:%x:%x:%x:%x:%x (%08x)",
                port->hw_addr[0],port->hw_addr[1],port->hw_addr[2],
                port->hw_addr[3],port->hw_addr[4],port->hw_addr[5],
                (*(unsigned int*)(port->hw_addr)));
    }
}

void
X_Core_Handle_Features_Request(X_CorePtr pCore)
{
    //X_ListNodePtr pNode;
    struct ofp_switch_features *osf;
    struct ofp_phy_cport *c_opp;
    char tmp[4096];
    char portName[6] = {'W', 'S','-', 'I','-','\0'};
    //char name[16] = { 'M', 'O','D','-','5','-','1','\0'}; /* Null-terminated */
    uint8_t hw_addr[6] = {00, 02, 03, 04, 05, 06};
    size_t len = sizeof(struct ofp_switch_features);
    int i=0,j;
    DBG("Size of struct ofp_switch_features: %d and cport %d", len, sizeof(struct ofp_phy_cport));

    /* Extensions for circuit switch ports */

        uint16_t peer_port_no = 11;     /* Discovered peer's switchport number */
        uint64_t peer_datapath_id = 100342; /* Discovered peer's datapath id */
        uint16_t num_bandwidth = 1;    /* Identifies number of bandwidth array elems */
        uint64_t bandwidth_feq = ( 1<<6 | 1<< 10 | 1<< 11 | 1<< 12 | 1<< 13 | \
                                   1<<14 | 1<< 15 | 1<< 16 | 1<< 17 );   /* supported bandwidth check enum port_1am_bw */

    osf = (struct ofp_switch_features*)tmp;
    //unsigned int tmpint = pCore->SnmpConf.Host;
    unsigned int tmpint; char* pAddr;
    pAddr = strdup(pCore->SnmpConf.Host);
    inet_aton(pAddr, (struct in_addr*)&tmpint);
    DBG("printing DPID:%s tmpint:%d", pCore->SnmpConf.Host, tmpint);
    uint64_t tmpport  = htobe64 (tmpint);
    
    // NEED TO CHANGE THAT
    uint64_t tmppeer1  = htobe64 (1694498816);
    uint64_t tmppeer2  = htobe64 (1711276032);
    uint64_t tmppeer3  = htobe64 (1728053248);    
    
    DBG("DPID is before  :%llu tmppot:%llu", (uint64_t)(pCore->SnmpConf.Host), tmpport);
    osf->datapath_id = htobe64 (tmpint);
    //osf->datapath_id = (uint64_t)(pCore->SnmpConf.Host);
    //osf->datapath_id = htobe64(00020345);
    osf->n_tables = (1);
    osf->n_cports = (5);// cports
    osf->capabilities = htonl(OFPC_FLOW_STATS | OFPC_TABLE_STATS | \
                              OFPC_PORT_STATS );

   /* X_List_ForEach(pCore->Ports, pNode)
    {
        opcp = (struct ofp_phy_cport*)pNode->Data;
        memcpy(tmp + len + i * (sizeof(struct ofp_phy_cport)), opcp,
                sizeof(struct ofp_phy_cport));
        i++;
    } */

	// TTT
    //c_opp = malloc(sizeof(struct ofp_phy_cport));
    c_opp = malloc(sizeof(struct ofp_phy_cport) + 8);

	// ********************** 5-port switch ************************ //
    while( i < 5) {


        int portBase = 101;

        if (c_opp == NULL)
            continue;

		// TTT
        //memset(c_opp, 0, sizeof(struct ofp_phy_cport));
        memset(c_opp, 0, sizeof(struct ofp_phy_cport) + 8);
        portBase = portBase + i;
        c_opp->port_no = htons(portBase);
        if (i == 4) portName[3] = 'O'; // hardcoding output port
        portName[5]= (char) (((int)'0')+i); // set ports
        portName[6] = '\0';
        strcpy(c_opp->name, portName);

        for (j=0; j<6; j++){
         c_opp->hw_addr[j] = hw_addr[j];
        }

        c_opp->config = htonl(OFPPC_PORT_DOWN | OFPPC_NO_FLOOD);
        c_opp->state = htonl(OFPPS_LINK_DOWN);
        c_opp->curr = htonl(OFPPF_X);
        c_opp->advertised = htonl(OFPPF_OC1 | OFPPF_OTU2 |OFPPF_OC768);
        c_opp->supported = htonl(OFPPF_OC3 | OFPPF_100GB | OFPPF_OTU1);
        c_opp->peer = htonl(OFPPF_X);

        c_opp->supp_sw_tdm_gran = 0;
        c_opp->supp_swtype = htons(OFPST_WAVE);
        if (i == 2 & !strcmp(pCore->SnmpConf.Host, "101"))
        {
            c_opp->peer_port_no = htons(104);
            c_opp->peer_datapath_id = tmppeer2;
            DBG("peer: %d", peer_port_no);
         }
         else if (i == 2 & !strcmp(pCore->SnmpConf.Host, "102"))
         {
            c_opp->peer_port_no = htons(102);
            c_opp->peer_datapath_id = tmppeer1;
            DBG("peer: %d", peer_port_no);
         }
         else if (i == 3 & !strcmp(pCore->SnmpConf.Host, "101"))
         {
            c_opp->peer_port_no = htons(103);
            c_opp->peer_datapath_id = tmppeer3;
            DBG("peer: %d", peer_port_no);
         }
        c_opp->num_bandwidth = htons(num_bandwidth);

        DBG("copp: %llu sizebwfeq:%d size:%d", c_opp->bandwidth, sizeof(&bandwidth_feq), sizeof(&c_opp->port_no));
        memcpy(c_opp->bandwidth, &bandwidth_feq, sizeof(&bandwidth_feq)) ;

        //DBG("sizebwfeq %d and sizecopp: %d and bandwidth:%" PRIu64 "\n", sizeof(bandwidth_feq), sizeof(&c_opp->bandwidth), c_opp->bandwidth);
        DBG("DPID is :%llu, host:%s, peeport:%lu", osf->datapath_id,pCore->SnmpConf.Host,c_opp->peer_port_no );

    /* prints all the details the Agent send in the features reply message */

      printf("\n ofp_phy_cport:port_no=<%d>"

           "\n ofp_phy_cport:hw_addr=<%d>"

           "\n ofp_phy_cport:namee=<%s>"

           "\n ofp_phy_cport:configg=<%d>"

           "\n ofp_phy_cport:supported=<%d>"

           "\n ofp_phy_cport:statee=<%d>"

           "\n ofp_phy_cport:currr=<%d>"

           "\n ofp_phy_cport:cpeer=<%d>"

           "\n ofp_phy_cport:advertisedd=<%d>"

           "\n ofp_phy_cport:sw_type=<%d>"

            "\n ofp_phy_cport:bandwidth=<%llu>"

           "\n ofp_phy_cport:peer_portno=<%lu>\n\n",

           c_opp->port_no,c_opp->hw_addr,c_opp->name,c_opp->config,c_opp->state,

           c_opp->curr,c_opp->advertised,c_opp->supported,c_opp->peer,

           c_opp->supp_swtype,c_opp->bandwidth, (c_opp->peer_port_no));

		   // TTT
           // memcpy(tmp +len + i * (sizeof(struct ofp_phy_cport)) , c_opp, sizeof (struct ofp_phy_cport));
           memcpy(tmp +len + i * (sizeof(struct ofp_phy_cport)+8) , c_opp, sizeof(struct ofp_phy_cport)+8);
           //opcp = (struct ofp_phy_cport *)osf->cports[i];
           //memcpy(opcp , c_opp, sizeof (struct ofp_phy_cport));

           i++;

    }//end of while for ports

	// TTT - add the size of ports variable to 
    //len += (5) * (sizeof(struct ofp_phy_cport));
    len += 5 * (sizeof(struct ofp_phy_cport) + 8);

    //osf->n_cports = (X_List_Size(pCore->Ports));

    osf->header.type = OFPT_FEATURES_REPLY;
    osf->header.length = htons(len);
    osf->header.version = OFP_VERSION;
    osf->header.xid = htonl(11);

    DBG("ncports: %x \n", osf->n_cports);

    _OpenFlow_Driver_Send_Raw(pCore->OpenFlowDrv, (char*)osf, len);
}

void
X_Core_Switching_Constraints(X_CorePtr pCore)
{
    X_ResourcePtr               pRes;
    X_ListNodePtr               pNode, pNode2;
    X_ModulePtr                 pMod, pMod2;
    X_ListPtr                   PathList;
    X_PathDataPtr               pPathData;
    unsigned int                NumOfPorts;
    unsigned int                ReachablePorts;
    X_ListPtr                   PortList;
    struct ofp_phy_cport*       pPort;
    struct ofp_phy_cport*       pPort0;
    char                        Raw[4096];
    int                         i;
    unsigned int                Len;
    unsigned int                Num;


    //struct ooe_header*                  Header;
    struct ooe_port_constraints*        PortConst;
    struct ooe_switch_constraints*      SwConst;

    pRes = pCore->Resources;

    fprintf(stderr, "X_Core_Switching_Constraints()...\r\n");

    PathList = X_List_New((X_ListMemFree_t)X_PathData_Free);
    PortList = X_List_New(NULL);

    NumOfPorts = 0;
    Num = 0;
    Len = 0;

   // Raw = Vector_New(sizeof(char), 4096, NULL);

    SwConst = (struct ooe_switch_constraints*)&Raw[0];

    SwConst->header.header.type = OFPT_VENDOR;
    SwConst->header.header.version = OFP_VERSION;
    SwConst->header.vendor = htonl(OOE_VENDOR_ID);
    SwConst->header.type = htonl(OOE_SWITCH_CONSTRAINTS_REPLY);

    SwConst->dpid = _htonll(pCore->SysIP);

    Len += sizeof(struct ooe_switch_constraints);

    X_List_ForEach((&pRes->Modules), pNode)
    {
        pMod = (X_ModulePtr)(pNode->Data);

        if (pMod->ModType != X_MOD_TYPE_TRANSPDR && pMod->ModType != X_MOD_TYPE_OL)
        {
            continue;
        }

        NumOfPorts++;
    }

    X_List_ForEach((&pRes->Modules), pNode)
    {
        pMod = (X_ModulePtr)(pNode->Data);

        if (pMod->ModType != X_MOD_TYPE_TRANSPDR && pMod->ModType != X_MOD_TYPE_OL)
        {
            continue;
        }

        pPort0 = X_Core_Find_Port(pCore, pMod);

        X_List_Clear(PathList);

        X_Resource_GetPathsTree(pRes, pMod, X_RES_DIR_OUT, 0, 0, PathList);

        X_List_Clear(PortList);

        X_List_RForEach(PathList, pNode2)
        {
            pPathData = (X_PathDataPtr)pNode2->Data;

            if (pPathData == NULL)
                continue;

            pMod2 = pPathData->Port->Module;

            if (pMod2 == pMod)
                continue;

            if (pMod2->ModType == X_MOD_TYPE_TRANSPDR || pMod2->ModType == X_MOD_TYPE_OL)
            {
                pPort = X_Core_Find_Port(pCore, pMod2);

                if (pPort != NULL)
                    X_List_Append(PortList, pPort);
            }
        }

        if (X_List_Size(PortList) > 0)
        {
            // generate constraints data

            PortConst = (struct ooe_port_constraints*)&Raw[Len];
            PortConst->port_no = pPort0->port_no;
            PortConst->num = htons(X_List_Size(PortList));

            i = 0;
            X_List_ForEach(PortList, pNode2)
            {
                pPort = (struct ofp_phy_cport*)pNode2->Data;
                PortConst->ports[i++] = (pPort->port_no);
            }

            /**
            if (i % 2)
            {
                PortConst->ports[i++] = 0;
            }
            **/

            Len += sizeof(struct ooe_port_constraints) + sizeof(uint16_t) * i;

            Num++;
        }
    }

    if (Num > 0)
    {
        SwConst->header.header.length = htons(Len);
        SwConst->num = htonl(Num);
        _OpenFlow_Driver_Send_Raw(pCore->OpenFlowDrv, (char*)SwConst, Len);
    }
}

void
X_Core_Handle_OpenFlow_Message(X_CorePtr pCore, struct ofp_header *pMsg)
{
    struct ofp_vendor_header *ofv;
    struct ofp_error_msg err;
    struct ooe_header* ooe_hdr;
    struct ofp_cflow_mod *cflow_mod;
    //uint16_t wildcards = htons(OFPCW_IN_WPORT|OFPCW_OUT_WPORT);
    uint16_t wildcards = htons(OFPCW_IN_PORT|OFPCW_OUT_PORT|OFPCW_IN_TPORT|OFPCW_OUT_TPORT);
    size_t freq = 0;
    struct ofp_wave_port in, out, *pwave;
    X_ListNodePtr pNode;
    char sendCmd[4096]= "1 0 339 5 1 ";
    char testCmd[4096]= "1 1 128 16 2 ";

    if(pMsg->type != 3) DBG("Received OF Msg type: %d and matching mod:%d",pMsg->type, OFPT_CFLOW_MOD);

    if (pMsg->type == OFPT_VENDOR)
    {
        ofv = (struct ofp_vendor_header*) (pMsg);

        if (ntohl(ofv->vendor) == OOE_VENDOR_ID)
        {
            ooe_hdr = (struct ooe_header*)(pMsg);

            if (ntohl(ooe_hdr->type) == OOE_SWITCH_CONSTRAINTS_REQUEST)
            {
                X_Core_Switching_Constraints(pCore);
                return;
            }
            else if (ntohl(ooe_hdr->type) == OOE_MGMT_INFO_REQUEST)
            {
                X_Core_Process_Mgmt_Info_Request(pCore);
                return;
            }
            else if (ntohl(ooe_hdr->type) == OOE_POWER_EQ_REQUEST)
            {
                X_Core_Process_Power_Equalization_Request(pCore, (struct ooe_power_eq*)(pMsg));
                return;
            }
        }

        err.header.type = OFPT_ERROR;
        err.header.xid = ofv->header.xid;
        err.header.version = OFP_VERSION;
        err.header.length = htons(sizeof(struct ofp_error_msg));

        err.type = htons(OFPET_BAD_REQUEST);
        err.code = htons(OFPBRC_BAD_VENDOR);
        _OpenFlow_Driver_Send_Raw(pCore->OpenFlowDrv,(char*)&err,sizeof(err));

        return;
    }


    if (pMsg->type == 255)
    {
        cflow_mod = (struct ofp_cflow_mod*)pMsg;

        if (cflow_mod == NULL)
            return;

        if (cflow_mod->command != htons(OFPFC_ADD) &&
                cflow_mod->command != htons(OFPFC_DELETE))
        {
            fprintf(stderr, "Unsupported command\n");
            return;
        }

        if(cflow_mod->connect.wildcards != wildcards)
        {
            return;
        }

        pwave = (struct ofp_wave_port*)&cflow_mod->connect.in_port;

        memcpy(&in, pwave, sizeof(in));

        pwave += 1;

        memcpy(&out, pwave, sizeof(out));

        printf("IN: %d OUT: %d\n", ntohs(in.wport), ntohs(out.wport));

        struct ofp_phy_cport *port_in = NULL, *port_out = NULL, *ofport;

        X_List_ForEach(pCore->Ports, pNode)
        {
            ofport = (struct ofp_phy_cport*)(pNode->Data);

            if (ofport->port_no == in.wport)
            {
                port_in = ofport;
            }
            else if (ofport->port_no == out.wport)
            {
                port_out = ofport;
            }

            if (port_in && port_out)
                break;
        }

        if (!port_in || !port_out)
        {
            //printf("Wrong port!?\n");
        }

/****************************FLEXI setup************************************************/

        int identifier =1, sign = 0, ret;
        //char IP[1024] = "155.245.64.10";
        char IP[1024] = "155.245.65.195";
        int portNum = 27015; uint64_t i,k;
        uint16_t n_value =0, m_value = 0;

       in.wavelength = ntohl(in.wavelength);
DBG("bandwidth now: %llu", in.wavelength);
        for(i=0;i<64;i++) {
           if(in.wavelength & (1 << i))
           DBG("set bit wavelength is, :%d", i);
        }

    DBG("bandwidth now: %llu", in.wavelength);
         for(i=10;i<26;i++)
         {
            uint64_t x = (uint64_t)((uint64_t)1 << (uint64_t)i);
                if((in.wavelength & (x)) != 0)
                {
                 if(((i-10) == 15) && (in.wavelength & (x)) )
                 {
                    sign = 1;
                    n_value &= ~(1 << (i-10));
                 }
                 else
                 n_value |= (1 << (i-10));
                }
            }
        i =0;

         for(i=26;i<42;i++)
         {
            uint64_t x = (uint64_t)((uint64_t)1 << (uint64_t)i);
                if((in.wavelength & (x)) != 0)
                {
                    m_value |= 1 << (i-26);
                }
         }
        //strcpy(sendCmd," ");
        if (sign == 1) sign = 0;
        else if (sign == 0) sign =1;

        ret = sprintf(sendCmd," %d %d %d %d %hu", identifier, sign,  \
                                                    (n_value), (m_value), ntohs(out.wport) );
        if (ret == 0)
         {
             DBG("could not read the flexi variables");
         }

        DBG("sending string: %s of size %d", sendCmd, ret);
        sleep(2);
        int ret1=1;

         ret1 = sendCommand (IP, portNum, sendCmd);
         portNum = 27016; sleep(2);
          int ret2 = sendCommand (IP, portNum, sendCmd);


        if (ret1 > 0 && ret2 > 0)
        {
           X_Core_cport_status(pCore, OFPPR_ADD);
        }
        else
        {
            DBG("Waveshaper connection Failed");
            //return;
        }



/****************************FLEXI setup************************************************/

    }


}

/**************send cport status*************/

void
X_Core_cport_status(X_CorePtr pCore, uint8_t reason)
{
    struct ofp_cport_status *cps;
    struct ofp_phy_cport *opcp, *c_opp;
    size_t len = sizeof(struct ofp_cport_status);
    X_ListNodePtr pNode;
    char tmp[4096];
    int i;

    assert(pCore != NULL);

    if(pCore == NULL) return;

    cps = (struct ofp_cport_status*)tmp;
    memset(tmp, 0, sizeof(tmp));

    cps->reason = reason;

    i=0;
    X_List_ForEach(pCore->Ports, pNode)
        {
            opcp = (struct ofp_phy_cport*)(pNode->Data);
            memcpy(tmp + len +i  *(sizeof(struct ofp_phy_cport)), opcp, sizeof(struct ofp_phy_cport));
            i++;
        }

    cps->header.type = 0xfe; //OFPT_CPORT_STATUS;
    cps->header.length = htons(len);
    cps->header.version = OFP_VERSION;
    cps->header.xid = htonl(11);

    _OpenFlow_Driver_Send_Raw(pCore->OpenFlowDrv, (char*)cps, len);

}

/**************sent cport status*************/

struct ofp_phy_cport*
X_Core_Find_Port(X_CorePtr pCore, X_ModulePtr pMod)
{
    X_ListNodePtr               pNode;
    struct ofp_phy_cport*       pPort;

    if (pCore == NULL || pMod == NULL)
        return NULL;

    X_List_ForEach(pCore->Ports, pNode)
    {
        pPort = (struct ofp_phy_cport*)(pNode->Data);

        if (pMod->Index == *(unsigned int*)pPort->hw_addr)
            return pPort;
    }
    return NULL;
}


void
X_Core_Process_Mgmt_Info_Request(X_CorePtr pCore)
{
    char                        Msg[256];
    struct ooe_mgmt_info*       MgmtInfo;
    char*                       pAddr;
    char*                       pIpStr;
    char*                       pPortStr;
    unsigned int                addr;

    assert(pCore != NULL);

    if (pCore == NULL)
        return;

    pAddr = strdup(pCore->SnmpConf.Host);

    pIpStr = pCore->SnmpConf.Host;
    pPortStr = strchr(pCore->SnmpConf.Host, ':');

    if (pPortStr != NULL)
    {
        *pPortStr = 0;
        pPortStr++;
    }

    fprintf(stderr, "PORT = %s\r\n", pPortStr);

    inet_aton(pIpStr, (struct in_addr*)&addr);

    MgmtInfo = (struct ooe_mgmt_info*)Msg;

    memset(Msg, 0, sizeof(Msg));

    MgmtInfo->header.header.type = OFPT_VENDOR;
    MgmtInfo->header.header.xid = 0;
    MgmtInfo->header.header.version = OFP_VERSION;


    MgmtInfo->header.vendor = htonl(OOE_VENDOR_ID);
    MgmtInfo->header.type = htonl(OOE_MGMT_INFO_REPLY);

    MgmtInfo->dpid = _htonll(pCore->SysIP);

    MgmtInfo->snmp_ip = htonl(addr);
    MgmtInfo->snmp_port = (pPortStr != NULL) ? htons(atoi(pPortStr)) : htons(161);

    MgmtInfo->num = strlen(pCore->SnmpConf.Community);

    memcpy(MgmtInfo->snmp_community, pCore->SnmpConf.Community,MgmtInfo->num);

    MgmtInfo->header.header.length = htons(sizeof(struct ooe_mgmt_info) + MgmtInfo->num);

    _OpenFlow_Driver_Send_Raw(pCore->OpenFlowDrv, (char*)Msg, ntohs(MgmtInfo->header.header.length));
}


/** Process OOE_POWER_EQ_REQUEST message **/
void
X_Core_Process_Power_Equalization_Request(X_CorePtr pCore, struct ooe_power_eq *eq)
{
    uint16_t in_port, out_port;
    int freq;

    assert(pCore != NULL);
    assert(eq != NULL);

    if (pCore == NULL || eq == NULL)
        return;

    in_port = ntohs(eq->in_port);
    out_port = ntohs(eq->out_port);

    if (in_port == 0 || out_port == 0)
        return;

    freq = ntohl(eq->freq);

    if (freq == 0)
        return;

    fprintf(stderr, "Equalization request: %d -> %d @ %d\r\n",
            in_port, out_port, freq);

    /// TODO: find the ports and trigger the equlization

}

/******************************************************************************/

void
SNMP_Channel_Callback(SNMP_ChannelPtr pCh, int type, void *data)
{
    X_CorePtr pCore;
    X_ListNodePtr pNode;
    X_ResourcePtr pRsrc;

    pCore = pCh->Context;

    if (pCore == NULL) return;

    pRsrc = pCore->Resources;

     X_Resource_Lock(pRsrc);

    if (type == SNMP_CHANNEL_CB_READY)
    {
        X_List_ForEach(pCh->Inventory.ShelfList, pNode)
        {
            X_LoadEntityShelf(pRsrc,(EntityShelf*) pNode->Data,X_RES_OPER_CREATE);
        }

        X_List_ForEach(pCh->Inventory.ModuleList, pNode)
        {
            X_LoadEntityModule(pRsrc,(EntityModule*) pNode->Data,X_RES_OPER_CREATE);
        }

        X_List_ForEach(pCh->Inventory.ExtObjList, pNode)
        {
            X_LoadEntityModule(pRsrc,(EntityModule*)pNode->Data, X_RES_OPER_CREATE);
        }

        X_List_ForEach(pCh->Inventory.OLList, pNode)
        {
            X_LoadEntityOL(pRsrc,(EntityOL*) pNode->Data,X_RES_OPER_CREATE);
        }

        X_List_ForEach(pCh->Inventory.ConnectionList, pNode)
        {
            X_LoadEntityConnection(pRsrc,(EntityConnection*) pNode->Data,1);
        }

        // Configuration was loaded - now prepare the list of OpenFlow enabled ports

        pCore->SysIP = pCh->Inventory.NE.SysIP;

        X_List_Clear(pCore->Ports);

        X_Core_Generate_OpenFlow_Ports(pCore);

        //OF_Driver_Connect(pCore->OpenFlowChannel,pCore->OpenFlowAddr,0);

        OpenFlow_Driver_Start(pCore->OpenFlowDrv);

        OpenFlow_Driver_Connect(pCore->OpenFlowDrv,pCore->OpenFlowAddr);

    }
    else if (type == SNMP_CHANNEL_CB_CONN_DESTROYED)
    {
        X_LoadEntityConnection(pRsrc,(EntityConnection*) data,X_RES_OPER_DESTROY);
    }
    else if (type == SNMP_CHANNEL_CB_CONN_CREATED)
    {
        X_LoadEntityConnection(pRsrc,(EntityConnection*) data, X_RES_OPER_CREATE);
    }
    else if (type == SNMP_CHANNEL_CB_MOD_CREATED)
    {
        X_LoadEntityModule(pRsrc,(EntityModule*) data, X_RES_OPER_CREATE);
    }
    else if (type == SNMP_CHANNEL_CB_MOD_DESTROYED)
    {
        X_LoadEntityModule(pRsrc,(EntityModule*) data,X_RES_OPER_DESTROY);
    }
    else if (type == SNMP_CHANNEL_CB_OL_CREATED)
    {
        X_LoadEntityOL(pRsrc,(EntityOL*) data,X_RES_OPER_CREATE);
    }
    else if (type == SNMP_CHANNEL_CB_OL_DESTROYED)
    {
        X_LoadEntityOL(pRsrc,(EntityOL*) data,X_RES_OPER_DESTROY);
    }

    X_Resource_Unlock(pRsrc);
}

int
OpenFlow_Driver_Callback(OpenFlow_DriverPtr pDrv, OpenFlow_Driver_Callback_Type type, struct ofp_header *pMsg)
{
    X_CorePtr pCore;

    if (pDrv == NULL)
        return 0;

    pCore = (X_CorePtr)pDrv->UserData;

    switch (type)
    {
    case OPENFLOW_DRV_CB_FEATURES_REQ:
        X_Core_Handle_Features_Request(pCore);
        break;

    case OPENFLOW_DRV_CB_MESSAGE:
        X_Core_Handle_OpenFlow_Message(pCore, pMsg);
        break;

    case OPENFLOW_DRV_CB_DISCONNECTED:
        break;

    default:
        break;
    }

    return 0;
}
