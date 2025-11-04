#include "config.h"

void intToStr(char* result, size_t resultSize, int32_t value) {
    snprintf(result, resultSize, "%d", value);
}

confStatus_t initialConfigFromGZFile(config_t ** confPtr, const path_t gzPath) {
    #ifdef CONFIG_LOG_ENTRY_EXIT
        __entry("initialConfigFromGZFile(%p, %s)", confPtr, gzPath);
    #endif 

    if (!confPtr) {
        #ifdef CONFIG_LOG_ENTRY_EXIT
            __exit("initialConfigFromGZFile() -> %s", NAME2STR(CONFIG_STATUS_NULL_ERROR));
        #endif 
        return CONFIG_STATUS_NULL_ERROR;
    }
    gzFile f = gzopen(gzPath, "rb");
    if (!f) {
        #ifdef CONFIG_LOG_ENTRY_EXIT
            __err("Cannot open %s file", gzPath);
            __exit("initialConfigFromGZFile() -> %s", NAME2STR(CONFIG_STATUS_NULL_ERROR));
        #endif 
        return CONFIG_STATUS_FILE_NOT_EXISTED;
    }

    char line[CONFIG_LABEL_SIZE + CONFIG_VALUE_SIZE + 8];

    // --- check prefix ---
    if (!gzgets(f, line, sizeof(line))) {
        #ifdef CONFIG_LOG
            __err("The %s file is empty!", gzPath);
        #endif 
        goto initialConfigFromGZFile_ReturnUnknownError;
    }
    line[strcspn(line, "\n")] = '\0';
    if (strcmp(line, CONFIG_PREFIX) != 0) {
        #ifdef CONFIG_LOG
            __err("The %s file is corrupted or wrong format!", gzPath);
        #endif 
        goto initialConfigFromGZFile_ReturnUnknownError;
    }

    // --- read config size ---
    if (!gzgets(f, line, sizeof(line))) {
        #ifdef CONFIG_LOG
            __err("Can not read config size! The %s file may be corrupted!", gzPath);
        #endif 
        goto initialConfigFromGZFile_ReturnUnknownError;
    }
    char *res = strstr(line, CONFIG_SIZE_PREFIX);
    if (!res) {
        #ifdef CONFIG_LOG
            __err("Can not read config size! The %s file may be corrupted!", gzPath);
        #endif 
        goto initialConfigFromGZFile_ReturnUnknownError;
    }
    res += strlen(CONFIG_SIZE_PREFIX);
    int configNum = atoi(res);

    #ifdef CONFIG_LOG
        __log("The %s file opened with size = %d", gzPath, configNum);
    #endif 

    // --- alloc config ---
    *confPtr = malloc(sizeof(config_t));
    if (!*confPtr) {
        gzclose(f);
        return CONFIG_STATUS_ALLOC_FAILED;
    }
    (*confPtr)->configPath = (path_t) malloc(CONFIG_PATH_SIZE);
    strncpy((*confPtr)->configPath, gzPath, CONFIG_PATH_SIZE);
    (*confPtr)->configNum = configNum;
    (*confPtr)->configType  = malloc(sizeof(uint8_t) * configNum);
    (*confPtr)->configLabel = malloc(sizeof(char*)  * configNum);
    (*confPtr)->configValue = malloc(sizeof(char*)  * configNum);

    // --- read configs ---
    int idx = 0; /// 
    while (idx < configNum + 1 /*config suffix*/ && gzgets(f, line, sizeof(line))) {
        line[strcspn(line, "\n")] = '\0';
        
        #ifdef CONFIG_LOG
            // __log("Line: [%s]", line);
        #endif 

        if (strcmp(line, CONFIG_SUFFIX) == 0) break;

        char *eq = strchr(line, '=');
        if (!eq) continue;

        *eq = '\0';
        (*confPtr)->configLabel[idx] = malloc(CONFIG_LABEL_SIZE);
        (*confPtr)->configValue[idx] = malloc(CONFIG_VALUE_SIZE);

        strncpy((*confPtr)->configLabel[idx], line, CONFIG_LABEL_SIZE);
        strncpy((*confPtr)->configValue[idx], eq + 1, CONFIG_VALUE_SIZE);

        #ifdef CONFIG_LOG
            __log("Content: [label=%s][value=%s]", (*confPtr)->configLabel[idx], (*confPtr)->configValue[idx]);
        #endif 

        (*confPtr)->configType[idx] = CONFIG_TYPE_TEXT;
        idx++;
    }

    // --- check suffix ---
    if (strcmp(line, CONFIG_SUFFIX) != 0) {
        #ifdef CONFIG_LOG
            __err("Not found config suffix %s in %s!", CONFIG_SUFFIX, line);
        #endif 
        deleteConfig(confPtr);
        goto initialConfigFromGZFile_ReturnUnknownError;
    }

    // initialConfigFromGZFile_ReturnStatusOke:
    gzclose(f);
    #ifdef CONFIG_LOG_ENTRY_EXIT
        __exit("initialConfigFromGZFile() -> %s", NAME2STR(CONFIG_STATUS_OKE));
    #endif
    return CONFIG_STATUS_OKE;

    initialConfigFromGZFile_ReturnUnknownError:
    gzclose(f);
    #ifdef CONFIG_LOG_ENTRY_EXIT
        __exit("initialConfigFromGZFile() -> %s", NAME2STR(CONFIG_STATUS_NULL_ERROR));
    #endif
    return CONFIG_STATUS_NULL_ERROR;
}

