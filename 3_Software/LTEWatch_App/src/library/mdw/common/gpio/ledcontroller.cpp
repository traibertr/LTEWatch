/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "ledcontroller.h"
#include "ledhal.h"

#include "factory.h"
#include "critical.h"
#include "xfthread.h"
#include "debug-config.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(led_ctrl, LOG_LEVEL_DBG);

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#define DEBUG_LED_ENABLE                    0
#if (DEBUG_LED_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #define LED_DEBUG(args...)              LOG_DBG(args) 
    #define LED_ERROR(args...)              LOG_ERR(args) 
#else
    #define LED_DEBUG(...)                  {(void) (0);}
    #define LED_ERROR(...)                  {(void) (0);}
#endif

// For State_machine debug
#define DEBUG_LED_ST_ENABLE                 0
#if (DEBUG_LED_ST_ENABLE != 0) && defined(DEBUG_NRF_USER)

    #define LEDS_DEBUG(args...)             LOG_DBG(args) 
#else
    #define LEDS_DEBUG(...)                 {(void) (0);}
#endif

    
/* **************************************************************************************************************** */
/* Namespace Declaration                                                                                            */
/* **************************************************************************************************************** */
using gpio::LedController;

    
/* **************************************************************************************************************** */
/* CONSTRUCTOR                                                                                                      */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
LedController::LedController() :
    _currentState(ST_INIT), _index(false), _ticksOn(0), _ticksOff(0), _blink(false), _isTimerActive(false)
{
    _quickOnTicks   = DEFAULT_TIME_ON_TICKS;
    _quickOffTicks  = k_ms_to_ticks_near32(500) - _quickOnTicks;

    _stdOnTicks     = _quickOnTicks;
    _stdOffTicks    = k_ms_to_ticks_near32(1000) - _stdOnTicks;

    _slowOnTicks    = _quickOnTicks;
    _slowOffTicks   = k_ms_to_ticks_near32(4000) - _slowOnTicks;
}

// ------------------------------------------------------------------------------------------------------------------
LedController::~LedController()
{
}


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void LedController::initialize(uint8_t index, uint32_t ticksOn, uint32_t totalTimeTicks)
{
    _index      = index;
    _ticksOn    = ticksOn;
    _ticksOff   = totalTimeTicks - _ticksOn;

    if (!hal::LedHal::initLed(index))
    {
        LED_ERROR("LED %#04u initialized [Failed]", index);
        return;
    }
    LED_DEBUG("LED %#04u initialized with _ticksOn: %lu & _ticksOff: %lu", index, _ticksOn, _ticksOff);
}

