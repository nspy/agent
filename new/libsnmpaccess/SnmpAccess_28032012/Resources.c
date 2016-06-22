
#include "Resources.h"
//#include <openflow/openflow.h>

X_AddrPtr
X_Addr_Init(X_AddrPtr pAddr)
{
    assert(pAddr != NULL);

    if (pAddr == NULL) return NULL;

    pAddr->Shelf        = 0;
    pAddr->Slot[0]      = '\0';
    pAddr->Port[0]      = '\0';

    return pAddr;
}

X_AddrPtr
X_Addr_Init2(X_AddrPtr pAddr, unsigned int shelf, const char *slot, const char *port)
{
    assert (pAddr != NULL);

    if (pAddr == NULL)
    {
        return NULL;
    }

    X_Addr_Init(pAddr);

    pAddr->Shelf = shelf;

    if (slot != NULL)
        strncpy(pAddr->Slot, slot, sizeof(pAddr->Slot));

    if (port != NULL)
        strncpy(pAddr->Port, port, sizeof(pAddr->Port));

    return pAddr;
}

int
X_Addr_Parse(X_AddrPtr pAddr, const char *s)
{
    int i, j, c = 0;
    char type[8];

    assert (pAddr != NULL);

    if (pAddr == NULL || s == NULL || s[0] == '\0')
    {
        return (-1);
    }

    X_Addr_Init(pAddr);

    i = j = 0;

    while (1)
    {
        if (s[i] == '-' || s[i] == '\0')
        {
            c += 1;

            if (s[j] != '\0')
            {
                switch (c)
                {
                case 1: // type
                    strncpy(type, &s[j], (i - j));
                    type[i-j] = 0;
                    break;
                case 2:
                    pAddr->Shelf = atoi(&s[j]);
                    break;
                case 3:
                    if (i - j >= sizeof(pAddr->Slot)) return (-1);
                    strncpy(pAddr->Slot, &s[j], (i - j));
                    pAddr->Slot[i - j] = 0;
                    break;
                case 4:
                    if (i - j >= sizeof(pAddr->Port)) return (-1);
                    strncpy(pAddr->Port, &s[j], (i - j));
                    pAddr->Port[i - j] = 0;
                    break;
                default:
                    break;
                }
            }

            j = i + 1;
        }

        if (s[i] == '\0') break;

        ++i;
    }

    /** if OL-X or PTP-X -> add offset **/
    if(!strcasecmp("OL", type) ||
            (!strcasecmp("PTP", type) && (pAddr->Slot[0] == '\0') && (pAddr->Port[0] == '\0')))
    {
        pAddr->Shelf += X_RES_OL_SHELF_BASE;
    }

    if(!strcasecmp("ECH", type) || !strcasecmp("VTP", type))
    {
        pAddr->Shelf += X_RES_ECH_SHELF_BASE;
    }

    return (0);
}

int
X_Addr_SetShelf(X_AddrPtr pAddr, const char *s)
{
    if (pAddr == NULL || s == NULL) return (-1);
    pAddr->Shelf = atoi(s);
    return 0;
}

int
X_Addr_SetSlot(X_AddrPtr pAddr, const char *s)
{
    if (pAddr == NULL || s == NULL) return (-1);
    strncpy(pAddr->Slot, s, sizeof(pAddr->Slot));
    return 0;
}

int
X_Addr_SetPort(X_AddrPtr pAddr, const char *s)
{
    if (pAddr == NULL || s == NULL) return (-1);
    strncpy(pAddr->Port, s, sizeof(pAddr->Port));
    return 0;
}

int
X_Addr_Equal(const X_AddrPtr a1, const X_AddrPtr a2)
{
    assert(a1 != NULL && a2 != NULL);

    if (a1 == NULL || a2 == NULL)
    {
        // ?
        return (0);
    }

    if ((a1->Shelf == a2->Shelf) &&
            !strcmp(a1->Slot, a2->Slot) &&
            !strcmp(a1->Port, a2->Port))
    {
        return 0;
    }

    return 1;
}

size_t
X_Addr_Dump(const X_AddrPtr pAddr, char *b, size_t s)
{
    size_t off = 0;

    if (pAddr == NULL || b == NULL) return (0);

    if (pAddr->Shelf)
    {
        off += snprintf(b, s, "%d", pAddr->Shelf);

        if (pAddr->Slot[0])
        {
            off += snprintf(b + off, s- off, "-%s", pAddr->Slot);

            if (pAddr->Port[0])
            {
                off += snprintf(b + off, s - off, "-%s", pAddr->Port);
            }
        }
    }

    return off;
}

X_AddrPtr
X_Addr_Copy(X_AddrPtr pDst, const X_AddrPtr pSrc)
{
    if (pDst == NULL || pSrc == NULL) return NULL;
    memcpy(pDst, pSrc, sizeof(X_Addr));
    return pSrc;
}

/******************************************************************************/

X_CrsPtr
X_Crs_New(void)
{
    X_CrsPtr pCrs = (X_CrsPtr)malloc(sizeof(X_Crs));

    if (pCrs != NULL)
    {
        X_Addr_Init(&pCrs->Addr[0]);
        X_Addr_Init(&pCrs->Addr[1]);
        pCrs->Freq[0] = pCrs->Freq[1] = 0;
        pCrs->Index[0] = pCrs->Index[1] = 0;
    }
    return pCrs;
}

void
X_Crs_Free(X_CrsPtr pCrs)
{
    if (pCrs != NULL)
    {
        free(pCrs);
    }
}


/******************************************************************************/

const char*
X_ModuleType_To_Str(X_ModuleType_t type)
{
    switch (type)
    {
    case X_MOD_TYPE_UNKNOWN:
        return "MOD_UNKNOWN";
    case X_MOD_TYPE_OL:
        return "MOD_OL";
    case X_MOD_TYPE_TRANSPDR:
        return "MOD_TRANSPDR";
    case X_MOD_TYPE_XC:
        return "MOD_XC";
    case X_MOD_TYPE_AMP:
        return "MOD_AMP";
    case X_MOD_TYPE_MUX:
        return "MOD_MUX";
    default: break;
    }
    return "MOD_???";
}


X_ModulePtr
X_Module_New(void)
{
    X_ModulePtr pMod = (X_ModulePtr)malloc(sizeof(X_Module));

    if (pMod == NULL)
    {
        return pMod;
    }

    pMod->ModType = X_MOD_TYPE_UNKNOWN;

    X_Addr_Init(&pMod->Addr);

    X_List_Init(&pMod->Ports, (X_ListMemFree_t)X_Port_Free);

    pMod->NetPort = NULL;
    pMod->AmpMidPort = NULL;
    pMod->UpgradePort = NULL;
    X_List_Init(&pMod->DemuxPorts, NULL);
    X_List_Init(&pMod->AdaptPorts, NULL);
    X_List_Init(&pMod->LinePorts, NULL);

    pMod->Index = 0;
    pMod->EqType = 0;
    pMod->Mode = 0;
    pMod->RoadmNo = 0;
    pMod->Aid[0] = '\0';
    pMod->VectorWch = X_Vector_New(sizeof(AuxEntityData), 100, (X_Vector_MemFree_t)AuxEntityData_Free);

    return pMod;
}

void
X_Module_Free(X_ModulePtr pMod)
{
    if (pMod == NULL)
    {
        return;
    }

    X_List_Clear(&pMod->LinePorts);
    X_List_Clear(&pMod->AdaptPorts);
    X_List_Clear(&pMod->DemuxPorts);

    X_List_Clear(&pMod->Ports);

    X_Vector_Free(pMod->VectorWch);

    free(pMod);

}

X_PortPtr
X_Module_FindPort(X_ModulePtr pMod, const X_AddrPtr pAddr)
{
    X_ListNodePtr pNode;
    X_PortPtr pPort = NULL, pTemp;

    assert (pMod != NULL);

    if (pMod == NULL || pAddr == NULL)
    {
        X_LOGERR(X_LOG_RES_ID, "X_Module_FindPort(): invalid arguments");
        return NULL;
    }

    X_List_ForEach(&pMod->Ports, pNode)
    {
        pTemp = (X_PortPtr)pNode->Data;

        if (!X_Addr_Equal(&pTemp->Addr, pAddr))
        {
            pPort = pTemp;
            break;
        }
    }

    return pPort;
}

X_PortPtr
X_Module_AddPort(X_ModulePtr pMod, const X_AddrPtr pAddr, X_PortCategory_t cat, unsigned int flags)
{
    X_PortPtr pPort;

    if (pMod == NULL || pAddr == NULL)
    {
        X_LOGERR(X_LOG_RES_ID, "X_Module_FindPort(): invalid arguments");
        return NULL;
    }

    pPort = X_Module_FindPort(pMod, pAddr);

    if (pPort != NULL)
    {
        return NULL;
    }

    pPort = X_Port_New(pAddr);

    pPort->Module = pMod;

    X_Port_Support(pPort, flags, 1);

    X_List_Append(&pMod->Ports, pPort);

    switch (cat)
    {
    case X_PORT_CAT_MUX:
        //X_List_Append(&pMod->MuxPorts, pPort);
        pMod->NetPort = pPort;
        break;

    case X_PORT_CAT_DEMUX:
        X_List_Append(&pMod->DemuxPorts, pPort);
        break;

    case X_PORT_CAT_ADAPT:
        X_List_Append(&pMod->AdaptPorts, pPort);
        break;

    case X_PORT_CAT_LINE:
        X_List_Append(&pMod->LinePorts, pPort);
        break;

    case X_PORT_CAT_AMP_MID:
        pMod->AmpMidPort = pPort;
        break;

    case X_PORT_CAT_UPGRADE:
        pMod->UpgradePort = pPort;
        break;

    default:
        break;
    }

    return pPort;
}

int
X_Module_Get_Ports(X_ModulePtr pMod, const X_AddrPtr pAddr, int dir, int freq, int alloc, X_ListPtr pList)
{
    X_ListNodePtr pNode;
    X_PortPtr pPort, pTempPort;

    if (pMod == NULL || pAddr == NULL || pList == NULL || !X_RES_IS_VALID_DIR(dir))
    {
        return (-1);
    }

    // check if port exists on a module
    pPort = X_Module_FindPort(pMod, pAddr);

    // if not present then return error
    if (pPort == NULL)
    {
        return (-1);
    }

    if (pMod->ModType == X_MOD_TYPE_XC || pMod->ModType == X_MOD_TYPE_MUX)
    {
        // if MUX port is not present then return error
        if (pMod->NetPort == NULL) return (-1);

        // special handling for EROADM-DC card
        if (pMod->EqType == EqType_EQPEROADMDC)
        {
            if (pPort == pMod->NetPort)
            {
                // if MUX port then add DEMUX ports and UPGRADE port
                X_List_ForEach(&pMod->DemuxPorts, pNode)
                {
                    X_List_Append(pList, pNode->Data);
                }
                X_List_Append(pList, pMod->UpgradePort);
            }
            else if (pPort == pMod->UpgradePort)
            {
                // if UPGRADE - assign only MUX port
                X_List_Append(pList, pMod->NetPort);
            }
            else
            {
                // if DEMUX -> MUX + UPGRADE
                X_List_Append(pList, pMod->NetPort);
            }
            return (0);
        }

        if (pPort == pMod->NetPort)
        {
            X_List_ForEach(&pMod->DemuxPorts, pNode)
            {
                X_List_Append(pList, pNode->Data);
            }
        }
        else
        {
            // check if demux port
            X_List_ForEach(&pMod->DemuxPorts, pNode)
            {
                if (!X_Addr_Equal(&((X_PortPtr) pNode->Data)->Addr,pAddr))
                {
                    X_List_Append(pList, pMod->NetPort);
                    return (0);
                }
            }
        }
        return (0);
    }
    else if (pMod->ModType == X_MOD_TYPE_AMP)
    {
        if (pMod->AmpMidPort != NULL)
        {
            // double stage AMP, find the other port
            pTempPort = NULL;
            X_List_ForEach((&pMod->Ports), pNode)
            {
                if(X_Addr_Equal(&pPort->Addr, &((X_PortPtr)(pNode->Data))->Addr))
                {
                    pTempPort = (X_PortPtr)(pNode->Data);
                    break;
                }
            }

            if (pTempPort == NULL)
            {
                return -1;
            }

            if (pPort == pMod->AmpMidPort)
            {
                X_List_Append(pList, pTempPort);
            }
            else
            {
                X_List_Append(pList, pMod->AmpMidPort);
            }

            return (0);
        }
        else
        {
            // single stage AMP
            X_List_ForEach(&pMod->Ports, pNode)
            {
                X_List_Append(pList, pNode->Data);
            }
            return (0);
        }
    }
    return (-1);
}

void
X_Module_Dump(X_ModulePtr pMod)
{
    X_ListNodePtr       pNode;
    X_PortPtr           pPort;
    char                tmp[32];

    if (pMod == NULL) return;

    X_Addr_Dump(&pMod->Addr, tmp, sizeof(tmp));

    X_Resource_Printf("Module (%08x): Addr = %s Type = %s (%d) Ports = %d",
            pMod, tmp, X_ModuleType_To_Str(pMod->ModType),
            pMod->ModType, X_List_Size((&(pMod->Ports))));
    X_Resource_Printf("\tAID = %s EqType = %d",
                pMod->Aid, pMod->EqType);

    X_List_ForEach(&pMod->Ports, pNode)
    {
        pPort = (X_PortPtr)pNode->Data;
        X_Port_Dump(pPort);
    }

    X_Resource_Printf("");
}

void
X_Module_Init_Constraints(X_ModulePtr pMod)
{
    X_ListNodePtr pNode;
    X_PortPtr pPort;

    assert(pMod != NULL);

    X_List_ForEach((&pMod->Ports), pNode)
    {
        pPort = (X_PortPtr)(pNode->Data);

        X_PortFeatures_Init_Constraints(&pPort->Features);
    }
}

void
X_Module_Update_Constraints(X_ModulePtr pMod, X_ResourcePtr pRes)
{
    X_ListPtr pList;
    X_ListNodePtr pNode;
    X_PathDataPtr pPathData; //, pPrev;
    //int i;
    char tmp[32];

    if (pMod == NULL)
    {
        return;
    }

    if (pMod->ModType != X_MOD_TYPE_OL && pMod->ModType != X_MOD_TYPE_TRANSPDR)
    {
        return;
    }

    pList = X_List_New((X_ListMemFree_t)X_PathData_Free);

    X_Resource_GetPathsTree(pRes, pMod, X_RES_DIR_IN, X_RES_FREQ_ANY, 0, pList);

    X_List_RForEach(pList, pNode)
    {
        pPathData = (X_PathDataPtr)pNode->Data;

        if (pPathData == NULL)
        {
            continue;
        }

        pMod = pPathData->Port->Module;

        if (pMod->ModType == X_MOD_TYPE_TRANSPDR || pMod->ModType == X_MOD_TYPE_OL)
        {
            while (pPathData != NULL)
            {
               X_Addr_Dump(&pPathData->Port->Addr,tmp,sizeof(tmp));

                if (pPathData->Prev != NULL)
                {
                    if (pPathData->Port->Module != pPathData->Prev->Port->Module)
                    {
                        X_PortFeatures_Update_Constraints(&pPathData->Port->Features,
                                &pPathData->Prev->Port->Features);
                    }
                }

                pPathData = pPathData->Prev;
            }
      //      X_Resource_Printf("--------------");
        }
    }

    X_List_Free(pList);

}

/******************************************************************************/

void
X_PortFeatures_Init(X_PortFeaturesPtr p)
{
    assert (p != NULL);

    p->Flags = 0;
    memset(p->Supported, 0, sizeof(p->Supported));
    memset(p->Constrained, 0xFFFF, sizeof(p->Constrained));
    memset(p->Allocated, 0, sizeof(p->Allocated));
}

void
X_PortFeatures_Init_Constraints(X_PortFeaturesPtr p)
{
    int i;

    assert (p != NULL);

    for(i = 0; i < sizeof(p->Supported) / sizeof(unsigned int); ++i)
    {
        p->Constrained[i] &= p->Supported[i];
    }
}

void
X_PortFeatures_Flag(X_PortFeaturesPtr p, unsigned int flags, int set)
{
    int i, base, offset;
    assert (p != NULL);

    if (set)
    {
        p->Flags |= flags;

        if (flags & X_PORT_SUPPORT_TRANSPARENT)
        {
            for (i = 0; i < sizeof(p->Supported) / sizeof(unsigned int); ++i)
            {
                p->Supported[i] = ~(0);
            }
        }

        if (flags & X_PORT_SUPPORT_C_BAND_EVEN)
        {
            for (i = 0; i <= X_RES_C_BAND_MAX_OFFSET; i += 2)
            {
                base = (X_RES_C_BAND_BASE + i) / (sizeof(unsigned int) << 3); // * 8
                offset = i - base * (sizeof(unsigned int) << 3);
                p->Supported[base] |= (1 << offset);
            }
        }

        if (flags & X_PORT_SUPPORT_C_BAND_ODD)
        {
            for (i = 1; i <= X_RES_C_BAND_MAX_OFFSET; i += 2)
            {
                base = (X_RES_C_BAND_BASE + i) / (sizeof(unsigned int) << 3); // * 8
                offset = i - base * (sizeof(unsigned int) << 3);
                p->Supported[base] |= (1 << offset);
            }
        }

        if (flags & X_PORT_SUPPORT_L_BAND)
        {
            for (i = 0; i <= X_RES_L_BAND_MAX_OFFSET; i++)
            {
                base = (X_RES_L_BAND_BASE + i) / (sizeof(unsigned int) << 3); // * 8
                offset = i - base * (sizeof(unsigned int) << 3);
                p->Supported[base] |= (1 << offset);
            }
        }
    }
    else
    {
        p->Flags &= ~(flags);
    }
}

