CC=gcc
CFLAGS=-Wall -luuid
CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS = -luuid $(shell pkg-config --libs json-c)
OBJECTS=server.o hashmap.o
.PHONY: all
all: server

.PHONY: clean
clean:
	rm  *.o server
hashmap.o: c_hashmap/hashmap.c
	gcc -c c_hashmap/hashmap.c -o hashmap.o

server: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o server $(LDFLAGS)