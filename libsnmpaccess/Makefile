#
# Makefile
#

CC = gcc

CPPFLAGS = -I./

ifneq ($(DEBUG), NO)
CFLAGS += -g
endif

ifeq ($(IMPORT_MIB), YES)
CFLAGS += -DIMPORT_MIB
endif

ifeq ($(BYTE_ORDER), YES)
CFLAGS += -DBYTE_ORDER_64
endif

#CFLAGS += -fno-strict-aliasing -O2 -Wall -fPIC
CFLAGS += -Wall -fPIC

LDFLAGS = -lnetsnmp -lpthread -lrt

lib_OBJ = ./SnmpAccess/Interface.o \
	./SnmpAccess/Inventory.o \
	./SnmpAccess/List.o \
	./SnmpAccess/Vector.o \
	./SnmpAccess/Utils.o \
	./SnmpAccess/Channel.o \
	./SnmpAccess/Timer.o \
	./SnmpAccess/Queue.o \
	./SnmpAccess/TrapHandler.o \
	./SnmpAccess/Common.o \
	./SnmpAccess/Threads.o \
	./SnmpAccess/Core.o \
	./SnmpAccess/Log.o \
	./SnmpAccess/Resources.o \
	./SnmpAccess/OpenFlow.o \
	./SnmpAccess/Worker.o \
	./SnmpAccess/Lightpath.o \

ifneq ($(IMPORT_MIB), YES)
lib_OBJ += ./SnmpAccess/MIB.o
endif

lib_openflow = ./openflow/lib/libopenflow.a

OBJ = $(lib_OBJ)

SONAME = libSnmpAccess.so

BIN = SnmpDemo

LIB = $(SONAME)
	
all: lib	
	$(CC) $(CPPFLAGS) $(CFLAGS) `pkg-config libxml-2.0 --cflags` -o $(BIN) ./SnmpAccess/Main.c `pkg-config libxml-2.0 --libs` -L. -lSnmpAccess
	
lib: $(lib_OBJ)	
	$(CC) $(CPPFLAGS) $(CFLAGS) -shared -Wl,-soname=$(SONAME) -o $(LIB) $(lib_OBJ) $(lib_openflow) $(LDFLAGS)

agent: lib
	@cd ./Agent; make

prepare:
	@cd ./GEN; ./prepare.sh; ./generate.py; cp ./MIB.h ./MIB.c ./Types.h ../SnmpAccess

demos: lib
	$(CC) $(CPPFLAGS) $(CFLAGS) -o Workers ./SnmpAccess/WorkerDemo.c -L. -lSnmpAccess
	$(CC) $(CPPFLAGS) $(CFLAGS) -o Workers2 ./SnmpAccess/WorkerDemo2.c -L. -lSnmpAccess
	$(CC) $(CPPFLAGS) $(CFLAGS) -o Lightpath ./SnmpAccess/LightpathDemo.c -L. -lSnmpAccess
	
tests: lib
	$(CC) $(CPPFLAGS) $(CFLAGS) -o VectorTest ./SnmpAccess/VectorTest.c -L. -lSnmpAccess

clean:
	@rm -f $(OBJ) $(BIN) $(LIB)
	@rm -f Workers Lightpath
	@cd ./Agent; make clean
	
help:
	@printf "make (all|lib|clean)\n"
	@printf "options:\n"
	@printf "\tIMPORT_MIB=YES - use MIB defintion from file\n"
	@printf "\tBYTE_ORDER=YES - fix for hto{b,l}e64 & {b,l}64etoh\n"
