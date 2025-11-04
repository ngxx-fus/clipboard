#include "include/init.h"



int main(void) {
    /// Set-up section ////////////////////////////////////////////////////////////////// 

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


    /// Main thread /////////////////////////////////////////////////////////////////////

    /* MainWindow control & routing */
    SDL_Event e;
    struct timespec ts = {.tv_sec = 0, .tv_nsec = __USEC(10)};
    while (!IS_SYSTEM_STOPPED) {
        flag_t currentUIFlag = __uiFlagGetFull();
        
        while(currentUIFlag){

            if(currentUIFlag & __flagMask(CPMW_RELOAD_ITEM)){
                SDL_RenderPresent(cbMainWindow->renderer);
                continue;
            }
            if(currentUIFlag) {
                __log("[main] Warn: Un-processed flag bit [0x%x]!", currentUIFlag);
                currentUIFlag = 0;
                __uiFlagSetFull(currentUIFlag);
            }
        }
        nanosleep(&ts, NULL);
    }

    __exit("main(void)");
    return 0;
}