void
X_PortFeatures_Freq(X_PortFeaturesPtr p, int freq, int set)
{
    int i, base, offset;

    assert (p != NULL);

    i = X_Resource_Freq_To_Index(freq);

    if (i < 0)
    {
        return;
    }

    base = i / (sizeof(unsigned int) << 3);
    offset = i - base * (sizeof(unsigned int) << 3);

    if (set)
    {
        p->Supported[base] |= (1 << offset);
    }
    else
    {
        p->Supported[base] &= ~(1 << offset);
    }
}

int
X_PortFeatures_Test_Supported_Freq(X_PortFeaturesPtr p, unsigned int idx)
{
    int base, offset;

    assert(p != NULL);

    base = idx / (sizeof(unsigned int) << 3);
    offset = idx - base * (sizeof(unsigned int) << 3);

    return (p->Supported[base] & (1 << offset)) ? 1 : 0;
}

void
X_PortFeatures_Update_Constraints(X_PortFeaturesPtr p1, const X_PortFeaturesPtr p2)
{
    int i;
    assert (p1 != NULL && p2 != NULL);

    if (p1 == NULL || p2 == NULL)
    {
        return;
    }

    for(i = 0; i < 4; ++i)
    {
        p1->Constrained[i] &= p2->Constrained[i];
    }
}

X_PortPtr
X_Port_New(const X_AddrPtr pAddr)
{
    X_PortPtr pPort = (X_PortPtr)malloc(sizeof(X_Port));

    if(pPort == NULL)
    {
        return NULL;
    }

    pPort->PortType = X_PORT_TYPE_UNKNOWN;

    X_Addr_Init(&pPort->Addr);

    pPort->Conn[0] = pPort->Conn[1] = NULL;

    if (pAddr != NULL)
    {
        X_Addr_Copy(&pPort->Addr, pAddr);
    }

    pPort->Module = NULL;

    X_PortFeatures_Init(&pPort->Features);

#if 0
    pPort->SupportFlags = 0;

    memset(pPort->Supported, 0, sizeof(pPort->Supported));

    memset(pPort->Constrained, 0xff, sizeof(pPort->Constrained));
#endif

    pPort->Index = 0;

    return pPort;
}

void
X_Port_Free(X_PortPtr pPort)
{
    if (pPort == NULL)
    {
        return;
    }

    X_Port_DeleteConn(pPort, X_RES_DIR_IN);
    X_Port_DeleteConn(pPort, X_RES_DIR_OUT);

    free(pPort);
}

int
X_Port_SetConn(X_PortPtr pPort, X_PortPtr pTo, int dir)
{
    if (pPort == NULL || pTo == NULL || !X_RES_IS_VALID_DIR(dir))
    {
        X_LOGWARN(X_LOG_RES_ID, "X_Port_SetConn: invalid params");
        return (-1);
    }

    if (pPort->Conn[dir] != NULL)
    {
        X_LOGWARN(X_LOG_RES_ID, "X_Port_SetConn: overriding pointer");
    }

    if (pPort->Conn[X_RES_REV_DIR(dir)] != NULL)
    {
        X_LOGWARN(X_LOG_RES_ID, "X_Port_SetConn: overriding pointer");
    }

    pPort->Conn[dir] = pTo;
    pTo->Conn[X_RES_REV_DIR(dir)] = pPort;

    return (0);
}

X_PortPtr
X_Port_GetConn(X_PortPtr pPort, int dir)
{
    assert(pPort != NULL);

    if (pPort == NULL || !X_RES_IS_VALID_DIR(dir))
    {
        return NULL;
    }

    return pPort->Conn[dir];
}

void
X_Port_DeleteConn(X_PortPtr pPort, int dir)
{
    if (pPort == NULL || !X_RES_IS_VALID_DIR(dir))
    {
        return;
    }

    if (pPort->Conn[dir] != NULL)
    {
        pPort->Conn[dir]->Conn[X_RES_REV_DIR(dir)] = NULL;
        pPort->Conn[dir] = NULL;
    }
}

void
X_Port_Dump(X_PortPtr pPort)
{
    char tmp[32];
    char freq[256];
    int i, base, offset, d;

    if (pPort == NULL) return;

    X_Addr_Dump(&pPort->Addr, tmp, sizeof(tmp));

    X_Resource_Printf("Port (%x): Addr = %s Module = %08x", pPort, tmp, pPort->Module);
    X_Resource_Printf("\tFlags: %08x", pPort->Features.Flags);
    X_Resource_Printf("\tIndex: %08x", pPort->Index);

    d = 0;
    X_Resource_Printf("\tSupported:"); // %08x %08x %08x", pPort->Features.Supported[0], pPort->Features.Supported[1], pPort->Features.Supported[2]);
    for (i = 0; i < sizeof(pPort->Features.Supported) * 8; ++i)
    {
        base = i / (sizeof(unsigned int) << 3);
        offset = i - base * (sizeof(unsigned int) << 3);
        d += sprintf(freq + d,"%c",(pPort->Features.Supported[base] & (1 << offset)) ? '*' : '-');
    }
    X_Resource_Printf("%s",freq);

    d = 0;
    X_Resource_Printf("\tConstrained:");
    for (i = 0; i < sizeof(pPort->Features.Constrained) * 8; ++i)
    {
        base = i / (sizeof(unsigned int) << 3);
        offset = i - base * (sizeof(unsigned int) << 3);
        d += sprintf(freq + d,"%c",(pPort->Features.Constrained[base] & (1 << offset)) ? '*' : '-');
    }
    X_Resource_Printf("%s",freq);

    if (pPort->Conn[X_RES_DIR_IN] == NULL && pPort->Conn[X_RES_DIR_OUT] == NULL)
    {
        return;
    }

    if(pPort->Conn[X_RES_DIR_IN] == pPort->Conn[X_RES_DIR_OUT])
    {
        X_Addr_Dump(&pPort->Conn[X_RES_DIR_OUT]->Addr, tmp, sizeof(tmp));
        X_Resource_Printf("\t<-> %s",tmp);
        return;
    }

    if (pPort->Conn[X_RES_DIR_OUT])
    {
        X_Addr_Dump(&pPort->Conn[X_RES_DIR_OUT]->Addr, tmp, sizeof(tmp));
        X_Resource_Printf("\t--> %s",tmp);
    }

    if (pPort->Conn[X_RES_DIR_IN])
    {
        X_Addr_Dump(&pPort->Conn[X_RES_DIR_IN]->Addr, tmp, sizeof(tmp));
        X_Resource_Printf("\t<-- %s",tmp);
    }


}

void
X_Port_Support(X_PortPtr pPort, unsigned int flags, int set)
{
#if 0
    int i = 0;
    int byte, offset;

    if (pPort == NULL)
    {
        return;
    }

    if (set)
    {
        pPort->SupportFlags |= flags;
    }
    else
    {
        pPort->SupportFlags &= ~(flags);
    }



    if(set)
    {
        if (flags & X_PORT_SUPPORT_TRANSPARENT)
        {
            for (i = 0; i < sizeof(pPort->Supported); ++i)
            {
                pPort->Supported[i] = 0xff;
            }
        }

        if (flags & X_PORT_SUPPORT_C_BAND_EVEN)
        {
            i = 0;

            while (X_RES_C_BAND_BASE + i < X_RES_C_BAND_MAX_OFFSET)
            {
                byte = (X_RES_C_BAND_BASE + i) / 8;
                offset = (X_RES_C_BAND_BASE + i) - byte * 8;
                pPort->Supported[byte] |= (1 << offset);
                i += 2;
            }
        }

        if (flags & X_PORT_SUPPORT_C_BAND_ODD)
        {
            i = 1;

            while (X_RES_C_BAND_BASE + i < X_RES_C_BAND_MAX_OFFSET)
            {
                byte = (X_RES_C_BAND_BASE + i) / 8;
                offset = (X_RES_C_BAND_BASE + i) - byte * 8;
                pPort->Supported[byte] |= (1 << offset);
                i += 2;
            }
        }
    }
#endif

    X_PortFeatures_Flag(&pPort->Features, flags, set);
}

void
X_Port_Set_Freq(X_PortPtr pPort, int freq, int set)
{
    assert (pPort != NULL);

    X_PortFeatures_Freq(&pPort->Features, freq, set);
}

/******************************************************************************/

X_PathDataPtr
X_PathData_New(void)
{
    X_PathDataPtr pData = (X_PathDataPtr)malloc(sizeof(X_PathData));
    if (pData != NULL)
    {
        pData->PathDir = X_RES_DIR_OUT;
        pData->Module = NULL;
        pData->Port = NULL;
        pData->Prev = NULL;
        pData->Dir = X_RES_DIR_OUT;
        pData->Term = 0;
    }
    return pData;
}

void
X_PathData_Free(X_PathDataPtr pData)
{
    if (pData != NULL)
    {
        free(pData);
    }
}

/******************************************************************************/

X_ResourcePtr
X_Resource_New(void)
{
    X_ResourcePtr pRes = (X_ResourcePtr)malloc(sizeof(X_Resource));

    if (pRes != NULL)
    {
        X_Resource_Init(pRes);
    }

    return pRes;
}

void
X_Resource_Free(X_ResourcePtr pRes)
{
    if (pRes == NULL) return;
    X_Resource_Deinit(pRes);
    free(pRes);
}

X_ResourcePtr
X_Resource_Init(X_ResourcePtr pRes)
{
    if (pRes == NULL)
    {
        return NULL;
    }

    pRes->Mutex = X_Mutex_New();

    X_List_Init(&pRes->Modules, (X_ListMemFree_t)X_Module_Free);

    X_List_Init(&pRes->CrossConnects, (X_ListMemFree_t)X_Crs_Free);

    pRes->SNMPChan = NULL;

    pRes->UserData = NULL;

    return pRes;
}

void
X_Resource_Deinit(X_ResourcePtr pRes)
{
    if (pRes == NULL) return;

    X_List_Clear(&pRes->Modules);

    X_List_Clear(&pRes->CrossConnects);

    X_Mutex_Free(pRes->Mutex);
}

void
X_Resource_Lock(X_ResourcePtr pRes)
{
    assert (pRes != NULL);
    X_Mutex_Lock(pRes->Mutex);
}

void
X_Resource_Unlock(X_ResourcePtr pRes)
{
    assert (pRes != NULL);
    X_Mutex_Unlock(pRes->Mutex);
}

X_ModulePtr
X_Resource_FindModule(X_ResourcePtr pRes, const X_AddrPtr pAddr)
{
    X_ModulePtr pMod = NULL, pTemp;
    X_ListNodePtr pNode;

    if (pRes == NULL || pAddr == NULL)
    {
        return NULL;
    }

    X_List_ForEach(&pRes->Modules, pNode)
    {
        pTemp = (X_ModulePtr)pNode->Data;

        if (!X_Addr_Equal(&pTemp->Addr, pAddr))
        {
            pMod = pTemp;
            break;
        }
    }

    return pMod;
}

X_PortPtr
X_Resource_FindPort(X_ResourcePtr pRes, const X_AddrPtr pAddr)
{
    X_ListNodePtr pNode;
    X_PortPtr pPort = NULL;

    if (pRes == NULL || pAddr == NULL)
    {
        return NULL;
    }

    X_List_ForEach(&pRes->Modules, pNode)
    {
        pPort = X_Module_FindPort((X_ModulePtr)pNode->Data, pAddr);
        if (pPort != NULL)
            break;
    }

    return pPort;
}

void
X_Resource_Clear(X_ResourcePtr pRes)
{
    if (pRes == NULL)
    {
        return;
    }

    while (X_List_Size((&pRes->Modules)) > 0)
    {
        X_List_Destroy(&pRes->Modules, X_List_Head(&pRes->Modules));
    }
}

void
X_Resource_Dump(X_ResourcePtr pRes)
{
    X_ModulePtr pMod;
    X_ListNodePtr pNode;

    if (pRes == NULL)
    {
        return;
    }

    X_Resource_Printf("========================================");
    X_Resource_Printf("X_Resource (%08x)", pRes);
    X_Resource_Printf("\tTotal Modules: %d", X_List_Size((&(pRes->Modules))));
    X_Resource_Printf("");

    X_List_ForEach(&pRes->Modules, pNode)
    {
        X_Resource_Printf("----------------------------------------");
        pMod = (X_ModulePtr)pNode->Data;
        X_Module_Dump(pMod);
    }
    X_Resource_Printf("========================================");
}

int
X_Resource_Set_User_Data(X_ResourcePtr pRes, void *ptr)
{
    if (pRes != NULL)
    {
        pRes->UserData = ptr;
    }
    return (pRes != NULL) ? 0 : -1;
}

int
X_Resource_Get_User_Data(X_ResourcePtr pRes, void **ptr)
{
    if (pRes == NULL || ptr == NULL)
    {
        return -1;
    }
    *ptr = pRes->UserData;
    return (0);
}

void
X_Resource_GetPathsTree(X_ResourcePtr pRes,
        X_ModulePtr pMod,
        int dir,
        int freq,
        int alloc,
        X_ListPtr pPaths)
{
    X_List NextPath, Ports;
    X_PortPtr pPort = NULL;
    X_PathDataPtr pPathData, pTempPath;
    X_ListNodePtr pNode;
    int FreqIdx = X_RES_FREQ_INVALID;

    if (pRes == NULL || pMod == NULL || pPaths == NULL)
    {
        X_LOGERR(3, "X_Resource_GetPathsTree: invalid arguments");
        return;
    }

    if (freq != X_RES_FREQ_ANY)
    {
        FreqIdx = X_Resource_Freq_To_Index(freq);
    }

    X_List_Init(&NextPath, NULL);
    X_List_Init(&Ports, NULL);

    if (pMod->ModType == X_MOD_TYPE_OL || pMod->ModType == X_MOD_TYPE_TRANSPDR)
    {
        //pPort = (X_PortPtr)X_List_Head(&(pMod->LinePorts))->Data;

        X_List_ForEach((&(pMod->LinePorts)), pNode)
        {
            pPort = (X_PortPtr)(pNode->Data);

            if (pPort == NULL)
                continue;

            if (FreqIdx != X_RES_FREQ_INVALID)
            {
                if (!X_PortFeatures_Test_Supported_Freq(&pPort->Features, FreqIdx))
                {
                    continue;
                }
            }

            pPathData = X_PathData_New();

            pPathData->PathDir = dir;
            pPathData->Port = pPort;
            pPathData->Module = NULL;
            pPathData->Dir = dir;

            X_List_Append(&NextPath, pPathData);
        }
    }

    while (!X_List_Empty((&NextPath)))
    {
        X_List_Remove(&NextPath, X_List_Head(&NextPath), (void**)&pPathData);

        if (pPathData == NULL)
        {
            continue;
        }

        if (pPathData->Module != NULL)
        {
            X_Module_Get_Ports(pPathData->Module, &pPathData->Port->Addr, pPathData->Dir, freq, alloc, &Ports);

            // can't reach any other port, hence terminate the fiber map
            if (X_List_Size((&Ports)) == 0)
            {
                pPathData->Term = 1;
            }

            while (!X_List_Empty((&Ports)))
            {
                X_List_Remove(&Ports, X_List_Head(&Ports), (void**)&pPort);

                if (pPort == NULL) continue;

                // check supported frequency

                if (FreqIdx != X_RES_FREQ_INVALID)
                {
                    if (!X_PortFeatures_Test_Supported_Freq(&pPort->Features, FreqIdx))
                    {
                        continue;
                    }
                }

                pTempPath = X_PathData_New();
                pTempPath->PathDir = pPathData->PathDir;
                pTempPath->Module = NULL;
                pTempPath->Port = pPort;
                pTempPath->Prev = pPathData;
                pTempPath->Dir = X_RES_REV_DIR(pPathData->Dir);
                pTempPath->Term = 0;

                X_List_Append(&NextPath, pTempPath);
            }
        }
        else
        {
            pPort = X_Port_GetConn(pPathData->Port,pPathData->Dir);

            if (pPort != NULL)
            {
                if (FreqIdx != X_RES_FREQ_INVALID)
                {
                    if (!X_PortFeatures_Test_Supported_Freq(&pPort->Features, FreqIdx))
                    {
                        pPort = NULL;
                    }
                }

                if (pPort != NULL)
                {
                    pTempPath = X_PathData_New();
                    pTempPath->PathDir = pPathData->PathDir;
                    pTempPath->Module = pPort->Module;
                    pTempPath->Port = pPort;
                    pTempPath->Prev = pPathData;
                    pTempPath->Dir = X_RES_REV_DIR(pPathData->Dir);
                    X_List_Append(&NextPath, pTempPath);
                }
            }
            else
            {
                // nothing is connected, hence mark it as a fiber map termination
                pPathData->Term = 1;
            }
        }

        X_List_Append(pPaths, pPathData);
    }


#if 0
    char tmp[32];
    X_List_ForEach(pPaths, pNode)
    {
        pTempPath = (X_PathDataPtr)pNode->Data;
        X_Addr_Dump(&pTempPath->Port->Addr, tmp, sizeof(tmp));

        fprintf(stderr,"Dir = %s Port = %s Term = %d\r\n",
                (pTempPath->Dir == X_RES_DIR_IN) ? "IN" : "OUT", tmp, pTempPath->Term);

    }
    fprintf(stderr,"\n");
#endif
}


