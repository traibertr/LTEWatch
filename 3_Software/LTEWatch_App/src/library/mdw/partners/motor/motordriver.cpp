/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * ******************************************************************************************************************/

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "motordriver.h"
#include "debug-config.h"
/* **************************************************************************************************************** */
/* LOG CONFIG                                                                                                       */
/* **************************************************************************************************************** */
#define DEBUG_MOTOR_DRIVER_ENABLE 0
#if (DEBUG_MOTOR_DRIVER_ENABLE == 1) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(motor_driver, LOG_LEVEL_DBG);
    #define MDRVR_ERR(args...)                    LOG_ERR(args)
    #define MDRVR_WRN(args...)                    LOG_WRN(args)
    #define MDRVR_INF(args...)                    LOG_INF(args)
    #define MDRVR_DBG(args...)                    LOG_DBG(args)
#else
    #define MDRVR_ERR(...)                        {(void) (0);}
    #define MDRVR_WRN(...)                         {(void) (0);}
    #define MDRVR_INF(...)                         {(void) (0);}
    #define MDRVR_DBG(...)                        {(void) (0);}
#endif

#if (USE_MOTORS != 0)
#include <string>

#include "eventid.h"
#include "motordriverhal.h"
#include "motorid.h"
#include "sdktools.h"
#include "factory.h"

/* **************************************************************************************************************** */
/* Namespace's Declaration                                                                                          */
/* **************************************************************************************************************** */
using motor::MotorDriver;

/* **************************************************************************************************************** */
/* Declaration section                                                                                              */
/* **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* CONSTRUCTOR                                                                                                      */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
MotorDriver::MotorDriver() : 
        _currentState(ST_INIT),
        _synchronousMode(true),
        _motorPositionsSize(0),
        _maxMotorSteps(Motor::SixtySteps), 
        _minMotorSteps(Motor::ThreeSixtySteps)
{
    memset(_motorPositions, 0, sizeof(_motorPositions));
}

