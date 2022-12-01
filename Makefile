CC = gcc
CFLAGS = -g -Wall -lm
 
all: build

build:
	$(CC) -o tema3 tema3.c $(CFLAGS) -std=c99 

.PHONY : clean
clean :
	rm -f tema3

