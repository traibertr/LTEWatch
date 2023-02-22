/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    gpio.h
 *
 * \defgroup GPIO
 * \{
 *
 * @class   Gpio
 * @brief   Abstraction of a gpio
 *
 * # Gpio
 *
 * This class is the Abstraction of a watch's gpio. Those gpio are defined by an identifier
 * (ID) and a state to let the appication know on which a special pressure or release has been
 * applied.
 *
 * The behaviour of the gpio or the user's actions applied on it are controlled by a
 * GpioController. See its documentation for more details.
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2016-01-09
 *
 * @author  Patrice Rudaz (patrice.rudaz@hevs.ch)
 * @date    July 2018
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <platform-config.h>
#include "isubject.hpp"
#include "gpio-config.h"
#include "gpioobserver.h"
#include "gpioid.h"
#include <nrfx_gpiote.h>

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* NAMESPACE Declaration                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
namespace gpio
{

    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* Class Declaration                                                                                            */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    class Gpio : public ISubject<GpioObserver, APP_GPIO_MAX_OBSERVERS>
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* CONSTRUCTOR DECLARATION SECTION                                                                          */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        Gpio()  {};
        ~Gpio() {};

        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Enumerates the available gpio's state
         *
         * This enumeration defines the available gpio's state:
         *   - `UnknownGpioState`:
         *     Just to define the 0 value...
         *
         *   - `GpioRisingEdge`:
         *     Gpio has gone to high level
         *
         *   - `GpioFallingEdge`:
         *     Gpio has gone to low state
         ************************************************************************************************************/
        typedef enum
        {
            UnknownGpioState = 0,
            GpioRisingEdge,
            GpioFallingEdge,
            TopEnumGpioState
        } eGpioState;

        /********************************************************************************************************//**
         * @brief   Initializes the components needed by this class
         *
         * This method initializes the components needed by the class and set as well an identifier for the current 
         * Gpio. This ID could be used to identify the gpio in terms of activities. With this ID, we can build up an
         * hierarchy or identify easily any gpio...
         *
         * @param   pinNumber   Defines the PIN number to be used for the current gpio
         * @param   gpioId      The identifier of the gpio.
         * @param   pullSetup   Defines if this input must be set with a pulldown, pullup or no pull.
         *
         * @warning Must be called before any other method of this class
         * \note    Requires existing instances of classes listed in parameter list
         ************************************************************************************************************/
        void initialize(uint8_t pinNumber,
                        gpio::GpioId::eGpioId gpioId,
                        nrf_gpio_pin_pull_t pullSetup = static_cast<nrf_gpio_pin_pull_t>(GPIO_PULL_SETUP)
                        );

        /********************************************************************************************************//**
         * @brief   Retrieves the identifier of the Gpio
         ************************************************************************************************************/
        inline gpio::GpioId::eGpioId gpioId() const                 { return _gpioId; }

        /********************************************************************************************************//**
         * @brief   Retrieves the pin Number of the Gpio
         ************************************************************************************************************/
        inline uint8_t pinNumber() const                            { return _pinNumber; }

        /********************************************************************************************************//**
         * @brief   Retrieves the pin Mask of the Gpio
         ************************************************************************************************************/
        inline uint64_t pinMask() const                             { return _pinMask; }

        /********************************************************************************************************//**
         * @brief   Retrieves the pin Mask of the Gpio
         ************************************************************************************************************/
        inline nrf_gpio_pin_pull_t pullSetup() const                { return _pullSetup; }

        /********************************************************************************************************//**
         * @brief   Retrieves the current state of the Gpio
         ************************************************************************************************************/
        inline eGpioState gpioState() const                         { return _state; }

        /********************************************************************************************************//**
         * @brief   Retrieves `true` if the Gpio is UP (in RisingEdge state) and `false` otherwise
         ************************************************************************************************************/
        inline bool isSet() const                                   { return _state == GpioRisingEdge; }

        /********************************************************************************************************//**
         * @brief   Returns `true` if any debounce's process is active on the current GPIO and `false` otherwise.
         ************************************************************************************************************/
        inline bool isDebouncing() const                            { return _isDebouncing; }

        /********************************************************************************************************//**
         * @brief   Returns `true` if the Gpio is DOWN (in FallingEdge state) and `false` otherwise
         ************************************************************************************************************/
        inline bool isCleared() const                               { return _state == GpioFallingEdge; }

        /********************************************************************************************************//**
         * @brief   Retrieves the last state of the Gpio
         ***********************************************************************************************************/
        inline eGpioState gpioLastState() const                     { return _lastState; }

        /********************************************************************************************************//**
         * @brief   Retrieves the RTC Counter value stored when the GPIOTE has fired the falling edge event
         ************************************************************************************************************/
        inline uint32_t getRtcCounterValueOnFallingEdge() const     { return _rtcCounterFalling; }

        /********************************************************************************************************//**
         * @brief   Retrieves the RTC Counter value stored when the GPIOTE has fired the rising edge event
         ************************************************************************************************************/
        inline uint32_t getRtcCounterValueOnRisingEdge() const      { return _rtcCounterRising; }

        /********************************************************************************************************//**
         * @return  Returns the GPIO identifier to string... For DEBUG purpose
         ************************************************************************************************************/
        inline const char* to_string() const                         { return GpioId::to_string(_gpioId); }

        /********************************************************************************************************//**
         * @brief   Stores the value of the RTC counter while the button event occurs.
         *
         * @param   rcCounterValue  The conter value of the RTC to be stored.
         * @param   fallingEdge     Flag to identify if the value should be stored for the falling or the rising edge.
         ************************************************************************************************************/
        inline void setRtcCounterValue(uint32_t rtcCounterValue, bool fallingEdge)
        {
            if (fallingEdge) _rtcCounterFalling = rtcCounterValue;
            else            _rtcCounterRising  = rtcCounterValue;
        }

        /********************************************************************************************************//**
         * @brief   Update the debouncing flag according to the given parameter.
         *
         * Update the debouncing flag to let the application know if any debounce's process is active for the current
         * GPIO.
         *
         * @param   active  `true` if a debounce's process is active and `false` otherwise.
         ************************************************************************************************************/
        inline void updateDebouncerFlag(bool active)                { _isDebouncing = active; }

        /********************************************************************************************************//**
         * @brief   Set the state of the gpio
         *
         * When an activity is detected on the current gpio, the `GpioController` call this method to change the 
         * current state of the gpio. The gpio's state is ONLY changed if the new state is different from the current 
         * one.
         *
         * @param   state   The new state. (Default value: Gpio::UnknownBtnState)
         ************************************************************************************************************/
        void setState(eGpioState state = UnknownGpioState);

    protected:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PROTECTED DECLARATION SECTION                                                                            */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        uint8_t                             _pinNumber;
        nrf_gpio_pin_pull_t                 _pullSetup;
        uint64_t                            _pinMask;
        bool                                _isDebouncing;

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        GpioId::eGpioId                     _gpioId;
        eGpioState                          _state;
        eGpioState                          _lastState;
        uint32_t                            _rtcCounterFalling;
        uint32_t                            _rtcCounterRising;

        void                                _notifyObservers();
    };

} // namespace gpio

/** \} */   // Group: GPIO