#if 0
void
X_Resource_GetPathsTreeFromPort(X_ResourcePtr pRes, X_PortPtr pPort, int dir,
        int freq, X_ListPtr pPaths)
{
    X_List NextPath, Ports;
    X_PathDataPtr pPathData, pTempPath;
    X_ListNodePtr pNode;

    if (pRes == NULL || pPort == NULL || pPaths == NULL)
    {
        X_LOGERR(3, "X_Resource_GetPathsTree: invalid arguments");
        return;
    }

    X_List_Init(&NextPath, NULL);
    X_List_Init(&Ports, NULL);

    pPathData = X_PathData_New();

    pPathData->Port = pPort;
    pPathData->Module = NULL;
    pPathData->Dir = dir;

    X_List_Append(&NextPath, pPathData);

    while (!X_List_Empty((&NextPath)))
    {
        X_List_Remove(&NextPath, X_List_Head(&NextPath), (void**)&pPathData);

        if (pPathData == NULL)
        {
            continue;
        }

        if (pPathData->Module != NULL)
        {
            X_Module_Get_Ports(pPathData->Module, &pPathData->Port->Addr, pPathData->Dir, freq, 0, &Ports);

            // can't reach any other port, hence terminate the fiber map
            if (X_List_Size((&Ports)) == 0)
            {
                pPathData->Term = 1;
            }

            while (!X_List_Empty((&Ports)))
            {
                X_List_Remove(&Ports, X_List_Head(&Ports), (void**)&pPort);

                if (pPort == NULL) continue;

                pTempPath = X_PathData_New();
                pTempPath->Module = NULL;
                pTempPath->Port = pPort;
                pTempPath->Prev = pPathData;
                pTempPath->Dir = X_RES_REV_DIR(pPathData->Dir);
                pTempPath->Term = 0;
                X_List_Append(&NextPath, pTempPath);
            }
        }
        else
        {
            pPort = X_Port_GetConn(pPathData->Port,pPathData->Dir);

            if (pPort != NULL)
            {
                pTempPath = X_PathData_New();
                pTempPath->Module = pPort->Module;
                pTempPath->Port = pPort;
                pTempPath->Prev = pPathData;
                pTempPath->Dir = X_RES_REV_DIR(pPathData->Dir);
                X_List_Append(&NextPath, pTempPath);
            }
            else
            {
                // nothing is connected, hence mark it as a fiber map termination
                pPathData->Term = 1;
            }
        }

        X_List_Append(pPaths, pPathData);
    }

#if 0
    char tmp[32];
    X_List_ForEach(pPaths, pNode)
    {
        pTempPath = (X_PathDataPtr)pNode->Data;
        X_Addr_Dump(&pTempPath->Port->Addr, tmp, sizeof(tmp));

        fprintf(stderr,"Dir = %s Port = %s Term = %d\r\n",
                (pTempPath->Dir == X_RES_DIR_IN) ? "IN" : "OUT", tmp, pTempPath->Term);
    }
    fprintf(stderr,"\n");
#endif
}
#endif

int
X_Resource_PrunePaths(X_ListPtr pList, X_ModulePtr pSrcMod)
{
    X_ListNodePtr pNode;
    X_PathDataPtr pPathData;
    X_ModulePtr pMod;
    char tmp[32];
    int i = 0;

    if (pList == NULL)
    {
        return (-1);
    }

    X_List_RForEach(pList, pNode)
    {
        pPathData = (X_PathDataPtr)pNode->Data;

        if (pPathData == NULL)
        {
            continue;
        }

        pMod = pPathData->Port->Module;

        if (pMod->ModType == X_MOD_TYPE_TRANSPDR || pMod->ModType == X_MOD_TYPE_OL)
        {
            X_Resource_Printf("Path %d Dir = %s", ++i, X_RES_DIR_TO_STR(pPathData->Dir));
            while (pPathData != NULL)
            {
                X_Addr_Dump(&pPathData->Port->Addr, tmp, sizeof(tmp));
                X_Resource_Printf("- %s", tmp);
                pPathData = pPathData->Prev;
            }
            X_Resource_Printf("--------------");
        }
    }

    return (0);
}

void
X_Resource_Update(X_ResourcePtr pRes)
{
    X_ListNodePtr pNode;

    X_List_ForEach((&pRes->Modules), pNode)
    {
        X_Module_Init_Constraints((X_ModulePtr)pNode->Data);
    }


    X_List_ForEach((&pRes->Modules), pNode)
    {
        X_Module_Update_Constraints((X_ModulePtr)pNode->Data, pRes);
    }
}

int
X_Resource_Find_Path(X_ResourcePtr pRes, int dir, int freq,
        X_ModulePtr pSrcMod, X_ModulePtr pDstMod, X_ListPtr pList)
{
    X_ListPtr pPathList;
    X_ListNodePtr pNode;
    X_PathDataPtr pPathData, pTempPathData;
    //char tmp[32];

    if (pRes == NULL || pList == NULL || pSrcMod == NULL || pDstMod == NULL)
    {
        return (-1);
    }

    pPathList = X_List_New((X_ListMemFree_t)X_PathData_Free);

    X_Resource_GetPathsTree(pRes, pSrcMod, dir, freq, 0, pPathList);

    if (X_List_Size(pPathList) == 0)
    {
        return (0);
    }

    pPathData = NULL;

    X_List_RForEach(pPathList, pNode)
    {
        pTempPathData = (X_PathDataPtr)(pNode->Data);

        if (pTempPathData->Term &&
                !X_Addr_Equal(&pDstMod->Addr, &pTempPathData->Port->Module->Addr))
        {
            pPathData = pTempPathData;
            break;
        }
    }

    if (pPathData == NULL)
    {
        X_List_Free(pPathList);
        return (0);
    }

    while (pPathData != NULL)
    {
        X_List_Prepend(pList, pPathData);
        //X_Addr_Dump(&pPathData->Port->Addr, tmp, sizeof(tmp));
        //X_Resource_Printf("- %s", tmp);
        pPathData = pPathData->Prev;
    }

    return (0);
}

X_CrsPtr
X_Resource_Find_Crs(X_ResourcePtr pRes, const X_AddrPtr From, int Freq)
{
    X_ListNodePtr       pNode;
    X_CrsPtr            pCrs;

    if (pRes == NULL || From == NULL)
        return NULL;

    X_List_ForEach((&pRes->CrossConnects), pNode)
    {
        pCrs = (X_CrsPtr)pNode->Data;

        assert(pCrs != NULL);

        if (pCrs == NULL)
            continue;

        if (0 == X_Addr_Equal(&pCrs->Addr[0], From) && pCrs->Freq[0] == Freq)
        {
            return pCrs;
        }
    }
    return NULL;
}

void
X_Resource_Set_SNMP_Channel(X_ResourcePtr pResource, SNMP_ChannelPtr pChannel)
{
    if (pResource == NULL || pChannel == NULL)
    {
        return;
    }

    pResource->SNMPChan = pChannel;
    pResource->Session = SNMP_Channel_Duplicate_Session(pChannel);
}

void
X_Resource_Printf(const char *fmt, ...)
{
    //char tmp[512];
    va_list ap;

    va_start(ap, fmt);
    X_Log_VPrintf(X_LOG_RES_ID, X_LOG_LEVEL_INFO, fmt, ap);
    va_end(ap);
}

int
X_Resource_Freq_To_Index(int freq)
{
    int i, tmp = 0;

    if (freq >= X_RES_C_BAND_MIN_FREQ && freq <= X_RES_C_BAND_MAX_FREQ)
    {
        // DWDM C-Band 50+100 GHz spacing
        for (i = 0; tmp <= X_RES_C_BAND_MAX_FREQ; ++i)
        {
            tmp = X_RES_C_BAND_MIN_FREQ + i * X_RES_50GHz;

            if (tmp == freq)
            {
                return (X_RES_C_BAND_BASE + i);
            }
        }
    }
    else if (freq >= X_RES_L_BAND_MIN_FREQ && freq <= X_RES_L_BAND_MAX_FREQ)
    {
        // DWDM L-Band 100 GHz spacing
        for( i = 0; tmp <= X_RES_L_BAND_MAX_FREQ; ++i)
        {
            tmp = X_RES_L_BAND_MIN_FREQ + i * X_RES_100GHz;
            if (tmp == freq)
            {
                return (X_RES_L_BAND_BASE + i);
            }
        }
    }
    return (-1);
}

int
X_Resource_Index_To_Freq(int idx)
{
    if (idx < 0)
    {
        return -1;
    }

    if (idx >= X_RES_C_BAND_BASE && idx <= X_RES_C_BAND_MAX_OFFSET)
    {
        return (X_RES_C_BAND_MIN_FREQ + idx * X_RES_50GHz);
    }
    else if (idx >= X_RES_L_BAND_BASE && idx <= X_RES_L_BAND_MAX_OFFSET)
    {
        return (X_RES_L_BAND_BASE + idx * X_RES_100GHz);
    }

    return -1;
}

/******************************************************************************/

X_ModuleType_t
X_GetModuleType(unsigned int eqtype)
{
    switch (eqtype)
    {
    case EqType_EQPWCA10GD:
    case EqType_EQPWCC10GD:
    case EqType_EQPWCC10GTD:

    case EXTERNAL_CHANNEL:
        return (X_MOD_TYPE_TRANSPDR);

    case EqType_EQPEROADMDC:
    case EqType_EQP8ROADMC40:
    case EqType_EQP8ROADMC80:
    case EqType_EQPCCM8:
    case EqType_EQP8CCMC80:
        return (X_MOD_TYPE_XC);

    case EqType_EQPEDFASGC:
    case EqType_EQPEDFASGCB:
    case EqType_EQPEDFADGCV:
        return (X_MOD_TYPE_AMP);

    case EqType_EQP40CSMD:
        return (X_MOD_TYPE_MUX);

    default:
        break;
    }

    return (X_MOD_TYPE_UNKNOWN);
}

int
X_Resource_DestroyModule(X_ResourcePtr pRes, const X_AddrPtr pAddr)
{
    X_ListNodePtr pNode;
    X_ModulePtr pMod;
    char tmp[32];

    if (pRes == NULL || pAddr == NULL)
    {
        return (-1);
    }

    X_Addr_Dump(pAddr, tmp, sizeof(tmp));

    X_List_ForEach(&pRes->Modules, pNode)
    {
        pMod = (X_ModulePtr)(pNode->Data);
        if (!X_Addr_Equal(&pMod->Addr, pAddr))
        {
            break;
        }
    }

    if (pNode == NULL)
    {
        X_LOGWARN(X_LOG_RES_ID, "X_Resource_DestroyModule: module %s not found", tmp);
        return (0);
    }

    X_LOGINFO(X_LOG_RES_ID, "X_Resource_DestroyModule: destroying module %s", tmp);

    X_List_Destroy(&pRes->Modules, pNode);

    return (0);
}

X_ModulePtr
X_LoadEntityShelf(X_ResourcePtr pRes, const EntityShelf *e, X_Resource_Oper_t oper)
{
    X_ModulePtr         pMod;
    X_PortPtr           pPort;
    X_Addr              Addr;
    int                 i;
    char                tmp[32];

    ErrCode             Err;
    SNMP_ErrCode        SnmpErr;
    AuxEntityData*      pAuxData;
    X_ListNodePtr       pNode;
    X_ListNodePtr       pNode2;
    X_ListPtr           pList;

    assert (pRes != NULL && e != NULL);

    if (pRes == NULL || e == NULL)
    {
        return NULL;
    }

    if (oper == X_RES_OPER_DESTROY)
    {
        X_Addr_Parse(&Addr, e->Aid);

        X_Resource_DestroyModule(pRes, &Addr);

        return NULL;
    }
    else if (oper == X_RES_OPER_CREATE)
    {
        if (X_GetModuleType(e->Type) == 0)
        {
            X_LOGWARN(X_LOG_RES_ID, "X_LoadEntityShelf: unsupported shelf type");
            return NULL;
        }

        pMod = X_Module_New();

        if (pMod == NULL)
        {
            return NULL;
        }

        X_Addr_Parse(&pMod->Addr,e->Aid);

        pMod->ModType = X_GetModuleType(e->Type);

        pMod->Index = Entity_Get_Index(e);
        pMod->EqType = e->Type;

        strncpy(pMod->Aid, e->Aid, sizeof(pMod->Aid));

        X_Addr_Copy(&Addr,&pMod->Addr);

        // load additional data from SNMP
        pList = X_List_New((X_ListMemFree_t)AuxEntityData_Free);

        Err = SNMP_Contains(pRes->Session, &SnmpErr, pMod->Index, pList);


        switch (e->Type)
        {
        case EqType_EQPEROADMDC:

            for (i = 1; i <= 40; i++)
            {
                sprintf(tmp,"C%d",i);
                X_Addr_SetSlot(&Addr,tmp);
                pPort = X_Module_AddPort(pMod,&Addr,X_PORT_CAT_DEMUX, 0);
                if (pPort != NULL)
                {
                    X_Port_Set_Freq(pPort, 195900 - (i - 1) * 100, 1);
                }
            }
            X_Addr_SetSlot(&Addr,"N");
            X_Module_AddPort(pMod,&Addr,X_PORT_CAT_MUX, X_PORT_SUPPORT_C_BAND_EVEN);
            X_Addr_SetSlot(&Addr,"U");
            X_Module_AddPort(pMod,&Addr,X_PORT_CAT_UPGRADE, X_PORT_SUPPORT_C_BAND_EVEN);
            break;
        }

        // assign port indexes
        if (pMod != NULL)
        {
            X_List_ForEach(&pMod->Ports, pNode)
            {
                pPort = (X_PortPtr) pNode->Data;

                X_List_ForEach(pList, pNode2)
                {
                    pAuxData = (AuxEntityData*) pNode2->Data;

                    if (pAuxData->Class != EntityClass_NETWORKPORT
                            && pAuxData->Class != EntityClass_CLIENTPORT
                            && pAuxData->Class != EntityClass_UPGRADEPORT)
                    {
                        continue;
                    }

                    X_Addr_Parse(&Addr,pAuxData->Aid);

                    if (0 == X_Addr_Equal(&Addr,&pPort->Addr))
                    {
                        pPort->Index = pAuxData->Index;
                        X_List_Destroy(pList,pNode2);
                        break;
                    }
                }
            }
        }

        X_List_Append(&pRes->Modules, pMod);

        return pMod;
    }

    return NULL;
}


