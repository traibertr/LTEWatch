/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * ******************************************************************************************************************/

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "motorcontroller.h"

#if (USE_MOTORS != 0)

#include <factory.h>
#include "eventid.h"
#include "motorid.h"
#include "motor.h"
#include "debug-config.h"

#include <zephyr/logging/log.h>
    LOG_MODULE_REGISTER(motor_controler, LOG_LEVEL_DBG); 

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#define DEBUG_ON                            0
#define DEBUG_MOTOR_CONTROLLER_ENABLE       DEBUG_ON 
#define DEBUG_MOTOR_CONTROLLER_ST_ENABLE    DEBUG_ON
#define DEBUG_MOTOR_CONTROLLER_DEBUG_ENABLE DEBUG_ON

#if (DEBUG_MOTOR_CONTROLLER_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #define MOC_DEBUG(args...)              LOG_DBG(args)
    #define MOC_ERROR(args...)              LOG_ERR(args) 
#else
    #define MOC_DEBUG(...)                  {(void) (0);}
    #define MOC_ERROR(...)                  {(void) (0);}
#endif

#if (DEBUG_MOTOR_CONTROLLER_ST_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #define MOCS_DEBUG(args...)             LOG_DBG(args) 
    #define MOCS_ERROR(args...)             LOG_ERR(args) 
#else
    #define MOCS_DEBUG(...)                 {(void) (0);}
    #define MOCS_ERROR(...)                 {(void) (0);}
#endif

#if (DEBUG_MOTOR_CONTROLLER_DEBUG_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #define MCD_DEBUG(args...)              LOG_DBG(args)
    #define MCD_ERROR(args...)              LOG_ERR(args) 
#else
    #define MCD_DEBUG(...)                  {(void) (0);}
    #define MCD_ERROR(...)                  {(void) (0);}
#endif


/* **************************************************************************************************************** */
/* Namespace's Declaration                                                                                          */
/* **************************************************************************************************************** */
using motor::MotorController;


/* **************************************************************************************************************** */
/* CONSTRUCTOR                                                                                                      */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
MotorController::MotorController() : _currentState(ST_INIT), _motorIds(0) {}

// ------------------------------------------------------------------------------------------------------------------
MotorController::~MotorController() {}


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void MotorController::initialize(bool hotReset)
{
    if (hotReset)
    {
        _currentState = ST_INIT_HOT;
    }
    else
    {
        _currentState = ST_INIT;
    }
    MOC_DEBUG("_currentState: %s\r\n", _state_to_string());

    _mDriver.initialize(MOTOR_PULSE_LEN, MOTOR_SINLGE_GAP_TIME, MOTOR_MIN_GAP_TIME);
    if (!_mDriver.addObserver(this))
    {
        MOC_ERROR("Impossible to attach mController to mDriver !\r\n");
    }
}

// ------------------------------------------------------------------------------------------------------------------
#if (SMS_SUPPORT != 0)
    void MotorController::bind(ble::SmsService* smsSrv)
    {
        _mDriver.bind(smsSrv);
    }
#endif

// ------------------------------------------------------------------------------------------------------------------
void MotorController::startBehaviour()
{
    _mDriver.startBehaviour();
    XFReactive::startBehaviour();
}

// ------------------------------------------------------------------------------------------------------------------
void MotorController::raiseMotorCmdEvent(bool calendarEvent)
{
    _motorEvent.setCalendarEvent(calendarEvent);
    MCD_DEBUG("pushEvent(&_motorEvent)\r\n");
    
    // Push event in the critical queue to react as quick as possible
    this->pushCriticalEvent(&_motorEvent);
}

// ------------------------------------------------------------------------------------------------------------------
void MotorController::raiseMotorInitEvent(motor::MotorId::eMotorId motorId, bool forward, bool fast, bool updatePosition)
{
    _mDriver.initMotor(motorId, forward, fast);
    MCD_DEBUG("motorId: 0x%02x, forward: %s, fast: %s\r\n", motorId, (forward ? "Yes" : "No"), (fast ? "Yes" : "No"));
    
    // Push event in the critical queue to react as quick as possible
    this->pushCriticalEvent(&_motorInitEvent);
}

