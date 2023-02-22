/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    xf.h
 *
 * @addtogroup XF eXecution Framework
 * \{
 *
 * @class   XF
 * @brief   Execution Framework
 *
 * \section sec_xf_intro Introduction
 *
 * This library implements an e<b>X</b>ecution <b>F</b>ramework (XF) to drive finite state machines. Using this 
 * framework, it is possible to drive multiple state machines in a concurrent manner (pseudo-parallel) without 
 * having an operation system.
 *
 * The XF defines an interface which gives the ability to reimplement parts of the XF to make adaptations to an other 
 * target. It is also possible to let the XF run with an RTOS. This gives the possibility to run state machines in a 
 * OS like concurrent manner with threads provided by the underlying RTOS.
 *
 * \section sec_xf_cmd XF Class Diagram
 *
 * Following class diagram shows the basic relations between the most important classes in the PTR XF. It shall give 
 * you a basic understanding about the relations between these classes:
 *
 * \image html cmd-xf.png "XF Class Diagram"
 *
 * \section sec_xf_start Starting Point
 *
 * A good point to jump into the definitions of the classes is the #XF class. Other classes of interest are 
 * XFTimeoutManager, XFThread and XFReactive.
 *
 * XF Version: 1.5 (2016-06-09)
 *  - sth: Added critical section to XFTimeoutManager::add() method. Can be enabled using the XF_TM_PROTECT_ADD_TM define.
 * XF Version: 1.4 (2016-05-24)
 *  - sth: Added xfPreExecuteHook() and xfPostExecuteHook() functions.
 * XF Version: 1.3 (2016-04-21)
 *  - sth: XFTimeouts in XFTimeoutManager can now be created using memory pool. See define XF_TM_USE_MEMORYPOOL_FOR_TIMEOUTS.
 * XF Version: 1.2 (2016-04-19)
 *  - sth: Reworked XFTimeoutManager class. Added defines XF_TM_USE_ISLIST and XF_TM_STATIC_INSTANCE
 * XF Version: 1.1 (2014-12-11)
 *  - rut: Removed the #define USE_STDLIB and assumed that we only use the template-queue
 *  - rut: To downsize the code, we only use uint8_t, uint16_t or uint32_t literal types depending on their usage
 * XF Version: 1.0 (2013-08-15)
 *  - rut: Adapted XF to fit with nRF51822 peripherals
 *  - rut: Added a "Critical Queue" to handle prioritary events
 *  - rut: Adapted the use of the XF's timer to safe current consumption
 * XF Version: 0.2 (2011-08-14)
 *  - sth: Fixed bug in XFTimeoutManager::addTimeout method
 * XF Version: 0.1 (2010-11-26)
 *  - sth: First release
 *
 * Initial author: Thomas Sterren
 * Creation date: 2010-11-26
 *
 * \author  Patrice Rudaz
 * \date    July 2018
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include "xf-config.h"

#include "xfthread.h"

#ifndef NULL
    #define NULL ((void*)0)
#endif

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class XF
{
public:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC DECLARATION SECTION                                                                                   */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
     * @brief   Starts the execution of the framework. This
     *
     * Starts the execution of the framework. This results in processing the events in the main loop.
     ****************************************************************************************************************/
    static void start();

    /************************************************************************************************************//**
     * @brief   Returns pointer the main thread.
     ****************************************************************************************************************/
    static XFThread* getMainThread();

    /************************************************************************************************************//**
     * @brief   Non-blocking version to processes events.
     ****************************************************************************************************************/
    static void execOnce();

protected:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PROTECTED DECLARATION SECTION                                                                                */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    // Attributes
    static XFThread _mainThread;            ///< Main thread executing the main loop.
};

/** \} */
