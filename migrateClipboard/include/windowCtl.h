#ifndef __MAIN_WIN_H__
#define __MAIN_WIN_H__

/// HEADERs ///////////////////////////////////////////////////////////////////////////////////////

#include "../helper/general.h"
#include "../windowContext/wdct.h"
#include "../clipboardManager/clipboardManager.h"

/// MAIN WINDOW ///////////////////////////////////////////////////////////////////////////////////

#define WINDOW_CONTEXT_NUM  2

typedef union cbWindowContexts{
    struct {
        windowContext_t* main;
        windowContext_t* sub;
    };
    windowContext_t* arr[WINDOW_CONTEXT_NUM];
} cbWindowContexts;

cbWindowContexts wdcts;

#define cbMainWindow    (wdcts.main)
#define fontBodyH       TTF_FontHeight(systemFont.body.ttf)

SDL_Texture *cbmwTempTexture;

typedef union cbmwItemInfo_t{
    SDL_Rect SDL;
    struct {
        int col, row;
        int w, h;
        int id;
    };
} cbmwItemInfo_t;

enum UI_STATUS_BITORDER{
    CB_RELOAD = 0,
    CB_SCROLL_UP,
    CB_SCROLL_DOWN,
    CB_MOUSE_CLICKED
};

cbmwItemInfo_t      cbmwItemArea[CBMW_ITEM_NUM] = {};
int                 cbmwItemNum = 0;
int                 cbmwCurrentItemIndex = 0;
int                 cbmwMouseX, cbmwMouseY;
char                cbmwItemContent[CBMW_ITEM_NUM][256];


void cbmwSetRenderTargetOnScreen(){
    SDL_SetRenderTarget(cbMainWindow->renderer, NULL);
}

void cbmwSetRenderTargetOffScreen(){
    SDL_SetRenderTarget(cbMainWindow->renderer, cbMainWindow->texture);
}

void cbmwClearBackground(){
    SDL_SetRenderDrawColor(cbMainWindow->renderer, splitRGBA(systemColor.background));
    SDL_RenderClear(cbMainWindow->renderer);
}

void cbmwUpdateOnScreen(){
    SDL_RenderPresent(cbMainWindow->renderer);
}

void cbmwLoadOffScreen(){
    SDL_SetRenderTarget(cbMainWindow->renderer, NULL);
    SDL_RenderCopy(cbMainWindow->renderer, cbMainWindow->texture, NULL, NULL);
    SDL_RenderPresent(cbMainWindow->renderer);
}

