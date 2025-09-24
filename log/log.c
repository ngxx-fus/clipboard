#include "log.h"

pthread_mutex_t logMutex = PTHREAD_MUTEX_INITIALIZER;

void __entryCriticalSection(pthread_mutex_t* mutex) {
    int rc = pthread_mutex_lock(mutex);
    if (rc) {
        fprintf(stderr, "[err] [critical section] Lock failed: %d\n", rc);
    }
}

void __exitCriticalSection(pthread_mutex_t* mutex) {
    int rc = pthread_mutex_unlock(mutex);
    if (rc) {
        fprintf(stderr, "[err] [critical section] Unlock failed: %d\n", rc);
    }
}

void __coreLog(const char* tag, const char* format, ...) {
    struct timeval __timeVal;
    gettimeofday(&__timeVal, NULL);

    struct tm __tmBuf;
    localtime_r(&__timeVal.tv_sec, &__tmBuf);

    char __buffer[32];
    strftime(__buffer, sizeof(__buffer), "%H:%M:%S", &__tmBuf);

    va_list args;
    va_start(args, format);
    __entryCriticalSection(&logMutex);
    fprintf(stderr, "[%s.%06ld] [%s] ", __buffer, __timeVal.tv_usec, tag);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    __exitCriticalSection(&logMutex);
    va_end(args);
}
