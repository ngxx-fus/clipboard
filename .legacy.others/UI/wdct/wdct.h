#ifndef __WINDOW_CONTEXT_H__
#define __WINDOW_CONTEXT_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "string.h"
#include "../log/log.h"
#include "../../helper/helper.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TITLE_SIZE      128
typedef int8_t              status_t;
typedef int32_t             xy_t;

enum WDCT_STATUS{
    ERROR_INVALID_PARAMS = -2,
    ERROR_UNKNOWN = -1,
    STATUS_OK = 0x0,
};

typedef struct windowContext_t{
    SDL_Window          *window;
    SDL_Renderer        *renderer;
    SDL_Texture         *texture;
    TTF_Font            *font;
    xy_t                w;
    xy_t                h;
    char                title[MAX_TITLE_SIZE];
}windowContext_t;

/**
 * @brief Create an SDL window inside the given window context.
 *
 * This function initializes the SDL_Window pointer using parameters
 * already stored in the window context (title, width, height).
 *
 * @param[in,out] wdct Pointer to a valid window context.
 *
 * @return STATUS_OK on success, ERROR_INVALID_PARAMS if wdct is NULL,
 *         or ERROR_UNKNOWN if SDL_CreateWindow fails.
 */
status_t wdctCreateWindow(windowContext_t * wdct);


/**
 * @brief Destroy the SDL window inside the given window context.
 *
 * This function calls SDL_DestroyWindow if the window is valid.
 *
 * @param[in,out] wdct Pointer to a valid window context.
 *
 * @return STATUS_OK on success, ERROR_INVALID_PARAMS if wdct or its window is NULL.
 */
status_t wdctDeleteWindow(windowContext_t * wdct);


/**
 * @brief Create an SDL renderer for the given window context.
 *
 * This function creates a hardware-accelerated SDL_Renderer for
 * the SDL_Window in the context. The window must already be created.
 *
 * @param[in,out] wdct Pointer to a valid window context.
 *
 * @return STATUS_OK on success, ERROR_INVALID_PARAMS if wdct or window is NULL,
 *         or ERROR_UNKNOWN if SDL_CreateRenderer fails.
 */
status_t wdctCreateRenderer(windowContext_t * wdct);


/**
 * @brief Destroy the SDL renderer in the given window context.
 *
 * This function calls SDL_DestroyRenderer if the renderer is valid.
 *
 * @param[in,out] wdct Pointer to a valid window context.
 *
 * @return STATUS_OK on success, ERROR_INVALID_PARAMS if wdct or renderer is NULL.
 */
status_t wdctDeleteRenderer(windowContext_t * wdct);


/**
 * @brief Create an SDL texture for the given window context.
 *
 * The texture is created with format SDL_PIXELFORMAT_RGBA8888
 * and access mode SDL_TEXTUREACCESS_STREAMING, sized to match the
 * width and height stored in the context.
 *
 * @param[in,out] wdct Pointer to a valid window context with a valid renderer.
 *
 * @return STATUS_OK on success, ERROR_INVALID_PARAMS if wdct or renderer is NULL,
 *         or ERROR_UNKNOWN if SDL_CreateTexture fails.
 */
status_t wdctCreateTexture(windowContext_t * wdct);


/**
 * @brief Destroy the SDL texture in the given window context.
 *
 * This function calls SDL_DestroyTexture if the texture is valid.
 *
 * @param[in,out] wdct Pointer to a valid window context.
 *
 * @return STATUS_OK on success, ERROR_INVALID_PARAMS if wdct or texture is NULL.
 */
status_t wdctDeleteTexture(windowContext_t * wdct);


/**
 * @brief Open a font using SDL_ttf and store it in the window context.
 *
 * The font will be loaded from the provided path with the given size.
 *
 * @param[in,out] wdct     Pointer to a valid window context.
 * @param[in]     fontPath Path to the TTF font file.
 * @param[in]     fontSize Font size (in points).
 *
 * @return STATUS_OK on success, ERROR_INVALID_PARAMS if wdct is NULL,
 *         or ERROR_UNKNOWN if TTF_OpenFont fails.
 */
status_t wdctOpenFont(windowContext_t * wdct, const char *fontPath, uint8_t fontSize);


/**
 * @brief Close and free the font stored in the window context.
 *
 * This function calls TTF_CloseFont if the font is valid.
 *
 * @param[in,out] wdct Pointer to a valid window context.
 *
 * @return STATUS_OK on success, ERROR_INVALID_PARAMS if wdct or font is NULL.
 */
status_t wdctCloseFont(windowContext_t * wdct);


/**
 * @brief Create and initialize a window context with SDL2 and SDL_ttf.
 *
 * This function allocates memory for a new window context and initializes:
 * - SDL_Window
 * - SDL_Renderer
 * - SDL_Texture
 * - TTF_Font (optional)
 *
 * On failure, all allocated resources are released and the pointer is set to NULL.
 *
 * @param[in,out] wdct      Pointer to a window context pointer. Will be allocated inside.
 * @param[in]     w         Width of the window.
 * @param[in]     h         Height of the window.
 * @param[in]     title     Title of the window.
 * @param[in]     fontPath  Path to the font file (can be NULL).
 * @param[in]     fontSize  Font size (ignored if fontPath is NULL).
 *
 * @return STATUS_OK on success, or an error code on failure.
 */
status_t createWindowContext(
    windowContext_t **wdct, xy_t w, xy_t h, const char *title, 
    const char *fontPath, uint8_t fontSize
);

/**
 * @brief Destroy and free a window context.
 *
 * This function releases all resources allocated inside a window context, including:
 * - TTF_Font
 * - SDL_Texture
 * - SDL_Renderer
 * - SDL_Window
 *
 * The memory of the context struct is freed and the pointer is set to NULL.
 *
 * @param[in,out] wdct Pointer to a window context pointer.
 */
status_t destroyWindowContext(windowContext_t **wdct);

#ifdef __cplusplus
} // extern "C"
#endif

#endif