X_ModulePtr
X_LoadEntityModule(X_ResourcePtr pRes, const EntityModule *e, X_Resource_Oper_t oper)
{
    X_ModulePtr         pMod;
    X_PortPtr           pPort;
    X_Addr              Addr;
    //X_Addr              TmpAddr;
    char                Tmp[8];
    X_ModuleType_t      Type;
    int                 i;

    ErrCode             Err;
    SNMP_ErrCode        SnmpErr;
    AuxEntityData*      pAuxData;
    X_ListNodePtr       pNode;
    X_ListNodePtr       pNode2;
    X_ListPtr           pList;

    assert (pRes != NULL && e != NULL);

    if (pRes == NULL || e == NULL)
    {
        X_LOGWARN(X_LOG_RES_ID, "X_LoadEntityModule: invalid params");
        return NULL;
    }

    if (oper == X_RES_OPER_DESTROY)
    {
        X_Addr_Parse(&Addr, e->Aid);
        X_Resource_DestroyModule(pRes, &Addr);
        return NULL;
    }
    else if (oper == X_RES_OPER_CREATE)
    {
        Type = X_GetModuleType(e->Type);

        if (Type == X_MOD_TYPE_UNKNOWN)
        {
            X_LOGWARN(X_LOG_RES_ID, "X_LoadEntityModule: unsupported module");
            return NULL;
        }

        pMod = X_Module_New();

        if (pMod == NULL)
        {
            return NULL;
        }

        X_Addr_Parse(&pMod->Addr, e->Aid);

        pMod->ModType = Type;

        pMod->Index = Entity_Get_Index(e);
        pMod->Mode = e->Mode;
        pMod->EqType = e->Type;
        pMod->RoadmNo = e->RoadmNo;
        strncpy(pMod->Aid, e->Aid, sizeof(pMod->Aid));
        X_Addr_Copy(&Addr, &pMod->Addr);

        // load additional data from SNMP
        pList = X_List_New((X_ListMemFree_t)AuxEntityData_Free);

        Err = SNMP_Contains(pRes->Session, &SnmpErr, pMod->Index, pList);

        switch (e->Type)
        {
        case EqType_EQPEDFASGC:
        case EqType_EQPEDFASGCB:
            X_Addr_SetPort(&Addr, "1");
            X_Module_AddPort(pMod, &Addr, X_PORT_CAT_OTHER, X_PORT_SUPPORT_TRANSPARENT);
            break;

        case EqType_EQPEDFADGCB:
        case EqType_EQPEDFADGCV:
            X_Addr_SetPort(&Addr, "1");
            X_Module_AddPort(pMod,&Addr, X_PORT_CAT_OTHER, X_PORT_SUPPORT_TRANSPARENT);
            X_Addr_SetPort(&Addr, "2");
            X_Module_AddPort(pMod,&Addr, X_PORT_CAT_AMP_MID, X_PORT_SUPPORT_TRANSPARENT);
            break;

        case EqType_EQPCCM8:
        case EqType_EQP8ROADMC40:
            for (i = 1; i <= 8; i++)
            {
                snprintf(Tmp, sizeof(Tmp), "C%d", i);
                X_Addr_SetPort(&Addr,Tmp);
                X_Module_AddPort(pMod,&Addr,X_PORT_CAT_DEMUX, X_PORT_SUPPORT_C_BAND_EVEN);
            }
            X_Addr_SetPort(&Addr, "N");
            X_Module_AddPort(pMod, &Addr, X_PORT_CAT_MUX, X_PORT_SUPPORT_C_BAND_EVEN);
            break;

        case EqType_EQP8CCMC80:
        case EqType_EQP8ROADMC80:
            for (i = 1; i <= 8; i++)
            {
                snprintf(Tmp, sizeof(Tmp), "C%d", i);
                X_Addr_SetPort(&Addr,Tmp);
                X_Module_AddPort(pMod,&Addr,X_PORT_CAT_DEMUX,X_PORT_SUPPORT_C_BAND_EVEN|X_PORT_SUPPORT_C_BAND_ODD);
            }
            X_Addr_SetPort(&Addr, "N");
            X_Module_AddPort(pMod, &Addr, X_PORT_CAT_MUX,X_PORT_SUPPORT_C_BAND_EVEN|X_PORT_SUPPORT_C_BAND_ODD);
            break;

        case EqType_EQP40CSMD:
            for (i = 1; i <= 40; i++)
            {
                snprintf(Tmp, sizeof(Tmp), "C%d", i);
                X_Addr_SetPort(&Addr, Tmp);
                pPort = X_Module_AddPort(pMod,&Addr,X_PORT_CAT_DEMUX, 0);

                if (i >= 2 && i <= 32)
                {
                    X_Port_Set_Freq(pPort, ADVA_Channel_To_Freq(Channel_D02 + i - 2), 1);
                }
                else if (i >= 33)
                {
                    X_Port_Set_Freq(pPort, ADVA_Channel_To_Freq(Channel_DC1 + i - 33), 1);
                }
                else
                {
                    X_Port_Set_Freq(pPort, ADVA_Channel_To_Freq(Channel_DC9), 1);
                }
            }
            X_Addr_SetPort(&Addr, "N");
            X_Module_AddPort(pMod, &Addr, X_PORT_CAT_MUX, X_PORT_SUPPORT_C_BAND_EVEN);
            break;

        case EqType_EQPWCC10GD:
        case EqType_EQPWCA10GD:
            X_Addr_SetPort(&Addr,"C");
            X_Module_AddPort(pMod,&Addr,X_PORT_CAT_ADAPT, 0);

            X_Addr_SetPort(&Addr,"N");
            pPort = X_Module_AddPort(pMod,&Addr,X_PORT_CAT_LINE, 0);
            if (pPort != NULL)
            {
                if (e->Channel != 0)
                    X_Port_Set_Freq(pPort, ADVA_Channel_To_Freq(e->Channel), 1);
            }
            break;

        case EqType_EQPWCC10GTD:
            X_Addr_SetPort(&Addr,"C");
            X_Module_AddPort(pMod,&Addr,X_PORT_CAT_ADAPT, 0);
            X_Addr_SetPort(&Addr,"N");
            X_Module_AddPort(pMod,&Addr,X_PORT_CAT_LINE,
                    (e->Band == Band_BANDC) ?
                            (X_PORT_SUPPORT_C_BAND_ODD | X_PORT_SUPPORT_C_BAND_EVEN) : X_PORT_SUPPORT_L_BAND);
            break;

        case EXTERNAL_CHANNEL:
            //X_Addr_SetPort(&addr, "N");
            pPort = X_Module_AddPort(pMod, &Addr, X_PORT_CAT_LINE,0);
            X_Port_Set_Freq(pPort, ADVA_Channel_To_Freq(e->Channel), 1);
            break;
        }

        // assign port indexes
        if (pMod != NULL && pMod->EqType != EXTERNAL_CHANNEL)
        {
            X_List_ForEach(&pMod->Ports, pNode)
            {
                pPort = (X_PortPtr)pNode->Data;

                X_List_ForEach(pList, pNode2)
                {
                    pAuxData = (AuxEntityData*)pNode2->Data;

                    if (pAuxData->Class != EntityClass_NETWORKPORT &&
                            pAuxData->Class != EntityClass_CLIENTPORT &&
                            pAuxData->Class != EntityClass_UPGRADEPORT)
                    {
                        continue;
                    }

                    X_Addr_Parse(&Addr, pAuxData->Aid);

                    if (0 == X_Addr_Equal(&Addr, &pPort->Addr))
                    {
                        pPort->Index = pAuxData->Index;
                        X_List_Destroy(pList, pNode2);
                        break;
                    }
                }
            }
        }


        X_List_Append(&pRes->Modules, pMod);
        X_List_Free(pList);

        return pMod;
    }
    else if (oper == X_RES_OPER_UPDATE)
    {

    }

    return NULL;

}

X_ModulePtr
X_LoadEntityOL(X_ResourcePtr pRes, const EntityOL *pEn, X_Resource_Oper_t oper)
{
    SNMP_ErrCode        SnmpErr;
    X_PortPtr           pPort;
    X_ModulePtr         pMod;
    X_Addr              Addr;

    assert (pRes != NULL && pEn != NULL);

    if (pRes == NULL || pEn == NULL)
    {
        return NULL;
    }

    pMod = NULL;

    if (oper == X_RES_OPER_DESTROY)
    {
        X_Addr_Parse(&Addr, pEn->Aid);
        X_Resource_DestroyModule(pRes, &Addr);
        return NULL;
    }
    else if (oper == X_RES_OPER_CREATE)
    {
        pMod = X_Module_New();

        if (pMod == NULL)
        {
            return NULL;
        }

        X_Addr_Parse(&pMod->Addr, pEn->Aid);

        pMod->ModType = X_MOD_TYPE_OL;

        pMod->Index = Entity_Get_Index(pEn);

        strncpy(pMod->Aid, pEn->Aid, sizeof(pMod->Aid));

        pPort = X_Module_AddPort(pMod, &pMod->Addr,
                X_PORT_CAT_LINE, X_PORT_SUPPORT_TRANSPARENT);

        X_List_Append(&pRes->Modules, pMod);

        X_Resource_Printf("X_LoadEntityOL: loading WCHs for %s...", pMod->Aid);

        SNMP_GetWch(pRes->Session, &SnmpErr, pMod->Index, pMod->VectorWch);

        return pMod;
    }

    return NULL;
}


int
X_LoadEntityConnection(X_ResourcePtr pRes, const EntityConnection *e, X_Resource_Oper_t oper)
{
    X_Addr From, To;
    X_PortPtr portFrom, portTo;
    char strFrom[32], strTo[32];

    assert (pRes != NULL);

    if (pRes == NULL || e == NULL)
    {
        return (-1);
    }

    X_Addr_Parse(&From, e->FromAid);

    X_Addr_Parse(&To, e->ToAid);

    portFrom = X_Resource_FindPort(pRes, &From);

    if (portFrom == NULL)
    {
        return (-1);
    }

    portTo = X_Resource_FindPort(pRes, &To);

    if (portTo == NULL)
    {
        return (-1);
    }

    X_Addr_Dump(&From, strFrom, sizeof(strFrom));

    X_Addr_Dump(&To, strTo, sizeof(strTo));

    if (oper == X_RES_OPER_DESTROY)
    {
        X_LOGINFO(X_LOG_RES_ID, "X_LoadEntityConnection: deleting connection from %s to %s type %d",
                strFrom, strTo, e->Type);

        if (e->Type == 1) // 1-way
        {
            X_Port_DeleteConn(portFrom, X_RES_DIR_OUT);
        }
        else if (e->Type == 2) // 2-way
        {
            X_Port_DeleteConn(portFrom, X_RES_DIR_OUT);
            X_Port_DeleteConn(portFrom, X_RES_DIR_IN);
        }
    }
    else if (oper == X_RES_OPER_CREATE)
    {
        X_LOGINFO(X_LOG_RES_ID, "X_LoadEntityConnection: creating connection from %s to %s type %d",
                strFrom, strTo, e->Type);

        if (e->Type == 1) // 1-way
        {
            X_Port_SetConn(portFrom,portTo,X_RES_DIR_OUT);
        }
        else if (e->Type == 2) // 2-way
        {
            X_Port_SetConn(portFrom,portTo,X_RES_DIR_OUT);
            X_Port_SetConn(portFrom,portTo,X_RES_DIR_IN);
        }
    }

    return (0);
}

#if 0
void
X_Generate_OpenFlow_Ports(X_ResourcePtr pRes, X_ListPtr pList)
{
    struct ofp_phy_port *p;
    //X_ListPtr pList;
    X_ListNodePtr pNode;
    X_ModulePtr pMod;
    int port = 1000;

    printf("sizeof port %d\n", sizeof(struct ofp_phy_port));

    unsigned int PeerIP;
    int PeerOL;

    if (pRes == NULL)
    {
        return;
    }

    //pList = X_List_New((X_ListMemFree_t)free);

    X_List_ForEach((&pRes->Modules), pNode)
    {
        pMod = (X_ModulePtr)(pNode->Data);

        // generate port description only for OLs and ADAPT modules
        if (pMod->ModType != X_MOD_TYPE_TRANSPDR && pMod->ModType != X_MOD_TYPE_OL)
        {
            continue;
        }

        p = malloc(sizeof(struct ofp_phy_port));

        if (p == NULL)
        {
            continue;
        }

        memset(p, 0, sizeof(struct ofp_phy_port));

        if (pMod->ModType == X_MOD_TYPE_TRANSPDR)
        {
            p->port_no = htons(++port);
        }
        else
        {
            p->port_no = htons(pMod->Addr.Shelf - X_RES_OL_SHELF_BASE);

            if (pRes->SNMPChan)
            {
                if (0 == FindPeer(pRes->SNMPChan->InventoryCP.TENumLinks,
                        pRes->SNMPChan->InventoryCP.TEUnnumLinks,
                        pRes->SNMPChan->Inventory.NE.SysIP,
                        pMod->Addr.Shelf - X_RES_OL_SHELF_BASE,
                        &PeerIP, &PeerOL))
                {
                    X_LOGINFO(X_LOG_RES_ID, "Found peer for OL-%d: %s / OL-%d",
                            pMod->Addr.Shelf - X_RES_OL_SHELF_BASE,
                            inet_ntoa(*((struct in_addr*)&PeerIP)),
                            PeerOL);
                    // ??? //

                    p->peer_dpid = _htonll(PeerIP);
                    p->peer_port = htons(PeerOL);
                }
            }
        }

        *(unsigned int*)(&p->hw_addr) = pMod->Index;

        strcpy((char*)p->name, pMod->Aid);

        p->config = htonl((OFPPC_NO_FLOOD | OFPPC_NO_STP | OFPPC_NO_PACKET_IN));
        p->supp_swtype = htons(OFPST_WAVE);

        // TODO:
        // assign bandwidth according to constraints bitmaps from the line port



        X_List_Append(pList, p);
    }


    X_List_ForEach(pList, pNode)
    {
        p = (struct ofp_phy_port*)(pNode->Data);

        X_Resource_Printf("Port: %d name = %s", ntohs(p->port_no), (char*)p->name);
        X_Resource_Printf("HW: %x:%x:%x:%x:%x:%x (%08x)",
                p->hw_addr[0],p->hw_addr[1],p->hw_addr[2],
                p->hw_addr[3],p->hw_addr[4],p->hw_addr[5],
                (*(unsigned int*)(p->hw_addr)));
    }

  //  X_List_Free(pList);
}
#endif

/*******************************************************************************
 *
 * Resource allocation functionality.
 *
 * EXPERIMENTAL
 *
 ******************************************************************************/

int
X_Allocate_Resource(X_ResourcePtr pRes, X_ListPtr pPathList, int freq)
{
    X_ListNodePtr       pNode;
    X_PathDataPtr       pPathData;
    X_ModulePtr         pMod;
    char                Buf[64];

    if (pRes == NULL || pPathList == NULL)
    {
        return (-1);
    }

    pNode = X_List_Head(pPathList);

    while (pNode != NULL)
    {
        pPathData = (X_PathDataPtr)(pNode->Data);

        pMod = pPathData->Port->Module;

        X_Addr_Dump(&pPathData->Port->Addr, Buf, sizeof(Buf));

        X_Resource_Printf("X_Allocate_Resource: allocating port = %s path-dir = %s dir = %s ",
                Buf,
                X_RES_DIR_TO_STR(pPathData->PathDir),
                X_RES_DIR_TO_STR(pPathData->Dir));

        /**

        pNode = pNode->Next;

        if (pNode == NULL)
            break;

        pNextPathData = (X_PathDataPtr)(pNode->Data);

        X_Module_Allocate(pRes, pPathList, pPathData, pNextPathData, freq);

        **/
        X_Module_Resource_Allocate(pRes, pPathList, pNode, freq);

       // X_Module_Power_Equalize(pRes, pPathList, pNode, freq);

        pNode = pNode->Next;
    }

    return (0);
}

int
X_Deallocate_Resources(X_ResourcePtr pRes, X_ListPtr pPathList, int freq)
{
    X_ListNodePtr       pNode;
    X_PathDataPtr       pPathData;
    X_ModulePtr         pMod;
    char                Buf[64];

    if (pRes == NULL || pPathList == NULL)
    {
        return (-1);
    }

    pNode = X_List_Head(pPathList);

    while (pNode != NULL)
    {
        pPathData = (X_PathDataPtr)(pNode->Data);

        pMod = pPathData->Port->Module;

        X_Addr_Dump(&pPathData->Port->Addr, Buf, sizeof(Buf));

        X_Resource_Printf("X_Deallocate_Resource: relasing port = %s path-dir = %s dir = %s ",
                Buf,
                X_RES_DIR_TO_STR(pPathData->PathDir),
                X_RES_DIR_TO_STR(pPathData->Dir));


        X_Module_Resource_Deallocate(pRes, pPathList, pNode, freq);

        pNode = pNode->Next;
    }

    return (0);
}

int
X_Update_Resources(X_ResourcePtr pRes, X_ListPtr pPathList, int freq, int enable)
{
    X_ListNodePtr       pNode;
    X_PathDataPtr       pPathData;
    X_ModulePtr         pMod;

    if (pRes == NULL || pPathList == NULL)
    {
        return (-1);
    }

    pNode = X_List_Tail(pPathList);

    while (pNode != NULL)
    {
        pPathData = (X_PathDataPtr)(pNode->Data);

        pMod = pPathData->Port->Module;

        if (pMod->ModType == X_MOD_TYPE_XC)
        {
            X_Module_Xc_Resource_Update(pRes, pPathList, pNode, freq, enable);
        }

        pNode = pNode->Prev;
    }

    return 0;
}

/**
 * Power equalization
 **/
int
X_Power_Equalize(X_ResourcePtr pRes, X_ListPtr pPathList, int freq)
{
    X_ListNodePtr       pNode;
    X_PathDataPtr       pPathData;
    X_ModulePtr         pMod;

    if (pRes == NULL || pPathList == NULL)
    {
        return (-1);
    }

    pNode = X_List_Tail(pPathList);

    while (pNode != NULL)
    {
        pPathData = (X_PathDataPtr)(pNode->Data);

        pMod = pPathData->Port->Module;

        X_Module_Power_Equalize(pRes, pPathList, pNode, freq);

        pNode = pNode->Prev;
    }

    return 0;
}

/*
 * Verify path allocation.
 */
int
X_Verify_Allocation(X_ResourcePtr pRes, X_ListPtr pPathList, int freq)
{
    X_ListNodePtr       pNode;
    X_PathDataPtr       pPathData;
    X_ModulePtr         pMod;
    int                 ret;

    if (pRes == NULL || pPathList == NULL)
    {
        return (-1);
    }

    ret = 0;

    pNode = X_List_Head(pPathList);

    while (pNode != NULL)
    {
        pPathData = (X_PathDataPtr)(pNode->Data);

        pMod = pPathData->Port->Module;

        ret = X_Module_Verify_Allocation(pRes, pPathList, pNode, freq);

        if (ret != 0)
        {
            X_Resource_Printf("X_Verify_Allocation: allocation verify failed on %s (freq = %d)",
                    pPathData->Port->Module->Aid, freq);
            break;
        }

        pNode = pNode->Next;
    }

    if (ret == 0)
    {
        X_Resource_Printf("X_Verify_Allocation: success!");
    }

    return ret;
}

