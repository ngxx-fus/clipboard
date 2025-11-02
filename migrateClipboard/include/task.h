#include "globalDef.h"

void* clipboardCaptureService(void* pv){
    __WAIT_FOR_(SYSTEM_INIT_L1);
    __entry("clipboardCaptureService(%p)", pv);
    
    if (ensure_dirs() != 0) return NULL;
    
    int screen_num;
    xcb_connection_t *c = xcb_connect(NULL, &screen_num);
    if (xcb_connection_has_error(c)) {
        __err("[clipboardCaptureService] Cannot connect to X server");
        __exit("clipboardCaptureService() : NULL");
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

    // __WAIT_FOR_INIT__;
    
    __log("clipboard watcher started (poll %d ms), history %s, images %s\n", POLL_MS, HISTORY_FILE, IMG_DIR);

    while (!IS_SYSTEM_STOPPED) {
        int saved = handle_poll_clipboard(c, win, &nextid);
        (void)saved;

        if(__hasFlagBitSet(uiStatusFlag, CB_MOUSE_CLICKED)){

            __entryCriticalSection(&cbFlagMutexLock);
            __clearFlagBit(uiStatusFlag, CB_MOUSE_CLICKED);
            __exitCriticalSection(&cbFlagMutexLock);

            __log("[clipboardCaptureService] Looking for cbBlock...");
            
            for(int i = 0; i < cbClickedClipboardNum; ++i){
                // if( cbba[i].row  )
                __log(
                    "[clipboardCaptureService] Block %d : [r0:%d r1:%d c0:%d c1:%d] <-- [Y:%d, X:%d]", 
                    i + cbCurrentHistoryIndex,
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
                    __log("[clipboardCaptureService] Restore: %s", historyPathFileList[cbCurrentHistoryIndex + i]);
                    restoreClipboardContent(c, win, historyPathFileList[cbCurrentHistoryIndex + i]);
                    break;
                }
            }

        }

        /* sleep POLL_MS ms */
        usleep(POLL_MS * 1000);
    }

    xcb_disconnect(c);

    __exit("clipboardCaptureService() : NULL");
    return NULL;
}

void* clipboardManageService(void* pv){
    __WAIT_FOR_(SYSTEM_INIT_L2);
    __entry("clipboardManageService(%p)", pv);

    ////
    
    size_t i = 0;
    size_t historyItemNum = readClipboardHistory(historyPathFileList, HISTORY_SIZE);
    renderHistory(cbMainWindow, cbba, i);

    cbmwSetRenderTargetOnScreen();
    cbmwClearBackground();
    cbmwDrawTitle();
    cbmwUpdateOnScreen();


    
    __WAIT_FOR_INFINITY__;
    //// 

    /*
    int index = 0;
    int itemNum = 1;
    itemNum = readClipboardHistory(historyPathFileList, HISTORY_SIZE);
    renderHistory(cbMainWindow, cbba, index);
    while(!IS_SYSTEM_STOPPED) {
        if(__hasFlagBitSet(uiStatusFlag, CB_SCROLL_DOWN)){
            __entryCriticalSection(&cbFlagMutexLock);
            __clearFlagBit(uiStatusFlag, CB_SCROLL_DOWN);
            __exitCriticalSection(&cbFlagMutexLock);

            index = (index + 1) % itemNum;
            renderHistory(cbMainWindow, cbba, index);
        }
        if(__hasFlagBitSet(uiStatusFlag, CB_SCROLL_UP)){
            __entryCriticalSection(&cbFlagMutexLock);
            __clearFlagBit(uiStatusFlag, CB_SCROLL_UP);
            __exitCriticalSection(&cbFlagMutexLock);

            index = (index - 1 + itemNum) % itemNum;
            renderHistory(cbMainWindow, cbba, index);
        }
        if(__hasFlagBitSet(uiStatusFlag, CB_RELOAD)){
            __entryCriticalSection(&cbFlagMutexLock);
            __clearFlagBit(uiStatusFlag, CB_RELOAD);
            __exitCriticalSection(&cbFlagMutexLock);

            itemNum = readClipboardHistory(historyPathFileList, HISTORY_SIZE);
            renderHistory(cbMainWindow, cbba, index);
        }
        __sleep_us(100);
    }
    */

    __exit("clipboardManageService() : NULL");
    return NULL;
} 

void* keyboardCaptureService(void* pv){
    __WAIT_FOR_(SYSTEM_INIT_L1);
    __entry("keyboardCaptureService()");
    SDL_Event e;
    while(!IS_SYSTEM_STOPPED){
        while (SDL_PollEvent(&e)) {
            switch (e.type)
            {
                case SDL_QUIT:
                    __log("Event <SDL_QUIT> occured! --> Stop the system!");
                    __SET_SYSTEM_STOP__;
                    break;
                    
                case SDL_MOUSEBUTTONDOWN:
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        int mouseX = e.button.x;
                        int mouseY = e.button.y;
                        __log("Mouse down at (%d, %d)\n", mouseX, mouseY);
                        cbClickedMouseInfo.mouseX = mouseX;
                        cbClickedMouseInfo.mouseY = mouseY;
                        __entryCriticalSection(&cbFlagMutexLock);
                        __setFlagBit(uiStatusFlag, CB_MOUSE_CLICKED);
                        __exitCriticalSection(&cbFlagMutexLock);
                    }
                    break;

                case SDL_MOUSEWHEEL:
                    if (e.wheel.y > 0) {
                        __log("Mouse wheel UP");
                        __entryCriticalSection(&cbFlagMutexLock);
                        __setFlagBit(uiStatusFlag, CB_SCROLL_UP);
                        __exitCriticalSection(&cbFlagMutexLock);
                    } else if (e.wheel.y < 0) {
                        __log("Mouse wheel DOWN");
                        __entryCriticalSection(&cbFlagMutexLock);
                        __setFlagBit(uiStatusFlag, CB_SCROLL_DOWN);
                        __exitCriticalSection(&cbFlagMutexLock);
                    }
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_q) {
                        __log("Event: <SDLK_q> is pressed!");
                        __SET_SYSTEM_STOP__;
                    }
                    else if(e.key.keysym.sym == SDLK_c){
                        __log("Event <SDL_KEYDOWN | SDLK_c> occured!");
                    }
                    else if(e.key.keysym.sym == SDLK_r){
                        __log("Event <SDL_KEYDOWN | SDLK_r> occured!");
                        __setFlagBit(uiStatusFlag, CB_RELOAD);
                    }
                    else if(e.key.keysym.sym == SDLK_w){
                        __log("Event <SDL_KEYDOWN | SDLK_w> occured!");
                        __entryCriticalSection(&cbFlagMutexLock);
                        __setFlagBit(uiStatusFlag, CB_SCROLL_UP);
                        __exitCriticalSection(&cbFlagMutexLock);
                    }
                    else if(e.key.keysym.sym == SDLK_s){
                        __log("Event <SDL_KEYDOWN | SDLK_s> occured!");
                        __entryCriticalSection(&cbFlagMutexLock);
                        __setFlagBit(uiStatusFlag, CB_SCROLL_DOWN);
                        __exitCriticalSection(&cbFlagMutexLock);
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


