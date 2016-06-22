
#ifndef __CORE_H__
#define __CORE_H__

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

int
SNMP_Lib_Init(void);

void
SNMP_Lib_Exit(void);

void
SNMP_Lib_Version(int *major, int *minor);


#endif
