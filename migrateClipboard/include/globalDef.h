/// HEADERs ///////////////////////////////////////////////////////////////////////////////////////

#include "../helper/general.h"
#include "../windowContext/wdct.h"
#include "../clipboardManager/clipboardManager.h"

#include "stageCtl.h"
#include "fileCtl.h"

/// VARs & DEFs ///////////////////////////////////////////////////////////////////////////////////

volatile flag_t systemFlag          = NONE_FLAG_SET;
pthread_mutex_t systemFlagLock      = PTHREAD_MUTEX_INITIALIZER;

windowContext_t*    cbMainWindow      = NULL;
pthread_mutex_t     cbFlagMutexLock = PTHREAD_MUTEX_INITIALIZER;

typedef struct cbClicked_t{
    size_t mouseX, mouseY;
}cbClicked_t;

typedef struct cbBlockArea_t{
    size_t row, col, h, w;
} cbBlockArea_t;

enum UI_STATUS_BITORDER{
    CB_RELOAD = 0,
    CB_SCROLL_UP,
    CB_SCROLL_DOWN,
    CB_MOUSE_CLICKED
};

#define HEX2RGB(hex) (((hex)>>16)&0xFF), (((hex)>>8)&0xFF), (((hex)>>0)&0xFF), 0xFF
#define MAX_CB_BLOCK_AREA_RECORD    200

cbBlockArea_t       cbba[MAX_CB_BLOCK_AREA_RECORD] = {};
int                 cbCurrentHistoryIndex;
cbClicked_t         cbClickedMouseInfo;
int                 cbClickedClipboardNum  = 0;

volatile flag_t  uiStatusFlag = 0;

void cbmwSetRenderTargetOnScreen(){
    SDL_SetRenderTarget(cbMainWindow->renderer, NULL);
}

void cbmwSetRenderTargetOffScreen(){
    SDL_SetRenderTarget(cbMainWindow->renderer, cbMainWindow->texture);
}

