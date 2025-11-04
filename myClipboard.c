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

    /* Event routing */
    SDL_Event e;
    struct timespec ts_idle = {.tv_sec = 0, .tv_nsec = 100};
    while (!IS_SYSTEM_STOPPED) {
        while (SDL_PollEvent(&e)) {
            switch (e.type){
                case SDL_QUIT:
                    __log("[main] Event <SDL_QUIT> occured! --> Stop the system!");
                    __SET_SYSTEM_STOP__;
                    break;
                
                case SDL_KEYUP:
                case SDL_KEYDOWN:
                    __processKeyboardEvent(e);
                    break;

                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEWHEEL:
                    __processMouseEvent(e);
                    break;

                case SDL_WINDOWEVENT:
                    __processWindowEvent(e);
                    break;

                default:
                    if(e.type == CB_POPUP_EVENT){
                        __processPopupEvent(e);
                    }
                    break;

            }
            nanosleep(&ts_idle, NULL);
        }
        nanosleep(&ts_idle, NULL);
    }

    __exit("main(void)");
    return 0;
}
