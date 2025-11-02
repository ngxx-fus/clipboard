#ifndef __HELPER_COLOR_H__
#define __HELPER_COLOR_H__


#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/// @brief Represents a color in multiple interchangeable formats.
typedef union cbColor_t {
    /// @brief SDL color structure (r, g, b, a as 8-bit values).
    SDL_Color SDL;

    /// @brief Packed 32-bit RGBA value (0xRRGGBBAA). 
    /// @note Endianness may affect byte order.
    uint32_t abgr;

    /// @brief Separate 8-bit color channels (r, g, b, a).
    struct {
        uint8_t r : 8; ///< Red component (0–255)
        uint8_t g : 8; ///< Green component (0–255)
        uint8_t b : 8; ///< Blue component (0–255)
        uint8_t a : 8; ///< Alpha component (0–255)
    };

    /// @brief Raw byte access to color data (4 bytes: RGBA order).
    uint8_t byteArr[4];
} cbColor_t;

typedef union systemColor_t{
    struct{
        cbColor_t title;        /// 0
        cbColor_t body;         /// 1
        cbColor_t heading0;     /// 2
        cbColor_t heading1;     /// 3
        cbColor_t heading2;     /// 4
        cbColor_t heading3;     /// 5
        cbColor_t error;        /// 6
        cbColor_t warning;      /// 7
        cbColor_t background;   /// 8
        cbColor_t footer;       /// 9
        cbColor_t header;       /// 10
    };
    cbColor_t arr[11];
} systemColor_t;

extern systemColor_t systemColor;

void systemColorCorrectByteOrder(systemColor_t * sysColor);
void cbColorCorrectByteOrder(cbColor_t *c);
void cbColorSetRGB(cbColor_t *c, uint32_t rgb);
void cbColorSetRGBA(cbColor_t *c, uint32_t rgb);

#define splitRGBA(cbColor) cbColor.r, cbColor.g, cbColor.b, cbColor.a

/// COLORS ////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Combine RGB values into a 32-bit RGBA color (alpha defaults to 255).
 * @param R Red channel (0-255).
 * @param G Green channel (0-255).
 * @param B Blue channel (0-255).
 * @return A 32-bit RGBA value.
 * @note Assumes simColor_t is defined (e.g., uint32_t in global.h).
 */
#define __combiRGB(R, G, B) ((((simColor_t)R)<<24) | (((simColor_t)G)<<16) | (((simColor_t)B)<<8) | ((simColor_t)0xFF))

/**
 * @brief Combine RGBA values into a 32-bit RGBA color.
 * @param R Red channel (0-255).
 * @param G Green channel (0-255).
 * @param B Blue channel (0-255).
 * @param A Alpha channel (0-255).
 * @return A 32-bit RGBA value.
 * @note Assumes simColor_t is defined (e.g., uint32_t in global.h).
 */
#define __combiRGBA(R, G, B, A) ((((simColor_t)R)<<24) | (((simColor_t)G)<<16) | (((simColor_t)B)<<8) | ((simColor_t)A))

/**
 * @brief Extract the red channel from a 32-bit RGBA color.
 * @param RGBA The 32-bit RGBA color value.
 * @return The 8-bit red channel value (0-255).
 */
#define __getRFromRGBA(RGBA)    (((simColor_t)(RGBA) >> 24) & 0xFF)

/**
 * @brief Extract the green channel from a 32-bit RGBA color.
 * @param RGBA The 32-bit RGBA color value.
 * @return The 8-bit green channel value (0-255).
 */
#define __getGFromRGBA(RGBA)    (((simColor_t)(RGBA) >> 16) & 0xFF)

/**
 * @brief Extract the blue channel from a 32-bit RGBA color.
 * @param RGBA The 32-bit RGBA color value.
 * @return The 8-bit blue channel value (0-255).
 */
#define __getBFromRGBA(RGBA)    (((simColor_t)(RGBA) >>  8) & 0xFF)

/**
 * @brief Extract the alpha channel from a 32-bit RGBA color.
 * @param RGBA The 32-bit RGBA color value.
 * @return The 8-bit alpha channel value (0-255).
 */
#define __getAFromRGBA(RGBA)    (((simColor_t)(RGBA) >>  0) & 0xFF)



#endif /// __HELPER_COLOR_H__
