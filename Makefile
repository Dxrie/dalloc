CC = gcc

CFLAGS = -Wall -Wextra -Werror -Iinclude -g

SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

SRC = $(wildcard $(SRC_DIR)/*.c)
TEST = $(wildcard $(TEST_DIR)/*.c)

LIB_NAME = libdalloc.a
TEST_BIN = test_dalloc

OBJ = $(SRC:.c=.o)

all: $(LIB_NAME)

$(LIB_NAME): $(OBJ)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(LIB_NAME)
	$(CC) $(CFLAGS) $(TEST) -L. -ldalloc -o $(TEST_BIN)

run: test
	./$(TEST_BIN)

clean:
	rm -f $(SRC_DIR)/*.o $(LIB_NAME) $(TEST_BIN)

re: clean all

.PHONY: all test run clean re
