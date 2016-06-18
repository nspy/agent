#!/usr/bin/python

import string
import shlex
import sys
import os
import re

OID_FILE = "oid.h"
MIB_HEADER = "MIB.h"
MIB_SOURCE = "MIB.c"
TYPES = "Types.h"

Types = []

Pattern = re.compile("(.*)_*")

TransTable = string.maketrans("-","_")

Oid_Node = """
typedef struct
{
	const char*		Name;
	const oid		Oid[MAX_OID_LEN];
	size_t			Size;
	unsigned char	Type;
} OID_Node;
"""

Lookup_Declaration = """
int
MIB_Lookup(const char *Name, oid **Oid, size_t *Size, unsigned char *Type);
"""

Lookup_Definition = """
int 
MIB_Lookup(const char *Name, oid **Oid, size_t *Size, unsigned char *Type)
{
    int i;
    
    if (Name == NULL || Oid == NULL || Size == NULL || Type == NULL)
    {
        return -1;
    }
    
    for(i = 0; i < __Entries; i++)
	{
		if (!strcmp(Name, __Name[i]))
		{
			*Oid 	= __Oid[i];
			*Size 	= __Size[i];
			*Type 	= __Type[i];
			return (i);
		}
	}

	return (-1);
}
"""

def generate_oids():

	try:
		src = open(OID_FILE, "r")
		mibh = open(MIB_HEADER, "w")
		mibc = open(MIB_SOURCE, "w")

		mibc.write("#include \"%s\"\n\n" % (MIB_HEADER))
		mibh.write("#ifndef __MIB_H__\n")
		mibh.write("#define __MIB_H__\n\n")

		mibh.write("#include <net-snmp/net-snmp-config.h>\n#include <net-snmp/net-snmp-includes.h>\n\n")

		mibh.write(Oid_Node)

		mibh.write(Lookup_Declaration)

		lines = src.readlines()

		for line in lines:
			if len(line) == 0:
				continue
			tokens = shlex.split(line)
			if len(tokens) == 0:
				continue
			mibc.write( "const oid %s_oid[] = %s;\n" % (tokens[0], tokens[1]) )
			mibc.write( "const size_t %s_oid_size = %s;\n" % (tokens[0], tokens[2]) )
			mibh.write( "extern const oid %s_oid[];\n" % (tokens[0]) )
			mibh.write( "extern const size_t %s_oid_size;\n" % (tokens[0]) )

		for line in lines:
			tokens = shlex.split(line)
			module = tokens[4].translate(TransTable)
			mibh.write( "extern OID_Node %s_%s;\n" % ( module, tokens[0] ) )

		mibh.write("#endif\n")
	except Exception, e:
		print e
		return False

	n = len(lines)

	mibc.write( "size_t __Entries = %d;\n" % (len(lines)) )

	i = 0
	## generate a table of MIB-modules ##
	mibc.write( "const char* __Name[ ] = {\n" )
	for line in lines:
		tokens = shlex.split(line)
		mibc.write( "\t\"%s::%s\", // %d\n" % (tokens[4], tokens[0], i) )
		i = i + 1
	mibc.write("\t\"\",\n")
	mibc.write( "};\n" )

	i = 0
	## generate a table of MIB-modules ##
	mibc.write( "unsigned char __Type[ ] = {\n" )
	for line in lines:
		tokens = shlex.split(line)
		mibc.write( "\t%s, // %d\n" % ( tokens[3], i ) )
		i = i + 1
	mibc.write( "};\n" )

	## generate a table of MIB-modules ##
	#mibc.write( "oid __Oid[%d][ ] = {\n" % (len(lines)) )
	i = 0
	mibc.write( "oid __Oid[][MAX_OID_LEN] = {\n" )
	for line in lines:
		tokens = shlex.split(line)
		mibc.write( "\t%s, // %d\n" % ( tokens[1], i ) )
		i = i + 1
	mibc.write( "};\n" )

	## generate a table of MIB-modules ##
	mibc.write( "size_t __Size[ ] = {\n" )
	for line in lines:
		tokens = shlex.split(line)
		mibc.write( "\t%s,\n" % ( tokens[2] ) )
	mibc.write( "};\n" )

	mibc.write(Lookup_Definition)

	## generate a table of MIB-modules ##
	for line in lines:
		tokens = shlex.split(line)

		module = tokens[4].translate(TransTable)

		mibc.write( "OID_Node %s_%s = {\n" % ( module, tokens[0] ) )
		mibc.write( "\t\"%s::%s\",\n" % ( tokens[4], tokens[0] ) )
		mibc.write( "\t%s,\n" % ( tokens[1] ) )
		mibc.write( "\t%s,\n" % ( tokens[2] ) )
		mibc.write( "\t%s\n" % ( tokens[3] ) )
		mibc.write( "};\n\n" )

	mibc.write( "OID_Node *Nodes[] = {\n", )
	for line in lines:
		tokens = shlex.split(line)
		module = tokens[4].translate(TransTable)
		mibc.write( "\t&%s_%s,\n" % ( module, tokens[0] ) )
	mibc.write( "NULL };\n" ) 
	
	src.close()
	mibc.close()
	mibh.close()
	
	
	print "Total oid number = %d" % (len(lines))

	return True

def generate_types():

	try:
		fp = open(TYPES, "w")
		
		fp.write("#ifndef __TYPES_H__\n")
		fp.write("#define __TYPES_H__\n\n")
		
		files = os.listdir("./types")
		for f in files:
			generate_types2( "./types/%s" % (f), f )
	
		for t in Types:
			write_enums(fp, t)

		fp.write("#endif\n")
	except Exception, e:
		print e
		return False
	return True

def generate_types2(path, name):
	global Types

	try:
		h = open(path)

		types = []
		values = []

		lines = h.readlines()

		for line in lines:
			if len(line) == 0:
				continue

			tokens = shlex.split(line)

			if len(tokens) == 0:
				continue

			match = re.match(Pattern, tokens[0])

			if None == match:
				continue

			types.append(match.group(0))
			values.append(int(tokens[1]))

		Types.append( [name, types, values] )
	except Exception, e:
		print e
		return False
	return True

def write_enums(fp, lst):
	try:
		fp.write("typedef enum\n")
		fp.write("{\n")

		for i in range(0, len(lst[1])):
			fp.write("\t%s = %d,\n" % (lst[1][i], lst[2][i]))
		fp.write("\t%s_MAX,\n" % (lst[0]))

		fp.write("} %s_t;\n\n" % (lst[0]) )
	except Exception, e:
		print e
		return False
	return True

def main(argc, argv):
	generate_oids()
	generate_types()
	return 0

if __name__ == "__main__":
	main(len(sys.argv), sys.argv)
