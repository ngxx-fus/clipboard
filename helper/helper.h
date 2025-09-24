#ifndef __HELPER_H__
#define __HELPER_H__

/**
 * @file helper.h
 * @brief Utility macros for common operations, including loops, delays, color manipulation,
 *        and bit mask operations. Includes necessary headers for SDL2, time, and threading.
 */

#ifdef LOG_HEADER_INCLUDE
#pragma message("INCLUDE: global.h")
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

/**
 * @brief Loop macro for iterating over a range with a specified type.
 * @param type The data type of the loop variable.
 * @param i The loop variable.
 * @param a The starting value (inclusive).
 * @param b The ending value (exclusive).
 * @example REPT(int, i, 0, 5) iterates i from 0 to 4.
 */
#ifndef REPTT
    #define REPTT(type, i, a, b) for(type i = (a); (i) < (b); ++(i))
#endif

/**
 * @brief Reverse loop macro for iterating downward with a specified type.
 * @param type The data type of the loop variable.
 * @param i The loop variable.
 * @param a The starting value (inclusive).
 * @param b The ending value (exclusive).
 * @example REVT(int, i, 5, 0) iterates i from 5 to 1.
 */
#ifndef REVT
    #define REVT(type, i, a, b) for(type i = (a); (i) > (b); --(i))
#endif

/**
 * @brief Loop macro for iterating over a range using simSize_t.
 * @param i The loop variable.
 * @param a The starting value (inclusive).
 * @param b The ending value (exclusive).
 * @note Assumes simSize_t is defined elsewhere (e.g., in global.h).
 */
#ifndef REP
    #define REP(i, a, b) for(simSize_t i = (a); (i) < (b); ++(i))
#endif

/**
 * @brief Reverse loop macro for iterating downward using simSize_t.
 * @param i The loop variable.
 * @param a The starting value (inclusive).
 * @param b The ending value (exclusive).
 * @note Assumes simSize_t is defined elsewhere (e.g., in global.h).
 */
#ifndef REV
    #define REV(i, a, b) for(simSize_t i = (a); (i) > (b); --(i))
#endif

/// DELAY /////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Convert microseconds to nanoseconds.
 * @param i The number of microseconds.
 * @return The equivalent number of nanoseconds.
 */
#define __USEC(i) ((i) * 1000ULL)

/**
 * @brief Convert milliseconds to nanoseconds.
 * @param i The number of milliseconds.
 * @return The equivalent number of nanoseconds.
 */
#define __MSEC(i) ((i) * 1000000ULL)

/**
 * @brief Convert seconds to nanoseconds.
 * @param i The number of seconds.
 * @return The equivalent number of nanoseconds.
 */
#define __SEC(i)  ((i) * 1000000000ULL)

/**
 * @brief Sleep for a specified number of milliseconds.
 * @param ms The number of milliseconds to sleep (must be non-negative integer).
 * @note Uses nanosleep for precise timing and handles EINTR (interrupted system call).
 */
#define __sleep_ms(ms) do {                           \
    if ((ms) < 0) break;                          \
    struct timespec ts;                           \
    ts.tv_sec  = (ms) / 1000;                     \
    ts.tv_nsec = ((ms) % 1000) * 1000000L;        \
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR); \
} while(0)

/**
 * @brief Sleep for a specified number of nanoseconds.
 * @param ns The number of nanoseconds to sleep (must be non-negative integer).
 * @note Uses nanosleep for precise timing.
 */
#define __sleep_ns(ns) do {                         \
    struct timespec ts;                                 \
    ts.tv_sec  = (time_t)((ns) / 1000000000UL);         \
    ts.tv_nsec = (long)((ns) % 1000000000UL);           \
    nanosleep(&ts, NULL);                               \
} while(0)

/**
 * @brief Sleep for a specified number of microseconds.
 * @param us The number of microseconds to sleep (must be non-negative integer).
 * @note Uses nanosleep for precise timing.
 */
#define __sleep_us(us) do {                   \
    struct timespec ts;                           \
    ts.tv_sec  = (us) / 1000000;                  \
    ts.tv_nsec = ((us) % 1000000) * 1000L;        \
    nanosleep(&ts, NULL);                         \
} while(0)

/**
 * @brief Sleep for a specified number of seconds.
 * @param sec The number of seconds to sleep (must be non-negative integer).
 * @note Uses nanosleep for precise timing.
 */
#define __sleep(sec) do {                     \
    struct timespec ts;                           \
    ts.tv_sec  = sec;                             \
    ts.tv_nsec = 0;                               \
    nanosleep(&ts, NULL);                         \
} while(0)

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

/// OTHERS ////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Return the maximum of two values.
 * @param a First value.
 * @param b Second value.
 * @return The larger of a or b.
 */
#define __max(a, b) ((a) > (b) ? (a) : (b))

