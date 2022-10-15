
CC = gcc
LDFLAGS=-lgdi32
 
EXECUTABLE = main

src = $(wildcard *.c)
obj = $(src:.c=.o)
 
all: program
 
program: $(obj)
	$(CC) -o $(EXECUTABLE) $^ $(LDFLAGS)

cleam: $(obj)
	rm $(obj)