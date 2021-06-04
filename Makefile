#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = queue.o server.o request.o segel.o clientStatic.o clientDynamic.o clientError.o client.o
TARGET = server

CC = gcc
CFLAGS = -g -Wall

LIBS = -lpthread 

.SUFFIXES: .c .o 

all: server client output.cgi
	-mkdir -p public
	-cp output.cgi favicon.ico home.html public

server: queue.o server.o request.o segel.o
	$(CC) $(CFLAGS) -o server queue.o server.o request.o segel.o $(LIBS)

client: clientStatic.o clientDynamic.o clientError.o client.o segel.o
	$(CC) $(CFLAGS) -o client clientStatic.o clientDynamic.o clientError.o client.o segel.o $(LIBS)

output.cgi: output.c
	$(CC) $(CFLAGS) -o output.cgi output.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f $(OBJS) server client queue clientStatic clientDynamic clientError output.cgi
	-rm -rf public
