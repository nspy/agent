#!/usr/bin/python

import sys, os, re

OID_USE_DEFINE=False

### utils ###

def write_enum(f,name,params, strxlat = False):

	if len(params) == 0:
		return

	### prepare dictionary
	pdict = dict()
	for x in params:
		pdict[int(x[1])] = x[0]

	print "Writing enum [%s]" % (name)

	vmin = int(params[0][1])
	vmax = int(params[-1][1])
	total = vmax - vmin + 1

	print "min/max/total = %d/%d/%d" % (vmin,vmax,total)

	f.write("\n/***** START %s <<< ******/\n\n" % (name))

	if (strxlat == True):
		f.write("const std::string %sStr[] = {\n" % (name))
		for i in range(0, total + 1):
			if pdict.has_key(i):
				f.write("\t\"%s\",\n" % (pdict[i]))
			else:
				f.write("\t\"\",\n")
		f.write("};\n\n")

#	f.write("class %s {\n" % (name))
#	f.write("public:\n")
	f.write("\nenum {\n")
	for p in params:
		f.write("\t%s_%s = %s,\n" % (name,p[0],p[1]))
	f.write("};\n\n")

#	if (strxlat == True):
#		f.write("static const std::string %sStr[];\n\n" % (name))

	if (strxlat == True):
		f.write("static std::string str(long t)\n{\n")
		f.write("\treturn (t > %d)	? \"?\" : %sStr[t];\n" % (vmax, name))
		f.write("}\n\n")
	
#	f.write("};\n\n")


	f.write("/***** >>> %s  END ******/\n\n" % (name))
	
def write_oids(f,oids):
	for oid in oids:
		for o in oid:
			f.write(o)
		f.write("\n")

def generate_from_mib(oid,keyword, dump = False):
	prefix = "tmp"
	output = "%s.h" % (prefix)
	cmd = "env MIBDIRS=../ADVA-MIB:./ADVA-MIB:/usr/share/snmp/mibs MIBS=ALL mib2c -c mib2c.conf -f %s %s 2>/dev/null" % (prefix, oid)
	os.system(cmd)

	print "Generating %s from %s" % (keyword, oid)

	try:
		f = open(output,"r+")
	except Exception, e:
		return None

	lines = f.readlines()
	pattern = re.compile(".*%s_([a-zA-Z0-9]+)[\s]+([0-9]+).*" % (keyword))
	out = []

	for line in lines:
		match= re.match(pattern,line)
		if match != None and match.group(1) != None and match.group(2) != None:
			out.append( [ match.group(1), match.group(2) ] )

	f.close()

	try:
		#os.unlink(output)
		pass
	except Exception, e:
		pass

	return out


def generate_oid_from_mib(oid,keywords = None):
	prefix = "tmp"
	output = "%s.h" % (prefix)
	if not OID_USE_DEFINE:
		cmd = "env MIBDIRS=../ADVA-MIB:./ADVA-MIB:/usr/share/snmp/mibs MIBS=ALL mib2c -c mib2c-oid.conf -f %s %s 2>/dev/null" % (prefix, oid)
	else:
		cmd = "env MIBDIRS=../ADVA-MIB:./ADVA-MIB:/usr/share/snmp/mibs MIBS=ALL mib2c -c mib2c-oid-define.conf -f %s %s 2>/dev/null" % (prefix, oid)
		
	os.system(cmd)

	print "Generating from oid %s" % (oid)
	
	try:
		f = open(output,"r+")
	except Exception, e:
		return None

	lines = f.readlines()

	out = []

	if keywords == None:
		keywords = []

	if len(keywords) == 0:
		keywords.append("[a-zA-Z]*");

	for key in keywords:
		oid = generate_oid(key,lines)
		if oid != None and len(oid) == 2:
			out.append(oid)
		elif len(oid) > 2 and (len(oid) % 2 == 0):
			while (len(oid) > 0):
				#print len(oid)
				out.append( [ oid[0], oid[1] ])
				del oid[0:2]
	f.close()
	try:
		os.unlink(output)	
	except Exception, e:
		pass
	return out


def generate_oid(keyword,lines):
	if not OID_USE_DEFINE:
		pattern = [ re.compile("const oid const %s_oid.*" % (keyword)), re.compile("const size_t %s_oid_size.*" % (keyword)) ]
	else:
		pattern = [ re.compile("#define %s_oid.*" % (keyword)), re.compile("#define %s_oid_size.*" % (keyword)) ]
	out = []
	for line in lines:
		match = re.match(pattern[0],line)
		if match != None: #and len(out) == 0:
			out.append(line)

		match = re.match(pattern[1],line)
		if match != None:
			out.append(line)	

	return out

### generate type ###

