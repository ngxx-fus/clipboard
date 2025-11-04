#ifndef __WINDOW_CONTEXT_H__
#define __WINDOW_CONTEXT_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "string.h"
#include "../helper/general.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TITLE_SIZE      128
typedef int32_t             dim_t;

typedef struct windowContext_t{
    SDL_Window          *window;
    SDL_Renderer        *renderer;
    SDL_Texture         *texture;
    dim_t                w;
    dim_t                h;
    char                title[MAX_TITLE_SIZE];
    uint32_t            id;
}windowContext_t;

/**
 * @brief Create an SDL window inside the given window context.
 *
 * This function initializes the SDL_Window pointer using parameters
 * already stored in the window context (title, width, height).
 *
 * @param[in,out] wdct Pointer to a valid window context.
 *
 * @return OKE on success, ERR_INVALID_ARG if wdct is NULL,
 *         or ERR if SDL_CreateWindow fails.
 */
def __wdctCreateWindow(windowContext_t * wdct);


/**
 * @brief Destroy the SDL window inside the given window context.
 *
 * This function calls SDL_DestroyWindow if the window is valid.
 *
 * @param[in,out] wdct Pointer to a valid window context.
 *
 * @return OKE on success, ERR_INVALID_ARG if wdct or its window is NULL.
 */
def __wdctDeleteWindow(windowContext_t * wdct);


/**
 * @brief Create an SDL renderer for the given window context.
 *
 * This function creates a hardware-accelerated SDL_Renderer for
 * the SDL_Window in the context. The window must already be created.
 *
 * @param[in,out] wdct Pointer to a valid window context.
 *
 * @return OKE on success, ERR_INVALID_ARG if wdct or window is NULL,
 *         or ERR if SDL_CreateRenderer fails.
 */
def __wdctCreateRenderer(windowContext_t * wdct);


/**
 * @brief Destroy the SDL renderer in the given window context.
 *
 * This function calls SDL_DestroyRenderer if the renderer is valid.
 *
 * @param[in,out] wdct Pointer to a valid window context.
 *
 * @return OKE on success, ERR_INVALID_ARG if wdct or renderer is NULL.
 */
def __wdctDeleteRenderer(windowContext_t * wdct);


/**
 * @brief Create an SDL texture for the given window context.
 *
 * The texture is created with format SDL_PIXELFORMAT_RGBA8888
 * and access mode SDL_TEXTUREACCESS_STREAMING, sized to match the
 * width and height stored in the context.
 *
 * @param[in,out] wdct Pointer to a valid window context with a valid renderer.
 *
 * @return OKE on success, ERR_INVALID_ARG if wdct or renderer is NULL,
 *         or ERR if SDL_CreateTexture fails.
 */
def __wdctCreateTexture(windowContext_t * wdct);


/**
 * @brief Destroy the SDL texture in the given window context.
 *
 * This function calls SDL_DestroyTexture if the texture is valid.
 *
 * @param[in,out] wdct Pointer to a valid window context.
 *
 * @return OKE on success, ERR_INVALID_ARG if wdct or texture is NULL.
 */
def __wdctDeleteTexture(windowContext_t * wdct);


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
 * @return OKE on success, or an error code on failure.
 */
def createWindowContext(
    windowContext_t **wdct, dim_t w, dim_t h, const char *title
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
def destroyWindowContext(windowContext_t **wdct);

#ifdef __cplusplus
} // extern "C"
#endif

#endif