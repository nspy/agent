
#include <SnmpAccess/SnmpAccess.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <unistd.h>
#include <signal.h>

sig_atomic_t            RunFlag = 0;

SNMP_Channel_Conf       SNMPConf;
SNMP_ChannelPtr         SNMPChan = NULL;
SNMP_TrapHandlerPtr     SNMPTrap = NULL;
X_ResourcePtr           XRes = NULL;

X_ListPtr               Ports;

char*                   ConfigFile = NULL;
char                    TrapAddr[64];

int
ParseCmdLine(int argc, char *argv[])
{
    int opt;

    while(-1 != (opt = getopt(argc, argv, "c:h")))
    {
        switch (opt)
        {
        case 'c':
            ConfigFile = optarg;
            break;

        case 'h':
            return (-1);

        default:
            break;
        }
    }

    if (ConfigFile != NULL)
    {
        return (0);
    }

    return -1;
}

int
LoadConfig(const char *path)
{
    xmlNodePtr root, node;
    xmlDocPtr doc;
    xmlChar *s;

    // init default values

    SNMP_Channel_Conf_Init(&SNMPConf);

    strcpy(TrapAddr, "0.0.0.0:1620");

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

    if(xmlStrcasecmp((xmlChar*)"config", root->name))
    {
        xmlFreeDoc(doc);
        return (-1);
    }

    for(node = root->children; node; node = node->next)
    {
        if(node->type != XML_ELEMENT_NODE)
        {
            continue;
        }

        if(!xmlStrcasecmp((xmlChar*)"host", node->name))
        {
            s = xmlNodeGetContent(node);
            SNMP_Channel_Conf_Set_Host(&SNMPConf, (char*)s);
            xmlFree(s);
        }
        else if(!xmlStrcasecmp((xmlChar*)"community", node->name))
        {
            s = xmlNodeGetContent(node);
            SNMP_Channel_Conf_Set_Community(&SNMPConf, (char*)s);
            xmlFree(s);
        }
        else if(!xmlStrcasecmp((xmlChar*)"trap", node->name))
        {
            s = xmlNodeGetContent(node);
            strncpy(TrapAddr, (char*)s, sizeof(TrapAddr));
            xmlFree(s);
        }
        else if (!xmlStrcasecmp((xmlChar*)"poll", node->name))
        {
            s = xmlNodeGetContent(node);
            SNMPConf.PollTimeout = atoi((char*)s);
            xmlFree(s);
        }
        else if (!xmlStrcasecmp((xmlChar*)"cpreload", node->name))
        {
            s = xmlNodeGetContent(node);
            SNMPConf.CPReloadTimeout = atoi((char*)s);
            xmlFree(s);
        }
    }

    xmlFreeDoc(doc);

    return (0);
}

void SNMP_Channel_Callback(SNMP_ChannelPtr pCh, int type, void *data)
{
    X_ListNodePtr pNode;
    X_ModulePtr pMod;

    X_Resource_Lock(XRes);

    if (type == SNMP_CHANNEL_CB_READY)
    {
        X_List_ForEach(pCh->Inventory.ShelfList, pNode)
        {
            X_LoadEntityShelf(XRes,(EntityShelf*)pNode->Data, X_RES_OPER_CREATE);
        }

        X_List_ForEach(pCh->Inventory.ModuleList, pNode)
        {
            X_LoadEntityModule(XRes,(EntityModule*)pNode->Data, X_RES_OPER_CREATE);
        }

        X_List_ForEach(pCh->Inventory.ExtObjList, pNode)
        {
            X_LoadEntityModule(XRes,(EntityModule*)pNode->Data, X_RES_OPER_CREATE);
        }

        X_List_ForEach(pCh->Inventory.OLList, pNode)
        {
            X_LoadEntityOL(XRes,(EntityOL*)pNode->Data, X_RES_OPER_CREATE);
        }

        X_List_ForEach(pCh->Inventory.ConnectionList, pNode)
        {
            X_LoadEntityConnection(XRes,(EntityConnection*)pNode->Data, 1);
        }
       // X_Resource_Dump(XRes);
    }
    else if (type == SNMP_CHANNEL_CB_CONN_DESTROYED)
    {
        X_LoadEntityConnection(XRes,(EntityConnection*)data, 0);
    }
    else if (type == SNMP_CHANNEL_CB_CONN_CREATED)
    {
        X_LoadEntityConnection(XRes,(EntityConnection*)data, 1);
    }
    else if (type == SNMP_CHANNEL_CB_MOD_CREATED)
    {
        X_LoadEntityModule(XRes, (EntityModule*)data, 1);
    }
    else if (type == SNMP_CHANNEL_CB_MOD_DESTROYED)
    {
        X_LoadEntityModule(XRes, (EntityModule*)data, 0);
    }
    else if (type == SNMP_CHANNEL_CB_OL_CREATED)
    {
        pMod = X_LoadEntityOL(XRes, (EntityOL*)data, 1);

    }
    else if (type == SNMP_CHANNEL_CB_OL_DESTROYED)
    {
        X_LoadEntityOL(XRes, (EntityOL*)data, 0);
    }

    X_Resource_Unlock(XRes);
}

