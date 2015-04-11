# Author: TJ Maynes and Chris Migut
# File: Makefile

CFLAGS = -Wall -g -o
PROGRAM = client server

main:
	gcc server.c $(CFLAGS) server -lpthread
	gcc client.c $(CFLAGS) client -lpthread

clean:
	rm -f $(PROGRAM)