SDL_Rect cbmwDrawTextLine(dim_t row, dim_t col, cbFont_t const * font, cbColor_t const *color, const char *text){
    dim_t wrapLength = (cbMainWindow->w) - col; 
    SDL_SetRenderDrawColor(cbMainWindow->renderer, splitRGBA((*color)));
    TTF_SetFontSize(font->ttf, font->size);
    TTF_SetFontStyle(font->ttf, font->style);
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(
        font->ttf, 
        text, 
        color->SDL, 
        wrapLength
    );
    SDL_Texture *texture = SDL_CreateTextureFromSurface(cbMainWindow->renderer, surface);
    SDL_Rect rect = {col, row, surface->w, surface->h};
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(texture, SDL_ScaleModeLinear);
    SDL_RenderCopy(cbMainWindow->renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    return rect;
}

SDL_Rect cbmwDrawTextLineWithWrap(dim_t row, dim_t col, cbFont_t * font, cbColor_t *color, dim_t wrapL, const char *text){
    SDL_SetRenderDrawColor(cbMainWindow->renderer, splitRGBA((*color)));
    TTF_SetFontSize(font->ttf, font->size);
    TTF_SetFontStyle(font->ttf, font->style);
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(
        font->ttf, 
        text, 
        color->SDL, 
        wrapL
    );
    SDL_Texture *texture = SDL_CreateTextureFromSurface(cbMainWindow->renderer, surface);
    SDL_Rect rect = {col, row, surface->w, surface->h};
    // SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    // SDL_SetTextureScaleMode(texture, SDL_ScaleModeLinear);
    SDL_RenderCopy(cbMainWindow->renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    return rect;
}

void cbmwDrawTitle(){
    SDL_SetRenderDrawColor(cbMainWindow->renderer, splitRGBA(systemColor.title));
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(systemFont.title.ttf, CLIPBOARD_TITLE, systemColor.title.SDL, 500);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(cbMainWindow->renderer, surface);
    SDL_Rect rect = {10, 10, surface->w, surface->h};
    SDL_RenderCopy(cbMainWindow->renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void cbmwDrawInfo(){
    cbmwDrawTextLine(
        CLIPBOARD_HEIGHT-fontBodyH-5, 
        CLIPBOARD_WIDTH/7,
        &systemFont.body, 
        &systemColor.footer, 
        CLIPBOARD_FOOTER_INFO
    );
}

SDL_Rect cbmwDrawTextItem(int id, dim_t rowStart, const char * text){
    __entry1("cbmwDrawTextItem(%d, %s)",rowStart, text);
    SDL_Rect itemBg = {0, 0, 0, 0};
    
    static dim_t marginWinH     = 50;       /// For item
    static dim_t marginWinW     = 40;       /// For item
    static dim_t marginBgH      = 10;        /// For text
    static dim_t marginBgW      = 10;        /// For text
    // static dim_t marginTxtW    = 5;
    // static dim_t marginTxtH    = 5;

    if(rowStart > cbMainWindow->h) return itemBg;

    /// r:50|c:40 ---> r:16|c:W-60
    itemBg.y = (rowStart < marginWinH) ? marginWinH :(rowStart); 
    itemBg.x = marginWinW; 
    itemBg.w = cbMainWindow->w - marginWinW * 2; 
    itemBg.h = fontBodyH + marginBgH * 2;

    /// Create text surface
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(
        systemFont.body.ttf, 
        text, 
        systemColor.body.SDL, 
        itemBg.w - marginBgW
    );

    /// Determine box Height
    if( surface->h < 70){
        if(surface->h > itemBg.h - marginBgH * 2) itemBg.h = surface->h + marginBgH * 2; 
    }else{
        itemBg.h = 70;
    } 

    /// Draw background
    cbColor_t color; cbColorSetRGB(&color, 0xF3F2EC);
    SDL_SetRenderDrawColor(cbMainWindow->renderer, splitRGBA(color));
    SDL_RenderFillRect(cbMainWindow->renderer, &itemBg);

    /// Draw text

    SDL_Rect itemDestText = {
        .x = itemBg.x + marginBgW,
        .y = itemBg.y + marginBgH,
        .w = itemBg.w - marginBgW * 2,
        .h = itemBg.h - marginBgH * 2
    };

    cbmwItemArea[id].id = id;
    cbmwItemArea[id].SDL.x = itemBg.x;
    cbmwItemArea[id].SDL.y = itemBg.y;
    cbmwItemArea[id].SDL.w = itemBg.w;
    cbmwItemArea[id].SDL.h = itemBg.h;

    SDL_Rect itemSourceText = {
        .x = 0,
        .y = 0,
        .w = surface->w,
        .h = surface->h
    };

    if(itemSourceText.h <= itemDestText.h && itemSourceText.w <= itemDestText.w){
        itemDestText.h = itemSourceText.h;
        itemDestText.w = itemSourceText.w;
    }else
    if(itemSourceText.h > itemDestText.h && itemSourceText.w > itemDestText.w){
        itemSourceText.h = itemDestText.h;
        itemSourceText.w = itemDestText.w;
    }else
    if(itemSourceText.h > itemDestText.h && itemSourceText.w <= itemDestText.w){
        itemDestText.w = itemSourceText.w;
        itemSourceText.h = itemDestText.h;
    }else
    if(itemSourceText.h <= itemDestText.h && itemSourceText.w > itemDestText.w){
        itemDestText.h = itemSourceText.h;
        itemSourceText.w = itemDestText.w;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(cbMainWindow->renderer, surface);

    SDL_RenderCopy(cbMainWindow->renderer, texture, &itemSourceText, &itemDestText);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);

    __exit1("cbmwDrawTextItem()");
    return itemBg;
}

int cbmwItemClickedFind(){
    REP(i, 0, cbmwItemNum){
        if(cbmwItemArea[i].col <= cbmwMouseX && (cbmwMouseX <= cbmwItemArea[i].col + cbmwItemArea[i].w)
            && cbmwItemArea[i].row <= cbmwMouseY && cbmwMouseY <= (cbmwItemArea[i].row + cbmwItemArea[i].h)){
                cbmwMouseX = -1;
                cbmwMouseY = -1;
                return i;
            }
    }
    return -1;
}

def cbmwItemChangeContent(int id, const char * newContent){
    if(id >= cbmwItemNum) return ERR;
    snprintf(cbmwItemContent[id], 256, "%s", newContent);
}

def cbmwItemAppendContent(const char * newContent){
    if(cbmwItemNum >= CBMW_ITEM_NUM) return ERR;
    snprintf(cbmwItemContent[cbmwItemNum], 256, "%s", newContent);
    return ++cbmwItemNum;
}

def cbmwDrawItemId(int id){
    if(id < 1)
        cbmwDrawTextItem(id, 0, cbmwItemContent[id]);
    else
        cbmwDrawTextItem(id, cbmwItemArea[id-1].row+cbmwItemArea[id-1].h+10, cbmwItemContent[id]);
}
    

/// POP-UP ////////////////////////////////////////////////////////////////////////////////////////
/// cbpu = Clipboard pop-up

/// Must register before use!
uint32_t CB_POPUP_EVENT = -1;
char cbpuTitle[64]   = "Default title";
char cbpuMessage[256] = "This is a default popup message. Replace it with your actual content.";

#define cbPopup                 (wdcts.sub)

#define CB_POPUP_WIDTH          300
#define CB_POPUP_HEIGHT         150

#define cbpuButtonCloseRow      117
#define cbpuButtonCloseCol      241
#define cbpuButtonCloseH        24
#define cbpuButtonCloseW        45

#define isInsideButtonClose(r, c) \
    ((r) >= cbpuButtonCloseRow && (r) < (cbpuButtonCloseRow + cbpuButtonCloseH) && \
     (c) >= cbpuButtonCloseCol && (c) < (cbpuButtonCloseCol + cbpuButtonCloseW))

#define cbpuButtonYesRow        117
#define cbpuButtonYesCol        221
#define cbpuButtonYesH          24
#define cbpuButtonYesW          30

#define isInsideButtonYes(r, c) \
    ((r) >= cbpuButtonYesRow && (r) < (cbpuButtonYesRow + cbpuButtonYesH) && \
     (c) >= cbpuButtonYesCol && (c) < (cbpuButtonYesCol + cbpuButtonYesW))

#define cbpuButtonNoRow         117
#define cbpuButtonNoCol         259
#define cbpuButtonNoH           24
#define cbpuButtonNoW           30

#define isInsideButtonNo(r, c) \
    ((r) >= cbpuButtonNoRow && (r) < (cbpuButtonNoRow + cbpuButtonNoH) && \
     (c) >= cbpuButtonNoCol && (c) < (cbpuButtonNoCol + cbpuButtonNoW))

enum CBPU_EVENT_CODE{
    CBPU_EVENT_SHOW_MSG = 0,    /// S
    CBPU_EVENT_SHOW_YESNO,
    CBPU_EVENT_CLOSE,
};

#define CBPU_SET_TITLE(newTitle)    snprintf(cbpuTitle, sizeof(cbpuTitle), newTitle);
#define CBPU_SET_MESSAGE(newMsg)    snprintf(cbpuMessage, sizeof(cbpuMessage), newMsg);

static inline void __cbpuCreatePopup(){
    createWindowContext(&cbPopup, CB_POPUP_WIDTH, CB_POPUP_HEIGHT, cbpuTitle);
    SDL_SetRenderTarget(cbPopup->renderer, NULL);
    SDL_SetRenderDrawBlendMode(cbPopup->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(cbPopup->renderer, splitRGBA(systemColor.background));
    SDL_RenderClear(cbPopup->renderer);
}

static inline void __cbpuDrawTitle(){
    SDL_SetRenderDrawColor(cbPopup->renderer, splitRGBA(systemColor.heading1));
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(
        systemFont.heading1.ttf, 
        cbpuTitle, 
        systemColor.heading1.SDL, 
        cbPopup->w - 10
    );
    SDL_Texture *texture = SDL_CreateTextureFromSurface(cbPopup->renderer, surface);
    SDL_Rect itemBg = { .y = 5, .x = 5, .w = surface->w, .h = surface->h};
    // SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    // SDL_SetTextureScaleMode(texture, SDL_ScaleModeLinear);
    SDL_RenderCopy(cbPopup->renderer, texture, NULL, &itemBg);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

static inline void __cbpuDrawMessage(){
    SDL_SetRenderDrawColor(cbPopup->renderer, splitRGBA(systemColor.body));
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(
        systemFont.body.ttf, 
        cbpuMessage, 
        systemColor.body.SDL, 
        cbPopup->w - 20
    );
    SDL_Texture *texture = SDL_CreateTextureFromSurface(cbPopup->renderer, surface);
    SDL_Rect itemBg;
    itemBg.y = 5 + TTF_FontHeight(systemFont.heading1.ttf) + 5; 
    itemBg.x = 10; 
    itemBg.w = surface->w; 
    itemBg.h = surface->h; 
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(texture, SDL_ScaleModeLinear);
    SDL_RenderCopy(cbPopup->renderer, texture, NULL, &itemBg);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

static inline void __cbpuDrawCloseButton(){
    cbColor_t color; 
    cbColorSetRGB(&color, 0x44444E);
    SDL_SetRenderDrawColor(cbPopup->renderer, splitRGBA(color));
    SDL_Rect itemBg;
    itemBg.y = CB_POPUP_HEIGHT - TTF_FontHeight(systemFont.heading1.ttf) - 10 -3; 
    itemBg.x = CB_POPUP_WIDTH * 4 / 5 + 1; 
    itemBg.w = 45; 
    itemBg.h = TTF_FontHeight(systemFont.heading1.ttf) + 4;
    SDL_RenderFillRect(cbPopup->renderer, &itemBg);

    cbColorSetRGB(&color, 0xFFFFFF);
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(
        systemFont.body.ttf, 
        "CLOSE", 
        color.SDL, 
        cbPopup->w - 20
    );
    SDL_Texture *texture = SDL_CreateTextureFromSurface(cbPopup->renderer, surface);
    itemBg.y = CB_POPUP_HEIGHT - TTF_FontHeight(systemFont.heading1.ttf) - 10; 
    itemBg.x = CB_POPUP_WIDTH * 4 / 5 + 5; itemBg.w = surface->w; itemBg.h = surface->h; 
    SDL_RenderCopy(cbPopup->renderer, texture, NULL, &itemBg);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

static inline void __cbpuDrawYesNoButton(){
    cbColor_t color; 
    cbColorSetRGB(&color, 0x44444E);
    SDL_SetRenderDrawColor(cbPopup->renderer, splitRGBA(color));

    SDL_Rect itemBg;

    itemBg.y = CB_POPUP_HEIGHT - TTF_FontHeight(systemFont.heading1.ttf) - 10 -3; 
    itemBg.x = CB_POPUP_WIDTH * 3 / 4 - 4; 
    itemBg.w = 30; 
    itemBg.h = TTF_FontHeight(systemFont.heading1.ttf) + 4;
    __log("itemBg{row (y): %d, col (x): %d, h: %d, w: %d}", itemBg.y, itemBg.x, itemBg.h, itemBg.w);
    SDL_RenderFillRect(cbPopup->renderer, &itemBg);
    
    itemBg.y = CB_POPUP_HEIGHT - TTF_FontHeight(systemFont.heading1.ttf) - 10 -3; 
    itemBg.x = CB_POPUP_WIDTH * 3 / 4 + 34; 
    itemBg.w = 30; 
    itemBg.h = TTF_FontHeight(systemFont.heading1.ttf) + 4;
    __log("itemBg{row (y): %d, col (x): %d, h: %d, w: %d}", itemBg.y, itemBg.x, itemBg.h, itemBg.w);
    SDL_RenderFillRect(cbPopup->renderer, &itemBg);

    cbColorSetRGB(&color, 0xFFFFFF);
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(
        systemFont.body.ttf, 
        "YES   NO", 
        color.SDL, 
        cbPopup->w - 20
    );
    SDL_Texture *texture = SDL_CreateTextureFromSurface(cbPopup->renderer, surface);
    itemBg.y = CB_POPUP_HEIGHT - TTF_FontHeight(systemFont.heading1.ttf) - 10; 
    itemBg.x = CB_POPUP_WIDTH * 3 / 4; itemBg.w = surface->w; itemBg.h = surface->h; 
    SDL_RenderCopy(cbPopup->renderer, texture, NULL, &itemBg);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void cbswShowPopupMessage(){
    // __cbpuCreatePopup();
    SDL_SetRenderDrawColor(cbPopup->renderer, splitRGBA(systemColor.background));
    SDL_RenderClear(cbPopup->renderer);
    SDL_ShowWindow(cbPopup->window);
    __cbpuDrawTitle();
    __cbpuDrawMessage();
    __cbpuDrawCloseButton();
    SDL_RenderPresent(cbPopup->renderer);
}

void cbpuShowPopupYesNo(){
    // __cbpuCreatePopup();
    SDL_SetRenderDrawColor(cbPopup->renderer, splitRGBA(systemColor.background));
    SDL_RenderClear(cbPopup->renderer);
    SDL_ShowWindow(cbPopup->window);
    __cbpuDrawTitle();
    __cbpuDrawMessage();
    __cbpuDrawYesNoButton();
    SDL_RenderPresent(cbPopup->renderer);
}

void cbpuHidePopup(){
    SDL_HideWindow(cbPopup->window);
}

void cbpuDestroyPopupMessage(){
    destroyWindowContext(&cbPopup);
}

def cbpuPushEvent(int code, void *pv1, void *pv2) {
    SDL_Event event;
    SDL_zero(event);

    if (CB_POPUP_EVENT == (Uint32)-1) {
        __err("[cbpuPushEvent] CB_POPUP_EVENT chưa được đăng ký!");
        return -1;
    }

    event.type = CB_POPUP_EVENT;
    event.user.code = code;
    event.user.data1 = pv1;
    event.user.data2 = pv2;

    if (SDL_PushEvent(&event) < 0) {
        __err("[cbpuPushEvent] SDL_PushEvent thất bại: %s", SDL_GetError());
        return -1;
    }

    __log1("[cbpuPushEvent] Pushed event (code=%d, data1=%p, data2=%p)", code, pv1, pv2);
    return 0;
}

#endif