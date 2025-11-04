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
    int itemNum = 1;
    itemNum = readClipboardHistory(historyList, HISTORY_SIZE);
    renderHistory(mainWin, cbba, index);

    while(hasFlag(systemStatusFlag, SYS_RUNNING)) {
        if(hasFlag(uiStatusFlag, CPMW_SCROLL_ITEM_DOWN)){
            entryCriticalSection(&uiFlagMutexLock);
            clrFlag(uiStatusFlag, CPMW_SCROLL_ITEM_DOWN);
            exitCriticalSection(&uiFlagMutexLock);

            index = (index + 1) % itemNum;
            renderHistory(mainWin, cbba, index);
        }
        if(hasFlag(uiStatusFlag, CPMW_SCROLL_ITEM_UP)){
            entryCriticalSection(&uiFlagMutexLock);
            clrFlag(uiStatusFlag, CPMW_SCROLL_ITEM_UP);
            exitCriticalSection(&uiFlagMutexLock);

            index = (index - 1 + itemNum) % itemNum;
            renderHistory(mainWin, cbba, index);
        }
        if(hasFlag(uiStatusFlag, CPMW_RELOAD_ITEM)){
            entryCriticalSection(&uiFlagMutexLock);
            clrFlag(uiStatusFlag, CPMW_RELOAD_ITEM);
            exitCriticalSection(&uiFlagMutexLock);

            itemNum = readClipboardHistory(historyList, HISTORY_SIZE);
            renderHistory(mainWin, cbba, index);
        }
        __sleep_us(100);
    }
    __exit("main(void)");
    return 0;
}