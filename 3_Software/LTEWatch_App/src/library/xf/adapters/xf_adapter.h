/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    xf_adapter.h
 *
 * \addtogroup XF eXecution Framework
 * \{
 *
 * \brief   Definition file for xf_adapter.cpp
 *
 * This adapter is dedicated to the nRF52840 Nordic chip. This file take care of the hardware specification to match 
 * the resources needed by the XF for "nrf Connect SDK" and "Zephyr RTOS"
 *
 * Initial author:  Patrice Rudaz
 * Creation date:   2013-09-19
 *
 * Zephyr RTOS adapter version :
 * \author  Tristan Traiber
 * \date    Mai 2022
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include "xf-config.h"
#include "xftimeoutmanager.h"
#include <zephyr.h>

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Declaration section                                                                                              */
/*                                                                                                                  */
/* **************************************************************************************************************** */
    extern bool     _is_xfTimer_started;        ///< \brief To know if the XF timer is already running
    extern k_ticks_t _xf_timeout_ticks;           ///< \brief Stores the RTC value when the app timer is started
    
    /************************************************************************************************************//**
     * \brief   Definite values related to the Low power counter (RTC) based on 32 kHz frequency.
     ****************************************************************************************************************/
     const uint32_t   RTC_250_US      = k_us_to_ticks_near32(250);              ///< \brief This is exactly 
     const uint32_t   RTC_500_US      = k_us_to_ticks_near32(500);              ///< \brief This is exactly 
     const uint32_t   RTC_1_MS        = k_ms_to_ticks_near32(1);                ///< \brief This is exactly 
     const uint32_t   RTC_2_MS        = k_ms_to_ticks_near32(2);                ///< \brief This is exactly 
     const uint32_t   RTC_4_MS        = k_ms_to_ticks_near32(4);                //<  \brief This is exactly 
     const uint32_t   RTC_10_MS       = k_ms_to_ticks_near32(10);               ///< \brief This is exactly 
     const uint32_t   RTC_15_MS_625   = k_ms_to_ticks_near32(15.625);           ///< \brief This is exactly 
     const uint32_t   RTC_16_MS       = k_ms_to_ticks_near32(16);               ///< \brief This is exactly 
     const uint32_t   RTC_31_MS_25    = k_ms_to_ticks_near32(31.25);            ///< \brief This is exactly 
     const uint32_t   RTC_32_MS       = k_ms_to_ticks_near32(32);               ///< \brief This is exactly 
     const uint32_t   RTC_50_MS       = k_ms_to_ticks_near32(50);               ///< \brief This is exactly 
     const uint32_t   RTC_62_MS_5     = k_ms_to_ticks_near32(62.5);             ///< \brief This is exactly 
     const uint32_t   RTC_64_MS       = k_ms_to_ticks_near32(64);               ///< \brief This is exactly 
     const uint32_t   RTC_100_MS      = k_ms_to_ticks_near32(100);              ///< \brief This is exactly 
     const uint32_t   RTC_125_MS      = k_ms_to_ticks_near32(125);              ///< \brief This is exactly 
     const uint32_t   RTC_250_MS      = k_ms_to_ticks_near32(250);              ///< \brief This is exactly 
     const uint32_t   RTC_500_MS      = k_ms_to_ticks_near32(500);              ///< \brief This is exactly 
     const uint32_t   RTC_1000_MS     = k_ms_to_ticks_near32(1000);

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* PUBLIC DECLARATION SECTION                                                                                       */
/*                                                                                                                  */
/* **************************************************************************************************************** */
/****************************************************************************************************************//**
 * \brief   Initialize the timer needed by the XF.
 *
 * This method initializes the timer used by the XF to perform some time-out events. In this case,
 * we use an `app_timer` (Nordic SoftDevice) to realize the timer. This timer is based on a RTC in
 * in counter mode.
 *
 * The RTC will run off a low-frequency clock (LFCLK) running at 32.768 kHz. The COUNTER
 * resolution will therefore be 30.517 �s. For more information, see nRF51_Reference_manual.pdf
 ********************************************************************************************************************/
void xfTimerInit();

/****************************************************************************************************************//**
 * \brief   Starts the XF's timer.
 *
 * This method starts the XF's timer. As the resolution is about 30 �s, we have to set-up a trick to correct the 
 * deviation of the timer. The chosen solution is a kind of inhibition.
 *
 * The timer's frequency is 32.768 kHz. This means that we need 32.768 timer's tick to reach the millisecond, which 
 * is the base time we need for the XF. Now we have to choose between 32 or 33 for the value of the timer's counter: 
 * to be able to do some inhibition, we need a base time less than 1 ms and so we'll take 32 for the timer's counter. 
 * According to this decision, we can compute the deviation of time of the timer for 1 ms: 
 *
 *                                         1 [ms] - (32 / 32.768) = 0.0234375!
 *
 * Let's try with a time base of 2 ms: the counter's value will be choose between 65 and 66 (2 [ms] * 32.768 = 65.536). 
 * We'll take 65 and the deviation becomes: 2 [ms] - (65 / 32.768) = 0.00817! This seems to be better than the one 
 * with 1 ms as base time.
 *
 * Now we have to find when a tick must be inhibited to correct the time's deviation. This is done by resolving the 
 * equation `a * x = b * (x + 1)` where `a` is the base time and `b` is (tickCounter / 32.768). So for a 1 ms base 
 * time, we have to skip a timer's tick every 42 ticks and the deviation will be 7.8125 �s. For 2 ms base time, 
 * we'll skip a timer's tick every 121 ticks and the deviation will be of 4.3945 �s.
 *
 * \param   timerTicks    The amount of RTC ticks to wait before sending a timeout event...
 *
 * \see     xfTimerInit
 ********************************************************************************************************************/
void xfTimerStart(unsigned int timerTicks);

/****************************************************************************************************************//**
 * \brief   Stops the XF's timer.
 *
 * If the queue of TimeoutEvent is empty, we simply stops the timer. This can save current consumption.
 *
 * \see     xfTimerInit
 ********************************************************************************************************************/
void xfTimerStop();

/****************************************************************************************************************//**
 * \brief   Stops and restarts the XF's timer.
 *
 * This method stops the XF's timer (if necessary) and starts it right after with the given duration. 
 *
 * \param   duration    The amount of RTC ticks to wait before sending a timeout event...
 *
 * \see     xfTimerStart
 ********************************************************************************************************************/
inline void xfTimerRestart(unsigned int duration)       { xfTimerStop(); xfTimerStart(duration); }

/****************************************************************************************************************//**
 * \brief	Returns the elapsed time of actually running timer in tenths of milliseconds.
 ********************************************************************************************************************/
unsigned int xfTimerGetElapsedTime();

/****************************************************************************************************************//**
 * \brief	Returns the elapsed time of actually running timer in XF's timer ticks.
 ********************************************************************************************************************/
unsigned int xfTimerGetElapsedTimerTicks();

/****************************************************************************************************************//**
 * \brief   Convert the given interval value to XF's timer tick counter.
 *
 * \param   interval    The timeout to be converted given in [ms]
 *
 * \return  The amount of tick that the XF's timer has to count before firing a timeout event.
 ********************************************************************************************************************/
unsigned int interval2TimerTick(unsigned int interval);

/****************************************************************************************************************//**
 * \brief   Convert the given timer's ticks value to the corresponding interval in [ms].
 *
 * \param   ticks   The XF's timer tick counter to be converted in ms. This value must greater then 
 *                  XF_TIMER_MIN_TIMEOUT_TICKS and lower than XF_TIMER_MAX_COUNTER_VAL.
 *
 * \return  The interval in [ms] to wait before firing a timeout event.
 ********************************************************************************************************************/
unsigned int timerTick2Interval(unsigned int ticks);

/****************************************************************************************************************//**
 * \brief   Called whenever there are actually no more events to execute.
 ********************************************************************************************************************/
void xfHookOnIdle();

/****************************************************************************************************************//**
 * \brief   Called before dispatch of the next event.
 ********************************************************************************************************************/
#if (XF_HOOK_PREEXECUTE_ENABLED != 0)
    void xfPreExecuteHook();
#endif // XF_HOOK_PREEXECUTE_ENABLED

/****************************************************************************************************************//**
 * \brief   Called after dispatch of an event.
 ********************************************************************************************************************/
#if (XF_HOOK_POSTEXECUTE_ENABLED != 0)
    void xfPostExecuteHook();
#endif // XF_HOOK_POSTEXECUTE_ENABLED

/** \} */
