
#include <SnmpAccess/Lightpath.h>

GMPLS_Controller GMPLSC;

int ConvertAid(const char *aid)
{
    int shelf, slot, port, n;
    char type[32];
    char *p = (char*)aid;
    char *r;

    int ret;

    if (aid == NULL || aid[0] == '\0')
    {
        return (0);
    }

    memset(type, 0, sizeof(type));

    r = strchr(p, '-');

    if (r == NULL)
        return (0);

    memcpy(type, p, r - p);


    if (strcmp(type, "MOD")) return (0);

    p += r - p + 1;

    if (2 == sscanf(p, "%d-%d%n", &shelf, &slot, &n))
    {
        ret = 0x06000000 | (((shelf * 10) & 0xff) << 16) | (((slot & 0xff)) << 8);
        return ret;
    }

    return 0;
}

void Finalize(int id, GMPLS_RequestPtr req, GMPLS_Request_Result_t result)
{
    printf("<<< request: %d result = %d >>>\r\n", id, result);
}

int main(int argc, char *argv[])
{
    GMPLS_RequestPtr pReq;
    char Cmd[128], *p;
    char Token[32];
    int n;


    char Agent[64], IP[32];
    long FromAid, ToAid;
    char FromAidStr[32], ToAidStr[32];

    GMPLS_Request_Type_t Type;

    GMPLS_Controller_Init(&GMPLSC);

    GMPLS_Controller_Add_Callback(&GMPLSC, Finalize);

    GMPLS_Controller_Start(&GMPLSC);

    while (fgets(Cmd, sizeof(Cmd), stdin))
    {
        p = strchr(Cmd, '\n');

        if (p != NULL)
        {
            *p = '\0';
        }

        p = Cmd;

        if (!strcmp(p, "help"))
        {
            printf("<agent> <dst-ip> <from-aid> <to-aid> <up|down>\n");
            printf("e.g.: 10.50.10.2:1610 10.50.10.3 107217152 107217152 up\n");
            continue;
        }

        if (sscanf(p, "%63s%n", Agent, &n) != 1)
        {
            continue;
        }

        p += n;

        if (sscanf(p, "%31s%n", IP, &n) != 1)
        {
            continue;
        }

        p += n;

        if (sscanf(p, "%30s%n", FromAidStr, &n) != 1)
        {
            continue;
        }

        p += n;

        if (sscanf(p, "%30s%n", ToAidStr, &n) != 1)
        {
            continue;
        }

        p += n;

        FromAid = ConvertAid(FromAidStr);
        ToAid = ConvertAid(ToAidStr);

        if (FromAid == 0 || ToAid == 0)
            continue;

        if (sscanf(p, "%31s%n", Token, &n) != 1)
        {
            continue;
        }

        p += n;

        if (!strcmp(Token, "up"))
        {
            Type = GMPLS_REQ_TYPE_SETUP;
        }
        else if(!strcmp(Token, "down"))
        {
            Type = GMPLS_REQ_TYPE_TEAR_DOWN;
        }
        else
        {
            continue;
        }

        // 107217408 -> MOD-10-2
        // 107217664 -> MOD-10-3
        // 107217152 -> MOD-10-1

        // e.g. 10.50.10.2:1610 10.50.10.3 107217152 107217152 up
        // e.g. 10.50.10.1:1610 10.50.10.3 107217152 107217152 up

        pReq = GMPLS_Request_Create(Type, IP, FromAid, ToAid);

        GMPLS_Request_Set_SNMP(pReq, Agent, "private");

        GMPLS_Controller_Put(&GMPLSC, pReq);
    }

    GMPLS_Controller_Stop(&GMPLSC);


    GMPLS_Controller_Uninit(&GMPLSC);

    return 0;
}
