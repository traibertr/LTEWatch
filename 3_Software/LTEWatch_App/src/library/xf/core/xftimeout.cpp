/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz & Thomas Sterren
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "xftimeout.h"
#include "xf_adapter.h"
#include "xf-config.h"

/* **************************************************************************************************************** */
/* Declaration section                                                                                              */
/* **************************************************************************************************************** */
#if (XF_TM_USE_MEMORYPOOL_FOR_TIMEOUTS != 0)
    IMPLEMENT_MEMORYPOOL_OPERATORS(XFTimeout)
#endif // XF_TM_USE_MEMORYPOOL_FOR_TIMEOUTS

    
/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#define DEBUG_XFTIMEOUT_ENABLE              0
#if (DEBUG_XFTIMEOUT_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(xf_timeout, CONFIG_XF_TIMEOUT_MANAGER_LOG_LEVEL);
    #define XFT_DEBUG(args...)              LOG_DBG(args) 
#else
    #define XFT_DEBUG(...)                  {(void) (0);}
#endif


/* **************************************************************************************************************** */
/* CONSTRUCTOR                                                                                                      */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
XFTimeout::XFTimeout(uint8_t id, uint32_t interval, IXFReactive* pBehaviour)
    : IXFEvent(IXFEvent::Timeout, id, pBehaviour), _minGapOverflow(0)
{
    uint32_t ticks = 0;
    if (interval > XF_CODE_COMPENSATION_TICKS)
        ticks = interval - XF_CODE_COMPENSATION_TICKS;

    _ticks              = ticks;
    if (_ticks > XF_TIMER_MAX_COUNTER_VAL)
    {
        _relTicks       = XF_TIMER_MAX_COUNTER_VAL;
        XFT_DEBUG("ticks greater than RTC counter capability (%lu)\r\n", xf_timer_max_counter_value);
    }
    else if (_ticks < XF_TIMER_MIN_TIMEOUT_TICKS)
    {
        _relTicks       = XF_TIMER_MIN_TIMEOUT_TICKS;
        _minGapOverflow = _relTicks - _ticks;
        XFT_DEBUG("ticks less than RTC counter min capability (%lu)\r\n", XF_TIMER_MIN_TIMEOUT_TICKS);
    }
    else
    {
        _relTicks       = (uint32_t) (_ticks & 0x00000000ffffffff);
    }
    XFT_DEBUG("id: %d; _interval: %8d, _ticks %10lu, _relTicks: %10lu, _minGapOverflow: %10lu\r\n", 
              id, _ticks, interval, _relTicks, _minGapOverflow);
}

// ------------------------------------------------------------------------------------------------------------------
XFTimeout::XFTimeout(const XFTimeout &timeout)
    : IXFEvent(IXFEvent::Timeout, timeout._id, timeout._pBehaviour)
{
    //__ASSERT(false);
}

// ------------------------------------------------------------------------------------------------------------------
XFTimeout::~XFTimeout()
{}

/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void XFTimeout::incrRelativeTicks(uint32_t toAdd)
{
    XFT_DEBUG("IN  -> _relTicks: %10lu & _minGapOverflow:  %10lu (toAdd: %10lu)\r\n", _relTicks, _minGapOverflow, toAdd);
    if (toAdd > 0)
    {
        const long ticks    = (long) _relTicks + (long) toAdd - (long) _minGapOverflow - XF_CODE_COMPENSATION_TICKS;
        _minGapOverflow     = 0;
        if (ticks > (long) XF_TIMER_MAX_COUNTER_VAL)
        {
            _relTicks       = XF_TIMER_MAX_COUNTER_VAL;
        }
        else if (ticks >= XF_TIMER_MIN_TIMEOUT_TICKS)
        {
            _relTicks       = (uint32_t) ticks;
        }
        else
        {
            _relTicks       = XF_TIMER_MIN_TIMEOUT_TICKS;
            _minGapOverflow = (uint32_t) (XF_TIMER_MIN_TIMEOUT_TICKS - ticks);
        }
    }
    XFT_DEBUG("OUT -> _relTicks: %10lu & _minGapOverflow:  %10lu\r\n", _relTicks, _minGapOverflow);
}
    
// ------------------------------------------------------------------------------------------------------------------
void XFTimeout::decrRelativeTicks(uint32_t toSubstract)
{
    XFT_DEBUG("IN  -> _relTicks: %10lu & _minGapOverflow:  %10lu (toSubstract: %10lu)\r\n", _relTicks, _minGapOverflow, toSubstract);
    if (toSubstract > 0)
    {
        const long ticks    = (long) _relTicks - (long) toSubstract - (long) _minGapOverflow - XF_CODE_COMPENSATION_TICKS;
        if (ticks >= XF_TIMER_MIN_TIMEOUT_TICKS)
        {
            _relTicks       = (uint32_t) ticks;
            _minGapOverflow = 0;
        }
        else
        {
            _relTicks       = XF_TIMER_MIN_TIMEOUT_TICKS;
            _minGapOverflow = (uint32_t) (XF_TIMER_MIN_TIMEOUT_TICKS - ticks);
        }
    }
    XFT_DEBUG("OUT -> _relTicks: %10lu & _minGapOverflow:  %10lu\r\n", _relTicks, _minGapOverflow);
}
    
// ------------------------------------------------------------------------------------------------------------------
const XFTimeout &XFTimeout::operator = (const XFTimeout &timeout)
{
    //__ASSERT(false);
    return timeout;
}

// ------------------------------------------------------------------------------------------------------------------
bool XFTimeout::operator ==(const XFTimeout &timeout) const
{
    // Check behavior and timeout id attributes, if there are equal
    return (_pBehaviour == timeout._pBehaviour && getId() == timeout.getId()) ? true : false;
}
