/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * ******************************************************************************************************************/

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "picowatch.h"
#include "factory.h"
#include <picoxf.h>
#include <spinlock.h>
#include "dispcontroller.h"

#include "modem/modem_info.h"

#if (USE_BUTTONS != 0)
    using gpio::ButtonId;
#endif

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#include "debug-config.h"

#define PICO_WATCH_LOG_LEVEL 3U

#if (PICO_WATCH_LOG_LEVEL != 0) && defined(DEBUG_NRF_USER)
    #include <zephyr/logging/log.h>
    LOG_MODULE_REGISTER(pico_watch, PICO_WATCH_LOG_LEVEL);
    #define PW_ERROR(args...)              LOG_ERR(args)
    #define PW_WARN(args...)               LOG_WRN(args)
    #define PW_INFO(args...)               LOG_INF(args)
    #define PW_DEBUG(args...)              LOG_DBG(args)
    #define PW_VERBOSE(args...)            LOG_INF(args)
#else

    #define PW_ERROR(...)                  {(void) (0);}
    #define PW_WARN(...)                   {(void) (0);}
    #define PW_INFO(...)                   {(void) (0);}
    #define PW_DEBUG(...)                  {(void) (0);}
    #define PW_VERBOSE(...)                {(void) (0);}
#endif

// For State_machine debug
#if (PICO_WATCH_LOG_LEVEL != 0)
    #define PWS_DEBUG(args...)             LOG_DBG(args)
    #define PWS_WARN(args...)              LOG_WRN(args)
    #define PWS_INFO(args...)              LOG_INF(args)
    #define PWS_ERROR(args...)             LOG_ERR(args)
#else
    #define PWS_DEBUG(...)                 {(void) (0);}
    #define PWS_WARN(...)                  {(void) (0);}
    #define PWS_INFO(...)                  {(void) (0);}
    #define PWS_ERROR(...)                 {(void) (0);}
#endif

/* **************************************************************************************************************** */
/* OBJECT'S DECLARATION                                                                                             */
/* **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Namespace Declaration                                                                                            */
/* **************************************************************************************************************** */
using application::PicoWatch;

struct k_timer          PicoWatch::_picoTimers[T_NR_TIMERS];
struct k_work           PicoWatch::_picoWorks[WRK_NR_WORKS];
/*
static void rsrp_cb(char rsrp_value)
{
    int32_t rsrp = RSRP_IDX_TO_DBM(rsrp_value);
    PW_INFO("LTE RSRP : %d dBm", rsrp);
}
*/
#if (USE_MQTT != 0)
    string              PicoWatch::_mqttData;
#endif

/* **************************************************************************************************************** */
/* STATIC ATTRIBUTE DECLARATION                                                                                     */
/* **************************************************************************************************************** */

void PicoWatch::picoWatchTaskHandler(struct k_work* work_id)
{
    if (work_id == &_picoWorks[WRK_BTN_LONG_PRESS])
    {
        if ((getInstance()->_currentButtonAction != BTN_RELEASE) && (getInstance()->_currentState == ST_SET_MOT_POS))
        {
            k_timer_start(&_picoTimers[T_LONG_PRESS], K_MSEC(LONG_PRESS_HOLD_TIMER/10), K_NO_WAIT);
            getInstance()->pushEvent(&getInstance()->evLongPress_);
        }
    }
    else if (work_id == &_picoWorks[WRK_UPDATE_GNSS])
    {
        #if (USE_GNSS != 0)
            Factory::gnss()->update();
        #endif
    }
    else if (work_id == &_picoWorks[WRK_UPDATE_MQTT])
    {
        #if (USE_MQTT != 0)
            Factory::dispCtrl()->mqttDisp(Factory::dispCtrl()->DISP_MQTT_SENDING);
            if(getInstance()->sendMqttLocation()){
                Factory::dispCtrl()->mqttDisp(Factory::dispCtrl()->DISP_MQTT_CONNECTED);
                k_timer_start(&_picoTimers[T_UPDATE_MQTT], K_MSEC(getInstance()->_updateMqttPeriode), K_NO_WAIT);
            }
            else{
                Factory::dispCtrl()->mqttDisp(Factory::dispCtrl()->DISP_MQTT_DISCONNECT);
                // Start LTE connection process
                getInstance()->_currentButtonAction = BTN_A_TRPL;
                getInstance()->pushEvent(&getInstance()->evRelease_);
            }
            //getInstance()->sendMqttTimeDate();
        #endif
    }
}

void PicoWatch::picoWatchTimeoutHandler(struct k_timer *timer_id)
{
    if (timer_id == &_picoTimers[T_LONG_PRESS])
    {
        k_work_submit(&_picoWorks[WRK_BTN_LONG_PRESS]);
    }
    else if (timer_id == &_picoTimers[T_UPDATE_GNSS])
    {
        k_work_submit(&_picoWorks[WRK_UPDATE_GNSS]);
    }
    else if (timer_id == &_picoTimers[T_UPDATE_MQTT])
    {
        k_work_submit(&_picoWorks[WRK_UPDATE_MQTT]);
    }
}

