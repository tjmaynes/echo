# Author: TJ Maynes and Chris Migut
# File: Makefile

CFLAGS = -Wall -o
PROGRAM = server client

main:
	gcc server.c $(CFLAGS) server -lpthread -lrt
	gcc client.c $(CFLAGS) client -lpthread -lrt

client:
	./client

server:
	./server

clean:
	rm -f $(PROGRAM)
