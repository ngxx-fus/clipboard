#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

///-------------------------------------- START USER CODE --------------------------------------///

#include "stdio.h"
#include "stdint.h"

/// LOG CONFIG ////////////////////////////////////////////////////////////////////////////////////

#define CB_LOG_L0_EN                1
#define CB_LOG_ERR_EN               1
#define CB_LOG_L0_ENTRY             1
#define CB_LOG_L0_EXIT              1

#define CB_LOG_L1_EN                1
#define CB_LOG_L1_ENTRY             1
#define CB_LOG_L1_EXIT              1

#define CB_LOG_L2_EN                0
#define CB_LOG_L2_ENTRY             0
#define CB_LOG_L2_EXIT              0

/// CB SYNC CONFIG ////////////////////////////////////////////////////////////////////////////////
#define POLL_MS                     200
#define BASE_DIR                    "/tmp/clipboard"
#define HISTORY_FILE BASE_DIR       "/history"
#define IMG_DIR BASE_DIR            "/imgs"
#define NEXTID_FILE BASE_DIR        "/.nextid"
#define HISTORY_SIZE                1000 /* adjustable */
#define ID_WIDTH                    4
#define MAX_FILENAME_SIZE           30
#define MAX_PATHNAME_SIZE           64


/// ??? ?????? ////////////////////////////////////////////////////////////////////////////////////
#define CLIPBOARD_WIDTH             640 
#define CLIPBOARD_HEIGHT            480 
#define CLIPBOARD_TITLE             " NGXXFUS' CLIPBOARD"
#define KEYCODE_ESC                 9
#define KEYCODE_Q                   24

/// ??? ?????? ////////////////////////////////////////////////////////////////////////////////////
#define FONT_TITLE_PATH         "/usr/share/fonts/NerdFonts/ttf/BitstromWeraNerdFontPropo-Bold.ttf"
#define FONT_BODY_PATH          "/usr/share/fonts/NerdFonts/ttf/BlexMonoNerdFontPropo-Medium.ttf"
#define FONT_HEADING0_PATH      "/usr/share/fonts/TTF/Roboto-Bold.ttf"
#define FONT_HEADING1_PATH      "/usr/share/fonts/TTF/Roboto-Medium.ttf"
#define FONT_HEADING2_PATH      "/usr/share/fonts/TTF/Roboto-Regular.ttf"
#define FONT_HEADING3_PATH      "/usr/share/fonts/TTF/DejaVuSans-Bold.ttf"
#define FONT_ERROR_PATH         "/usr/share/fonts/TTF/FreeSansBoldOblique.ttf"
#define FONT_WARNING_PATH       "/usr/share/fonts/TTF/FreeSansOblique.ttf"

#define FONT_TITLE_SIZE         22
#define FONT_HEADING0_SIZE      18
#define FONT_HEADING1_SIZE      16
#define FONT_HEADING2_SIZE      14
#define FONT_HEADING3_SIZE      13
#define FONT_BODY_SIZE          12
#define FONT_ERROR_SIZE         13
#define FONT_WARNING_SIZE       13

#define STYLE_TITLE             (CBF_STYLE_BOLD)
#define STYLE_BODY              (CBF_STYLE_NORMAL)
#define STYLE_HEADING0          (CBF_STYLE_BOLD)
#define STYLE_HEADING1          (CBF_STYLE_BOLD | CBF_STYLE_ITALIC | CBF_STYLE_UNDERLINE)
#define STYLE_HEADING2          (CBF_STYLE_BOLD | CBF_STYLE_ITALIC)
#define STYLE_HEADING3          (CBF_STYLE_BOLD | CBF_STYLE_ITALIC)
#define STYLE_ERROR             (CBF_STYLE_BOLD | CBF_STYLE_UNDERLINE)
#define STYLE_WARNING           (CBF_STYLE_BOLD)

#define COLOR_RGB_TITLE         016B61
#define COLOR_RGB_BODY          DDDDDD
#define COLOR_RGB_HEADING0      FFE97F
#define COLOR_RGB_HEADING1      7FD4FF
#define COLOR_RGB_HEADING2      FFAAFF
#define COLOR_RGB_HEADING2      FFAAFF
#define COLOR_RGB_HEADING3      C0FFC0
#define COLOR_RGB_ERROR         BF092F
#define COLOR_RGB_WARNING       FFD633
#define COLOR_RGB_BACKGROUND    FFFFFF

/// FIXED SECTION | DO NOT CHANGE /////////////////////////////////////////////////////////////////

#define _COLOR_RGB_COMBINE(hex) 0x##hex##FFu                /// Get value of hex if it's macro.
#define COLOR_RGB2RGBA(hex)     _COLOR_RGB_COMBINE(hex)     /// Append "FFU" to convert RGB to RGBA.

#define COLOR_TITLE             COLOR_RGB2RGBA(COLOR_RGB_TITLE)
#define COLOR_BODY              COLOR_RGB2RGBA(COLOR_RGB_BODY)
#define COLOR_HEADING0          COLOR_RGB2RGBA(COLOR_RGB_HEADING0)
#define COLOR_HEADING1          COLOR_RGB2RGBA(COLOR_RGB_HEADING1)
#define COLOR_HEADING2          COLOR_RGB2RGBA(COLOR_RGB_HEADING2)
#define COLOR_HEADING3          COLOR_RGB2RGBA(COLOR_RGB_HEADING3)
#define COLOR_ERROR             COLOR_RGB2RGBA(COLOR_RGB_ERROR)
#define COLOR_WARNING           COLOR_RGB2RGBA(COLOR_RGB_WARNING)
#define COLOR_BACKGROUND        COLOR_RGB2RGBA(COLOR_RGB_BACKGROUND)

#define FONT_NORMAL_SIZE        FONT_BODY_SIZE
#define FONT_NORNAL_PATH        FONT_BODY_PATH

///////////////////////////////////////////////////////////////////////////////////////////////////


///--------------------------------------- END USER CODE ---------------------------------------///

#ifdef __cplusplus
}
#endif  /// __cplusplus

#endif  /// __CONFIG_H__
