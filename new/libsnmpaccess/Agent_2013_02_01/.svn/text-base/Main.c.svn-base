
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

    X_Log_Open(X_LOG_MASK_STDERR | X_LOG_MASK_FILE);
    X_Log_SetFile("log.txt");
    X_Log_SetLevel(0, 3); // enable logging for channel module
    X_Log_SetLevel(1, 3); // enable logging for interface
    X_Log_SetLevel(2, 3); // enable logging for resources

    /** Initialize the SNMP lib. **/
    SNMP_Lib_Init();

    Core = X_Core_Create_From_File(argv[1]);

    while (fgets(cmd,sizeof(cmd),stdin))
    {

    }

    X_Core_Free(Core);

    return 0;
}
