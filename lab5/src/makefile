CC=gcc
CFLAGS=-I.

mutex : mutex.c
	$(CC) -o mutex mutex.c -pthread $(CFLAGS)

factorial : factorial.c 
	$(CC) -o factorial factorial.c -pthread $(CFLAGS)

deadlock : deadlock.c 
	$(CC) -o deadlock deadlock.c -pthread $(CFLAGS)

all : mutex.c factorial.c deadlock.c
	$(CC) -o mutex mutex.c -pthread $(CFLAGS)
	$(CC) -o factorial factorial.c -pthread $(CFLAGS)
	$(CC) -o deadlock deadlock.c -pthread $(CFLAGS)

clean : 
	rm mutex factorial deadlock