#if 0
int OpenFlow_Driver_Callback(OpenFlow_DriverPtr pDrv,
        OpenFlow_Driver_Callback_Type Type,
        struct ofp_header *hdr)
{
    X_ListPtr pList;
    X_ListNodePtr pNode;
    //struct ofp_header *hdr;
    struct ofp_switch_features *swf;
    struct ofp_phy_port *p;
    struct ofp_cflow_mod *cflow_mod;
    char tmp[4096];
    size_t len = sizeof(struct ofp_switch_features);
    int i;
    uint16_t wildcards = htons(OFPCW_IN_WPORT|OFPCW_OUT_WPORT);
    size_t freq = 0;

    struct ofp_wave_port in, out, *pwave;

    switch (Type)
    {
    case OPENFLOW_DRV_CB_FEATURES_REQ:
        memset(tmp, 0, sizeof(tmp));

       // pList = X_List_New(free);

        swf = (struct ofp_switch_features*)tmp;

        //X_Generate_OpenFlow_Ports(XRes, pList);
        X_List_Clear(Ports);

        X_Generate_OpenFlow_Ports(XRes, Ports);

        swf->datapath_id = htobe64(SNMPChan->Inventory.NE.SysIP);

        i = 0;
        X_List_ForEach(Ports, pNode)
        {
            p = (struct ofp_phy_port*)pNode->Data;

            memcpy(tmp + len + i * (sizeof(struct ofp_phy_port)), p,
                    sizeof(struct ofp_phy_port));
            i++;
        }

        len += (i) * (sizeof(struct ofp_phy_port));

        swf->header.type = OFPT_FEATURES_REPLY;
        swf->header.length = htons(len);
        swf->header.version = OFP_VERSION;
        swf->header.xid = htonl(1);

        _OpenFlow_Driver_Send_Raw(pDrv, tmp, len);
        break;

    case OPENFLOW_DRV_CB_MESSAGE:

        cflow_mod = (struct ofp_cflow_mod*)hdr;

        if (cflow_mod == NULL)
            return 0;

        if (cflow_mod->command != htons(OFPFC_ADD) &&
                cflow_mod->command != htons(OFPFC_DELETE)
                )
        {
            X_LOGWARN(1, "Unsupported command");
            return 0;
        }

        if(cflow_mod->connect.wildcards != wildcards)
        {
            X_LOGWARN(1, "Unsupported wildcards");
            return 0;
        }

        pwave = (struct ofp_wave_port*)&cflow_mod->connect.in_port;

        memcpy(&in, pwave, sizeof(in));

        pwave += 1;

        memcpy(&out, pwave, sizeof(out));

        printf("IN: %d OUT: %d\n", ntohs(in.wport), ntohs(out.wport));

        struct ofp_phy_port *port_in = NULL, *port_out = NULL, *ofport;

        X_List_ForEach(Ports, pNode)
        {
            ofport = (struct ofp_phy_port*)(pNode->Data);

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
            printf("Wrong port!?\n");
            return 0;
        }

        freq = ntohl(in.wavelength); // HACK!


        X_Addr inaddr, outaddr;

        X_Addr_Parse(&inaddr, (char*)port_in->name);
        X_Addr_Parse(&outaddr, (char*)port_out->name);

        X_ModulePtr pMod = X_Resource_FindModule(XRes, &inaddr);
        X_ModulePtr pMod2 = X_Resource_FindModule(XRes, &outaddr);

        if (!pMod || !pMod2)
        {
            printf("No matching module found!\n");
            return 0;
        }


        pList = X_List_New((X_ListMemFree_t) X_PathData_Free);

        X_Resource_Find_Path(XRes,X_RES_DIR_IN,freq,pMod,pMod2,pList);
        if (cflow_mod->command == htons(OFPFC_ADD))
        {
            X_Allocate_Resource(XRes,pList,freq);
        }
        else
        {
            X_Deallocate_Resources(XRes,pList,freq);
        }
        X_List_Clear(pList);

        X_Resource_Find_Path(XRes,X_RES_DIR_OUT,freq,pMod,pMod2,pList);
        if (cflow_mod->command == htons(OFPFC_ADD))
        {
            X_Allocate_Resource(XRes,pList,freq);
        }
        else
        {
            X_Deallocate_Resources(XRes,pList,freq);
        }
        X_List_Clear(pList);

        X_List_Free(pList);


        break;

    default:
        break;
    }