/**
 * @brief Return the minimum of two values.
 * @param a First value.
 * @param b Second value.
 * @return The smaller of a or b.
 */
#define __min(a, b) ((a) < (b) ? (a) : (b))

/**
 * @brief Return the absolute value of a number.
 * @param x The input value.
 * @return The absolute value of x.
 */
#define __abs(x) ((x) >= 0 ? (x) : -(x))

/**
 * @brief Check if a pointer is NULL.
 * @param ptr The pointer to check.
 * @return Non-zero if ptr is NULL, zero otherwise.
 */
#define __is_null(ptr) ((ptr) == NULL)

/**
 * @brief Check if a pointer is not NULL.
 * @param ptr The pointer to check.
 * @return Non-zero if ptr is not NULL, zero otherwise.
 */
#define __is_not_null(ptr) ((ptr) != NULL)

#include <stdint.h>
#include <limits.h>  // để có UINT8_MAX, UINT16_MAX, ...

#include <stdint.h>
#include <limits.h>  // For UINT*_MAX

/**
 * @brief Create a mask with only bit i set (for uint8_t).
 * @param i Bit position (0–7).
 * @return A uint8_t value with only bit i set to 1.
 * @example __mask8(3) -> 0b00001000
 */
#define __mask8(i)   ((uint8_t)(1U << (i)))

/**
 * @brief Create a mask with only bit i set (for uint16_t).
 * @param i Bit position (0–15).
 * @return A uint16_t value with only bit i set to 1.
 * @example __mask16(3) -> 0b0000000000001000
 */
#define __mask16(i)  ((uint16_t)(1U << (i)))

/**
 * @brief Create a mask with only bit i set (for uint32_t).
 * @param i Bit position (0–31).
 * @return A uint32_t value with only bit i set to 1.
 * @example __mask32(3) -> 0b000...1000
 */
#define __mask32(i)  ((uint32_t)(1UL << (i)))

/**
 * @brief Create a mask with only bit i set (for uint64_t).
 * @param i Bit position (0–63).
 * @return A uint64_t value with only bit i set to 1.
 * @example __mask64(3) -> 0b000...1000
 */
#define __mask64(i)  ((uint64_t)(1ULL << (i)))


/**
 * @brief Create an inverted mask with all bits =1 except bit i =0 (for uint8_t).
 * @param i Bit position (0–7).
 * @return A uint8_t value with all bits =1 except bit i cleared.
 * @example __inv_mask8(3) -> 0b11110111
 */
#define __inv_mask8(i)   ((uint8_t)(~(1U << (i))))

/**
 * @brief Create an inverted mask with all bits =1 except bit i =0 (for uint16_t).
 * @param i Bit position (0–15).
 * @return A uint16_t value with all bits =1 except bit i cleared.
 * @example __inv_mask16(3) -> 0b1111111111110111
 */
#define __inv_mask16(i)  ((uint16_t)(~(1U << (i))))

/**
 * @brief Create an inverted mask with all bits =1 except bit i =0 (for uint32_t).
 * @param i Bit position (0–31).
 * @return A uint32_t value with all bits =1 except bit i cleared.
 * @example __inv_mask32(3) -> 0b...11110111
 */
#define __inv_mask32(i)  ((uint32_t)(~(1UL << (i))))

/**
 * @brief Create an inverted mask with all bits =1 except bit i =0 (for uint64_t).
 * @param i Bit position (0–63).
 * @return A uint64_t value with all bits =1 except bit i cleared.
 * @example __inv_mask64(3) -> 0xFFFF...FFF7
 */
#define __inv_mask64(i)  ((uint64_t)(~(1ULL << (i))))

/**
 * @brief Check if a value is zero.
 * @param x The value to check.
 * @return Non-zero if x is zero, zero otherwise.
 */
#define __is_zero(x) ((x) == 0)

/**
 * @brief Check if a value is positive.
 * @param x The value to check.
 * @return Non-zero if x is greater than zero, zero otherwise.
 */
#define __is_positive(x) ((x) > 0)

/**
 * @brief Check if a value is negative.
 * @param x The value to check.
 * @return Non-zero if x is less than zero, zero otherwise.
 */
#define __is_negative(x) ((x) < 0)

/**
 * @brief Check if a value is not zero.
 * @param x The value to check.
 * @return Non-zero if x is not zero, zero otherwise.
 */
#define __isnot_zero(x) ((x) != 0)

/**
 * @brief Check if a value is not positive.
 * @param x The value to check.
 * @return Non-zero if x is less than or equal to zero, zero otherwise.
 */
#define __isnot_positive(x) ((x) <= 0)

/**
 * @brief Check if a value is not negative.
 * @param x The value to check.
 * @return Non-zero if x is greater than or equal to zero, zero otherwise.
 */
#define __isnot_negative(x) ((x) >= 0)

#endif