/*
* Do resource allocation on a module.
*/
int
X_Module_Resource_Allocate(X_ResourcePtr pRes,
       X_ListPtr pPathDataList,
       X_ListNodePtr pPathDataNode, int freq)
{
    X_ModulePtr         pMod;
    X_PathDataPtr       pPathData;
    int ret = -1;

    if (pRes == NULL || pPathDataList == NULL || pPathDataNode == NULL)
    {
        return (-1);
    }

    pPathData = (X_PathDataPtr)(pPathDataNode->Data);

    if (pPathData == NULL || pPathData->Port == NULL)
    {
        return (-1);
    }

    pMod = pPathData->Port->Module;

    X_Resource_Printf("X_Module_Resource_Allocate: resource allocation for module %s, dir = %s",
            pMod->Aid, X_RES_DIR_TO_STR(pPathData->PathDir));

    switch (pMod->ModType)
    {
    case X_MOD_TYPE_XC:
        ret = X_Module_Xc_Resource_Allocate(pRes, pPathDataList, pPathDataNode, freq);
        break;
    case X_MOD_TYPE_AMP:
        ret = X_Module_Amp_Resource_Allocate(pRes, pPathDataList, pPathDataNode, freq);
        break;
    case X_MOD_TYPE_TRANSPDR:
        ret = X_Module_Adapt_Resource_Allocate(pRes, pPathDataList, pPathDataNode, freq);
        break;

#if 1
    case X_MOD_TYPE_OL:
        ret = X_Module_OL_Resource_Allocate(pRes, pPathDataList, pPathDataNode, freq);
        break;
#endif

    default:
        break;
    }
    return ret;
}

int
X_Module_Resource_Deallocate(X_ResourcePtr pRes,
       X_ListPtr pPathDataList,
       X_ListNodePtr pPathDataNode, int freq)
{
    X_ModulePtr pMod;
    X_PathDataPtr pPathData;
    int ret = -1;

    if (pRes == NULL || pPathDataList == NULL || pPathDataNode == NULL)
    {
        return (-1);
    }

    pPathData = (X_PathDataPtr)(pPathDataNode->Data);

    if (pPathData == NULL || pPathData->Port == NULL)
    {
        return (-1);
    }

    pMod = pPathData->Port->Module;

    switch (pMod->ModType)
    {
    case X_MOD_TYPE_XC:
        ret = X_Module_Xc_Resource_Deallocate(pRes, pPathDataList, pPathDataNode, freq);
        break;
    case X_MOD_TYPE_TRANSPDR:
        ret = X_Module_Adapt_Resource_Deallocate(pRes, pPathDataList, pPathDataNode, freq);
        break;
    case X_MOD_TYPE_OL:
        ret = X_Module_OL_Resource_Deallocate(pRes, pPathDataList, pPathDataNode, freq);
        break;
    default:
        break;
    }
    return ret;
}


/** Allocate resources on a cross-connect module **/
int
X_Module_Xc_Resource_Allocate(X_ResourcePtr pRes,
       X_ListPtr pPathDataList,
       X_ListNodePtr pPathDataNode,
       int freq)
{
    X_ModulePtr         pMod;
    X_PortPtr           pPort;
    X_PathDataPtr       pPathData, pPrevPathData;
    X_ListPtr           pList;
    X_ListNodePtr       pNode;
    X_Addr              Addr;
   // netsnmp_session*    pSession;
    SNMP_ErrCode        SnmpErr;
    ErrCode             Err;
    AuxEntityData*      pAuxData;
    AuxEntityData       AuxData;
    unsigned int        Idx;
    unsigned int        Tmp;
    unsigned int        Idx1 = 0, Idx2 = 0;
    char                Buf[128];
    char                Aid1[32], Aid2[32];


    pPathData = (X_PathDataPtr)(pPathDataNode->Data);
    pMod = pPathData->Port->Module;
    pPort = pPathData->Port;

    SNMP_ErrCode_Init(&SnmpErr);
    //pSession = SNMP_Channel_Duplicate_Session(pRes->SNMPChan);
    pList = X_List_New((X_ListMemFree_t)(AuxEntityData_Free));

    /*
    X_Resource_Printf("X_Module_Xc_Resource_Allocate: module %s dir %s",
            pMod->Aid, X_RES_DIR_TO_STR(pPathData->PathDir));
*/
    if (pMod->EqType == EqType_EQPEROADMDC)
    {
        Idx = 0;

#if 0
        Err = SNMP_Contains(pRes->Session, &SnmpErr, pMod->Index, pList);

        // 1. allocate OM or CH
        X_List_ForEach(pList, pNode)
        {
            pAuxData = (AuxEntityData*)(pNode->Data);

            if (pAuxData->Class != EntityClass_UPGRADEPORT &&
                    pAuxData->Class != EntityClass_NETWORKPORT &&
                    pAuxData->Class != EntityClass_CLIENTPORT)
            {
                continue;
            }

            X_Addr_Parse(&Addr, pAuxData->Aid);

            if (0 != X_Addr_Equal(&Addr, &pPort->Addr))
                continue;

            if (pPort == pMod->NetPort || pPort == pMod->UpgradePort)
            {
                // allocate OM
                if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
                {
                    X_Resource_Printf("X_Module_Xc_Resource_Allocate: creating %s",
                            pAuxData->Aid);

                    Err = SNMP_Create_OM_Facility(pRes->Session, &SnmpErr, pAuxData->Index);

                    if (Err != ERRCODE_NOERROR)
                    {
                        X_Resource_Printf("X_Module_Xc_Resource_Allocate: failed to create %s",
                                pAuxData->Aid);
                    }
                }

                if (pPort == pMod->NetPort)// || pMod->UpgradePort)
                    Idx = pAuxData->Index;
            }
            else
            {
                if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
                {
                    X_Resource_Printf("X_Module_Xc_Resource_Allocate: creating %s", pAuxData->Aid);

                    Err = SNMP_Create_CH_Facility(pRes->Session, &SnmpErr,
                            pAuxData->Index, FacilityType_IFTYPEOCH, 0);

                    if (Err != ERRCODE_NOERROR)
                    {
                        X_Resource_Printf("X_Module_Xc_Resource_Allocate: failed to create %s",
                                pAuxData->Aid);
                    }
                }

                strcpy(Aid1, pAuxData->Aid);

                Idx1 = pAuxData->Index;
            }
            break;
        }

        X_List_Clear(pList);
#endif

        pAuxData = &AuxData;
        AuxEntityData_Init(pAuxData);
        pAuxData->Index = pPort->Index;

        Err = SNMP_GetAuxData(pRes->Session, &SnmpErr, pAuxData);

        if (pPort == pMod->NetPort || pPort == pMod->UpgradePort)
        {
            // allocate OM
            if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
            {
                X_Resource_Printf("X_Module_Xc_Resource_Allocate: creating %s",
                        pAuxData->Aid);

                Err = SNMP_Create_OM_Facility(pRes->Session, &SnmpErr, pAuxData->Index);

                if (Err != ERRCODE_NOERROR)
                {
                    X_Resource_Printf("X_Module_Xc_Resource_Allocate: failed to create %s",
                            pAuxData->Aid);
                }
            }
            if (pPort == pMod->NetPort)// || pMod->UpgradePort)
                Idx = pAuxData->Index;
        }
        else
        {
            if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
            {
                X_Resource_Printf("X_Module_Xc_Resource_Allocate: creating %s", pAuxData->Aid);
                Err = SNMP_Create_CH_Facility(pRes->Session, &SnmpErr,
                        pAuxData->Index, FacilityType_IFTYPEOCH, 0);

                if (Err != ERRCODE_NOERROR)
                {
                    X_Resource_Printf("X_Module_Xc_Resource_Allocate: failed to create %s",
                            pAuxData->Aid);
                }
            }
            strcpy(Aid1, pAuxData->Aid);
            Idx1 = pAuxData->Index;
        }

        // 2. Allocate VCH on N, Idx points to OM
        if (Idx != 0)
        {
            Tmp = 0;
            Err = SNMP_Contains(pRes->Session, &SnmpErr, Idx, pList);

            X_List_ForEach(pList, pNode)
            {
                pAuxData = (AuxEntityData*)(pNode->Data);

                if (pAuxData->Class != EntityClass_VIRTUALCHANNEL)
                    continue;

                X_Addr_Parse(&Addr, pAuxData->Aid);

                if (1 != sscanf(&Addr.Slot[1], "%d", &Tmp))
                    continue;

                if (X_RES_C_BAND_MAX_FREQ - Tmp*X_RES_100GHz != freq)
                    continue;

                if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
                {
                    X_Resource_Printf("X_Module_Xc_Resource_Allocate: creating %s",
                            pAuxData->Aid);

                    Err = SNMP_Create_VCH_Facility(pRes->Session, &SnmpErr, pAuxData->Index);
                }

                strcpy(Aid1, pAuxData->Aid);
                Idx1 = pAuxData->Index;
                break;
            }
        }

        X_List_Clear(pList);

        if (Idx1 != 0)
        {

        // 3. Check if to make an internal cross-connect
        // Idx1 stores VCH or CH, need to find Idx2

            pNode = pPathDataNode->Prev;

            while (pNode != NULL)
            {
                pPrevPathData = (X_PathDataPtr)(pNode->Data);

                if (pPrevPathData->Port != NULL && pPrevPathData->Port->Module == pMod)
                {
                    // if both ports on the same EROADM are != U port then make CH<>VCH cross
                    if (pPort != pMod->UpgradePort && pPrevPathData->Port != pMod->UpgradePort)
                        break;
                }
                else if (pPrevPathData->Port != NULL && pPrevPathData->Port->Module->EqType == EqType_EQPEROADMDC)
                {
                    // if other EROADM then make VCH<>VCH cross.
                    if (pPrevPathData->Port == pPrevPathData->Port->Module->NetPort)
                        break;
                }

                pNode = pNode->Prev;
            }

            if (pNode != NULL)
            {
                pPrevPathData = (X_PathDataPtr)(pNode->Data);

//            if (pPrevPathData->Port != NULL && pPrevPathData->Port->Module == pMod)

                // do the CH <> VCH cross connection

                // find the Idx2
                Err = SNMP_Contains(pRes->Session, &SnmpErr, pPrevPathData->Port->Module->Index, pList);

                Idx = 0;

                X_List_ForEach(pList, pNode)
                {
                    pAuxData = (AuxEntityData*)(pNode->Data);

                    if (pAuxData->Class != EntityClass_NETWORKPORT &&
                            pAuxData->Class != EntityClass_CLIENTPORT)
                        continue;

                    X_Addr_Parse(&Addr, pAuxData->Aid);

                    X_Addr_Dump(&pPrevPathData->Port->Addr,Buf,sizeof(Buf));

                    X_Resource_Printf("Addr = %s AID = %s", Buf, pAuxData->Aid);

                    if (0 != X_Addr_Equal(&Addr, &pPrevPathData->Port->Addr))
                        continue;

                    if (pPrevPathData->Port == pPrevPathData->Port->Module->NetPort)
                    {
                        Idx = pAuxData->Index;
                    }
                    else
                    {
                        strcpy(Aid2, pAuxData->Aid);
                        Idx2 = pAuxData->Index;
                    }
                    break;
                }

                X_List_Clear(pList);

                // find the VCH
                if (Idx != 0)
                {
                    Err = SNMP_Contains(pRes->Session, &SnmpErr, Idx, pList);
                    X_List_ForEach(pList, pNode)
                    {
                        pAuxData = (AuxEntityData*)(pNode->Data);
                        X_Addr_Parse(&Addr, pAuxData->Aid);
                        if (1 != sscanf(&Addr.Slot[1], "%d", &Tmp))
                            continue;

                        if (X_RES_C_BAND_MAX_FREQ - Tmp*X_RES_100GHz != freq)
                            continue;

                        strcpy(Aid2, pAuxData->Aid);
                        Idx2 = pAuxData->Index;
                        break;
                    }
                }

                // now should have Idx1 and Idx2, do the cross-connect

                if (Idx1 != 0 && Idx2 != 0)
                {
                    if (pPathData->PathDir == X_RES_DIR_IN)
                    {
                        X_Resource_Printf("Making CRS_CH %s -> %s", Aid1, Aid2);
                        Err = SNMP_Create_Crs(pRes->Session, &SnmpErr, Idx1, Idx2, 0, 0);
                        //Err = SNMP_GetCrsAdminCap(pSession, &SnmpErr, Idx1, Idx2, &AdminCaps);
                    }
                    else
                    {
                        X_Resource_Printf("Making CRS_CH %s -> %s", Aid2, Aid1);
                        Err = SNMP_Create_Crs(pRes->Session, &SnmpErr, Idx2, Idx1, 0, 0);
                        //Err = SNMP_GetCrsAdminCap(pSession, &SnmpErr, Idx2, Idx1, &AdminCaps);
                    }
                }

        }

        }
    }
    else if (pMod->EqType == EqType_EQP8ROADMC40)
    {
        Idx = 0;
#if 0
        Err = SNMP_Contains(pRes->Session, &SnmpErr, pMod->Index, pList);

        // 1. allocate OM
        X_List_ForEach(pList, pNode)
        {
            pAuxData = (AuxEntityData*)(pNode->Data);

            if (pAuxData->Class != EntityClass_CLIENTPORT &&
                    pAuxData->Class != EntityClass_NETWORKPORT)
            {
                continue;
            }

            X_Addr_Parse(&Addr, pAuxData->Aid);

            if (0 != X_Addr_Equal(&Addr, &pPort->Addr))
                continue;

            if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
            {
                X_Resource_Printf("X_Module_Xc_Resource_Allocate: creating %s",
                        pAuxData->Aid);

                Err = SNMP_Create_OM_Facility(pRes->Session, &SnmpErr, pAuxData->Index);

                if (Err != ERRCODE_NOERROR)
                {
                    X_Resource_Printf("X_Module_Xc_Resource_Allocate: failed to create %s",
                            pAuxData->Aid);
                }
            }

            if ( 1 != sscanf(&pPort->Addr.Port[1], "%d", &Tmp) )
            {
                Tmp = 0;
            }

            if (pMod->NetPort == pPort || Tmp == pMod->RoadmNo)
                Idx = pAuxData->Index;

            break;
        }
#endif

        pAuxData = &AuxData;

        AuxEntityData_Init(pAuxData);

        pAuxData->Index = pPort->Index;

        Err = SNMP_GetAuxData(pRes->Session, &SnmpErr, pAuxData);

        if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
        {
            X_Resource_Printf("X_Module_Xc_Resource_Allocate: creating %s",
                    pAuxData->Aid);

            Err = SNMP_Create_OM_Facility(pRes->Session, &SnmpErr, pAuxData->Index);
        }

        Idx = pAuxData->Index;

        X_List_Clear(pList);

        // 2. allocate VCH
        if (Idx != 0)
        {
            Tmp = 0;

            Err = SNMP_Contains(pRes->Session, &SnmpErr, Idx, pList);

            X_List_ForEach(pList, pNode)
            {
                pAuxData = (AuxEntityData*)(pNode->Data);

                if (pAuxData->Class != EntityClass_VIRTUALCHANNEL)
                    continue;

                X_Addr_Parse(&Addr, pAuxData->Aid);

                if (1 != sscanf(&Addr.Port[1], "%d", &Tmp))
                    continue;

                if (X_RES_C_BAND_MAX_FREQ - Tmp*X_RES_100GHz != freq)
                    continue;

                if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
                {
                    X_Resource_Printf("X_Module_Xc_Resource_Allocate: creating %s", pAuxData->Aid);

                    Err = SNMP_Create_VCH_Facility(pRes->Session, &SnmpErr, pAuxData->Index);
                }

                strcpy(Aid1, pAuxData->Aid);
                Idx1 = pAuxData->Index;
                break;
            }
        }

        X_List_Clear(pList);

        if (Idx1 != 0)
        {
            pNode = pPathDataNode->Prev;

            while (pNode != NULL)
            {
                pPrevPathData = (X_PathDataPtr)(pNode->Data);

                if (pPrevPathData->Port != NULL && pPrevPathData->Port->Module == pMod)
                {
                    // if both ports on the same EROADM are != U port then make VCH<>VCH cross
                    //if (pMod->RoadmNo)
                    //break;
                    if (pPort == pMod->NetPort && (1 == sscanf(&pPrevPathData->Port->Addr.Port[1], "%d", &Tmp)) && Tmp == pMod->RoadmNo)
                        break;

                }
                else if (pPrevPathData->Port != NULL &&
                        pPrevPathData->Port->Module->EqType == EqType_EQP8ROADMC40)
                {
                    // VCH-VCH cross
                    if (pPrevPathData->Port == pPrevPathData->Port->Module->NetPort)
                        break;
                }

                pNode = pNode->Prev;
            }

            if (pNode != NULL)
            {
                pPrevPathData = (X_PathDataPtr)(pNode->Data);

                // find the Idx2
                //pAuxData = &AuxData;
                //AuxEntityData_Init(pAuxData);

                Idx = pPrevPathData->Port->Index;

#if 0
                Err = SNMP_Contains(pRes->Session, &SnmpErr, pPrevPathData->Port->Module->Index, pList);

                X_List_ForEach(pList, pNode)
                {
                    pAuxData = (AuxEntityData*)(pNode->Data);

                    if (pAuxData->Class != EntityClass_NETWORKPORT &&
                            pAuxData->Class != EntityClass_CLIENTPORT)
                        continue;

                    X_Addr_Parse(&Addr, pAuxData->Aid);

                    X_Addr_Dump(&pPrevPathData->Port->Addr,Buf,sizeof(Buf));

                    //X_Resource_Printf("Addr = %s AID = %s", Buf, pAuxData->Aid);

                    if (0 != X_Addr_Equal(&Addr, &pPrevPathData->Port->Addr))
                        continue;

                    Idx = pAuxData->Index;

                    break;
                }

                X_List_Clear(pList);
#endif

                // find the VCH
                if (Idx != 0)
                {
                    Err = SNMP_Contains(pRes->Session, &SnmpErr, Idx, pList);

                    X_List_ForEach(pList, pNode)
                    {
                        pAuxData = (AuxEntityData*)(pNode->Data);
                        X_Addr_Parse(&Addr, pAuxData->Aid);

                        X_Addr_Dump(&Addr,Buf,sizeof(Buf));

                        //X_Resource_Printf("Addr = %s AID = %s", Buf, pAuxData->Aid);

                        if (1 != sscanf(&Addr.Port[1], "%d", &Tmp))
                            continue;

                        if (X_RES_C_BAND_MAX_FREQ - Tmp*X_RES_100GHz != freq)
                            continue;

                        Idx2 = pAuxData->Index;
                        strcpy(Aid2, pAuxData->Aid);
                        break;
                    }
                }

                if (Idx1 != 0 && Idx2 != 0)
                {
                    if (pPathData->PathDir == X_RES_DIR_IN)
                    {
                        X_Resource_Printf("Making CRS_CH %s -> %s", Aid1, Aid2);
                        Err = SNMP_Create_Crs(pRes->Session, &SnmpErr, Idx1, Idx2, FacilityType_IFTYPEOTU2, 1);
                    }
                    else
                    {
                        X_Resource_Printf("Making CRS_CH %s -> %s", Aid2, Aid1);
                        Err = SNMP_Create_Crs(pRes->Session, &SnmpErr, Idx2, Idx1, FacilityType_IFTYPEOTU2, 1);
                    }
                }
            }
        }
    }
    else if (pMod->EqType == EqType_EQPCCM8)
    {
        Idx = 0;

        Err = SNMP_Contains(pRes->Session, &SnmpErr, pMod->Index, pList);

        X_List_ForEach(pList, pNode)
        {
            pAuxData = (AuxEntityData*)(pNode->Data);

            if (pAuxData->Class != EntityClass_CLIENTPORT &&
                    pAuxData->Class != EntityClass_NETWORKPORT)
                continue;

            X_Addr_Parse(&Addr, pAuxData->Aid);

            if (0 != X_Addr_Equal(&Addr, &pPort->Addr))
                continue;

            if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
            {
                X_Resource_Printf("X_Module_Xc_Resource_Allocate: creating %s", pAuxData->Aid);

                if (pMod->NetPort == pPort)
                {
                    Err = SNMP_Create_OM_Facility(pRes->Session, &SnmpErr, pAuxData->Index);
                }
                else
                {
                    Err = SNMP_Create_CH_Facility(pRes->Session, &SnmpErr, pAuxData->Index,
                            FacilityType_IFTYPEOCH,
                            Freq_To_ADVA_Channel(freq));
                }
            }

            break;
        }
        X_List_Clear(pList);
    }

    //SNMP_Channel_Destroy_Session(pRes->SNMPChan, pSession);

    X_List_Free(pList);

    return 0;
}


