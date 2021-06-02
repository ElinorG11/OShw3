#
# To compile, type "make" or make "all"
# To remove files, type "make clean"
#
OBJS = queue.o server.o request.o segel.o client.o
TARGET = server

CC = gcc
CFLAGS = -g -Wall

LIBS = -lpthread 

.SUFFIXES: .c .o 

all: server client output.cgi
	-mkdir -p public
	-cp output.cgi favicon.ico home0.html home1.html home2.html home3.html home4.html home5.html home6.html home7.html home8.html home9.html public

server: queue.o server.o request.o segel.o
	$(CC) $(CFLAGS) -o server queue.o server.o request.o segel.o $(LIBS)

client: client.o segel.o
	$(CC) $(CFLAGS) -o client client.o segel.o $(LIBS)

output.cgi: output.c
	$(CC) $(CFLAGS) -o output.cgi output.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f $(OBJS) server client queue output.cgi
	-rm -rf public
