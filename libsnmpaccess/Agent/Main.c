
#include "Agent.h"

int main(int argc, char *argv[])
{
    X_CorePtr Core;
    
    char cmd[128];
    if (argc < 2)
    {
        printf("Usage: %s <path>\n", argv[0]);
        return 1;
    }
    SNMP_Lib_Init();

    
    Core = X_Core_Create_From_File(argv[1]);
    
    while (fgets(cmd,sizeof(cmd),stdin))
    {

    }
   


    X_Core_Free(Core);

    return 0;
}
