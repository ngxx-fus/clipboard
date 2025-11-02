#include "windowContext/wdct.h"
#include "../helper/general.h"

#include <zlib.h>

#define HEX2RGB(hex) (((hex)>>16)&0xFF), (((hex)>>8)&0xFF), (((hex)>>0)&0xFF), 0xFF
#define MAX_CB_BLOCK_AREA_RECORD    200

extern windowContext_t* cbMainWindow;
extern flag_t           uiStatusFlag;
extern char             historyList[HISTORY_SIZE][MAX_PATHNAME_SIZE];
extern pthread_mutex_t  uiMutexLock;
extern pthread_mutex_t  cbFlagMutexLock;
extern pthread_mutex_t  clickedClipboardIDMutexLock;
extern int              cbHistoryCount;
extern int              cbCurrentHistoryIndex;
extern int              cbClickedClipboardNum;



void drawTextBlock(windowContext_t *wctx, SDL_Rect * outline, const char *text, SDL_Color color);
int readTextContent(char *historyContent, size_t bufSize, const char *historyTextPath);
int readClipboardHistory(char history[HISTORY_SIZE][MAX_PATHNAME_SIZE], int read_size);
void drawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color);
void renderHistory(windowContext_t *ctx, cbBlockArea_t * cbBlckArea, int startIndex);