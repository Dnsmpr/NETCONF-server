CC=gcc
CFLAGS=-Wall -g -I./parsing -I./parsing/DataStructures -I./networking
LIBS=-lxml2 -lmbedtls -lmbedx509 -lmbedcrypto
TARGET=run

SRCS=$(wildcard *.c parsing/*.c parsing/DataStructures/*.c networking/*.c)
OBJS=$(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

parsing/%.o: parsing/%.c
	$(CC) $(CFLAGS) -c $< -o $@

parsing/DataStructures/%.o: parsing/DataStructures/%.c
	$(CC) $(CFLAGS) -c $< -o $@

networking/%.o: networking/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
