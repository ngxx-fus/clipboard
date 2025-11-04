/*
 * cbFont.c
 * Font management for SDL2/SDL_ttf, with automatic style/size/color sync.
 *
 * Author: HuuNam (2025)
 * License: MIT
 */

#include "fontUtil.h"
#include "../helper/loop.h"

/* -------------------------
 * Global font instances
 * ------------------------- */
// cbFont_t fontTitle      = {.ttf = NULL, .color.abgr = 0xFFFFFFFF, .style = STYLE_TITLE,    .size = FONT_TITLE_SIZE,    .path = FONT_TITLE_PATH};
// cbFont_t fontBody       = {.ttf = NULL, .color.abgr = 0xFFFFFFFF, .style = STYLE_BODY,     .size = FONT_BODY_SIZE,     .path = FONT_BODY_PATH};
// cbFont_t fontHeading0   = {.ttf = NULL, .color.abgr = 0xFFFFFFFF, .style = STYLE_HEADING0, .size = FONT_HEADING0_SIZE, .path = FONT_HEADING0_PATH};
// cbFont_t fontHeading1   = {.ttf = NULL, .color.abgr = 0xFFFFFFFF, .style = STYLE_HEADING1, .size = FONT_HEADING1_SIZE, .path = FONT_HEADING1_PATH};
// cbFont_t fontHeading2   = {.ttf = NULL, .color.abgr = 0xFFFFFFFF, .style = STYLE_HEADING2, .size = FONT_HEADING2_SIZE, .path = FONT_HEADING2_PATH};
// cbFont_t fontHeading3   = {.ttf = NULL, .color.abgr = 0xFFFFFFFF, .style = STYLE_HEADING3, .size = FONT_HEADING3_SIZE, .path = FONT_HEADING3_PATH};
// cbFont_t fontError      = {.ttf = NULL, .color.abgr = 0xFFFF0000, .style = STYLE_ERROR,    .size = FONT_ERROR_SIZE,    .path = FONT_ERROR_PATH};
// cbFont_t fontWarning    = {.ttf = NULL, .color.abgr = 0xFFFFFF00, .style = STYLE_WARNING,  .size = FONT_WARNING_SIZE,  .path = FONT_WARNING_PATH};

cbSysFont_t  systemFont = {
    .title =    {.ttf = NULL, .color.abgr = COLOR_TITLE, .style = STYLE_TITLE,    .size = FONT_TITLE_SIZE,    .path = FONT_TITLE_PATH},
    .body  =    {.ttf = NULL, .color.abgr = COLOR_BODY, .style = STYLE_BODY,     .size = FONT_BODY_SIZE,     .path = FONT_BODY_PATH},
    .heading0 = {.ttf = NULL, .color.abgr = COLOR_HEADING0, .style = STYLE_HEADING0, .size = FONT_HEADING0_SIZE, .path = FONT_HEADING0_PATH},
    .heading1 = {.ttf = NULL, .color.abgr = COLOR_HEADING1, .style = STYLE_HEADING1, .size = FONT_HEADING1_SIZE, .path = FONT_HEADING1_PATH},
    .heading2 = {.ttf = NULL, .color.abgr = COLOR_HEADING2, .style = STYLE_HEADING2, .size = FONT_HEADING2_SIZE, .path = FONT_HEADING2_PATH},
    .heading3 = {.ttf = NULL, .color.abgr = COLOR_HEADING3, .style = STYLE_HEADING3, .size = FONT_HEADING3_SIZE, .path = FONT_HEADING3_PATH},
    .error =    {.ttf = NULL, .color.abgr = COLOR_ERROR, .style = STYLE_ERROR,    .size = FONT_ERROR_SIZE,    .path = FONT_ERROR_PATH},
    .warning =  {.ttf = NULL, .color.abgr = COLOR_WARNING, .style = STYLE_WARNING,  .size = FONT_WARNING_SIZE,  .path = FONT_WARNING_PATH}
};

/* -------------------------
 * Internal helpers
 * ------------------------- */

