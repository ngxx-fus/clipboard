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

#define __log(...)   __coreLog("log",  __VA_ARGS__)
#define __err(...)   __coreLog("err",  __VA_ARGS__)
#define __entry(...) __coreLog(">>>",  __VA_ARGS__)
#define __exit(...)  __coreLog("<<<",  __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif