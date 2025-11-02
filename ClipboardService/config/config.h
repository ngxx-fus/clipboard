#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <zlib.h>   // for gzFile, gzopen, gzprintf, gzgets

#include "../log/log.h"
/// Un comment to enable log (except entry and exit)
// #define CONFIG_LOG
/// Un comment to enable log at ENTRY and EXIT
// #define CONFIG_LOG_ENTRY_EXIT

#define CONFIG_PATH_SIZE     256
#define CONFIG_LABEL_SIZE     64
#define CONFIG_VALUE_SIZE    256

/// 29 first bytes for config prefix + 1 byte '\n'
#define CONFIG_PREFIX        "__NGXXFUS__BEGIN_OF_CONFIG__"
/// 29 first bytes for config size prefix + number + 1 byte '\n'
#define CONFIG_SIZE_PREFIX   "__NGXXFUS__NUMBER_OF_CONFIG_"
/// 29 last bytes for config suffix
#define CONFIG_SUFFIX        "__NGXXFUS__END_OF_CONFIG____"

/// Convert variable name to string
#define STRINGIFY(x) #x
#define NAME2STR(x) STRINGIFY(x)

/// Return types of config status
enum CONFIG_STATUS_RETURN_TYPE {
    CONFIG_STATUS_OKE = 0,              /// No error
    CONFIG_STATUS_NULL_ERROR,           /// Try to access to NULL pointer
    CONFIG_STATUS_ALLOC_FAILED,         /// Can not allocate a memory block
    CONFIG_STATUS_UNKNOWN_ERROR,        /// Just occured an error
    CONFIG_STATUS_FILE_NOT_FOUND,       /// Can not open the file
    CONFIG_STATUS_PARAMETER_ERROR,      /// The param is invalid
    CONFIG_STATUS_FILE_NOT_EXISTED,     /// File not existed
};

typedef int8_t      confStatus_t;
typedef int32_t     numVal_t;
typedef char *      txtVal_t;
typedef char *      label_t;
typedef char *      path_t;

/// Config entry type
enum CONFIG_TYPE {
    CONFIG_TYPE_NOT_SET = 0,
    CONFIG_TYPE_TEXT,
    CONFIG_TYPE_NUMBER,
};

/// Config object
typedef struct config_t {
    path_t      configPath;                     ///< Path to gz file
    int32_t     configNum;                      ///< Number of entries
    uint8_t*    configType;                     ///< Entry type
    label_t*    configLabel;                    ///< Entry label
    txtVal_t*   configValue;                    ///< Entry value
} config_t;

/// @brief Convert from int to string
void intToStr(char* result, size_t resultSize, int32_t value);

/// @brief Convert an integer value to string
/// @param result The buffer to store the result string
/// @param resultSize The size of the result buffer
/// @param value The integer value to convert
void intToStr(char* result, size_t resultSize, int32_t value);

/// @brief Initialize configuration from a GZipped file
/// @param confPtr Pointer to a config_t* that will store the created config
/// @param gzPath Path to the gzipped config file
/// @return Status of the operation (confStatus_t)
confStatus_t initialConfigFromGZFile(config_t ** confPtr, const path_t gzPath);

/// @brief Create a configuration object
/// @param confPtr Pointer to a config_t* that will store the created config
/// @param path Path to the config file
/// @param confNum Number of configuration entries
/// @return Status of the operation (confStatus_t)
confStatus_t createConfig(config_t ** confPtr, const path_t path, int32_t confNum);

/// @brief Delete a configuration object and free all memory
/// @param confPtr Pointer to the config_t* to delete
void deleteConfig(config_t ** confPtr);

/// @brief Save a configuration object to its associated file
/// @param conf Pointer to the configuration object
/// @return Status of the operation (confStatus_t)
confStatus_t saveConfig(config_t * conf);

/// @brief Set a numeric configuration entry
/// @param conf Pointer to the configuration object
/// @param idx Index of the configuration entry
/// @param label Label of the configuration entry
/// @param value Numeric value to set
void setNumberConfig(config_t * conf, int idx, label_t label, numVal_t value);

/// @brief Set a text configuration entry
/// @param conf Pointer to the configuration object
/// @param idx Index of the configuration entry
/// @param label Label of the configuration entry
/// @param value Text value to set
void setTextConfig(config_t * conf, int idx, label_t label, txtVal_t value);

/// @brief Get the value of a configuration entry by its label
/// @param conf Pointer to the configuration object
/// @param label Label of the configuration entry
/// @return The configuration value as a string, or NULL if not found
const char* getConfig(config_t * conf, label_t label);

#endif // __CONFIG_H__
