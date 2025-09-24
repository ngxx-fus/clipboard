#include "wdct/wdct.h"
#include "../global.h"

#include <zlib.h>

#define HEX2RGB(hex) (((hex)>>16)&0xFF), (((hex)>>8)&0xFF), (((hex)>>0)&0xFF), 0xFF

extern windowContext_t* mainWin;
extern flag             uiStatusFlag;
extern char             historyList[HISTORY_SIZE][MAX_PATHNAME_SIZE];
extern pthread_mutex_t  uiMutexLock;
extern pthread_mutex_t  uiFlagMutexLock;

enum UI_STATUS_BITORDER{
    CB_RELOAD = 0,
    CB_SCROLL_UP,
    CB_SCROLL_DOWN
};

int readTextContent(char *historyContent, size_t bufSize, const char *historyTextPath);
int readClipboardHistory(char history[HISTORY_SIZE][MAX_PATHNAME_SIZE], int read_size);
void drawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color);
void renderHistory(windowContext_t *ctx, int startIndex, int lineHeight);