// ------------------------------------------------------------------------------------------------------------------
uint16_t MotorController::getStepsForInitAnimation(uint8_t motorId)
{
    MotorId::eMotorId motId = _mDriver.checkMotorId(motorId);
    if (motId != MotorId::UnknownMotor)
    {
        uint16_t nbSteps = _mDriver.nbStepsForAnimation(motId);
        MOC_DEBUG("motorId: %d (0x%02x), nbSteps: %d\r\n", motorId, motId, nbSteps);
        return nbSteps;
    }
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorController::setDestination(MotorId::eMotorId motorId,
                                     uint16_t destination,
                                     int8_t forcedDirection,
                                     bool autoStart,
                                     bool calendarEvent)
{
    _mDriver.setUpdatePosition(true);
    _mDriver.getMotorById(motorId)->setForceDirection(forcedDirection);
    MCD_DEBUG("motorId: 0x%02x, destination: %d, forcedDirection: %s\r\n", motorId, destination, (forcedDirection ? "Yes" : "No"));
    if (_mDriver.setDestination(motorId, destination))
    {
        _autoStartMove(autoStart, calendarEvent);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorController::setDestinationForTicks(MotorId::eMotorId motorId,
                                             uint8_t tick,
                                             int8_t forcedDirection,
                                             bool autoStart,
                                             bool calendarEvent)
{
    _mDriver.setUpdatePosition(true);
    _mDriver.getMotorById(motorId)->setForceDirection(forcedDirection);
    MCD_DEBUG("motorId: 0x%02x, tick: %d, forcedDirection: %s\r\n", motorId, tick, (forcedDirection ? "Yes" : "No"));
    if (_mDriver.setDestinationForTicks(motorId, tick))
    {
        _autoStartMove(autoStart, calendarEvent);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorController::setDestForSecond(MotorId::eMotorId motorId,
                                       uint8_t second,
                                       bool updatePosition,
                                       uint8_t mirroring,
                                       int8_t forcedDirection,
                                       bool autoStart,
                                       bool calendarEvent)
{
    _mDriver.getMotorById(motorId)->setForceDirection(forcedDirection);
    _mDriver.setUpdatePosition(updatePosition);
    MCD_DEBUG("motorId: 0x%02x, second: %d, forcedDirection: %s\r\n", motorId, second, (forcedDirection ? "Yes" : "No"));
    if (_mDriver.setDestForSecond(motorId, second, mirroring))
    {
        _autoStartMove(autoStart, calendarEvent);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorController::setDestForMinute(MotorId::eMotorId motorId,
                                       uint8_t minute,
                                       uint8_t second,
                                       bool updatePosition,
                                       uint8_t mirroring,
                                       int8_t forcedDirection,
                                       bool autoStart,
                                       bool calendarEvent)
{
    _mDriver.getMotorById(motorId)->setForceDirection(forcedDirection);
    _mDriver.setUpdatePosition(updatePosition);
    MCD_DEBUG("motorId: 0x%02x, minute: %d (second: %d), forcedDirection: %s\r\n", motorId, minute, second, (forcedDirection ? "Yes" : "No"));
    if (_mDriver.setDestForMinute(motorId, minute, second, mirroring))
    {
        _autoStartMove(autoStart, calendarEvent);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorController::setDestForHour(MotorId::eMotorId motorId,
                                     uint16_t hour,
                                     uint8_t minute,
                                     bool updatePosition,
                                     uint8_t mirroring,
                                     int8_t forcedDirection,
                                     bool autoStart,
                                     bool calendarEvent)
{
    _mDriver.getMotorById(motorId)->setForceDirection(forcedDirection);
    _mDriver.setUpdatePosition(updatePosition);
    MCD_DEBUG("motorId: 0x%02x, hour: %d (minute: %d), forcedDirection: %s\r\n", motorId, hour, minute, (forcedDirection ? "Yes" : "No"));
    if (_mDriver.setDestForHour(motorId, hour, minute, mirroring))
    {
        _autoStartMove(autoStart, calendarEvent);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorController::setDestinations(uint16_t* destinations,
                                      uint8_t size,
                                      bool updatePosition,
                                      int8_t forcedDirection,
                                      bool autoStart,
                                      bool calendarEvent)
{
    bool stepNeeded = false;
    for(uint8_t i = 0; i < size; i++)
    {
        if (_mDriver.setDestination((MotorId::eMotorId)(1 << i), destinations[ i ]))
        {
            stepNeeded = true;
        }
        _mDriver.getMotorById((MotorId::eMotorId)(1 << i))->setForceDirection(forcedDirection);
    }

    if (stepNeeded)
    {
        _mDriver.setUpdatePosition(updatePosition);
        _autoStartMove(autoStart, calendarEvent);
    }
    return stepNeeded;
}

#if (USE_CALENDAR != 0)
// ------------------------------------------------------------------------------------------------------------------
bool MotorController::setDestinationsForTime(cal::DateTime* time,
                                             bool    adjustSecond,
                                             bool    secondAtNoon,
                                             bool    displayWeekNumber,
                                             int8_t  forcedDirection,
                                             int8_t  batteryLevel,
                                             uint8_t mirroring,
                                             bool    autoStart,
                                             bool    calendarEvent)
{
    uint8_t dayOrWeek   = time->day();
    if (displayWeekNumber)
    {
        dayOrWeek       = time->weekNumber();
    }
    return setDestinationsForTime(adjustSecond, secondAtNoon, time->second(), time->minute(), time->hour(), dayOrWeek, batteryLevel, forcedDirection, mirroring, autoStart, calendarEvent);
}
#endif  // #if (USE_CALENDAR != 0)

// ------------------------------------------------------------------------------------------------------------------
bool MotorController::setDestinationsForTime(bool    adjustSecond,
                                             bool    secondAtNoon,
                                             uint8_t second,
                                             uint8_t minute,
                                             uint8_t hour,
                                             uint8_t dayOrWeek,
                                             int8_t  batteryLevel,
                                             int8_t  forcedDirection,
                                             uint8_t mirroring,
                                             bool    autoStart,
                                             bool    calendarEvent)
{
    MOC_DEBUG("setDestinationsForTime", "%02d:%02d:%02d (day or week number: %02d)\r\n", hour, minute, second, dayOrWeek);
    bool needMove       = _mDriver.setDestForMinute(motor::MotorId::Motor2, minute, second, mirroring);
    needMove           |= _mDriver.setDestForHour(  motor::MotorId::Motor3, hour,   minute, mirroring);

    if (!adjustSecond && secondAtNoon && (_mDriver.getPositionByMotorId(motor::MotorId::Motor1) != 0))
    {
        MOC_DEBUG("secondAtNoon: YES -> move to 00\r\n");
        adjustSecond    = true;
        second          = 0;
    }

    if (adjustSecond)
    {
        MOC_DEBUG("adjustSecond: YES\r\n");
        needMove       |= _mDriver.setDestForSecond(motor::MotorId::Motor1, second, mirroring);
    }

    // BatteryLevel
    if (batteryLevel == 100)
    {
        batteryLevel    = 99;
    }
    if (batteryLevel >= 0 && batteryLevel < 100)
    {
        MOC_DEBUG("battery level: %d\r\n", batteryLevel);
        needMove       |= _mDriver.setDestinationForTicks(motor::MotorId::Motor4, (uint8_t) batteryLevel / 10);
        needMove       |= _mDriver.setDestinationForTicks(motor::MotorId::Motor5, (uint8_t) batteryLevel % 10);
    }
    // Display date or week number
    else
    {
        MOC_DEBUG("Day or Week number: %d\r\n", dayOrWeek);
        needMove       |= _mDriver.setDestinationForTicks(motor::MotorId::Motor4, dayOrWeek / 10);
        needMove       |= _mDriver.setDestinationForTicks(motor::MotorId::Motor5, dayOrWeek % 10);
    }

    if (needMove)
    {
        // Forcing direction is only needed for motors of hour and minute (minute only if destination is more than 1 step away from position)
        Motor* motMin  = _mDriver.getMotorById(motor::MotorId::Motor2);
        uint16_t pos   = motMin->position();
        uint16_t dest  = motMin->destination();
        uint8_t maskId = static_cast<uint8_t>(motor::MotorId::Motor3);
        if (pos != dest)
        {
            if ((dest > pos) && (dest - pos > 1))
            {
                maskId |= static_cast<uint8_t>(motor::MotorId::Motor2);
            }
            else if ((pos > dest) && (pos - dest > 1))
            {
                maskId |= static_cast<uint8_t>(motor::MotorId::Motor2);
            }
        }

        _mDriver.forceDirection(forcedDirection, maskId);
        _mDriver.setUpdatePosition(true);
        _autoStartMove(autoStart, calendarEvent);
    }
    MOC_DEBUG("Returning needMove: %s\r\n", needMove ? "YES":"NO");
    return needMove;
}

// ------------------------------------------------------------------------------------------------------------------
void MotorController::setThresholdForMinute(MotorId::eMotorId motorId)
{
    _mDriver.setThresholdForMinute(motorId);
}


// ------------------------------------------------------------------------------------------------------------------
void MotorController::setThresholdForHour(MotorId::eMotorId motorId)
{
    _mDriver.setThresholdForHour(motorId);
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorController::setStepsForFastInit(uint8_t motorId, uint16_t nbSteps)
{
    MotorId::eMotorId motId = _mDriver.checkMotorId(motorId);
    if (motId != MotorId::UnknownMotor)
    {
        if (nbSteps == 0)
        {
            nbSteps         = _mDriver.maxMotorSteps();;
        }
        _mDriver.setStepsForFastInit(motId, nbSteps);
        MOC_DEBUG("motorId: %d (0x%02x), nbSteps: %d\r\n", motorId, motId, nbSteps);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorController::setStepsForInitAnimation(uint8_t motorId)
{
    MotorId::eMotorId motId = _mDriver.checkMotorId(motorId);
    if (motId != MotorId::UnknownMotor)
    {
        uint16_t nbSteps = _mDriver.nbStepsForAnimation(motId);
        _mDriver.setStepsForFastInit(motId, nbSteps);
        MOC_DEBUG("motorId: %d (0x%02x), nbSteps: %d\r\n", motorId, motId, nbSteps);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
void MotorController::initializeAllMotors(bool forward, uint16_t nbSteps)
{
    _mDriver.ctrlMotor(_mDriver.registeredMotorsMask(), forward, nbSteps, false);
}

// ------------------------------------------------------------------------------------------------------------------
void MotorController::acceleratedAgingSynchrone(bool forward,  uint16_t nbSteps, bool updatePosition)
{
    if (_currentState == ST_IDLE)
    {
        _currentState = ST_ACCEL_SYNC;
        MOC_DEBUG("start moving all motors in same direction...\r\n");
        _mDriver.ctrlMotor(_mDriver.registeredMotorsMask(), forward, nbSteps, updatePosition);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorController::acceleratedAgingAsynchrone(bool forward, uint16_t nbSteps, bool updatePosition)
{
    if (_currentState == ST_IDLE)
    {
        _currentState = ST_ACCEL_ASYNC;
        MOC_DEBUG("start moving all motors in opposite direction...\r\n");
        _mDriver.ctrlMotor(_mDriver.registeredMotorsMask(), forward, nbSteps, updatePosition, false, true);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorController::onStepsEnd(MotorDriver* mDriver)
{
    MCD_DEBUG("pushCriticalEvent(_motorEvent)\r\n");
    
    // Push event in the critical queue to react as quick as possible
    this->pushCriticalEvent(&_motorEndEvent);
}


/* **************************************************************************************************************** */
/* PROTECTED SECTION                                                                                                */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
EventStatus MotorController::processEvent()
{
    switch(getCurrentEvent()->getEventType())
    {
        /* -------------------------------------------------------------------------------------------------------- */
        /* Event event                                                                                              */
        /* -------------------------------------------------------------------------------------------------------- */
        case IXFEvent::Event:
            switch(_currentState)
            {
                case ST_MOTOR_INIT_CW:
                {
                    if (getCurrentEvent()->getId() == EventId::MotorEndEvent)
                    {
                        _currentState = ST_MOTOR_INIT_CCW;
                        _mDriver.ctrlMotor(_mDriver.registeredMotorsMask(), false, NB_STEPS_FOR_MOTOR_INIT, false);
                        MOCS_DEBUG("ST_MOTOR_INIT_CW -> ST_MOTOR_INIT_CCW\r\n");
                    }
                    else
                    {
                        MCD_DEBUG("ST_MOTOR_INIT_CW: Event not handled (eventType: 0x%02x, eventId: % 2d)\r\n", (uint8_t) getCurrentEvent()->getEventType(), (uint8_t) getCurrentEvent()->getId());
                    }
                    break;
                }
                case ST_MOTOR_INIT_CCW:
                {
                    if (getCurrentEvent()->getId() == EventId::MotorEndEvent)
                    {
                        _currentState = ST_IDLE;
                        MOCS_DEBUG("%s -> ST_IDLE\r\n", ((_currentState == ST_MOTOR_INIT_CW) ? "ST_MOTOR_INIT_CW" : "ST_MOVE_FAST"));
                        _mDriver.motorMoveEnd();
                        _notifyMotorMoveEnd();
                    }
                    else
                    {
                        MCD_DEBUG("ST_MOTOR_INIT_CCW: Event not handled (eventType: 0x%02x, eventId: % 2d)\r\n", (uint8_t) getCurrentEvent()->getEventType(), (uint8_t) getCurrentEvent()->getId());
                    }
                    break;
                }
                case ST_IDLE:
                {
                    switch(getCurrentEvent()->getId())
                    {
                        case EventId::MotorEvent:
                            _currentState = ST_CHECK;
                            MOCS_DEBUG("ST_IDLE -> ST_CHECK\r\n");
                            _checkStateAction(false, ((MotorEvent*) getCurrentEvent())->fromCalendarEvent());
                            break;

                        case EventId::MotorInitEvent:
                            _currentState = ST_CHECK_INIT;
                            MOCS_DEBUG("ST_IDLE -> ST_CHECK_INIT\r\n");
                            _checkStateAction(true, false);
                            break;

                        default:
                            MCD_DEBUG("ST_IDLE: Event not handled (eventId: % 2d)\r\n", (uint8_t) getCurrentEvent()->getId());
                            break;
                    }
                    break;
                }
                case ST_MOVE:
                {
                    if ((getCurrentEvent()->getId() == EventId::MotorEvent) ||
                       (getCurrentEvent()->getId() == EventId::MotorEndEvent))
                    {
                        _currentState = ST_CHECK;
                        MOCS_DEBUG("ST_MOVE -> ST_CHECK\r\n");
                        _checkStateAction(false, ((MotorEvent*) getCurrentEvent())->fromCalendarEvent());
                    }
                    else
                    {
                        MCD_DEBUG("ST_MOVE: Event not handled (eventType: 0x%02x, eventId: % 2d)\r\n", (uint8_t) getCurrentEvent()->getEventType(), (uint8_t) getCurrentEvent()->getId());
                    }
                    break;
                }
                case ST_MOVE_INIT:
                {
                    if ((getCurrentEvent()->getId() == EventId::MotorEvent) ||
                       (getCurrentEvent()->getId() == EventId::MotorEndEvent))
                    {
                        _currentState = ST_CHECK_INIT;
                        MOCS_DEBUG("ST_MOVE_INIT -> ST_CHECK_INIT\r\n");
                        _checkStateAction(true, false);
                    }
                    else
                    {
                        MCD_DEBUG("ST_MOVE_INIT: Event not handled (eventType: 0x%02x, eventId: % 2d)\r\n", (uint8_t) getCurrentEvent()->getEventType(), (uint8_t) getCurrentEvent()->getId());
                    }
                    break;
                }
                case ST_ACCEL_SYNC:
                case ST_ACCEL_ASYNC:
                {
                    if (getCurrentEvent()->getId() == EventId::MotorEndEvent)
                    {
                        MOCS_DEBUG("ST_ACCEL_SYNC or ST_ACCEL_ASYNC received MotorEndEvent\r\n");
                        _currentState = ST_IDLE;
                        _checkStateAction(true, false);
                    }
                    break;
                }
                default:
                {
                    MCD_DEBUG("Unhandled state (stateID: %3d)...\r\n", (uint8_t) _currentState);
                    break;
                }
            }
            break;

        /* -------------------------------------------------------------------------------------------------------- */
        /* Initial event                                                                                            */
        /* -------------------------------------------------------------------------------------------------------- */
        case IXFEvent::Initial:
            _mDriver.updateMotorSpecification();
            if (_currentState == ST_INIT)
            {
                _currentState = ST_MOTOR_INIT_CW;
                MOCS_DEBUG("ST_INIT -> ST_MOTOR_INIT_CW ((IXFEvent::Initial)\r\n");
                _mDriver.ctrlMotor(_mDriver.registeredMotorsMask(), true, NB_STEPS_FOR_MOTOR_INIT, false);
            }
            else if (_currentState == ST_INIT_HOT)
            {
                _currentState = ST_IDLE;
                MOCS_DEBUG("ST_INIT_HOT -> ST_IDLE((IXFEvent::Initial)\r\n");
                _mDriver.motorMoveEnd();
                _notifyMotorMoveEnd();
            }
            break;

        default:
            break;
    }

    return EventStatus::Consumed;
}


/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void MotorController::_checkStateAction(bool initMotor, bool calendarEvent)
{
    _motorIds = _mDriver.checkMotors(initMotor);
    MCD_DEBUG("_motorIds: 0x%02x\r\n", _motorIds);
    if (_motorIds != 0)
    {
        if (_currentState == ST_CHECK)
        {
            _currentState = ST_MOVE;
            MOCS_DEBUG("ST_CHECK -> ST_MOVE\r\n");
        }
        else if (_currentState == ST_CHECK_INIT)
        {
            _currentState = ST_MOVE_INIT;
            MOCS_DEBUG("ST_CHECK_INIT -> ST_MOVE_INIT\r\n");
        }
        _mDriver.moveMotors(_motorIds, calendarEvent);
    }
    else
    {
#if (DEBUG_MOTOR_CONTROLLER_DEBUG_ENABLE != 0)
        if (_currentState == ST_CHECK)
        {
            MOCS_DEBUG("ST_CHECK -> ST_IDLE\r\n");
        }
        else if (_currentState == ST_CHECK_INIT)
        {
            MOCS_DEBUG("ST_CHECK_INIT -> ST_IDLE\r\n");
        }
#endif
        _currentState = ST_IDLE;
        _mDriver.motorMoveEnd();
        if (!calendarEvent)
        {
            MOCS_DEBUG("_notifyMotorMoveEnd()\r\n");
            _notifyMotorMoveEnd();
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorController::_autoStartMove(bool autostart, bool fromCalendar)
{
    if (autostart)
    {
        raiseMotorCmdEvent(fromCalendar);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorController::_notifyMotorMoveEnd()
{
    for(uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onMotorMoveEnd(this);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
const char* MotorController::_state_to_string(eMainState state)
{
    if (state == ST_UNKOWN) 
    {
        state = _currentState;
    }

    switch (state)
    {
        case ST_UNKOWN:         return "ST_UNKOWN";
        case ST_INIT:           return "ST_INIT";
        case ST_INIT_HOT:       return "ST_INIT_HOT";
        case ST_MOTOR_INIT_CW:  return "ST_MOTOR_INIT_CW";
        case ST_MOTOR_INIT_CCW: return "ST_MOTOR_INIT_CCW";
        case ST_IDLE:           return "ST_IDLE";
        case ST_CHECK:          return "ST_CHECK";
        case ST_MOVE:           return "ST_MOVE";
        case ST_CHECK_INIT:     return "ST_CHECK_INIT";
        case ST_MOVE_INIT:      return "ST_MOVE_INIT";
        case ST_ACCEL_SYNC:     return "ST_ACCEL_SYNC";
        case ST_ACCEL_ASYNC :   return "ST_ACCEL_ASYNC";    
        default:                return "State NOT defined !";
    }
}
#endif  // #if (USE_MOTORS != 0)
