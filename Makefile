CC=gcc
CFLAGS=-Wall -luuid
CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS = -luuid $(shell pkg-config --libs json-c)
OBJECTS=server.o
.PHONY: all
all: server

.PHONY: clean
clean:
	rm  *.o server


server: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o server $(LDFLAGS)