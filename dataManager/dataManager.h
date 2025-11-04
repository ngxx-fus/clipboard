#ifndef __DATA_MANAGER_H__
#define __DATA_MANAGER_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>

#include <zlib.h> /* link -lz */

#include "../helper/return.h"
#include "../log/log.h"

#if 1
    #ifndef __log
        #define __log(...) fprintf(stderr, "[LOG]" __VA_ARGS__)
    #endif
    #ifndef __entry
        #define __entry(...) fprintf(stderr, "[>>>]" __VA_ARGS__)
    #endif
    #ifndef __exit
        #define __exit(...) fprintf(stderr, "[<<<]" __VA_ARGS__)
    #endif
    #ifndef __err
        #define __err(...) fprintf(stderr, "[ERR]" __VA_ARGS__)
    #endif


    #ifndef DATAMANAGER_LOG_L0_EN
        #define DATAMANAGER_LOG_L0_EN   1
    #endif 
    #ifndef DATAMANAGER_LOG_L1_EN
        #define DATAMANAGER_LOG_L1_EN   1
    #endif
    #ifndef DATAMANAGER_ENTRY_EN
        #define DATAMANAGER_ENTRY_EN   1
    #endif
    #ifndef DATAMANAGER_EXIT_EN
        #define DATAMANAGER_EXIT_EN   1
    #endif
    #ifndef DATAMANAGER_ERR_EN
        #define DATAMANAGER_ERR_EN   1
    #endif

    #if (DATAMANAGER_LOG_L0_EN == 1)
        #define __dmLog(...) __log(__VA_ARGS__)
    #else
        #define __dmLog(...)
    #endif

    #if (DATAMANAGER_LOG_L1_EN == 1)
        #define __dmLog1(...) __log(__VA_ARGS__)
    #else
        #define __dmLog1(...)
    #endif

    #if (DATAMANAGER_ENTRY_EN == 1)
        #define __dmEntry(...) __entry(__VA_ARGS__)
    #else
        #define __dmEntry(...)
    #endif

    #if (DATAMANAGER_EXIT_EN == 1)
        #define __dmExit(...) __exit(__VA_ARGS__)
    #else
        #define __dmExit(...)
    #endif

    #if (DATAMANAGER_ERR_EN == 1)
        #define __dmErr(...) __err(__VA_ARGS__)
    #else
        #define __dmErr(...)
    #endif

#endif

#define DM_IS_LOWER(c)      ((c) >= 'a' && (c) <= 'z')
#define DM_IS_UPPER(c)      ((c) >= 'A' && (c) <= 'Z')
#define DM_IS_DIGIT(c)      ((c) >= '0' && (c) <= '9')
#define DM_IS_UNDERSCORE(c) ((c) == '_')
#define DM_IS_DASH(c)       ((c) == '-')
#define DM_IS_HASH(c)       ((c) == '#')
#define DM_IS_NEWLINE(c)    ((c) == '\n')
#define DM_IS_VALID(c)      (DM_IS_LOWER(c) || DM_IS_UPPER(c) || DM_IS_DIGIT(c) || DM_IS_DASH(c) ||  DM_IS_NEWLINE(c))

#ifndef DM_ITEM_KEY_SIZE
    #define DM_ITEM_KEY_SIZE 64
#endif
#ifndef DM_ITEM_VALUE_SIZE
    #define DM_ITEM_VALUE_SIZE 256
#endif
#ifndef DM_ITEMS_NUM
    #define DM_ITEMS_NUM 1000
#endif

typedef union dmItem_t{
    struct{
        char key[DM_ITEM_KEY_SIZE];
        char value[DM_ITEM_VALUE_SIZE];
        char type;          /// =T : text | =I : Image | * : unknown
    };
    char arr[DM_ITEM_KEY_SIZE + DM_ITEM_VALUE_SIZE + 1];
}dmItem_t;

