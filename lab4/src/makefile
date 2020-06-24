CC=gcc
CFLAGS=-I.

all: task2 parallel_sum parallel_min_max

task2:
	$(CC) -o task2 task2.c

parallel_min_max: utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

parallel_sum: utils.o parallel_sum.o
	$(CC) -o parallel_sum utils.o parallel_sum.o $(CFLAGS) -pthread

parallel_sum.o:
	$(CC) -c parallel_sum.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

clean :
	rm utils.o find_min_max.o parallel_min_max task2 parallel_sum
