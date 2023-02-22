/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    gpiocontroller.h
 *
 * @addtogroup GPIO
 * \{
 *
 * @class   GpioController
 * @brief   This class is the controller of gpios used as inputs.
 *
 * # GPIO Controller
 *
 * ## De-bouncer
 *
 * to be sure that a pressure has been applied on a gpio, we have implemented a de-bouncer which
 * use an `app_timer` with the parameters:
 *
 *  - APP_GPIO_HANDLER_MS_TIMER_TICK: The Tick duration used by the timer of the gpio's debouncer.
 *
 *  - GPIO_DEBOUNCE_MS_DELAY: The debounce time [ms].
 *
 * In other words when an activity is detected on a gpio, it must be applied at least for a certain time
 * at least to be considered as a real gpio's action. As soon as the debounce time is over the controller
 * keep and increase the duration of each gpio's action until a release and determines if it's a single click,
 * a long or very long press that has been applied.
 *
 * ## Notification
 *
 * When the gesture is identified, the controller will notify the observer of that fact !
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

#include "gpio.h"
#include "basecontroller.h"
#include "config/gpio-config.h"

#include <zephyr/zephyr.h>
//#include <app_timer.h>
#include <nrfx_gpiote.h>

#if defined(__cplusplus)
extern "C"
{
#endif

    namespace gpio
    {
        /********************************************************************************************************//**
         * @brief   Gpio's event handler for the GPIOTE driver.
         *
         * This function will be called each time a registered gpio is pressed. This function will wake up the CPU.
         *
         * @param   pin     The PIN's number to enable interruption by risisng edge on the GPIOs.
         * @param   action  The Polarity for the GPIOTE channel.
         ************************************************************************************************************/
        void gpioControllerTaskEventHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

        /********************************************************************************************************//**
         * @brief   Timeout's handler of the `app_timer` module (library) (GPIO's Handler timer).
         *
         * This function will be called each time the GPIO's handler interval timer expires. It will also wake up the
         * CPU and increment a variable.
         * It is also the callback method of the `app_timer` used by the GPIO controller.
         *
         * @param   p_context   Pointer used for passing some arbitrary information (context) from the
         *                      `app_start_timer()` call to the timeout handler.
         ************************************************************************************************************/
        void gpioControllerTimeoutHandler(struct k_timer *timer_id);

    }

#if defined(__cplusplus)
}
#endif

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
    class GpioController : public BaseController
    {
        friend void gpioControllerTaskEventHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
        friend void gpioControllerTimeoutHandler(struct k_timer *timer_id);

    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Initializes the components needed by this class
         *
         * This method initializes the GPIOTE (hardware) used to handle the gpio events. To define which GPIOTE are
         * used for a gpio, see gpiohal.h.
         *
         * @param timerTickDelay    The tick delay to used for the timer of the debouncer given in [ms].
         * @param resetValue        Reset value to be set when reseting the counter delay of the gpio [ms].
         *
         * @warning Must be called before any other method of this class
         ************************************************************************************************************/
        void initialize(uint32_t timerTickDelay = APP_GPIO_HANDLER_MS_TIMER_TICK, uint32_t resetValue = 0);

        /********************************************************************************************************//**
         * @brief   Register a new gpio in the controller class.
         *
         * This method `registerGpio` MUST be used to register a new Gpio. In fact, this method just save
         * the pointer to the instance of a gpio in an array to keep a reference and be able to detect a single
         * action on any gpio's instance.
         *
         * @param   gpio            Pointer to the new gpio to register.
         * @param   useTimer        Specify if the GpioController will add a delay before notifying any gpio's
         *                          state change.
         * @param   debouncDelay    The debouncer's delay in [ms].
         * @param   eventHandler    Pointer to the method handling the GPIO Task and Events
         *
         * @return  `true` if the registration succeeds or `false otherwise`.
         ************************************************************************************************************/
        bool registerGpio(Gpio* gpio,
                          bool useTimer = false,
                          uint32_t debounceDelay = APP_GPIO_MS_DELAY,
                          nrfx_gpiote_evt_handler_t eventHandler = gpio::gpioControllerTaskEventHandler);
        /********************************************************************************************************//**
         * @brief   Unregister the given gpio in the controller class.
         *
         * This method just release the pointer to the instance of a gpio and disable the task and event controller
         * on this GPIO.
         *
         * @param   gpio        Pointer to the gpio to unregister.
         ************************************************************************************************************/
        void unregisterGpio(Gpio* gpio);

        /********************************************************************************************************//**
         * @brief   Retrieves the number of registered Object
         *
         * @return  The amount of registered Object...
         ************************************************************************************************************/
        inline uint8_t  nbOfRegisteredObject() const        { return _nbRegisteredObject; }

        /********************************************************************************************************//**
         * @brief   Enable the gpio's controller
         *
         * @warning This method MUST ONLY be called once all gpios are registered by using the `registerGpio` method.
         ************************************************************************************************************/
        void enableController();

        /********************************************************************************************************//**
         * @brief   Retrieves the PIN number used for the gpio
         *
         * @param   index  The index of the gpio.
         *
         * @return  The PIN Number as an `uint8_t`
         ************************************************************************************************************/
        uint8_t pinNumber(uint8_t index) const;

        /********************************************************************************************************//**
         * @brief   Retrieves the index number used for the gpio
         *
         * @param   pinNumber   The pin number of the gpio.
         *
         * @return  The index of the gpio as an `uint8_t`
         ************************************************************************************************************/
        uint8_t indexFromPinNumber(uint8_t pinNumber) const;

        /********************************************************************************************************//**
         * @brief   Retrieves the index number used for the gpio specified by its GpioId
         *
         * @param   gpioId      The identifier of the gpio.
         *
         * @return  The index of the gpio as an `uint8_t`
         ************************************************************************************************************/
        uint8_t indexFromId(uint8_t gpioId) const;

        /********************************************************************************************************//**
         * @brief   Retrieves the MASK used for the gpio. It's stricly related with the GPIO
         *
         * @param   index   The identifier of the gpio.
         *
         * @return  The GPIO MASK for the current gpio as an `uint32_t`
         ************************************************************************************************************/
        uint32_t pinMask(uint8_t index) const;

        /********************************************************************************************************//**
         * @brief   Retrieves \c true if an `app_timer` is associated to the GPIO identified by the given \c index
         *          parameter.
         *
         * @param   index   The identifier of the gpio.
         *
         * @return  \c true or \c false if an app_timer is used or not by the GPIO.
         ************************************************************************************************************/
        bool isTimerEnabled(uint8_t index) const;

        /********************************************************************************************************//**
         * @brief   Notify the controller that a gpio has been pressed
         *
         * Every time a gpio is released after the Debounce delay, the controller is notified.
         * This method needs the index of the gpio and which state is applied.
         *
         * `gpioState` can take the values :
         *   - `GpioShortClick`:    Gpio pressed for a short delay.
         *   - `GpioLongClick`:     Gpio pressed for a long delay.
         *   - `GpioVeryLongClick`: Gpio pressed for a very long delay.
         * Other value should not be considered here.
         *
         * @param   index       The identifier of the gpio.
         * @param   gpioState   The kind of pressed applied on the gpio.
         ************************************************************************************************************/
        void stateChanged(uint8_t index, Gpio::eGpioState gpioState);

        /********************************************************************************************************//**
         * @brief   To know if the timer used for the button's handler is running or not.
         *
         * This method retrieves `TRUE` if either the hardware TIMER or the software timer `app_timer`
         * used for the button's controller is running and `FALSE` otherwise.
         *
         * @return  `True` if the TIMER or `app_timer` used to debounce is running.
         ************************************************************************************************************/
        inline bool isHandlerTimerActive() const        { return _isHandlerTimerActive; }

        /********************************************************************************************************//**
         * @brief   Accessor to the _isHandlerTimerActive private variable
         *
         * @param   active  New value to be set.
         ************************************************************************************************************/
        void setHandlerTimerActive(bool active);

        /********************************************************************************************************//**
         * @brief   GPIOTE's event handler.
         *
         * This function will be called each time a registered button is pressed. This function will wake up the CPU.
         *
         * @param   pin             The PIN's number to enable interruption by risisng edge on the GPIOs.
         * @param   action          The Polarity for the GPIOTE channel.
         * @param   rtcCounterValue The counter value of the RTC timer used to debounce the button's events
         ************************************************************************************************************/
        void gpioteEventHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action, uint32_t rtcCounterValue);

        /********************************************************************************************************//**
         * @brief   Timeout's handler for the SoftDevice timer (Gpio's Handler timer).
         *
         * This function will be called each time the Button's handler interval timer expires. This function will 
         * wake up the CPU and increment a variable. It is also the callback method of the `app_timer` used by the
         * button controller.
         ************************************************************************************************************/
        void timerEventHandler();

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Attributes
        Gpio*       _gpios[NB_MAX_GPIOS];           // Array of pointers on registered gpios for the current watch's movement
        bool        _timerEnabled[NB_MAX_GPIOS];    // Array to measure the duration of the Button's press
        uint32_t    _timerTickDelay;                // Tick delay to used for the timer of the debouncer [ms]
        uint32_t    _timerTickDelayValue;           // Tick counter value to used for the timer of the debouncer
        uint32_t    _resetDelayValue;               // Reset value for the Tick counter value to used for the timer of the debouncer

        // Methods
        /************************************************************************************************************
         * @brief   Remove a gpio from the gpio controller
         *
         * This method let the Gpio's controller remove a given gpio. The pin_number defining the gpio is removed in
         * the array of registered gpios `_registeredGpios[]`. The pin mask is updated to disable events on rising
         * and falling edges.
         *
         * @param   gpio        Pointer to the new gpio to register.
         ************************************************************************************************************/
        inline void _removeGpio(Gpio* gpio);

        /************************************************************************************************************
         * @brief   Delete all gpios from the controller
         *
         * This method let the Gpio's controller clear all gpios. The array of registered gpios
         * `_registeredGpios[]` is cleared. The pin mask is set back to 0.
         ************************************************************************************************************/
        inline void _clearAllGpios();

        /************************************************************************************************************
         * @brief   Timer's initialization for GPIO's controller (`app_timer` module).
         *
         * Creates a new SoftDevice timer for the current application used by the gpio's controller.
         ************************************************************************************************************/
        inline void _gpioControllerTimerInit();

        /************************************************************************************************************
         * @brief   SoftDevice's gpio initialization.
         *
         * Initialize the different gpio for the current application. If there are more than 5 gpios, the
         * APP_GPIOTE_MAX_USERS define must be changed..
         ************************************************************************************************************/
        inline void _gpioControllerInit();
    };

}   // namespace gpio

/** \} */   // Group: GPIO
