/****************************************************************************************************************//**
 * Copyright (C) MSE Lausanne Hes-so VAUD/WAADT, Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2023
 * Modified by Tristan Traiber
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    buttoncontroller.h
 *
 * @addtogroup GPIO
 * \{
 *
 * @addtogroup Button
 * \{
 *
 * @class   ButtonController
 * @brief   This class is the controller of the user's actions applied on the buttons.
 *
 * # ButtonController
 *
 * The purpose of this class is to know what kind of gestures has been applied on a button. We have identified four
 * of them:
 *
 *   - SINGLE, DOUBLE or TRIPLE CLICK: A simple, double or triple pressure on a button. `simple` means that a
 *     pressure of less than `APP_BUTTON_CLICK_MS_DELAY` has been detected.
 *
 *   - LONG PRESS: A long click is a pressure on a button with a duration between `APP_BUTTON_CLICK_MS_DELAY` and
 *     `APP_BUTTON_LONG_PRESS_MS_DELAY`.
 *
 *   - VERY LONG PRESS: The duration a the pressure must be over `APP_BUTTON_LONG_PRESS_MS_DELAY`.
 *
 *   - RELEASED: The button was released.
 *
 * ## Debouncer
 *
 * The ButtonController uses the debouncer of the GpioController class (@see GpioController).
 *
 * ## Notification
 *
 * When the gesture is identified, the controller will notify the observer of that fact !
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2018-02-26
 *
 * @author  Tristan Traiber (tristan.traiber@master.hes-so.ch)
 * @date    December 2023
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include"platform-config.h"

#if (USE_BUTTONS != 0)

#include "button.h"
#include "button-config.h"

#include "basecontroller.h"
#include "gpiocontroller.h"
#include "gpio-config.h"
#include "gpioobserver.h"


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* APPLICATION defintion                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#define NB_MAX_BUTTONS                      BUTTONS_NUMBER

#if defined(__cplusplus)
extern "C"
{
#endif
    namespace gpio
    {
        /********************************************************************************************************//**
         * @brief   Button's event handler for the GPIOTE driver.
         *
         * This function will be called each time a registered gpio is pressed. This function
         * will wake up the CPU.
         *
         * @param   pin     The PIN's number to enable interruption by risisng edge on
         *                      the GPIOs.
         * @param   action  The Polarity for the GPIOTE channel.
         ************************************************************************************************************/
        void btnControllerTaskEventHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

        /********************************************************************************************************//**
         * @brief   Timeout's handler for the SoftDevice timer (Button's Handler timer).
         *
         * This function will be called each time the Button's handler interval timer expires. This function will
         * wake up the CPU and increment a variable. It is also the callback method of the `app_timer` used by the
         * button controller.
         *
         * @param   pContext    Pointer used for passing some arbitrary information (context) from the
         *                      app_start_timer() call to the timeout handler.
         ************************************************************************************************************/
        void btnCtrlTimeoutHandler(struct k_timer *timer_id);
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
    class ButtonController : public BaseController, public GpioObserver
    {
        friend void btnControllerTaskEventHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
        friend void btnCtrlTimeoutHandler(struct k_timer *timer_id);

    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Enumerates the available state's delay for the button's actions
         *
         * This enumeration defines the available state's delay for the button's actions:
         *  - `ButtonDoubleClickDelay`: This is delay allowed to detect a double or triple click on a button..
         *  - `ButtonSingleClickDelay`: Every click during less than this delay is considered as a  short click.
         *  - `ButtonLongPressDelay`:   Every pressure less than this delay long is considered as a long press
         *                              (`ButtonLongPress`). Any pressure exceeding this delay will be consider as a
         *                              very long press (`ButtonVeryLongPress`).
         *
         * @see Button::eButtonState
         ************************************************************************************************************/
        typedef enum
        {
            ButtonDebounceDelay     = APP_BUTTON_DEBOUNCE_MS_DELAY,
            ButtonDoubleClickDelay  = APP_BUTTON_DBL_CLICK_MS_DELAY,
            ButtonSingleClickDelay  = APP_BUTTON_CLICK_MS_DELAY,
            ButtonLongPressDelay    = APP_BUTTON_LONG_PRESS_MS_DELAY
        } eButtonControllerDelay;

        /********************************************************************************************************//**
         * @brief   Initializes the components needed by this class
         *
         * This method initializes the GPIOTE (hardware) used to handle the button events. To define which GPIOTE are
         * used for a button, see buttonhal.h.
         *
         * @warning Must be called before any other method of this class
         ************************************************************************************************************/
        void            initialize();

        /********************************************************************************************************//**
         * @brief   Register a new button in the controller class.
         *
         * This method `registerButton` MUST be used to register a new Button. In fact, this method just save the 
         * pointer to the instance of a button in an array to keep a reference and be able to detect a single action
         * on any button's instance.
         *
         * @param   button              Pointer to the new button to register.
         *
         * @return `true` if the button has been registered successfully and `false` otherwise.
         ************************************************************************************************************/
        bool            registerButton(Button* button);

        /********************************************************************************************************//**
         * @brief    Retrieves the number of registered buttons
         ************************************************************************************************************/
        inline uint8_t  nbOfRegisteredButtons() const       { return _nbRegisteredObject; }

        /********************************************************************************************************//**
         * @brief   Enable the button's controller
         *
         * @warning This method MUST ONLY be called once all buttons are registered by using the `registerButton` 
         *          method.
         ************************************************************************************************************/
        void            enableController();

        /********************************************************************************************************//**
         * @brief   Retrieves the PIN number used for the button
         *
         * @param   index               The identifier of the button.
         *
         * @return  The PIN Number as an `uint8_t`
         ************************************************************************************************************/
        uint8_t         pinNumber(uint8_t index) const;

        /********************************************************************************************************//**
         * @brief   Retrieves the PIN number used for the button
         *
         * @param   index               The identifier of the button.
         *
         * @return  The PIN Number as an `uint8_t`
         ************************************************************************************************************/
        uint8_t         indexFromPinNumber(uint8_t pinNumber) const;

        /********************************************************************************************************//**
         * @brief   Retrieves the index of the button specified by its ButtonId
         *
         * @param   buttonId            The identifier of the button.
         *
         * @return  The index as an `uint8_t`
         ************************************************************************************************************/
         uint8_t         indexFromId(uint8_t buttonId) const;

        /********************************************************************************************************//**
         * @brief   Retrieves the MASK used for the button. It's stricly related with the GPIO
         *
         * @param   index               The identifier of the button.
         *
         * @return  The GPIO MASK for the current button as an `uint32_t`
         ************************************************************************************************************/
        uint32_t        pinMask(uint8_t index) const;

        /********************************************************************************************************//**
         * @brief   Retrieves the current number of click applied on the button specified by its `index`.
         *
         * @param   index               The identifier of the button.
         *
         * @return  The number of click on a given button as an `uint8_t`
         ************************************************************************************************************/
        uint8_t         clickCounter(uint8_t index) const;

        /********************************************************************************************************//**
         * @brief   Reset the number of click applied on the button specified by its `index`.
         *
         * @param   index               The identifier of the button.
         ************************************************************************************************************/
        void            resetClickCounter(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Increment the current number of click applied on the button specified by its `index`.
         *
         * @param   index               The identifier of the button.
         ************************************************************************************************************/
        void            incrClickCounter(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Notify the controller that a button has been pressed
         *
         * Every time a button is released after the Debounce delay, the controller is notified.
         * This method needs the index of the button and which state is applied.
         *
         * `buttonState` can take the values :
         *  - `ButtonShortClick`:       Button pressed for a short delay.
         *  - `ButtonLongClick`:        Button pressed for a long delay.
         *  - `ButtonVeryLongClick`:    Button pressed for a very long delay.
         * Other value should not be considered here.
         *
         * @param   index               The identifier of the button.
         * @param   buttonState         The kind of pressed applied on the button.
         ************************************************************************************************************/
        void            buttonPressed(uint8_t index, Button::eButtonState buttonState);

        /********************************************************************************************************//**
         * @brief   Notify the controller that a button has been released
         *
         * Every time a pressed button is released, the controller is notified. This method needs the  index of the
         * released button.
         *
         * @param   index               The identifier of the button.
         ************************************************************************************************************/
        void            buttonReleased(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Retrieves the MASK used for all registered buttons.
         *
         * @return  The GPIO MASK used by all registered buttons as an `uint32_t`
         ************************************************************************************************************/
        inline uint32_t pinMaskForRegisteredButtons() const     { return _pinMaskForRegisteredObject; }

        /********************************************************************************************************//**
         * @brief   Retrieves the counter value of the RTC timer for a given button on click or on release gesture.
         *
         * this method retrieves the counter value of the RTC timer for Click or Release action applied on the button 
         * identified by its index.
         *
         * @param   index               Index to identify the button to work with.
         * @param   onRelease           Specify the type of action: `true` for release and `false` for a click.
         *
         * @return  An `uint32_t` value correpsonding of the RTC counter when the specified gesture was applied...
         ************************************************************************************************************/
        inline uint32_t getRtcCounterValue(uint8_t index, bool onRelease = false) const;

        /********************************************************************************************************//**
         * @brief   Timeout's handler for the SoftDevice timer (Button's Handler timer).
         *
         * This function will be called each time the Button's handler interval timer expires. This function will 
         * wake up the CPU and increment a variable. It is also the callback method of the `app_timer` used by the
         * button controller.
         ************************************************************************************************************/
        void            timerEventHandler();

        /********************************************************************************************************//**
         * @brief   Reset all counters used by the controller.
         ************************************************************************************************************/
        void            resetCounters();

        /********************************************************************************************************//**
         * @brief   Returns the reset value of the delay's counters
         ************************************************************************************************************/
        inline uint8_t resetDelayCounterValue() const   { return _resetDelayCounterValue; }

        /********************************************************************************************************//**
         * @brief   Returns the reset value of the click's counters
         ************************************************************************************************************/
        inline uint8_t resetClickCounterValue() const   { return _resetClickCounterValue; }

    protected:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PROTECTED DECLARATION SECTION                                                                            */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   GpioObserver Interface implementation
         ************************************************************************************************************/
        virtual void onFallingEdge(gpio::Gpio* gpio);

        /********************************************************************************************************//**
         * @brief   GpioObserver Interface implementation
         ************************************************************************************************************/
        virtual void onRisingEdge(gpio::Gpio* gpio);

        /********************************************************************************************************//**
         * @brief   Retrieves a ppinter to the GpioCOntroller of the ButtonController
         ************************************************************************************************************/
        inline GpioController*  gpioController()    { return &_gpioController; }

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Attributes
        GpioController          _gpioController;                // GpioController to handle the changes on the Buttons...
        Button*                 _buttons[NB_MAX_BUTTONS];       // Array of pointers on registered buttons for the current watch's movement
        uint8_t                 _clickCounters[NB_MAX_BUTTONS]; // Array to measure the duration of the Button's press

        // Value of reset for several counters
        static const uint8_t    _resetDelayCounterValue = ((APP_BUTTON_DEBOUNCE_MS_DELAY / APP_GPIO_HANDLER_MS_TIMER_TICK)  + 1) * APP_BUTTON_DEBOUNCE_MS_DELAY;
        static const uint8_t    _resetClickCounterValue = 0xff;

        uint32_t                _buttonTimerTickDelay;
        uint32_t                _buttonTimerTickDelayValue;

        // Methods
        void                    _buttonControllerTimerInit();

        void                    _startButtonControllerTimer();
        void                    _stopButtonControllerTimer();

        void                    _onGpioStateChanged(uint8_t index, bool buttonRelease);

        inline bool             _isButtonReleased(uint8_t index)    { return (_buttons[index]->buttonState() == Button::ButtonReleased); }
    };

} // Namespace gpio

#endif  // #if (USE_BUTTONS != 0)

/** \} */   // Group: Button

/** \} */   // Group: GPIO