/// @brief Re-apply saved font attributes (style, size, etc.) to the TTF font.
void __cbSyncFontAttributes(cbFont_t *cbFont)
{
    if (!cbFont || !cbFont->ttf) return;

    // Reapply style
    TTF_SetFontStyle(cbFont->ttf, (int)cbFont->style);

    // SDL_ttf không có hàm set size trực tiếp → phải reload nếu khác size
    int currentHeight = TTF_FontHeight(cbFont->ttf);
    if (currentHeight != (int)cbFont->size) {
        TTF_CloseFont(cbFont->ttf);
        cbFont->ttf = TTF_OpenFont(cbFont->path, (int)cbFont->size);
        if (cbFont->ttf) {
            TTF_SetFontStyle(cbFont->ttf, (int)cbFont->style);
        } else {
            __err("[__cbSyncFontAttributes] reload failed: %s (%s)\n", cbFont->path, TTF_GetError());
        }
    }
}

/* -------------------------
 * Public API
 * ------------------------- */

/// @brief Reload (self-load) font from its own stored attributes.
/// Useful after manually changing cbFont->path / size / style / color.
/// @return OKE on success, ERR_* otherwise.
def cbSelfLoad(cbFont_t *cbFont)
{
    __entry1("cbSelfLoad(%p)", cbFont);

    if (!cbFont) {
        __exit1("cbSelfLoad() : ERR_NULL");
        return ERR_NULL;
    }

    if (strlen(cbFont->path) == 0) {
        __err("[cbSelfLoad] Empty font path");
        __exit1("cbSelfLoad() : ERR_PATH_INVALID");
        return ERR_PATH_INVALID;
    }

    if (cbFont->size == 0) {
        __err("[cbSelfLoad] Invalid font size=0");
        __exit1("cbSelfLoad() : ERR_INVALID_ARG");
        return ERR_INVALID_ARG;
    }

    // Init SDL_ttf if needed
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            __err("[TTF] Init failed: %s", TTF_GetError());
            __exit1("cbSelfLoad() : ERR_SDL_TTF_FAILED");
            return ERR_SDL_TTF_FAILED;
        }
    }

    // Close previous font if needed
    if (cbFont->ttf) {
        __log1("[cbSelfLoad] Closing previous font: %s", cbFont->path);
        TTF_CloseFont(cbFont->ttf);
        cbFont->ttf = NULL;
    }

    // Open font again
    cbFont->ttf = TTF_OpenFont(cbFont->path, (int)cbFont->size);
    if (!cbFont->ttf) {
        __err("[cbSelfLoad] Reload failed: %s (%s)", cbFont->path, TTF_GetError());
        __exit1("cbSelfLoad() : ERR_FILE_NOT_FOUND");
        return ERR_FILE_NOT_FOUND;
    }

    // Apply saved style
    TTF_SetFontStyle(cbFont->ttf, (int)cbFont->style);

    __exit1("cbSelfLoad() : OKE");
    return OKE;
}


/// @brief Load a font, apply defaults, and save attributes to struct and TTF_Font.
def cbLoadFont(cbFont_t *cbFont,fontFlag_t defaultFontStyle,cbColor_t defaultColor, size_t fontSize, const char *fontPath)
{
    __entry("cbLoadFont(%p, 0x%x, 0x%x, %ld, %s)", cbFont, defaultFontStyle, defaultColor.abgr, fontSize, fontPath);

    if (!cbFont) {
        __exit("cbLoadFont() : ERR_NULL");
        return ERR_NULL;
    }
    if (!fontPath || strlen(fontPath) == 0) {
        __exit("cbLoadFont() : ERR_PATH_INVALID");
        return ERR_PATH_INVALID;
    }
    if (fontSize == 0) {
        __exit("cbLoadFont() : ERR_INVALID_ARG");
        return ERR_INVALID_ARG;
    }

    // Init SDL_ttf if needed
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            __err("[TTF] Init failed: %s\n", TTF_GetError());
            __exit("cbLoadFont() : ERR_SDL_TTF_FAILED");
            return ERR_SDL_TTF_FAILED;
        }
    }

    // Clear struct
    memset(cbFont, 0, sizeof(cbFont_t));

    // Copy path
    strncpy(cbFont->path, fontPath, sizeof(cbFont->path) - 1);
    cbFont->path[sizeof(cbFont->path) - 1] = '\0';

    // Load font
    cbFont->ttf = TTF_OpenFont(cbFont->path, (int)fontSize);
    if (!cbFont->ttf) {
        __err("[cbLoadFont] Load failed: %s (%s)\n", cbFont->path, TTF_GetError());
        __exit("cbLoadFont() : ERR_FILE_NOT_FOUND");
        return ERR_FILE_NOT_FOUND;
    }

    // Save & apply style, color, size
    cbFont->style = defaultFontStyle;
    cbFont->size  = fontSize;
    cbFont->color = defaultColor;

    TTF_SetFontStyle(cbFont->ttf, (int)defaultFontStyle);

    __exit("cbLoadFont() : OKE");
    return OKE;
}