    return 0;
}
#endif

void
ResourceCmd(char *s)
{
    int ret, len;
    char token[512];
    X_Addr addr;
    X_Addr addr2;
    X_ModulePtr pMod, pMod2;
    X_ListPtr pList;
    int freq = 0;

    ret = sscanf(s, "%s%n", token, &len);

    if (ret != 1)
    {
        return;
    }

    s += len;

    if (!strcmp(token, "dump"))
    {
        X_Resource_Lock(XRes);
        X_Resource_Dump(XRes);
        X_Resource_Unlock(XRes);
        return;
    }

    if (!strcmp(token, "update"))
    {
        X_Resource_Lock(XRes);
        X_Resource_Update(XRes);
        X_Resource_Unlock(XRes);
        return;
    }

#if 0
    if (!strcmp(token, "ofdump"))
    {
        pList = X_List_New(free);
        X_Generate_OpenFlow_Ports(XRes, pList);
        X_List_Free(pList);
        return;
    }
#endif

    if (!strcmp(token, "path"))
    {
        ret = sscanf(s, "%s%n", token, &len);
        if (ret != 1) return;
        s += len;

        ret = sscanf(s, "%d%n", &freq, &len);
        if (ret != 1) { freq = X_RES_FREQ_ANY; }

        X_Resource_Lock(XRes);

        pList = X_List_New((X_ListMemFree_t)(X_PathData_Free));

        if (0 == X_Addr_Parse(&addr, token))
        {
            pMod = X_Resource_FindModule(XRes, &addr);

            if (pMod != NULL)
            {
                X_Resource_GetPathsTree(XRes,pMod, X_RES_DIR_IN, freq, 0, pList);
                X_Resource_PrunePaths(pList, pMod);
                X_List_Clear(pList);

                X_Resource_GetPathsTree(XRes,pMod, X_RES_DIR_OUT, freq, 0, pList);
                X_Resource_PrunePaths(pList, pMod);
                X_List_Clear(pList);
            }
        }

        X_Resource_Unlock(XRes);

        X_List_Free(pList);

        return;
    }

    if (!strcmp(token, "path2"))
    {
        ret = sscanf(s, "%s%n", token, &len);
        if (ret != 1) return;
        s += len;

        X_Addr_Parse(&addr, token);

        ret = sscanf(s, "%s%n", token, &len);
        if (ret != 1) return;
        s += len;

        ret = sscanf(s, "%d%n", &freq, &len);
        if (ret != 1) return;
        s += len;

        X_Addr_Parse(&addr2, token);

        X_Resource_Lock(XRes);

        pList = X_List_New((X_ListMemFree_t)(X_PathData_Free));

        pMod = X_Resource_FindModule(XRes, &addr);
        pMod2 = X_Resource_FindModule(XRes, &addr2);

        if (pMod != NULL && pMod2 != NULL)
        {
            X_Resource_Find_Path(XRes,X_RES_DIR_IN, freq, pMod, pMod2, pList);
            X_Allocate_Resource(XRes, pList, freq);

            X_List_Clear(pList);
            X_Resource_Find_Path(XRes,X_RES_DIR_OUT, freq, pMod, pMod2, pList);
            X_Allocate_Resource(XRes, pList, freq);
            X_List_Clear(pList);

        }

        X_Resource_Unlock(XRes);

        X_List_Free(pList);

        return;
    }

    if (!strcmp(token, "path3"))
      {
          ret = sscanf(s, "%s%n", token, &len);
          if (ret != 1) return;
          s += len;

          X_Addr_Parse(&addr, token);

          ret = sscanf(s, "%s%n", token, &len);
          if (ret != 1) return;
          s += len;

          ret = sscanf(s, "%d%n", &freq, &len);
                  if (ret != 1) return;
                  s += len;

          X_Addr_Parse(&addr2, token);

          X_Resource_Lock(XRes);

          pList = X_List_New((X_ListMemFree_t)(X_PathData_Free));

          pMod = X_Resource_FindModule(XRes, &addr);
          pMod2 = X_Resource_FindModule(XRes, &addr2);

          if (pMod != NULL && pMod2 != NULL)
          {
              X_Resource_Find_Path(XRes,X_RES_DIR_IN, freq, pMod, pMod2, pList);
              X_Deallocate_Resources(XRes, pList, freq);
              X_List_Clear(pList);

              X_Resource_Find_Path(XRes,X_RES_DIR_OUT, freq, pMod, pMod2, pList);
              X_Deallocate_Resources(XRes, pList, freq);
              X_List_Clear(pList);
          }

          X_Resource_Unlock(XRes);

          X_List_Free(pList);

          return;
      }


    if (!strcmp(token, "enable"))
    {
        ret = sscanf(s, "%s%n", token, &len);
        if (ret != 1) return;
        s += len;

        X_Addr_Parse(&addr, token);
        ret = sscanf(s, "%s%n", token, &len);
        if (ret != 1) return;
        s += len;

        ret = sscanf(s, "%d%n", &freq, &len);
        if (ret != 1) return;
        s += len;

        X_Addr_Parse(&addr2, token);

        X_Resource_Lock(XRes);

        pList = X_List_New((X_ListMemFree_t)(X_PathData_Free));

        pMod = X_Resource_FindModule(XRes, &addr);
        pMod2 = X_Resource_FindModule(XRes, &addr2);

        if (pMod != NULL && pMod2 != NULL)
        {
            X_Resource_Find_Path(XRes,X_RES_DIR_IN, freq, pMod, pMod2, pList);
            X_Update_Resources(XRes, pList, freq, 1);
            X_List_Clear(pList);

            X_Resource_Find_Path(XRes,X_RES_DIR_OUT, freq, pMod, pMod2, pList);
            X_Update_Resources(XRes, pList, freq, 1);
            X_List_Clear(pList);
        }

        X_Resource_Unlock(XRes);
        X_List_Free(pList);
        return;
    }

    if (!strcmp(token, "disable"))
       {
           ret = sscanf(s, "%s%n", token, &len);
           if (ret != 1) return;
           s += len;

           X_Addr_Parse(&addr, token);
           ret = sscanf(s, "%s%n", token, &len);
           if (ret != 1) return;
           s += len;

           ret = sscanf(s, "%d%n", &freq, &len);
           if (ret != 1) return;
           s += len;

           X_Addr_Parse(&addr2, token);

           X_Resource_Lock(XRes);

           pList = X_List_New((X_ListMemFree_t)(X_PathData_Free));

           pMod = X_Resource_FindModule(XRes, &addr);
           pMod2 = X_Resource_FindModule(XRes, &addr2);

           if (pMod != NULL && pMod2 != NULL)
           {
               X_Resource_Find_Path(XRes,X_RES_DIR_IN, freq, pMod, pMod2, pList);
               X_Update_Resources(XRes, pList, freq, 0);
               X_List_Clear(pList);

               X_Resource_Find_Path(XRes,X_RES_DIR_OUT, freq, pMod, pMod2, pList);
               X_Update_Resources(XRes, pList, freq, 0);
               X_List_Clear(pList);
           }

           X_Resource_Unlock(XRes);
           X_List_Free(pList);
           return;
       }

    if (!strcmp(token, "eq"))
    {
        ret = sscanf(s, "%s%n", token, &len);
        if (ret != 1) return;
        s += len;

        X_Addr_Parse(&addr, token);
        ret = sscanf(s, "%s%n", token, &len);
        if (ret != 1) return;
        s += len;

        ret = sscanf(s, "%d%n", &freq, &len);
        if (ret != 1) return;
        s += len;

        X_Addr_Parse(&addr2, token);
        X_Resource_Lock(XRes);

        pList = X_List_New((X_ListMemFree_t)(X_PathData_Free));

        pMod = X_Resource_FindModule(XRes, &addr);
        pMod2 = X_Resource_FindModule(XRes, &addr2);

        if (pMod != NULL && pMod2 != NULL)
        {
            X_Resource_Find_Path(XRes,X_RES_DIR_IN, freq, pMod, pMod2, pList);
            X_Power_Equalize(XRes, pList, freq);
            X_List_Clear(pList);


            X_Resource_Find_Path(XRes,X_RES_DIR_OUT, freq, pMod, pMod2, pList);
            X_Power_Equalize(XRes, pList, freq);
            X_List_Clear(pList);
        }

        X_Resource_Unlock(XRes);
        X_List_Free(pList);
        return;
    }

    if (!strcmp(token, "verify"))
      {
          ret = sscanf(s, "%s%n", token, &len);
          if (ret != 1) return;
          s += len;

          X_Addr_Parse(&addr, token);
          ret = sscanf(s, "%s%n", token, &len);
          if (ret != 1) return;
          s += len;

          ret = sscanf(s, "%d%n", &freq, &len);
          if (ret != 1) return;
          s += len;

          X_Addr_Parse(&addr2, token);
          X_Resource_Lock(XRes);

          pList = X_List_New((X_ListMemFree_t)(X_PathData_Free));

          pMod = X_Resource_FindModule(XRes, &addr);
          pMod2 = X_Resource_FindModule(XRes, &addr2);

          if (pMod != NULL && pMod2 != NULL)
          {
              X_Resource_Find_Path(XRes,X_RES_DIR_IN, freq, pMod, pMod2, pList);
              X_Verify_Allocation(XRes, pList, freq);
              X_List_Clear(pList);

              X_Resource_Find_Path(XRes,X_RES_DIR_OUT, freq, pMod, pMod2, pList);
              X_Verify_Allocation(XRes, pList, freq);
              X_List_Clear(pList);
          }

          X_Resource_Unlock(XRes);
          X_List_Free(pList);
          return;
      }
}

