CC=gcc
CFLAGS=-Wall -g -I./parsing -I./parsing/DataStructures -I./parsing/module -I./networking -I./Unity
LIBS=-lxml2 -lmbedtls -lmbedx509 -lmbedcrypto
TARGET=run
TEST_EXEC=run_tests

SRCS=$(wildcard *.c parsing/*.c parsing/DataStructures/*.c parsing/module/*.c networking/*.c)
OBJS=$(SRCS:.c=.o)

TEST_SRCS=$(wildcard tests/*.c Unity/unity.c)
TEST_OBJS=$(TEST_SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

parsing/%.o: parsing/%.c
	$(CC) $(CFLAGS) -c $< -o $@

parsing/DataStructures/%.o: parsing/DataStructures/%.c
	$(CC) $(CFLAGS) -c $< -o $@

parsing/module/%.o: parsing/module/%.c
	$(CC) $(CFLAGS) -c $< -o $@

networking/%.o: networking/%.c
	$(CC) $(CFLAGS) -c $< -o $@

Unity/%.o: Unity/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build the unit tests
$(TEST_EXEC): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $(TEST_EXEC) $(TEST_OBJS) $(LIBS)

tests/%.o: tests/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the unit tests
test: $(TEST_EXEC)
	./$(TEST_EXEC)

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS) $(TEST_EXEC)

.PHONY: all clean test
