#include "dataManager.h"

#define currentPath     "/workspaces/clipboard/dataManager"
#define baseDir         currentPath "/.testPath_BaseDir"
#define baseImg         baseDir "/image"
#define baseTxt         baseDir "/text"
#define configPath      "/home/codespace/.config/clipboard"

int main(){
    dmEnsureDirectory(configPath);
    // dmRemoveDirectory()
    dmCreateNewObject(&dm);
    // dm->curSize          = 3;
    // DM_SET_ITEM(dm, 0, "AppName", "NgxxFus' Clipboard");
    // DM_SET_ITEM(dm, 1, "Author", "Nguyen Thanh Phu");
    // DM_SET_ITEM(dm, 2, "Version", "0.0.1");
    // dmStoreItemsToBinaryFile(dm, configPath "/config");

    dmLoadItemsFromBinaryFile(dm, configPath "/config");
    __log("item, %s - %s", DM_GET_ITEM_KEY(0), DM_GET_ITEM_VALUE(0));
    __log("item, %s - %s", DM_GET_ITEM_KEY(1), DM_GET_ITEM_VALUE(1));
    __log("item, %s - %s", DM_GET_ITEM_KEY(2), DM_GET_ITEM_VALUE(2));
    return 0;
}