def make_types(path):
	output = path

	#print "Generating row status types..."
	rowTypes = generate_from_mib("1.3.6.1.4.1.2544.1.11.2.5.2","deployProvModuleRowStatus")

	#print "Generating force destroy types..."
	fdTypes = generate_from_mib("1.3.6.1.4.1.2544.1.11.2.5.2","deployProvModuleForceDestroy")

	#print "Generating module types..."
	modTypes = generate_from_mib("1.3.6.1.4.1.2544.1.11.2.5.2","deployProvModuleType")

	#print "Generating band types..."
	bandTypes = generate_from_mib("1.3.6.1.4.1.2544.1.11.2.5.2","deployProvModuleBand")

	#print "Generating reach types..."
	reachTypes = generate_from_mib("1.3.6.1.4.1.2544.1.11.2.5.2","deployProvModuleReach")

	#print "Generating module mode types..."
	modeTypes = generate_from_mib("1.3.6.1.4.1.2544.1.11.2.5.2","deployProvModuleMode")

	#print "Generating module deploy types..."
	moduleDeployTypes = generate_from_mib("1.3.6.1.4.1.2544.1.11.2.5.2","deployProvModuleDeploy")

	#print "Generating Traffic Engineering types..."
	teTypes = generate_from_mib("1.3.6.1.4.1.2544.1.11.2.5.8.21","deployProvPathElementWdmTrafficEngineeringType")

	connCmd = generate_from_mib(".1.3.6.1.4.1.2544.1.11.2.5.5","deployProvConnCommand")

	#print "Generating connections types..."
	connTypes = generate_from_mib("1.3.6.1.4.1.2544.1.11.2.4.5","intraNeConnectionsType")

	#print "Generating entity admin states..."
	adminTypes = generate_from_mib("1.3.6.1.4.1.2544.1.11.2.4.1","entityStateAdmin")

	#print "Generating entity oper states..."
	operTypes = generate_from_mib("1.3.6.1.4.1.2544.1.11.2.4.1","entityStateOper")

	entityClass = generate_from_mib("1.3.6.1.4.1.2544.2.5.5.2", "entityClass")

	cpEntityClass = generate_from_mib("1.3.6.1.4.1.2544.2.5.5.4", "controlPlaneWdmEntityClass")

	facClass = generate_from_mib("1.3.6.1.4.1.2544.1.11.2.5.4.1", "deployProvIfType")

	eqlzAdmin = generate_from_mib("1.3.6.1.4.1.2544.1.11.2.5.4.4", "deployProvVchChannelEqlzAdmin")

	chanType = generate_from_mib("1.3.6.1.4.1.2544.1.11.2.5.4.1", "deployProvIfChannelProvisioning")

	crsConnType = generate_from_mib( "1.3.6.1.4.1.2544.1.11.2.5.6.3", "crsConnDeployProvConn" )

	alsMode = generate_from_mib( "1.3.6.1.4.1.2544.1.11.2.5.4.1", "deployProvIfAlsMode" )

	eqCapTypes = generate_from_mib( "1.3.6.1.4.1.2544.1.11.2.5.3", "deployProvPlugCapType")


	f = open(output,"w")
	f.write("\n#ifndef _Types_h_\n#define _Types_h_\n\n")
	f.write("\n/** auto generated **/\n\n")
	write_enum(f, "RowStatus",rowTypes)
	write_enum(f, "ForceDestroy",fdTypes)
	write_enum(f, "AdminState",adminTypes)
	write_enum(f, "OperState",operTypes)
	write_enum(f, "EqType",modTypes)
	write_enum(f, "BandType",bandTypes)
	write_enum(f, "ReachType",reachTypes)
	write_enum(f, "ModeType",modeTypes)
	write_enum(f, "DeployType",moduleDeployTypes)
	write_enum(f, "TrafficEngType",teTypes)
	write_enum(f, "ConnType",connTypes)
	write_enum(f, "ConnCommand",connCmd)
	write_enum(f, "EntityClass", entityClass)
	write_enum(f, "CpEntityClass", cpEntityClass)
	write_enum(f, "FacilityClass", facClass)
	write_enum(f, "EqlzAdmin", eqlzAdmin)
	write_enum(f, "ChannelType", chanType)
	write_enum(f, "CrsConn", crsConnType)
	write_enum(f, "AlsMode", alsMode)
	write_enum(f, "EqCapTypes", eqCapTypes)
	f.write("#endif\n")
	f.close()

