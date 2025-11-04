#include "clipboardService.h"

int main() {
    config_t *conf = NULL;

    int age = 21;
    // --- Step 1: Create config with 3 entries ---
    createConfig(&conf, "test_config.gz", 5);
    setTextConfig(conf, 0, "username", "ngxxfus");
    setTextConfig(conf, 1, "uid", "12345");
    setTextConfig(conf, 2, "language", "C");
    setNumberConfig(conf, 3, NAME2STR(age), age);

    // --- Step 2: Save to gz file ---
    __log("Saving config...");
    saveConfig(conf);

    // Free old object
    deleteConfig(&conf);

    // --- Step 3: Load config back ---
    __log("Loading config back...");
    confStatus_t status = createConfig(&conf, "test_config.gz", 3);
    if (status != CONFIG_STATUS_OKE) {
        __err("Failed to load config! Status=%d", status);
        return -1;
    }

    // --- Step 4: Print loaded values ---
    __log("Loaded config from file: %s", conf->configPath);
    for (int i = 0; i < conf->configNum; i++) {
        __log("  [label=%s][value=%s]", conf->configLabel[i], conf->configValue[i]);
    }

    // Cleanup
    deleteConfig(&conf);
    return 0;
}