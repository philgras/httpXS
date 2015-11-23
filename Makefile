CC = gcc
CFLAGS =-pedantic -Wall -Wfatal-errors -std=c11 -D_POSIX_SOURCE -g -O0
LFLAGS = #-lpthread
TESTDIR = test
BINDIR = bin

.PHONY: all

all: httpXS test clean


httpXS: *.c *.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(BINDIR)/httpXS httpXS.c 

test: $(TESTDIR)/*.c *.c *.h
#hxs_conn_list_test
	$(CC) $(CFLAGS) $(LFLAGS) -o $(BINDIR)/hxs_conn_list_test $(TESTDIR)/hxs_conn_list_test.c hxs_connection.c

clean:
	rm -f *.o
	rm -f $(TESTDIR)/*.o