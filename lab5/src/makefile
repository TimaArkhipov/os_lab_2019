CC=gcc
CFLAGS=-I.  #C помощью `-I.' вы можете указать поиск в каталоге, который был текущим, когда вызывался компилятор

mutex : mutex.o
	$(CC) mutex.o -o mutex -lpthread $(CFLAGS)

mutex.o :
	$(CC) -c mutex.c -o mutex.o -lpthread $(CFLAGS)

clean :
	rm mutex.o mutex