typedef union dmItems_t{
    struct {
        int curSize;
        dmItem_t item[DM_ITEMS_NUM];
    };
    char arr[sizeof(int) + sizeof(dmItem_t) * DM_ITEMS_NUM];
} dmItems_t;

#define DM_SET_ITEM_KEY(dm, i, newKey)          snprintf(dm->item[i].key, DM_ITEM_KEY_SIZE, "%s", newKey); 
#define DM_SET_ITEM_VALUE(dm, i, newValue)      snprintf(dm->item[i].value, DM_ITEM_VALUE_SIZE, "%s", newValue); 
#define DM_SET_ITEM(dm, i, newKey, newValue)    snprintf(dm->item[i].key, DM_ITEM_KEY_SIZE, "%s", newKey); \
                                                snprintf(dm->item[i].value, DM_ITEM_VALUE_SIZE, "%s", newValue);

#define DM_GET_ITEM_KEY(i)                      dm->item[i].key
#define DM_GET_ITEM_VALUE(i)                    dm->item[i].value
#define DM_GET_ITEM(i)                          dm->item[i]

// / Store all items
dmItems_t *dm;

def dmRemoveDirectory(const char *path) {
    int *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d) {
        struct dirent *p;
        r = 0;

        while (!r && (p = readdir(d))) {
            int r2 = -1;
            char *buf;
            size_t len;

            // bỏ qua . và ..
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);
            if (buf) {
                struct stat statbuf;
                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode))
                        r2 = dmRemoveDirectory(buf); // gọi đệ quy
                    else
                        r2 = unlink(buf);
                }
                free(buf);
            }
            r = r2;
        }
        closedir(d);
    }

    if (!r)
        r = rmdir(path); // xóa thư mục rỗng

    return r;
}

def dmEnsureDirectory(const char* path){
    if(path == NULL) return ERR_NULL;
    __dmEntry("ensureDirectory(%s)", path);
    /// Supposed that the path is /home/foo/dir
    /// Store that in a buffer
    char buff[256];
    snprintf(buff, sizeof(buff), "%s", path);
    /// This loop will create /home, /home/foo, /home/foo if these directory are not existed
    /// For the last '/', that path will be /root/dir/foo, just to be skipped
    /// The loop start from the 2nd pointer instead of 1st to skip '/' (root directory)
    struct stat st;
    for (char *p = buff + 1; *p; ++p){
        if(*p == '/'){
            /// Temporary replace '/' with '\0' - trick too crop the path
            *p = 0;
            /// Check if the path is existed?
            if (stat(buff, &st) != 0) {
                __dmLog1("[ensureDirectory] Create: %s", buff);
                if (mkdir(buff, 0755) != 0 && errno != EEXIST) {
                    __dmErr("[ensureDirectory] mkdir : %s", strerror(errno));
                    __dmExit("ensureDirectory(): -1");
                    return -1;
                }
            }else{
                __dmLog1("[ensureDirectory] Skip: %s ", buff);
            }
            /// Recovery the '/' the move to next '/'.
            *p = '/';
        }
    }
    /// For the full path if that path no end with '/' (like /home/foo/dir)
    if (stat(buff, &st) != 0) {
        __dmLog1("[ensureDirectory] Create: %s", buff);
        if (mkdir(buff, 0755) != 0 && errno != EEXIST) {
            __dmErr("[ensureDirectory] mkdir : %s", strerror(errno));
            __dmExit("ensureDirectory(): -1");
            return -1;
        }
    }else{
        __dmLog1("[ensureDirectory] Skip: %s ", buff);
    }
    __dmExit("ensureDirectory(): 0");
    return 0;
}

def dmCreateNewObject(dmItems_t **dm){
    if(dm == NULL) {return ERR_NULL;}
    *dm  = (dmItems_t *)malloc(sizeof(dmItems_t));
    memset((*dm), 0, sizeof(*dm));
    return OKE;
}

