/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    ledcontroller.h
 *
 * @addtogroup GPIO
 * \{
 *
 * \defgroup LED
 * \{
 *
 * @class   LedController
 * @brief   Controller able to switch ON or OFF a specific LED
 *
 * # LED Controller
 *
 * This class handles LED by switching it ON or OFF. The selection of the LED to work with
 * depends of the hardware and it's set through the initialize() method which maps the MSP430
 * port.
 *
 * @image html ./images/LedControllerStateMachine.png "LED Controller State Machine"
 *
 * The switching between On and OFF is based on a very simple state machine.  The different states
 * of the transition are the following:
 *   - <i>ST_Init:</i>
 *
 *     This is the first state where every state machine MUST pass through and where every object and
 *     elements are initialized.
 *
 *   - <i>ST_OFF:</i>
 *
 *     The state where The LED is turned off. After a `TimerEvent` the state machine goes to `ST_ON`
 *     state.
 *
 *   - <i>ST_ON:</i>
 *
 *     The state where The LED is turned on. After a `TimerEvent` the state machine goes to `ST_OFF`
 *     state.
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
#include "xfreactive.h"
#include "xf_adapter.h"

#include <stdint.h>


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
    class LedController : public XFReactive
    {
    public:
        /********************************************************************************************************//**
         * @brief   Enumeration used to have a unique identifier for every state in the state machine.
         ************************************************************************************************************/
        typedef enum
        {
            ST_UNKNOWN = 0,
            ST_INIT,
            ST_LED_OFF,
            ST_LED_ON
        } eLedState;

        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* CONSTRUCTOR/DESTRUCTOR DECLARATION SECTION                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        LedController();
        virtual ~LedController();

        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Initializes the components needed by this class
         *
         * This initialization method needs at least one parameter, which is the identifier of the LED to be
         * controlled. The different timing ON and OFF time can also be given here. If they are not defined, the
         * default value of 1 second is applied for both of them.
         *
         * @param   index           The identifier of the LED to control
         * @param   ticksOn         The delay, in terms of timer's ticks, the led should be ON, default value is 125 [ms]
         * @param   totalTimeTicks  The duration of a single sequence, default value is 1000 [ms]
         *
         * @warning Must be called before any other method of this class
         * \note    Requires pre-existing instances of classes listed in parameter list
         ************************************************************************************************************/
        void initialize(uint8_t index, uint32_t ticksOn = k_ms_to_ticks_near32(125), uint32_t totalTimeTicks = k_ms_to_ticks_near32(1000));

        /********************************************************************************************************//**
         * @brief   Switch ON the LED.
         ************************************************************************************************************/
        void ledOn();

        /********************************************************************************************************//**
         * @brief   Turn OFF the LED.
         ************************************************************************************************************/
        void ledOff();

        /********************************************************************************************************//**
         * @brief   Replace both values of the length of time the LED will be turned ON and OFF.
         *
         * @param   timeOn          The new delay in millisecond for the ON state
         * @param   totalTime       The new total duration of the sequence in millisecond
         ************************************************************************************************************/
        inline void setTimings(uint16_t timeOn, uint16_t totalTime)
        {
            _ticksOn    = interval2TimerTick(timeOn);
            _ticksOff   = interval2TimerTick(totalTime) - _ticksOn;
        }

        /********************************************************************************************************//**
         * @brief   Replace both values of the length of time the LED will be turned ON and OFF.
         *
         * @param   timeOn          The new delay in millisecond for the ON state
         * @param   totalTime       The new total duration of the sequence in millisecond
         ************************************************************************************************************/
        inline void setTicksTimings(uint32_t ticksOn, uint32_t totalTicks)
        {
            _ticksOn    = ticksOn;
            _ticksOff   = totalTicks - ticksOn;
        }

        /********************************************************************************************************//**
         * @brief   Replace both values of the length of time the LED will be turned ON and OFF.
         *
         * @param   ticksOnQuick    The new delay in tick's counter for the ON state for quick blinking
         * @param   totalTicksQuick The new total duration of the sequence in tick's counter for quick blinking
         * @param   ticksOnStd      The new delay for the ON state for standard blinking
         * @param   totalTicksStd   The new total duration of the sequence for standard blinking
         * @param   ticksOnSlow     The new delay for the ON state for slow blinking
         * @param   totalTicksSlow  The new total duration of the sequence for slow blinking
         ************************************************************************************************************/
        void setTimerTicksforQuickStandardAndSlowBlinkingState(uint32_t ticksOnQuick, uint32_t totalTicksQuick,
                                                               uint32_t ticksOnStd,   uint32_t totalTicksStd,
                                                               uint32_t ticksOnSlow,  uint32_t totalTicksSlow);


        /********************************************************************************************************//**
         * @brief   Returns `true` or `false` according to the blinking state of the LED.
         ************************************************************************************************************/
        inline bool isBlinking() const      { return _blink; }

        /********************************************************************************************************//**
         * @brief   Returns `true` or `false` according to the state of the LED.
         ************************************************************************************************************/
        inline bool isOn() const            { return _currentState == ST_LED_ON; }

        /********************************************************************************************************//**
         * @brief   Start to blink the LED.
         *
         * This method will set ON the led and start the behaviour of the state machine by starting a timer with the
         * `timeOn` value. This value should be set before.
         *
         * @param force If set, it forces the LED to start blinking again by restarting the timer...
         ************************************************************************************************************/
        void startBlinking(bool force = false);

        /********************************************************************************************************//**
         * @brief   Start to blink the LED quickly.
         *
         * This method will set ON the led and start the behaviour of the state machine by starting a timer with the
         * `timeOn` value. This value is a default value for blinking quickly.
         ************************************************************************************************************/
        void startQuickBlinking();

        /********************************************************************************************************//**
         * @brief   Start to blink the LED.
         *
         * This method will set ON the led and start the behaviour of the state machine by starting a timer with the
         * `timeOn` value. This value is a default value for blinking in a standard way.
         ************************************************************************************************************/
        void startStandardBlinking();

        /********************************************************************************************************//**
         * @brief   Start to blink the LED.
         *
         * This method will set ON the led and start the behaviour of the state machine by starting a timer with the 
         * `timeOn` value. This value is a default value for blinking slowly.
         ************************************************************************************************************/
        void startSlowBlinking();

        /********************************************************************************************************//**
         * @brief   turn ON or OFF the led according to tis current's state.
         *
         * This method will set ON the led if it is not and switch it OFF if it's ON.
         ************************************************************************************************************/
        void toggle();


    protected:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PROTECTED DECLARATION SECTION                                                                            */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Timeout identifier(s) for this state machine
         ************************************************************************************************************/
        typedef enum
        {
            Timeout_LED_id  = 1,
            Timeout_end_of_list
        } eTimeoutId;

        /********************************************************************************************************//**
         * @brief   Implements state machine behavior.
         ************************************************************************************************************/
        virtual EventStatus processEvent();

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Constants
        uint32_t   DEFAULT_TIME_ON_TICKS = k_ms_to_ticks_near32(125);

        // Attributes
        eLedState               _currentState;

        short                   _index;
        uint32_t                _ticksOn;
        uint32_t                _ticksOff;

        bool                    _blink;
        bool                    _isTimerActive;

        uint32_t                _quickOnTicks;
        uint32_t                _quickOffTicks;

        uint32_t                _stdOnTicks;
        uint32_t                _stdOffTicks;

        uint32_t                _slowOnTicks;
        uint32_t                _slowOffTicks;

        // Methods
        void                    _setInternalTimings(uint32_t on, uint32_t off) ;
        void                    _stopBlinking();
        void                    _startBlinking();
        void                    _ST_LED_OFF_Action();
        void                    _ST_LED_ON_Action();
    };  // class LedController

}   // namespace mdw

/** \} */   // Group: LED

/** \} */   // Group: GPIO
