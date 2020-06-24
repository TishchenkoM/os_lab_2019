CC=gcc
CFLAGS=-I.

all : lib server client

lib: include/header.c
	$(CC) -o include/header.o -c include/header.c $(CFLAGS)
	ar rcs include/libhead.a include/header.o

server: server.c include/libhead.a
	$(CC) -o server.o -c server.c $(CFLAGS)
	$(CC) -o server server.o -Linclude -lhead -pthread  $(CFLAGS)

client: client.c include/libhead.a
	$(CC) -o client.o -c client.c $(CFLAGS)
	$(CC) -o client client.o -Linclude -lhead $(CFLAGS)

clean:
	rm client server client.o server.o include/header.o include/libhead.a
