CC=mpicc
CFLAGS=-g3 -Wall
OBJECTS=graph_main_mpi.o
LIBS=-lm

all:	main

main:	$(OBJECTS)
	$(CC) -o main $(OBJECTS) $(LIBS)

graph_main_mpi.o:	graph_main_mpi.c
	$(CC) $(CFLAGS) -c graph_main_mpi.c

clean:
	rm -v $(OBJECTS)
