#include "return.h"
#include "color.h"
#include "loop.h"
#include "arithmetic.h"
#include "../log/log.h"
#include "../config/config.h"

systemColor_t systemColor = {
    .title.abgr =       COLOR_TITLE,
    .body.abgr =        COLOR_BODY,
    .heading0.abgr =    COLOR_HEADING0,
    .heading1.abgr =    COLOR_HEADING1,
    .heading2.abgr =    COLOR_HEADING2,
    .heading3.abgr =    COLOR_HEADING3,
    .error.abgr =       COLOR_ERROR,
    .warning.abgr =     COLOR_WARNING,
    .background.abgr =  COLOR_BACKGROUND
};

void systemColorCorrectByteOrder(systemColor_t * sysColor){
    uint8_t tmpVar = 0, r, g, b, a;
    #if SDL_BYTEORDER == SDL_LIL_ENDIAN
        REP(i, 0, sizeof(systemColor_t)/sizeof(cbColor_t)){
            
            r = sysColor->arr[i].r;
            g = sysColor->arr[i].g; 
            b = sysColor->arr[i].b; 
            a = sysColor->arr[i].a; 
            
            sysColor->arr[i].r = a;
            sysColor->arr[i].g = b;
            sysColor->arr[i].b = g;
            sysColor->arr[i].a = r;
            
            __log1("[systemColorCorrectByteOrder] [Loop-%d.1] r=%d g=%d b=%d a=%d", i, r, g, b, a);
            #if CB_LOG_L1_EN
                a = sysColor->arr[i].a; g = sysColor->arr[i].g; b = sysColor->arr[i].b; r = sysColor->arr[i].r;
            #endif
            __log1("[systemColorCorrectByteOrder] [Loop-%d.2] r=%d g=%d b=%d a=%d\n", i, r, g, b, a);
        }
    #endif
}

const char *STR_DEFAULT_RETURN_STATUS(enum DEFAULT_RETURN_STATUS ret)
{
    switch (ret) {
        case OKE:                     return STR_OKE;
        case ERR:                     return STR_ERR;
        case ERR_NULL:                return STR_ERR_NULL;
        case ERR_MALLOC_FAILED:       return STR_ERR_MALLOC_FAILED;
        case ERR_TIMEOUT:             return STR_ERR_TIMEOUT;
        case ERR_BUSY:                return STR_ERR_BUSY;
        case ERR_INVALID_ARG:         return STR_ERR_INVALID_ARG;
        case ERR_OVERFLOW:            return STR_ERR_OVERFLOW;
        case ERR_UNDERFLOW:           return STR_ERR_UNDERFLOW;
        case ERR_NOT_FOUND:           return STR_ERR_NOT_FOUND;
        case ERR_ALREADY_EXISTS:      return STR_ERR_ALREADY_EXISTS;
        case ERR_NOT_IMPLEMENTED:     return STR_ERR_NOT_IMPLEMENTED;
        case ERR_UNSUPPORTED:         return STR_ERR_UNSUPPORTED;
        case ERR_IO:                  return STR_ERR_IO;
        case ERR_PERMISSION:          return STR_ERR_PERMISSION;
        case ERR_CRC:                 return STR_ERR_CRC;
        case ERR_INIT_FAILED:         return STR_ERR_INIT_FAILED;
        case ERR_PSRAM_FAILED:        return STR_ERR_PSRAM_FAILED;

        case ERR_SDL_INIT_FAILED:     return STR_ERR_SDL_INIT_FAILED;
        case ERR_SDL_WINDOW_FAILED:   return STR_ERR_SDL_WINDOW_FAILED;
        case ERR_SDL_RENDER_FAILED:   return STR_ERR_SDL_RENDER_FAILED;
        case ERR_SDL_TTF_FAILED:      return STR_ERR_SDL_TTF_FAILED;
        case ERR_SDL_IMAGE_FAILED:    return STR_ERR_SDL_IMAGE_FAILED;

        case ERR_XORG_DISPLAY_FAILED: return STR_ERR_XORG_DISPLAY_FAILED;
        case ERR_XORG_WINDOW_FAILED:  return STR_ERR_XORG_WINDOW_FAILED;
        case ERR_XORG_DRAW_FAILED:    return STR_ERR_XORG_DRAW_FAILED;

        case ERR_FILE_NOT_FOUND:      return STR_ERR_FILE_NOT_FOUND;
        case ERR_FILE_READ_FAILED:    return STR_ERR_FILE_READ_FAILED;
        case ERR_FILE_WRITE_FAILED:   return STR_ERR_FILE_WRITE_FAILED;
        case ERR_PATH_INVALID:        return STR_ERR_PATH_INVALID;

        default:                      return "ERR_UNKNOWN";
    }
}