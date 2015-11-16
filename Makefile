CC = gcc
CFLAGS = -pedantic -Wall -Wfatal-errors -std=c11
LFLAGS = 

.PHONY: all

all: httpXS clean


httpXS: httpXS.c
	$(CC) $(CFLAGS) -o httpXS httpXS.c $(LFLAGS)

clean:
	rm -f httpXS.o
