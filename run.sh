#!/bin/zsh

TARGET=./myClipboard
SRC_DIR=./
LOGFILE=run.log

BOLD="\033[1m"
NORM="\033[0m"
YELLOW="\033[1;33m"
GREEN="\033[1;32m"
RED="\033[1;31m"

echo -e "\n${BOLD}${YELLOW}[Auto]${NORM} Clearing screen..."
clear 

echo -e "\n${BOLD}${YELLOW}[Auto]${NORM} Rebuilding..."
make -j8 clean >/dev/null 2>&1

if make -j8 -s; then
    echo -e "${GREEN}[OK] Build successful — running under GDB...${NORM}"

    # Xóa log cũ
    rm -f "$LOGFILE"

    # Ghi log + in realtime
    echo -e "\n${YELLOW}[GDB] Running... (output logged to ${LOGFILE})${NORM}\n"
    gdb -batch -ex run -ex "bt" -ex quit --args "$TARGET" 2>&1 | tee "$LOGFILE"


    echo -e "\n${GREEN}[OK] GDB session finished. Log saved to ${LOGFILE}.${NORM}"
else
    echo -e "${RED}[ERR] Build failed.${NORM}"
fi
