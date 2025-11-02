#include "wdct.h"

def __wdctCreateWindow(windowContext_t * wdct){
    if(__is_null(wdct)){
        __err("[__wdctCreateWindow] wdct = %p", wdct);
        return ERR_INVALID_ARG;
    }

    wdct->window = SDL_CreateWindow(
        wdct->title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        wdct->w,
        wdct->h,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (__is_null(wdct->window)) {
        __err("[__wdctCreateWindow] SDL_CreateWindow failed: %s", SDL_GetError());
        return ERR;
    }
    return OKE;
}

def __wdctDeleteWindow(windowContext_t * wdct){
    if(__is_null(wdct)){
        __err("[__wdctDeleteWindow] wdct = %p", wdct);
        return ERR_INVALID_ARG;
    }
    if (__is_null(wdct->window)) {
        __err("[__wdctDeleteWindow] wdct->window = %p", wdct->window);
        return ERR_INVALID_ARG;
    } else {
        SDL_DestroyWindow(wdct->window);
    }
    return OKE;
}

def __wdctCreateRenderer(windowContext_t * wdct){
    __entry1("__wdctCreateRenderer(%p)", wdct);

    if(__is_null(wdct)){
        __err("[__wdctCreateRenderer] wdct = %p", wdct);
        __exit1("__wdctCreateRenderer(): ERR_INVALID_ARG");
        return ERR_INVALID_ARG;
    }
    if(__is_null(wdct->window)) {
        __err("[__wdctCreateRenderer] wdct->window = %p ; The <window> must be created before!", wdct->window);
        __exit1("__wdctCreateRenderer(): ERR_INVALID_ARG");
        return ERR_INVALID_ARG;
    }
    __log1("[__wdctCreateRenderer] SDL_CreateRenderer");
    wdct->renderer = SDL_CreateRenderer(
        wdct->window, 
        -1, 
        SDL_RENDERER_ACCELERATED
    );
    if(__is_null(wdct->renderer)){
        __err("[__wdctCreateRenderer] SDL_CreateRenderer failed: %s", SDL_GetError());
        __exit1("__wdctCreateRenderer(): ERR");
        return ERR;
    }
    __exit1("__wdctCreateRenderer(): OKE");
    return OKE;
}

def __wdctDeleteRenderer(windowContext_t * wdct){
    if(__is_null(wdct)){
        __err("[__wdctDeleteRenderer] wdct = %p", wdct);
        return ERR_INVALID_ARG;
    }
    if (__is_null(wdct->renderer)) {
        __err("[__wdctDeleteRenderer] wdct->renderer = %p", wdct->renderer);
        return ERR_INVALID_ARG;
    } else {
        SDL_DestroyRenderer(wdct->renderer);
    }
    return OKE;
}

def __wdctCreateTexture(windowContext_t * wdct){
    if(__is_null(wdct)){
        __err("[__wdctCreateTexture] wdct = %p", wdct);
        return ERR_INVALID_ARG;
    }
    if(__is_null(wdct->renderer)) 
        return ERR_INVALID_ARG;

    wdct->texture = SDL_CreateTexture(
        wdct->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        wdct->w,
        wdct->h
    );
    if(__is_null(wdct->texture)){
        __err("[__wdctCreateTexture] SDL_CreateTexture failed: %s", SDL_GetError());
    }
    return OKE;
}

def __wdctDeleteTexture(windowContext_t * wdct){
    if(__is_null(wdct)){
        __err("[__wdctDeleteTexture] wdct = %p", wdct);
        return ERR_INVALID_ARG;
    }
    if (__is_null(wdct->texture)) {
        __err("[__wdctDeleteTexture] wdct->texture = %p", wdct->texture);
        return ERR_INVALID_ARG;
    } else {
        SDL_DestroyTexture(wdct->texture);
    }
    return OKE;
}

def createWindowContext(windowContext_t **wdct, dim_t w, dim_t h, const char *title){
    __entry("createWindowContext(%p, %d, %d, %s)", wdct, w, h, title);

    __log1("[createWindowContext] Check __is_null(wdct)");
    if (__is_null(wdct)) {
        __err("[createWindowContext] wdct = %p", wdct);
        return ERR_INVALID_ARG;
    }

    __log1("[createWindowContext] Allocate wdct *...");
    *wdct = (windowContext_t*) malloc(sizeof(windowContext_t));
    if (*wdct == NULL) {
        __err("[createWindowContext] malloc failed!");
        return ERR_INVALID_ARG;
    }

    __log1("[createWindowContext] Preset *wdct");
    (*wdct)->w = w;
    (*wdct)->h = h;
    strncpy((*wdct)->title, title, MAX_TITLE_SIZE - 1);
    (*wdct)->title[MAX_TITLE_SIZE - 1] = '\0';
    (*wdct)->window  = NULL;
    (*wdct)->renderer = NULL;
    (*wdct)->texture = NULL;

    __log1("[createWindowContext] Create Windows (>>> __wdctCreateWindow)");
    if (__wdctCreateWindow(*wdct) != OKE) 
        goto __fail_window__;

    __log1("[createWindowContext] Create Renderer (>>> __wdctCreateRenderer)");
    if (__wdctCreateRenderer(*wdct) != OKE) 
        goto __fail_renderer__;

    __log1("[createWindowContext] Create Texture (>>> __wdctCreateTexture)");
    if (__wdctCreateTexture(*wdct) != OKE) 
        goto __fail_texture__;

    __exit("createWindowContext()");
    return OKE;

__fail_texture__:
    __wdctDeleteRenderer(*wdct);

__fail_renderer__:
    __wdctDeleteWindow(*wdct);

__fail_window__:
    free(*wdct);
    *wdct = NULL;
    __exit("createWindowContext() failed");
    return ERR_INVALID_ARG;
}

def destroyWindowContext(windowContext_t **wdct)
{
    if (__is_null(wdct) || __is_null(*wdct)) {
        __err("[destroyWindowContext] Cannot destroy! Because: wdct = %p, *wdct = %p", wdct, *wdct);
        return ERR_INVALID_ARG;
    }

    __entry("destroyWindowContext(%p)", *wdct);

    __wdctDeleteTexture(*wdct);
    __wdctDeleteRenderer(*wdct);
    __wdctDeleteWindow(*wdct);

    free(*wdct);
    *wdct = NULL;
    
    __exit("destroyWindowContext()");
    return OKE;
}
