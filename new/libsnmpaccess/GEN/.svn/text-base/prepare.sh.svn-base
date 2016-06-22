#!/bin/bash

MIBDIRS=/usr/share/snmp/mibs:/usr/share/mibs:../ADVA-MIB:../../ADVA-MIB
MIBS=+ALL

OID_FILE_PREFIX=oid
OID_FILE=./${OID_FILE_PREFIX}.h
OID_CONF=./conf/oid.conf
TYPE_CONF=./conf/type.conf
TYPE_DIR=./types
WKG_DIR=$PWD

function prepare_env()
{
	export MIBDIRS=${MIBDIRS}
	export MIBS=${MIBS}
	echo "MIBDIRS=$MIBDIRS"
	echo "MIBS=$MIBS"
}

###
### Generate OIDs defintions from the MIB
### @param ${1} OID
###
function generate_oid()
{	
	echo "generate_oid ${@}"
	if [ -z ${1} ]; then
		echo "generate_oid(): invalid arguments (${@})"
		return 1
	fi

	mib2c -i -c ${OID_CONF} -f ${OID_FILE_PREFIX} ${1} 2> /dev/null
}

###
### Generate types defintions from the MIB
### @param ${1} OID
### @param ${2} pattern to match
### @param ${3} the name of the enum
###
function generate_type()
{
	echo "generate_type ${@}"
	if [ -z ${1} ] || [ -z ${2} ] || [ -z {$3} ]; then
		echo "generate_type(): invalid arguments (${@})"
	fi
	mib2c -i -c ${TYPE_CONF} -f ${2} ${1} 2> /dev/null
	grep "${2}_" ${2}.h > ${TYPE_DIR}/${3}
	sed -i s/${2}/${3}/g ${TYPE_DIR}/${3}
	rm ${2}.h
}

function generate()
{
	generate_oid "FspR7-MIB::entityStateTable"
	generate_oid "FspR7-MIB::entityStateCapTable"
	generate_oid "FspR7-MIB::controlPlaneWdmEntityStateTable"
	generate_oid "FspR7-MIB::neAdmin"	
	generate_oid "FspR7-MIB::tunnelWdmDataTable"
	generate_oid "FspR7-MIB::tunnelWdmConfigTable"
	generate_oid "FspR7-MIB::connectionWdmDataTable"
	generate_oid "FspR7-MIB::intraNeConnectionsTable"
	generate_oid "FspR7-MIB::shelfDeployProv"
	generate_oid "FspR7-MIB::moduleDeployProv"
	generate_oid "FspR7-MIB::plugDeployProv"
	generate_oid "FspR7-MIB::unusedWdmTunnelIndex"
	generate_oid "FspR7-MIB::unusedWdmPathIndex"
	generate_oid "FspR7-MIB::teUnnumLinkWdmDataTable"
	generate_oid "FspR7-MIB::teNumLinkWdmDataTable"
	generate_oid "FspR7-MIB::deployProvTunnelWdmTable"
	generate_oid "FspR7-MIB::deployProvOpticalLinkTable"
	generate_oid "FspR7-MIB::pathWdmDataTable"
	generate_oid "FspR7-MIB::pathElementWdmDataTable"
	generate_oid "FspR7-MIB::inventoryCpWdmDownDataTable"
	generate_oid "FspR7-MIB::inventoryCpWdmUpDataTable"
	generate_oid "FspR7-MIB::deployProvIfTable"
	generate_oid "FspR7-MIB::deployProvVchChannelTable"
	generate_oid "FspR7-MIB::crsConnDeployProvTable"	
	generate_oid "FspR7-MIB::crsConnDeployProvCapTable"
	generate_oid "FspR7-MIB::deployProvExChannelTable"	
	generate_oid "FspR7-MIB::deployProvPathWdmTable"
	generate_oid "FspR7-MIB::deployProvPathElementWdmTable"
	generate_oid "FspR7-MIB::exChannelDataTable"
	generate_oid "FspR7-MIB::deployProvVirtualOpticalChannelTable"
	generate_oid "FspR7-MIB::interfaceConfigTable"
	generate_oid "ADVA-MIB::entityTable"
	generate_oid "ADVA-MIB::containsTable"
	generate_oid "ADVA-MIB::controlPlaneWdmContainsTable"
	generate_oid "ADVA-MIB::controlPlaneWdmEntityTable"
	generate_oid "ADVA-MIB::ptpEntityTable"
	generate_oid "ADVA-MIB::vtpEntityTable"

	generate_type "FspR7-MIB::entityStateTable" "entityStateAdmin" "AdminState"
	generate_type "FspR7-MIB::entityStateTable" "entityStateOper" "OperState"
	generate_type "FspR7-MIB::deployProvShelfTable" "deployProvShelfType" "EqType"
	generate_type "FspR7-MIB::deployProvModuleTable" "deployProvModuleMode" "CardMode"
	generate_type "FspR7-MIB::deployProvModuleTable" "deployProvModuleChannel" "Channel"
	generate_type "FspR7-MIB::deployProvModuleTable" "deployProvModuleBand" "Band"
	generate_type "FspR7-MIB::tunnelWdmDataTable" "tunnelWdmDataComputationState" "PathComputationState"
	generate_type "FspR7-MIB::tunnelWdmDataTable" "tunnelWdmDataErrorCategory" "ErrorCategory"
	generate_type "FspR7-MIB::pathElementWdmDataTable" "pathElementWdmDataTrafficEngineeringType" "TeType"
	generate_type "FspR7-MIB::pathElementWdmDataTable" "pathElementWdmDataTie" "PathFollow"
	generate_type "FspR7-MIB::deployProvIfTable" "deployProvIfType" "FacilityType"
	generate_type "FspR7-MIB::connectionWdmDataTable" "connectionWdmDataEqualizationState" "PwrEqState"
	generate_type "ADVA-MIB::entityTable" "entityClass" "EntityClass"
	generate_type "ADVA-MIB::entityTable" "entityAssignmentState" "EntityAssignmentState"
	generate_type "ADVA-MIB::controlPlaneWdmEntityTable" "controlPlaneWdmEntityClass" "CPEntityClass"
}

function main()
{
	prepare_env

	if [ -z `which mib2c` ]; then
		echo "Couldn't find the 'mib2c' executable!"
		exit 1
	fi

	if [ -d ${TYPE_DIR} ]; then
		rm -rf ${TYPE_DIR}
	fi

	if [ ! -d ${TYPE_DIR} ]; then
		mkdir ${TYPE_DIR}
	fi

	if [ -f ${OID_FILE} ]; then
		rm ${OID_FILE}
	fi

	generate

	exit 0
}

### start ###
main $@


