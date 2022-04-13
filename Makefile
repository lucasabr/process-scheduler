# makefile for scheduling program        
CC=gcc
DEPS = scheduler.h queue.h process.h
OBJ = scheduler.o queue.o
CFLAGS=-Wall
STD_FLAG=-std=c99

scheduler.o: scheduler.c scheduler.h 
	$(CC) $(CFLAGS) scheduler.c queue.c -lm $(STD_FLAG) -o scheduler

queue.o: queue.c queue.h queue.h
	$(CC) $(CFLAGS) -c queue.c

clean:
	rm -rf *.o    

