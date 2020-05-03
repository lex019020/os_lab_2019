CC=gcc
CFLAGS=-I.

parallel_min_max2 : utils.o find_min_max.o utils.h find_min_max.h parallel_min_max2.c
	$(CC) -o parallel_min_max2 utils.o find_min_max.o parallel_min_max2.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

libsum.so : sum.c sum.h
	$(CC) -o sum.o -c -fPIC sum.c $(CFLAGS)
	$(CC) -shared sum.o -o libsum.so

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

zombie_demo : zombie_demo.c 
	$(CC) -o zombie_demo zombie_demo.c $(CFLAGS)

parallel_sum : parallel_sum.c utils.o utils.h libsum.so
	$(CC) -o parallel_sum parallel_sum.c utils.o -pthread -L. -lsum $(CFLAGS)

all : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max2 utils.o find_min_max.o parallel_min_max2.c $(CFLAGS)
	
clean :
	rm utils.o find_min_max.o parallel_min_max2 