void cbmwClearBackground(){
    cbMainWindow;
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

void cbmwDrawTextLine(dim_t row, dim_t col, cbFont_t const * font, cbColor_t const *color, const char *text){
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
    SDL_Rect dstRect = {col, row, surface->w, surface->h};
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(texture, SDL_ScaleModeLinear);
    SDL_RenderCopy(cbMainWindow->renderer, texture, NULL, &dstRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void cbmwDrawTextLineWithWrap(dim_t row, dim_t col, cbFont_t * font, cbColor_t *color, dim_t wrapL, const char *text){

}

void cbmwDrawTitle(){
    SDL_SetRenderDrawColor(cbMainWindow->renderer, splitRGBA(systemColor.title));
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(systemFont.title.ttf, CLIPBOARD_TITLE, systemColor.title.SDL, 500);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(cbMainWindow->renderer, surface);
    SDL_Rect dstRect = {10, 10, surface->w, surface->h};
    SDL_RenderCopy(cbMainWindow->renderer, texture, NULL, &dstRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void cbmwShowPopup(windowContext_t *diag, const char *title, const char *message) {
    __entry1("cbmwShowPopup(%s)", message);

    // if (createWindowContext(&diag, 200, 100, "Pop-up") != 0 || !diag) {
    //     __log1("[cbmwShowPopup] Failed to create window context");
    //     __exit1("cbmwShowPopup");
    //     return;
    // }

    SDL_Renderer *renderer = diag->renderer;
    if (!renderer) {
        __err("[cbmwShowPopup] Renderer is NULL");
        destroyWindowContext(&diag);
        __exit1("cbmwShowPopup");
        return;
    }

    __log1("[cbmwShowPopup] Begin drawing popup background");
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_Rect overlay = {0, 0, 800, 600};
    SDL_RenderFillRect(renderer, &overlay);

    SDL_Rect dialog = {200, 200, 400, 200};
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderFillRect(renderer, &dialog);

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &dialog);

    __log1("[cbmwShowPopup] Rendering message text");

    SDL_Color color = {0, 0, 0, 255};
    SDL_Surface *surf = TTF_RenderUTF8_Blended(systemFont.heading1.ttf, message, systemColor.heading1.SDL);
    if (!surf) {
        __log("[cbmwShowPopup] TTF_RenderUTF8_Blended failed: %s", TTF_GetError());
        destroyWindowContext(&diag);
        __exit("cbmwShowPopup");
        return;
    }

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (!tex) {
        __log("[cbmwShowPopup] SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        SDL_FreeSurface(surf);
        destroyWindowContext(&diag);
        __exit("cbmwShowPopup");
        return;
    }

    SDL_Rect textRect = {
        dialog.x + (dialog.w - surf->w) / 2,
        dialog.y + 60,
        surf->w,
        surf->h
    };

    SDL_RenderCopy(renderer, tex, NULL, &textRect);
    SDL_RenderPresent(renderer);

    __log1("[cbmwShowPopup] Waiting for user input to close popup...");

    SDL_Event e;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN) {
                running = 0;
                break;
            }
        }
        SDL_Delay(10);
    }

    __log1("[cbmwShowPopup] Cleaning up popup resources");
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);

    __exit1("cbmwShowPopup()");
}

void drawTextBlock(windowContext_t *wctx, SDL_Rect *outline, const char *text, SDL_Color color){
    __entry1("[drawTextBlock] (%p, {%d,%d,%d,%d}, \"%s\", {%d,%d,%d,%d})",
        wctx, outline->x, outline->y, outline->w, outline->h,
        text ? text : "(null)", color.r, color.g, color.b, color.a);

    if (!wctx || !outline || !text) {
        __err("[drawTextBlock] Invalid argument(s)");
        return;
    }

    int fontH = TTF_FontHeight(systemFont.body.ttf);
    int lineSkip = TTF_FontLineSkip(systemFont.body.ttf);
    __log1("[drawTextBlock] fontH=%d lineSkip=%d", fontH, lineSkip);

    SDL_SetRenderDrawColor(wctx->renderer, color.r, color.g, color.b, color.a);

    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(systemFont.body.ttf, text, color, outline->w - 4);
    if (!surface) {
        __err("[drawTextBlock] TTF_RenderUTF8_Blended_Wrapped failed: %s", TTF_GetError());
        return;
    }
    __log1("[drawTextBlock] Surface created WxH=%dx%d", surface->w, surface->h);

    SDL_Rect src = { .x = 0, .y = 0, .h = surface->h, .w = surface->w };
    SDL_Rect dst = *outline;
    SDL_Rect out = *outline;

    if (surface->h < 5 * fontH + 4 * lineSkip) {
        dst.h = src.h;
    } else {
        dst.h = src.h = 4 * fontH + 3 * lineSkip;
    }

    outline->h = out.h = src.h + 4;
    outline->y += out.h + 2;

    __log1("[drawTextBlock] Adjusted rects: src(%d,%d,%d,%d), dst(%d,%d,%d,%d)",
        src.x, src.y, src.w, src.h, dst.x, dst.y, dst.w, dst.h);

    SDL_RenderDrawRect(wctx->renderer, &out);

    if (surface->w < outline->w - 4) {
        dst.w = src.w;
    } else {
        src.w = dst.w = outline->w - 4;
    }

    dst.x += 2;
    dst.y += 2;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(wctx->renderer, surface);
    if (!texture) {
        __err("[drawTextBlock] SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_RenderCopy(wctx->renderer, texture, &src, &dst);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);

    __exit1("[drawTextBlock] Done rendering text block.\n");
}

void drawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color) {
    __entry1("[drawText] (renderer=%p, font=%p, text=\"%s\", x=%d, y=%d, color={%d,%d,%d,%d})",
        renderer, font, text ? text : "(null)", x, y, color.r, color.g, color.b, color.a);

    if (!renderer || !font || !text) {
        __err("[drawText] Invalid argument(s)");
        return;
    }

    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(font, text, color, 250);
    if (!surface) {
        __err("[drawText] TTF_RenderUTF8_Blended_Wrapped failed: %s", TTF_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        __err("[drawText] SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    __log1("[drawText] Drawn text \"%s\" at (%d,%d) WxH=%dx%d", text, x, y, surface->w, surface->h);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    __exit1("[drawText]");
}

void renderHistory(windowContext_t *ctx, cbBlockArea_t *cbBlckArea, int startIndex) {
    __entry1("renderHistory(%p, %d)", ctx, startIndex);
    if (!ctx || !cbBlckArea) {
        __err("[renderHistory] Invalid argument(s)");
        return;
    }

    cbCurrentHistoryIndex = startIndex;

    // TTF_SetFontSize(systemFont.heading1.ttf, 18);
    
    SDL_SetRenderTarget(ctx->renderer, NULL);
    SDL_SetRenderDrawColor(ctx->renderer, HEX2RGB(0xE7F2EF));
    SDL_RenderClear(ctx->renderer);

    SDL_Color white = {HEX2RGB(0x1B3C53)};
    // drawText(ctx->renderer, systemFont.heading1.ttf, "NGXXFUS' CLIPBOARD", 5, 5, white);

    char buff[512];
    char content[256];
    size_t contentSize;
    int i = 0;

    SDL_Rect outline = {
        .y = TTF_FontHeight(systemFont.heading1.ttf) + 6,
        .x = 5,
        .h = 0,
        .w = ctx->w - 10
    };

    TTF_SetFontSize(systemFont.body.ttf, systemFont.body.size);
    __log1("[renderHistory] Start rendering clipboard history...");

    while (i < HISTORY_SIZE) {
        if (strlen(historyPathFileList[(startIndex + i) % historyPathFileNum])) {
            if (outline.y > ctx->h) break;

            cbBlckArea[i].row = outline.y;
            contentSize = readTextContent(content, sizeof(content),
                                          historyPathFileList[(startIndex + i) % HISTORY_SIZE]);
            if (contentSize == 0) {
                snprintf(buff, sizeof(buff), "[%d]\n%s",
                         (startIndex + i) % historyPathFileNum, content);
            } else {
                snprintf(buff, sizeof(buff), "[%d]\n%s",
                         (startIndex + i) % historyPathFileNum, "Error data!");
            }

            drawTextBlock(ctx, &outline, buff, white);

            cbBlckArea[i].col = outline.x;
            cbBlckArea[i].w = outline.w;
            cbBlckArea[i].h = outline.h;

            __log1("[renderHistory] Block[%d] rendered at y=%d h=%d", i, outline.y, outline.h);
        }
        ++i;
    }

    cbClickedClipboardNum = i;
    SDL_RenderPresent(ctx->renderer);
    __exit1("[renderHistory] Total rendered blocks: %d", i);
}
