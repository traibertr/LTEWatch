/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "gpio.h"
#include "gpiohal.h"
#include <nrf.h>
#include "debug-config.h"

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#define DEBUG_GPIO_ENABLE                   0
#if (DEBUG_GPIO_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(gpio, LOG_LEVEL_DBG);
    #define GPIO_DEBUG(args...)             LOG_DBG(args) 
    #define GPIO_INFO(args...)              LOG_INF(args) 
    #define GPIO_ERROR(args...)             LOG_ERR(args) 
#else
    #define GPIO_DEBUG(...)                 {(void) (0);}
    #define GPIO_INFO(...)                  {(void) (0);}
    #define GPIO_ERROR(...)                 {(void) (0);}
#endif

/* **************************************************************************************************************** */
/* Namespace Declaration                                                                                            */
/* **************************************************************************************************************** */
using hal::GpioHal;
using gpio::Gpio;

    
/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void Gpio::initialize(uint8_t pinNumber, GpioId::eGpioId gpioId, nrf_gpio_pin_pull_t pullSetup)
{
    _gpioId         = gpioId;
    _pinNumber      = pinNumber;
    _pinMask        = 1 << pinNumber;
    _pullSetup      = pullSetup;
    _isDebouncing   = false;
    
    if (pullSetup == NRF_GPIO_PIN_PULLDOWN)
    {
        _state      = Gpio::GpioFallingEdge;
    }
    else if (pullSetup == NRF_GPIO_PIN_PULLUP)
    {
        _state      = Gpio::GpioRisingEdge;
    }
    else 
    {
        _state      = Gpio::UnknownGpioState;
    }
    _lastState      = _state;

    // Configure the GPIO as an input
    if (pullSetup == NRF_GPIO_PIN_PULLDOWN)
    {
        GPIO_DEBUG("%s (PIN %d) set as Input PullDown.", to_string(), _pinNumber);
        hal::GpioHal::cfgPinInputPullDown(_pinNumber);
    }
    else if (pullSetup == NRF_GPIO_PIN_PULLUP)
    {
        GPIO_DEBUG("%s (PIN %d) set as Input PullUp.", to_string(), _pinNumber);
        hal::GpioHal::cfgPinInputPullup(_pinNumber);
    }
    else
    {
        GPIO_DEBUG("%s (PIN %d) set as Input without any PullUp or PullDown.", to_string(), _pinNumber);
        hal::GpioHal::cfgPinInputNoPull(_pinNumber);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Gpio::setState(Gpio::eGpioState state)
{
    if (state != _state)
    {
        _lastState  = _state;
        _state      =  state;
        GPIO_INFO("%s Edge on %s with PIN %d (rtc counter: %d)", 
                  (state == GpioFallingEdge ? "Falling":"Rising "), to_string(), _pinNumber, 
                  (state == GpioFallingEdge ? _rtcCounterFalling:_rtcCounterRising));
        _notifyObservers();
    }
}


/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void Gpio::_notifyObservers()
{
    for (uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            if (_state == GpioFallingEdge)
            {
                observer(i)->onFallingEdge(this);
            }
            else
            {
                observer(i)->onRisingEdge(this);
            }
        }
    }
}