// ------------------------------------------------------------------------------------------------------------------
void LedController::ledOn()
{
    _stopBlinking();
    if (_currentState == ST_LED_OFF)
    {
        _ST_LED_ON_Action();
        LED_DEBUG("LED %#04x set ON" ,_index);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void LedController::ledOff()
{
    _stopBlinking();
    if (_currentState == ST_LED_ON)
    {
        _ST_LED_OFF_Action();
        LED_DEBUG("LED %#04x set OFF" ,_index);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void LedController::setTimerTicksforQuickStandardAndSlowBlinkingState(uint32_t ticksOnQuick, uint32_t totalTicksQuick,
                                                                      uint32_t ticksOnStd,   uint32_t totalTicksStd,
                                                                      uint32_t ticksOnSlow,  uint32_t totalTicksSlow)
{
    bool blink = _blink;
    _stopBlinking();
    
    _quickOnTicks   = ticksOnQuick;
    _quickOffTicks  = totalTicksQuick - _quickOnTicks;
    LED_DEBUG("quickOn =    %lu, quickOff =    %lu", _quickOnTicks, _quickOffTicks);
    _stdOnTicks     = ticksOnStd;
    _stdOffTicks    = totalTicksStd - _stdOnTicks;
    LED_DEBUG("standardOn = %lu, standardOff = %lu", _stdOnTicks, _stdOffTicks);
    _slowOnTicks    = ticksOnSlow;
    _slowOffTicks   = totalTicksSlow - _slowOnTicks;
    LED_DEBUG("slowOn =     %lu, slowOff =     %lu", _slowOnTicks, _slowOffTicks);
    
    if (blink)
    {
        startBlinking(true);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void LedController::startBlinking(bool force)
{
    // The LED can not be ON for 0 [ms] !!!
    if (_ticksOn == 0)
    {
        LED_DEBUG("Time ON is `0`, led#%u will not blink !", _index);
        _stopBlinking();
        return;
    }
    
    // Checs if the LED is already blinking...
    if (_blink && !force)
    {
        LED_DEBUG("led#%u allreading blinking with _ticksOn = %lu & _ticksOff = %lu (_blink: %s, force: %s, _isTimerActive: %s)", 
                    _index, _ticksOn, _ticksOff, (_blink ? "true":"false"), (force ? "true":"false"), (_isTimerActive ? "true":"false"));
        return; 
    }
    
    _startBlinking();
}

// ------------------------------------------------------------------------------------------------------------------
void LedController::startQuickBlinking()
{
    _setInternalTimings(_quickOnTicks, _quickOffTicks);
    startBlinking();
}

// ------------------------------------------------------------------------------------------------------------------
void LedController::startStandardBlinking()
{
    _setInternalTimings(_stdOnTicks, _stdOffTicks);
    startBlinking();
}

// ------------------------------------------------------------------------------------------------------------------
void LedController::startSlowBlinking()
{
    _setInternalTimings(_slowOnTicks, _slowOffTicks);
    startBlinking();
}

// ------------------------------------------------------------------------------------------------------------------
void LedController::toggle()
{
    if (_blink)
    {
        _stopBlinking();
    }

    if (_currentState == ST_LED_OFF)
    {
        _ST_LED_ON_Action();
    }
    else
    {
        _ST_LED_OFF_Action();
    }
}


/* **************************************************************************************************************** */
/* PROTECTED SECTION                                                                                                */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
EventStatus LedController::processEvent()
{
    switch (getCurrentEvent()->getEventType())
    {
        /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
        /* Timeout event                                                                                      */
        /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
        case IXFEvent::Timeout:
        {
            switch (getCurrentTimeout()->getId())
            {
                case Timeout_LED_id:
                {
                    LEDS_DEBUG("Timeout_LED_id received ...");
                    _isTimerActive = false;
                    if (_blink)
                    {
                        switch (_currentState) 
                        {
                            case ST_LED_OFF:
                                if (_ticksOn > 0)
                                {
                                    LEDS_DEBUG("-> ST_LED_ON from ST_LED_OFF.");
                                    _ST_LED_ON_Action();
                                    LED_DEBUG("scheduleTimeoutWithTicks %lu for LED#%d", _ticksOn, _index);
                                    getThread()->scheduleTimeoutWithTicks(Timeout_LED_id, _ticksOn, this);
                                    _isTimerActive = true;
                                }
                                break;

                            case ST_LED_ON:
                                if (_ticksOff > 0)
                                {
                                    LEDS_DEBUG("-> ST_LED_OFF from ST_LED_ON.");
                                    _ST_LED_OFF_Action();
                                    LED_DEBUG("scheduleTimeoutWithTicks %lu for LED#%d", _ticksOff, _index);
                                    getThread()->scheduleTimeoutWithTicks(Timeout_LED_id, _ticksOff, this);
                                    _isTimerActive = true;
                                }
                                break;
                            
                            default:
                                break;
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }

        /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
        /* Initial event                                                                                      */
        /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
        case IXFEvent::Initial:
        {
            if (_currentState == ST_INIT)
            {
                // Update State
                LEDS_DEBUG("-> ST_LED_OFF from ST_INIT (IXFEvent::Initial).");
                _ST_LED_OFF_Action();
            }
            break;
        }
        default:
            break;
    }

    return EventStatus::Consumed;
}


/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void LedController::_setInternalTimings(uint32_t on, uint32_t off)
{
    // Change the timings to the desired ones...
    _ticksOn    = on;
    _ticksOff   = off;
    LED_DEBUG("_ticksOn: %lu,_ticksOff: %lu", _ticksOn, _ticksOff);
}

// ------------------------------------------------------------------------------------------------------------------
void LedController::_startBlinking()
{
    // Turn OFF any active timeout
    _stopBlinking();

    _blink = true;
    getThread()->scheduleTimeoutWithTicks(Timeout_LED_id, _ticksOn, this);
    LED_DEBUG("scheduleTimeoutWithTicks %lu for LED#%d", _ticksOn, _index);
    _isTimerActive = true;

    // Turns the LED ON
    _ST_LED_ON_Action();
    LED_DEBUG("led%u should start blinking now with _ticksOn = %lu ...", _index, _ticksOn);
}

// ------------------------------------------------------------------------------------------------------------------
void LedController::_stopBlinking()
{
    _blink = false;
    if (_isTimerActive)
    {
        _isTimerActive = false;
        LED_DEBUG("unscheduleTimeout for LED#%d", _index);
        getThread()->unscheduleTimeout(Timeout_LED_id, this);
    }

    // Turns the LED ON
    _ST_LED_OFF_Action();
}

// ------------------------------------------------------------------------------------------------------------------
void LedController::_ST_LED_OFF_Action()
{
    LED_DEBUG("Turning OFF led#%u ...", _index);
    _currentState = ST_LED_OFF;
    hal::LedHal::setLedOff(_index);
}

// ------------------------------------------------------------------------------------------------------------------
void LedController::_ST_LED_ON_Action()
{
    LED_DEBUG("Turning ON  led#%u ...", _index);
    _currentState = ST_LED_ON;
    hal::LedHal::setLedOn(_index);
}
