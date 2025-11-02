#ifndef __FLAG_H__
#define __FLAG_H__

/*
 * flag.h
 * Cross-platform flag helpers and critical-section wrappers.
 *
 * This version targets POSIX (Linux / Voidlinux / SDL builds).
 * It uses pthread mutex and your __entryCriticalSection / __exitCriticalSection()
 * wrappers defined in log.h instead of FreeRTOS vPort* calls.
 *
 * Link with -lpthread when building.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include "../log/log.h"

#if defined(__unix__) || defined(__APPLE__) || defined(__linux__)
    #include <pthread.h>
    #define FLAG_POSIX 1
#else
    /* Fall back to single-thread/no-op */
    #define FLAG_POSIX 0
#endif

/* -------------------------
 * Bit-mask helpers
 * ------------------------- */
#define __mask8(i)          ((uint8_t)((uint8_t)1U << (i)))
#define __mask16(i)         ((uint16_t)((uint16_t)1U << (i)))
#define __mask32(i)         ((uint32_t)((uint32_t)1UL << (i)))
#define __mask64(i)         ((uint64_t)((uint64_t)1ULL << (i)))

#define __inv_mask8(i)      ((uint8_t)(~((uint8_t)1U << (i))))
#define __inv_mask16(i)     ((uint16_t)(~((uint16_t)1U << (i))))
#define __inv_mask32(i)     ((uint32_t)(~((uint32_t)1UL << (i))))
#define __inv_mask64(i)     ((uint64_t)(~((uint64_t)1ULL << (i))))

/* -------------------------
 * OR combinators for multiple bit positions
 * ------------------------- */
#define __mask_or1(f,a0) (f(a0))
#define __mask_or2(f,a0,a1) (f(a0)|f(a1))
#define __mask_or3(f,a0,a1,a2) (f(a0)|f(a1)|f(a2))
#define __mask_or4(f,a0,a1,a2,a3) (f(a0)|f(a1)|f(a2)|f(a3))
#define __mask_or5(f,a0,a1,a2,a3,a4) (f(a0)|f(a1)|f(a2)|f(a3)|f(a4))
#define __mask_or6(f,a0,a1,a2,a3,a4,a5) (f(a0)|f(a1)|f(a2)|f(a3)|f(a4)|f(a5))
#define __mask_or7(f,a0,a1,a2,a3,a4,a5,a6) (f(a0)|f(a1)|f(a2)|f(a3)|f(a4)|f(a5)|f(a6))
#define __mask_or8(f,a0,a1,a2,a3,a4,a5,a6,a7) (f(a0)|f(a1)|f(a2)|f(a3)|f(a4)|f(a5)|f(a6)|f(a7))
#define __mask_or9(f,a0,a1,a2,a3,a4,a5,a6,a7,a8) (f(a0)|f(a1)|f(a2)|f(a3)|f(a4)|f(a5)|f(a6)|f(a7)|f(a8))
#define __mask_or10(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9) (f(a0)|f(a1)|f(a2)|f(a3)|f(a4)|f(a5)|f(a6)|f(a7)|f(a8)|f(a9))
#define __mask_or11(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10) (f(a0)|f(a1)|f(a2)|f(a3)|f(a4)|f(a5)|f(a6)|f(a7)|f(a8)|f(a9)|f(a10))
#define __mask_or12(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11) (f(a0)|f(a1)|f(a2)|f(a3)|f(a4)|f(a5)|f(a6)|f(a7)|f(a8)|f(a9)|f(a10)|f(a11))
#define __mask_or13(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12) (f(a0)|f(a1)|f(a2)|f(a3)|f(a4)|f(a5)|f(a6)|f(a7)|f(a8)|f(a9)|f(a10)|f(a11)|f(a12))
#define __mask_or14(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13) (f(a0)|f(a1)|f(a2)|f(a3)|f(a4)|f(a5)|f(a6)|f(a7)|f(a8)|f(a9)|f(a10)|f(a11)|f(a12)|f(a13))
#define __mask_or15(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14) (f(a0)|f(a1)|f(a2)|f(a3)|f(a4)|f(a5)|f(a6)|f(a7)|f(a8)|f(a9)|f(a10)|f(a11)|f(a12)|f(a13)|f(a14))
#define __mask_or16(f,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15) (f(a0)|f(a1)|f(a2)|f(a3)|f(a4)|f(a5)|f(a6)|f(a7)|f(a8)|f(a9)|f(a10)|f(a11)|f(a12)|f(a13)|f(a14)|f(a15))

