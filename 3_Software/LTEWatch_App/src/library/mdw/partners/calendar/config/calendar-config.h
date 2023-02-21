/* Copyright (c) 2021, HES-SO Valais
 * All rights reserved.
 */
#pragma once

// ---------------------------------------------------------
// Calendar related
#define CALENDAR_TICK_COUNTER_VALUE         k_ms_to_ticks_ceil32(1000)

// ---------------------------------------------------------
// ISubject related
#define CALENDAR_MAX_OBSERVERS              8

//==========================================================
// <h> Calendar defines


//==========================================================
// <o> Amount of date and time 
// <i> The Calendar Module can handle more than one date and time.
// <i> All Date and Time sets are baesd on UTC.

#ifndef APP_NB_DATE_TIME_MAX
    #define APP_NB_DATE_TIME_MAX            3
#endif

//==========================================================
// </h>

