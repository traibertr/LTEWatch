/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "buttoncontroller.h"

#if (USE_BUTTONS != 0)

#include "factory.h"
//#include <assert-config.h>
#include "sdktools.h"
#include "debug-config.h"

// APP_TIMER / K_TIMER OPTION
#define USE_K_TIMER 1

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */

#include <logging/log.h>
LOG_MODULE_REGISTER(button_ctrl, BTCTRL_DEBUG);

#define DEBUG_BUTTON_CONTROLLER_ENABLE      BTCTRL_DEBUG
#if (DEBUG_BUTTON_CONTROLLER_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #define BC_DEBUG(args...)               LOG_DBG(args)
    #define BC_ERROR(args...)               LOG_ERR(args)
#else
    #define BC_DEBUG(...)                   {(void) (0);}
    #define BC_ERROR(...)                   {(void) (0);}
#endif

#define DEBUG_BUTTON_CONTROLLER_DEEP_ENABLE BTCTRL_DEBUG
#if (DEBUG_BUTTON_CONTROLLER_DEEP_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #define BCD_DEBUG(args...)              LOG_DBG(args)
#else
    #define BCD_DEBUG(...)                  {(void) (0);}
#endif

#define DEBUG_BUTTON_CTRL_EVENT_ENABLE      BTCTRL_DEBUG
#if (DEBUG_BUTTON_CTRL_EVENT_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #define BCE_DEBUG(args...)              LOG_DBG(args)
#else
    #define BCE_DEBUG(...)                  {(void) (0);}
#endif

#define DEBUG_BUTTON_CTRL_TIMER_ENABLE      BTCTRL_DEBUG
#if (DEBUG_BUTTON_CTRL_TIMER_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #define BCT_DEBUG(args...)              LOG_DBG(args)
    #define BCT_ERROR(args...)              LOG_ERR(args)
#else
    #define BCT_DEBUG(...)                  {(void) (0);}
    #define BCT_ERROR(...)                  {(void) (0);}
#endif

#define DEBUG_BUTTON_CTRL_TIMER_DEEP_ENABLE BTCTRL_DEBUG
#if (DEBUG_BUTTON_CTRL_TIMER_DEEP_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #define BCTD_DEBUG(args...)             LOG_DBG(args)
#else
    #define BCTD_DEBUG(...)                 {(void) (0);}
#endif


/* **************************************************************************************************************** */
/* Object Declaration                                                                                               */
/* **************************************************************************************************************** */
static struct k_timer _buttonHandlerTimerId;


/* **************************************************************************************************************** */
/* Namespace Declaration                                                                                            */
/* **************************************************************************************************************** */
using gpio::ButtonController;

/* **************************************************************************************************************** */
/* --- Extern "C" Section ---                                                                                       */
/*     Callback's section                                                                                           */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void gpio::btnControllerTaskEventHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    BCT_DEBUG("Callback");
    #if (USE_BUTTONS != 0)
        // Save the RTC1 counter value
        static uint32_t counterValue = 0;
        #if (USE_K_TIMER)
            counterValue = static_cast<uint32_t>(k_uptime_ticks());
        #else
            counterValue = tools::SDKTools::appTimerGetCounter();
        #endif
        Factory::buttonController.gpioController()->gpioteEventHandler(pin, action, counterValue);
    #endif
}

// ------------------------------------------------------------------------------------------------------------------
void gpio::btnCtrlTimeoutHandler(struct k_timer *timer_id)
{
    if(timer_id == &_buttonHandlerTimerId)
    {
        #if (USE_BUTTONS != 0)
        BCT_DEBUG("Callback");
        void* pContext = k_timer_user_data_get(&_buttonHandlerTimerId);
            if (pContext != NULL)
            {
                (reinterpret_cast<ButtonController*>(pContext))->timerEventHandler();
            }
        #endif
    }
}


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void ButtonController::initialize()
{
    BCT_DEBUG("=====================================================================================");
    // Initializations
    _gpioController.initialize((APP_GPIO_HANDLER_MS_TIMER_TICK));
    for(uint8_t i = 0; i < NB_MAX_BUTTONS; i++)
    {
        _buttons[i]             = NULL;
        _clickCounters[i]       = _resetClickCounterValue;
    }

    _buttonTimerTickDelay       = APP_BUTTON_MS_TIMER_TICK;
    _buttonTimerTickDelayValue  = k_ms_to_ticks_ceil32(APP_BUTTON_MS_TIMER_TICK);

    BCT_DEBUG("_buttonTimerTickDelay      = %d [ms] = %d [ticks]", _buttonTimerTickDelay, _buttonTimerTickDelayValue);
    BCT_DEBUG("_resetDelayCounterValue    = %d [ms]", _resetDelayCounterValue);
}