/************************************************************************************************************/
/* Constructor | Destructor                                                                                 */
/************************************************************************************************************/
// ----------------------------------------------------------------------------------------------------------
PicoWatch::PicoWatch() : _currentState(ST_INITIAL)
{
    _mqttCurrentState = ST_UNKNOWN;

    evDefault_.set(PicoEvent::evDefault, this);
    evInitial_.set(PicoEvent::evInitial, this);
    evRelease_.set(PicoEvent::evReleased, this);
    evLongPress_.set(PicoEvent::evLongPress, this);

    #if (USE_MQTT != 0)
        evConnectionTimeout_.setId(evLTEConnectionKeepAlive);
        evConnectionTimeout_.setDnd(true);
        evConnectionTimeout_.setTarget(this);
        evConnectionTimeout_.setDelay(10000);

        evConnected_.set(PicoEvent::evConnected, this);
        evError_.set(PicoEvent::evError, this);

        connectionKeepAliveCounter_ = 4;
        conAttemptcounter_          = 0;

        isLteConnectProcessActive_  = false;
        isMqttConnectProcessActive_ = false;
        isLteActive_                = false;
        batLvlMvMode_               = false;
    #endif
}

/************************************************************************************************************/
/*                                              PUBLIC SECTION                                              */
/************************************************************************************************************/
// ----------------------------------------------------------------------------------------------------------
void PicoWatch::init()
{
    _currentState                           = ST_INITIAL;
    _mqttCurrentState                       = ST_INITIAL;

    #if (USE_BUTTONS != 0)
        for(uint8_t i = 0; i < _nbrButtons; i++)
        {
            _buttons[i]                     = NULL;
        }
    #endif  // #if (USE_BUTTONS != 0)

    //Timeout value
    _updateGnssPeriode      = DEFAULT_UPDATE_GNSS_P;
    _updateMqttPeriode      = DEFAULT_UPDATE_MQTT_P;

    _isMotorOn = false;

    for (size_t i = 0; i < T_NR_TIMERS; i++)
    {
       k_timer_init(&_picoTimers[i], picoWatchTimeoutHandler, NULL);
    }

    for (size_t i = 0; i < WRK_NR_WORKS; i++)
    {
       k_work_init(&_picoWorks[i], picoWatchTaskHandler);
    }
    PW_DEBUG("Init done");
}

// ----------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------
void PicoWatch::bind(
#if (USE_BUTTONS != 0) && (BUTTONS_NUMBER != 0)
      gpio::Button* buttonA
    #if (BUTTONS_NUMBER > 1)
    , gpio::Button* buttonB
    #endif  // #if (BUTTONS_NUMBER > 1)
    #if (BUTTONS_NUMBER > 2)
    , gpio::Button* buttonC
    #endif  // #if (BUTTONS_NUMBER > 2)
    #if (BUTTONS_NUMBER > 3)
    , gpio::Button* buttonD
    #endif  // #if (BUTTONS_NUMBER > 3)
#endif  // #if (USE_BUTTONS != 0) && (BUTTONS_NUMBER != 0)
)
{
    #if (USE_BUTTONS != 0)
    {
        if (buttonA == NULL || buttonB == NULL)
        {
            PW_ERROR("buttonA, buttonB NULL error !");
            //ASSERT(false);
        }
        _buttons[0]                 = buttonA;
        _buttons[1]                 = buttonB;
        _buttons[2]                 = buttonC;
        // Add button here...

         for (uint8_t i = 0; i < BUTTONS_NUMBER; i++)
            {
                if (_buttons[i] != NULL)
                {
                    _buttons[i]->addObserver(getInstance());
                }
            }
    }
    #endif
    PW_DEBUG("Binding done...");
}

PicoWatch* PicoWatch::getInstance()
{
    static PicoWatch instance;
    return &instance;
}

#if (USE_BUTTONS != 0)
// -----------------------------------------------------------------------------------------------------------
const gpio::Button* PicoWatch::button(int index) const
{
    return _buttons[index];
}

// -----------------------------------------------------------------------------------------------------------
gpio::Button* PicoWatch::button(int index)
{
    return _buttons[index];
}

// -----------------------------------------------------------------------------------------------------------
void PicoWatch::onButtonSingleClick(gpio::Button * button)
{
    PW_VERBOSE("`Single Click` on %s", button->toString());
}

// -----------------------------------------------------------------------------------------------------------
void PicoWatch::onButtonDoubleClick(gpio::Button * button)
{
    PW_VERBOSE("`Double Click` on %s", button->toString());
}

// -----------------------------------------------------------------------------------------------------------
void PicoWatch::onButtonTripleClick(gpio::Button * button)
{
    PW_VERBOSE("`Triple Click` on %s", button->toString());
}

