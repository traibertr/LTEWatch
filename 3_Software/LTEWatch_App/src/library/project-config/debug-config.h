/* Copyright (c) 2018, HES-SO Valais
 * All rights reserved.
 */
#ifndef MDW_DEBUG_CONFIG_H
#define MDW_DEBUG_CONFIG_H

//==========================================================
// <h> General DEBUG Configuration Tool 
// ---------------------------------------------------------
// <i> With this file, the developer can easily enable or disable the LOG trace of each module
#ifndef DEBUG_NRF_USER
    #define DEBUG_NRF_USER                          1
#endif


//==========================================================
// <q> Factory DEBUG.
// ---------------------------------------------------------
#ifndef DEBUG_FACTORY_ENABLE
    #define DEBUG_FACTORY_ENABLE                    1
#endif

//==========================================================
// <q> SDKTools DEBUG.
// ---------------------------------------------------------
#ifndef DEBUG_SDKTOOLS_ENABLE
    #define DEBUG_SDKTOOLS_ENABLE                    0
#endif

//==========================================================
// <h> Application Function Manager
// ---------------------------------------------------------

//==========================================================
// <q> Application Function Manager general DEBUG.
// ---------------------------------------------------------
#ifndef DEBUG_APP_FUNCTION_MANAGER_ENABLE
    #define DEBUG_APP_FUNCTION_MANAGER_ENABLE       0
#endif

//==========================================================
// <q> Button handling DEBUG.
// ---------------------------------------------------------
#ifndef BTCTRL_DEBUG
    #define BTCTRL_DEBUG                            3U
#endif

#ifndef DEBUG_APP_FUNCTION_MANAGER_BH_ENABLE
    #define DEBUG_APP_FUNCTION_MANAGER_BH_ENABLE    0
#endif

//==========================================================
// <q> GPIO handling DEBUG.
// ---------------------------------------------------------
#ifndef DEBUG_APP_FUNCTION_MANAGER_GH_ENABLE
    #define DEBUG_APP_FUNCTION_MANAGER_GH_ENABLE    0
#endif

//==========================================================
// <q> BLE handling DEBUG.
// ---------------------------------------------------------
#ifndef DEBUG_APP_FUNCTION_MANAGER_BLE_ENABLE
    #define DEBUG_APP_FUNCTION_MANAGER_BLE_ENABLE   0
#endif

//==========================================================
// </h>

//==========================================================
// <h> Main Class Application (MOSKITO)
// ---------------------------------------------------------

//==========================================================
// <q> Main Class Debug
// ---------------------------------------------------------
#ifndef DEBUG_MOSKITO_ENABLE
    #define DEBUG_MOSKITO_ENABLE					0
#endif

//==========================================================
// <q> Main Class State Machine Debug
// ---------------------------------------------------------
#ifndef DEBUG_MOSKITO_ST_ENABLE
    #define DEBUG_MOSKITO_ST_ENABLE                 0
#endif

//==========================================================
// </h>

//==========================================================
// </h>

#endif // MDW_BLE_CONFIG_H