int
X_Module_Amp_Resource_Allocate(X_ResourcePtr pRes,
        X_ListPtr pPathDataList,
        X_ListNodePtr pPathDataNode,
        int freq)
{
    X_ModulePtr         pMod;
    X_PortPtr           pPort;
    X_PathDataPtr       pPathData;
    X_ListPtr           pList;
    X_ListNodePtr       pNode;
    X_Addr              Addr;
    netsnmp_session*    pSession;
    SNMP_ErrCode        SnmpErr;
    ErrCode             Err;
    AuxEntityData*      pAuxData;
    unsigned int        Idx;

    pPathData = (X_PathDataPtr)(pPathDataNode->Data);
    pMod = pPathData->Port->Module;
    pPort = pPathData->Port;

    pSession = SNMP_Channel_Duplicate_Session(pRes->SNMPChan);
    pList = X_List_New((X_ListMemFree_t)(AuxEntityData_Free));

    Idx = 0;

    Err = SNMP_Contains(pSession, &SnmpErr, pMod->Index, pList);

    X_List_ForEach(pList, pNode)
    {
        pAuxData = (AuxEntityData*)(pNode->Data);
        X_Addr_Parse(&Addr, pAuxData->Aid);
        if (X_Addr_Equal(&Addr, &pPort->Addr))
            continue;

        Idx = pAuxData->Index;
        break;
    }

    if (Idx != 0)
    {
        Err = SNMP_Create_OM_Facility(pSession, &SnmpErr, Idx);
    }

    X_List_Free(pList);

    SNMP_Channel_Destroy_Session(pRes->SNMPChan, pSession);

    return 0;
}

int
X_Module_OL_Resource_Allocate(X_ResourcePtr pRes,
        X_ListPtr pPathDataList,
        X_ListNodePtr pPathDataNode,
        int freq)
{
    X_ModulePtr         pMod;
    X_PortPtr           pPort;
    X_PathDataPtr       pPathData, pNextPathData;
    X_ListPtr           pList;
    X_ListNodePtr       pNode;
    X_Addr              Addr;
    //netsnmp_session*    pSession;
    SNMP_ErrCode        SnmpErr;
    ErrCode             Err;
    AuxEntityData*      pAuxData;
    unsigned int        Idx;
    unsigned int        Idx1;
    int                 i;
    unsigned int        FacType;

    pPathData = (X_PathDataPtr)(pPathDataNode->Data);
    pMod = pPathData->Port->Module;
    pPort = pPathData->Port;

    //pSession = SNMP_Channel_Duplicate_Session(pRes->SNMPChan);


    Idx = 0;
//    Err = SNMP_Contains(pSession, &SnmpErr, pMod->Index, pList);

    /*
    i = 0;
    X_List_ForEach(pList, pNode)
    {
        pAuxData = (AuxEntityData*)(pNode->Data);

        if (pAuxData->Class != EntityClass_VIRTUALOPTICALCHANNEL)
            continue;

        i += 1;

        if (freq !=  ADVA_Channel_To_Freq((Channel_t)i))
            continue;

        Idx = pAuxData->Index;
        break;
    }
    */

    // find the WCH index

    i = Freq_To_ADVA_Channel(freq) - 1;

    if (i < 0)
    {
        return -1;
    }

    pAuxData = (AuxEntityData*)X_Vector_At(pMod->VectorWch, i);

    Idx = pAuxData->Index;

    pNextPathData = NULL;

    if (pPathDataNode == X_List_Head(pPathDataList))
    {
        pNextPathData = (X_PathDataPtr)(X_List_Tail(pPathDataList)->Data);
    }
    else
    {
        pNextPathData = (X_PathDataPtr)(X_List_Head(pPathDataList)->Data);
    }

    pList = X_List_New((X_ListMemFree_t)(AuxEntityData_Free));

    X_Resource_Printf("X_Module_OL_Resource_Allocate: creating %s", pAuxData->Aid);

    Err = SNMP_Create_WCH(pRes->Session, &SnmpErr, Idx);

    if (pNextPathData != NULL)
    {
        if (pNextPathData->Port->Module->ModType == X_MOD_TYPE_OL)
        {
            //FacType = 0;
            pAuxData = (AuxEntityData*)X_Vector_At(pNextPathData->Port->Module->VectorWch, i);
            Idx1 = pAuxData->Index;

            if (Idx1 != 0)
            {
                X_Resource_Printf("creating %s", pAuxData->Aid);
                Err = SNMP_Create_WCH(pRes->Session, &SnmpErr, Idx1);
            }
        }
        else if (pNextPathData->Port->Module->ModType == X_MOD_TYPE_TRANSPDR)
        {
#if 0
            Err = SNMP_Contains(pRes->Session, &SnmpErr, pNextPathData->Port->Module->Index, pList);

            X_List_ForEach(pList, pNode)
            {
                pAuxData = (AuxEntityData*)(pNode->Data);

                if (pAuxData->Class != EntityClass_NETWORKPORT)
                    continue;

                X_Addr_Parse(&Addr, pAuxData->Aid);

                if (X_Addr_Equal(&Addr, &pNextPathData->Port->Addr))
                    continue;

                Idx1 = pAuxData->Index;
                break;
            }
#endif
            if (pNextPathData->Port->Module->EqType == EXTERNAL_CHANNEL)
            {
                Idx1 = pNextPathData->Port->Module->Index;
                FacType = FacilityType_IFTYPEOTU2;
            }
            else
            {
                Idx1 = pNextPathData->Port->Index;
                FacType = 0;
            }
        }

        if (Idx1 != 0)
        {

            //if (pPathData->Dir == X_RES_DIR_IN)
            SNMP_Create_Crs(pRes->Session, &SnmpErr, Idx, Idx1, 0, 0 );
            //else
            SNMP_Create_Crs(pRes->Session, &SnmpErr, Idx1, Idx, 0, 0 );
        }
    }

    return 0;
}

int
X_Module_Adapt_Resource_Allocate(X_ResourcePtr pRes,
        X_ListPtr pPathDataList,
        X_ListNodePtr pPathDataNode,
        int freq)
{
    X_ModulePtr         pMod;
    X_PortPtr           pPort;
    X_PathDataPtr       pPathData, pPrevPathData;
    X_ListPtr           pList;
    X_ListNodePtr       pNode;
    X_Addr              Addr;
    netsnmp_session*    pSession;
    SNMP_ErrCode        SnmpErr;
    ErrCode             Err;
    AuxEntityData*      pAuxData;
    unsigned int        Idx;
    unsigned int        Idx1;
    unsigned int        Tmp;
    int                 i;
    unsigned int        FacType = 0;
    //char                Buf[64];

    pPathData = (X_PathDataPtr)(pPathDataNode->Data);
    pMod = pPathData->Port->Module;
    pPort = pPathData->Port;

    pSession = pRes->Session; // SNMP_Channel_Duplicate_Session(pRes->SNMPChan);

    i = Freq_To_ADVA_Channel(freq) - 1;

    if (i < 0)
    {
        return -1;
    }

    // no prov. on ECH is required
    if (pMod->EqType != EXTERNAL_CHANNEL)
    {
#if 0
        pList = X_List_New((X_ListMemFree_t) (AuxEntityData_Free));

        Idx = 0;

        // find a matching CH
        Err = SNMP_Contains(pSession,&SnmpErr,pMod->Index,pList);

        X_List_ForEach(pList, pNode)
        {
            pAuxData = (AuxEntityData*) (pNode->Data);

            if (pAuxData->Class != EntityClass_NETWORKPORT)
                continue;

            if (0 != X_Addr_Parse(&Addr,pAuxData->Aid))
                continue;

            Idx = pAuxData->Index;
            break;
        }

        if (Idx == 0)
        {
            X_Resource_Printf("No CH entity was found!!!");
            return -1;
        }

        Tmp = (pMod->EqType == EqType_EQPWCC10GD || pMod->EqType == EqType_EQPWCA10GD) ? 0 : Freq_To_ADVA_Channel(freq);


        Err = SNMP_Create_CH_Facility(pSession,&SnmpErr, Idx, (Tmp == 0) ? 0 : FacilityType_IFTYPEOTU2, Tmp);

        X_List_Free(pList);
#endif
        Idx = pPort->Index;
        Tmp = (pMod->EqType == EqType_EQPWCC10GD || pMod->EqType == EqType_EQPWCA10GD) ? 0 : Freq_To_ADVA_Channel(freq);
        Err = SNMP_Create_CH_Facility(pSession,&SnmpErr, Idx, (Tmp == 0) ? 0 : FacilityType_IFTYPEOTU2, Tmp);
    }
    else
    {
        FacType = FacilityType_IFTYPEOTU2;
        Idx = pMod->Index;
    }

    if (pPathDataNode == X_List_Tail(pPathDataList))
    {
        pPrevPathData = (X_PathDataPtr)X_List_Head(pPathDataList)->Data;

        if (pPrevPathData->Port->Module->ModType == X_MOD_TYPE_OL)
        {
            /*
            for(i = 0; i < pPrevPathData->Port->Module->VectorWch->Size; ++i)
            {
                pAuxData = (AuxEntityData*)X_Vector_At(pPrevPathData->Port->Module->VectorWch, i);

                X_Resource_Printf("%s", pAuxData->Aid);

                if (freq !=  ADVA_Channel_To_Freq((Channel_t)i))
                    continue;

                Idx1 = pAuxData->Index;
                break;
            }
            */
            pAuxData = (AuxEntityData*)X_Vector_At(pPrevPathData->Port->Module->VectorWch, i);
            Idx1 = pAuxData->Index;

            if (Idx1 != 0)
            {
                Err = SNMP_Create_Crs(pSession, &SnmpErr, Idx, Idx1, 0, 0);
                Err = SNMP_Create_Crs(pSession, &SnmpErr, Idx1, Idx, 0, 0);
            }
        }
    }


  //  SNMP_Channel_Destroy_Session(pRes->SNMPChan, pSession);

    return 0;
}

/** Allocate resources on a cross-connect module **/
int
X_Module_Xc_Resource_Deallocate(X_ResourcePtr pRes,
        X_ListPtr pPathDataList,
        X_ListNodePtr pPathDataNode,
        int freq)
{
    X_ModulePtr         pMod;
    X_PortPtr           pPort;
    X_PathDataPtr       pPathData;
    X_ListPtr           pList;
    X_ListNodePtr       pNode;
    X_Addr              Addr;
    //netsnmp_session*    pSession;
    SNMP_ErrCode        SnmpErr;
    ErrCode             Err;
    AuxEntityData*      pAuxData;
    //AuxEntityData       AuxData;
    unsigned int        Idx;
    unsigned int        Tmp;
    //char                Buf[64];

    pPathData = (X_PathDataPtr)(pPathDataNode->Data);
    pMod = pPathData->Port->Module;
    pPort = pPathData->Port;

//    pSession = SNMP_Channel_Duplicate_Session(pRes->SNMPChan);
    pList = X_List_New((X_ListMemFree_t)(AuxEntityData_Free));

    if (pMod->EqType == EqType_EQPEROADMDC)
    {
        // destroy any VCH or CH cross-connects
        if (pPort != pMod->UpgradePort)
        {
            Idx = pPort->Index;
            Tmp = 0;

#if 0
            X_Addr_Dump(&pPort->Addr, Buf, sizeof(Buf));

            Err = SNMP_Contains(pRes->Session, &SnmpErr, pMod->Index, pList);

            X_List_ForEach(pList, pNode)
            {
                pAuxData = (AuxEntityData*)(pNode->Data);

                if (pAuxData->Class != EntityClass_CLIENTPORT &&
                        pAuxData->Class != EntityClass_NETWORKPORT)
                    continue;

                X_Addr_Parse(&Addr, pAuxData->Aid);

                if (0 != X_Addr_Equal(&Addr, &pPort->Addr))
                    continue;

                Idx = pAuxData->Index;
                break;
            }

            X_List_Clear(pList);
#endif

            // fetch VCH
            if (Idx != 0 && pPort == pMod->NetPort)
            {
                Err = SNMP_Contains(pRes->Session, &SnmpErr, Idx, pList);

                X_List_ForEach(pList, pNode)
                {
                    pAuxData = (AuxEntityData*) (pNode->Data);

                    if (pAuxData->Class != EntityClass_VIRTUALCHANNEL)
                        continue;

                    X_Addr_Parse(&Addr,pAuxData->Aid);

                    X_Resource_Printf(pAuxData->Aid);

                    if (1 != sscanf(&Addr.Slot[1], "%d", &Tmp))
                        continue;

                    if (X_RES_C_BAND_MAX_FREQ - Tmp*X_RES_100GHz != freq)
                        continue;

                    if (pAuxData->Assignment == EntityAssignmentState_ASSIGNED)
                        Tmp = pAuxData->Index;

                    break;
                }

                if (Tmp != 0)
                {
                    SNMP_Destroy_Crs_From(pRes->Session, &SnmpErr, Tmp);
                    SNMP_Destroy_VCH(pRes->Session, &SnmpErr, Tmp);
                }
            }
            else if (Idx != 0 && pPort != pMod->NetPort)
            {
                SNMP_Destroy_Crs_From(pRes->Session, &SnmpErr, Idx);
                SNMP_Destroy_If(pRes->Session, &SnmpErr, Idx);
            }

            X_List_Clear(pList);
        }
      }
      else if (pMod->EqType == EqType_EQP8ROADMC40)
      {
          //if (pPort == pMod->NetPort)
          {
              Idx = pPort->Index;
              Tmp = 0;

#if 0
              X_Addr_Dump(&pPort->Addr, Buf, sizeof(Buf));

              Err = SNMP_Contains(pRes->Session, &SnmpErr, pMod->Index, pList);

              X_List_ForEach(pList, pNode)
              {
                  pAuxData = (AuxEntityData*)(pNode->Data);

                  if (pAuxData->Class != EntityClass_CLIENTPORT ||
                          pAuxData->Class != EntityClass_NETWORKPORT)

                  X_Addr_Parse(&Addr, pAuxData->Aid);

                  if (0 != X_Addr_Equal(&Addr, &pPort->Addr))
                      continue;

                  Idx = pAuxData->Index;
                  break;
              }

              X_List_Clear(pList);
#endif

              Err = SNMP_Contains(pRes->Session, &SnmpErr, Idx, pList);

              X_List_ForEach(pList, pNode)
              {
                  pAuxData = (AuxEntityData*)(pNode->Data);

                  if (pAuxData->Class != EntityClass_VIRTUALCHANNEL)
                      continue;

                  X_Addr_Parse(&Addr, pAuxData->Aid);

                  if(1 != sscanf(&Addr.Port[1],"%d",&Tmp))
                      continue;

                  if (X_RES_C_BAND_MAX_FREQ - Tmp*X_RES_100GHz != freq)
                      continue;

                  if (pAuxData->Assignment == EntityAssignmentState_ASSIGNED)
                      Tmp = pAuxData->Index;
                  break;
              }

              if (Tmp != 0)
              {
                  SNMP_Destroy_Crs_From(pRes->Session, &SnmpErr, Tmp);
                  SNMP_Destroy_VCH(pRes->Session, &SnmpErr, Tmp);
              }
          }
      }
      else if (pMod->EqType == EqType_EQPCCM8)
      {
          if (pPort != pMod->NetPort)
          {
#if 1
              Err = SNMP_Contains(pRes->Session, &SnmpErr, pMod->Index, pList);

              X_List_ForEach(pList, pNode)
              {
                  pAuxData = (AuxEntityData*)(pNode->Data);

                  X_Resource_Printf("%s %d", pAuxData->Aid, pAuxData->Class);

                  if (pAuxData->Class != EntityClass_CLIENTPORT &&
                          pAuxData->Class != EntityClass_NETWORKPORT)
                      continue;

                  X_Addr_Parse(&Addr, pAuxData->Aid);

                  if (0 != X_Addr_Equal(&Addr, &pPort->Addr))
                      continue;

                  if (pAuxData->Assignment == EntityAssignmentState_ASSIGNED)
                  {
                      X_Resource_Printf("Destroying %s", pAuxData->Aid);
                      SNMP_Destroy_If(pRes->Session, &SnmpErr, pAuxData->Index);
                  }
                  break;
              }
              X_List_Clear(pList);
#endif
          }
      }

    X_List_Free(pList);

    //SNMP_Channel_Destroy_Session(pRes->SNMPChan, pSession);
    return 0;
}



