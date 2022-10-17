
CC = gcc
LDFLAGS=-lgdi32
 
EXECUTABLE = main

src = $(wildcard *.c) $(wildcard *.h)

# .o files are buggy lol

all: program
 
program: $(src)
	$(CC) -o $(EXECUTABLE) $^ $(LDFLAGS)
