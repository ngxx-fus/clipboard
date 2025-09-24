#include "wdct.h"

status_t wdctCreateWindow(windowContext_t * wdct){
    if(__is_null(wdct)){
        __err("[wdctCreateWindow] wdct = %p", wdct);
        return ERROR_INVALID_PARAMS;
    }
    // wdct->window = SDL_CreateWindow(
    //     wdct->title,
    //     SDL_WINDOWPOS_CENTERED, 
    //     SDL_WINDOWPOS_CENTERED,
    //     wdct->w,
    //     wdct->h,
    //     SDL_WINDOW_SHOWN
    // );
    wdct->window = SDL_CreateWindow(
        wdct->title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        wdct->w,
        wdct->h,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (__is_null(wdct->window)) {
        __err("[wdctCreateWindow] SDL_CreateWindow failed: %s", SDL_GetError());
        return ERROR_UNKNOWN;
    }
    return STATUS_OK;
}

status_t wdctDeleteWindow(windowContext_t * wdct){
    if(__is_null(wdct)){
        __err("[wdctDeleteWindow] wdct = %p", wdct);
        return ERROR_INVALID_PARAMS;
    }
    if (__is_null(wdct->window)) {
        __err("[wdctDeleteWindow] wdct->window = %p", wdct->window);
        return ERROR_INVALID_PARAMS;
    }else{
        SDL_DestroyWindow(wdct->window);
    }
    return STATUS_OK;
}

status_t wdctCreateRenderer(windowContext_t * wdct){
    if(__is_null(wdct)){
        __err("[wdctCreateRenderer] wdct = %p", wdct);
        return ERROR_INVALID_PARAMS;
    }
    if(__is_null(wdct->window)) {
        __err("[wdctCreateRenderer] wdct->window = %p ; The <window> must be created before!", wdct->window);
        return ERROR_INVALID_PARAMS;
    }
    wdct->renderer = SDL_CreateRenderer(
        wdct->window, 
        -1, 
        SDL_RENDERER_ACCELERATED
    );
    if(__is_null(wdct->renderer)){
        __err("[wdctCreateRenderer] SDL_CreateRenderer failed: %s", SDL_GetError());
        return ERROR_UNKNOWN;
    }
    return STATUS_OK;
}

status_t wdctDeleteRenderer(windowContext_t * wdct){
    if(__is_null(wdct)){
        __err("[wdctDeleteRenderer] wdct = %p", wdct);
        return ERROR_INVALID_PARAMS;
    }
    if (__is_null(wdct->renderer)) {
        __err("[wdctDeleteRenderer] wdct->renderer = %p", wdct->renderer);
        return ERROR_INVALID_PARAMS;
    }else{
        SDL_DestroyRenderer(wdct->renderer);
    }
    return STATUS_OK;
}

status_t wdctCreateTexture(windowContext_t * wdct){
    if(__is_null(wdct)){
        __err("[wdctCreateTexture] wdct = %p", wdct);
        return ERROR_INVALID_PARAMS;
    }
    if(__is_null(wdct->renderer)) 
        return ERROR_INVALID_PARAMS;
    wdct->texture = SDL_CreateTexture(
        wdct->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        wdct->w,
        wdct->h
    );
    if(__is_null(wdct->texture)){
        __err("[wdctCreateTexture] SDL_CreateTexture failed: %s", SDL_GetError());
    }
    return STATUS_OK;
}

status_t wdctDeleteTexture(windowContext_t * wdct){
    if(__is_null(wdct)){
        __err("[wdctDeleteTexture] wdct = %p", wdct);
        return ERROR_INVALID_PARAMS;
    }
    if (__is_null(wdct->texture)) {
        __err("[wdctDeleteTexture] wdct->texture = %p", wdct->texture);
        return ERROR_INVALID_PARAMS;
    }else{
        SDL_DestroyTexture(wdct->texture);
    }
    return STATUS_OK;
}

status_t wdctOpenFont(windowContext_t * wdct, const char *fontPath, uint8_t fontSize){
    if(__is_null(wdct)){
        __err("[wdctOpenFont] wdct = %p", wdct);
        return ERROR_INVALID_PARAMS;
    }
    if (TTF_Init() == -1) {
        __err("[wdctOpenFont] TTF_Init failed: %s\n", TTF_GetError());
        return -1;
    }
    wdct->font = TTF_OpenFont(fontPath, fontSize);
    if (!wdct->font) {
        __err("[wdctOpenFont] TTF_OpenFont failed: %s", TTF_GetError());
        return ERROR_UNKNOWN;
    }
    return STATUS_OK;
}

status_t wdctCloseFont(windowContext_t * wdct){
    if(__is_null(wdct)){
        __err("[wdctCloseFont] wdct = %p", wdct);
        return ERROR_INVALID_PARAMS;
    }
    if(__is_null(wdct->font)){
        __err("[wdctCloseFont] wdct->font = %p", wdct->font);
        return ERROR_INVALID_PARAMS;
    }
    TTF_CloseFont(wdct->font);
    return STATUS_OK;
}

status_t createWindowContext(
    windowContext_t **wdct, xy_t w, xy_t h, const char *title, 
    const char *fontPath, uint8_t fontSize
){
    __entry("createWindowContext(%p, %d, %d, %s, %s, %d)", wdct, w, h, title, fontPath, fontSize);

    if (__is_null(wdct)) {
        __err("[createWindowContext] wdct = %p", wdct);
        return ERROR_INVALID_PARAMS;
    }

    *wdct = (windowContext_t*) malloc(sizeof(windowContext_t));
    if (*wdct == NULL) {
        __err("[createWindowContext] malloc failed!");
        return ERROR_INVALID_PARAMS;
    }

    (*wdct)->w = w;
    (*wdct)->h = h;
    strncpy((*wdct)->title, title, MAX_TITLE_SIZE - 1);
    (*wdct)->title[MAX_TITLE_SIZE - 1] = '\0';
    (*wdct)->window  = NULL;
    (*wdct)->renderer = NULL;
    (*wdct)->texture = NULL;
    (*wdct)->font    = NULL;

    if (wdctCreateWindow(*wdct) != STATUS_OK) 
        goto __fail_window__;

    if (wdctCreateRenderer(*wdct) != STATUS_OK) 
        goto __fail_renderer__;

    if (wdctCreateTexture(*wdct) != STATUS_OK) 
        goto __fail_texture__;

    if (__is_not_null(fontPath) && __is_positive(fontSize)) {
        if (wdctOpenFont(*wdct, fontPath, fontSize) != STATUS_OK) {
            goto __fail_font__;
        }
    } else {
        __log("[createWindowContext] Font not created (NULL path or invalid size)");
    }

    __exit("createWindowContext()");
    return STATUS_OK;

__fail_font__:
    wdctDeleteTexture(*wdct);

__fail_texture__:
    wdctDeleteRenderer(*wdct);

__fail_renderer__:
    wdctDeleteWindow(*wdct);

__fail_window__:
    free(*wdct);
    *wdct = NULL;
    __exit("createWindowContext() failed");
    return ERROR_INVALID_PARAMS;
}

status_t destroyWindowContext(windowContext_t **wdct)
{
    if (__is_null(wdct) || __is_null(*wdct)) {
        __err("[destroyWindowContext] Cannot destroy! Because: wdct = %p, *wdct = %p", wdct, *wdct);
        return ERROR_INVALID_PARAMS;
    }

    __entry("destroyWindowContext(%p)", *wdct);

    wdctCloseFont(*wdct);
    wdctDeleteTexture(*wdct);
    wdctDeleteRenderer(*wdct);
    wdctDeleteWindow(*wdct);

    free(*wdct);
    *wdct = NULL;
    
    __exit("destroyWindowContext()");
    return STATUS_OK;
}