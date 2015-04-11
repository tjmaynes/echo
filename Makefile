# Author: TJ Maynes and Chris Migut
# File: Makefile

CFLAGS = -Wall -o
PROGRAM = client server

main:
	gcc server.c $(CFLAGS) server -lpthread
	gcc client.c $(CFLAGS) client -lpthread

client:
	./client

server:
	./server

clean:
	rm -f $(PROGRAM)
