CC=gcc
CFLAGS=-g3 -Wall
OBJECTS=life.o
LIBS=-lm

all:	main

main:	$(OBJECTS)
	$(CC) -o main $(OBJECTS) $(LIBS)

life.o:	life.c
	$(CC) $(CFLAGS) -c life.c

clean:
	rm -v $(OBJECTS)
