# Makefile for dataManager test with zlib

CC = gcc
CFLAGS = -std=gnu11 -O2 -Wall -I.
LDFLAGS = -lz

SRC = dataManager.c
OBJ = dataManager.o

TEST_SRC = .test.c ../log/log.c 
TEST_BIN = test_dataManager

.PHONY: all clean exec

all: $(TEST_BIN) exec

# Compile main object
$(OBJ): $(SRC) dataManager.h
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

# Compile and link test binary
$(TEST_BIN): $(OBJ) $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) $(OBJ) -o $(TEST_BIN) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TEST_BIN)

exec:
	./$(TEST_BIN)