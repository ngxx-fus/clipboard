#include "global.h"
#include "helper/helper.h"
#include "log/log.h"
#include "clipboard/clipboard.h"
#include "UI/UI.h"

#include <pthread.h>

// void* clipboardControlService(void* pv){
//     __entry("clipboardControlService(%p)", pv);

//     while(hasFlag(systemStatusFlag, SYS_RUNNING)){
//         entryCriticalSection(&clickedClipboardIDMutexLock);
//         if(currentHistoryIndex > 0){

//         }
//         exitCriticalSection(&clickedClipboardIDMutexLock);

//         SDL_Delay(100);
//     }

//     __exit("clipboardControlService");
// }

void* clipboardCaptureService(void* pv){
    __entry("clipboardCaptureService(%p)", pv);

    if (ensure_dirs() != 0) return NULL;

    int screen_num;
    xcb_connection_t *c = xcb_connect(NULL, &screen_num);
    if (xcb_connection_has_error(c)) {
        fprintf(stderr, "Cannot connect to X server\n");
        return NULL;
    }

    const xcb_setup_t *setup = xcb_get_setup(c);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    for (int i = 0; i < screen_num; ++i) xcb_screen_next(&iter);
    xcb_screen_t *screen = iter.data;

    /* input-only window to receive SelectionNotify */
    xcb_window_t win = xcb_generate_id(c);
    xcb_create_window(c, XCB_COPY_FROM_PARENT, win, screen->root, 0, 0, 1, 1, 0,
                      XCB_WINDOW_CLASS_INPUT_ONLY, screen->root_visual, 0, NULL);

    /* atoms */
    atom_clipboard = get_atom(c, "CLIPBOARD");
    atom_utf8 = get_atom(c, "UTF8_STRING");
    if (atom_utf8 == XCB_ATOM_NONE) atom_utf8 = XCB_ATOM_STRING;
    atom_targets = get_atom(c, "TARGETS");
    atom_png = get_atom(c, "image/png");
    atom_jpeg = get_atom(c, "image/jpeg");
    atom_prop = get_atom(c, PROP_NAME);
    if (atom_prop == XCB_ATOM_NONE) atom_prop = get_atom(c, PROP_NAME);

    unsigned int nextid = read_nextid();

    __log("clipboard watcher started (poll %d ms), history %s, images %s\n", POLL_MS, HISTORY_FILE, IMG_DIR);

    while (hasFlag(systemStatusFlag, SYS_RUNNING)) {
        int saved = handle_poll_clipboard(c, win, &nextid);
        (void)saved;

        if(hasFlag(uiStatusFlag, CBMW_MOUSE_CLICKED)){

            entryCriticalSection(&uiFlagMutexLock);
            clrFlag(uiStatusFlag, CBMW_MOUSE_CLICKED);
            exitCriticalSection(&uiFlagMutexLock);

            __log("[clipboardCaptureService] Looking for cbBlock...");
            
            for(int i = 0; i < clickedClipboardNum; ++i){
                // if( cbba[i].row  )
                __log(
                    "[clipboardCaptureService] Block %d : [r0:%d r1:%d c0:%d c1:%d] <-- [Y:%d, X:%d]", 
                    i + currentHistoryIndex,
                    cbba[i].row, 
                    cbba[i].row + cbba[i].h,
                    cbba[i].col,
                    cbba[i].col + cbba[i].w,
                    cbClickedMouseInfo.mouseY,
                    cbClickedMouseInfo.mouseX
                );
                if(
                    cbba[i].row <= cbClickedMouseInfo.mouseY &&
                    cbClickedMouseInfo.mouseY < cbba[i].row + cbba[i].h &&
                    cbba[i].col <= cbClickedMouseInfo.mouseX &&
                    cbClickedMouseInfo.mouseY < cbba[i].col + cbba[i].w
                ){
                    __log("[clipboardCaptureService] Restore: %s", historyList[currentHistoryIndex + i]);
                    restoreClipboardContent(c, win, historyList[currentHistoryIndex + i]);
                    break;
                }
            }

        }

        /* sleep POLL_MS ms */
        usleep(POLL_MS * 1000);
    }

    xcb_disconnect(c);

    __exit("clipboardCaptureService()");
    return NULL;
}

