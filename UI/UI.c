#include "UI.h"

windowContext_t*    mainWin         = NULL;
pthread_mutex_t     uiMutexLock     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     uiFlagMutexLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     clickedClipboardIDMutexLock = PTHREAD_MUTEX_INITIALIZER;
int                 historyCount    = 0;
cbBlockArea_t       cbba[MAX_CB_BLOCK_AREA_RECORD] = {};
int                 currentHistoryIndex;
cbClicked_t         cbClickedMouseInfo;
int                 clickedClipboardNum  = 0;

flag uiStatusFlag = 0;
char historyList[HISTORY_SIZE][MAX_PATHNAME_SIZE];

typedef enum {
    HISTORY_UNKNOWN = 0,
    HISTORY_TEXT,
    HISTORY_IMAGE
} history_t;

history_t getHistoryType(const char *path) {
    if (!path) return HISTORY_UNKNOWN;

    const char *ext = strrchr(path, '.');  // tìm extension
    if (!ext) return HISTORY_UNKNOWN;

    if (strcmp(ext, ".txt") == 0 || strcmp(ext, ".gz") == 0) {
        return HISTORY_TEXT;
    }
    else if (strcmp(ext, ".png") == 0 || strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
        return HISTORY_IMAGE;
    }

    return HISTORY_UNKNOWN;
}

int readTextContent(char *historyContent, size_t bufSize, const char *historyTextPath) {
    if (!historyContent || !historyTextPath) return -1;

    if (getHistoryType(historyTextPath) == HISTORY_IMAGE) return -1;

    gzFile gz = gzopen(historyTextPath, "rb");
    if (!gz) {
        perror("gzopen");
        return -1;
    }

    int n = gzread(gz, historyContent, bufSize - 1);
    if (n < 0) {
        int errnum;
        const char *errmsg = gzerror(gz, &errnum);
        fprintf(stderr, "gzread error: %s\n", errmsg);
        gzclose(gz);
        return -1;
    }

    historyContent[n] = '\0';  // null-terminate
    gzclose(gz);

    return 0;
}


int readClipboardHistory(char history[HISTORY_SIZE][MAX_PATHNAME_SIZE], int read_size) {
    FILE *f = fopen(HISTORY_FILE, "r");
    if (!f) {
        perror("fopen history read");
        return -1;
    }

    historyCount = 0;
    char line[512];
    while (historyCount < read_size && fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = '\0';
        strncpy(history[historyCount], line, MAX_PATHNAME_SIZE - 1);
        history[historyCount][MAX_PATHNAME_SIZE - 1] = '\0';
        historyCount++;
    }

    fclose(f);
    return historyCount;
}


/// @brief Draw a text block onto the texture in wctx
/// @param wctx The Window Context
/// @param outline 
/// @param text The text to be printed in the block
/// @param color The color
void drawTextBlock(windowContext_t *wctx, SDL_Rect * outline, const char *text, SDL_Color color){
    __entry("drawTextBlock(%p, {%d, %d, %d, %d}, \"%s\", {%d, %d, %d, %d})", wctx, outline->x, outline->y, outline->h, outline->w, text, color.r, color.g, color.b, color.a);
    /// Set target of Renderer to the temp 
    int fontH = TTF_FontHeight(wctx->font);
    int lineSkip = TTF_FontLineSkip(wctx->font);
    SDL_SetRenderDrawColor(wctx->renderer, color.r, color.g, color.b, color.a);
    
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(wctx->font, text, color, outline->w - 4);
    if (!surface) {
        __err("TTF_RenderText_Blended failed: %s\n", TTF_GetError());
        return;
    }
    __log("[drawTextBlock] surface_WxH=%dx%d", surface->w, surface->h);
    SDL_Rect src = { .y = 0, .y = 0, .h = surface->h, .w = surface->w };
    SDL_Rect dst = *outline;
    SDL_Rect out = *outline;

    if(surface->h < 5 * fontH + 4 * lineSkip){
        dst.h = src.h;
    }else{
        dst.h = src.h = 4 * fontH + 3 * lineSkip;
    }

    outline->h = out.h = src.h + 4;
    outline->y = outline->y + out.h + 2;

    SDL_RenderDrawRect(wctx->renderer, &out);

    if(surface->w < outline->w - 4){
        dst.w = src.w;
    }else{
        src.w = dst.w = outline->w - 4;
    }

    dst.x+=2;
    dst.y+=2;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(wctx->renderer, surface);
    if (!texture) {
        fprintf(stderr, "SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    // SDL_SetRenderTarget(wctx->renderer, NULL);
    if(surface->w < outline->w || surface->h < outline->h){
        SDL_RenderCopy(wctx->renderer, texture, &src, &dst);
    }else{
        SDL_RenderCopy(wctx->renderer, texture, &src, &dst);
    }

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    __exit("drawTextBlock");
}

void drawText(SDL_Renderer *renderer, TTF_Font *font, 
              const char *text, int x, int y, SDL_Color color) {
    // SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);  // dùng blended
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(font, text, color, 250);
    if (!surface) {
        fprintf(stderr, "TTF_RenderText_Blended failed: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        fprintf(stderr, "SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


void renderHistory(windowContext_t *ctx, cbBlockArea_t * cbBlckArea, int startIndex) {
    TTF_SetFontSize(ctx->font, 18);

    currentHistoryIndex = startIndex;

    SDL_SetRenderDrawColor(ctx->renderer, HEX2RGB(0xE7F2EF));
    SDL_RenderClear(ctx->renderer);

    SDL_Color white = {HEX2RGB(0x1B3C53)};
    drawText(ctx->renderer, ctx->font, "NGXXFUS' CLIPBOARD", 5, 5, white);
    
    char buff[512];
    char content[256]; size_t contentSize;
    int i = 0;
    
    SDL_Rect outline = {
        .y = TTF_FontHeight(ctx->font)+6, 
        .x = 5, 
        .h = 0, /// Don't care! 
        .w = ctx->w - 10
    };
    
    TTF_SetFontSize(ctx->font, FONT_SIZE);
    
    while(i < HISTORY_SIZE){
        if (strlen(historyList[(startIndex+i)%historyCount])) {
            if(outline.y > ctx->h) break;
            cbBlckArea[i].row = outline.y;
            contentSize = readTextContent(content, sizeof(content), historyList[(startIndex+i)%HISTORY_SIZE]);
            if(contentSize == 0){
                snprintf(buff, 256, "[%d]\n%s", (startIndex+i)%historyCount, content);
            }else{
                snprintf(buff, 256, "[%d]\n%s", (startIndex+i)%historyCount, "Error data!");

            }

            drawTextBlock(ctx, &outline, buff, white);
            cbBlckArea[i].col = outline.x;
            cbBlckArea[i].w   = outline.w;
            cbBlckArea[i].h   = outline.h;
        }
        ++i;
    }
    clickedClipboardNum = i;
    SDL_RenderPresent(ctx->renderer);
}