confStatus_t createConfig(config_t ** confPtr, const path_t path, int32_t confNum) {
    #ifdef CONFIG_LOG_ENTRY_EXIT
        __entry("createConfig(%p, %s, %d)", confPtr, path, confNum);
    #endif 

    if (!confPtr) {
        #ifdef CONFIG_LOG_ENTRY_EXIT
            __exit("createConfig() -> %s", NAME2STR(CONFIG_STATUS_NULL_ERROR));
        #endif 
        return CONFIG_STATUS_NULL_ERROR;
    }

    confStatus_t status = initialConfigFromGZFile(confPtr, path);
    if( status == CONFIG_STATUS_OKE){
        #ifdef CONFIG_LOG_ENTRY_EXIT
            __exit("createConfig() -> %s", NAME2STR(CONFIG_STATUS_OKE));
        #endif 
        return CONFIG_STATUS_OKE;
    }

    if(!confNum){
        #ifdef CONFIG_LOG
            __err("Can not create config_t object! Because the %s file not exited and confNum = %d", path, confNum);
        #endif
        return CONFIG_STATUS_PARAMETER_ERROR;
    }

    *confPtr = (config_t*) malloc(sizeof(config_t));
    if (!*confPtr) {
        #ifdef CONFIG_LOG_ENTRY_EXIT
            __exit("createConfig() -> %s", NAME2STR(CONFIG_STATUS_NULL_ERROR));
        #endif 
        return CONFIG_STATUS_NULL_ERROR;
    }

    (*confPtr)->configPath = (path_t) malloc(CONFIG_PATH_SIZE);
    strncpy((*confPtr)->configPath, path, CONFIG_PATH_SIZE);
    (*confPtr)->configNum = confNum;

    (*confPtr)->configType  = (uint8_t*) malloc(sizeof(uint8_t) * confNum);
    (*confPtr)->configLabel = (char**)  malloc(sizeof(char*) * confNum);
    (*confPtr)->configValue = (char**)  malloc(sizeof(char*) * confNum);

    char buff[CONFIG_VALUE_SIZE];
    for (int i = 0; i < confNum; ++i) {
        (*confPtr)->configType[i]  = CONFIG_TYPE_NOT_SET;
        (*confPtr)->configLabel[i] = (char*) malloc(CONFIG_LABEL_SIZE);
        (*confPtr)->configValue[i] = (char*) malloc(CONFIG_VALUE_SIZE);

        snprintf(buff, sizeof(buff), "Default-Label-%d", i);
        strncpy((*confPtr)->configLabel[i], buff, CONFIG_LABEL_SIZE);

        snprintf(buff, sizeof(buff), "Default-Value-%d", i);
        strncpy((*confPtr)->configValue[i], buff, CONFIG_VALUE_SIZE);
    }

    #ifdef CONFIG_LOG_ENTRY_EXIT
        __exit("createConfig() -> %d", CONFIG_STATUS_OKE);
    #endif 
    return CONFIG_STATUS_OKE;
}

