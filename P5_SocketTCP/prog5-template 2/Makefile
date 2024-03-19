CC = gcc -g #-O3
CFLAGS = -Wall -Wextra -pedantic

all: mixed

remote: RPFLAGS += -DRP_TYPE=RP_REMOTE
remote: build

local: RPFLAGS += -DRP_TYPE=RP_LOCAL
local: build

mixed: RPFLAGS += -DRP_TYPE=RP_MIXED
mixed: build

build: rp_client.o rp_user1 rp_user2 rp_server


rp_user1: rp_client.h rp_user1.c
	$(CC) $(CFLAGS) $(RPFLAGS) rp_client.o rp_user1.c -o rp_user1

rp_user2: rp_client.h rp_user2.c
	$(CC) $(CFLAGS) $(RPFLAGS) rp_client.o rp_user2.c -o rp_user2

rp_client.o: rp_calls.h rp_client.c
	$(CC) $(CFLAGS) -c rp_client.c

rp_server: rp_calls.h rp_server.c
	$(CC) $(CFLAGS) rp_server.c -o rp_server


clean:
	rm rp_user1 rp_user2 rp_client.o rp_server

submission:
	zip prog5.zip Makefile rp_calls.h rp_procs.h rp_client.h rp_client.c rp_user1.c rp_user2.c rp_server.c
