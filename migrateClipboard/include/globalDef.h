#ifndef __GLOBAL_DEF_H__
#define __GLOBAL_DEF_H__

/// HEADERs ///////////////////////////////////////////////////////////////////////////////////////

#include "../helper/general.h"
#include "../windowContext/wdct.h"
#include "../clipboardManager/clipboardManager.h"

#include "stageCtl.h"
#include "fileCtl.h"
#include "windowCtl.h" 

/// VARs & DEFs ///////////////////////////////////////////////////////////////////////////////////

/// SYSTEM FLAG /////////////////////////////////////////////////////////////////////////

/// @brief System flag, do not access directly
_Atomic flag_t systemFlag;

#define __systemFlagFull()          atomic_load(&systemFlag)
#define __systemFlagCheck(bit)      atomic_load(&systemFlag) & __flagMask(bit)
#define __systemFlagSet(bit)        atomic_fetch_or(&systemFlag, __flagMask(bit))
#define __systemFlagClr(bit)        atomic_fetch_and(&systemFlag, __flagInvMask(bit))

enum SYSTEM_FLAG {
    SYSTEM_FLAG_NUM,
};

/// UI FLAG /////////////////////////////////////////////////////////////////////////////

_Atomic flag_t uiFlag;

#define __uiFlagGetFull()           atomic_load(&uiFlag)
#define __uiFlagSetFull(new)        atomic_store(&uiFlag, new)
#define __uiFlagCheck(bit)          atomic_load(&uiFlag) & __flagMask(bit)
#define __uiFlagSet(bit)            atomic_fetch_or(&uiFlag, __flagMask(bit))
#define __uiFlagClr(bit)            atomic_fetch_and(&uiFlag, __flagInvMask(bit))

enum UI_FLAG{
    RESERVED = 0,
    /// POP_UP
    UI_FLAG_POPUP_CLICKED_CLOSE,
    UI_FLAG_POPUP_CLICKED_YES,
    UI_FLAG_POPUP_CLICKED_NO,
    UI_FLAG_NUM
};

uint8_t appAgreement = 0;

/// OLD 

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

#define MAX_CB_BLOCK_AREA_RECORD    200

cbBlockArea_t       cbba[MAX_CB_BLOCK_AREA_RECORD] = {};
int                 cbCurrentHistoryIndex;
cbClicked_t         cbClickedMouseInfo;
int                 cbClickedClipboardNum  = 0;



// void drawTextBlock(windowContext_t *wctx, SDL_Rect *outline, const char *text, SDL_Color color){
//     __entry1("[drawTextBlock] (%p, {%d,%d,%d,%d}, \"%s\", {%d,%d,%d,%d})",
//         wctx, outline->x, outline->y, outline->w, outline->h,
//         text ? text : "(null)", color.r, color.g, color.b, color.a);

//     if (!wctx || !outline || !text) {
//         __err("[drawTextBlock] Invalid argument(s)");
//         return;
//     }

//     int fontH = TTF_FontHeight(systemFont.body.ttf);
//     int lineSkip = TTF_FontLineSkip(systemFont.body.ttf);
//     __log1("[drawTextBlock] fontH=%d lineSkip=%d", fontH, lineSkip);

//     SDL_SetRenderDrawColor(wctx->renderer, color.r, color.g, color.b, color.a);

//     SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(systemFont.body.ttf, text, color, outline->w - 4);
//     if (!surface) {
//         __err("[drawTextBlock] TTF_RenderUTF8_Blended_Wrapped failed: %s", TTF_GetError());
//         return;
//     }
//     __log1("[drawTextBlock] Surface created WxH=%dx%d", surface->w, surface->h);

//     SDL_Rect src = { .x = 0, .y = 0, .h = surface->h, .w = surface->w };
//     SDL_Rect dst = *outline;
//     SDL_Rect out = *outline;

//     if (surface->h < 5 * fontH + 4 * lineSkip) {
//         dst.h = src.h;
//     } else {
//         dst.h = src.h = 4 * fontH + 3 * lineSkip;
//     }

//     outline->h = out.h = src.h + 4;
//     outline->y += out.h + 2;

//     __log1("[drawTextBlock] Adjusted rects: src(%d,%d,%d,%d), dst(%d,%d,%d,%d)",
//         src.x, src.y, src.w, src.h, dst.x, dst.y, dst.w, dst.h);

//     SDL_RenderDrawRect(wctx->renderer, &out);

