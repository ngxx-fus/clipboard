#ifndef __FILE_CONTROL_H__
#define __FILE_CONTROL_H__

#include "../helper/general.h"
#include "../windowContext/wdct.h"
#include "../clipboardManager/clipboardManager.h"

typedef enum historyType_t{
    HISTORY_UNKNOWN = 0,
    HISTORY_TEXT,
    HISTORY_IMAGE
} historyType_t;

volatile int     historyPathFileNum  = 0;
volatile char    historyPathFileList[HISTORY_SIZE][MAX_PATHNAME_SIZE];

historyType_t getHistoryType(const char *path) {
    if (!path) return HISTORY_UNKNOWN;

    /// Find the last position of '.' 
    const char *ext = strrchr(path, '.');  // tìm extension
    if (!ext) return HISTORY_UNKNOWN;

    if (strcmp(ext, ".gz") == 0 || strcmp(ext, ".txt") == 0) {
        return HISTORY_TEXT;
    }
    else if (strcmp(ext, ".png") == 0 || strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
        return HISTORY_IMAGE;
    }

    return HISTORY_UNKNOWN;
}

def readTextContent(char *historyContent, size_t bufSize, const char *historyTextPath) {
    if (!historyContent || !historyTextPath) return ERR_INVALID_ARG;

    if (getHistoryType(historyTextPath) == HISTORY_IMAGE) return ERR;

    gzFile gz = gzopen(historyTextPath, "rb");
    if (!gz) {
        perror("gzopen");
        __err("[readTextContent] Can not open gz file!");
        return ERR;
    }

    size_t n = gzread(gz, historyContent, bufSize - 1);
    if (n < 0) {
        int errnum;
        const char *errmsg = gzerror(gz, &errnum);
        __err("[readTextContent] Can not open gz file! (%s)", errmsg);
        gzclose(gz);
        return ERR;
    }

    historyContent[n] = '\0';  // null-terminate
    gzclose(gz);

    return OKE;
}

def readClipboardHistory(char history[HISTORY_SIZE][MAX_PATHNAME_SIZE], int readNum) {
    __entry("readClipboardHistory(%p@[%d][%d], %d)", history, HISTORY_SIZE, MAX_PATHNAME_SIZE, readNum);
    FILE *f = fopen(HISTORY_FILE, "r");
    if (!f) {
        __err("[readClipboardHistory] Failed to read history!");
        __exit("readClipboardHistory() : ERR_FILE_READ_FAILED");
        return ERR_FILE_READ_FAILED;
    }

    historyPathFileNum = 0;
    char line[512];
    while (historyPathFileNum < readNum && fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = '\0'; /// remove \r\n 
        __log1("[readClipboardHistory] line = %s", line);
        strncpy(history[historyPathFileNum], line, MAX_PATHNAME_SIZE - 1);
        history[historyPathFileNum][MAX_PATHNAME_SIZE - 1] = '\0';
        historyPathFileNum++;
    }

    fclose(f);
    __exit("readClipboardHistory() : %d", historyPathFileNum);
    return historyPathFileNum;
}

#endif