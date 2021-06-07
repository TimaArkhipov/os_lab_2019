CC=gcc
CFLAGS=-I.  #C помощью `-I.' вы можете указать поиск в каталоге, который был текущим, когда вызывался компилятор

parallel_sum : utils.o sum_th.o utils.h sum_th.h
	$(CC) utils.o sum_th.o parallel_sum.c -o parallel_sum -lpthread $(CFLAGS)

process_memory: 
	$(CC) process_memory.c -o process_memory $(CFLAGS)

zombie:
	$(CC) zombie.c -o zombie $(CFLAGS)

utils.o : utils.h
	$(CC) -c utils.c -o utils.o $(CFLAGS)

clean :
	rm zombie process_memory utils.o sum_th.o parallel_sum parallel_min_max