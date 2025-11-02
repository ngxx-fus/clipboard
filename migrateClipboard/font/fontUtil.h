#ifndef __FONT_UTIL_H__
#define __FONT_UTIL_H__

#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "../config/config.h"
#include "../helper/color.h"
#include "../helper/return.h"
#include "../log/log.h"

typedef uint32_t fontFlag_t;

enum CB_FONT_STYLE {
    CBF_STYLE_NORMAL        = TTF_STYLE_NORMAL,
    CBF_STYLE_BOLD          = TTF_STYLE_BOLD,
    CBF_STYLE_ITALIC        = TTF_STYLE_ITALIC,
    CBF_STYLE_UNDERLINE     = TTF_STYLE_UNDERLINE,
    CBF_STYLE_STRIKETHROUGH = TTF_STYLE_STRIKETHROUGH,
    CBF_STYLE_NUM
};

typedef struct cbFont_t {
    char path[256];     ///< Font file path
    size_t size;        ///< Font pixel size
    fontFlag_t style;   ///< Bitmask for font style (TTF_STYLE_BOLD, etc.)
    cbColor_t color;    ///< Default font color
    TTF_Font *ttf;      ///< Pointer to the loaded TTF font
} cbFont_t;

typedef union cbSysFont_t{
    struct {
        cbFont_t title;
        cbFont_t body;
        cbFont_t heading0;
        cbFont_t heading1;
        cbFont_t heading2;
        cbFont_t heading3;
        cbFont_t error;
        cbFont_t warning;
    };
    cbFont_t arr[8];
} cbSysFont_t;

/// @brief Reload (self-load) font from its own stored attributes.
/// Useful after manually changing cbFont->path / size / style / color.
/// @return OKE on success, ERR_* otherwise.
def cbSelfLoad(cbFont_t *cbFont);

/**
 * @brief Load a font, apply defaults, and save attributes to struct and TTF_Font.
 * @param cbFont Pointer to cbFont_t to initialize.
 * @param defaultFontStyle Default TTF style flags (e.g., TTF_STYLE_BOLD).
 * @param defaultColor Default font color.
 * @param fontSize Font size in pixels.
 * @param fontPath Path to the .ttf font file.
 * @return OKE on success or error code on failure.
 */
def cbLoadFont(cbFont_t *cbFont,
               fontFlag_t defaultFontStyle,
               cbColor_t defaultColor,
               size_t fontSize,
               const char *fontPath);

/**
 * @brief Set font size and reload TTF_Font if changed.
 * @param cbFont Pointer to loaded font.
 * @param newSize New font size in pixels.
 * @return OKE on success or error code on failure.
 */
def cbSetFontSize(cbFont_t *cbFont, size_t newSize);

/**
 * @brief Set font style (bold, italic, underline, etc.).
 * @param cbFont Pointer to loaded font.
 * @param newStyle New font style bitmask.
 * @return OKE on success or error code on failure.
 */
def cbSetFontStyle(cbFont_t *cbFont, fontFlag_t newStyle);

/**
 * @brief Set default font color (only stored in struct, not applied to TTF).
 * @param cbFont Pointer to loaded font.
 * @param color New color to assign.
 * @return OKE on success or error code on failure.
 */
def cbSetFontColor(cbFont_t *cbFont, cbColor_t color);

/**
 * @brief Delete and free a font safely.
 * @param cbFont Pointer to font to delete.
 * @return OKE on success or error code on failure.
 */
def cbDeleteFont(cbFont_t *cbFont);

/**
 * @brief Re-apply stored font attributes (size, style) to its TTF_Font.
 * @param cbFont Pointer to font to synchronize.
 */
void __cbSyncFontAttributes(cbFont_t *cbFont);

void syncSystemColor(cbSysFont_t * systemFont);


/// PUBLIC ////////////////////////////////////////////////////////////////////////////////////////

// extern cbFont_t fontTitle;
// extern cbFont_t fontBody;
// extern cbFont_t fontHeading1;
// extern cbFont_t fontHeading2;
// extern cbFont_t fontHeading3;
// extern cbFont_t fontError;
// extern cbFont_t fontWarning;

extern cbSysFont_t  systemFont;

#endif /// __FONT_UTIL_H__