// ------------------------------------------------------------------------------------------------------------------
MotorDriver::~MotorDriver() {}


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
motor::MotorId::eMotorId MotorDriver::checkMotorId(uint8_t motorId)
{
    switch (motorId)
    {
        case MotorId::Motor1:
            return MotorId::Motor1;
        case MotorId::Motor2:
            return MotorId::Motor2;
        case MotorId::Motor3:
            return MotorId::Motor3;
        case MotorId::Motor4:
            return MotorId::Motor4;
        case MotorId::Motor5:
            return MotorId::Motor5;
        case MotorId::Motor6:
            return MotorId::Motor6;
        case MotorId::Motor7:
            return MotorId::Motor7;
        case MotorId::Motor8:
            return MotorId::Motor8;
        default:
            return MotorId::UnknownMotor;
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::initialize(uint32_t pulseLen, uint32_t singleGap, uint32_t minGap)
{
    _pulseLen   = pulseLen;
    _singleGap  = singleGap;
    _minGap     = minGap;
    _ppdc       = 1;
    MDRVR_DBG("pulseLen: %d, singleGap: %d, minGap: %d", _pulseLen, _singleGap, _minGap);

    _mDriverHAL.initialize();
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorDriver::addMotor(Motor* motor)
{
    if (motor == NULL)
    {
        MDRVR_ERR("addMotor : NULL pointer ERROR !");
        return false;
    }

    if (_nbRegisteredMotors < NB_MAX_MOTORS)
    {
        _motors[_nbRegisteredMotors] = motor;
        _setupCoils(motor->coilA(), motor->coilB(), motor->coilC());
        _nbRegisteredMotors++;
        _motorPositionsSize = 2 * _nbRegisteredMotors;
        #if (MOTORS_CONN_SERIES != 0)
            _motorCoilsSize = 2 * _nbRegisteredMotors + 1;
        #else
            _motorCoilsSize = 3 * _nbRegisteredMotors;
        #endif
        _mDriverHAL.setGpioMask(_motorCoils, _motorCoilsSize);
        _mDriverHAL.setGpioOutputs(_motorCoils, _motorCoilsSize);
        _mDriverHAL.clearMotorPulse();
        MDRVR_DBG("%s, motorId: 0x%02x, _nbRegisteredMotors: %d", motor->toString(), motor->id(), _nbRegisteredMotors);
        _registerdMotorsMask |= motor->id();

        uint8_t tmpSteps = motor->nbSteps();
        if (tmpSteps > _maxMotorSteps)
        {
            _maxMotorSteps = tmpSteps;
        }
        if (tmpSteps < _minMotorSteps)
        {
            _minMotorSteps = tmpSteps;
        }
        MDRVR_DBG("%s, _maxMotorSteps: %3d, _minMotorSteps: %d",  motor->toString(), _maxMotorSteps, _minMotorSteps);
        return true;
    }
    else
    {
        MDRVR_ERR("%s ERROR: No more space !",  motor->toString());
        return false;
    }
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t MotorDriver::checkMotors(bool initializingMotor)
{
    uint8_t tmpMotorIds = 0;
    if (initializingMotor)
    {
        if (_moveMotorNbSteps != 0)
        {
            tmpMotorIds = _moveMotorIds;
        }
    }
    else
    {
        for (uint8_t i = 0; i < _nbRegisteredMotors; i++)
        {
            tmpMotorIds += _checkMotor(i, _motors[i]->forceDirection());
        }
    }
    MDRVR_DBG("%stmpMotorIds = 0x%02x", (initializingMotor ? "INIT Mode - " : ""), tmpMotorIds);
    return tmpMotorIds;
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::clearPulse()
{
    _mDriverHAL.clearMotorPulse();
    MDRVR_DBG("(MDD) _");
    if (_currentState == ST_NO_MORE_PULSE)
    {
        _currentState = ST_IDLE;
        MDRVR_DBG("ST_NO_MORE_PULSE -> ST_IDLE");
        _notifyStepsEnd();
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::ctrlMotor(uint8_t motorIds, bool forward, uint16_t nbSteps, bool updatePosition, bool fastMove, bool asynchroneDirection)
{
    _setDirectionOfMotors(motorIds, forward, asynchroneDirection);
    _updatePosition                 = updatePosition;
    _activeMotorIdx                 = 0;
    _circularMotorHandlerCounter    = 0;
    _moveMotorNbSteps               = _validateNbStepsToMove(nbSteps);
    _fastMoveEnabled                = fastMove;

    MDRVR_DBG("motorId = 0x%02x, nbSteps = %d, forward: %s, updatePosition:  %s", (uint8_t) motorIds, _moveMotorNbSteps, (forward ? "Yes" : "No"), (_updatePosition ? "Yes" : "No"));
    moveMotors(motorIds, false);
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::initMotor(MotorId::eMotorId motorId, bool forward, bool fastForward, bool updatePosition)
{
    if (fastForward)
    {
        ctrlMotor(motorId, forward, _getMotorById(motorId)->nbStepsForFastInit(), updatePosition, false);
    }
    else
    {
        ctrlMotor(motorId, forward, 1, updatePosition, false);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::cancelAllMotorsMove()
{
    if (_currentState != ST_IDLE && _currentState != ST_NO_MORE_PULSE)
    {
        _cancelAnyMove = true;
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::cancelMotorInitMove()
{
    _resetMotorMoveParam();
    MDRVR_DBG("_moveMotorNbSteps = 0, _fastMoveEnabled = false;");
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::moveMotors(uint8_t motorIds, bool fromCalendarEvent)
{
    _cancelAnyMove      = false;
    _moveMotorIds       = motorIds;
    MDRVR_DBG("motorIds = 0x%02x, fromCalendar: %d", motorIds, fromCalendarEvent);

    if ((motorIds > 0) && (_currentState == ST_IDLE))
    {
        MDRVR_DBG("ST_IDLE -> ST_SWITCH_MOTOR");
        _currentState   = ST_SWITCH_MOTOR;
        _switchMotor(fromCalendarEvent);
        #if (SMOOTH_MOTOR_MOVE != 0)
            if (!updateMotorPosition())
            {
                MDRVR_DBG("ST_IDLE: Something goes wrong while updating the motor's positions (SMOOTH_MOTOR_MOVE option enabled) !");
            }
        #endif // #if (SMOOTH_MOTOR_MOVE != 0)
    }
    else
    {
        MDRVR_DBG("NO MOVE: ids = 0x%02x, state: %2d", motorIds, _currentState);
    }
}

// ------------------------------------------------------------------------------------------------------------------
motor::MotorId::eMotorId MotorDriver::firstMotorId() const
{
    if (_motors[0] != NULL)
    {
        return static_cast<MotorId::eMotorId>(_motors[0]->id());
    }
    return MotorId::UnknownMotor;
}

// ------------------------------------------------------------------------------------------------------------------
motor::MotorId::eMotorId MotorDriver::lastMotorId() const
{
    if (_motors[_nbRegisteredMotors - 1] != NULL)
    {
        return static_cast<MotorId::eMotorId>(_motors[_nbRegisteredMotors - 1]->id());
    }
    return MotorId::UnknownMotor;
}

// ------------------------------------------------------------------------------------------------------------------
motor::MotorId::eMotorId MotorDriver::nextMotorId(uint8_t motorId) const
{
    for (uint8_t i = 0; i < _nbRegisteredMotors; i++)
    {
        if (_motors[i]->id() == motorId)
        {
            if (i + 1 == _nbRegisteredMotors)
            {
                MDRVR_DBG("id = 0x%02x", _motors[0]->id());
                return static_cast<MotorId::eMotorId>(_motors[0]->id());
            }
            else
            {
                MDRVR_DBG("id = 0x%02x", _motors[i + 1]->id());
                return static_cast<MotorId::eMotorId>(_motors[i + 1]->id());
            }
        }
    }
    MDRVR_ERR("UnknownMotor");
    return MotorId::UnknownMotor;
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::motorTimerEvent()
{
    if (_currentState == ST_PULSE)
    {
        _currentState = ST_SWITCH_MOTOR;
        MDRVR_DBG("ST_PULSE -> ST_SWITCH_MOTOR");
        _switchMotor(_fromCalendarEvt);
    }
    else
    {
        MDRVR_DBG("State not handled: %d", _currentState);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::motorMoveEnd()
{
    if (_currentState == ST_IDLE)
    {
        MDRVR_DBG("Reseting all parameters.");
        _mDriverHAL.stopTimer();
        _activeMotorIdx              = 0;
        _circularMotorHandlerCounter = 0;
        if (!updateMotorPosition())
        {
            MDRVR_DBG("ST_IDLE: Something goes wrong while updating the motor's positions !");
        }
    }
    else
    {
        MDRVR_DBG("State not handled (%d)", _currentState);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::setMotorDirectionConfiguration(motor::MotorId::eMotorId motorId, bool clockwise)
{
    Motor* motor = _getMotorById(motorId);
    if (motor != NULL)
    {
        motor->setDirectionConfiguration(clockwise);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::setAllMotorsDirectionConfiguration(bool clockwise)
{
    for (uint8_t i = 0; i < _nbRegisteredMotors; i++)
    {
        if (_motors[i] != NULL)
        {
            _motors[i]->setDirectionConfiguration(clockwise);
        }
        else
        {
            MDRVR_DBG("_motors[%d] retrieves a NULL motor!",i);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::setDirectionOfMotor(MotorId::eMotorId motorId, bool forward)
{
    Motor* motor = _getMotorById(motorId);
    if (motor != NULL)
    {
        motor->setDirection(forward);
    }
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorDriver::setDestination(MotorId::eMotorId motorId, uint16_t destination)
{
    Motor* motor = _getMotorById(motorId);
    if (motor != NULL)
    {
        return _setDestination(motor, destination);
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorDriver::setDestinationForTicks(MotorId::eMotorId motorId, uint8_t tick)
{
    Motor* motor = _getMotorById(motorId);
    if (motor != NULL)
    {
        return _setDestination(motor, tick * motor->nbStepsForTick());
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorDriver::setDestForSecond(MotorId::eMotorId motorId, uint8_t second, uint8_t mirroring)
{
    Motor* motor = _getMotorById(motorId);
    if (motor != NULL)
    {
        uint8_t steps        = motor->nbSteps() / 60;
        uint8_t destination  = _handleMirroring(mirroring, second * steps, motor);

        return _setDestination(motor, destination);
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
uint16_t MotorDriver::getDestForSecond(MotorId::eMotorId motorId, uint8_t second)
{
    Motor* motor = _getMotorById(motorId);
    if (motor != NULL)
    {
        uint8_t steps        = motor->nbSteps() / 60;
        uint8_t destination  = _handleMirroring(false, second * steps, motor);
        return destination;
    }
    return 0xFFFF;
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorDriver::setDestForMinute(MotorId::eMotorId motorId, uint8_t minute, uint8_t second, uint8_t mirroring)
{
    Motor* motor = _getMotorById(motorId);
    if (motor != NULL)
    {
        int8_t  threshold    = second - _minuteThresholdHalf;
        uint8_t steps        = motor->nbSteps() / 60;
        uint8_t destination  = minute * steps;

        if (threshold > 0)
        {
            steps            = (uint8_t)(threshold / _minuteThreshold);
            destination     += steps * motor->nbStepsForTick();
        }
        destination = _handleMirroring(mirroring, destination, motor);
        return _setDestination(motor, destination);
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
uint16_t MotorDriver::getDestForMinute(MotorId::eMotorId motorId, uint8_t minute, uint8_t second)
{
    Motor* motor = _getMotorById(motorId);
    if (motor != NULL)
    {
        int8_t  threshold    = second - _minuteThresholdHalf;
        uint8_t steps        = motor->nbSteps() / 60;
        uint8_t destination  = minute * steps;

        if (threshold > 0)
        {
            steps            = (uint8_t)(threshold / _minuteThreshold);
            destination     += steps * motor->nbStepsForTick();
        }
        return destination;
    }
    return 0xFFFF;
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::setThresholdForMinute(MotorId::eMotorId motorId)
{
    _minuteThreshold        = (uint8_t) tools::SDKTools::roundedDiv(3600, (uint16_t) nbStepsByMotorId(motorId));
#if (TICK_AT_NOON != 0)
    _minuteThresholdHalf    = 0;
#else
    _minuteThresholdHalf = (uint8_t) tools::SDKTools::roundedDiv(_minuteThreshold, 2);
#endif
    MDRVR_DBG("_minuteThreshold = %d, _minuteThresholdHalf = %d", _minuteThreshold, _minuteThresholdHalf);
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorDriver::setDestForHour(MotorId::eMotorId motorId, uint16_t hour, uint8_t minute, uint8_t mirroring)
{
    Motor* motor = _getMotorById(motorId);
    if (motor != NULL)
    {
        int8_t  threshold    = minute - _hourThresholdHalf;
        uint8_t steps        = motor->nbSteps() / 12;
        uint8_t destination  = (hour % 12) * steps;

        if (threshold > 0)
        {
            steps            = (uint8_t)(threshold / _hourThreshold);
            destination     += steps * motor->nbStepsForTick();
        }
        destination = _handleMirroring(mirroring, destination, motor);
        return _setDestination(motor, destination);
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
uint16_t MotorDriver::getDestForHour(MotorId::eMotorId motorId, uint16_t hour, uint8_t minute)
{
    Motor* motor = _getMotorById(motorId);
    if (motor != NULL)
    {
        int8_t  threshold    = minute - _hourThresholdHalf;
        uint8_t steps        = motor->nbSteps() / 12;
        uint8_t destination  = (hour % 12) * steps;

        if (threshold > 0)
        {
            steps            = (uint8_t)(threshold / _hourThreshold);
            destination     += steps * motor->nbStepsForTick();
        }
        return destination;
    }
    return 0xFFFF;
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::setThresholdForHour(MotorId::eMotorId motorId)
{
    _hourThreshold     = (uint8_t) tools::SDKTools::roundedDiv(720, (uint16_t) nbStepsByMotorId(motorId));
#if (TICK_AT_NOON != 0)
    _hourThresholdHalf = 0;
#else
    _hourThresholdHalf = (uint8_t) tools::SDKTools::roundedDiv(_hourThreshold, 2);
#endif
    MDRVR_DBG("_hourThreshold = %d, _hourThresholdHalf = %d", _hourThreshold, _hourThresholdHalf);
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::setUpdatePosition(bool updatePosition)
{
    _updatePosition = updatePosition;
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::forceDirection(int8_t direction, uint8_t motorIdMask)
{
    for (uint8_t i = 0; i < _nbRegisteredMotors; i++)
    {
        if (_motors[i]->id() & motorIdMask)
        {
            MDRVR_DBG("forceDirection : Direction forced to %s for %s", direction == 1 ? "clockwise":"counter clockwise", _motors[i]->toString());
            _motors[i]->setForceDirection(direction);
        }
        else
        {
            _motors[i]->setForceDirection(0);
            MDRVR_DBG("forceDirection : Motor's ID does NOT match to the given mask ! (id: 0x%02x, mask: 0x%02x)", _motors[i]->id(), motorIdMask);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::setStepsForFastInit(MotorId::eMotorId motorId, uint16_t stepsForFastInit)
{
    Motor* motor = _getMotorById(motorId);
    if (motor != NULL)
    {
        motor->setMotorStepsForFastInit(stepsForFastInit);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::startBehaviour()
{
    MDRVR_DBG("ST_INIT -> ST_IDLE");
    _ST_INIT_Action();
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorDriver::updateMotorPosition()
{
    return true;
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorDriver::updateMotorSpecification()
{
    memset(_motorSpecifications, 0, sizeof(_motorSpecifications));

    // set the number of motor which are specified 
    _motorSpecifications[0] = _nbRegisteredMotors;

    // loop through all registered motors
    for (uint8_t i = 0; i < _nbRegisteredMotors; i++)
    {
        // Set up index of motors to match motorId and position in motors
        uint8_t index = 1 << i;
        Motor* motor = _getMotorById(index);

        if (motor != NULL)
        {
            MDRVR_DBG("index: 0x%02x, motorId: 0x%02x, nbSteps: %3d (i: %d)", index, _motors[i]->id(), _motors[i]->nbSteps(), i);
            tools::SDKTools::uint16Encode(motor->nbSteps(), &_motorSpecifications[(2 * i) + 1]);
        }
        else
        {
            MDRVR_ERR("NULL Motor for index 0x%04x", index);
            return false;
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::handleSynchronousMode(bool enable)
{
    if (_synchronousMode != enable)
    {
        _synchronousMode = enable;
        _updateNbStepsForAnimation();
    }
}

/* **************************************************************************************************************** */
/* PROTECTED SECTION                                                                                                */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------

/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */

// ------------------------------------------------------------------------------------------------------------------
uint8_t MotorDriver::_checkMotor(uint8_t motorId, int8_t forceDirection)
{
    if (motorId < _nbRegisteredMotors)
    {
        Motor* motor       = _motors[motorId];                                                                          // Get the pointer to the motor
        uint8_t destination = motor->destination();                                                                     // Retrieve its destination
        uint8_t position    = motor->position();                                                                        // and position

        if (position == destination)                                                                                     // checks that a motor's step is needed
        {
            return 0;                                                                                                   // otherwise exit
        }

        if (forceDirection == 0)
        {
            uint8_t threshold = motor->nbStepsHalf();                                                                   // Compute direction:
            if (destination > position)                                                                                  // - destination > position
            {                                                                                                           //   +-> destination - position > (MAX STEPS)/2 => backwards
                motor->setDirection((destination - position) < threshold);                                              //   +-> destination - position < (MAX STEPS)/2 => forwards
            }
            else                                                                                                        // - destination < position
            {                                                                                                           //   +-> position - destination > (MAX STEPS)/2 => forwards
                motor->setDirection((position - destination) > threshold);                                              //   +-> position - destination < (MAX STEPS)/2 => backwards
            }
        }
        else
        {
            motor->setDirection(forceDirection > 0);                                                                    // Force the direction to move...
        }
        MDRVR_DBG("%d (id: 0x%02x, pos: %3d, dest: %3d) p!=d", motorId, motor->id(), position, destination);
        return motor->id();
    }
    MDRVR_ERR("outOfRange ! %d",  motorId);
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
motor::Motor* MotorDriver::_getMotorById(uint8_t motorId) const
{
    // Looks for the motor's pointer with `motorId` as Id through all registered motors.
    for (uint8_t i = 0; i < _nbRegisteredMotors; i++)                                                                   
    {
        if (_motors[i]->id() == motorId)
        {
            return _motors[i];
        }
    }
    
    // Return NULL if the motor could not be found
    MDRVR_ERR("No motor found with id 0x%02x. Return a NULL pointer!", (uint8_t) motorId);
    return NULL;
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t MotorDriver::_handleMirroring(uint8_t mirroring, uint16_t position, Motor* motor)
{
    uint16_t newPosition = position;

    if (motor == NULL)
    {
        MDRVR_DBG("NULL Pointer on motor !");
        return position;
    }

    switch (mirroring)
    {
        case 0:
            break;

        case 1:
            break;

        case 2:
            if (position > motor->nbStepsHalf())
            {
                newPosition = motor->nbSteps() - (position - motor->nbStepsHalf());
            }
            else
            {
                newPosition = motor->nbStepsHalf() - position;
            }

        case 3:
            break;

        case 4:
            break;
    }
    return newPosition;
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::_initializeMotorParameters()
{
    // Setup the pulse and gap durations
    _setTimerDuration();                

    // compute the greatest common divisor of max number of steps for all motors
    uint16_t gcd = tools::SDKTools::GCD(_minMotorSteps, _maxMotorSteps);
    _ppdc = _maxMotorSteps / gcd;
    
    // Compute the stepThreshold...
    switch (_maxMotorSteps - _minMotorSteps)
    {
        case 0:
        {
            for (uint8_t i = 0; i < _nbRegisteredMotors; i++)
            {
                _motors[i]->setStepThreshold(_ppdc);
                MDRVR_DBG("[MotorDriver] _initializeMotorParameters() >motor#%d: id 0x%02x, stepThreshold: %3d", i, _motors[i]->id(), _motors[i]->stepThreshold());
            }
            break;
        }
        case 60:
        {
            for (uint8_t i = 0; i < _nbRegisteredMotors; i++)
            {
                if (_motors[i]->nbSteps() == _maxMotorSteps)
                {
                    _motors[i]->setStepThreshold(_ppdc);
                }
                else
                {
                    _motors[i]->setStepThreshold(_ppdc - 1);
                }
                MDRVR_DBG("motor#%d: id 0x%02x, stepThreshold: %3d", i, _motors[i]->id(), _motors[i]->stepThreshold());
            }
            break;
        }
        case 120:
        {
            for (uint8_t i = 0; i < _nbRegisteredMotors; i++)
            {
                if (_motors[i]->nbSteps() == _maxMotorSteps)
                {
                    _motors[i]->setStepThreshold(_ppdc);
                }
                else if (_motors[i]->nbSteps() == _minMotorSteps)
                {
                    _motors[i]->setStepThreshold(_ppdc - 2);
                }
                else
                {
                    _motors[i]->setStepThreshold(_ppdc - 1);
                }
                MDRVR_DBG("motor#%d: id 0x%02x, stepThreshold: %3d", i, _motors[i]->id(), _motors[i]->stepThreshold());
            }
            break;
        }
        default:
        {
            MDRVR_DBG("Value for `_maxMotorSteps - _minMotorSteps` not handled: %d)", _maxMotorSteps - _minMotorSteps);
            break;
        }
    }

    // Set up the singleGapTick value for all motors
    for (uint8_t i = 0; i < _nbRegisteredMotors; i++)
    {
        uint32_t sgt = tools::SDKTools::roundedDiv(_singleGapTick + PULSE_GAP_CORRECTION, _ppdc + 1 - _motors[i]->stepThreshold()) - PULSE_GAP_CORRECTION;
        MDRVR_DBG("motor#%d: id 0x%02x, singleGapTick: %3d", i, _motors[i]->id(), sgt);
        _motors[i]->setSingleGapTick(sgt);

        // Set up index of motors to match motorId and position in motors
        for (uint8_t j = 0; j < _nbRegisteredMotors; j++)
        {
            uint8_t index = 1 << j;
            if (index == _motors[i]->id())
            {
                _motors[i]->setMotorPositionIndex(j);
                break;
            }
        }
        
        _motors[i]->updateNbStepsForAnimation(_synchronousMode, _ppdc);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::_notifyStepsEnd()
{
    MDRVR_DBG("(MDD) END !!!");
    for (uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onStepsEnd(this);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::_resetAllMotorsDestination()
{
    for (uint8_t i = 0; i < _nbRegisteredMotors; i++)
    {
        _motors[i]->resetDestination();
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::_resetMotorMoveParam()
{
    // By clearing the number of steps to do, it stops moving the active motor
    _moveMotorNbSteps = 0;
    
    _moveMotorIds     = 0;
    _fastMoveEnabled  = false;
}

// ------------------------------------------------------------------------------------------------------------------
bool MotorDriver::_setDestination(Motor* motor, uint16_t destination)
{
    if (motor->position() != destination)
    {
        motor->setDestination(destination);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::_setDirectionOfMotors(uint8_t motorIds, bool forward, bool asynchroneDirection)
{
    for (uint8_t i = 0; i < _nbRegisteredMotors; i++)
    {
        if (motorIds & _motors[i]->id())
        {
            if (asynchroneDirection)
            {
                bool tmpForward = ((i & 0x01) == 0) ? forward : !forward;
                _motors[i]->setDirection(tmpForward);
            }
            else
            {
                _motors[i]->setDirection(forward);
            }
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::_setTimerDuration()
{
    _mDriverHAL.setPulseTick(_pulseLen);
    _singleGapTick = k_us_to_ticks_near32(_singleGap)   - PULSE_GAP_CORRECTION; //NRFX_RTC_US_TO_TICKS
    _minGapTick    = k_us_to_ticks_near32(_minGap)      - PULSE_GAP_CORRECTION; //NRFX_RTC_US_TO_TICKS
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::_setupCoils(uint8_t coilA, uint8_t coilB, uint8_t coilC)
{
    _motorCoils[_motorCoilsFactor * _nbRegisteredMotors]            = coilA;
    #if (COMMON_COIL_C != 0)
    {
        _motorCoils[_motorCoilsFactor * _nbRegisteredMotors + 1]    = coilC;
        _motorCoils[_motorCoilsFactor * _nbRegisteredMotors + 2]    = coilB;
    }
    #else
    {
        _motorCoils[_motorCoilsFactor * _nbRegisteredMotors + 1]    = coilB;
        _motorCoils[_motorCoilsFactor * _nbRegisteredMotors + 2]    = coilC;
    }
    #endif
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::_setupPulseMask()
{
    const uint8_t* pulse    = _activeMotor->activePulse();
    uint8_t* coils          = _activeMotor->activeCoilEndPoints();
    uint64_t pulseMask      = 0;
    #if (MOTORS_CONN_SERIES != 0)
    {
        uint8_t  idx            = 0;
        uint8_t  pulseSet       = pulse[idx];

        for (uint8_t i = 0; i < _motorCoilsSize; i++)
        {
            if (pulseSet != 0)
            {
                pulseMask |= (1ULL << _motorCoils[i]);
            }

            if (_motorCoils[i] == coils[idx])
            {
                idx++;
                if (idx < NB_COILS_PER_MOTOR)
                {
                    pulseSet = pulse[idx];
                }
            }
        }
    }
    #else
    {
        for (uint8_t i = 0; i < NB_COILS_PER_MOTOR; i++)
        {
            if (pulse[i] != 0)
            {
                pulseMask |= (1ULL << coils[i]);
            }
        }
    }
    #endif // #if (MOTORS_CONN_SERIES != 0)
    _mDriverHAL.setMotorPulse(pulseMask);
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::_singleStep()
{
    _setupPulseMask();
    _activeMotor->toggleAlim();
    if (_updatePosition)
    {
        _activeMotor->updatePosition();
        tools::SDKTools::uint16Encode(_activeMotor->position(), &_motorPositions[_activeMotor->motorPositionIndex()]);
        if (_fastMoveEnabled)
        {
            _activeMotor->setDestination(_activeMotor->position());
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::_ST_INIT_Action()
{
    _initializeMotorParameters();
    _circularMotorHandlerCounter    = 0;
    _activeMotorIdx                 = 0;
    _activeMotor                    = NULL;
    _currentState                   = ST_IDLE;
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::_switchMotor(bool fromCalendarEvent)                                                                  // If we have one motor with 180 steps and one motor with 60 steps, to give the impression 
{                                                                                                                       // that both motors rotate at the same speed, the motor with the less number of steps
    static bool skipMotor = false;                                                                                      // must make less steps. In this case, the motor with 60 steps should only step each three 
                                                                                                                        // steps of the motor of 180 steps.
    while (_currentState == ST_SWITCH_MOTOR)
    {
        if (_cancelAnyMove)                                                                                              // Should we stop any motor's movement ?
        {
            _cancelAnyMove = false;                                                                                     // Reset the flag
            _currentState = ST_IDLE;                                                                                    // Update state
            _resetMotorMoveParam();                                                                                     // Reset the motor move's parameters
            _resetAllMotorsDestination();                                                                               // Reset all motor's destinations and set them to the actual position
            _notifyStepsEnd();
            return;
        }

        if (_activeMotorIdx < _nbRegisteredMotors)
        {
            if (_activeMotorIdx == 0)
            {
                _processGapTick();                                                                                      // Process the gap duration according to the number of motors to move
                if (!_findNextMotorIdx())
                {
                    MDRVR_DBG("(MDD) _switchMotor ERROR");
                    //ASSERT(false);
                }
            }

            if ((!_synchronousMode || 
                (_synchronousMode && _circularMotorHandlerCounter < _motors[_activeMotorIdx]->stepThreshold())) &&
                (_moveMotorIds & _motors[_activeMotorIdx]->id()))                                                       // Checks if a steps MUST be done on the active motor
            {
                _currentState = ST_PULSE;                                                                               // Update State
                _activeMotor  = _motors[_activeMotorIdx];                                                               // Set up the pointer to the active motor
                _singleStep();                                                                                          // Apply the motor's pulse on the GPIO
                _startPulseTimer();                                                                                     // Start the timer with the delay of a motor's pulse
                MDRVR_DBG("(MDD) (P)  motorIds: 0x%04x, motIdx: %d, threshold: %d", _moveMotorIds, _activeMotorIdx, _activeMotor->stepThreshold());
                skipMotor = false;
            }
            else
            {
                skipMotor = true;
            }

            if (_incrActiveMotorIdx(fromCalendarEvent))                                                                  // Update the motor's index
            {
                if (skipMotor)
                {
                    _currentState = ST_PULSE;                                                                           // Update State
                    _startNoPulseTimer();                                                                               // Starts the app_timer for the gap delay between two motor's steps
                    MDRVR_DBG("(MDD) (NP) motorIds: 0x%04x, motIdx: %d, threshold: %d", _moveMotorIds, _activeMotorIdx, _activeMotor->stepThreshold());
                }
            }
            else
            {
                _mDriverHAL.disableGapPulse();
                _currentState = ST_NO_MORE_PULSE;                                                                       // Update state
            }
            _fromCalendarEvt = fromCalendarEvent;                                                                       // Store where it comes from
        }
        else
        {
            _incrCircularCounter();                                                                                     // Increment circular counter
            _activeMotorIdx = 0;                                                                                        // Reset motor's index

            if (_moveMotorNbSteps > 0)                                                                                   // Decrement the number of steps to do
            {
                _moveMotorNbSteps--;
            }

            if (_moveMotorNbSteps == 0)                                                                                  // This is for initialisation or special motor's move
            {
                _currentState = ST_IDLE;                                                                                // Update state
                _notifyStepsEnd();                                                                                      // Notify the Event to the MotorController
            }
            else
            {
                _currentState = ST_SWITCH_MOTOR;                                                                        // Update state
            }
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorDriver::_updateNbStepsForAnimation()
{
    for (uint8_t i = 0; i < _nbRegisteredMotors; i++)
    {
        _motors[i]->updateNbStepsForAnimation(_synchronousMode, _ppdc);
    }
}

// ------------------------------------------------------------------------------------------------------------------
uint16_t MotorDriver::_validateNbStepsToMove(uint16_t nbSteps)
{
    uint16_t steps = nbSteps;
    if (steps > 1)                                                                                                       // if `nbSteps` is 1, nothing to do !
    {
        while ((steps % _ppdc) != 0)                                                                                     // If `nbSteps` is not a multiple of the _ppdc, 
        {                                                                                                               // the value is adjusted and incremented of one step...
            steps++;
        }
    }
    return steps;                                                                                                       // returns the new value of number os steps to move
}


// ------------------------------------------------------------------------------------------------------------------
inline bool MotorDriver::_findNextMotorIdx()
{
    // Loops through all registered motors and checks if the current one should move
    while (!(_moveMotorIds & _motors[_activeMotorIdx]->id()))
    {
        if (++_activeMotorIdx >= _nbRegisteredMotors)
        {
            return false;
        }
    }
    
    // As soon as we've found a motor to move, the method returns `true`
    return true;
}

// ------------------------------------------------------------------------------------------------------------------
inline bool MotorDriver::_incrActiveMotorIdx(bool fromCalendarEvent)
{
    uint8_t activeMotorIdx = _activeMotorIdx;                                                                           // Save the actual value...
    while (++_activeMotorIdx < _nbRegisteredMotors)
    {
        if ((_moveMotorIds & _motors[_activeMotorIdx]->id()))
        {
            return true;                                                                                                // Returns `true` if a motor is found
        }
    }
                                                                                                                        // If we reach this point, no other motor has to move !
    if (_motors[activeMotorIdx]->position() == _motors[activeMotorIdx]->destination())                                  // We just check here that the last motor we moved has
    {                                                                                                                   // reached its destination. If it's the case
        return !fromCalendarEvent;                                                                                      // a GAP timing is not necessary if the motor's move comes from a calendar event
    }
    else                                                                                                                // If the last motor has not reached its destination
    {
        return true;                                                                                                    // a GAP delay is still needed, if _activeMotorIdx is.
    }
}

// ------------------------------------------------------------------------------------------------------------------
inline void  MotorDriver::_incrCircularCounter()
{
    _circularMotorHandlerCounter = (++_circularMotorHandlerCounter == _ppdc ? 0 : _circularMotorHandlerCounter);
}

// ------------------------------------------------------------------------------------------------------------------
inline void MotorDriver::_processGapTick()
{
    uint32_t gapTick = _singleGapTick;
    uint8_t counter = 0;
    #if (SPEED_UP_SINGLE_MOTOR != 0)
        uint8_t index = 0;
    #endif

    for (uint8_t i = 0; i < _nbRegisteredMotors; i++)
    {
        if (_moveMotorIds & _motors[i]->id())
        {
            #if (SPEED_UP_SINGLE_MOTOR != 0)
                index = i;
            #endif
            counter++;
        }
    }

    #if (SPEED_UP_SINGLE_MOTOR != 0)
    if (counter == 1)
    {
        gapTick = _motors[index]->singleGapTick();
        MDRVR_DBG("(MDD) gapTick: %d, gap: %7.4f [ms] (motorIds: 0x%02x, threshold: %d)", 
                    gapTick, static_cast<float>(gapTick * 1000) / 32768, _moveMotorIds, _motors[index]->stepThreshold());
    }
    else if (counter != 0)
    #else
    if (counter > 1)
    #endif
    {
        gapTick = tools::SDKTools::roundedDiv(gapTick + PULSE_GAP_CORRECTION, counter) - PULSE_GAP_CORRECTION;
        MDRVR_DBG("(MDD) gapTick: %d", gapTick);
    }

    if (gapTick < _minGapTick)
    {
        gapTick = _minGapTick;
    }
    _mDriverHAL.setGapTick(gapTick);
}

#endif  // #if (USE_MOTORS != 0)