void
TunnelCmd(char *s)
{
    SNMP_ErrCode Err;
    EntityTunnel EntTunnel;
    EntityTunnelResult EntResult;
    char DstIP[32];
    int ret;
    size_t len;
    //unsigned int Admin = AdminState_IS;
    ErrCode errCode;
    int n = 0;

    EntityTunnel_Init(&EntTunnel);

    ret = sscanf(s, "%31s%n", EntTunnel.Id, &len);

    if (ret != 1)
    {
        return;
    }

    s += len;

    ret = sscanf(s, "%31s%n", DstIP, &len);

    if (ret != 1)
    {
        return;
    }

    s += len;

    ret = sscanf(s, "%x %x", &EntTunnel.FromAid, &EntTunnel.ToAid);

    if (ret != 2)
    {
        return;
    }

    EntTunnel.ToIP = inet_addr(DstIP);

    printf(">> Creating tunnel %s\n", EntTunnel.Id);

    errCode = SNMP_CreateTunnel(SNMPChan->Session, &Err, &EntTunnel);

    if (errCode != ERRCODE_NOERROR ||
            Err.Status != STAT_SUCCESS ||
            Err.Err != SNMP_ERR_NOERROR)
    {
        printf(">> Tunnel creation failed\n");
        return;
    }

    errCode = SNMP_SetTunnelAdmin(SNMPChan->Session, &Err, &EntTunnel, AdminState_IS);

    if (errCode != ERRCODE_NOERROR ||
            Err.Status != STAT_SUCCESS || Err.Err != SNMP_ERR_NOERROR)
    {
        printf(">> Tunnel admin state set failed\n");
        return;
    }

    EntResult.Index = EntTunnel.Generic.Index;

    while ( n < 10 )
    {
        errCode = SNMP_GetTunnelResult(SNMPChan->Session, &Err, &EntResult);

        if (errCode != ERRCODE_NOERROR ||
                Err.Status != STAT_SUCCESS || Err.Err != SNMP_ERR_NOERROR)
        {
            printf(">> Error occured while checking tunnel state\n");
            return;
        }

        printf(">> PathComputationState %d ErrCat = %d SecStates = %08x\n",
                EntResult.ComputationState, EntResult.ErrCategory, EntResult.SecondayStates);

        if (EntResult.ComputationState == PathComputationState_INPROGS)
        {
            printf(">> Path Computation in-progress\n");
        }
        else
        {
            printf(">> Path Computation state %d\n", EntResult.ComputationState);
        }

        X_Sleep(1000);

        ++n;
    }
}

