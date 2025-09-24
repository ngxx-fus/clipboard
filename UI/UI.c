#include "UI.h"

windowContext_t*    mainWin         = NULL;
pthread_mutex_t     uiMutexLock     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     uiFlagMutexLock = PTHREAD_MUTEX_INITIALIZER;

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

    int count = 0;
    char line[512];
    while (count < read_size && fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = '\0';
        strncpy(history[count], line, MAX_PATHNAME_SIZE - 1);
        history[count][MAX_PATHNAME_SIZE - 1] = '\0';
        count++;
    }

    fclose(f);
    return count;
}

void drawText(SDL_Renderer *renderer, TTF_Font *font, 
              const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);  // dùng blended
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


void renderHistory(windowContext_t *ctx, int startIndex, int lineHeight) {

    lineHeight = (lineHeight)?20:lineHeight;
    // stopIndex  = (HISTORY_SIZE > stopIndex) ? stopIndex : HISTORY_SIZE;
    // stopIndex  = ((ctx->h - 30) / lineHeight) < stopIndex ? ((ctx->h - 30) / lineHeight) : stopIndex;

    SDL_SetRenderDrawColor(ctx->renderer, HEX2RGB(0xE7F2EF));
    SDL_RenderClear(ctx->renderer);

    SDL_Color white = {HEX2RGB(0x1B3C53)};
    drawText(ctx->renderer, ctx->font, "NGXXFUS' CLIPBOARD", 5, 5, white);
    char buff[512];
    char content[256]; size_t contentSize;
    int i = 0;
    while(i < HISTORY_SIZE){
        if (strlen(historyList[(startIndex+i)%HISTORY_SIZE])) {
            if(30 + (i+1) * lineHeight > ctx->h) break;
            
            contentSize = readTextContent(content, sizeof(content), historyList[(startIndex+i)%HISTORY_SIZE]);
            if(contentSize == 0){
                snprintf(buff, 256, "[%03d] : %s", i, content);
            }else{
                snprintf(buff, 256, "[%03d] : %s", i, "Error data!");

            }

            drawText(ctx->renderer, ctx->font, buff, 10, 30 + i * lineHeight, white);
        }
        ++i;
    }

    SDL_RenderPresent(ctx->renderer);
}


