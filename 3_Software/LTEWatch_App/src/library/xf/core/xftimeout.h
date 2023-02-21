/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    xftimeout.h
 *
 * \addtogroup XF eXecution Framework
 * \{
 *
 * \class   XFTimeout
 * \brief   Interface to be implemented by time-outs.
 *
 * Interface to be implemented by time-outs. Defined methods are needed by the framework classes 
 * (see also: \ref XFTimeoutManager).
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
#if (XF_TM_USE_MEMORYPOOL_FOR_TIMEOUTS != 0)
   #include "memorypoolmacros.h"
#endif // XF_TM_USE_MEMORYPOOL_FOR_TIMEOUTS


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class XFTimeout : public IXFEvent
{
    friend class XFTimeoutManager;

public:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* CONSTRUCTOR SECTION                                                                                          */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    XFTimeout(uint8_t id, uint32_t interval, IXFReactive * pBehaviour);
    virtual ~XFTimeout();

    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC DECLARATION SECTION                                                                                   */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
     * \brief   Operator checks if #_pBehaviour and #_id are equal.
     *
     * Operator checks if #_pBehaviour and #_id are equal and returns true if so. All other attributes get no
     * attention.
     ****************************************************************************************************************/
    bool operator ==(const XFTimeout & timeout) const;

#if (XF_TM_USE_MEMORYPOOL_FOR_TIMEOUTS != 0)
    /************************************************************************************************************//**
     * \brief   Create custom new and delete operators using memory pool
     ****************************************************************************************************************/
    DECLARE_MEMORYPOOL_OPERATORS(XFTimeout)
#endif // XF_TM_USE_MEMORYPOOL_FOR_TIMEOUTS

    /************************************************************************************************************//**
     * \brief   Returns the interval value set for the current timeout
     ****************************************************************************************************************/
    //inline uint32_t interval() const        { return _interval; }

    /************************************************************************************************************//**
     * \brief   Returns the timer tick's counter computed from the interval of the current timeout
     ****************************************************************************************************************/
    inline uint32_t ticks() const           { return _ticks; }

    /************************************************************************************************************//**
     * \brief   Returns the relative tick's counter coming from the gap between the current timeout and hte previous
     *          one.
     ****************************************************************************************************************/
    inline uint32_t relTicks() const        { return _relTicks; }

    /************************************************************************************************************//**
     * \brief   Returns the difference between the relative Tick's number and the one it should be for very short timer
     ****************************************************************************************************************/
    inline uint32_t minGapOverflow() const  { return _minGapOverflow; }

    /************************************************************************************************************//**
     * \brief   Returns the difference between the relative Tick's number and the one it should be for very short timer
     ****************************************************************************************************************/
    inline void clearMinGapOverflow()       { _minGapOverflow = 0; }

    /************************************************************************************************************//**
     * \brief   Increments the ticks value with the given parameter
     ****************************************************************************************************************/
    void incrRelativeTicks(uint32_t toAdd);

    /************************************************************************************************************//**
     * \brief   Decrement the ticks value...
     ****************************************************************************************************************/
    void decrRelativeTicks(uint32_t toSubstract);

protected:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PROTECTED DECLARATION SECTION                                                                                */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    // Attributes
    uint32_t _ticks;             ///< \brief Duration of the timeout expressed in number of Tick counter.
    uint32_t _relTicks;          ///< \brief Used by the \ref XFTimeoutManager "TimeoutManager"  to calculate remaining time. Can get negative!
    uint32_t _minGapOverflow;    ///< \brief Stores the differnce between _relticks and XF_TIMER_MIN_TIMEOUT_TICKS, if the _relTicks is lower than this limit.

    // Methods
    /************************************************************************************************************//**
     * \brief   Protected Constructor
     *
     * \warning Creating copies of XFTimeout is not allowed.
     ****************************************************************************************************************/
    XFTimeout(const XFTimeout & timeout);

    /************************************************************************************************************//**
     * \brief   Operator overload for XFTimeout
     *
     * \warning Creating copies of XFTimeout is not allowed.
     ****************************************************************************************************************/
    const XFTimeout & operator = (const XFTimeout & timeout);
};

/** \} */
