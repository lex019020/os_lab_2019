CC=gcc
CFLAGS=-I.

server : server.c libmultmod.so
	$(CC) -o server server.c -pthread -L. -lmultmod $(CFLAGS)

libmultmod.so : mult_modul.c mult_modul.h
	$(CC) -o multmod.o -c -fPIC mult_modul.c $(CFLAGS)
	$(CC) -shared multmod.o -o libmultmod.so

client : client.c libmultmod.so
	$(CC) -o client client.c -pthread -L. -lmultmod $(CFLAGS)

all : client server
	echo я сделяль

america_great_again:
	make clean
	make all

clean : 
	rm server
	rm client
	rm libmultmod.so