CC       := gcc
CFLAGS   := -Wall -g
LDFLAGS  := -lSDL2 -lSDL2_ttf -lpthread -lxcb -lz -lxcb-keysyms -lX11
INCFLAGS := \
			-Ihelper						\
			-Ilog							\
			-Iclipboard 					\
			-IUI/wdct						\
			-IUI							\

APP      := myclipboard.c
BIN      := myclipboard

SRC      := $(APP) 							\
			$(wildcard helper/*.c)			\
			$(wildcard log/*.c)				\
			$(wildcard clipboard/*.c) 		\
			$(wildcard UI/*.c)				\
			$(wildcard UI/wdct/*.c)			\

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