#ifndef __GLOBAL_DEF_H__
#define __GLOBAL_DEF_H__

/// HEADERs ///////////////////////////////////////////////////////////////////////////////////////

#include "../helper/general.h"
#include "../windowContext/wdct.h"
#include "../clipboardManager/clipboardManager.h"

#include "stageCtl.h"
#include "fileCtl.h"
#include "windowCtl.h" 

/// VARs & DEFs ///////////////////////////////////////////////////////////////////////////////////

/// SYSTEM FLAG /////////////////////////////////////////////////////////////////////////

/// @brief System flag, do not access directly
_Atomic flag_t systemFlag;

#define __systemFlagFull()          atomic_load(&systemFlag)
#define __systemFlagCheck(bit)      atomic_load(&systemFlag) & __flagMask(bit)
#define __systemFlagSet(bit)        atomic_fetch_or(&systemFlag, __flagMask(bit))
#define __systemFlagClr(bit)        atomic_fetch_and(&systemFlag, __flagInvMask(bit))

enum SYSTEM_FLAG {
    SYSTEM_FLAG_NUM,
};

/// UI FLAG /////////////////////////////////////////////////////////////////////////////

_Atomic flag_t uiFlag;

#define __uiFlagGetFull()           atomic_load(&uiFlag)
#define __uiFlagSetFull(new)        atomic_store(&uiFlag, new)
#define __uiFlagCheck(bit)          atomic_load(&uiFlag) & __flagMask(bit)
#define __uiFlagSet(bit)            atomic_fetch_or(&uiFlag, __flagMask(bit))
#define __uiFlagClr(bit)            atomic_fetch_and(&uiFlag, __flagInvMask(bit))

enum UI_FLAG{
    RESERVED = 0,
    /// POP_UP
    UI_FLAG_POPUP_CLICKED_CLOSE,
    UI_FLAG_POPUP_CLICKED_YES,
    UI_FLAG_POPUP_CLICKED_NO,
    UI_FLAG_NUM
};

uint8_t appAgreement = 0;





#endif