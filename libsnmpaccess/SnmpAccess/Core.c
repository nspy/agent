
#include "Core.h"
#include "Log.h"

#define SNMP_LIB_REVISION "$Revision: 9 $"

static int SNMP_LIB_MAJOR_VERSION = 0;
static int SNMP_LIB_MINOR_VERSION = 1;




int
SNMP_Lib_Init(void)
{
#ifdef IMPORT_MIB
    init_snmp("app");
#endif
    return 0;
}

void
SNMP_Lib_Exit(void)
{

}

void
SNMP_Lib_Version(int *major, int *minor)
{
    *major = SNMP_LIB_MAJOR_VERSION;
    *minor = SNMP_LIB_MINOR_VERSION;
}

