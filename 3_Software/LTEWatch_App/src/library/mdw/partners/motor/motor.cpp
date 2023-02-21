/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * ******************************************************************************************************************/

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "motor.h"

#if (USE_MOTORS != 0)
#include <string>
#include "sdktools.h"
#include "debug-config.h"

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#define DEBUG_MOTOR_ENABLE                  0
#define DEBUG_MOTOR_DEEP_ENABLE             0
#if (DEBUG_MOTOR_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(motor, LOG_LEVEL_DBG);
    #define MOTOR_DEBUG(args...)            LOG_DBG(args) 
    #define MOTOR_ERROR(args...)            LOG_ERR(args) 
#else
    #define MOTOR_DEBUG(...)                {(void) (0);}
    #define MOTOR_ERROR(...)                {(void) (0);}
#endif


/* **************************************************************************************************************** */
/* Namespace's Declaration                                                                                          */
/* **************************************************************************************************************** */
using motor::Motor;


/* **************************************************************************************************************** */
/* Declaration section                                                                                              */
/* **************************************************************************************************************** */
const uint8_t Motor::_PULSE[2][2][3] = { { { 0, 1, 1 }, { 1, 0, 0 } }, { { 1, 1, 0 }, { 0, 0, 1 } } };


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void Motor::initialize(bool              hotReset,
                       MotorId::eMotorId id,
                       Motor::eNbSteps   nbSteps,
                       uint8_t           nbStepsForTick,
                       uint8_t           coilA,
                       uint8_t           coilB,
                       uint8_t           coilC,
                       bool              inverseRotation,
                       uint8_t           nbStepsForFastInit)
{
    _id                         = (uint8_t) id;
    _nbStepsForTick             = nbStepsForTick;
    _nbStepsForFastInit         = nbStepsForFastInit;
    _nbStepsForAnimation        = nbStepsForFastInit;

    _clockwise                  = true;
    _stepThreshold              = 3;

    _nbSteps                    = (uint16_t) nbSteps;
    _nbStepsHalf                = _nbSteps / 2;
    _nbStepsThreeHalf           = 3 * _nbStepsHalf;

    _inverseRotation            = inverseRotation;
    _clockwiseConfiguration     = inverseRotation;

    // Setup the GPIO
    _motorGpioMask              = 0;
    _coilA                      = coilA;
    _motorGpioMask             |= (1UL << coilA);
    _coilB                      = coilB;
    _motorGpioMask             |= (1UL << coilB);
    _coilC                      = coilC;
    _motorGpioMask             |= (1UL << coilC);
    MOTOR_DEBUG("_motorGpioMask %#010x\r\n", _motorGpioMask);

    // Setup the motor's pulse to assign to the GPIO to move the motor
    #if (COMMON_COIL_C != 0)
        _activeCoilEndPoint[0]  = coilA;
        _activeCoilEndPoint[1]  = coilC;
        _activeCoilEndPoint[2]  = coilB;
    #else
        _activeCoilEndPoint[0]  = coilA;
        _activeCoilEndPoint[1]  = coilB;
        _activeCoilEndPoint[2]  = coilC;
    #endif    // COMMON_COIL_C
    
    // Handle hot Reset
    if (hotReset)
    {
        MOTOR_DEBUG("Hot Reset\r\n");
        setDestination(_destination);
        setPosition(_persistentParams.position());
    }
    else
    {
        _destination            = 0;
        setPosition(0);
    }

    MOTOR_DEBUG("Initialize motor %2d (%#04x)\r\n", (uint8_t)id, (uint8_t)id);
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::setPosition(uint16_t position)
{
    // To get rid of `modulo`...
    uint16_t pos = tools::SDKTools::modulo16(position, _nbSteps);
    _persistentParams.setPosition(position);
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::setDestination(uint16_t destination)
{
    // To get rid of `modulo`...
    _destination = tools::SDKTools::modulo16(destination, _nbSteps);
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::setDirectionConfiguration(bool clockwise)
{
    if (clockwise)
    {
        _inverseRotation = _clockwiseConfiguration;
    }
    else
    {
        _inverseRotation = !_clockwiseConfiguration;
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::setDirection(bool clockwise)
{
    if (clockwise != _clockwise)
    {
        _clockwise = clockwise;
        MOTOR_DEBUG("rotation %s (ID: 0x%02x)\r\n", (_clockwise ? "CW" : "CCW"), (uint8_t)_id);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::setForceDirection(int8_t forceDirection)
{
    _forceDirection = forceDirection;
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::updatePosition()
{
    uint16_t position = _persistentParams.position();
    if (_clockwise)
    {
        position = (++position == _nbSteps ? 0 : position);
    }
    else
    {
        position = (position == 0 ? _nbSteps - 1 : position - 1);
    }
    _persistentParams.setPosition(position);
    #if (DEBUG_MOTOR_DEEP_ENABLE != 0) && defined(DEBUG_NRF_USER)
    MOTOR_DEBUG("Position: %3d, Destination: %3d (_motorPosIndex: 0x%02x)\r\n", position, _destination, _motorPosIndex);
    #endif
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::setNbStepsForTick(uint8_t nbStepsForTick)
{
    if (nbStepsForTick < _nbSteps)
    {
        _nbStepsForTick = nbStepsForTick;
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::setMotorPositionIndex(uint8_t index)
{
    _motorPosIndex = 2 * index;
    MOTOR_DEBUG("Motor's index: %02d (_motorPosIndex: %02d)\r\n", index, _motorPosIndex);
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::setMotorSteps(uint16_t steps)
{
    _nbSteps = steps;
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::setMotorStepsForFastInit(uint16_t stepsForFastInit)
{
    _nbStepsForFastInit = stepsForFastInit;
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::setStepThreshold(uint8_t stepThreshold)
{
    _stepThreshold = stepThreshold;
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::setSingleGapTick(uint32_t gapTick)
{
    _singleGapTick = gapTick;
}

#if (USE_FLASH != 0)
// ------------------------------------------------------------------------------------------------------------------
uint32_t* Motor::save(uint16_t* size)
{
    *size = sizeof(MotorPersistor) / 4;
    MOTOR_DEBUG("saving motor parameters: stepcounter = %d, position = %d, alim = %d \r\n", 
                _persistentParams.stepCounter(), _persistentParams.position(), _persistentParams.alim());
    return _persistentParams.data();
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::restore(uint32_t* data)
{
    memcpy(_persistentParams.data(), data, sizeof(MotorPersistor));
    MOTOR_DEBUG("restoring motor parameters: stepcounter = %d, position = %d, alim = %d \r\n", 
                _persistentParams.stepCounter(), _persistentParams.position(), _persistentParams.alim());
}
#endif  // #if (USE_FLASH != 0)

// ------------------------------------------------------------------------------------------------------------------
void Motor::stepForward()
{
    if (_inverseRotation)
    {
        setDestination(_persistentParams.position() - _nbStepsForTick);
    }
    else
    {
        setDestination(_persistentParams.position() + _nbStepsForTick);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::singleStepForward()
{
    if (_inverseRotation)
    {
        setDestination(_persistentParams.position() - 1);
    }
    else
    {
        setDestination(_persistentParams.position() + 1);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::stepBackward()
{
    if (_inverseRotation)
    {
        setDestination(_persistentParams.position() + _nbStepsForTick);
    }
    else
    {
        setDestination(_persistentParams.position() - _nbStepsForTick);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::singleStepBackward()
{
    if (_inverseRotation)
    {
        setDestination(_persistentParams.position() + 1);
    }
    else
    {
        setDestination(_persistentParams.position() - 1);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Motor::updateNbStepsForAnimation(bool synchronousModeEnabled, uint8_t ppdc)
{
    
    if (synchronousModeEnabled)
    {
        _nbStepsForAnimation    = _nbSteps / NB_STEPS_FOR_ANIMATION_DIVIDER * (ppdc + 1 - _stepThreshold);
    }
    else
    {
        _nbStepsForAnimation    = _nbSteps / NB_STEPS_FOR_ANIMATION_DIVIDER;
    }
    MOTOR_DEBUG("_nbStepsForAnimation for %s: %d\r\n", toString(), _nbStepsForAnimation);
}

#endif  // #if (USE_MOTORS != 0)
