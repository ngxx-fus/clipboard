#ifndef __HELPER_SLEEP_H__
#define __HELPER_SLEEP_H__

#include <time.h>
#include <errno.h>
#include <stdint.h>

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


#endif  /// __HELPER_SLEEP_H__
