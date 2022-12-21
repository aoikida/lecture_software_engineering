T1 = simple_database

CC = g++
CFLAGS = -g -c -Wall -O2 -std=c++11
LDFLAGS = -lm -pthread 

OBJS = simple_database.o transaction.o

all: $(T1) 

$(T1): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

.cc.o: 
	$(CC) $(CFLAGS) $<

clean:
	rm -f *~ *.o *.exe *.stackdump $(T1) 
