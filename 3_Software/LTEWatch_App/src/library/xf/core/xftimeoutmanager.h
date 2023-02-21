/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    xftimeoutManager.h
 *
 * \addtogroup XF eXecution Framework
 * \{
 *
 * \class   XFTimeoutManager
 * \brief   The TimeoutManager is responsible to handle time-outs used in the eXecution Framework.
 *
 * The TimeoutManager is responsible to handle time-outs used in state machines. It decrements them accordingly and 
 * inject them back to the state machine when time-outed.
 *
 * <b>Defines:</b>
 * - XF_TM_USE_ISLIST:      Set to 1 to let the XFTimeoutManager use a IsList (interrupt safe list) class to queue 
 *                          timeouts. If not defined or set to 0 a std::list is used.
 * - XF_TM_STATIC_INSTANCE: Set to 1 to statically allocate the instance for the XFTimeoutManager.
 *                          If not defined or set to 0 the instance is allocated on the heap.
 *
 * <b>Requirements:</b>
 * - Implements the Singleton pattern
 * - Handle time-outs (hold, decrement, re-inject)
 *
 * Initial author: Thomas Sterren
 * Creation date: 2010-11-26
 *
 * \author  Patrice Rudaz
 * \date    July 2018
 ********************************************************************************************************************/
#pragma once

#if defined(__cplusplus)
/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include "xf_adapter.h"
#include "xftimeout.h"
#if (XF_TM_USE_ISLIST != 0)
    #include "islist.hpp"
#else
    //#include <list.h>
    #include <list>
#endif


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* NAMESPACE Declaration                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#if (XF_TM_USE_ISLIST == 0)
using namespace std;
#endif


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class XFTimeoutManager
{
#if (XF_TM_USE_ISLIST != 0)
    typedef IsList<XFTimeout *> TimeoutList;
#else
    typedef list<XFTimeout *> TimeoutList;
#endif
public:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* CONSTRUCTOR SECTION                                                                                          */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    XFTimeoutManager();
    virtual ~XFTimeoutManager();


    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC DECLARATION SECTION                                                                                   */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
     * \brief   Returns a pointer to the single instance of XFTimeoutManager.
     ****************************************************************************************************************/
    static XFTimeoutManager* getInstance();

    /************************************************************************************************************//**
     * \brief   Starts the time-out manager.
     ****************************************************************************************************************/
    void start();

    /************************************************************************************************************//**
     * \brief   Adds a time-out to the time-out manager.
     *
     * \param   timeoutId   The time-out id known by the reactive parameter. Is needed by the reactive part to
     *                      uniquely identify the time-out.
     * \param   interval    The time in milliseconds or timer's ticks unit to wait until the time-out expires.
     * \param   pReactive   The reactive instance where to inject the time-out when it time-outs.
     ****************************************************************************************************************/
     void scheduleTimeout(uint8_t timeoutId, unsigned int interval, IXFReactive* pReactive);

    /************************************************************************************************************//**
     * \brief   Removes all time-outs corresponding the given parameters.
     ****************************************************************************************************************/
    void unscheduleTimeout(uint8_t timeoutId, IXFReactive* pReactive);

    /************************************************************************************************************//**
     * \brief   Called after expiration of the first timeout in list.
     *
	 * When this method is called, the first timeout in the list is time-outed and it has to be injected to the 
     * respective behavior.
     ****************************************************************************************************************/
    void timeout();

protected:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PROTECTED DECLARATION SECTION                                                                                */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    // Attributes
    static XFTimeoutManager*    _pInstance;     ///< Pointer to single instance of XFTimeoutManager.
    TimeoutList                 _timeouts;      ///< Container holding time-outs to manage            

    // Methods
    /************************************************************************************************************//**
     * \brief   Adds the time-out to #_timeouts.
     ****************************************************************************************************************/
    void addTimeout(XFTimeout* pNewTimeout);

    /************************************************************************************************************//**
     * \brief   Removes all time-outs corresponding the given parameters.
     ****************************************************************************************************************/
    void removeTimeouts(uint8_t timeoutId, IXFReactive* pReactive);

    /************************************************************************************************************//**
     * \brief   Returns the time-out back to the queue of the thread executing the behavioural instance.
     ****************************************************************************************************************/
    void returnTimeout(XFTimeout* pTimeout);

private:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PRIVATE DECLARATION SECTION                                                                                  */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    // Method
    void _startXfTimer();
};
#endif // C++

/** \} */
