CC=mpicc
CFLAGS=-g3 -Wall
OBJECTS=graph_main_openmp.o
LIBS=-lm -fopenmp

all:	main

main:	$(OBJECTS)
	$(CC) -o main $(OBJECTS) $(LIBS)

graph_main_openmp.o:	graph_main_openmp.c
	$(CC) $(CFLAGS) -c graph_main_openmp.c

clean:
	rm -v $(OBJECTS)
