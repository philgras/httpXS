CC = gcc
CFLAGS = -pedantic -Wall -Wfatal-errors -std=c11
LFLAGS = -lpthread

.PHONY: all

all: httpXS clean


httpXS: httpXS.c
	$(CC) $(CFLAGS) $(LFLAGS) -o httpXS httpXS.c 

clean:
	rm -f httpXS.o
