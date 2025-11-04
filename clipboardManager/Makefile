# clipboardManager/Makefile (test harness)
# Build test binary .test.cbm.c and link with prebuilt objects, zlib and SDL2

CC = gcc
CFLAGS = -std=gnu11 -O2 -Wall -I. -I../helper -I../include -I../font $(shell sdl2-config --cflags)
LDFLAGS = -lxcb -lz $(shell sdl2-config --libs)

TEST_SRC = .test.cbm.c
TEST_BIN = test_clipboardManager

# Link with prebuilt objects in ../build/*
OBJS = \
	../build/clipboardManager/clipboardManager.o \
	../build/font/cbFont.o \
	../build/helper/helper.o \
	../build/log/log.o \
	../build/myClipboard.o \
	../build/windowContext/wdct.o

.PHONY: all clean

all: $(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(OBJS)
	$(CC) $(CFLAGS) $(TEST_SRC) $(OBJS) -o $(TEST_BIN) $(LDFLAGS)

clean:
	rm -f $(TEST_BIN)
