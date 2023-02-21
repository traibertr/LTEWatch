/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    xfthread.h
 *
 * \addtogroup XF eXecution Framework
 * \{
 *
 * \class   XFThread
 * \brief   XFThread represents the instance executing the behaviour.
 *
 * XFThread representing the instance executing the behaviour. In a OS less XF, only one instance of XFThread gets 
 * created (see XF::_mainThread). In a XF interfacing an OS, multiple instances can be made, each interfacing the 
 * real OS thread.
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
#include "ixfevent.h"
#include "../queue/template-queue.h"

//#include <assert-config.h>

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* NAMESPACE Declaration                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
using namespace std;


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class XFThread
{
public:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* CONSTRUCTOR SECTION                                                                                          */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    XFThread( uint8_t priorityLevelerThreshold = 20 );
    virtual ~XFThread();


    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC DECLARATION SECTION                                                                                   */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
     * \brief   Starts the thread.
     ****************************************************************************************************************/
    void start()                                        { execute(false); }

    /************************************************************************************************************//**
     * \brief   Process event queue until empty.
     ****************************************************************************************************************/
    void execOnce()                                     { execute(true); }

    #if (XF_CRITICAL_QUEUE != 0)
        /********************************************************************************************************//**
        * \brief   Adds event to the #_criticalEvents queue.
        ************************************************************************************************************/
        void pushCriticalEvent(IXFEvent * pEvent);
    #endif // XF_CRITICAL_QUEUE

    /************************************************************************************************************//**
     * \brief   Adds event to the #_events queue.
     ****************************************************************************************************************/
    void pushEvent(IXFEvent * pEvent);

    /************************************************************************************************************//**
     * \brief   Adds a new time-out to be handled. 
     *
     * Adds a new time-out to be handled. The XFThread will forward the time-out information to the time-out manager 
     * which is responsible to handle all time-outs.
     *
     * \param   timeoutId   The time-out id known by the reactive parameter. Is needed by the reactive part to
     *                      uniquely identify the time-out.
     * \param   interval    The time in milliseconds or timer's ticks unit to wait until the time-out expires.
     * \param   pReactive   The reactive instance where to inject the time-out when it time-outs.
     ****************************************************************************************************************/
    void scheduleTimeout(uint8_t timeoutId, uint32_t interval, IXFReactive * pReactive);

    /************************************************************************************************************//**
     * \brief   Adds a new time-out to be handled.
     *
     * Adds a new time-out to be handled. The XFThread will forward the time-out information to the time-out manager
     * which is responsible to handle all time-outs.
     *
     * \param   timeoutId   The time-out id known by the reactive parameter. Is needed by the reactive part to
     *                      uniquely identify the time-out.
     * \param   ticks       The number of timer's ticks unit to wait until the time-out expires.
     * \param   pReactive   The reactive instance where to inject the time-out when it time-outs.
     ****************************************************************************************************************/
    void scheduleTimeoutWithTicks(uint8_t timeoutId, uint32_t ticks, IXFReactive * pReactive);

    /************************************************************************************************************//**
     * \brief   Removes all time-outs corresponding the given parameters.
     ****************************************************************************************************************/
    void unscheduleTimeout(uint8_t timeoutId, IXFReactive * pReactive);

protected:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PROTECTED DECLARATION SECTION                                                                                */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    // Attributes
    bool 	_bExecuting;                                ///< \brief True as long as the thread is executing the main loop.
    uint8_t _priorityThreshold;                         ///< \brief This the threshold that let process first the critical events
    #if (XF_CRITICAL_QUEUE != 0)
        uint8_t	_priorityCounter;                       ///< \brief Count the number of critical events handled while normal events are waiting
        queue<IXFEvent*, XF_QUEUE_SIZE> _criticalEvents;///< \brief Queue holding critical events waiting to get dispatched.  
    #endif  // XF_CRITICAL_QUEUE
    queue<IXFEvent *, XF_QUEUE_SIZE> _events;           ///< \brief Queue holding events waiting to get dispatched.

    // Methods
    /************************************************************************************************************//**
     * \brief   Main loop of the thread. Implements event loop processing.
     ****************************************************************************************************************/
    void execute(const bool execOnce = false);

    /************************************************************************************************************//**
     * \brief   Returns pointer to time-out manager.
     ****************************************************************************************************************/
    //XFTimeoutManager * getTimeoutManager() const { return XFTimeoutManager::getInstance(); }

    /************************************************************************************************************//**
     * \brief   Dispatches the event to the corresponding behavioural part. 
     *
     * Dispatches the event to the corresponding behavioural part. For example the state machine which should process 
     * the event.
     *
     * \param   pEvent  The event to dispatch
     ****************************************************************************************************************/
    void dispatchEvent(IXFEvent * pEvent) const;
};

/** \} */
