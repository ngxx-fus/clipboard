#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "stdint.h"

#define MAX_FILENAME_SIZE           64
#define MAX_PATHNAME_SIZE           128
#define CLIPBOARD_WIDTH             640 
#define CLIPBOARD_HEIGHT            480 
#define CLIPBOARD_TITLE             "Ngxxfus' clipboard"
#define FONT_PATH                   "/usr/share/fonts/TTF/DejaVuSans.ttf"
#define FONT_SIZE                   12
#define HISTORY_SIZE                1000 /* adjustable */
#define POLL_MS                     200
#define BASE_DIR                    "/tmp/clipboard"
#define HISTORY_FILE BASE_DIR       "/history"
#define IMG_DIR BASE_DIR            "/imgs"
#define NEXTID_FILE BASE_DIR        "/.nextid"
#define ID_WIDTH                    4
#define KEYCODE_ESC                 9
#define KEYCODE_Q                   24

typedef uint64_t                    flag;

#define setFlag(flagReg, flag)      ((flagReg) |= (1ULL<<(flag)) )
#define clrFlag(flagReg, flag)      ((flagReg) &= (~(1ULL<<(flag)))) 
#define hasFlag(flagReg, flag)      ((flagReg) & (1ULL<<(flag)))

enum SYSTEM_STAGE_FLAG_BITORDER{
    SYS_RUNNING = 0,
};



#endif