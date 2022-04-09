CC=gcc
CFLAGS=-I.

all: mutex without_mutex factorial deadlock

mutex: mutex.c
	$(CC) -pthread -o mutex mutex.c $(CFLAGS)

without_mutex: mutex.c
	$(CC) -pthread -o without_mutex without_mutex.c $(CFLAGS)

factorial: factorial.c
	$(CC) -pthread -o factorial factorial.c

deadlock: deadlock.c
	$(CC) -pthread -o deadlock deadlock.c

clean :
	rm mutex without_mutex factorial deadlock
