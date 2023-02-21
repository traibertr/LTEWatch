/* ******************************************************************************************************************
 * Copyright (C) Hes-so VD, MSE Lausanne, EIE. 2022
 * Created by Tristan Traiber
 * Based on code by Patrice Rudaz
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "xf_adapter.h"
#include <zephyr.h>
#include "debug-config.h"
#include "factory.h"

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#if (DEBUG_XF_ADAPTER_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(xf_adapter);

    #define XFA_DEBUG(args...)              LOG_DBG(args) 
    #define XFA_ERROR(args...)              LOG_ERR(args) 
#else
    #define XFA_DEBUG(...)                  {(void) (0);}
    #define XFA_ERROR(...)                  {(void) (0);}
#endif

/* **************************************************************************************************************** */
/* --- Extern "C" Section ---                                                                                       */
/*     Callback's section                                                                                           */
/* **************************************************************************************************************** */

//----- Timout Implementation -----------------------------------------------------------------------------------------

void xfTimerTimeout(struct k_timer *timer)
    {
        _is_xfTimer_started = false;

        XFTimeoutManager::getInstance()->timeout();
    }

/* **************************************************************************************************************** */
/* Declaration section                                                                                              */
/* **************************************************************************************************************** */

bool                    _is_xfTimer_started     = false;
k_ticks_t               _xf_timeout_ticks       = 0;
static struct k_timer   _xfTimerId;           //xfTimeout kernel timer declaration
static uint32_t         startTime;

/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------

void xfTimerInit()
{
    // Init xfTimeout timer  
    k_timer_init(&_xfTimerId, xfTimerTimeout, NULL); 
    XFA_DEBUG("%s","Timer init successfully");
    XFA_DEBUG("Hardware n. ticks per second : %u",sys_clock_hw_cycles_per_sec());
    startTime = 0;
}

// ------------------------------------------------------------------------------------------------------------------
void xfTimerStart(unsigned int timerTicks)
{
    _xf_timeout_ticks = timerTicks;
    // Check timerTicks
    if (timerTicks == 0)
    {
        XFA_ERROR("%s","TimerTicks is null => calling XFTimeoutManager::getInstance()->timeout() directly ...");
        XFTimeoutManager::getInstance()->timeout();
        return;
    }

    if (!_is_xfTimer_started)
    {
        // Register XFTimeoutManager instance inside the timer (if needed)
        k_timer_user_data_set(&_xfTimerId,static_cast<void *>(XFTimeoutManager::getInstance()));
        k_timer_start(&_xfTimerId, K_TICKS(static_cast<k_ticks_t>(timerTicks)), K_FOREVER);

        _is_xfTimer_started = true;
        XFA_DEBUG("Start XF Timer -> timerTicks: %u", timerTicks);
    }
    else
    {
        XFA_ERROR("%s","Failed : XF Timer already started");
    }
}

// ------------------------------------------------------------------------------------------------------------------
void xfTimerStop()
{
    if (_is_xfTimer_started)
    {
        // Stop xfTimeout timer
        k_timer_stop(&_xfTimerId);
        _is_xfTimer_started = false;
        XFA_DEBUG("Stop XF Timer -> timerTicks");
    }
}

// ------------------------------------------------------------------------------------------------------------------
unsigned int xfTimerGetElapsedTime()
{
	// Get the elapsed time of the actuall TIMER in ticks
	// Convert TIMER elapsed ticks to tenths of milliseconds and return that value
	return (timerTick2Interval(xfTimerGetElapsedTimerTicks())/10);
}

// ------------------------------------------------------------------------------------------------------------------
unsigned int xfTimerGetElapsedTimerTicks()
{

    // Get time remaining before a timer next timeout in system ticks
    // If the timer is not running, it returns zero.
    k_ticks_t remaining_ticks = k_timer_remaining_ticks(&_xfTimerId);

    // Compute and returns the difference between timeout value [system ticks] and remaining value [system ticks]
    return static_cast<unsigned int>(_xf_timeout_ticks) - remaining_ticks;
}

// ------------------------------------------------------------------------------------------------------------------
unsigned int interval2TimerTick(unsigned int interval)
{
    // Convert the given interval (ms) value to XF's timer tick counter
    // Computes result in 32 bit precision. Rounds to the nearest output unit
    return static_cast<unsigned int>(k_ms_to_ticks_near32(interval));
}

// ------------------------------------------------------------------------------------------------------------------
unsigned int timerTick2Interval(unsigned int ticks)
{
    // Convert the given timer's ticks value to the corresponding interval in ms
    // Computes result in 32 bit precision. Rounds to the nearest output unit.
    return static_cast<unsigned int>(k_cyc_to_ms_near32(ticks));
}

// ------------------------------------------------------------------------------------------------------------------
void xfHookOnIdle()
{
    Factory::picoXF()->execute();

    uint32_t actualTime = k_uptime_get_32();

    if (actualTime - startTime >= 100)
    {
        lv_task_handler();
        startTime = k_uptime_get_32();
    }
}

// ------------------------------------------------------------------------------------------------------------------
#if (XF_HOOK_PREEXECUTE_ENABLED != 0)
void xfPreExecuteHook()
{
    //Factory::picoXF()->execute();
}
#endif // XF_HOOK_PREEXECUTE_ENABLED

// ------------------------------------------------------------------------------------------------------------------
#if (XF_HOOK_POSTEXECUTE_ENABLED != 0)
void xfPostExecuteHook()
{
    //Factory::picoXF()->execute();
}
#endif // XF_HOOK_POSTEXECUTE_ENABLED
