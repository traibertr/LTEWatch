/* Copyright (C) MSE Lausanne Hes-so VAUD/WAADT, Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2023
 * Modified by Tristan Traiber
 * All rights reserved.
 */
#ifndef MDW_GPIO_CONFIG_H
#define MDW_GPIO_CONFIG_H

#include "platform-config.h"
#include <drivers/gpio.h>

//==========================================================
// <h> GPIO defines

#define GPIO_DEFAULT_PRIORITY 4

//==========================================================
// <o> Sense Setup  - GPIO sensing mechanism
// <i> Enumerator used for selecting the pin to sense high or low level on the pin input.
// <i> Avialable values:
// <0=> Disable
// <1=> High level
// <2=> Low level

#define GPIO_NOSENSE 0

#ifndef APP_GPIO_SENSE_SETUP
    #define APP_GPIO_SENSE_SETUP            1
    #if (APP_GPIO_SENSE_SETUP == 0)
        #define GPIO_SENSE_SETUP            GPIO_NOSENSE
    #elif (APP_GPIO_SENSE_SETUP == 1)
        #define GPIO_SENSE_SETUP            GPIO_ACTIVE_HIGH
    #else
        #define GPIO_SENSE_SETUP            GPIO_ACTIVE_LOW
    #endif
#endif

//==========================================================
// <o> GPIO pull configuration
// <i> Enumerator used for selecting the pin to be pulled down or up at the time of pin configuration.
// <i> Avaialble setup:
// <0=> No pull
// <1=> Pull down
// <2=> Pull up

#define GPIO_NO_PULL 0

#ifndef APP_GPIO_PULL_SETUP
    #define APP_GPIO_PULL_SETUP             2
    #if (APP_GPIO_PULL_SETUP == 0)
        #define GPIO_PULL_SETUP             NRF_GPIO_PIN_NOPULL
    #elif (APP_GPIO_PULL_SETUP == 1)
        #define GPIO_PULL_SETUP             NRF_GPIO_PIN_PULLDOWN
    #else
        #define GPIO_PULL_SETUP             NRF_GPIO_PIN_PULLUP
    #endif
#endif

//==========================================================
// <o> GPIO connect or disconnect
// <i> Enumerator used for selecting the pin is connected or disconnected
// <i> Avaialble setup:
// <0=> No pull
// <1=> Pull down
// <2=> Pull up

#define GPIO_CONNECTED 0

#ifndef APP_GPIO_CONNECT_SETUP
    #define APP_GPIO_CONNECT_SETUP              1
    #if (APP_GPIO_CONNECT_SETUP == 0)
        #define GPIO_CONNECT_SETUP              0
    #elif (APP_GPIO_CONNECT_SETUP == 1)
        #define GPIO_CONNECT_SETUP              GPIO_CONNECTED
    #else
        #define GPIO_PULL_SETUP                 GPIO_DISCONNECTED
    #endif
#endif

//==========================================================
// <o> GPIO drive configuration
// <i> Enumerator used for selecting the output pin drive mode.
// <i> Avaialble setup:
// <0=> No drive (default)
// <1=> GPIO output in open drain mode (wired AND)
//       -----------------------------------------------------------------------------------
//      | ‘Open Drain’ mode also known as ‘Open Collector’ is an output configuration which |
//      |  behaves like a switch that is either connected to ground or disconnected.        |
//       -----------------------------------------------------------------------------------
// <2=> GPIO output in open source mode (wired OR)
//       -------------------------------------------------------------------------------------
//      | ‘Open Source’ is a term used by software engineers to describe output mode opposite | 
//      |  to ‘Open Drain’. It behaves like a switch that is either connected to power supply |
//      |  or disconnected. There exist no corresponding hardware schematic and the term is   |
//      |  generally unknown to hardware engineers.                                           |
//       -------------------------------------------------------------------------------------

#define GPIO_DEFAULT_DRIVE 0

#ifndef APP_GPIO_DRIVE_SETUP
    #define APP_GPIO_DRIVE_SETUP            0
    #if (APP_GPIO_DRIVE_SETUP == 0)
        #define GPIO_DRIVE_SETUP            GPIO_DEFAULT_DRIVE
    #elif (APP_GPIO_DRIVE_SETUP == 1)
        #define GPIO_DRIVE_SETUP            GPIO_OPEN_DRAIN
    #else
        #define GPIO_DRIVE_SETUP            GPIO_OPEN_SOURCE
    #endif
#endif

//==========================================================
// </h>


//==========================================================
// <h> GPIO Controller defines

//==========================================================
// <o> Maximum number of GPIOs used.
// <i> Maximum number of users of the GPIOTE handler.

#ifndef APP_GPIOTE_MAX_USERS
    #define APP_GPIOTE_MAX_USERS            5
    #define NB_MAX_GPIOS                    MAX(APP_GPIOTE_MAX_USERS, BUTTONS_NUMBER)
#endif

//==========================================================
// <o> Maximum number of GPIO state observers.

#ifndef APP_GPIO_MAX_OBSERVERS
    #define APP_GPIO_MAX_OBSERVERS          NB_MAX_GPIOS
#endif


//==========================================================
// <o> GPIOs delay in [ms]
// <i> Standard delay used by the GPIO's controller to consider an effective state's change

#ifndef APP_GPIO_MS_DELAY
    #define APP_GPIO_MS_DELAY               50
#endif


//==========================================================
// <o> Timer tick for GPIO debouncer [ms]
// <i> Standard delay used by the GPIO's controller to consider an effective state's change

#ifndef APP_GPIO_HANDLER_MS_TIMER_TICK
    #define APP_GPIO_HANDLER_MS_TIMER_TICK  20
#endif

//==========================================================
// </h>

//==========================================================
// <e> GPIO_INIT_AS_INPUT - GPIOs initialization process
// <i> If `true`, all GPIOs are initialized as input without any pool.
// <i> If `false`, they are set as output


#ifndef GPIO_INIT_AS_INPUT
    #define GPIO_INIT_AS_INPUT            0
#endif

//==========================================================
// <e> GPIO_INIT_OUT_AND_IN - GPIOs initialization process
// <i> If `true`, all GPIOs are, at initialization, set first as clearded output and then as disconnected input.
// <i> If `false`, they are only set as input disconnected


#ifndef GPIO_INIT_OUT_AND_IN
    #define GPIO_INIT_OUT_AND_IN            0
#endif


//==========================================================
// <o> GPIO_INIT_OUT_AND_IN_DELAY - Delay [ms]
// <i> Delay in [ms] to wait before setting all GPIOs as disconnected inputs

#ifndef GPIO_INIT_OUT_AND_IN_DELAY
    #define GPIO_INIT_OUT_AND_IN_DELAY      1000
#endif

//==========================================================
// <e> GPIO debug on a specific pin
// <i> Enable or disable the DEBUG feature on a specific pin number.

#ifndef APP_DEBUG_GPIO_PIN_XX
    #define APP_DEBUG_GPIO_PIN_XX           0
#endif


//==========================================================
// <o> GPIO debug pin number
// <i> Defines the PIN number de enable debug on...


#ifndef APP_DEBUG_PIN_NUMBER
    #define APP_DEBUG_PIN_NUMBER            10
#endif

//==========================================================
// </e>

#endif // MDW_GPIO_CONFIG_H
