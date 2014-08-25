CFLAGS=`pkg-config fuse sqlite3 --cflags` -std=gnu11 -g -O0 -Wall -Werror
LDFLAGS=`pkg-config fuse sqlite3 --libs`

fusesql: fusesql.o

fusesql.o: fusesql.c

clean:
	rm -f *.o fusesql