// -----------------------------------------------------------------------------------------------------------
void PicoWatch::onButtonLongPress(gpio::Button * button)
{
    PW_VERBOSE("`Long Press` on %s", button->toString());

    switch(button->buttonId())
    {
        case gpio::ButtonId::Button1:
        {
            _currentButtonAction  = BTN_A_LNG;
            break;
        }
        case gpio::ButtonId::Button2:
        {
            _currentButtonAction = BTN_B_LNG;
            break;
        }
        case gpio::ButtonId::Button3:
        {
            _currentButtonAction = BTN_C_LNG;
            break;
        }
        default:
        {
            break;
        }
    }
    getInstance()->pushEvent(&getInstance()->evLongPress_);
}

// -----------------------------------------------------------------------------------------------------------
void PicoWatch::onButtonDoubleLongPress(gpio::Button* button)
{
    PW_VERBOSE("`Double Long Press` on %s", button->toString());
}

// -----------------------------------------------------------------------------------------------------------
void PicoWatch::onButtonTripleLongPress(gpio::Button* button)
{
    PW_VERBOSE("`Triple Long Press` on %s", button->toString());
}

// -----------------------------------------------------------------------------------------------------------
void PicoWatch::onButtonVeryLongPress(gpio::Button * button)
{
    PW_VERBOSE("`Very Long Press` on %s", button->toString());

}

