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

    __log("[myClipboardInit] Setup main window [%dx%d]", CLIPBOARD_HEIGHT, CLIPBOARD_WIDTH);
    createWindowContext(&cbMainWindow, CLIPBOARD_WIDTH, CLIPBOARD_HEIGHT, CLIPBOARD_TITLE);
    
    /*  /// TEST SCREEN 
    __log("[myClipboardInit] Test SDL...");
    const char* testMsg[] = {
        "Initializing | Font demo: Title",
        "Initializing | Font demo: Body",
        "Initializing | Font demo: Heading1",
        "Initializing | Font demo: Heading2",
        "Initializing | Font demo: Heading3",
        "Initializing | Font demo: Error",
        "Initializing | Font demo: Warning",
    };
    dim_t baseLine = 0;
    SDL_SetRenderDrawColor(cbMainWindow->renderer, splitRGBA(systemColor.background));
    SDL_RenderClear(cbMainWindow->renderer);
    SDL_RenderPresent(cbMainWindow->renderer);
    REP(i, 0, 7){
        __log("[myClipboardInit] 0x%x --> 0x%x | 0x%x | 0x%x | 0x%x", systemColor.arr[i].abgr, splitRGBA(systemColor.arr[i]));
        SDL_SetRenderDrawColor(cbMainWindow->renderer, splitRGBA(systemColor.arr[i]));
        SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(systemFont.arr[i].ttf, testMsg[i], systemColor.arr[i].SDL, 500);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(cbMainWindow->renderer, surface);
        SDL_Rect dstRect = {5, baseLine, surface->w, surface->h};
        SDL_RenderCopy(cbMainWindow->renderer, texture, NULL, &dstRect);
        SDL_RenderPresent(cbMainWindow->renderer);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
        baseLine += TTF_FontHeight(systemFont.arr[i].ttf);
    }
    */
    
    __log("[myClipboardInit] [+task] clipboardCaptureDaemon");
    pthread_t clipboardCaptureDaemon;
    pthread_create(&clipboardCaptureDaemon, NULL, clipboardCaptureService, NULL);
    
    // __log("[myClipboardInit] [+task] clipboardManageDaemon");
    // pthread_t clipboardManage;
    // pthread_create(&clipboardManage, NULL, clipboardManageService, NULL);
    
    __log("[myClipboardInit] [+task] keyboardDaemon");
    pthread_t keyboardDaemon;
    pthread_create(&keyboardDaemon, NULL, keyboardCaptureService, NULL);

    __exit("myClipboardInit()");
}

void myClipboardExit(){
    __entry("myClipboardExit()");

    REP(i, 0, 7) cbDeleteFont(&(systemFont.arr[i]));
    
    destroyWindowContext(&cbMainWindow);

    TTF_Quit();
    SDL_Quit();

    exit(0);
    
    __exit("myClipboardExit()");
}