//     if (surface->w < outline->w - 4) {
//         dst.w = src.w;
//     } else {
//         src.w = dst.w = outline->w - 4;
//     }

//     dst.x += 2;
//     dst.y += 2;

//     SDL_Texture *texture = SDL_CreateTextureFromSurface(wctx->renderer, surface);
//     if (!texture) {
//         __err("[drawTextBlock] SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
//         SDL_FreeSurface(surface);
//         return;
//     }

//     SDL_RenderCopy(wctx->renderer, texture, &src, &dst);

//     SDL_DestroyTexture(texture);
//     SDL_FreeSurface(surface);

//     __exit1("[drawTextBlock] Done rendering text block.\n");
// }

// void drawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color) {
//     __entry1("[drawText] (renderer=%p, font=%p, text=\"%s\", x=%d, y=%d, color={%d,%d,%d,%d})",
//         renderer, font, text ? text : "(null)", x, y, color.r, color.g, color.b, color.a);

//     if (!renderer || !font || !text) {
//         __err("[drawText] Invalid argument(s)");
//         return;
//     }

//     SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(font, text, color, 250);
//     if (!surface) {
//         __err("[drawText] TTF_RenderUTF8_Blended_Wrapped failed: %s", TTF_GetError());
//         return;
//     }

//     SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
//     if (!texture) {
//         __err("[drawText] SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
//         SDL_FreeSurface(surface);
//         return;
//     }

//     SDL_Rect dstRect = {x, y, surface->w, surface->h};
//     SDL_RenderCopy(renderer, texture, NULL, &dstRect);

//     __log1("[drawText] Drawn text \"%s\" at (%d,%d) WxH=%dx%d", text, x, y, surface->w, surface->h);

//     SDL_FreeSurface(surface);
//     SDL_DestroyTexture(texture);
//     __exit1("[drawText]");
// }

// void renderHistory(windowContext_t *ctx, cbBlockArea_t *cbBlckArea, int startIndex) {
//     __entry1("renderHistory(%p, %d)", ctx, startIndex);
//     if (!ctx || !cbBlckArea) {
//         __err("[renderHistory] Invalid argument(s)");
//         return;
//     }

//     cbCurrentHistoryIndex = startIndex;

//     // TTF_SetFontSize(systemFont.heading1.ttf, 18);
    
//     SDL_SetRenderTarget(ctx->renderer, NULL);
//     SDL_SetRenderDrawColor(ctx->renderer, HEX2RGB(0xE7F2EF));
//     SDL_RenderClear(ctx->renderer);

//     SDL_Color white = {HEX2RGB(0x1B3C53)};
//     // drawText(ctx->renderer, systemFont.heading1.ttf, "NGXXFUS' CLIPBOARD", 5, 5, white);

//     char buff[512];
//     char content[256];
//     size_t contentSize;
//     int i = 0;

//     SDL_Rect outline = {
//         .y = TTF_FontHeight(systemFont.heading1.ttf) + 6,
//         .x = 5,
//         .h = 0,
//         .w = ctx->w - 10
//     };

//     TTF_SetFontSize(systemFont.body.ttf, systemFont.body.size);
//     __log1("[renderHistory] Start rendering clipboard history...");

//     while (i < HISTORY_SIZE) {
//         if (strlen(historyPathFileList[(startIndex + i) % historyPathFileNum])) {
//             if (outline.y > ctx->h) break;

//             cbBlckArea[i].row = outline.y;
//             contentSize = readTextContent(content, sizeof(content),
//                                           historyPathFileList[(startIndex + i) % HISTORY_SIZE]);
//             if (contentSize == 0) {
//                 snprintf(buff, sizeof(buff), "[%d]\n%s",
//                          (startIndex + i) % historyPathFileNum, content);
//             } else {
//                 snprintf(buff, sizeof(buff), "[%d]\n%s",
//                          (startIndex + i) % historyPathFileNum, "Error data!");
//             }

//             drawTextBlock(ctx, &outline, buff, white);

//             cbBlckArea[i].col = outline.x;
//             cbBlckArea[i].w = outline.w;
//             cbBlckArea[i].h = outline.h;

//             __log1("[renderHistory] Block[%d] rendered at y=%d h=%d", i, outline.y, outline.h);
//         }
//         ++i;
//     }

//     cbClickedClipboardNum = i;
//     SDL_RenderPresent(ctx->renderer);
//     __exit1("[renderHistory] Total rendered blocks: %d", i);

// }



#endif