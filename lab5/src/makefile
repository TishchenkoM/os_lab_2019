CC=gcc
CFLAGS=-I.

#цель: зависимости
#[tab] команда
all: nomutex mutex factorial deadlock

nomutex : nomutex.c
	$(CC) nomutex.c -pthread -o nomutex
mutex : mutex.c
	$(CC) mutex.c -pthread -o mutex
factorial : factorial.c
	$(CC) factorial.c -pthread -o factorial
	#./factorial --k 10 --pnum 5 --mod 15
deadlock: deadlock.c
	$(CC) deadlock.c -pthread -o deadlock
clean :
	rm nomutex mutex factorial deadlock