int
X_Module_Adapt_Resource_Deallocate(X_ResourcePtr pRes,
        X_ListPtr pPathDataList,
        X_ListNodePtr pPathDataNode,
        int freq)
{
    X_ModulePtr         pMod;
    X_PortPtr           pPort;
    X_PathDataPtr       pPathData;
    X_ListPtr           pList;
    X_ListNodePtr       pNode;
    X_Addr              Addr;
    netsnmp_session*    pSession;
    SNMP_ErrCode        SnmpErr;
    ErrCode             Err;
    AuxEntityData*      pAuxData;
    unsigned int        Idx;
    unsigned int        Idx1;
   // unsigned int        Tmp;
   // char                Buf[64];

    pPathData = (X_PathDataPtr)(pPathDataNode->Data);
    pMod = pPathData->Port->Module;
    pPort = pPathData->Port;

    // no prov. on ECH is required

    if (pMod->EqType != EqType_EQPWCC10GD && pMod->EqType != EqType_EQPWCC10GTD)
    {
        X_Resource_Printf("Unsupported adatp. module type !!!");
       // return -1;
    }

    pSession = SNMP_Channel_Duplicate_Session(pRes->SNMPChan);
    pList = X_List_New((X_ListMemFree_t)(AuxEntityData_Free));


    Idx = 0;

    if (pMod->EqType != EXTERNAL_CHANNEL)
    {
        // find a matching CH
        Err = SNMP_Contains(pSession, &SnmpErr, pMod->Index, pList);

        X_List_ForEach(pList, pNode)
        {
            pAuxData = (AuxEntityData*)(pNode->Data);

            if (pAuxData->Class != EntityClass_NETWORKPORT)
                continue;

            X_Resource_Printf("%s", pAuxData->Aid);

            if (0 != X_Addr_Parse(&Addr, pAuxData->Aid))
                continue;

            Idx = pAuxData->Index;
            break;
        }
    }
    else
    {
        Idx = pMod->Index;
    }

    Err = SNMP_Get_Crs(pSession, &SnmpErr, Idx, &Idx1);

    Err = SNMP_Destroy_Crs_From(pSession, &SnmpErr, Idx);


    if (pMod->EqType != EXTERNAL_CHANNEL)
    {
        Err = SNMP_Destroy_If(pSession, &SnmpErr, Idx);
    }

    X_List_Free(pList);

    SNMP_Channel_Destroy_Session(pRes->SNMPChan,pSession);

    return 0;
}

int
X_Module_OL_Resource_Deallocate(X_ResourcePtr pRes,
        X_ListPtr pPathDataList,
        X_ListNodePtr pPathDataNode,
        int freq)
{
    X_ModulePtr         pMod;
    X_PortPtr           pPort;
    X_PathDataPtr       pPathData, pPrevPathData;
    //X_ListNodePtr       pNode;
    netsnmp_session*    pSession;
    SNMP_ErrCode        SnmpErr;
    ErrCode             Err;
    AuxEntityData*      pAuxData;
    unsigned int        Idx;
    unsigned int        Idx1;
    int                 i;

    pPathData = (X_PathDataPtr)(pPathDataNode->Data);
    pMod = pPathData->Port->Module;
    pPort = pPathData->Port;

    i = Freq_To_ADVA_Channel(freq) - 1;

    if (i < 0)
    {
        return -1;
    }

    pSession = pRes->Session; //SNMP_Channel_Duplicate_Session(pRes->SNMPChan);

    Idx = 0;

    pAuxData = (AuxEntityData*)X_Vector_At(pMod->VectorWch, i);

    if (pAuxData == NULL)
    {
        return -1;
    }

    Idx = pAuxData->Index;

    Err = SNMP_Get_Crs(pSession, &SnmpErr, Idx, &Idx1);

    if (Err != ERRCODE_NOERROR)
    {
        //X_Resource_Printf("X_Module_OL_Resource_Deallocate: destroying CRS_CH");
    }

    if (Idx1 != 0)
    {
        X_Resource_Printf("X_Module_OL_Resource_Deallocate: destroying CRS_CH");
        Err = SNMP_Destroy_Crs_From(pSession, &SnmpErr, Idx);
    }

    X_Resource_Printf("X_Module_OL_Resource_Deallocate: destroying %s",
            pAuxData->Aid);

    Err = SNMP_Destroy_WCH(pSession, &SnmpErr, Idx);

  //  SNMP_Channel_Destroy_Session(pRes->SNMPChan, pSession);

    return 0;
}


int
X_Module_Xc_Resource_Update(X_ResourcePtr pRes,
        X_ListPtr pPathDataList,
        X_ListNodePtr pPathDataNode,
        int Freq, int Enable)
{
    X_ModulePtr         pMod;
    X_PortPtr           pPort;
    X_PathDataPtr       pPathData; //, pPrevPathData;
    X_ListPtr           pList;
    X_ListNodePtr       pNode;
    X_Addr              Addr;
    netsnmp_session*    pSession;
    SNMP_ErrCode        SnmpErr;
    ErrCode             Err;
    AuxEntityData*      pAuxData;
    unsigned int        Idx;
    unsigned int        Tmp;
    unsigned int        Idx1 = 0, Idx2 = 0;

    pPathData = (X_PathDataPtr)(pPathDataNode->Data);
    pMod = pPathData->Port->Module;
    pPort = pPathData->Port;

    pSession = pRes->Session; //SNMP_Channel_Duplicate_Session(pRes->SNMPChan);
    pList = X_List_New((X_ListMemFree_t)(AuxEntityData_Free));

    if (pMod->EqType == EqType_EQPEROADMDC)
    {
        Idx = 0;
#if 0
        Err = SNMP_Contains(pSession, &SnmpErr, pMod->Index, pList);
        // 1. Find CH or VCH
        X_List_ForEach(pList, pNode)
        {
            pAuxData = (AuxEntityData*)(pNode->Data);

            if (pAuxData->Class != EntityClass_UPGRADEPORT &&
                    pAuxData->Class != EntityClass_NETWORKPORT &&
                    pAuxData->Class != EntityClass_CLIENTPORT)
            {
                continue;
            }

            X_Addr_Parse(&Addr, pAuxData->Aid);

            if (0 != X_Addr_Equal(&Addr, &pPort->Addr))
                continue;

            if (pPort == pMod->NetPort || pPort == pMod->UpgradePort)
            {
                // OM
                Idx = pAuxData->Index;
            }
            else
            {
                // CH
                Idx1 = pAuxData->Index;
            }
            break;
        }
        X_List_Clear(pList);
#endif

        if (pPort == pMod->NetPort || pPort == pMod->UpgradePort)
        {
            // OM
            Idx = pPort->Index;
        }
        else
        {
            // CH
            Idx1 = pPort->Index;
        }

        // 2. Find a VCH for OM
        if (Idx != 0)
        {
            Tmp = 0;
            Err = SNMP_Contains(pSession, &SnmpErr, Idx, pList);

            X_List_ForEach(pList, pNode)
            {
                pAuxData = (AuxEntityData*)(pNode->Data);

                if (pAuxData->Class != EntityClass_VIRTUALCHANNEL)
                {
                    continue;
                }

                X_Addr_Parse(&Addr, pAuxData->Aid);

                if (1 != sscanf(&Addr.Slot[1], "%d", &Tmp))
                    continue;

                if (X_RES_C_BAND_MAX_FREQ - Tmp*X_RES_100GHz != Freq)
                    continue;

                // VCH
                Idx1 = pAuxData->Index;
                break;
            }
        }


        // 3. Enable/disable VCH, CH and CRS_CH
        if (Idx1 != 0)
        {
            Err = SNMP_Get_Crs(pSession, &SnmpErr, Idx1, &Idx2);
            if (Idx2 != 0)
            {
                if (Enable)
                {
                    Err = SNMP_SetAdminEnable(pSession, &SnmpErr, Idx1, Enable);
                    Err = SNMP_SetAdminEnable(pSession, &SnmpErr, Idx2, Enable);
                    Err = SNMP_SetCrsAdminEnable(pSession, &SnmpErr, Idx1, Idx2, Enable);

                }
                else
                {
                    Err = SNMP_SetCrsAdminEnable(pSession, &SnmpErr, Idx1, Idx2, Enable);
                    Err = SNMP_SetAdminEnable(pSession, &SnmpErr, Idx1, Enable);
                    Err = SNMP_SetAdminEnable(pSession, &SnmpErr, Idx2, Enable);
                }
            }
        }

    }
    else if (pMod->EqType == EqType_EQP8ROADMC40)
    {
        Idx = 0;
#if 0
        Err = SNMP_Contains(pSession, &SnmpErr, pMod->Index, pList);

        // 1. Find an OM
        X_List_ForEach(pList, pNode)
        {
            pAuxData = (AuxEntityData*)(pNode->Data);

            if ( pAuxData->Class != EntityClass_NETWORKPORT &&
                    pAuxData->Class != EntityClass_CLIENTPORT)
                continue;

            X_Addr_Parse(&Addr, pAuxData->Aid);

            if (0 != X_Addr_Equal(&Addr, &pPort->Addr))
                continue;

            Idx = pAuxData->Index;
            break;
        }

        X_List_Clear(pList);
#endif
        Idx = pPort->Index;

        // 2. Find a VCH
        if (Idx != 0)
        {
            Tmp = 0;

            Err = SNMP_Contains(pSession, &SnmpErr, Idx, pList);

            X_List_ForEach(pList, pNode)
            {
                pAuxData = (AuxEntityData*)(pNode->Data);

                if (pAuxData->Class != EntityClass_VIRTUALCHANNEL)
                    continue;

                X_Addr_Parse(&Addr, pAuxData->Aid);
                if (1 != sscanf(&Addr.Port[1], "%d", &Tmp))
                    continue;

                if (X_RES_C_BAND_MAX_FREQ - Tmp*X_RES_100GHz != Freq)
                    continue;

                Idx1 = pAuxData->Index;
                break;
            }
        }

        X_List_Clear(pList);

        if (Idx1 != 0)
        {
            Err = SNMP_Get_Crs(pSession,&SnmpErr,Idx1,&Idx2);
            if (Idx2 != 0)
            {
                if (Enable)
                {
                    Err = SNMP_SetAdminEnable(pSession,&SnmpErr,Idx1,Enable);
                    Err = SNMP_SetAdminEnable(pSession,&SnmpErr,Idx2,Enable);
                    Err = SNMP_SetCrsAdminEnable(pSession,&SnmpErr,Idx1,Idx2,Enable);
                    Err = SNMP_SetCrsAdminEnable(pSession,&SnmpErr,Idx2,Idx1,Enable);
                }
                else
                {
                    Err = SNMP_SetCrsAdminEnable(pSession,&SnmpErr,Idx1,Idx2,Enable);
                    Err = SNMP_SetCrsAdminEnable(pSession,&SnmpErr,Idx2,Idx1,Enable);
                    Err = SNMP_SetAdminEnable(pSession,&SnmpErr,Idx1,Enable);
                    Err = SNMP_SetAdminEnable(pSession,&SnmpErr,Idx2,Enable);
                }
            }
        }
    }
    else if (pMod->EqType == EqType_EQPCCM8)
    {
        Idx = pPort->Index;
        if (Enable)
        {
            Err = SNMP_SetAdminEnable(pSession,&SnmpErr,Idx,Enable);
        }
        else
        {
            if (pPort != pMod->NetPort)
            {
                Err = SNMP_SetAdminEnable(pSession,&SnmpErr,Idx,Enable);
            }
        }
    }

    X_List_Free(pList);

    //SNMP_Channel_Destroy_Session(pRes->SNMPChan, pSession);

    return 0;
}


/**
 * Trigger per-channel power equalization.
 */
int
X_Module_Power_Equalize(X_ResourcePtr pRes,
       X_ListPtr pPathDataList,
       X_ListNodePtr pPathDataNode,
       int freq)
{
    X_ModulePtr         pMod;
    X_PortPtr           pPort;
    X_PathDataPtr       pPathData; //, pPrevPathData;
    X_ListPtr           pList;
    X_ListNodePtr       pNode;
    X_Addr              Addr;
    netsnmp_session*    pSession;
    SNMP_ErrCode        SnmpErr;
    ErrCode             Err;
    AuxEntityData*      pAuxData;
    unsigned int        Tmp;
    unsigned int        Idx1 = 0;//, Idx2 = 0;

    pPathData = (X_PathDataPtr) (pPathDataNode->Data);
    pMod = pPathData->Port->Module;
    pPort = pPathData->Port;


    /** Currently this is supported on a limited set of modules ! **/
    if (pMod->EqType != EqType_EQP8ROADMC40 && pMod->EqType != EqType_EQPEROADMDC)
    {
        return 0;
    }

    /** Equalization can be only performed on the network-side port **/
    if (pPort != pMod->NetPort)
    {
        return 0;
    }

    if (pPathData->Dir == X_RES_DIR_IN)
    {
        return 0;
    }

   // X_Addr_Dump(&pPort->Addr, Buf, sizeof(Buf));
   // X_Resource_Printf("Equalization triggered on %s", Buf);

   pSession = SNMP_Channel_Duplicate_Session(pRes->SNMPChan);

   pList = X_List_New((X_ListMemFree_t) (AuxEntityData_Free));

   Err = SNMP_Contains(pSession, &SnmpErr, pMod->Index, pList);

   X_List_ForEach(pList, pNode)
   {
       pAuxData = (AuxEntityData*)(pNode->Data);

       if (pAuxData->Class != EntityClass_UPGRADEPORT &&
               pAuxData->Class != EntityClass_NETWORKPORT &&
               pAuxData->Class != EntityClass_CLIENTPORT)
       {
           continue;
       }

       X_Addr_Parse(&Addr, pAuxData->Aid);

       if (X_Addr_Equal(&Addr, &pPort->Addr) != 0)
           continue;

       Idx1 = pAuxData->Index;
       break;
   }

   X_List_Clear(pList);

   Err = SNMP_Contains(pSession, &SnmpErr, Idx1, pList);

   X_List_ForEach(pList, pNode)
   {
       pAuxData = (AuxEntityData*) (pNode->Data);

        if (pAuxData->Class != EntityClass_VIRTUALCHANNEL)
        {
            continue;
        }


        X_Addr_Parse(&Addr, pAuxData->Aid);

        // get the correct VCH...
        if (pMod->EqType == EqType_EQPEROADMDC)
        {
            if (1 != sscanf(&Addr.Slot[1], "%d", &Tmp))
                continue;
        }
        else
        {
            if (1 != sscanf(&Addr.Port[1], "%d", &Tmp))
                continue;
        }

        if (X_RES_C_BAND_MAX_FREQ - Tmp * X_RES_100GHz != freq)
            continue;

        //X_Addr_Dump(&pPort->Addr, Buf, sizeof(Buf));
        X_Resource_Printf("Equalization triggered on %s", pAuxData->Aid);

        Err = SNMP_InitiatePowerEqualization(pSession, &SnmpErr, pAuxData->Index);

        if (Err != ERRCODE_NOERROR ||
                SnmpErr.Status != STAT_SUCCESS ||
                SnmpErr.Err != SNMP_ERR_NOERROR)
        {
            X_Resource_Printf("Error occured while initiating equalization on %s (errmsg=%s,stat=%d,err=%d)",
                    pAuxData->Aid, GetErrMsg(Err), SnmpErr.Status, SnmpErr.Err);
        }

        break;
    }

    SNMP_Channel_Destroy_Session(pRes->SNMPChan, pSession);

    return 0;
}

/**
 * Verify resource allocation
 */
