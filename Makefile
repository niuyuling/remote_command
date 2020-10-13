CROSS_COMPILE ?= 
CC := $(CROSS_COMPILE)gcc
STRIP := $(CROSS_COMPILE)strip
CFLAGS += -g -O2 -Wall -I ../libconf -L ../libconf
LIBS = -pthread -lconf -static
SER := server
CLI := client

all: server client

server: server.o popen.o conf.o
	$(CC) $(CFLAGS) -o $(SER) $^ $(LIBS)
    
client: client.o
	$(CC) $(CFLAGS) -o $(CLI) $^ $(LIBS)
    
.c.o:
	$(CC) $(CFLAGS) -c $< $(LIBS)

clean:
	rm -rf *.o
	rm $(SER) $(CLI)

