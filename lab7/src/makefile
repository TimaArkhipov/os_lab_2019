CC=gcc
CFLAGS=-I.


all: tcp udp

tcp: tcpclient tcpserver

tcpclient : 
	$(CC) -c tcpclient.c -o tcpclient.o $(CFLAGS)
	$(CC) -o tcpclient tcpclient.o $(CFLAGS)

tcpserver : 
	$(CC) -c tcpserver.c -o tcpserver.o $(CFLAGS)
	$(CC) -o tcpserver tcpserver.o $(CFLAGS)

udp: udpclient udpserver

udpclient : 
	$(CC) -c udpclient.c -o udpclient.o $(CFLAGS)
	$(CC) -o udpclient udpclient.o $(CFLAGS)

udpserver : 
	$(CC) -c udpserver.c -o udpserver.o $(CFLAGS)
	$(CC) -o udpserver udpserver.o $(CFLAGS)
	
libconn: 
	$(CC) -o conn.o -c conn.c $(CFLAGS)
	ar rcs libconn.a conn.o
	
clean :
	rm conn.o udpserver.o udpclient.o tcpserver.o tcpclient.o
	
clean_app: clean
	rm libconn.a udpserver udpclient tcpserver tcpclient