void
SnmpCmd(char *s)
{
    SNMP_ErrCode err;
    ErrCode errc;
    char token[32];
    int ret, len;
    long idx;
    X_ListPtr pList;
    X_ListNodePtr pNode;
//    Entity *pEnt;
    AuxEntityData *pData;

    ret = sscanf(s, "%30s%n", token, &len);

    if (ret != 1)
    {
        return;
    }

    s += len;

    if (!strcmp(token, "dump"))
    {
        SNMP_Channel_Inventory_Dump(&SNMPChan->Inventory, stderr);
        SNMP_Channel_InventoryCP_Dump(&SNMPChan->InventoryCP);
    }
    else if (!strcmp(token, "contained"))
    {
        if (1 != sscanf(s, "%ld%n", &idx, &len))
        {
            return;
        }

        pList = X_List_New(free);

        errc = SNMP_Contains(SNMPChan->Session, &err, idx, pList);

        printf(">> Contained in %ld: %d\r\n", idx, X_List_Size(pList));

        X_List_ForEach(pList, pNode)
        {
            pData = (AuxEntityData*)pNode->Data;

            printf(">> Index = %08x Class: %d AID=%s\r\n",
                    (int)pData->Index, pData->Class, pData->Aid);
        }


        X_List_Free(pList);
    }
}

