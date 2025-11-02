#ifndef __STAGE_CONTROL_H__
#define __STAGE_CONTROL_H__

#include "../helper/general.h"
#include "../windowContext/wdct.h"
#include "../clipboardManager/clipboardManager.h"

/// ==========================================================================
/// SYSTEM STAGE CONTROL
/// ==========================================================================
enum SYSTEM_STAGE {
    SYSTEM_INIT_L0 = 0,
    SYSTEM_INIT_L1,
    SYSTEM_INIT_L2,
    SYSTEM_INIT_L3,
    SYSTEM_INIT_L4,
    SYSTEM_INIT_L5,
    SYSTEM_INIT_L6,
    SYSTEM_INIT_L7,
    SYSTEM_INIT_L8,
    SYSTEM_INIT_L9,
    SYSTEM_INIT_L10,
    SYSTEM_RUNNING,
    SYSTEM_STOPPED,
    SYSTEM_STAGE_NUM
};

/// @brief Global atomic system stage variable
_Atomic uint8_t systemStage = SYSTEM_INIT_L0;

/// ==========================================================================
/// BASIC HELPERS
/// ==========================================================================

/// @brief Internal function to set stage with log
#define __SET_STAGE(s)                                                      \
    do {                                                                    \
        uint8_t __old = atomic_load(&systemStage);                          \
        atomic_store(&systemStage, (s));                                    \
        __log("[stage] %02d → %02d", __old, (uint8_t)(s));                  \
    } while (0)

/// @brief Check if system is still initializing
#define IS_SYSTEM_INIT        (atomic_load(&systemStage) <  SYSTEM_RUNNING)

/// @brief Check if system is currently running
#define IS_SYSTEM_RUNNING     (atomic_load(&systemStage) == SYSTEM_RUNNING)

/// @brief Check if system has stopped
#define IS_SYSTEM_STOPPED     (atomic_load(&systemStage) >= SYSTEM_STOPPED)

/// ==========================================================================
/// STAGE SETTERS
/// ==========================================================================
#define __SET_SYSTEM_INIT__   __SET_STAGE(SYSTEM_INIT_L0)
#define __SET_SYSTEM_RUN__    __SET_STAGE(SYSTEM_RUNNING)
#define __SET_SYSTEM_STOP__   __SET_STAGE(SYSTEM_STOPPED)

/// ==========================================================================
/// WAITING / SYNC HELPERS
/// ==========================================================================

/// @brief Infinite wait (used for halting safely)
#define __WAIT_FOR_INFINITY__                                             \
    do {                                                                  \
        struct timespec ts = {.tv_sec = 1, .tv_nsec = 0};                 \
        __log("[stage] Waiting infinitely...");                           \
        while (1) nanosleep(&ts, NULL);                                   \
    } while (0)

/// @brief Wait until system leaves INIT state (< SYSTEM_RUNNING)
#define __WAIT_FOR_INIT__                                                 \
    do {                                                                  \
        struct timespec ts = {.tv_sec = 0, .tv_nsec = __MSEC(1)};         \
        __log("[stage] Waiting for INIT to finish...");                   \
        while (atomic_load(&systemStage) < SYSTEM_RUNNING) {              \
            nanosleep(&ts, NULL);                                         \
        }                                                                 \
        __log("[stage] INIT finished (stage=%d)", atomic_load(&systemStage)); \
    } while (0)

/// @brief Wait until system reaches given level
#define __WAIT_FOR_(level)                                                \
    do {                                                                  \
        struct timespec ts = {.tv_sec = 0, .tv_nsec = 20000};             \
        uint8_t lvl = (level);                                            \
        __log("[stage] Waiting for stage >= %d...", lvl);                 \
        while (atomic_load(&systemStage) < lvl) {                         \
            nanosleep(&ts, NULL);                                         \
        }                                                                 \
        __log("[stage] Reached stage %d", atomic_load(&systemStage));     \
    } while (0)

/// ==========================================================================
/// STAGE ADVANCEMENT
/// ==========================================================================

/// @brief Move to next init stage, capped at SYSTEM_RUNNING
#define __MV_2NEXT_INIT_STAGE__                                           \
    do {                                                                  \
        uint8_t cur = atomic_load(&systemStage);                          \
        if (cur < SYSTEM_RUNNING) {                                       \
            uint8_t next = cur + 1;                                       \
            if (next >= SYSTEM_RUNNING) next = SYSTEM_RUNNING;            \
            __log("[stage] NEXT_INIT %02d → %02d", cur, next);            \
            atomic_store(&systemStage, next);                             \
        } else {                                                          \
            __log("[stage] Already >= SYSTEM_RUNNING (cur=%d)", cur);     \
        }                                                                 \
    } while (0)

/// @brief Move system directly to RUNNING stage
#define __MV_2RUNNING_STAGE__                                             \
    do {                                                                  \
        uint8_t cur = atomic_load(&systemStage);                          \
        __log("[stage] FORCE RUNNING %02d → %02d", cur, SYSTEM_RUNNING);  \
        atomic_store(&systemStage, SYSTEM_RUNNING);                       \
    } while (0)

/// @brief Move system directly to STOPPED stage
#define __MV_2STOPPED_STAGE__                                             \
    do {                                                                  \
        uint8_t cur = atomic_load(&systemStage);                          \
        __log("[stage] FORCE STOPPED %02d → %02d", cur, SYSTEM_STOPPED);  \
        atomic_store(&systemStage, SYSTEM_STOPPED);                       \
    } while (0)

#endif /* __STAGE_CONTROL_H__ */
