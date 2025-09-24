#include "myclipboard.h"

uint64_t systemStatusFlag = 0;

void myClipboardInit(){
    __entry("myClipboardInit()");

    __log("Setup main window [%dx%d]", CLIPBOARD_HEIGHT, CLIPBOARD_WIDTH);
    createWindowContext(&mainWin, CLIPBOARD_WIDTH, CLIPBOARD_HEIGHT, CLIPBOARD_TITLE, FONT_PATH, FONT_SIZE);

    __log("Set SYS_RUNNING flag in systemStatusFlag!");
    setFlag(systemStatusFlag, SYS_RUNNING);

    __log("[+task] clipboardDaemon");
    pthread_t clipboardDaemon;
    pthread_create(&clipboardDaemon, NULL, clipboardCaptureService, NULL);
    
    __log("[+task] keyboardDaemon");
    pthread_t keyboardDaemon;
    pthread_create(&keyboardDaemon, NULL, keyboardCaptureService, NULL);

    __exit("myClipboardInit()");
}

void myClipboardExit(){
    __entry("myClipboardExit()");

    destroyWindowContext(&mainWin);

    __exit("myClipboardExit()");
}

int main(void) {
    __entry("main(void)");

    atexit(myClipboardExit);
    myClipboardInit();

    __log("Load history!");
    int index = 0;
    while(hasFlag(systemStatusFlag, SYS_RUNNING)) {
        if(hasFlag(uiStatusFlag, CB_SCROLL_DOWN)){
            entryCriticalSection(&uiFlagMutexLock);
            clrFlag(uiStatusFlag, CB_SCROLL_DOWN);
            exitCriticalSection(&uiFlagMutexLock);

            index = (index + 1) % HISTORY_SIZE;
            renderHistory(mainWin, index, 10);
        }
        if(hasFlag(uiStatusFlag, CB_SCROLL_UP)){
            entryCriticalSection(&uiFlagMutexLock);
            clrFlag(uiStatusFlag, CB_SCROLL_UP);
            exitCriticalSection(&uiFlagMutexLock);

            index = (index - 1 + HISTORY_SIZE) % HISTORY_SIZE;
            renderHistory(mainWin, index, 10);
        }
        if(hasFlag(uiStatusFlag, CB_RELOAD)){
            entryCriticalSection(&uiFlagMutexLock);
            clrFlag(uiStatusFlag, CB_RELOAD);
            exitCriticalSection(&uiFlagMutexLock);

            readClipboardHistory(historyList, HISTORY_SIZE);
            renderHistory(mainWin, index,  25);
        }
        __sleep_us(100);
    }
    __exit("main(void)");
    return 0;
}