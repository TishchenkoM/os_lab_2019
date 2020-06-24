CC=gcc
CFLAGS=-I.

#цель: зависимости
#[tab] команда
all : parallel_min_max

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)
	#./parallel_min_max --seed 1 --array_size 100 --pnum 4 --by_files 0 --timeout 1

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

clean :
	rm utils.o find_min_max.o sequential_min_max parallel_min_max task5.o task5.out