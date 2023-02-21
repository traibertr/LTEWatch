/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "button.h"

#include <nrf.h>
#include <string>

#include "debug-config.h"

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#define DEBUG_BUTTON_ENABLE                 0
#if (DEBUG_BUTTON_ENABLE == 1) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(button, LOG_LEVEL_DBG);
    #define BTN_ERROR(args...)              LOG_ERR(args)
    #define BTN_DEBUG(args...)              LOG_DBG(args)
    #define BTN_INFO(args...)               LOG_INF(args)
    #define BTN_VERBOSE(args...)            LOG_DBG(args)
#else
    #define BTN_ERROR(...)                  {(void) (0);}
    #define BTN_DEBUG(...)                  {(void) (0);}
    #define BTN_INFO(...)                   {(void) (0);}
    #define BTN_VERBOSE(...)                {(void) (0);}
#endif

    
/* **************************************************************************************************************** */
/* Namespace Declaration                                                                                            */
/* **************************************************************************************************************** */
using gpio::Button;


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------
void Button::initialize(uint8_t pinNumber, ButtonId::eButtonId buttonId, nrf_gpio_pin_pull_t pullSetup)
{
    // Configure GPIO
    _gpio.initialize(pinNumber, ButtonId::toGpioId(buttonId), pullSetup);
    
    _buttonId   = buttonId;
    _state      = Button::ButtonReleased;
    _lastState  = _state;
    BTN_DEBUG("PIN %d set as %s.", pinNumber, toString());
}

// ------------------------------------------------------------------------------------------------------------------
bool Button::setState(Button::eButtonState state)
{
    if (state != _state)
    {
        _lastState = _state;
        _state     =  state;
        BTN_INFO("%s (PIN %02d) -> `%s`", toString(), pinNumber(), stateToString(state));
        _notifyObservers(_state);

        // for SAV
        switch((uint8_t) _state)
        {
            case ButtonSingleClick:
                _persistentParams.incrSingleClickCounter();
                break;

            case ButtonDoubleClick:
                _persistentParams.incrDoubleClickCounter();
                break;

            case ButtonTripleClick:
                _persistentParams.incrTripleClickCounter();
                break;

            case ButtonLongPress:
                _persistentParams.decrSingleClickCounter();
                _persistentParams.incrLongPressCounter();
                break;

            case ButtonVeryLongPress:
                _persistentParams.decrLongPressCounter();
                _persistentParams.incrVeryLongPressCounter();
                break;

            case ButtonDoubleLongPress:
                _persistentParams.decrDoubleClickCounter();
                _persistentParams.incrDoubleLongPressCounter();
                break;

            case ButtonTripleLongPress:
                _persistentParams.incrDoubleLongPressCounter();
                _persistentParams.incrTripleLongPressCounter();
                break;

            case ButtonReleased:
                _persistentParams.incrReleaseCounter();
                break;
        }
        BTN_VERBOSE("     Single click counter for %s: %d", toString(), _persistentParams.singleClickCounter());
        BTN_VERBOSE("     Double click counter for %s: %d", toString(), _persistentParams.doubleClickCounter());
        BTN_VERBOSE("     Triple click counter for %s: %d", toString(), _persistentParams.tripleClickCounter());
        BTN_VERBOSE("       Long press counter for %s: %d", toString(), _persistentParams.longPressCounter());
        BTN_VERBOSE("  Very Long press counter for %s: %d", toString(), _persistentParams.veryLongPressCounter());
        BTN_VERBOSE("Double Long press counter for %s: %d", toString(), _persistentParams.doubleLongPressCounter());
        BTN_VERBOSE("Triple Long press counter for %s: %d", toString(), _persistentParams.tripleLongPressCounter());
        BTN_VERBOSE("          Release counter for %s: %d", toString(), _persistentParams.releaseCounter());
        return true;
    }
    BTN_VERBOSE("%s is already in `%s` state", toString(), stateToString(state));
    return false;
}

#if (USE_FLASH != 0)
// ------------------------------------------------------------------------------------------------------------------
uint32_t* Button::save(uint16_t* size)
{
    *size = sizeof(ButtonPersistor) / 4;
    BTN_DEBUG("[toto] save() > saving parameters for %s: single click = %d, double click = %d, triple click = %d, long press = %d, very-long press = %d,  double-long press = %d, triple-long press = %d, release = %d",
              toString(),
              _persistentParams.singleClickCounter(), 
              _persistentParams.doubleClickCounter(), 
              _persistentParams.tripleClickCounter(),
              _persistentParams.longPressCounter(), 
              _persistentParams.veryLongPressCounter(), 
              _persistentParams.doubleLongPressCounter(), 
              _persistentParams.tripleLongPressCounter(), 
              _persistentParams.releaseCounter());
    return _persistentParams.data();
}

// ------------------------------------------------------------------------------------------------------------------
void Button::restore(uint32_t* data)
{
    memcpy(_persistentParams.data(), data, sizeof(ButtonPersistor));
    BTN_DEBUG("restoring parameters for %s: single click = %d, double click = %d, ...",
              toString(),
              _persistentParams.singleClickCounter(), _persistentParams.doubleClickCounter());
}
#endif // #if (USE_FLASH != 0)

/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void Button::_notifyObservers(Button::eButtonState state)
{
    for(uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            switch(state)
            {
                case ButtonReleased:        observer(i)->onButtonReleased(this);        break;
                case ButtonSingleClick:     observer(i)->onButtonSingleClick(this);     break;
                case ButtonDoubleClick:     observer(i)->onButtonDoubleClick(this);     break;
                case ButtonTripleClick:     observer(i)->onButtonTripleClick(this);     break;
                case ButtonLongPress:       observer(i)->onButtonLongPress(this);       break;
                case ButtonVeryLongPress:   observer(i)->onButtonVeryLongPress(this);   break;
                case ButtonDoubleLongPress: observer(i)->onButtonDoubleLongPress(this); break;
                case ButtonTripleLongPress: observer(i)->onButtonTripleLongPress(this); break;
            }
        }
        else
        {
            BTN_ERROR("Observer(%d) is NULL ! (%s, state `%s`)", i, toString(), stateToString(state));
        }
    }
}
