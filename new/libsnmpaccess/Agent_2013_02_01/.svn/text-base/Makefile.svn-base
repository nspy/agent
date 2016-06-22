
CC = gcc

CPPFLAGS = -I../ `pkg-config libxml-2.0 --cflags`

CFLAGS = -Wall -g

LDFLAGS =  `pkg-config libxml-2.0 --libs` -L../ -lSnmpAccess

OBJ = Agent.o Main.o

BIN = Agent

all: $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(BIN) $(OBJ)