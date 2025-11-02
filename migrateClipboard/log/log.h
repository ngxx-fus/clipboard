#ifndef __LOG_H__
#define __LOG_H__

#ifdef LOG_HEADER_INCLUDE
#pragma message("INCLUDE: log.h")
#endif 

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stddef.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

extern pthread_mutex_t  logMutex;             /// Mutex lock for logging lock

void __entryCriticalSection(pthread_mutex_t* mutex);
void __exitCriticalSection(pthread_mutex_t* mutex);
void __coreLog(const char* tag, const char* format, ...)
    __attribute__((format(printf, 2, 3)));

#if 1   /// Log config
    #ifndef CB_LOG_L0_EN
    #define CB_LOG_L0_EN            1
    #endif

    #ifndef CB_LOG_L1_EN
    #define CB_LOG_L1_EN            0
    #endif

    #ifndef CB_LOG_L2_EN
    #define CB_LOG_L2_EN            0
    #endif

    #ifndef CB_LOG_ERR_EN
    #define CB_LOG_ERR_EN           1
    #endif

    #ifndef CB_LOG_L0_ENTRY
    #define CB_LOG_L0_ENTRY         1
    #endif

    #ifndef CB_LOG_L0_EXIT
    #define CB_LOG_L0_EXIT          1
    #endif

    #ifndef CB_LOG_L1_ENTRY
    #define CB_LOG_L1_ENTRY         0
    #endif

    #ifndef CB_LOG_L1_EXIT
    #define CB_LOG_L1_EXIT          0
    #endif

    #ifndef CB_LOG_L2_ENTRY
    #define CB_LOG_L2_ENTRY         0
    #endif

    #ifndef CB_LOG_L2_EXIT
    #define CB_LOG_L2_EXIT          0
    #endif
#endif

#if (CB_LOG_L0_EN == 1)
    #define __log(...)   __coreLog("log",  __VA_ARGS__)
#else
    #define __log(...)
#endif

#if (CB_LOG_L1_EN == 1)
    #define __log1(...)   __coreLog("log",  __VA_ARGS__)
#else
    #define __log1(...)
#endif

#if (CB_LOG_L2_EN == 1)
    #define __log2(...)   __coreLog("log",  __VA_ARGS__)
#else
    #define __log2(...)
#endif

#if (CB_LOG_ERR_EN == 1)
    #define __err(...)   __coreLog("err",  __VA_ARGS__)
#else
    #define __err(...)
#endif

#if (CB_LOG_L0_ENTRY == 1)
    #define __entry(...) __coreLog(">>>",  __VA_ARGS__)
#else
    #define __entry(...)
#endif

#if (CB_LOG_L0_EXIT == 1)
    #define __exit(...)  __coreLog("<<<",  __VA_ARGS__)
#else
    #define __exit(...)
#endif

#if (CB_LOG_L1_ENTRY == 1)
    #define __entry1(...) __coreLog(">>>",  __VA_ARGS__)
#else
    #define __entry1(...)
#endif

#if (CB_LOG_L1_EXIT == 1)
    #define __exit1(...)  __coreLog("<<<",  __VA_ARGS__)
#else
    #define __exit1(...)
#endif

#if (CB_LOG_L2_ENTRY == 1)
    #define __entry1(...) __coreLog(">>>",  __VA_ARGS__)
#else
    #define __entry2(...)
#endif

#if (CB_LOG_L2_EXIT == 1)
    #define __exit2(...)  __coreLog("<<<",  __VA_ARGS__)
#else
    #define __exit2(...)
#endif


#ifdef __cplusplus
}
#endif

#endif