void* keyboardCaptureService(void* pv){
    __entry("keyboardCaptureService()");
    SDL_Event e;
    while(hasFlag(systemStatusFlag, SYS_RUNNING)){
        while (SDL_PollEvent(&e)) {
            switch (e.type)
            {
                case SDL_QUIT:
                    __log("Event <SDL_QUIT> occured!");
                    clrFlag(systemStatusFlag, SYS_RUNNING);
                    break;
                    
                case SDL_MOUSEBUTTONDOWN:
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        int mouseX = e.button.x;
                        int mouseY = e.button.y;
                        __log("Mouse down at (%d, %d)\n", mouseX, mouseY);
                        cbClickedMouseInfo.mouseX = mouseX;
                        cbClickedMouseInfo.mouseY = mouseY;
                        entryCriticalSection(&uiFlagMutexLock);
                        setFlag(uiStatusFlag, CBMW_MOUSE_CLICKED);
                        exitCriticalSection(&uiFlagMutexLock);
                    }
                    break;

                case SDL_MOUSEWHEEL:
                    if (e.wheel.y > 0) {
                        __log("Mouse wheel UP");
                        entryCriticalSection(&uiFlagMutexLock);
                        setFlag(uiStatusFlag, CPMW_SCROLL_ITEM_UP);
                        exitCriticalSection(&uiFlagMutexLock);
                    } else if (e.wheel.y < 0) {
                        __log("Mouse wheel DOWN");
                        entryCriticalSection(&uiFlagMutexLock);
                        setFlag(uiStatusFlag, CPMW_SCROLL_ITEM_DOWN);
                        exitCriticalSection(&uiFlagMutexLock);
                    }
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_q) {
                        __log("Event: <SDLK_q> is pressed!");
                        clrFlag(systemStatusFlag, SYS_RUNNING);
                    }
                    else if(e.key.keysym.sym == SDLK_c){
                        __log("Event <SDL_KEYDOWN | SDLK_c> occured!");
                    }
                    else if(e.key.keysym.sym == SDLK_r){
                        __log("Event <SDL_KEYDOWN | SDLK_r> occured!");
                        setFlag(uiStatusFlag, CPMW_RELOAD_ITEM);
                    }
                    else if(e.key.keysym.sym == SDLK_w){
                        __log("Event <SDL_KEYDOWN | SDLK_w> occured!");
                        entryCriticalSection(&uiFlagMutexLock);
                        setFlag(uiStatusFlag, CPMW_SCROLL_ITEM_UP);
                        exitCriticalSection(&uiFlagMutexLock);
                    }
                    else if(e.key.keysym.sym == SDLK_s){
                        __log("Event <SDL_KEYDOWN | SDLK_s> occured!");
                        entryCriticalSection(&uiFlagMutexLock);
                        setFlag(uiStatusFlag, CPMW_SCROLL_ITEM_DOWN);
                        exitCriticalSection(&uiFlagMutexLock);
                    }
                    // else if(SDLK_0 <= e.key.keysym.sym && e.key.keysym.sym <= SDLK_9){
                    //     uint8_t i = e.key.keysym.sym - SDLK_0;
                    // }
                    break;
                case SDL_KEYUP:
                    // if(SDLK_0 <= e.key.keysym.sym && e.key.keysym.sym <= SDLK_9){
                    //     uint8_t i = e.key.keysym.sym - SDLK_0;
                    // }else{
                    //     if(e.key.keysym.sym == SDLK_c){
                    //     }
                    // }
                    break;
            }
        }
    }
    __exit("keyboardCaptureService()");
    return NULL;
}