def dmFreeObject(dmItems_t **dm){
    if(dm == NULL) {return ERR_NULL;}
    free(*dm);
    *dm = NULL;
    return OKE;
}

def dmAppendItem(dmItems_t *dm, dmItem_t *item){
    if(dm == NULL) {return ERR_NULL;}
    memcpy(dm->item[dm->curSize].arr, item->arr, sizeof(dmItem_t));
    dm->curSize;
    return OKE;
}

def dmPopBackItem(dmItems_t *dm, dmItem_t *item){
    --(dm->curSize);
    return OKE;
}

def dmLoadItemsFromBinaryFile(dmItems_t *dm, const char* path){
    if(dm == NULL) {return ERR_NULL;}
    /// Open info file
    gzFile file = gzopen(path, "rb");
    if (file == NULL) {
        __dmErr("[dmLoadItemsFromBinaryFile] Can not open %s!", path);
        return ERR_FILE_READ_FAILED;
    }

    int bytesRead = gzread(file, &dm->curSize, sizeof(int));
    if (bytesRead != sizeof(int) || dm->curSize < 0 || dm->curSize > DM_ITEMS_NUM) {
        __dmErr("[dmLoadItemsFromBinaryFile] Invalid curSize read: %d", dm->curSize);
        gzclose(file);
        return ERR_FILE_READ_FAILED;
    }

    bytesRead = gzread(file, dm->arr + sizeof(int), sizeof(dmItem_t) * dm->curSize);
    if (bytesRead != sizeof(dmItem_t) * dm->curSize) {
        __dmErr("[dmLoadItemsFromBinaryFile] Could not read all items, read %d bytes", bytesRead);
        gzclose(file);
        return ERR_FILE_READ_FAILED;
    }
    gzclose(file);
    return OKE;
}

def dmStoreItemsToBinaryFile(dmItems_t *dm, const char* path){
    if(dm == NULL) {return ERR_NULL;}
    /// Open info file
    gzFile file = gzopen(path, "wb");
    if(file == NULL){
        __dmErr("[dmStoreInfoToBinaryFile] Can not open %s!", path);
        return ERR_FILE_WRITE_FAILED;
    }
    /// Write curSize
    int bytesWritten = gzwrite(file, dm->arr, sizeof(int)); /// sizeof(dmItems_t)
    if (bytesWritten != sizeof(int)) {
        __dmErr("[dmStoreInfoToBinaryFile] Write current size failed, only %d/%d bytes written", bytesWritten, sizeof(int));
        gzclose(file);
        return ERR_FILE_WRITE_FAILED;
    }
    /// Write items
    bytesWritten = gzwrite(file, dm->arr + sizeof(int), sizeof(dmItems_t)-sizeof(int)); /// sizeof(dmItems_t)
    if (bytesWritten != sizeof(dmItems_t)-sizeof(int)) {
        __dmErr("[dmStoreInfoToBinaryFile] Write failed, only %d/%d bytes written", bytesWritten, sizeof(dmItems_t)-sizeof(int));
        gzclose(file);
        return ERR_FILE_WRITE_FAILED;
    }

    gzclose(file);
    return OKE;
}

dmItem_t* dmFindItemByKey(dmItems_t *dm, const char key[DM_ITEM_KEY_SIZE] ){
    for(int i = 0; i < dm->curSize; ++i){
        if(memcmp(dm->item[i].key, key, DM_ITEM_KEY_SIZE) == 0) return ((dm->item) + i);
    }
    return NULL;
}

const char* dmGetValueByKeyEditable(dmItems_t *dm, const char key[DM_ITEM_KEY_SIZE]) {
    for (int i = 0; i < dm->curSize; ++i) {
        if (memcmp(dm->item[i].key, key, DM_ITEM_KEY_SIZE) == 0) {
            return dm->item[i].value;
        }
    }
    return NULL;
}

#endif