int
X_Module_Verify_Allocation(X_ResourcePtr pRes,
       X_ListPtr pPathDataList,
       X_ListNodePtr pPathDataNode,
       int freq)
{
    X_ModulePtr         pMod;
    X_PathDataPtr       pPathData;
    int ret = -1;

    assert(pRes != NULL && pPathDataList != NULL && pPathDataNode != NULL);

    if (pRes == NULL || pPathDataList == NULL || pPathDataNode == NULL)
    {
        return (-1);
    }

    pPathData = (X_PathDataPtr)(pPathDataNode->Data);

    if (pPathData == NULL || pPathData->Port == NULL)
    {
        return (-1);
    }

    pMod = pPathData->Port->Module;

    switch (pMod->ModType)
    {
    case X_MOD_TYPE_XC:
        ret = X_Module_Xc_Verify(pRes, pPathDataList, pPathDataNode, freq);
        break;
    case X_MOD_TYPE_OL:
        ret = X_Module_OL_Verify(pRes, pPathDataList, pPathDataNode, freq);
        break;
    default:
        ret = 0;
        break;
    }
    return ret;
}

/** Verify path allocation on cross-connect modules **/
int
X_Module_Xc_Verify(X_ResourcePtr pRes,
        X_ListPtr pPathDataList,
        X_ListNodePtr pPathDataNode,
        int freq)
{
    X_ModulePtr         pMod;
    X_PortPtr           pPort;
    X_PathDataPtr       pPathData, pNextPathData;
    X_ListPtr           pList;
    X_ListNodePtr       pNode;
    X_Addr              Addr;
    netsnmp_session*    pSession;
    SNMP_ErrCode        SnmpErr;
    ErrCode             Err;
    AuxEntityData*      pAuxData;
    unsigned int        Idx;
    unsigned int        Tmp;
    unsigned int        Idx1 = 0, Idx2 = 0;
    char                Buf[128];
    //char                Aid1[32], Aid2[32];
    int                 Ret = 0;

    pPathData = (X_PathDataPtr)(pPathDataNode->Data);
    pMod = pPathData->Port->Module;
    pPort = pPathData->Port;


    SNMP_ErrCode_Init(&SnmpErr);
    pSession = pRes->Session; // SNMP_Channel_Duplicate_Session(pRes->SNMPChan);
    pList = X_List_New((X_ListMemFree_t)(AuxEntityData_Free));

    if (pMod->EqType == EqType_EQPEROADMDC)
    {
         Idx = 0;
         Err = SNMP_Contains(pSession, &SnmpErr, pMod->Index, pList);

         // 1. find OM or CH
         X_List_ForEach(pList, pNode)
         {
             pAuxData = (AuxEntityData*)(pNode->Data);

             if (pAuxData->Class != EntityClass_UPGRADEPORT &&
                     pAuxData->Class != EntityClass_NETWORKPORT &&
                     pAuxData->Class != EntityClass_CLIENTPORT)
             {
                 continue;
             }

             X_Addr_Parse(&Addr, pAuxData->Aid);

             if (0 != X_Addr_Equal(&Addr, &pPort->Addr))
                 continue;

             if (pPort == pMod->NetPort || pPort == pMod->UpgradePort)
             {
                 if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
                 {
                     X_Resource_Printf("X_Resource_Xc_Verify: %s not created",
                             pAuxData->Aid);
                     // OM was not created
                     Ret = -1;
                     goto end;
                 }
                 if (pPort == pMod->NetPort)
                     Idx = pAuxData->Index;
             }
             else
             {
                 if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
                 {
                     X_Resource_Printf("X_Resource_Xc_Verify: %s not created",
                                                 pAuxData->Aid);
                     // CH was not created
                     Ret = -1;
                     goto end;
                 }
                 Idx1 = pAuxData->Index;
             }
             break;
         }

         X_List_Clear(pList);

         // 2. Find VCH on N, Idx points to OM
         if (Idx != 0)
         {
             Tmp = 0;
             Err = SNMP_Contains(pSession, &SnmpErr, Idx, pList);

             X_List_ForEach(pList, pNode)
             {
                 pAuxData = (AuxEntityData*)(pNode->Data);

                 if (pAuxData->Class != EntityClass_VIRTUALCHANNEL)
                     continue;

                 X_Addr_Parse(&Addr, pAuxData->Aid);

                 if (1 != sscanf(&Addr.Slot[1], "%d", &Tmp))
                     continue;

                 if (X_RES_C_BAND_MAX_FREQ - Tmp*X_RES_100GHz != freq)
                     continue;

                 if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
                 {
                     X_Resource_Printf("X_Resource_Xc_Verify: %s not created",
                             pAuxData->Aid);
                     // VCH not created
                     Ret = -1;
                     goto end;
                 }

                 Idx1 = pAuxData->Index;
                 break;
             }
         }

         X_List_Clear(pList);

         if (Idx1 != 0)
         {
             // 3. Check if to make an internal cross-connect
             // Idx1 stores VCH or CH, need to find Idx2

             pNode = pPathDataNode->Next;

             while (pNode != NULL)
             {
                 pNextPathData = (X_PathDataPtr)(pNode->Data);

                 if (pNextPathData->Port != NULL && pNextPathData->Port->Module == pMod)
                 {
                     // if both ports on the same EROADM are != U port then make CH<>VCH cross
                     if (pPort != pMod->UpgradePort && pNextPathData->Port != pMod->UpgradePort)
                         break;
                 }
                 else if (pNextPathData->Port != NULL && pNextPathData->Port->Module->EqType == EqType_EQPEROADMDC)
                 {
                     // if other EROADM then make VCH<>VCH cross.
                     if (pNextPathData->Port == pNextPathData->Port->Module->NetPort)
                         break;
                 }
                 pNode = pNode->Next;
             }

             if (pNode != NULL)
             {
                 pNextPathData = (X_PathDataPtr)(pNode->Data);

                 // find the Idx2
                 Err = SNMP_Contains(pSession, &SnmpErr, pNextPathData->Port->Module->Index, pList);

                 Idx = 0;

                 X_List_ForEach(pList, pNode)
                 {
                     pAuxData = (AuxEntityData*)(pNode->Data);

                     if (pAuxData->Class != EntityClass_NETWORKPORT &&
                             pAuxData->Class != EntityClass_CLIENTPORT)
                         continue;

                     X_Addr_Parse(&Addr, pAuxData->Aid);

                     X_Addr_Dump(&pNextPathData->Port->Addr,Buf,sizeof(Buf));

                     if (0 != X_Addr_Equal(&Addr, &pNextPathData->Port->Addr))
                         continue;

                     if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
                     {
                         X_Resource_Printf("X_Resource_Xc_Verify: %s not created",
                                                     pAuxData->Aid);
                         Ret = -1;
                         break;
                     }

                     if (pNextPathData->Port == pNextPathData->Port->Module->NetPort)
                     {
                         Idx = pAuxData->Index;
                     }
                     else
                     {
                         Idx2 = pAuxData->Index;
                     }
                     break;
                 }

                 X_List_Clear(pList);

                 // find the VCH
                 if (Idx != 0)
                 {
                     Err = SNMP_Contains(pSession, &SnmpErr, Idx, pList);
                     X_List_ForEach(pList, pNode)
                     {
                         pAuxData = (AuxEntityData*)(pNode->Data);
                         X_Addr_Parse(&Addr, pAuxData->Aid);
                         if (1 != sscanf(&Addr.Slot[1], "%d", &Tmp))
                             continue;

                         if (X_RES_C_BAND_MAX_FREQ - Tmp*X_RES_100GHz != freq)
                             continue;

                         if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
                         {
                             X_Resource_Printf("X_Resource_Xc_Verify: %s not created",
                                     pAuxData->Aid);
                             Ret = -1;
                             break;
                         }

                         Idx2 = pAuxData->Index;
                         break;
                     }
                 }

#if 0
                 // now should have Idx1 and Idx2, do the cross-connect

                 if (Idx1 != 0 && Idx2 != 0)
                 {
                     if (pPathData->PathDir == X_RES_DIR_IN)
                     {
                         X_Resource_Printf("Making CRS_CH %s -> %s", Aid1, Aid2);
                         Err = SNMP_Create_Crs(pSession, &SnmpErr, Idx1, Idx2, 0, 0);
                         //Err = SNMP_GetCrsAdminCap(pSession, &SnmpErr, Idx1, Idx2, &AdminCaps);
                     }
                     else
                     {
                         X_Resource_Printf("Making CRS_CH %s -> %s", Aid2, Aid1);
                         Err = SNMP_Create_Crs(pSession, &SnmpErr, Idx2, Idx1, 0, 0);
                         //Err = SNMP_GetCrsAdminCap(pSession, &SnmpErr, Idx2, Idx1, &AdminCaps);
                     }
                 }
#endif

         }

         }
     }
     else if (pMod->EqType == EqType_EQP8ROADMC40)
     {
         Idx = 0;
         Err = SNMP_Contains(pSession, &SnmpErr, pMod->Index, pList);

         // 1. find OM
         X_List_ForEach(pList, pNode)
         {
             pAuxData = (AuxEntityData*)(pNode->Data);

             if (pAuxData->Class != EntityClass_CLIENTPORT &&
                     pAuxData->Class != EntityClass_NETWORKPORT)
             {
                 continue;
             }

             X_Addr_Parse(&Addr, pAuxData->Aid);

             if (0 != X_Addr_Equal(&Addr, &pPort->Addr))
                 continue;

             if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
             {
                 X_Resource_Printf("X_Module_Xc_Verify: %s not created",
                         pAuxData->Aid);
                 Ret = -1;
                 goto end;
             }

             if ( 1 != sscanf(&pPort->Addr.Port[1], "%d", &Tmp) )
             {
                 Tmp = 0;
             }

             if (pMod->NetPort == pPort || Tmp == pMod->RoadmNo)
                 Idx = pAuxData->Index;

             break;
         }

         X_List_Clear(pList);

         // 2. find VCH
         if (Idx != 0)
         {
             Tmp = 0;

             Err = SNMP_Contains(pSession, &SnmpErr, Idx, pList);

             X_List_ForEach(pList, pNode)
             {
                 pAuxData = (AuxEntityData*)(pNode->Data);

                 if (pAuxData->Class != EntityClass_VIRTUALCHANNEL)
                     continue;

                 X_Addr_Parse(&Addr, pAuxData->Aid);

                 if (1 != sscanf(&Addr.Port[1], "%d", &Tmp))
                     continue;

                 if (X_RES_C_BAND_MAX_FREQ - Tmp*X_RES_100GHz != freq)
                     continue;

                 if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
                 {
                     X_Resource_Printf("X_Module_Xc_Verify: %s not created",
                             pAuxData->Aid);
                     Ret = -1;
                     goto end;
                 }
                 Idx1 = pAuxData->Index;
                 break;
             }
         }

         X_List_Clear(pList);

         if (Idx1 != 0)
         {
             pNode = pPathDataNode->Next;

             while (pNode != NULL)
             {
                 pNextPathData = (X_PathDataPtr)(pNode->Data);

                 if (pNextPathData->Port != NULL && pNextPathData->Port->Module == pMod)
                 {
                     // if both ports on the same EROADM are != U port then make VCH<>VCH cross
                     //if (pMod->RoadmNo)
                     //break;
                 }
                 else if (pNextPathData->Port != NULL &&
                         pNextPathData->Port->Module->EqType == EqType_EQP8ROADMC40)
                 {
                     // VCH-VCH cross
                     if (pNextPathData->Port == pNextPathData->Port->Module->NetPort)
                         break;
                 }

                 pNode = pNode->Prev;
             }

             if (pNode != NULL)
             {
                 pNextPathData = (X_PathDataPtr)(pNode->Data);

                 // find the Idx2

                 Err = SNMP_Contains(pSession, &SnmpErr, pNextPathData->Port->Module->Index, pList);

                 Idx = 0;

                 X_List_ForEach(pList, pNode)
                 {
                     pAuxData = (AuxEntityData*)(pNode->Data);

                     if (pAuxData->Class != EntityClass_NETWORKPORT &&
                             pAuxData->Class != EntityClass_CLIENTPORT)
                         continue;

                     X_Addr_Parse(&Addr, pAuxData->Aid);

                     X_Addr_Dump(&pNextPathData->Port->Addr,Buf,sizeof(Buf));

                     //X_Resource_Printf("Addr = %s AID = %s", Buf, pAuxData->Aid);

                     if (0 != X_Addr_Equal(&Addr, &pNextPathData->Port->Addr))
                         continue;

                     if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
                     {
                         X_Resource_Printf("X_Module_Xc_Verify: %s not created",
                                 pAuxData->Aid);
                         Ret = -1;
                         goto end;
                     }

                     Idx = pAuxData->Index;

                     break;
                 }

                 X_List_Clear(pList);

                 // find the VCH
                 if (Idx != 0)
                 {
                     Err = SNMP_Contains(pSession, &SnmpErr, Idx, pList);

                     X_List_ForEach(pList, pNode)
                     {
                         pAuxData = (AuxEntityData*)(pNode->Data);
                         X_Addr_Parse(&Addr, pAuxData->Aid);

                         X_Addr_Dump(&Addr,Buf,sizeof(Buf));

                         //X_Resource_Printf("Addr = %s AID = %s", Buf, pAuxData->Aid);

                         if (1 != sscanf(&Addr.Port[1], "%d", &Tmp))
                             continue;

                         if (X_RES_C_BAND_MAX_FREQ - Tmp*X_RES_100GHz != freq)
                             continue;

                         if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
                         {
                             X_Resource_Printf("X_Module_Xc_Verify: %s not created",
                                     pAuxData->Aid);
                             Ret = -1;
                             goto end;
                         }
                         Idx2 = pAuxData->Index;
                         break;
                     }
                 }
#if 0
                 if (Idx1 != 0 && Idx2 != 0)
                 {
                     if (pPathData->PathDir == X_RES_DIR_IN)
                     {
                         X_Resource_Printf("Making CRS_CH %s -> %s", Aid1, Aid2);
                         Err = SNMP_Create_Crs(pSession, &SnmpErr, Idx1, Idx2, FacilityType_IFTYPEOTU2, 1);
                     }
                     else
                     {
                         X_Resource_Printf("Making CRS_CH %s -> %s", Aid2, Aid1);
                         Err = SNMP_Create_Crs(pSession, &SnmpErr, Idx2, Idx1, FacilityType_IFTYPEOTU2, 1);
                     }
                 }
#endif
             }
         }
     }
#if 0
     else if (pMod->EqType == EqType_EQPCCM8)
     {
         Idx = 0;

         Err = SNMP_Contains(pSession, &SnmpErr, pMod->Index, pList);

         X_List_ForEach(pList, pNode)
         {
             pAuxData = (AuxEntityData*)(pNode->Data);

             if (pAuxData->Class != EntityClass_CLIENTPORT &&
                     pAuxData->Class != EntityClass_NETWORKPORT)
                 continue;

             X_Addr_Parse(&Addr, pAuxData->Aid);

             if (0 != X_Addr_Equal(&Addr, &pPort->Addr))
                 continue;

             if (pAuxData->Assignment != EntityAssignmentState_ASSIGNED)
             {
                 X_Resource_Printf("X_Module_Xc_Resource_Allocate: creating %s", pAuxData->Aid);

                 if (pMod->NetPort == pPort)
                 {
                     Err = SNMP_Create_OM_Facility(pSession, &SnmpErr, pAuxData->Index);
                 }
                 else
                 {
                     Err = SNMP_Create_CH_Facility(pSession, &SnmpErr, pAuxData->Index,
                             FacilityType_IFTYPEOCH,
                             Freq_To_ADVA_Channel(freq));
                 }
             }

             break;
         }
         X_List_Clear(pList);
     }
#endif

end:

    // SNMP_Channel_Destroy_Session(pRes->SNMPChan, pSession);
     X_List_Free(pList);
     return Ret;
}

/** Verify path allocation on OL modules **/
int
X_Module_OL_Verify(X_ResourcePtr pRes,
        X_ListPtr pPathDataList,
        X_ListNodePtr pPathDataNode,
        int freq)
{
    X_ModulePtr         pMod;
    X_PortPtr           pPort;
    X_PathDataPtr       pPathData; //, pPrevPathData;
    //X_ListNodePtr       pNode;
   // netsnmp_session*    pSession;
    SNMP_ErrCode        SnmpErr;
    ErrCode             Err;
    AuxEntityData*      pAuxData;
    unsigned int        Idx;
    unsigned int        Idx1;
    int                 i;
    int                 Ret = 0;

    pPathData = (X_PathDataPtr)(pPathDataNode->Data);
    pMod = pPathData->Port->Module;
    pPort = pPathData->Port;

    i = Freq_To_ADVA_Channel(freq) - 1;

    if ((X_List_Tail(pPathDataList) == pPathDataNode &&
            pPathData->PathDir == X_RES_DIR_OUT) ||
            (X_List_Head(pPathDataList) == pPathDataNode &&
                    pPathData->PathDir == X_RES_DIR_IN))
    {
        return 0;
    }

    if (i < 0)
    {
        return -1;
    }

    pAuxData = (AuxEntityData*)X_Vector_At(pMod->VectorWch, i);

    if (pAuxData == NULL)
    {
        return -1;
    }

    Idx = pAuxData->Index;

    SNMP_ErrCode_Init(&SnmpErr);

   // pSession = SNMP_Channel_Duplicate_Session(pRes->SNMPChan);

    Err = SNMP_Get_Crs(pRes->Session, &SnmpErr, Idx, &Idx1);

    if (Err != ERRCODE_NOERROR || Idx1 == 0)
    {
        Ret = -1;
    }

   // SNMP_Channel_Destroy_Session(pRes->SNMPChan, pSession);

    return Ret;
}