def make_oids(path):

	output = path

	print "Generating OIDs..."

	oidList = []

	# neAdmin
	oidList.append( generate_oid_from_mib(".1.3.6.1.4.1.2544.1.11.2.2.1") )

	# enityState
	oidList.append( generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.4.1.1.1") )

	# deployProvShelf
	oidList.append( generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.5.1.1") )

	# deployProvModule
	oidList.append( generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.5.2.1") )

	# deployProvPlug
	oidList.append( generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.5.3.1") )

	# deployProvConn (connectionDeployProv)
	oidList.append( generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.5.5") )

	# deployProvFacility
	oidList.append( generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.5.4") )

	# deployProvTunnelWdm
	deployProvTunnelWdmTokens = [
			"deployProvTunnelWdmRowStatus",
			"deployProvTunnelWdmTunnelId",
			"deployProvTunnelWdmToNodeIp",
			"deployProvTunnelWdmFromAid",
			"deployProvTunnelWdmToAid",
			]
	deployProvTunnelWdmOids = generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.5.8.14",deployProvTunnelWdmTokens)
	oidList.append( deployProvTunnelWdmOids )

	# crossConnectionDeployProv
	oidList.append( generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.5.6") )

	# intraNeConnection
	intraNeConnectionTokens = [
			"intraNeConnectionsType",
			"intraNeConnectionsEndpointA",
			"intraNeConnectionsEndpointB"
			]
	intraNeConnectionOids = generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.4.5.3.1",intraNeConnectionTokens)
	oidList.append( intraNeConnectionOids )

	# controlPlaneWdmEntityState
	controlPlaneWdmEntityStateOids = generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.4.1.3.1")
	oidList.append( controlPlaneWdmEntityStateOids )

	connectionWdmDataTokens = [
			"connectionWdmDataTunnelId",
			"connectionWdmDataTunnelNo",
			"connectionWdmDataFromNodeIp",
			"connectionWdmDataToNodeIp",
			"connectionWdmDataSignalledPaths",
			"connectionWdmDataComputedPaths",
			]
	connectionWdmDataOids = generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.4.7.10",connectionWdmDataTokens)
	oidList.append( connectionWdmDataOids ) 

	pathWdmDataTokens = [
			"pathWdmDataPathId",
			]
	pathWdmDataOids = generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.4.7.6",None)#pathWdmDataTokens)
	oidList.append(	pathWdmDataOids )

	pathElementWdmDataTokens = [
			"pathElementWdmDataPathId",
			"pathElementWdmDataSequenceNumber",
			"pathElementWdmDataPathType",
			"pathElementWdmDataTrafficEngineeringType",
			"pathElementWdmDataLifIp",
			"pathElementWdmDataNodeIp",
			"pathElementWdmDataTie",
			"pathElementWdmDataChannelUp",
			"pathElementWdmDataChannelDown",
			]
	pathElementWdmDataOids = generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.4.7.8",pathElementWdmDataTokens)
	oidList.append(	pathElementWdmDataOids )

	inventoryCpWdmDownDataTokens = [
			"inventoryCpWdmDownDataPhysicalTermPoints",
			"inventoryCpWdmDownDataFacilities",
			"inventoryCpWdmDownDataResources"
			]
	inventoryCpWdmDownDataOids = generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.4.7.13.1",inventoryCpWdmDownDataTokens)
	oidList.append( inventoryCpWdmDownDataOids )

	inventoryCpWdmUpDataTokens = [
			"inventoryCpWdmUpDataPhysicalTermPoints",
			"inventoryCpWdmUpDataFacilities",
			"inventoryCpWdmUpDataResources"
			]
	inventoryCpWdmUpDataOids = generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.4.7.14.1",inventoryCpWdmUpDataTokens)
	oidList.append( inventoryCpWdmUpDataOids )

	ptpEntityDataTokens = [
			"ptpEntityDataConnEqptState",
			"ptpEntityDataChannelProvision"
			]
	ptpEntityDataOids = generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.4.34.1",ptpEntityDataTokens)
	oidList.append( ptpEntityDataOids ) 

	physicalTerminationPointOids = generate_oid_from_mib(".1.3.6.1.4.1.2544.1.11.2.4.5.8")
	oidList.append( physicalTerminationPointOids )

	# ptpEntity
	oidList.append( generate_oid_from_mib(".1.3.6.1.4.1.2544.2.5.5.10.1") )

	# entityEntry
	oidList.append( generate_oid_from_mib("1.3.6.1.4.1.2544.2.5.5.2") )

	# controlPlaneWdmEntityTable
	oidList.append( generate_oid_from_mib("1.3.6.1.4.1.2544.2.5.5.4") )

	# teUnnumLinkWdmData
	oidList.append( generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.4.7.18.1") )

	# teNumLinkWdmData
	oidList.append( generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.4.7.19.1") )

	# tunnelWdmData
	oidList.append( generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.4.7.4") )

	unusedWdmTunnelIndexOids = generate_oid_from_mib("1.3.6.1.4.1.2544.1.11.2.5.8.1")
	oidList.append( unusedWdmTunnelIndexOids )

	# containsTable
	oidList.append( generate_oid_from_mib(".1.3.6.1.4.1.2544.2.5.5.3") )

	f = open(output,"w")
	f.write("\n#ifndef _Oid_h_\n#define _Oid_h_\n\n")
	f.write("#include <net-snmp/net-snmp-config.h>\n")
	f.write("#include <net-snmp/net-snmp-includes.h>\n\n")
	for oids in oidList:
		write_oids(f,oids)
	f.write("#endif\n")
	f.close()


def main(argc,argv):

	target = "all"

	if argc > 1:
		target = argv[1]


	if target == "all":
		make_types("Types.h")
		make_oids("Oids.h")
	elif target == "types":
		make_types("Types.h")
	elif target == "oids":
		make_oids("Oids.h")
	elif target == "help":
		print "Targets: all | types | oids"
	else:
		print "Unrecognized target [%s]" % target


	
if __name__ == "__main__":
	main(len(sys.argv),sys.argv)
