#ifndef __HELPER_CONDITION_H__
#define __HELPER_CONDITION_H__

/**
 * @file helper.h
 * @brief Utility macros for common operations, including loops, delays, color manipulation,
 *        and bit mask operations. Includes necessary headers for SDL2, time, and threading.
 */

#ifdef LOG_HEADER_INCLUDE
#pragma message("INCLUDE: global.h")
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

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
#define __isnot_null(ptr) ((ptr) != NULL)

#include <stdint.h>
#include <limits.h>  // để có UINT8_MAX, UINT16_MAX, ...


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
