CC       := gcc
CFLAGS   := -Wall -g
LDFLAGS  := -lSDL2 -lSDL2_ttf -lpthread -lxcb -lz -lxcb-keysyms -lX11
INCFLAGS := \
			-IfileManager						\

APP      := myclipboard.c
BIN      := myclipboard

SRC      := $(APP) 							\
			$(wildcard fileManager/*.c)		\

OBJ      := $(SRC:.c=.o)

all: $(BIN)

.PHONY: all clean exec leak_check

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

exec:
	./$(BIN)