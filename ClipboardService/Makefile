CC       	:= gcc
C_FLAGS   	:= -Wall -g
LD_FLAGS  	:= -lSDL2 -lSDL2_ttf -lpthread -lxcb -lz -lxcb-keysyms -lX11
INCLUDE_FLAGS := \
			-Ihelper						\
			-Ilog							\
			-Iconfig 						\
			-Iclipboard 					\

APP      	:= clipboardService.c
BIN      	:= clipboardService

SRC      	:= $(APP) 						\
			$(wildcard helper/*.c)			\
			$(wildcard log/*.c)				\
			$(wildcard config/*.c)			\
			$(wildcard clipboard/*.c) 		\

OBJ      	:= $(SRC:.c=.o)

all: $(BIN)

.PHONY: all clean exec leak_check

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LD_FLAGS)

%.o: %.c
	$(CC) $(C_FLAGS) $(INCLUDE_FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

exec:
	./$(BIN)

leak_check:
	valgrind --leak-check=summary ./$(BIN)