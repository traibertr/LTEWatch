/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "gpiocontroller.h"

#include "factory.h"

#include "sdktools.h"
#include "debug-config.h"

// APP_TIMER / K_TIMER OPTION
#define USE_K_TIMER 1

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */

#include <logging/log.h>
LOG_MODULE_REGISTER(gpio_ctrl, LOG_LEVEL_DBG);

#define DEBUG_GPIO_CONTROLLER_ENABLE        0
#if (DEBUG_GPIO_CONTROLLER_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #define PIOC_DEBUG(args...)             LOG_DBG(args)
    #define PIOC_ERROR(args...)             LOG_ERR(args)
#else
    #define PIOC_DEBUG(...)                 {(void) (0);}
    #define PIOC_ERROR(...)                 {(void) (0);}
#endif

#define DEBUG_GPIO_CONTROLLER_TE_ENABLE     0
#if (DEBUG_GPIO_CONTROLLER_TE_ENABLE == 1)
    #define PIOCTE_DEBUG(args...)           LOG_DBG(args)
    #define PIOCTE_INFO(args...)            LOG_INF(args)
    #define PIOCTE_ERROR(args...)           LOG_ERR(args)
#else
    #define PIOCTE_DEBUG(...)               {(void) (0);}
    #define PIOCTE_INFO(...)                {(void) (0);}
    #define PIOCTE_ERROR(...)               {(void) (0);}
#endif

/* **************************************************************************************************************** */
/* Object Declaration                                                                                               */
/* **************************************************************************************************************** */
static struct k_timer _gpioHandlerTimerId;

/* **************************************************************************************************************** */
/* Namespace Declaration                                                                                            */
/* **************************************************************************************************************** */
using gpio::GpioController;

/* **************************************************************************************************************** */
/* --- Extern "C" Section ---                                                                                       */
/*     Callback's section                                                                                           */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void gpio::gpioControllerTaskEventHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    #if (USE_GPIOS != 0) || (USE_CHARGER != 0)
        // Save the RTC1 counter value
        static uint32_t counterValue = 0;
        #if (USE_K_TIMER)
            counterValue = static_cast<uint32_t>(k_uptime_ticks());
        #else
            counterValue = tools::SDKTools::appTimerGetCounter();
        #endif
        Factory::gpioController.gpioteEventHandler(pin, action, counterValue);
    #endif
}

// ------------------------------------------------------------------------------------------------------------------
//void gpio::gpioControllerTimeoutHandler(void* pContext)
void gpio::gpioControllerTimeoutHandler(struct k_timer *timer_id)
{
    //if (pContext != NULL)
    //{
    //    (reinterpret_cast<GpioController*>(pContext))->timerEventHandler();
    //}
    if(timer_id == &_gpioHandlerTimerId)
        {
        PIOC_DEBUG("Callback -> gpioControllerTimeoutHandler");
        void* pContext = k_timer_user_data_get(&_gpioHandlerTimerId);
        if (pContext != NULL)
        {
            (reinterpret_cast<GpioController*>(pContext))->timerEventHandler();
        }
    }
}


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void GpioController::initialize(uint32_t timerTickDelay, uint32_t resetValue)
{
    // Initializations
    BaseController::initialize();
    for (int i = 0; i < NB_MAX_GPIOS; i++)
    {
        _timerEnabled[i]    = false;
        _gpios[i]           = NULL;
    }
    
    _timerTickDelay         = timerTickDelay;
    _timerTickDelayValue    = k_ms_to_ticks_near32(timerTickDelay);
    _resetDelayValue        = resetValue;
}

// ------------------------------------------------------------------------------------------------------------------
bool GpioController::registerGpio(Gpio* gpio, bool useTimer, uint32_t debounceDelay, nrfx_gpiote_evt_handler_t eventHandler)
{
    if (_nbRegisteredObject >= NB_MAX_GPIOS)
    {
        PIOC_DEBUG("No more GPIO can be registered ! The controller is full");
        return false;
    }
        
    _gpios[_nbRegisteredObject]  = gpio;
    _pinMaskForRegisteredObject |= gpio->pinMask();
    if (useTimer)
    {
        _gpioControllerTimerInit();
        setDelay(_nbRegisteredObject, debounceDelay);
        _timerEnabled[_nbRegisteredObject] = true;
    }
    _nbRegisteredObject++;

    nrfx_gpiote_in_config_t gpioConfig;
    gpioConfig.sense             = NRF_GPIOTE_POLARITY_TOGGLE;
    gpioConfig.pull              = gpio->pullSetup();
    gpioConfig.is_watcher        = false;
    gpioConfig.hi_accuracy       = false;
    uint32_t errCode             = nrfx_gpiote_in_init(gpio->pinNumber(), &gpioConfig, eventHandler);
    if (errCode != NRFX_SUCCESS)
    {
        if (errCode == NRFX_ERROR_NO_MEM)
        {
            PIOC_ERROR("nrfx_gpiote_in_init() failed for %s (PIN %d). Error: NRF_ERROR_NO_MEM", gpio->to_string(), gpio->pinNumber());
        }
        else
        {
            PIOC_ERROR("nrfx_gpiote_in_init() failed for %s (PIN %d). errCode: %d", gpio->to_string(), gpio->pinNumber(), errCode);
        }
        return false;
    }
    else
    {
        nrfx_gpiote_in_event_enable(gpio->pinNumber(), true);
    }
    return true;
}

