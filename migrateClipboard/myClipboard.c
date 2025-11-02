#include "include/init.h"


int main(void) {
    __SET_SYSTEM_INIT__;
    __entry("main(void)");

    atexit(myClipboardExit);
    myClipboardInit();

    __SET_SYSTEM_RUN__;

    /* Manually move to next initializing stage 
       if not automatically moved */
    struct timespec ts_advance = {.tv_sec = 0, .tv_nsec = __MSEC(1) /*__MSEC(500)*/};
    /* advance init stages until we reach RUNNING (or stopped) */
    while (IS_SYSTEM_INIT) {
        __log("[main] Auto move next init stage (--> %d)", atomic_load(&systemStage)+1);
        __MV_2NEXT_INIT_STAGE__;
        nanosleep(&ts_advance, NULL);
    }

    __MV_2NEXT_INIT_STAGE__;
    

    cbmwSetRenderTargetOnScreen();
    cbmwClearBackground();
    cbmwDrawTitle();
    cbmwDrawTextLine(
        CLIPBOARD_HEIGHT-TTF_FontHeight(systemFont.body.ttf)-5, 
        CLIPBOARD_WIDTH/7,
        &systemFont.body, 
        &systemColor.body, 
        " ngxxfus (Nguyễn Thanh Phú)  0845939722 󰇮 msnp@outlook.com.vn"
    );
    cbmwUpdateOnScreen();
    windowContext_t * diag;
    createWindowContext(&diag, 200, 100, "Pop-up");
    cbmwShowPopup(diag, "Message", "hehehe");
    destroyWindowContext(&diag);

    /* Render & control screen */
    struct timespec ts_idle = {.tv_sec = 0, .tv_nsec = 500};
    while (!IS_SYSTEM_STOPPED) {




        nanosleep(&ts_idle, NULL);
    }

    __exit("main(void)");
    return 0;
}