void deleteConfig(config_t ** confPtr) {
    #ifdef CONFIG_LOG_ENTRY_EXIT
        __entry("deleteConfig(%p)", confPtr);
    #endif 
    if (!confPtr || !*confPtr) return;

    for (int i = 0; i < (*confPtr)->configNum; ++i) {
        free((*confPtr)->configLabel[i]);
        free((*confPtr)->configValue[i]);
    }

    free((*confPtr)->configLabel);
    free((*confPtr)->configValue);
    free((*confPtr)->configType);
    free(*confPtr);
    *confPtr = NULL;
    #ifdef CONFIG_LOG_ENTRY_EXIT
        __exit("deleteConfig()");
    #endif 
}

confStatus_t saveConfig(config_t * conf) {
    #ifdef CONFIG_LOG_ENTRY_EXIT
        __entry("saveConfig(%p)", conf);
    #endif 
    if (!conf) {
        #ifdef CONFIG_LOG_ENTRY_EXIT
        __err("The conf is NULL!");
        __exit("saveConfig() -> %s", NAME2STR(CONFIG_STATUS_NULL_ERROR));
        #endif 
        return CONFIG_STATUS_NULL_ERROR;
    }

    gzFile f = gzopen(conf->configPath, "wb");
    if (!f) {
        #ifdef CONFIG_LOG_ENTRY_EXIT
        __err("Cannot open the %s file!", conf->configPath);
        __exit("saveConfig() -> %s", NAME2STR(CONFIG_STATUS_FILE_NOT_FOUND));
        #endif 
        return CONFIG_STATUS_FILE_NOT_FOUND;
    }

    gzprintf(f, "%s\n", CONFIG_PREFIX);
    gzprintf(f, "%s%d\n", CONFIG_SIZE_PREFIX, conf->configNum);

    for (int i = 0; i < conf->configNum; ++i) {
        gzprintf(f, "%s=%s\n", conf->configLabel[i], conf->configValue[i]);
    }

    gzprintf(f, "%s\n", CONFIG_SUFFIX);
    gzclose(f);

    #ifdef CONFIG_LOG_ENTRY_EXIT
        __exit("saveConfig() -> %s", NAME2STR(CONFIG_STATUS_OKE));
    #endif 
    return CONFIG_STATUS_OKE;
}

void setNumberConfig(config_t * conf, int idx, label_t label, numVal_t value) {
    #ifdef CONFIG_LOG_ENTRY_EXIT
        __entry("setNumberConfig(%p, %d, %s, %d)", conf, idx, label, value);
    #endif 

    if (!conf || idx >= conf->configNum) return;
    
    char valBuff[64];
    snprintf(valBuff, sizeof(valBuff), "%d", value);
    
    strncpy(conf->configLabel[idx], label, CONFIG_LABEL_SIZE);
    strncpy(conf->configValue[idx], valBuff, CONFIG_VALUE_SIZE);
    conf->configType[idx] = CONFIG_TYPE_TEXT;
    #ifdef CONFIG_LOG_ENTRY_EXIT
        __exit("setConfig()");
    #endif 
}

void setTextConfig(config_t * conf, int idx, label_t label, txtVal_t value) {
    #ifdef CONFIG_LOG_ENTRY_EXIT
        __entry("setTextConfig(%p, %d, %s, %s)", conf, idx, label, value);
    #endif 
    if (!conf || idx >= conf->configNum) return;
    strncpy(conf->configLabel[idx], label, CONFIG_LABEL_SIZE);
    strncpy(conf->configValue[idx], value, CONFIG_VALUE_SIZE);
    conf->configType[idx] = CONFIG_TYPE_TEXT;
    #ifdef CONFIG_LOG_ENTRY_EXIT
        __exit("setConfig()");
    #endif 
}

const char* getConfig(config_t * conf, label_t label) {
    #ifdef CONFIG_LOG
        __entry("getConfig(%p, %s)", conf, label);
    #endif 
    if (!conf) {
        #ifdef CONFIG_LOG_ENTRY_EXIT
            __exit("getConfig() -> %p", NULL);
        #endif 
        return NULL;
    }
    for (int i = 0; i < conf->configNum; ++i) {
        if (strcmp(conf->configLabel[i], label) == 0) {
            #ifdef CONFIG_LOG_ENTRY_EXIT
                __exit("getConfig() -> %p", conf->configValue[i]);
            #endif 
            return conf->configValue[i];
        }
    }

    #ifdef CONFIG_LOG_ENTRY_EXIT
        __exit("getConfig() -> NULL");
    #endif 
    return NULL;
}