// ------------------------------------------------------------------------------------------------------------------
bool ButtonController::registerButton(Button* button)
{
    BCT_DEBUG("Start ->");
    _buttons[_nbRegisteredObject++] = button;
    _pinMaskForRegisteredObject    |= button->pinMask();

    #if (DEBUG_BUTTON_CONTROLLER_ENABLE != 0) && defined(DEBUG_NRF_USER)
        uint32_t dMask              = static_cast<uint32_t>(_pinMaskForRegisteredObject & 0xffffffff);
        BC_DEBUG("pinMask = 0x%08x, _pinMaskForRegisteredButtons = 0x%08x", button->pinMask(), dMask);
    #endif // #if (DEBUG_BUTTON_CONTROLLER_ENABLE != 0) && defined(DEBUG_NRF_USER)

    _gpioController.registerGpio(button->gpio(), true, APP_BUTTON_DEBOUNCE_MS_DELAY, gpio::btnControllerTaskEventHandler);
    if (button->gpio()->addObserver(this))
    {
        return true;
        BCT_DEBUG("Successfully add ButtonContrller as observer of button->gpio() !");
    }
    else
    {
        BC_ERROR("Impossible to add ButtonContrller as observer of button->gpio() !");
        //ASSERT(false);
        return false;
    }
    BCT_DEBUG("-> Finish");
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t ButtonController::pinNumber(uint8_t index) const
{
    if (index < nbOfRegisteredButtons())
    {
        return _buttons[index]->pinNumber();
    }
    return 255;
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t ButtonController::indexFromPinNumber(uint8_t pinNumber) const
{
    for(uint8_t i = 0; i < _nbRegisteredObject; i++)
    {
        if (_buttons[i]->pinNumber() == pinNumber)
        {
            return i;
        }
    }
    BCD_DEBUG("No entry for id %02d !", pinNumber);
    return 255;
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t ButtonController::indexFromId(uint8_t buttonId) const
{
    for(uint8_t i = 0; i < _nbRegisteredObject; i++)
    {
        if (_buttons[i]->buttonId() == buttonId)
        {
            return i;
        }
    }
    BCD_DEBUG("No entry for id %02d !", buttonId);
    return 255;
}

// ------------------------------------------------------------------------------------------------------------------
uint32_t ButtonController::pinMask(uint8_t index) const
{
    BC_DEBUG("Start ->");
    if (index < _nbRegisteredObject)
    {
        BC_DEBUG("index %d -> pinMask: %X", index, _buttons[index]->pinMask());
        return _buttons[index]->pinMask();
    }
    return 0;
    BC_DEBUG("-> Finish");
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t ButtonController::clickCounter(uint8_t index) const
{
    BCD_DEBUG("Start ->");
    if (index < _nbRegisteredObject)
    {
        BCD_DEBUG("index %d -> counter: %d", index, _clickCounters[index]);
        return _clickCounters[index];
    }
    return 255;
    BCD_DEBUG("-> Finish");
}

// ------------------------------------------------------------------------------------------------------------------
void ButtonController::resetClickCounter(uint8_t index)
{
    BCD_DEBUG("Start ->");
    if (index < _nbRegisteredObject)
    {
        _clickCounters[index] = _resetClickCounterValue;
        BCD_DEBUG("index %d -> counter: %d", index, _clickCounters[index]);
    }
    BCD_DEBUG("-> Finish");
}

// ------------------------------------------------------------------------------------------------------------------
void ButtonController::incrClickCounter(uint8_t index)
{
    BCD_DEBUG("Start ->");
    if (index < _nbRegisteredObject)
    {
        _clickCounters[index]++;
        BCD_DEBUG("index %d -> click counter: %d", index, _clickCounters[index]);
    }
    BCD_DEBUG("-> Finish");
}

// ------------------------------------------------------------------------------------------------------------------
void ButtonController::resetCounters()
{
    BCD_DEBUG("Start ->");
    for(uint8_t i = 0; i < _nbRegisteredObject; i++)
    {
        _delayCounters[i] = 0;
        _clickCounters[i] = _resetClickCounterValue;
        BCD_DEBUG("clickCounter[%d]: %d, delayCounters[%d]: %d", i, _clickCounters[i], i, delayCounters(i));
    }
    BCD_DEBUG("Finish ->");
}

// ------------------------------------------------------------------------------------------------------------------
void ButtonController::enableController()
{
    BC_DEBUG("Start ->");
    resetCounters();
    _buttonControllerTimerInit();
    BC_DEBUG("-> Finish");
}

// ------------------------------------------------------------------------------------------------------------------
void ButtonController::buttonPressed(uint8_t index, Button::eButtonState buttonState)
{
    BCD_DEBUG("Start ->");
    if ((index < _nbRegisteredObject) && (_buttons[index] != NULL)) 
    {
        BCD_DEBUG("%s on %s", Button::stateToString(buttonState), _buttons[index]->toString());
        _buttons[index]->setState(buttonState);
    }
    BCD_DEBUG("-> Finish");
}

// ------------------------------------------------------------------------------------------------------------------
void ButtonController::buttonReleased(uint8_t index)
{
    BCD_DEBUG("Start ->");
    if ((index < _nbRegisteredObject) && (_buttons[index] != NULL))
    {
        BCD_DEBUG("RELEASED on %s", _buttons[index]->toString());
        _buttons[index]->setState(Button::ButtonReleased);
    }
    BCD_DEBUG("-> Finish");
}

// ------------------------------------------------------------------------------------------------------------------
inline uint32_t ButtonController::getRtcCounterValue(uint8_t index, bool onRelease) const
{
    BCD_DEBUG("Start ->");
    if ((index < _nbRegisteredObject) && (_buttons[index] != NULL))
    {
        if (onRelease)
        {
            return _buttons[index]->getRtcCounterValueOnReleaseEvent();
        }
        else
        {
            return _buttons[index]->getRtcCounterValueOnClickEvent();
        }
    }
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
void ButtonController::timerEventHandler()
{
    BCTD_DEBUG("Start ->");
    #if (USE_BUTTONS != 0)
        uint8_t pin = 255;
        uint8_t i   = 0;

        // Loop on all registered Buttons
        BCTD_DEBUG("+");
        for(i = 0; i < _nbRegisteredObject; i++)
        {
            pin = pinNumber(i);
            if (pin < NUMBER_OF_PINS)
            {
                incrDelayCounter(i, _buttonTimerTickDelay);
                BCT_DEBUG("%s > delayCounters: %d, clickCounters: %d",_buttons[i]->toString(), delayCounters(i), clickCounter(i));

                // ************* Handling "PRESS Action" *************
                if ((BTN_IS_ACTIVE_HIGH != 0 &&  nrfx_gpiote_in_is_set(pin)) || // Condition when buttons are "Active High"
                    (BTN_IS_ACTIVE_HIGH == 0 && !nrfx_gpiote_in_is_set(pin)))   // Condition when buttons are "Active Low"
                // #if (BTN_IS_ACTIVE_HIGH != 0)
                // if (nrfx_gpiote_in_is_set(pin))
                // #else
                // if (!nrfx_gpiote_in_is_set(pin))
                // #endif
                {
                    // Check the VERY_LONG press for a single click (only available on a single click)
                    if (delayCounters(i) > ButtonLongPressDelay && clickCounter(i) == 0)
                    {
                        buttonPressed(i, Button::ButtonVeryLongPress);
                        BCT_DEBUG("VERY_LONG PRESS on button %s", _buttons[i]->toString());
                    }
                    // Check the LONG press
                    else if (delayCounters(i) > ButtonSingleClickDelay)
                    {
                        if (clickCounter(i) == 0)
                        {
                            buttonPressed(i, Button::ButtonLongPress);
                            BCT_DEBUG("LONG PRESS on button %s", _buttons[i]->toString());
                        }
                        else if (clickCounter(i) == 1)
                        {
                            buttonPressed(i, Button::ButtonDoubleLongPress);
                            BCT_DEBUG("DOUBLE LONG PRESS on button %s", _buttons[i]->toString());
                        }
                        else if (clickCounter(i) == 2)
                        {
                            buttonPressed(i, Button::ButtonTripleLongPress);
                            BCT_DEBUG("TRIPLE LONG PRESS on button %s", _buttons[i]->toString());
                        }
                    }
                }

                // ************* Handling "RELEASE Action" *************
                else
                {
                    // Checks the timings for a double or triple click
                    if (delayCounters(i) >= ButtonDoubleClickDelay)
                    {
                        resetClickCounter(i);
                        BCT_DEBUG("RELEASE on %s", _buttons[i]->toString());
                        buttonReleased(i);
                    }
                }
            }
        }
        BCTD_DEBUG("-");

        bool stop = true;
        for(i = 0; i < _nbRegisteredObject; i++)
        {
            if (!_isButtonReleased(i))
            {
                BCTD_DEBUG("%d: P", i);
                stop = false;
                break;
            }
            else if ((delayCounters(i) > _resetDelayCounterValue) && (delayCounters(i) < ButtonDoubleClickDelay))
            {
                BCTD_DEBUG("delay(%d): %d", i, delayCounters(i));
                stop = false;
                break;
            }
        }

        if (stop)
        {
            _stopButtonControllerTimer();
            BCTD_DEBUG("Should have stopped the timer");
            resetCounters();
        }
    #endif
    BCTD_DEBUG("-> Finish");
}


/* **************************************************************************************************************** */
/* PROTECTED SECTION                                                                                                */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void ButtonController::onFallingEdge(gpio::Gpio* gpio)
{
    BCE_DEBUG("Start ->");
    if (gpio == NULL)
    {
        BC_ERROR("NULL pointer error !!!");
        //ASSERT(false);
        return;
    }

    uint8_t buttonIdx = indexFromId(gpio->gpioId());
    #if (BTN_IS_ACTIVE_HIGH != 0)
    // Handles the "Button Released" gesture
    _onGpioStateChanged(buttonIdx, true);
    #else
    // Handles the "Button Pressed" gesture
    _onGpioStateChanged(buttonIdx, false);
    #endif  // #if (BTN_IS_ACTIVE_HIGH != 0)
    BCE_DEBUG("-> Finish");
}

// ------------------------------------------------------------------------------------------------------------------
void ButtonController::onRisingEdge(gpio::Gpio* gpio)
{
    BCE_DEBUG("Start ->");
    if (gpio == NULL)
    {
        BCE_DEBUG("NULL pointer error !!!");
        //ASSERT(false);
        return;
    }

    uint8_t buttonIdx = indexFromId(gpio->gpioId());
    #if (BTN_IS_ACTIVE_HIGH != 0)
    // Handles the "Button Pressed" gesture
    _onGpioStateChanged(buttonIdx, false);
    #else
    // Handles the "Button Released" gesture
    _onGpioStateChanged(buttonIdx, true);
    #endif  // #if (BTN_IS_ACTIVE_HIGH != 0)
    BCE_DEBUG("-> Finish");
}


/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void ButtonController::_buttonControllerTimerInit()
{
    BC_DEBUG("Start ->");
    if (!_isHandlerTimerInitialized)
    {
        #if(USE_K_TIMER)
            k_timer_init(&_buttonHandlerTimerId, gpio::btnCtrlTimeoutHandler, NULL);
            uint32_t errCode = NRFX_SUCCESS;
        #else
            uint32_t errCode = tools::SDKTools::appTimerCreate(&_buttonHandlerTimerId,
                                                                APP_TIMER_MODE_SINGLE_SHOT,
                                                                (void*) gpio::btnCtrlTimeoutHandler);
        #endif
        if (errCode == NRFX_SUCCESS)
        {
            _isHandlerTimerInitialized = true;
            BC_DEBUG("Succeeded");
        }
        else
        {
            BC_ERROR("appTimerCreate() FAILED! (errCode: %d)", errCode);
        }
    }
    BC_DEBUG("-> Finish");
}

// ------------------------------------------------------------------------------------------------------------------
void ButtonController::_startButtonControllerTimer()
{
    BCT_DEBUG("Start ->");
    if (!_isHandlerTimerActive)
    {
        BCT_DEBUG("Starting the `app_timer` ...");

        #if(USE_K_TIMER)
            k_timer_user_data_set(&_buttonHandlerTimerId, this);
            k_timer_start(&_buttonHandlerTimerId, K_TICKS(_buttonTimerTickDelayValue), K_TICKS(_buttonTimerTickDelayValue));
            uint32_t errCode = NRFX_SUCCESS;
        #else
            uint32_t errCode = tools::SDKTools::appTimerStart(&_buttonHandlerTimerId, _buttonTimerTickDelayValue, this);
        #endif

        if (errCode == NRFX_SUCCESS)
        {
            _isHandlerTimerActive = true;
        }
        else
        {
            BCT_ERROR("appTimerStart() FAILED! (errCode: %d)", errCode);
            //ASSERT(false);
        }
    }
    BCT_DEBUG("-> Finish");
}

// ------------------------------------------------------------------------------------------------------------------
void ButtonController::_stopButtonControllerTimer()
{
    BCT_DEBUG("Start ->");
    if (_isHandlerTimerActive)
    {
        BCT_DEBUG("Stopping the `app_timer` ...");

        #if(USE_K_TIMER)
            k_timer_stop(&_buttonHandlerTimerId);
            uint32_t errCode = NRFX_SUCCESS;
        #else
            uint32_t errCode = tools::SDKTools::appTimerStop(&_buttonHandlerTimerId);
        #endif

        if (errCode == NRFX_SUCCESS)
        {
            _isHandlerTimerActive = false;
        }
        else
        {
            BCT_ERROR("appTimerStop() FAILED! (errCode: %d)", errCode);
            //ASSERT(false);
        }
    }
    BCT_DEBUG("-> Finish");
}

// ------------------------------------------------------------------------------------------------------------------
void ButtonController::_onGpioStateChanged(uint8_t index, bool buttonRelease)
{
    BCE_DEBUG("Start ->");
    if (index != 255)
    {
        static uint8_t clickCount = 0;
        incrDelayCounter(index, _resetDelayCounterValue);

        // ************* Handling the "Button Released" gesture *************
        if (buttonRelease)
        {
            BCE_DEBUG("Release action on %s.", _buttons[index]->toString());

            // checks if we are in a double or triple click action...
            if (delayCounters(index) > ButtonSingleClickDelay)
            {
                BCE_DEBUG("%s is released. Delay counter: %d", _buttons[index]->toString(), delayCounters(index));
                resetClickCounter(index);
                resetDelayCounter(index);
                buttonReleased(index);
            }
        }
        // ************* Handling the "Button Pressed" gesture *************
        else
        {
            BCE_DEBUG("Press action on %s ...", _buttons[index]->toString());

            // checks if we are in a double or triple click action...
            if (delayCounters(index) < ButtonSingleClickDelay)
            {

                incrClickCounter(index);
                clickCount = clickCounter(index);
                BCE_DEBUG("Incr. click counter for %s: %d", _buttons[index]->toString(), clickCount);

                // Handle the Single, Double and triple click
                if (clickCount == 0)
                {
                    buttonPressed(index, Button::ButtonSingleClick);
                    BCE_DEBUG("SINGLE CLICK on %s",  _buttons[index]->toString());
                }
                else if (clickCount == 1)
                {
                    buttonPressed(index, Button::ButtonDoubleClick);
                    BCE_DEBUG("DOUBLE CLICK on %s",  _buttons[index]->toString());
                }
                else if (clickCount == 2)
                {
                    buttonPressed(index, Button::ButtonTripleClick);
                    BCE_DEBUG("TRIPLE CLICK on %s",  _buttons[index]->toString());
                    resetClickCounter(index);
                }
                
                // This is for the double-click detection
                resetDelayCounter(index);
            }
        }
        
        // Starts the timer if needed.
        _startButtonControllerTimer();
    }
    BCE_DEBUG("-> Finish");
}

#endif  // #if (USE_BUTTONS != 0)