// -----------------------------------------------------------------------------------------------------------
void PicoWatch::onButtonReleased(gpio::Button * button)
{
    PW_VERBOSE("`Button Released` on %s", button->toString());

    _currentButtonAction = BTN_RELEASE;

    switch(button->buttonId())
    {
        case gpio::ButtonId::Button1:
        {
            switch (Factory::buttonA.buttonLastState())
            {
                case Factory::buttonA.ButtonSingleClick:
                {
                    _currentButtonAction = BTN_A_CLC;
                    break;
                }
                case Factory::buttonA.ButtonDoubleClick:
                {
                    _currentButtonAction = BTN_A_DBL;
                    break;
                }
                case Factory::buttonA.ButtonTripleClick:
                {
                    _currentButtonAction = BTN_A_TRPL;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case gpio::ButtonId::Button2:
        {
            switch (Factory::buttonB.buttonLastState())
            {
                case Factory::buttonB.ButtonSingleClick:
                {
                    _currentButtonAction = BTN_B_CLC;
                    break;
                }
                case Factory::buttonB.ButtonDoubleClick:
                {
                    _currentButtonAction = BTN_B_DBL;
                    break;
                }
                case Factory::buttonB.ButtonTripleClick:
                {
                    _currentButtonAction = BTN_B_TRPL;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case gpio::ButtonId::Button3:
        {
            switch (Factory::buttonC.buttonLastState())
            {
                case Factory::buttonC.ButtonSingleClick:
                {
                    _currentButtonAction = BTN_C_CLC;
                    break;
                }
                case Factory::buttonC.ButtonDoubleClick:
                {
                    _currentButtonAction = BTN_C_DBL;
                    break;
                }
                case Factory::buttonC.ButtonTripleClick:
                {
                    _currentButtonAction = BTN_C_TRPL;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
        {
            break;
        }
    }

    pushEvent(&evRelease_);
}
#endif // #if (USE_BUTTONS != 0)

// ------------------------------------------------------------------------------------------------------------------
void PicoWatch::pushEvent(PicoEvent* event)
{
    Factory::picoXF()->pushEvent(event);
}

void PicoWatch::startBehaviour(void)
{
    pushEvent(&evInitial_);
}

//------------------------------------------------------------------------------------------------------------------
bool PicoWatch::processEvent(PicoEvent* event)
{
    bool ret = true;

    // Storing old state...
    ePicoWatchState oldState = _currentState;
    _mqttOldState = _mqttCurrentState;

    // ----------- The Transition Switch -------------------------------------------------------
    switch (_currentState)
    {
        case ST_INITIAL:
        {
            if (event->getId() == PicoEvent::evInitial)
            {
                _currentState = ST_INIT;
            }
            break;
        }
        case ST_INIT:
        {
            if (event->getId() == PicoEvent::evDefault)
            {
                _currentState = ST_SET_MOT_POS;
            }
            break;
        }
        case ST_IDLE:
        {
            if (event->getId() == PicoEvent::evReleased)
            {
                if (_currentButtonAction == BTN_A_TRPL)
                {
                    isLteActive_ = true;
                    _mqttCurrentState = ST_WAIT;
                    _currentState = ST_IDLE;
                    pushDefaultEvent_();
                }
                else if (_currentButtonAction == BTN_A_CLC)
                {
                    _currentState = ST_UPDATE_LOCAL_TIME;
                }
                else if (_currentButtonAction == BTN_C_CLC)
                {
                    _currentState = ST_GET_BAT_MANAGER_CONFIG;
                }
                else if (_currentButtonAction == BTN_C_DBL)
                {
                    if (batLvlMvMode_)
                    {
                        batLvlMvMode_ = false;
                    }
                    else
                    {
                        batLvlMvMode_ = true;
                    }
                }
                else if (_currentButtonAction == BTN_C_TRPL)
                {
                    _currentState = ST_SET_BAT_MANAGER_CONFIG;
                }
            }
            else if (event->getId() == PicoEvent::evLongPress)
            {
                if (_currentButtonAction == BTN_C_LNG)
                {
                    _currentState = ST_CLEAR_BAT_MANAGER_FLAGS;
                }
                if (_currentButtonAction == BTN_A_LNG)
                {
                    _currentState = ST_GPS_TRACKING_MODE;
                }
            }
            break;
        }
        case ST_SET_MOT_POS:
        {
            if (event->getId() == PicoEvent::evReleased)
            {
                if (_currentButtonAction == BTN_B_TRPL)
                {
                    _currentState = ST_SET_GNSS_ANT;
                }
            }
            break;
        }
        case ST_SET_GNSS_ANT:
        {
            if (event->getId() == PicoEvent::evReleased)
            {
                if (_currentButtonAction == BTN_B_TRPL)
                {
                    _currentState = ST_CONNECT;
                }
                if (_currentButtonAction == BTN_A_TRPL)
                {
                    _currentState = ST_SET_MOT_POS;
                }
            }
            break;
        }
        case ST_CONNECT:
        {
            if (event->getId() == PicoEvent::evDefault)
            {
                _currentState = ST_IDLE;
            }
            break;
        }
        case ST_UPDATE_LOCAL_TIME:
            {
                if (event->getId() == PicoEvent::evDefault)
                {
                    _currentState = ST_IDLE;
                }
                break;
            }
        case ST_GET_BAT_MANAGER_CONFIG:
        {
            if (event->getId() == PicoEvent::evDefault)
            {
                _currentState = ST_IDLE;
            }
            break;
        }
        case ST_CLEAR_BAT_MANAGER_FLAGS:
        {
            if (event->getId() == PicoEvent::evDefault)
            {
                _currentState = ST_IDLE;
            }
            break;
        }
        case ST_GPS_TRACKING_MODE:
        {
            if (event->getId() == PicoEvent::evLongPress)
            {
                if (_currentButtonAction == BTN_A_LNG)
                {
                    _currentState = ST_IDLE;
                }
            }
            break;
        }
        default:
            ret = false;
            break;
    }

    // ----------- The Action Switch -------------------------------------------------------
    if((oldState != _currentState) || (event->getId() == PicoEvent::evReleased) || (event->getId() == PicoEvent::evLongPress))
    {
        switch (_currentState)
        {
            case ST_INITIAL:
                pushDefaultEvent_();
                break;
            case ST_INIT:
            {
                PWS_WARN("-> ST_INIT");
                pushDefaultEvent_();
                break;
            }
            case ST_IDLE:
            {
                PWS_WARN("-> ST_IDLE");
                char modeStr[20] = {0};
                sprintf(modeStr, "LTEWatch");
                Factory::dispCtrl()->setModeDisp(modeStr);
                break;
            }
            case ST_SET_MOT_POS:
            {
                if (oldState != _currentState)
                {
                    PWS_WARN("-> ST_SET_MOT_POS");
                    Factory::dispCtrl()->cleanDisp();
                }
                setMotorPosition(_currentButtonAction);
                break;
            }
            case ST_SET_GNSS_ANT:
            {
                if (oldState != _currentState)
                {
                    PWS_WARN("-> ST_SET_GNSS_ANT");
                    Factory::dispCtrl()->cleanDisp();
                }
                setGnssAnt(_currentButtonAction);
                break;
            }
            case ST_CONNECT:
            {
                PWS_WARN("-> ST_CONNECT");
                Factory::dispCtrl()->cleanDisp();

                // Battery Charger
                Factory::batManager()->start();

                //Display
                Factory::dispCtrl()->start();

                // GNSS
                Factory::gnss()->connect();

                // MQTT
                isLteActive_ = true;
                _mqttCurrentState = ST_WAIT;
                //modem_info_rsrp_register(rsrp_cb);

                _isMotorOn = true;

                #if (USE_CALENDAR != 0)
                PWS_DEBUG("starting calendar");
                Factory::calendar.startCalendar();
                #endif

                pushDefaultEvent_();
                break;
            }
            case ST_UPDATE_LOCAL_TIME:
            {
                PWS_WARN("-> ST_UPDATE_LOCAL_TIME");
                //Update Calender Time from GNSS
                Factory::calendar.setSecond(0, _gnssDateTime.sec);
                Factory::calendar.setMinute(0, _gnssDateTime.min);
                Factory::calendar.setHour(0, _gnssDateTime.hour + 1);

                Factory::calendar.getDateTime(0)->setYear(_gnssDateTime.year, false);
                Factory::calendar.getDateTime(0)->setMonth(_gnssDateTime.month-1, false);
                Factory::calendar.getDateTime(0)->setDay(_gnssDateTime.day, false);
                pushDefaultEvent_();
                break;
            }
            case ST_GET_BAT_MANAGER_CONFIG:
            {
                PWS_WARN("-> ST_GET_BAT_MANAGER_CONFIG");
                Factory::batManager()->getBatteryChargerConfig();
                pushDefaultEvent_();
                break;
            }
            case ST_SET_BAT_MANAGER_CONFIG:
            {
                PWS_WARN("-> ST_SET_BAT_MANAGER_CONFIG");
                Factory::batManager()->configBatteryCharger();
                pushDefaultEvent_();
                break;
            }
            case ST_CLEAR_BAT_MANAGER_FLAGS:
            {
                PWS_WARN("-> ST_CLEAR_BAT_MANAGER_FLAGS");
                Factory::batManager()->clearBatteryChargerFlags();
                pushDefaultEvent_();
                break;
            }
            case ST_GPS_TRACKING_MODE:
            {
                _updateMqttPeriode = 1000;
                _updateGnssPeriode = 1000;
                char modeStr[20] = {0};
                sprintf(modeStr, "Tracking Mode");
                Factory::dispCtrl()->setModeDisp(modeStr);
                break;
            }
            default:
                ret = false;
                PWS_ERROR("ERROR: Next State is not valid of not implemented yet");
                break;
        }
    }
    // Internal MQTT process event
    if(isLteActive_)
    {
        mqttInternalProcessEvent(event);
    }
    return ret;
}

// -----------------------------------------------------------------------------------------------------------

#if(USE_MQTT != 0)
void PicoWatch::mqttInternalProcessEvent(PicoEvent* event)
{
     // ----------- The Transition Switch -------------------------------------------------------
    switch (_mqttCurrentState)
    {
        case ST_IDLE:
        {
            break;
        }
        case ST_WAIT:
        {
            if (event->getId() == evLTEConnectionKeepAlive)
            {
                _mqttCurrentState = ST_CONNECTION_KEEPALIVE;
            }
            else if ((event->getId() == PicoEvent::evConnected)||(event->getId() == PicoEvent::evError))
            {
                _mqttCurrentState = ST_IDLE;
            }
            break;
        }
        case ST_CONNECTION_KEEPALIVE:
        {
            if (event->getId() == PicoEvent::evDefault)
            {
                _mqttCurrentState = ST_WAIT;
            }
            break;
        }
        default:
            break;
    }

    // ----------- The Action Switch -------------------------------------------------------
     if(_mqttOldState != _mqttCurrentState)
    {
        switch (_mqttCurrentState)
        {
            case ST_IDLE:
            {
                PWS_WARN("-> ST_MQTT_IDLE");
                break;
            }
            case ST_WAIT:
            {
                PWS_WARN("-> ST_MQTT_WAIT");
                startConnectionTimeout_();      // Start LTE connect precess
                break;
            }
             case ST_CONNECTION_KEEPALIVE:
            {
                PWS_WARN("-> ST_MQTT_KEEPALIVE");
                if (connectionKeepAliveCounter_++ >= 4)
                {
                    connectionKeepAliveCounter_ = 0;

                    if (!Factory::lte()->isConnected() && !isLteConnectProcessActive_)
                    {
                        PWS_WARN("LTE not connected ! -> Reconnect");
                        if(Factory::lte()->connect())
                        {
                            isLteConnectProcessActive_ = true;
                            PWS_INFO("Waiting for LTE connecting...");
                            // Setting "waitForConnection" non-blocking with K_NO_WAIT ! With K_FOREVER this method becomes blocking
                            Factory::lte()->waitForConnection(K_NO_WAIT);
                            Factory::dispCtrl()->mqttDisp(Factory::dispCtrl()->DISP_MQTT_CONNECTING_1);
                        }
                        else
                        {
                            PWS_ERROR("Factory::lte()->connect() FAILED !");
                            Factory::dispCtrl()->mqttDisp(Factory::dispCtrl()->DISP_MQTT_ERROR);
                        }
                    }
                    else
                    {
                        isLteConnectProcessActive_  = false;
                        isMqttConnectProcessActive_ = false;
                    }
                }
                pushDefaultEvent_();
                break;
            }
            default:
                break;
        }
    }
}
#endif

// -----------------------------------------------------------------------------------------------------------
void PicoWatch::nextID(uint8_t* pID, uint8_t MAX, uint8_t MIN)
{
    if((*pID) < (MAX))
    {
        (*pID)++;
    }
    else
    {
        (*pID) = MIN;
    }
}

//------------------------------------------------------------------------------------------------------------------
void PicoWatch::previousID(uint8_t* pID, uint8_t MAX, uint8_t MIN)
{
    if((*pID) > (MIN))
    {
        (*pID)--;
    }
    else
    {
        (*pID) = MAX;
    }
}
/************************************************************************************************************/
/*                                             PRIVATE SECTION                                              */
/************************************************************************************************************/

// -----------------------------------------------------------------------------------------------------------
void PicoWatch::_ST_SYSTEM_OFF_Action()
{
	PW_INFO("Setup the pin to wake up the device...");
    nrf_gpio_cfg_sense_input(BUTTON_2, BUTTON_PULL_SETUP, NRF_GPIO_PIN_SENSE_LOW);
	PW_DEBUG("Device enter system OFF mode...");
}

//------------------------------------------------------------------------------------------------------------------
void PicoWatch::pushDefaultEvent_(int delay)
{
    if (evDefault_.getDelay() != 0)
    {
        PicoXF::getInstance()->unscheduleTM(&evDefault_);
    }

    evDefault_.setDelay(delay);
    pushEvent(&evDefault_);
}

#if (USE_BUTTONS != 0) && (BUTTONS_NUMBER != 0)
//------------------------------------------------------------------------------------------------------------------
void PicoWatch::setMotorPosition(uint8_t buttonAction)
{
    static uint8_t motorID;
    if (motorID < 0) motorID = 0;

    switch (_currentButtonAction)
    {
        PWS_DEBUG("Motor: %d", motorID);
        case BTN_B_CLC:
            // Move motor backward for a single step
            /* Def: ctrlMotor(uint8_t motorIds, bool forward, uint16_t nbSteps, bool updatePosition, bool fastMove, bool asynchroneDirection)*/
            Factory::motorController.motorDriver().ctrlMotor(BIT(motorID), true, 1, true, true, false);
            break;
        case BTN_B_LNG:
            // Move motor forward for multiple steps
            Factory::motorController.motorDriver().ctrlMotor(BIT(motorID), true, 10, false, true, false);
            // Start hold long press detection timeout
            k_timer_start(&_picoTimers[T_LONG_PRESS],K_MSEC(LONG_PRESS_HOLD_TIMER), K_NO_WAIT);
            break;
        case BTN_A_CLC:
            // Move motor backward for a single step
            Factory::motorController.motorDriver().ctrlMotor(BIT(motorID), false, 1, true, true, false);
            break;
        case BTN_A_LNG:
            // Move motor forward for multiple steps
            Factory::motorController.motorDriver().ctrlMotor(BIT(motorID), false, 10, false, true, false);
            // Start hold long press detection timeout
            k_timer_start(&_picoTimers[T_LONG_PRESS],K_MSEC(LONG_PRESS_HOLD_TIMER), K_NO_WAIT);
            break;
        case BTN_B_DBL:
            // Next motor id
            nextID(&motorID, NB_MAX_MOTORS-1, 0);
            break;
        case BTN_A_DBL:
            // Next motor id
            previousID(&motorID, NB_MAX_MOTORS-1, 0);
            break;
        default:
            break;
    }
    // Set current motor new "zero" position
    Factory::motorController.motorDriver().getMotorById((motor::MotorId::eMotorId)(BIT(motorID)))->setPosition(0);

    Factory::dispCtrl()->setMotDisp(motorID);
    PWS_DEBUG("Change position of motor: %d", motorID);
}
#endif

// ------------------------------------------------------------------------------------------------------------------
void PicoWatch::setGnssAnt(uint8_t buttonAction)
{
    static uint8_t gnssAnt;
    uint8_t gnssAntList[2] = {0,2};
    if (gnssAnt<0) gnssAnt = 0;
    switch (_currentButtonAction)
    {
        case BTN_B_CLC:
            // Move motor forward for 1 step
            PWS_DEBUG("+");
            nextID(&gnssAnt, sizeof(gnssAntList)/sizeof(gnssAntList[0])-1, 0);
            Factory::gnss()->setLNAMode(gnssAntList[gnssAnt]);
            break;
        case BTN_A_CLC:
            PWS_DEBUG("-");
            // Move motor backward for 1 step
            previousID(&gnssAnt, sizeof(gnssAntList)/sizeof(gnssAntList[0])-1, 0);
            Factory::gnss()->setLNAMode(gnssAntList[gnssAnt]);
            break;
        default:
            break;
    }
    Factory::dispCtrl()->setGnssAnt(gnssAntList[gnssAnt]);
    PWS_DEBUG("Change GNSS antenna setting: %d", gnssAnt);
}

#if(USE_MQTT != 0)
// LTEObserver Implementation
// ------------------------------------------------------------------------------------------------------------------
void PicoWatch::onConnected(LTE* lte)
{
    PW_INFO("LTE get connected!");

    isLteConnectProcessActive_  = false;
    if (!isMqttConnectProcessActive_)
    {
        isMqttConnectProcessActive_ = true;
        if (!Factory::mqtt()->connect())
        {
            isMqttConnectProcessActive_ = false;
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void PicoWatch::onDisconnected(LTE* lte)
{
    PW_INFO("LTE has disconnected");

    Factory::dispCtrl()->mqttDisp(Factory::dispCtrl()->DISP_MQTT_DISCONNECT);

    isLteConnectProcessActive_  = false;
    isLteActive_ = false;

    if (Factory::mqtt()->isConnected())
    {
        LOG_DBG("MQTT still connected... Should disconnect !");
        Factory::mqtt()->disconnect();
    }
}

// ------------------------------------------------------------------------------------------------------------------
void PicoWatch::onError(LTE* lte)
{
    PW_WARN("LTE get in trouble !");

    isLteConnectProcessActive_  = false;
    isMqttConnectProcessActive_ = false;
    Factory::dispCtrl()->mqttDisp(Factory::dispCtrl()->DISP_MQTT_ERROR);

    if (Factory::lte()->isConnected())
    {
        PW_DEBUG("LTE should disconnect...\n");
        Factory::lte()->disconnect();
    }
    pushEvent(&evError_);
}

// ------------------------------------------------------------------------------------------------------------------
void PicoWatch::onConnected(MQTTController* mqtt)
{
    PW_INFO("MQTT is connected to %s", CONFIG_MQTT_BROKER_HOSTNAME);

    isMqttConnectProcessActive_ = false;

    k_timer_start(&_picoTimers[T_UPDATE_MQTT], K_MSEC(_updateMqttPeriode), K_NO_WAIT);

    Factory::dispCtrl()->mqttDisp(Factory::dispCtrl()->DISP_MQTT_CONNECTED);
    pushEvent(&evConnected_);
}

// ------------------------------------------------------------------------------------------------------------------
void PicoWatch::onDisconnected(MQTTController* mqtt)
{
    PW_INFO("MQTT has disconnected from %s", CONFIG_MQTT_BROKER_HOSTNAME);

    isMqttConnectProcessActive_ = false;
    Factory::dispCtrl()->mqttDisp(Factory::dispCtrl()->DISP_MQTT_DISCONNECT);

    if (Factory::lte()->isConnected())
    {
        PW_DEBUG("LTE should disconnect...\n");
        Factory::lte()->disconnect();
    }
}

// ------------------------------------------------------------------------------------------------------------------
void PicoWatch::onError(MQTTController* mqtt, int error)
{
    PW_ERROR("MQTT Error: %d", error);
    if (mqtt->isConnected())
    {
        mqtt->disconnect();
    }
    Factory::dispCtrl()->mqttDisp(Factory::dispCtrl()->DISP_MQTT_ERROR);

    isMqttConnectProcessActive_ = false;
    isLteConnectProcessActive_  = false;

    pushEvent(&evError_);
}

//------------------------------------------------------------------------------------------------------------------
bool PicoWatch::sendMqttLocation()
{
    string json;
    if (_gnssLocation.svs > 0)
    {
        if (_currentState != ST_GPS_TRACKING_MODE)
        {
            if (_gnssLocation.speedMillimetresPerSecond >= 20000)
            {
                _updateMqttPeriode = 2000;
                _updateGnssPeriode = 1000;
            }
            else if (_gnssLocation.speedMillimetresPerSecond >= 10000)
            {
                _updateMqttPeriode = 4000;
                _updateGnssPeriode = 2000;
            }
            else if (_gnssLocation.speedMillimetresPerSecond >= 1000)
            {
                _updateMqttPeriode = 8000;
                _updateGnssPeriode = 4000;
            }
            else
            {
                _updateMqttPeriode = DEFAULT_UPDATE_MQTT_P;
                _updateGnssPeriode = DEFAULT_UPDATE_GNSS_P;
            }
        }
        LOG_INF("Push location messages to MQTT...");
        //Send Latitude in degree
        json = "{\"latitude\":" + std::to_string(_gnssLocation.latitudeX1e7) + "}";
        if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
            return false;
        }

        //Send Longitude in degree
        json = "{\"longitude\":" + std::to_string(_gnssLocation.longitudeX1e7) + "}";
        if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
            return false;
        }

        //Send altitude in meters
        json = "{\"altitude\":" + std::to_string((_gnssLocation.altitudeMillimetres/1000)) + "}";
        if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
            return false;
        }

        //Send radius in meters
        json = "{\"radius\":" + std::to_string((_gnssLocation.radiusMillimetres/1000)) + "}";
        if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
            return false;
        }

        //Send speed in m/s
        json = "{\"speed\":" + std::to_string((_gnssLocation.speedMillimetresPerSecond)) + "}";
        if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
            return false;
        }

        //Send timeutc
        json = "{\"timeUtc\":" + std::to_string(_gnssLocation.timeUtc) + "}";
        if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
            return false;
        }
    }
    //Send svs
    json = "{\"svs\":" + std::to_string(_gnssLocation.svs) + "}";
    if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
        return false;
    }

    //Send Battery statistics
    json = "{\"isBatCharging\":" + std::to_string(batData_.isBatCharging) + "}";
    if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
        return false;
    }

    json = "{\"batLvlInMV\":" + std::to_string(batData_.batLvlInMV) + "}";
    if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
        return false;
    }

    json = "{\"batLvlInPercent\":" + std::to_string(batData_.batLvlInPercent) + "}";
    if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
        return false;
    }

    _updateMqttPeriode = 4*DEFAULT_UPDATE_MQTT_P;
    _updateGnssPeriode = 4*DEFAULT_UPDATE_GNSS_P;
    return true;
}

//------------------------------------------------------------------------------------------------------------------
bool PicoWatch::sendMqttTimeDate()
{
    //Send year
    string json = "{\"year\":" + std::to_string(_gnssDateTime.year) + "}";
    if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
        return false;
    }

    //Send month
    json = "{\"month\":" + std::to_string(_gnssDateTime.month) + "}";
    if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
        return false;
    }

    //Send day
    json = "{\"day\":" + std::to_string((_gnssDateTime.day)) + "}";
    if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
        return false;
    }

    //Send hour
    json = "{\"hour\":" + std::to_string((_gnssDateTime.hour)) + "}";
    if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
        return false;
    }

    //Send day
    json = "{\"min\":" + std::to_string((_gnssDateTime.min)) + "}";
    if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
        return false;
    }

    //Send day
    json = "{\"sec\":" + std::to_string((_gnssDateTime.sec)) + "}";
    if(!sendMqttMessage_(Factory::mqtt(), json, CONFIG_MQTT_PUB_TOPIC)){
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------------------------------------------
bool PicoWatch::sendMqttMessage_(MQTTController* mqtt, string json, string topic)
{
    if (Factory::lte()->isConnected())
    {
        if (mqtt != nullptr && mqtt->isConnected())
        {
            LOG_DBG("pushing MQTT message: %s...", json.c_str());
            mqtt->addMessage(json, topic);
            return true;
        }
        else
        {
            LOG_WRN("MQTT not connected, %s", json.c_str());
            pushEvent(&evError_);
            return false;
        }
    }
    else
    {
        LOG_WRN("LTE is not connected -> Try to Reconnect...");
        return false;
    }
}

//------------------------------------------------------------------------------------------------------------------
void PicoWatch::startConnectionTimeout_()
{
    // Just to be sure...
    Factory::picoXF()->unscheduleTM(&evConnectionTimeout_);

    evConnectionTimeout_.setDelay(LTE_CONNECTION_TIMEOUT);
    LOG_WRN("Starting Connection's check timeout of %d [ms]...", evConnectionTimeout_.getDelay());
    Factory::picoXF()->pushEvent(&evConnectionTimeout_);
}

//------------------------------------------------------------------------------------------------------------------
void PicoWatch::disconnect_()
    {
        if (Factory::mqtt()->isConnected())
        {
            Factory::mqtt()->disconnect();
        }
    }

//------------------------------------------------------------------------------------------------------------------
void PicoWatch::mqttConnect_(void)
{
    if (Factory::lte()->isConnected())
    {
        if (!isMqttConnectProcessActive_)
        {
            isMqttConnectProcessActive_ = Factory::mqtt()->connect();
        }
    }
    else
    {
        PWS_ERROR("LTE not connected -> MQTT connection aborted...");
        pushEvent(&evError_);
    }
}
#endif

#if (USE_CALENDAR != 0)
//------------------------------------------------------------------------------------------------------------------
    void PicoWatch::onSecond(cal::Calendar* calendar)
    {
        if (_isMotorOn)
        {
            Factory::motorController.setDestinationsForTime(calendar->getDateTime(0),true,false,false,0,100,0U,true,false);

            PWS_DEBUG("->Date: %02d/%02d/%04d, Time: %02d:%02d:%02d",
            calendar->day(0), calendar->month(0), calendar->year(0),
            calendar->hour(0), calendar->minute(0), calendar->second(0));

            Factory::dispCtrl()->gnssDisp(Factory::gnss()->getGnssData().svs);
            Factory::dispCtrl()->timeDisp(Factory::calendar.hour(0), Factory::calendar.minute(0), Factory::calendar.second(0));
            Factory::dispCtrl()->dateDisp(Factory::calendar.year(0), Factory::calendar.month(0), Factory::calendar.day(0));
        }
    }
#endif


#if (USE_GNSS != 0)
    //------------------------------------------------------------------------------------------------------------------
    void PicoWatch::onReceive(GnssController* gnss)
    {
        //Update Data from GNSS
        _gnssLocation = Factory::gnss()->getGnssData();
        _gnssDateTime = Factory::gnss()->getGnssDate();

        k_timer_start(&_picoTimers[T_UPDATE_GNSS], K_MSEC(_updateGnssPeriode), K_NO_WAIT);
    }
    //------------------------------------------------------------------------------------------------------------------
    void PicoWatch::onConnected(GnssController* gnss)
    {
        k_timer_start(&_picoTimers[T_UPDATE_GNSS], K_MSEC(_updateGnssPeriode), K_NO_WAIT);
    }
    //------------------------------------------------------------------------------------------------------------------
    void PicoWatch::onDisconnected(GnssController* gnss)
    {
    }
    //------------------------------------------------------------------------------------------------------------------
    void PicoWatch::onError(GnssController* gnss)
    {
    }
#endif

//------------------------------------------------------------------------------------------------------------------
void  PicoWatch::onInterrupt(BatteryManager* batterManager)
{
    //nothing yet
    PW_WARN("Received an interrupt from battery manager!");
}

//------------------------------------------------------------------------------------------------------------------
void  PicoWatch::onBatLvlValReady(BatteryManager* batterManager)
{
    Factory::batManager()->getBatData(&batData_);

    PWS_DEBUG("-> Battery level:%d%% = %dmV", batData_.batLvlInPercent, batData_.batLvlInMV);
    Factory::dispCtrl()->battDisp(batData_.batLvlInPercent, batData_.batLvlInMV, batLvlMvMode_, batData_.isBatCharging);
}

//------------------------------------------------------------------------------------------------------------------
void  PicoWatch::onError(BatteryManager* batterManager, int error)
{
    //nothing yet
    PW_ERROR("Received an error from battery manager!");
}