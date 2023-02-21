/* Copyright (c) 2021, HES-SO Valais
 * All rights reserved.
 */
#ifndef MDW_BUTTON_CONFIG_H
#define MDW_BUTTON_CONFIG_H

//#include <assert-config.h>

// <<< Use Configuration Wizard in Context Menu >>>\n


//==========================================================
// <h> Button defines

//==========================================================
// <o> Sense Setup  - Button sensing mechanism
// <i> Enumerator used for selecting the pin to sense high or low level on the pin input.
// <i> Avialable values:
// <i> - Disable : GPIO_PIN_CNF_SENSE_Disabled (0UL) (default)
// <i> - High level: GPIO_PIN_CNF_SENSE_High (2UL)
// <i> - Low level: GPIO_PIN_CNF_SENSE_Low (3UL)
// <i> Sense on GPIO are globally disabled

// <0=> Disable
// <1=> High level
// <2=> Low level

#ifndef APP_BUTTON_SENSE_SETUP
    #define APP_BUTTON_SENSE_SETUP          0
    #if (APP_BUTTON_SENSE_SETUP == 0)
        #define BUTTON_SENSE_SETUP          NRF_GPIO_PIN_NOSENSE
    #elif (APP_BUTTON_SENSE_SETUP == 1)
        #define BUTTON_SENSE_SETUP          NRF_GPIO_PIN_SENSE_HIGH
    #else
        #define BUTTON_SENSE_SETUP          NRF_GPIO_PIN_SENSE_LOW
    #endif
#endif


//==========================================================
// <o> Timer tick for Button delay timer [ms]
// <i> Standard delay used by the Button's controller to detect a double or a tripple click

#ifndef APP_BUTTON_MS_TIMER_TICK
    #define APP_BUTTON_MS_TIMER_TICK        100
#endif


//==========================================================
// <o> Button Debouncer delay [ms] - Set the debounce time

#ifndef APP_BUTTON_DEBOUNCE_MS_DELAY
    #define APP_BUTTON_DEBOUNCE_MS_DELAY    50
#endif


//==========================================================
// <o> Button Double Click delay [ms]
// <i> Set the delay to be consider for a double click on a button [ms]

#ifndef APP_BUTTON_DBL_CLICK_MS_DELAY
    #define APP_BUTTON_DBL_CLICK_MS_DELAY   400
#endif


//==========================================================
// <o> SINGLE CLICK delay [ms]
// <i> If the duration of a button's pressure is below this value, it is considered as a `SINGLE CLICK`

#ifndef APP_BUTTON_CLICK_MS_DELAY
    #define APP_BUTTON_CLICK_MS_DELAY       1000
#endif


//==========================================================
// <o> LONG PRESS delay [ms]
// <i> If the button's pressure lasts between `APP_BUTTON_CLICK_MS_DELAY` and this value (`APP_BUTTON_LONG_PRESS_MS_DELAY`), 
// <i> it's considered as a `LONG PRESS`. If the pressure lasts more than this value, it's a `VERY LONG PRESS` 
#ifndef APP_BUTTON_LONG_PRESS_MS_DELAY
    #define APP_BUTTON_LONG_PRESS_MS_DELAY  3000
#endif


//==========================================================
// </h>


//==========================================================
// <o> Maximum number of Button's state observers.

#ifndef APP_BUTTON_MAX_OBSERVERS
    #define APP_BUTTON_MAX_OBSERVERS        1
#endif




#endif // MDW_BUTTON_CONFIG_H