// ------------------------------------------------------------------------------------------------------------------
void GpioController::unregisterGpio(Gpio* gpio)
{
    // Un-init the GPIOTE for the given gpio
    nrfx_gpiote_in_uninit(gpio->pinNumber());

    // remove the given gpio form the controller
    _removeGpio(gpio);
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t GpioController::pinNumber(uint8_t index) const
{
    if (index < _nbRegisteredObject)
    {
        return _gpios[index]->pinNumber();
    }
    return 255;
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t GpioController::indexFromPinNumber(uint8_t pinNumber) const
{
    for (uint8_t i = 0; i < _nbRegisteredObject; i++)
    {
        if (_gpios[i]->pinNumber() == pinNumber)
        {
            return i;
        }
    }
    return 255;
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t GpioController::indexFromId(uint8_t gpioId) const
{
    for (uint8_t i = 0; i < _nbRegisteredObject; i++)
    {
        if (_gpios[i]->gpioId() == gpioId)
        {
            return i;
        }
    }
    PIOC_DEBUG("No entry for gpioId %02d !", gpioId);
    return 255;
}

// ------------------------------------------------------------------------------------------------------------------
uint32_t GpioController::pinMask(uint8_t index) const
{
    if (index < _nbRegisteredObject)
    {
        return _gpios[index]->pinMask();
    }
    PIOC_DEBUG("Out of bound index: %d!", index);
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
bool GpioController::isTimerEnabled(uint8_t index) const
{
    if (index < _nbRegisteredObject)
    {
        return _timerEnabled[index];
    }
    PIOC_DEBUG("Out of bound index: %d!", index);
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
void GpioController::enableController()
{
    _gpioControllerInit();
}

// ------------------------------------------------------------------------------------------------------------------
void GpioController::stateChanged(uint8_t index, Gpio::eGpioState gpioState)
{
    if (_gpios[index] != NULL)
    {
        _gpios[index]->setState(gpioState);
        PIOC_DEBUG("%s (%d), gpioState: %d", _gpios[index]->to_string(), index, gpioState);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void GpioController::setHandlerTimerActive(bool active)
{
    _isHandlerTimerActive = active;
}

// ------------------------------------------------------------------------------------------------------------------
void GpioController::gpioteEventHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action, uint32_t rtcCounterValue)
{
    // Check which gpios generates an event
    uint8_t gpioIdx  = indexFromPinNumber(pin);
    PIOCTE_INFO("gpioIdx %02d (pin: %02d), action: %s", 
                gpioIdx, pin, (action == NRF_GPIOTE_POLARITY_LOTOHI ? "GPIOTE_CONFIG_POLARITY_LoToHi":
                              (action == NRF_GPIOTE_POLARITY_HITOLO ? "GPIOTE_CONFIG_POLARITY_HiToLo":"GPIOTE_CONFIG_POLARITY_Toggle")));

    if (gpioIdx != 255)
    {
        const bool gpioIsSet = nrfx_gpiote_in_is_set(pin);
        
        // Checks if the debouncer is activated
        if (isTimerEnabled(gpioIdx))
        {
            // Update debounce's process flag
            _gpios[gpioIdx]->updateDebouncerFlag(true);
            
            // Checks if the Debouncer's timer is running
            if (!_isHandlerTimerActive)                                                                                  
            {
                #if (USE_K_TIMER)
                    k_timer_user_data_set(&_gpioHandlerTimerId, this);
                    k_timer_start(&_gpioHandlerTimerId, K_TICKS(_timerTickDelayValue), K_TICKS(_timerTickDelayValue));
                    uint32_t errCode = NRFX_SUCCESS;
                #else
                    uint32_t errCode = tools::SDKTools::appTimerStart(&_gpioHandlerTimerId, _timerTickDelayValue, this);
                #endif
                if (errCode == NRFX_SUCCESS)
                {
                    _isHandlerTimerActive = true;
                    PIOC_DEBUG("Starts the app_timer");
                }
                else
                {
                    PIOC_ERROR("appTimerStart() FAILED! (errCode: %d)", errCode);
                    //ASSERT(false);
                }
            }
        }
        else
        {
            // Update the State...
            resetDelayCounter(gpioIdx, _resetDelayValue);
            if (gpioIsSet)
            {
                PIOCTE_DEBUG("%s rising.", _gpios[gpioIdx]->to_string());
                stateChanged(gpioIdx, gpio::Gpio::GpioRisingEdge);
            }
            else
            {
                PIOCTE_DEBUG("%s falling.", _gpios[gpioIdx]->to_string());
                stateChanged(gpioIdx, Gpio::GpioFallingEdge);
            }
        }

        // Storing the RTC counter value
        if (gpioIsSet)
        {
            PIOCTE_DEBUG("%s > setRtcCounterValue (%d) for RISING edge...", _gpios[gpioIdx]->to_string(), rtcCounterValue);
            _gpios[gpioIdx]->setRtcCounterValue(rtcCounterValue, false);
        }
        else
        {
            PIOCTE_DEBUG("%s > setRtcCounterValue (%d) for FALLING edge...", _gpios[gpioIdx]->to_string(), rtcCounterValue);
            _gpios[gpioIdx]->setRtcCounterValue(rtcCounterValue, true);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void GpioController::timerEventHandler()
{
    PIOC_DEBUG("Callback -> timerEventHandler");
    bool stopAppTimer = true;
    for (int i = 0; i < _nbRegisteredObject; i++)
    {
        uint8_t pin = pinNumber(i);
        if (pin < NUMBER_OF_PINS && isTimerEnabled(i))
        {
            incrDelayCounter(i, _timerTickDelay);
            PIOC_DEBUG("delayCounter[%d] = %04d.", i, _delayCounters[i]);
            
            // checks debouncer's delay
            PIOC_DEBUG("Is delayCounter[%d]: %04d >= delay(%d): %04d ?", i, _delayCounters[i],i, delay(i));
            if ((delayCounters(i) >= delay(i)))
            {
                _gpios[i]->updateDebouncerFlag(false);
                if (nrfx_gpiote_in_is_set(pin))
                {
                    PIOCTE_DEBUG("Rising Edge detected on %s.", _gpios[i]->to_string());
                    stateChanged(i, gpio::Gpio::GpioRisingEdge);
                }
                else
                {
                    PIOCTE_DEBUG("Falling Edge detected on %s.", _gpios[i]->to_string());
                    stateChanged(i, gpio::Gpio::GpioFallingEdge);
                }
            }
            else
            {
                stopAppTimer = false;
            }
        }
    }

    if (stopAppTimer)
    {
        #if (USE_K_TIMER)
            k_timer_stop(&_gpioHandlerTimerId);
        #else
            tools::SDKTools::appTimerStop(&_gpioHandlerTimerId);
        #endif
        
        _isHandlerTimerActive = false;
        for (int i = 0; i < _nbRegisteredObject; i++)
        {
            resetDelayCounter(i, _resetDelayValue);
            _gpios[i]->updateDebouncerFlag(false);
        }
        PIOC_DEBUG("Stops the app_timer");
    }
    PIOC_DEBUG("End -> timerEventHandler");
}

/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
inline void GpioController::_removeGpio(Gpio* gpio)
{
    Gpio* pToMove                               = NULL;
    bool useTimer                               = false;
    for (uint8_t i = _nbRegisteredObject; i > 0; i--)
    {
        if (_gpios[i - 1] == gpio)
        {
            _gpios[i - 1]                       = pToMove;
            _timerEnabled[i - 1]                = useTimer;
            _pinMaskForRegisteredObject        &= ~gpio->pinMask();
            _nbRegisteredObject--;
            break;
        }
        else if (pToMove == NULL)
        {
            pToMove                             = _gpios[i - 1];
            useTimer                            = _timerEnabled[i - 1];
            _gpios[i - 1]                       = NULL;
            _timerEnabled[i - 1]                = false;
        }
    }
    if (pToMove != NULL)
    {
        _gpios[_nbRegisteredObject - 1]         = pToMove;
        _timerEnabled[_nbRegisteredObject - 1]  = useTimer;
    }
}

// ------------------------------------------------------------------------------------------------------------------
inline void GpioController::_clearAllGpios()
{
    for (uint8_t i = _nbRegisteredObject; i > 0; i--)
    {
        // Un-init the GPIOTE for the given gpio
        nrfx_gpiote_in_uninit(_gpios[i - 1]->pinNumber());

        _gpios[i - 1]                           = NULL;
        _timerEnabled[i - 1]                    = false;
    }
    _nbRegisteredObject                         = 0;
    _pinMaskForRegisteredObject                 = 0;
}

// ------------------------------------------------------------------------------------------------------------------
inline void GpioController::_gpioControllerTimerInit()
{
    if (!_isHandlerTimerInitialized)
    {
        #if (USE_K_TIMER)
            k_timer_init(&_gpioHandlerTimerId, gpio::gpioControllerTimeoutHandler, NULL);
            uint32_t errCode = NRFX_SUCCESS;
        #else
            uint32_t errCode = tools::SDKTools::appTimerCreate(&_gpioHandlerTimerId,
                                                               APP_TIMER_MODE_SINGLE_SHOT,
                                                               (void*) gpio::gpioControllerTimeoutHandler);
        #endif

        if (errCode == NRFX_SUCCESS)
        {
            _isHandlerTimerInitialized          = true;
        }
        else
        {
            PIOC_ERROR("_gpioControllerTimerInit() FAILED! (errCode: %d)", errCode);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
inline void GpioController::_gpioControllerInit()
{
    for (uint8_t i = 0; i < NB_MAX_GPIOS; i++)
    {
        _delayCounters[i]                       = 0;
    }
}