void
DbgCmd(char *s)
{
    char token[32];
    int ret, len, idx;
    EntityCNX cnx;
    SNMP_ErrCode err;
    X_ListPtr pList;
    X_ListNodePtr pNode;
    char temp[64];

    EntityCNX_Init(&cnx);

    ret = sscanf(s, "%30s%n", token, &len);

    if (ret != 1)
    {
        return;
    }

    s += len;

    if (!strcmp(token, "cnx"))
    {
        ret = sscanf(s, "%d%n", &idx, &len);

        if (ret != 1)
        {
            return;
        }

        pList = X_List_New(free);

        Entity_Set_Index(&cnx, idx);

        SNMP_GetSignalledPath(SNMPChan->Session, &err, &cnx, pList);

        X_List_ForEach(pList, pNode)
        {
            Entity_Dump(pNode->Data, temp, sizeof(temp));
            printf("\t%s\n",temp);
        }


        X_List_Free(pList);

        return;
    }

}

int main(int argc, char *argv[])
{
    char cmd[512];
    char token[512];
    int len;
    char *p;
    int ret;

    X_Log_Open(X_LOG_MASK_STDERR | X_LOG_MASK_FILE);
    X_Log_SetFile("log.txt");
    X_Log_SetLevel(X_LOG_CHAN_ID, X_LOG_LEVEL_INFO); // enable logging for channel module
    X_Log_SetLevel(X_LOG_INTF_ID, X_LOG_LEVEL_INFO); // enable logging for interface
    X_Log_SetLevel(X_LOG_RES_ID, X_LOG_LEVEL_INFO); // enable logging for resources

    /** Initialize the SNMP lib. **/
    SNMP_Lib_Init();

    if (-1 == ParseCmdLine(argc, argv))
    {
        printf("Usage: %s -c <config>\n", argv[0]);
        SNMP_Lib_Exit();
        return (1);
    }

    /** Load configuration from the XML file **/
    if (-1 == LoadConfig(ConfigFile))
    {
        printf("Failed to load config file: %s\n", ConfigFile);
        SNMP_Lib_Exit();
        return 1;
    }

    printf("Host = %s Community = %s\n", SNMPConf.Host, SNMPConf.Community);

    #if 0
    OpenFlowDrv = OpenFlow_Driver_New();

    OpenFlow_Driver_Set_Callback(OpenFlowDrv, OPENFLOW_DRV_CB_DISCONNECTED, OpenFlow_Driver_Callback);
    OpenFlow_Driver_Set_Callback(OpenFlowDrv, OPENFLOW_DRV_CB_FEATURES_REQ, OpenFlow_Driver_Callback);
    OpenFlow_Driver_Set_Callback(OpenFlowDrv, OPENFLOW_DRV_CB_MESSAGE, OpenFlow_Driver_Callback);

    OpenFlow_Driver_Start(OpenFlowDrv);
#endif

    Ports = X_List_New((free));
    /**/

    /** Create SNMP_Channel instance **/
    SNMPChan = SNMP_Channel_New(&SNMPConf);

    /** Create trap receiver instance **/
    SNMPTrap = SNMP_TrapHandler_New(TrapAddr);

    /** Create resource database instance **/
    XRes = X_Resource_New();

    // TODO: fix it ??
    //XRes->SNMPChan = SNMPChan;
    X_Resource_Set_SNMP_Channel(XRes, SNMPChan);

    /** Set callback function for receiving notifications from SNMP_Channel **/
    SNMP_Channel_SetCallback(SNMPChan, SNMP_CHANNEL_CB_READY, SNMP_Channel_Callback);
    SNMP_Channel_SetCallback(SNMPChan, SNMP_CHANNEL_CB_CONN_CREATED, SNMP_Channel_Callback);
    SNMP_Channel_SetCallback(SNMPChan, SNMP_CHANNEL_CB_CONN_DESTROYED, SNMP_Channel_Callback);
    SNMP_Channel_SetCallback(SNMPChan, SNMP_CHANNEL_CB_MOD_CREATED, SNMP_Channel_Callback);
    SNMP_Channel_SetCallback(SNMPChan, SNMP_CHANNEL_CB_MOD_DESTROYED, SNMP_Channel_Callback);
    SNMP_Channel_SetCallback(SNMPChan, SNMP_CHANNEL_CB_OL_CREATED, SNMP_Channel_Callback);
    SNMP_Channel_SetCallback(SNMPChan, SNMP_CHANNEL_CB_OL_DESTROYED, SNMP_Channel_Callback);

    /** Add channel to receive traps **/
    SNMP_TrapHandler_Add(SNMPTrap, SNMPChan);

    /** Initialize processing **/
    SNMP_Channel_Init(SNMPChan);

    /** Start thread **/
    SNMP_Channel_Start(SNMPChan);

    /** Start thread **/
    SNMP_TrapHandler_Start(SNMPTrap);

    while (fgets(cmd, sizeof(cmd), stdin))
    {
        p = strchr(cmd, '\n');
        if (p != NULL) *p = 0;

        ret = sscanf(cmd, "%s%n", token, &len);

        if (ret != 1)
        {
            continue;
        }

        if (!strcmp(token, "resource"))
        {
            ResourceCmd(cmd + len);
            continue;
        }

        if (!strcmp(token, "tunnel"))
        {
            TunnelCmd(cmd + len);
            continue;
        }

        if (!strcmp(token, "snmp"))
        {
            SnmpCmd(cmd + len);
            continue;
        }

        if (!strcmp(token, "dbg"))
        {
            DbgCmd(cmd + len);
            continue;
        }
    }


    SNMP_Lib_Exit();

    return (0);
}
