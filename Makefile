# ==== Compiler & Flags ====
CC        := gcc
CFLAGS    := -Wall -Wextra -g
LIBS      := -lSDL2 -lSDL2_ttf -lpthread -lxcb -lxcb-keysyms -lX11 -lz
ROOT_DIR  := $(realpath .)
INCFLAGS  := \
	-I$(ROOT_DIR)/helper \
	-I$(ROOT_DIR)/log \
	-I$(ROOT_DIR)/clipboardManager \
	-I$(ROOT_DIR)/windowContext \
	-I$(ROOT_DIR)/font \
	-I$(ROOT_DIR)/config

# ==== Colors ====
RESET  := \033[0m
BOLD   := \033[1m
RED    := \033[31m
GREEN  := \033[32m
YELLOW := \033[33m
BLUE   := \033[34m

# ==== Output & Directories ====
TARGET    := myClipboard
BUILD_DIR := build

# ==== Source Files ====
SRC := \
	myClipboard.c \
	$(wildcard helper/*.c) \
	$(wildcard log/*.c) \
	$(wildcard clipboardManager/*.c) \
	$(wildcard windowContext/*.c) \
	$(wildcard font/*.c)

# ==== Object Files ====
OBJ := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC))

# ==== Default Target ====
all: $(TARGET)

# ==== Build Executable ====
$(TARGET): $(OBJ)
	@echo "$(BOLD)$(BLUE)[LD]$(RESET) Linking $(YELLOW)$@$(RESET)"
	$(CC) $(OBJ) -o $@ $(LIBS)
	@echo "$(GREEN)[OK]$(RESET) Build complete: $(YELLOW)$@$(RESET)"

# ==== Compile Stage ====
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "$(BOLD)$(GREEN)[CC]$(RESET) Compiling $(YELLOW)$<$(RESET)"
	$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $@

# ==== Utilities ====
.PHONY: all clean run leak_check dirs

dirs:
	@mkdir -p $(BUILD_DIR)

clean:
	@echo "$(BOLD)$(RED)[CLEAN]$(RESET) Removing build files"
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "$(GREEN)[OK]$(RESET) Clean done"

run: $(TARGET)
	@echo "$(BOLD)$(BLUE)[RUN]$(RESET) Executing $(YELLOW)./$(TARGET)$(RESET)"
	./$(TARGET)

# ==== Leak check ====
leak_check: $(TARGET)
	@echo "$(BOLD)$(YELLOW)[VALGRIND]$(RESET) Running memory check"
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)