#define __mask_overload( \
    _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,NAME,...) NAME

#define __masks_generic(f, ...) \
    (__mask_overload(__VA_ARGS__, \
        __mask_or16,__mask_or15,__mask_or14,__mask_or13,__mask_or12,__mask_or11,\
        __mask_or10,__mask_or9,__mask_or8,__mask_or7,__mask_or6,__mask_or5,\
        __mask_or4,__mask_or3,__mask_or2,__mask_or1)(f,__VA_ARGS__))

#define __masks8(...)   __masks_generic(__mask8,__VA_ARGS__)
#define __masks16(...)  __masks_generic(__mask16,__VA_ARGS__)
#define __masks32(...)  __masks_generic(__mask32,__VA_ARGS__)
#define __masks64(...)  __masks_generic(__mask64,__VA_ARGS__)

/* -------------------------
 * Flag type and operations
 * ------------------------- */

/// 64-bit flag
typedef uint64_t flag_t;

#define __flagMask(i)             __mask64(i)
#define __flagInvMask(i)          __inv_mask64(i)
#define __setFlagBit(flag, i)     ((flag) |= __flagMask(i))
#define __clearFlagBit(flag, i)   ((flag) &= __flagInvMask(i))
#define __toggleFlagBit(flag, i)  ((flag) ^= __flagMask(i))
#define __hasFlagBitSet(flag, i)  (((flag) & __flagMask(i)) != 0U)
#define __hasFlagBitClr(flag, i)  (((flag) & __flagMask(i)) == 0U)

#define NONE_FLAG_SET             ((flag_t)0x00000000U)
#define ALL_FLAG_SET              ((flag_t)0xFFFFFFFFU)

/* -------------------------
 * Critical section abstraction
 * ------------------------- */
#if FLAG_POSIX

typedef pthread_mutex_t flag_mutex_t;
#define FLAG_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER

/* Use the log.h helpers for critical entry/exit */
extern void __entryCriticalSection(pthread_mutex_t* mutex);
extern void __exitCriticalSection(pthread_mutex_t* mutex);

#else /* Non-POSIX fallback */

typedef int flag_mutex_t;
#define FLAG_MUTEX_INITIALIZER 0
#define __entryCriticalSection(m)  do { (void)(m); } while(0)
#define __exitCriticalSection(m)   do { (void)(m); } while(0)

#endif /* FLAG_POSIX */

/* -------------------------
 * Safe flag operations with mutex
 * ------------------------- */
#define FLAG_OP_W_MUTEX(p2mutex, flagOp, flag, bitOrder) \
    do {                                                 \
        __entryCriticalSection(p2mutex);                 \
        flagOp(flag, bitOrder);                          \
        __exitCriticalSection(p2mutex);                  \
    } while (0)

/*
 * Execute arbitrary statements in critical section
 * Example:
 *   PERFORM_IN_CRITICAL(&mutex, doSomething(); doOtherThing(); );
 */
#define PERFORM_IN_CRITICAL(p_mux, ...) \
    do {                                \
        __entryCriticalSection(p_mux);  \
        __VA_ARGS__                     \
        __exitCriticalSection(p_mux);   \
    } while (0)



/* -------------------------
 * End of header
 * ------------------------- */
#endif /* __FLAG_H__ */
