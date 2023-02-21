/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    button.h
 *
 * @addtogroup GPIO
 * \{
 *
 * \defgroup Button
 * \{
 *
 * @class   Button
 * @brief   Abstraction of a button...
 *
 * # Button
 *
 * This class is the Abstraction of a button. Those button are defined by an identifier (ID) and a state to 
 * let the appication know on which button a special pressure or release has been applied.
 *
 * The behaviour of the button or the user's actions applied on it are controlled by a ButtonController. 
 * See its documentation for more details.
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2018-02-26
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
#include "platform-config.h"
//#include <boards.h>

#include "isubject.hpp"

#include "buttonid.h"
#include "buttonPersistor.h"
#include "button-config.h"
#include "gpio.h"
#include "buttonobserver.h"

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
    class Button : public ISubject<ButtonObserver, APP_BUTTON_MAX_OBSERVERS> 
    #if (USE_FLASH != 0)
        , public flash::Persister
    #endif  // #if (USE_FLASH != 0)
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Enumerates the available button's state
         *
         * This enumeration defines the available button's state:
         *  - `ButtonSingleClick`:
         *     Button pressed for a short delay.
         *
         *  - `ButtonDoubleClick`:
         *     Button pressed twice for a short delay.
         *
         *  - `ButtonTripleClick`:
         *     Button pressed three times for a short delay.
         *
         *  - `ButtonLongPress`:
         *     Button pressed for a long delay.
         *
         *  - `ButtonVeryLongPress`:
         *     Button pressed for a very long delay.
         *
         *  - `ButtonDoubleLongPress`:
         *     Button pressed twice but the last click is pressed for a long delay.
         *
         *  - `ButtonTripleLongPress`:
         *     Button pressed three times but the last click is pressed for a long delay.
         *
         *  - `ButtonReleased`:
         *     Button released.
         *
         * For developers, when chages are made to this list don't miss to update:
         *  - the private method `_notifyObservers(Button::eButtonState state)`, which notify any observer
         *  - the class `ButtonObserver`, which defines the callback's methods related to the actions on buttons
         *  - the class `ButtonPersistor`, which stores the activites on button for history
         *
         ************************************************************************************************************/
        typedef enum
        {
            ButtonSingleClick = 0,  ///< Button pressed for a short delay defined by SHORT_CLICK_MS_DELAY in `private-config.h`
            ButtonDoubleClick,      ///< Button double-pressed in a short delay (DOUBLE_CLICK_MS_DELAY).
            ButtonTripleClick,      ///< Button triple-pressed in a short delay (TRIPLE_CLICK_MS_DELAY).
            ButtonLongPress,        ///< Button pressed for a long delay (between SHORT_CLICK_MS_DELAY and LONG_CLICK_MS_DELAY).
            ButtonVeryLongPress,    ///< Button pressed for a very long delay (longer than LONG_CLICK_MS_DELAY).
            ButtonDoubleLongPress,  ///< Button double-pressed but the last pressure was a long press...
            ButtonTripleLongPress,  ///< Button triple-pressed with the last pressure as a long press...
            ButtonReleased          ///< Button released.
        } eButtonState;

        /********************************************************************************************************//**
         * @return  Transform and returns the current button event as string.
         ************************************************************************************************************/
        static const char* stateToString(eButtonState state) 
        {
            switch(state)
            {
                case ButtonSingleClick:     return "Single Click";
                case ButtonDoubleClick:     return "Double Click";
                case ButtonTripleClick:     return "Triple Click";
                case ButtonLongPress:       return "Long Press";
                case ButtonVeryLongPress:   return "Very Long Press";
                case ButtonDoubleLongPress: return "Double Long Press";
                case ButtonTripleLongPress: return "Triple Long Press";
                default:                    return "Released";
            }
        }
        
        /********************************************************************************************************//**
         * @brief   Initializes the components needed by this class
         *
         * This method initializes the components needed by the class and set as well an identifier for the current
         * Button. This ID could be used to identify the button in terms of activities. With this ID, we can build
         * up an hierarchy or identify easily any button...
         *
         * @param   pinNumber   Defines the PIN number to be used for the current button
         * @param   buttonId    The identifier of the button.
         * @param   pullSetup   Defines if this input must be set with a pulldown, pullup or no pull.
         *
         * @warning Must be called before any other method of this class
         * \note    Requires existing instances of classes listed in parameter list
         ************************************************************************************************************/
        void initialize(uint8_t pinNumber,
                        ButtonId::eButtonId buttonId,
                        nrf_gpio_pin_pull_t pullSetup = BUTTON_PULL_SETUP);
        
        /********************************************************************************************************//**
         * @return  Retrieves a pointer to the GPIO used by the button.
         ************************************************************************************************************/
        inline Gpio* gpio()                                         { return &_gpio; }
        
        /********************************************************************************************************//**
         * @return  Retrieves the identifier of the Button
         ************************************************************************************************************/
        inline ButtonId::eButtonId buttonId() const                 { return _buttonId; }

        /********************************************************************************************************//**
         * @brief   Retrieves the pin Number of the Gpio
         ************************************************************************************************************/
        inline uint8_t pinNumber() const                            { return _gpio.pinNumber(); }

        /********************************************************************************************************//**
         * @brief   Retrieves the pin Mask of the Gpio
         ************************************************************************************************************/
        inline uint32_t pinMask() const                             { return _gpio.pinMask(); }

        /********************************************************************************************************//**
         * @brief   Retrieves the pin Mask of the Gpio
         ************************************************************************************************************/
        inline nrf_gpio_pin_pull_t pullSetup() const                { return _gpio.pullSetup(); }

        /********************************************************************************************************//**
         * @brief   Return `true` if a debounce's process is active on the current button and `false` otherwise.
         ************************************************************************************************************/
        inline bool isDebouncing() const                            { return _gpio.isDebouncing(); }

        /********************************************************************************************************//**
         * @return  Retrieves the value of the RTC counter while the button Click event occurs.
         ************************************************************************************************************/
        inline uint32_t getRtcCounterValueOnClickEvent() const
        {
        #if (BTN_IS_ACTIVE_HIGH != 0)
            return _gpio.getRtcCounterValueOnRisingEdge();
        #else
            return _gpio.getRtcCounterValueOnFallingEdge();
        #endif // #if (BTN_IS_ACTIVE_HIGH != 0)
        }

        /********************************************************************************************************//**
         * @return  Retrieves the value of the RTC counter while the button Release event occurs.
         ************************************************************************************************************/
        inline uint32_t getRtcCounterValueOnReleaseEvent() const
        {
        #if (BTN_IS_ACTIVE_HIGH != 0)
            return _gpio.getRtcCounterValueOnFallingEdge();
        #else
            return _gpio.getRtcCounterValueOnRisingEdge();
        #endif // #if (BTN_IS_ACTIVE_HIGH != 0)
        }

        /********************************************************************************************************//**
         * @return  Retrieves the current state of the Button
         ************************************************************************************************************/
        inline Button::eButtonState buttonState()                   { return _state; }

        /********************************************************************************************************//**
         * @return  Retrieves the last known state of the Button
         ************************************************************************************************************/
        inline Button::eButtonState buttonLastState()               { return _lastState; }
        
        /********************************************************************************************************//**
         * @return  Retrieves the counter of single-click done during all the live of the product.
         ************************************************************************************************************/
        inline uint32_t singleClickCounter() const                  { return _persistentParams.singleClickCounter(); }

        /********************************************************************************************************//**
         * @return  Retrieves the counter of double-click done during all the live of the product.
         ************************************************************************************************************/
        inline uint32_t doubleClickCounter() const                  { return _persistentParams.doubleClickCounter(); }

        /********************************************************************************************************//**
         * @return  Retrieves the counter of tripple-click done during all the live of the product.
         ************************************************************************************************************/
        inline uint32_t tripleClickCounter() const                  { return _persistentParams.tripleClickCounter(); }

        /********************************************************************************************************//**
         * @return  Retrieves the counter of long press done during all the live of the product.
         ************************************************************************************************************/
        inline uint32_t longPressCounter() const                    { return _persistentParams.longPressCounter(); }

        /********************************************************************************************************//**
         * @return  Retrieves the counter of very long press done during all the live of the product.
         ************************************************************************************************************/
        inline uint32_t veryLongPressCounter() const                { return _persistentParams.veryLongPressCounter(); }

        /********************************************************************************************************//**
         * @return  Retrieves the counter of release action done during all the live of the product.
         ************************************************************************************************************/
        inline uint32_t releaseCounter() const                      { return _persistentParams.releaseCounter(); }
        
        /********************************************************************************************************//**
         * @return  Returns the Button's identifier to string... For DEBUG purpose
         ************************************************************************************************************/
        inline const char* toString() const                         { return ButtonId::toString(_buttonId); }

        /********************************************************************************************************//**
         * @brief   Set the state of the button
         *
         * When an activity is detected on the current button, the `ButtonController` call this method to change the
         * current state of the button. The button's state is ONLY changed if the new state is different from the
         * current one.
         *
         * @param   state       The new state. (Default value: Button::UnknownBtnState)
         *
         * @return  `true` if the state has been changed and `false` if the new state has not been changed or the
         *          button was already in that state...
         ************************************************************************************************************/
        bool setState(Button::eButtonState state = Button::ButtonReleased);


    #if (USE_FLASH != 0)
        /********************************************************************************************************//**
         * \name   FlashPersiterInheritance
         * @brief  Inherit from FlashPersiter.
         * \{
         ************************************************************************************************************/
        virtual uint32_t* save(uint16_t* size);
        virtual void restore(uint32_t* data);
        /** \} */
    #endif  // #if (USE_FLASH != 0)

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Attributes
        Gpio                    _gpio;                      // The physical GPIO used to be a button
        ButtonId::eButtonId     _buttonId;                  // The identifier of the button
        ButtonPersistor         _persistentParams;          // Persistent parameters
        Button::eButtonState    _state;                     // The current state of the button
        Button::eButtonState    _lastState;                 // The last known state of the button

        // Methods
        void _notifyObservers(Button::eButtonState state);  // Method used to notify all observers of the button.
    };

}   // Namespace gpio

/** \} */   // Group: Button

/** \} */   // Group: GPIO
