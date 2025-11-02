#include "task.h"



void myClipboardInit(){
    __entry("myClipboardInit()");
    
    /// Because of endianess and the popular corlor type is RGBA (e.g, with colorhunt.co, ...), 
    /// So 1st i preset .abgr = <RGBA hex color> for systemColor, 
    /// then call systemColorCorrectByteOrder to correct them.
    systemColorCorrectByteOrder(&systemColor);
    /// Noted that the order of color type in systemFont and systemColor are the same.
    syncSystemColor(&systemFont);

    __log("[myClipboardInit] Init SDL...");
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        __err("[wdctInitSystem] SDL_Init failed: %s", SDL_GetError());
        exit(ERR);
    }

    __log("[myClipboardInit] Init TTF...");
    if (TTF_Init() == -1) {
        __err("[wdctInitSystem] TTF_Init failed: %s", TTF_GetError());
        exit(ERR);
    }

    __log("[myClipboardInit] Load system fonts...");
    REP(i, 0, 7) cbSelfLoad(&(systemFont.arr[i]));

    __log("[myClipboardInit] Register CB_POPUP_EVENT...");
    CB_POPUP_EVENT = SDL_RegisterEvents(1);

    __log("[myClipboardInit] Setup main window [%dx%d]", CLIPBOARD_HEIGHT, CLIPBOARD_WIDTH);
    createWindowContext(&cbMainWindow, CLIPBOARD_WIDTH, CLIPBOARD_HEIGHT, CLIPBOARD_TITLE);
    __cbpuCreatePopup();
    cbpuHidePopup();
    cbmwTempTexture = SDL_CreateTexture(
            cbMainWindow->renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET, /// SDL_TEXTUREACCESS_STREAMING,
            cbMainWindow->w,
            cbMainWindow->h
        );

    __log("[myClipboardInit] [+task] clipboardCaptureDaemon");
    pthread_t clipboardCaptureDaemon;
    pthread_create(&clipboardCaptureDaemon, NULL, clipboardCaptureService, NULL);

    __log("[myClipboardInit] [+task] cbTestThread");
    pthread_t cbTestDaemon;
    pthread_create(&cbTestDaemon, NULL, cbTestThread, NULL);

    cbmwSetRenderTargetOffScreen();
    cbmwClearBackground();
    cbmwDrawTitle();
    cbmwDrawInfo();
    cbmwLoadOffScreen();

    __exit("myClipboardInit()");
}

void myClipboardExit(){
    __entry("myClipboardExit()");

    REP(i, 0, 7) cbDeleteFont(&(systemFont.arr[i]));
    
    destroyWindowContext(&cbMainWindow);
    destroyWindowContext(&cbPopup);

    TTF_Quit();
    SDL_Quit();

    exit(0);
    
    __exit("myClipboardExit()");
}

