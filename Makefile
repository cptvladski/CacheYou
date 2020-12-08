CC=gcc
CFLAGS=-Wall -luuid -lpthread
CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS = -lpthread -luuid $(shell pkg-config --libs json-c)
OBJECTS=server.o hashmap.o utils.o worker.o
.PHONY: all
all: server

.PHONY: clean
clean:
	rm  *.o server
hashmap.o: c_hashmap/hashmap.c
	$(CC) -c c_hashmap/hashmap.c -o hashmap.o
utils.o: utils.c
	$(CC) -c utils.c -o utils.o
worker.o: worker.c
	${CC} -c $(shell pkg-config --cflags json-c) worker.c -o worker.o $(LDFLAGS)
server: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o server $(LDFLAGS)
