CC = gcc
CFLAGS =-pedantic -Wall -Wfatal-errors -std=c11 -D_POSIX_SOURCE -g -O0
LFLAGS = 
LIBS = -ljansson
TESTDIR = test
BINDIR = bin

.PHONY: all

all: httpXS test clean


httpXS: *.c *.h
	$(CC) $(CFLAGS) $(LFLAGS) -o $(BINDIR)/httpXS httpXS.c hxs_connection.c hxs_service.c hxs_http.c http-parser/http_parser.o hxs_server.c $(LIBS)

test: $(TESTDIR)/*.c *.c *.h
#hxs_conn_list_test
	#$(CC) $(CFLAGS) $(LFLAGS) -o $(BINDIR)/hxs_conn_list_test $(TESTDIR)/hxs_conn_list_test.c hxs_connection.c
	#$(CC) $(CFLAGS) $(LFLAGS) -o $(BINDIR)/hxs_list_test $(TESTDIR)/hxs_list_test.c
	$(CC) $(CFLAGS) $(LFLAGS) -o $(BINDIR)/hxs_map_test $(TESTDIR)/hxs_map_test.c
clean:
	rm -f *.o
	rm -f $(TESTDIR)/*.o