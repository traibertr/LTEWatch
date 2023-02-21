/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz & Thomas Sterren
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "xf.h"
#include "xftimeoutmanager.h"


/* **************************************************************************************************************** */
/* Declaration section                                                                                              */
/* **************************************************************************************************************** */
XFThread XF::_mainThread;


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void XF::start()
{
    // Start it timeout manager
    XFTimeoutManager::getInstance()->start();
    
    // Start main thread
    _mainThread.start();
}

// ------------------------------------------------------------------------------------------------------------------
XFThread* XF::getMainThread()
{
    return &_mainThread;
}

// ------------------------------------------------------------------------------------------------------------------
void XF::execOnce()
{
    static bool bFirstTime = true;

    if (bFirstTime)
    {
        bFirstTime = false;

        // Start it timeout manager
        XFTimeoutManager::getInstance()->start();
    }

    // Process events once
    _mainThread.execOnce();
}