/// @brief Set font size (reloads TTF_Font if changed)
def cbSetFontSize(cbFont_t *cbFont, size_t newSize)
{
    __entry("cbSetFontSize(%p, %zu)", cbFont, newSize);

    if (!cbFont || !cbFont->path[0]) {
        __exit("cbSetFontSize() : ERR_INVALID_ARG");
        return ERR_INVALID_ARG;
    }
    if (newSize == 0) {
        __exit("cbSetFontSize() : ERR_INVALID_ARG");
        return ERR_INVALID_ARG;
    }

    cbFont->size = newSize;

    // Reload
    if (cbFont->ttf) TTF_CloseFont(cbFont->ttf);
    cbFont->ttf = TTF_OpenFont(cbFont->path, (int)newSize);
    if (!cbFont->ttf) {
        __err("[cbSetFontSize] reload failed: %s (%s)\n", cbFont->path, TTF_GetError());
        __exit("cbSetFontSize() : ERR_SDL_TTF_FAILED");
        return ERR_SDL_TTF_FAILED;
    }

    TTF_SetFontStyle(cbFont->ttf, (int)cbFont->style);
    __exit("cbSetFontSize() : OKE");
    return OKE;
}

/// @brief Set font style (bold, italic, underline, etc.)
def cbSetFontStyle(cbFont_t *cbFont, fontFlag_t newStyle)
{
    __entry("cbSetFontStyle(%p, 0x%x)", cbFont, newStyle);

    if (!cbFont || !cbFont->ttf) {
        __exit("cbSetFontStyle() : ERR_INVALID_ARG");
        return ERR_INVALID_ARG;
    }

    cbFont->style = newStyle;
    TTF_SetFontStyle(cbFont->ttf, (int)newStyle);

    __exit("cbSetFontStyle() : OKE");
    return OKE;
}

/// @brief Set default font color (only stored in struct)
def cbSetFontColor(cbFont_t *cbFont, cbColor_t color)
{
    __entry("cbSetFontColor(%p, 0x%x)", cbFont, color.abgr);

    if (!cbFont) {
        __exit("cbSetFontColor() : ERR_INVALID_ARG");
        return ERR_INVALID_ARG;
    }

    cbFont->color = color;

    __exit("cbSetFontColor() : OKE");
    return OKE;
}

/// @brief Delete and free a font safely
def cbDeleteFont(cbFont_t *cbFont)
{
    __entry1("cbDeleteFont(%p)", cbFont);

    if (!cbFont) {
        __exit1("cbDeleteFont() : ERR_NULL");
        return ERR_NULL;
    }

    if (cbFont->ttf) {
        TTF_CloseFont(cbFont->ttf);
        cbFont->ttf = NULL;
    }

    memset(cbFont->path, 0, sizeof(cbFont->path));
    cbFont->size  = 0;
    cbFont->style = 0;
    cbFont->color.abgr = 0;

    __exit1("cbDeleteFont() : OKE");
    return OKE;
}


void syncSystemColor(cbSysFont_t * systemFont){
    REP(i, 0, sizeof(cbSysFont_t) / sizeof(cbFont_t)){
        systemFont->arr[i].color.abgr = systemColor.arr[i].abgr; 
    }
}