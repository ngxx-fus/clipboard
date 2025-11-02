#ifndef __TASK_H__
#define __TASK_H__

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

        /* sleep POLL_MS ms */
        usleep(POLL_MS * 1000);
    }

    xcb_disconnect(c);

    __exit("clipboardCaptureService() : NULL");
    return NULL;
}

void* cbTestThread(void* pv){
    __WAIT_FOR_INIT__;

    CBPU_SET_TITLE("Message from ngxxfus");
    CBPU_SET_MESSAGE("This is a demo version. Use it at your own risk — I am not responsible for any issues that may occur! (Vietnamese: Đây là phiên bản thử nghiệm! Hãy cẩn thận khi sử dụng — tôi không chịu trách nhiệm nếu xảy ra sự cố.)");
    cbpuPushEvent(CBPU_EVENT_SHOW_YESNO, NULL, NULL);
    __log("[cbTestThread] sizeof(₍₍⚞(˶˃ ꒳ ˂˶)⚟⁾⁾) = %d", sizeof("₍₍⚞(˶˃ ꒳ ˂˶)⚟⁾⁾"));

    flag_t clickedNo    = __flagMask(UI_FLAG_POPUP_CLICKED_NO);
    flag_t clickedYes   = __flagMask(UI_FLAG_POPUP_CLICKED_YES);
    flag_t clickedClose = __flagMask(UI_FLAG_POPUP_CLICKED_CLOSE);
    flag_t currentState = 0;
    do{
        currentState = __uiFlagGetFull();
        if(currentState & clickedYes){
            __uiFlagClr(UI_FLAG_POPUP_CLICKED_YES);
            appAgreement = 0xAC;
            break;
        }
        if(currentState & clickedNo){
            __uiFlagClr(UI_FLAG_POPUP_CLICKED_NO);
            CBPU_SET_MESSAGE("Closing...");
            cbpuPushEvent(CBPU_EVENT_SHOW_MSG, NULL, NULL);
            sleep(1);            
            __SET_SYSTEM_STOP__;
        }
        __sleep_ns(1000);
    }while(1);

    SDL_SetRenderTarget(cbMainWindow->renderer, cbmwTempTexture);
    cbmwClearBackground();

    cbmwItemAppendContent("This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.) This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.)");
    cbmwItemAppendContent("This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.) This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.)");
    cbmwItemAppendContent("This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.) This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.)");
    cbmwItemAppendContent("This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.) This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.)");
    cbmwItemAppendContent("This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.) This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.)");
    cbmwItemAppendContent("This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.) This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.)");
    cbmwItemAppendContent("This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.) This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.)");
    cbmwItemAppendContent("This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.) This is a default clipboard content. Replace it with your actual content. (Vietnamese: Đây là nội dung mẫu. Hãy thay thế nội dung mẫu này bằng nội dung thực.)");

    REP(i, 0, cbmwItemNum){
        cbmwDrawItemId(i);
    }

    cbmwItemNum = 8;
    SDL_SetRenderTarget(cbMainWindow->renderer, cbMainWindow->texture);
    SDL_Rect scrop = {
        .y = CBMW_MARGIN_TOP,
        .x = CBMW_MARGIN_LEFT,
        .h = CLIPBOARD_HEIGHT - CBMW_MARGIN_TOP - CBMW_MARGIN_BOTTOM,
        .w = CLIPBOARD_WIDTH  - CBMW_MARGIN_LEFT - CBMW_MARGIN_RIGHT
    }, dcrop = scrop;
    SDL_RenderCopy(cbMainWindow->renderer, cbmwTempTexture, &scrop, &dcrop);
    cbmwLoadOffScreen();
    cbmwUpdateOnScreen();

    struct timespec ts = {.tv_sec = 0, .tv_nsec = __USEC(500)};
    while (1){
        int clickedItemID = cbmwItemClickedFind();
        if(clickedItemID >= 0) __log("Clicked item-%d", clickedItemID);
        nanosleep(&ts, NULL);
    }
    

    __WAIT_FOR_INFINITY__;
}

static inline void __processMouseEvent_Popup(SDL_Event e){
    if(isInsideButtonClose(e.button.y, e.button.x)){
        __log("[__processMouseEvent] Clicked close popup button!");
        __uiFlagSet(UI_FLAG_POPUP_CLICKED_CLOSE);
        cbpuPushEvent(CBPU_EVENT_CLOSE, 0, 0);
    }
    if(isInsideButtonYes(e.button.y, e.button.x)){
        __log("[__processMouseEvent] Clicked close popup button!");
        __uiFlagSet(UI_FLAG_POPUP_CLICKED_YES);
        cbpuPushEvent(CBPU_EVENT_CLOSE, 0, 0);
    }
    if(isInsideButtonNo(e.button.y, e.button.x)){
        __log("[__processMouseEvent] Clicked close popup button!");
        __uiFlagSet(UI_FLAG_POPUP_CLICKED_NO);
        cbpuPushEvent(CBPU_EVENT_CLOSE, 0, 0);
    }
}

static inline void __processMouseEvent_MainWindow(SDL_Event e){
    if(appAgreement != 0xAC) return;

    switch (e.type){
        case SDL_MOUSEBUTTONUP:
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(e.button.button != SDL_BUTTON_LEFT) break;
            __log("[__processMouseEvent_MainWindow] Clicked at (r=%d, c=%d)", e.button.y, e.button.x);
            
            break;

        case SDL_MOUSEWHEEL:
            if (e.wheel.y > 0){
                __log("[__processMouseEvent_MainWindow] Scroll up");
            }
            if (e.wheel.y < 0){
                __log("[__processMouseEvent_MainWindow] Scroll down");
            }
            break;
    }
}

void __processKeyboardEvent(SDL_Event e){
    switch (e.type){
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
                __uiFlagSet(CB_RELOAD);
            }
            else if(e.key.keysym.sym == SDLK_w){
                __log("Event <SDL_KEYDOWN | SDLK_w> occured!");
                __uiFlagSet(CB_SCROLL_UP);
                
            }
            else if(e.key.keysym.sym == SDLK_s){
                __log("Event <SDL_KEYDOWN | SDLK_s> occured!");
                __uiFlagSet(CB_SCROLL_DOWN);
                
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

void __processMouseEvent(SDL_Event e){
    switch (e.type){
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            cbmwMouseX = e.button.x;
            cbmwMouseY = e.button.y;
            if(__isnot_null(wdcts.sub) && e.button.windowID == cbPopup->id){
                __processMouseEvent_Popup(e);
            }
            if(__isnot_null(wdcts.main) && e.button.windowID == cbMainWindow->id){
                __processMouseEvent_MainWindow(e);
            }
            break;

        case SDL_MOUSEWHEEL:
            if(__isnot_null(wdcts.sub) && e.button.windowID == cbPopup->id){
                __processMouseEvent_Popup(e);
            }
            if(__isnot_null(wdcts.main) && e.button.windowID == cbMainWindow->id){
                __processMouseEvent_MainWindow(e);
            }
            break;
    }
}

void __processWindowEvent(SDL_Event e){
    __log("[__processWindowEvent] Currently, __processWindowEvent is empty!");
}

void __processPopupEvent(SDL_Event e){
    __log("[__processPopupEvent] Currently, __processPopupEvent is empty!");
    if(e.type == CB_POPUP_EVENT){
        switch (e.user.code)
        {
        case CBPU_EVENT_SHOW_MSG:
            __log("[__processPopupEvent] Show popup...");
            cbswShowPopupMessage();
            break;
        
        case CBPU_EVENT_SHOW_YESNO:
            __log("[__processPopupEvent] Close popup...");
            cbpuShowPopupYesNo();
            break;
            
        case CBPU_EVENT_CLOSE:
            __log("[__processPopupEvent] Close popup...");
            // cbpuDestroyPopupMessage();
            cbpuHidePopup();
            break;

        default:
            break;
        }
    }
}



#endif