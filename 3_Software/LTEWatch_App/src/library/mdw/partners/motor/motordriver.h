/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * \file    motordriver.h
 *
 * @addtogroup Motor
 * \{
 *
 * @class   motor::MotorDriver
 * @brief   This class is the driver of a bidirectional stepper motor.
 *
 * The purpose of this class is to drive a bidirectional stepper motor. This class is responsible
 * for the timings of the pulse to be applied on the outputs driven the motor and the gap between
 * two motor's steps as displayed in the figure below.
 *
 * \image   html ./images/522.png "Caliber 522"
 * \image   html ./images/motorSpecifications.png "Stepper Motor's Specifications"
 *
 * To keep this timing stricly to the specifications, no XF's event are used but the concept of
 * state machine is keeped here. The different states of the state machine of this driver
 * is specified by the picture below:
 *
 * \image   html ./images/motorDriverStateMachine.png "Motor Driver State Machine"
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2016-08-18
 *
 * \author  Patrice Rudaz (patrice.rudaz@hevs.ch)
 * \date    July 2018
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include "isubject.hpp"
#include "motordriverhal.h"
#include "motor-config.h"
#include "motor.h"
#include "motorid.h"
#include "motordriverobserver.h"


#if(USE_MOTORS != 0)

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* NAMESPACE Declaration                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
namespace motor
{

    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* Class Declaration                                                                                            */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    class MotorDriver : public ISubject<MotorDriverObserver, 1>
    {
    public :
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* CONSTRUCTOR SECTION                                                                                      */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        MotorDriver();
        virtual ~MotorDriver();

        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Checks that the given motorId is part of the MotorId::eMotorId enumeration and returns the
         *          corresponding identifier.
         ************************************************************************************************************/
        MotorId::eMotorId checkMotorId(uint8_t motorId);

        /********************************************************************************************************//**
         * @brief   Initializes the components needed by this class
         *
         * This method initializes the GPIO (hardware) used to drive the stepper motors. To define which GPIO are
         * used for a motor, see motorhal.h.
         *
         * According to the number of motors involved in the watch movement, this method computes also the time for
         * the gap absolutely needed between two motor's impulse.
         *
         * @param   pulseLen    Defines in ms the duration of the pulse to be assigned on the motor's coil to
         *                      generate one step in a single direction.
         * @param   singleGap   The time to wait, in ms, between two steps on the same motor
         * @param   minGapMin   The time to wait, in ms, between two steps on two different motors
         *
         * \see     motorgpiodefine.h
         * \warning Must be called before any other method of this class
         ************************************************************************************************************/
        void initialize(uint32_t pulseLen           = MOTOR_PULSE_LEN,
                        uint32_t singleGap          = MOTOR_SINLGE_GAP_TIME,
                        uint32_t minGapMin          = MOTOR_MIN_GAP_TIME);

        /********************************************************************************************************//**
         * @brief   Register a new motor in the driver class.
         *
         * This driver can be used with watch's movement with one and up to five motors. Each motor has to be defined
         * and configured before to be added to the driver.
         *
         * This method `addMotor` MUST be used to register a new motor. In fact, this method just save the pointer to
         * the instance of a motor in a `vector` to keep a reference and be able to act on any motor's instance.
         *
         * @param   motor   Pointer to the new motor to register.
         *
         * @return  `true` if the new motor has been added to the driver successfully and `false` otherwise.
         ************************************************************************************************************/
        bool addMotor(Motor* motor);

        /********************************************************************************************************//**
         * @brief   Checks if each motor has reached its destination or which motor is being initiliazed.
         *
         * If `initializeMotor` is set to `FALSE`, this method checks if the motors have reached their destination.
         * The bit corresponding to a motor is set to `1` if this motor has not reached its destination.
         *
         * When `initializeMotor` is set to `TRUE`, this method is used to select the motor to be initialized (this
         * means the motor to be set in position 0). This is used to align the motor and to initialize the position
         * of the motors.
         *
         * The method retrieves an 8bit value where each bit identifies a motor and the value of each bit's value
         * specifies if the motor should move or not.
         *
         * \image   html ./images/motorIdx.png "Stepper Motor's 8bits Identifier"
         *
         * @param   initializingMotor   To know if we are in Motor Initialization's mode or not
         * @param   forcedDirection     If set to `0`, the direction  is not forced and the shortest way will
         *                              choosen by MotorDriver. If set to `+1`, `-1`, the direction is forced to be
         *                              clockwise, respectively counter clockwise. Default value: `0`.
         *
         * @return  An 8bit register specifying which motor should still move
         ************************************************************************************************************/
        uint8_t checkMotors(bool initializingMotor);

        /********************************************************************************************************//**
         * @brief   Clear the motor's pulse by setting all GPIOs to their low level
         ************************************************************************************************************/
        void clearPulse();

        /********************************************************************************************************//**
         * @brief   Apply on selected motor a defined number of steps.
         *
         * With this method, you can apply on a motor, defined by its identifier, a certain amount of steps in any
         * direction you want. To do, just specify the amount of steps and the direction of rotation.
         *
         * @param   motorId         The identifier of the motor to work with.
         * @param   forward         The direction of the rotation: `true` means clockwise.
         * @param   nbSteps         The number of steps to be done by the motor.
         * @param   updatePosition  If set to `true`, the position of the motor MUST be updated on each step.
         * @param   fastMove        MUST be set to `true`, if the method is used for moving the motor fast.
         * @param   asynchrone      Specify if all motors should move in the same or opposite direction.
         ************************************************************************************************************/
        void ctrlMotor(uint8_t  motorIds        = 0xff,
                       bool     forward         = true,
                       uint16_t nbSteps         = 180,
                       bool     updatePosition  = false,
                       bool     fastMove        = false,
                       bool     asynchrone      = false);

        /********************************************************************************************************//**
         * @brief   Initialize the motors we need to make some steps in one direction
         *
         * @param   motorId         The identifier of the motor to work with.
         * @param   forward         The direction of the rotation: `true` means clockwise.
         * @param   fastForward     The number of steps to be done by the motor.
         * @param   updatePosition  To know if the new position should be saved or not.
         ************************************************************************************************************/
        void initMotor(motor::MotorId::eMotorId motorId, bool forward, bool fastForward, bool updatePosition = false);

        /********************************************************************************************************//**
         * @brief   Cancel any movement of the current motor.
         ************************************************************************************************************/
        void cancelAllMotorsMove();

        /********************************************************************************************************//**
         * @brief   Cancel the initialization of current motor.
         ************************************************************************************************************/
        void cancelMotorInitMove();

        /********************************************************************************************************//**
         * @brief   Check all motors and move only those who have to.
         *
         * This method is used to move motors specified by `motorIds`. `motorIds` is an 8 bit register where each bit
         * is dedicated to a single motor as shown in the figure below. A `1` value in a bitmeans that this motor
         * must move.
         *
         * \image   html ./images/motorIdx.png "Stepper Motor's 8bits Identifier"
         *
         * To keep the synchronization and the timing for all motors of the watch's movement, a timer is started for
         * each motor and only for those who need to move, the pulse is given to the coil. The order in which each
         * motor are moved is defined by the `motorId`. It goes from the least to the most significant bit.
         *
         * @param   motorIds            The motor Identifiers. Each bit identify a motor.
         * @param   fromCalendarEvent   Set to `true` means that the move is given by the calendar (time keeping)
         * @param   fastMove            MUST be set to `true`, if the method is used for moving the motor fast.
         ************************************************************************************************************/
        void moveMotors(uint8_t motorIds, bool fromCalendarEvent);

        /********************************************************************************************************//**
         * @brief   Reset motor's move process
         *
         * This method resets all variables needed to achieve a motor step. It's called when all motors have reached
         * their destinations.
         ************************************************************************************************************/
        void motorMoveEnd();

        /********************************************************************************************************//**
         * @brief   Enable or disable the synchronous Mode.
         *
         * Synchronous mode is the way to move all motors at the same maximum speed even if they don't have all the
         * the same number of max steps. If you disable this mode, a motor of 60 steps will run three times quicker
         * than the one with 180 steps.
         *
         * @param   enable  `true` will enable the synchronous mode (default setting) and `false` will disable it.
         ************************************************************************************************************/
        void handleSynchronousMode(bool enable);

        /********************************************************************************************************//**
         * @brief   Retrieves the number of registered Motors
         ************************************************************************************************************/
        inline uint8_t nbRegisteredMotors() const
        {
            return _nbRegisteredMotors;
        }

        /********************************************************************************************************//**
         * @brief   Retrieves the identifier's mask of registered motors.
         ************************************************************************************************************/
        inline uint8_t registeredMotorsMask() const
        {
            return _registerdMotorsMask;
        }

        /********************************************************************************************************//**
         * @brief   Retruns the maximal number of steps of all registered motors.
         ************************************************************************************************************/
        inline uint16_t maxMotorSteps() const
        {
            return _maxMotorSteps;
        }

        /********************************************************************************************************//**
         * @brief   Retruns the minimal number of steps of all registered motors.
         ************************************************************************************************************/
        inline uint16_t minMotorSteps() const
        {
            return _minMotorSteps;
        }

        /********************************************************************************************************//**
         * @brief   Retruns the the number of steps for each of all registered motors in an array of bytes.
         ************************************************************************************************************/
        inline uint8_t* motorSpecifications()
        {
            return _motorSpecifications;
        }

        /********************************************************************************************************//**
         * @brief   Retrieves a pointer to the motor identified by its identifier.
         *
         * @param   motorId    The identifier of the motor to get back.
         ************************************************************************************************************/
        inline Motor* getMotorById(MotorId::eMotorId motorId) const
        {
            return _getMotorById(motorId);
        }

        /********************************************************************************************************//**
         * @brief   Retrieves the current position of the motor identified by its motorId.
         *
         * @param   motorId    The identifier of the motor to work with.
         ************************************************************************************************************/
        inline uint16_t getPositionByMotorId(MotorId::eMotorId motorId) const
        {
            return _getMotorById(motorId)->position();
        }

        /********************************************************************************************************//**
         * @brief   Retrieves the number of steps for the motor identified by its motorId.
         *
         * @param   motorId    The identifier of the motor to work with.
         ************************************************************************************************************/
        inline uint16_t nbStepsByMotorId(MotorId::eMotorId motorId) const
        {
            return _getMotorById(motorId)->nbSteps();
        }

        /********************************************************************************************************//**
         * @brief   Retrieves the number of steps for the motor identified by its motorId.
         *
         * @param   motorId    The identifier of the motor to work with.
         ************************************************************************************************************/
        inline uint16_t nbStepsForAnimation(MotorId::eMotorId motorId) const
        {
            return _getMotorById(motorId)->nbStepsForAnimation();
        }

        /********************************************************************************************************//**
         * @brief   Retrieves the number of steps for the motor identified by its motorId.
         *
         * @param   motorId    The identifier of the motor to work with.
         ************************************************************************************************************/
        inline uint8_t nbStepsForTick(MotorId::eMotorId motorId) const
        {
            return _getMotorById(motorId)->nbStepsForTick();
        }

        /********************************************************************************************************//**
         * @brief   Returns the last available motorId
         ************************************************************************************************************/
        MotorId::eMotorId firstMotorId() const;

        /********************************************************************************************************//**
         * @brief   Returns the last available motorId
         ************************************************************************************************************/
        MotorId::eMotorId lastMotorId() const;

        /********************************************************************************************************//**
         * @brief   Returns the next available motorId
         *
         * @param   motorId The current motor's identifier.
         ************************************************************************************************************/
        MotorId::eMotorId nextMotorId(uint8_t motorId) const;

        /********************************************************************************************************//**
         * @brief   Send a MotorTimerEvent to the main event critical queue.
         *
         * This method send an event to the main event's queue. This event is due to a motor step. When the impulse
         * is applied to the GPIO, an `app_timer` (nRF51) or the RTCTimer2 (nRF52) is started to handle the duration
         * of the motor's impulse or the duration of the gap between two motor's commands. This timer use a callback
         * method when its timeout is reached and this callback needs to raise a MotorTimerEnd to inform change the
         * state of the state machine of the motor driver.
         *
         * \see        MotorTimerEvent
         ************************************************************************************************************/
        void motorTimerEvent();

        /********************************************************************************************************//**
         * @brief   Set the new Motor Direction Configuration for a specific motor.
         *
         * This method sets the new Motor Direction Configuration to the MotorDriver
         *
         * @param   motorId         The identifier of the motor to work with.
         * @param   clockwise       The direction of the rotation: `true` means clockwise.
         ************************************************************************************************************/
        void setMotorDirectionConfiguration(motor::MotorId::eMotorId motorId, bool clockwise);

        /********************************************************************************************************//**
         * @brief   Set the new Motor Direction Configuration for all motors.
         *
         * This method sets the new Motor Direction Configuration for all motors to the MotorDriver
         *
         * @param   clockwise       The direction of the rotation: `true` means clockwise.
         ************************************************************************************************************/
        void setAllMotorsDirectionConfiguration(bool clockwise);

        /********************************************************************************************************//**
         * @brief   Set the direction to the given motor motors.
         *
         * Set the direction for the motor specified by its id by setting the parameter `forward` to  `TRUE` to go
         * clockwise and `FALSE` otherwise.
         *
         * @param   motorId     Identifier of the motor (as shown in the figure above).
         * @param   forward     Boolean value to specifiy the new direction to be applied to the motor specified by
         *                      `motorId`. `TRUE` to a clockwise rotation and `FALSE` to a counter-clockwise one.
         ************************************************************************************************************/
        void setDirectionOfMotor(MotorId::eMotorId motorId, bool forward);

        /********************************************************************************************************//**
         * @brief   Set the destination to a specific motor.
         *
         * \image   html ./images/motorIdx.png "Stepper Motor's 8bits Identifier"
         *
         * @param   motorId     Identifier of the motor (as shown in the figure above).
         * @param   destination The new destination given as an `int` value.
         ************************************************************************************************************/
        bool setDestination(MotorId::eMotorId motorId, uint16_t destination);

        /********************************************************************************************************//**
         * @brief   Set the destination to a specific motor.
         *
         * \image   html ./images/motorIdx.png "Stepper Motor's 8bits Identifier"
         *
         * @param   motorId     Identifier of the motor (as shown in the figure above).
         * @param   tick        The new tick's value to be display by the motor.
         ************************************************************************************************************/
        bool setDestinationForTicks(MotorId::eMotorId motorId, uint8_t tick);

        /********************************************************************************************************//**
         * @brief   Pre-formatted method to be used to update the position of the motor when it is
         *          associated to a watch's hand displaying the second.
         *
         * With this method you can directly give a value representing a second. If you want the motor to go to the
         * position representing the second 30, you just need to give the value 30 and the method will use the motor's
         * configuration to compute the new destination.
         *
         * \image   html ./images/10-10-30.png "Watch displaying 10:10:30"
         *
         * @param   motorId     The identifier of the motor to be used to display the second.
         * @param   second      The value of second to be displayed.
         * @param   mirroring   Flag to know if a mirroring filter should be applied to the position... Filter's
         *                      values are:
         *                        - 0: No mirror
         *                        - 1: Vertical mirror
         *                        - 2: Vertical mirror and hands set in opposite direction
         *                        - 3: Horizontal mirror
         *                        - 4: Horizontal mirror and hands set in opposite direction
         *
         * @return  The method returns `true` only if a motor's step is needed, otherwise `false` is returned.
         ************************************************************************************************************/
        bool setDestForSecond(MotorId::eMotorId motorId, uint8_t second, uint8_t mirroring = 0);

        /********************************************************************************************************//**
         * @brief   Pre-formatted method to be used to update the position of the motor when it is
         *          associated to a watch's hand displaying the second.
         *
         * With this method you get directly the position for a motor representing a second. If you want the
         * destination of the motor representing the second 30, you just need to give the value 30 and the method
         * will use the motor's configuration to compute the new destination.
         *
         * @param   motorId     The identifier of the motor to be used to display the second.
         * @param   second      The value of second to be displayed.
         *
         * @return  The method returns the position of the motor for a given "second".
         ************************************************************************************************************/
        uint16_t getDestForSecond(MotorId::eMotorId motorId, uint8_t second);

        /********************************************************************************************************//**
         * @brief   Pre-formatted method to be used to update the position of the motor when it is associated to a
         *          watch's hand displaying the minute.
         *
         * With this method you can directly give a value representing the minutes of the current time. If you want
         * the motor to go to the position representing the minute 20, you just need to give the value 20 and the
         * method will use the motor's configuration to compute the new destination. You need to give also the
         * current value of the second, to adapt the position of the minute's hand...
         *
         * As shown in the figure below, if the time is something like 10:10:30, we are used to see the hand of the
         * minutes between 10 and 11. This method will update the position of the hand of the minutes according to
         * the value of the seconds
         *
         * \image   html ./images/10-10-30.png "Watch displaying 10:10:30"
         *
         * @param   motorId     The identifier of the motor to be used to display the minute.
         * @param   minute      The value for the minute to be displayed.
         * @param   second      The value of second to update if it's needed the minute's position.
         * @param   mirroring   Flag to know if a mirroring filter should be applied to the position... Filter's
         *                      values are:
         *                        - 0: No mirror
         *                        - 1: Vertical mirror
         *                        - 2: Vertical mirror and hands set in opposite direction
         *                        - 3: Horizontal mirror
         *                        - 4: Horizontal mirror and hands set in opposite direction
         *
         * @return  The method returns `true` only if a motor's step is needed, otherwise `false` is returned.
         ************************************************************************************************************/
        bool setDestForMinute(MotorId::eMotorId motorId, uint8_t minute, uint8_t second, uint8_t mirroring = 0);

        /********************************************************************************************************//**
         * @brief   Pre-formatted method to be used to update the position of the motor when it is
         *          associated to a watch's hand displaying the minute.
         *
         * With this method you get directly the position for a motor representing a minute. If you want
         * the position representing the minute 20, you just need to give the value 20 and the
         * method will use the motor's configuration to compute the new destination. You also need to give the
         * current value of the second, to adapt the position of the minute's hand...
         *
         * @param   motorId     The identifier of the motor to be used to display the second.
         * @param   minute      The value for the minute to be displayed.
         * @param   second      The value of second to update if it's needed the minute's position.
         *
         * @return  The method returns the position of the motor for a given "second".
         ************************************************************************************************************/
        uint16_t getDestForMinute(MotorId::eMotorId motorId, uint8_t minute, uint8_t second);

        /********************************************************************************************************//**
         * @brief   Set the threshold for the minutes.
         *
         * In high-end watch movement, it's usual that the hand displaying the minutes move not only at the second 0.
         * According to the number of steps available on the motor of the minutes, one can process how many time in
         * a minute the hand should move. This threshold is processed by this method with the following algorithm:
         *
         *             `Threshold for minute  = (uint32_t) (60 [second] / (nbSteps / 60 [minute]))`
         *
         * For example, if the motor representing the minute has 180 steps, the hand of the minute could move of one
         * step each 20 seconds (= 60 [second] / (180 [steps] / 60 [minute]);).
         *
         * @param   motorId The identifier of the motor.
         ************************************************************************************************************/
        void setThresholdForMinute(MotorId::eMotorId motorId);

        /********************************************************************************************************//**
         * @brief   Pre-formatted method to be used to update the position of the motor when it is associated to a
         *          watch's hand displaying the hour.
         *
         * With this method you can directly give a value representing the hours of the current time.
         * If you want the motor to go to the position representing the hour 10, you just need to give the value 10
         * and the method will use the motor's configuration to compute the new destination. You need to give also
         * the current value of the minute, to adapt the position of the hour's hand...
         *
         * As shown in the figure below, if the time is something like 10:10:30, we are used to see the hand of the
         * hours a bit away of the 10. This method will update the position of the hand of the hours according to the
         * value of the minutes
         *
         * \image   html ./images/10-10-30.png "Watch displaying 10:10:30"
         *
         * @param   motorId     The identifier of the motor to be used to display the hour.
         * @param   hour        The value for the hour to be displayed.
         * @param   minute      The value of minute to update, if it's needed, the hour's position.
         * @param   mirroring   Flag to know if a mirroring filter should be applied to the position... Filter's
         *                      values are:
         *                        - 0: No mirror
         *                        - 1: Vertical mirror
         *                        - 2: Vertical mirror and hands set in opposite direction
         *                        - 3: Horizontal mirror
         *                        - 4: Horizontal mirror and hands set in opposite direction
         *
         * @return  The method returns `true` only if a motor's step is needed, otherwise `false` is returned.
         ************************************************************************************************************/
        bool setDestForHour(MotorId::eMotorId motorId, uint16_t hour, uint8_t minute, uint8_t mirroring = 0);

        /********************************************************************************************************//**
         * @brief   Pre-formatted method to be used to update the position of the motor when it is
         *          associated to a watch's hand displaying the hour.
         *
         * With this method you get directly the position for a motor representing an hour. If you want the
         * position representing the hour 10, you just need to give the value 10 and the method will use the
         * motor's configuration to compute the new destination. You also need to give the current value of the
         * minute, to adapt the position of the hour's hand...
         *
         * @param   motorId     The identifier of the motor to be used to display the second.
         * @param   hour        The value for the hour to be displayed.
         * @param   minute      The value of minute to update, if it's needed, the hour's position.
         *
         * @return  The method returns the position of the motor for a given "hour".
         ************************************************************************************************************/
        uint16_t getDestForHour(MotorId::eMotorId motorId, uint16_t hour, uint8_t minute);

        /********************************************************************************************************//**
         * @brief   Set the threshold for the hours.
         *
         * In high-end watch movement, it's usual that the hand displaying the hours move not only at the minute 0.
         * According to the number of steps available on the motor of the hours, one can process how many time in an
         * hour the hand should move. This threshold is processed by this method with the following algorithm:
         *
         *              `Threshold for minute  = (uint32_t) (60 [minute] / (nbSteps / 12 [hour]))`
         *
         * For example, if the motor representing the hour has 180 steps, the hand of the hour could move of one step
         * each 4 minutes (= 60 [minute] / (180 [steps] / 12 [hour]);).
         *
         * @param   motorId The identifier of the motor to be used to display the hour.
         ************************************************************************************************************/
        void setThresholdForHour(MotorId::eMotorId motorId);

        /********************************************************************************************************//**
         * @brief   Set the boolean parameter to known if the step MUST be considered or not.
         *
         * When the watch is running normally, each step of the motors must update a value defining their actual
         * position. There is one only situation where these values MUST not be updated: this is the initialization
         * of the motor. This mode exists to move each motor to a distination considered as the initial position.
         *
         * This method allows the software to change the value to the `_updatePosisition` parameter. The position
         * will be updated if it's set to `true` and won't otherwise.
         *
         * @param   updatePosition  The new value for `_updatePosisition` parameter.
         ************************************************************************************************************/
        void setUpdatePosition(bool updatePosition);

        /********************************************************************************************************//**
         * @brief   Fix the direction regarding the given one and the mask of the motor's identifiers.
         *
         * This method makes it possible to force the direction of rotation of all the engines whose identifier
         * corresponds to the given mask.
         *
         * @param   direction   If set to `0`, the direction is not forced and the shortest way will choosen. If set
         *                      to `+1`, `-1`, the direction is forced to be clockwise, respectively counter clockwise.
         * @param   motorIdMask The mask of motor's identifier defining which motor should has its direction forced.
         *                      Default value: 0xff, All motors !
         ************************************************************************************************************/
        void forceDirection(int8_t direction, uint8_t motorIdMask = 0xff);

        /********************************************************************************************************//**
         * @brief   Set the number of steps for fast init to a specific motor.
         *
         * \image   html ./images/motorIdx.png "Stepper Motor's 8bits Identifier"
         *
         * @param   motorId     Identifier of the motor (as shown in the figure above).
         * @param   destination The new destination given as an `int` value.
         ************************************************************************************************************/
        void setStepsForFastInit(MotorId::eMotorId motorId, uint16_t stepsForFastInit);

        /********************************************************************************************************//**
         * @brief   Initialise the state machine, without using the heritage of XFReactive.
         ************************************************************************************************************/
        void startBehaviour();

        /********************************************************************************************************//**
         * @brief   Send the actual position of all motors to the connected device.
         ************************************************************************************************************/
        bool updateMotorPosition();

        /********************************************************************************************************//**
         * @brief   Update the `Motor Specification` characteristic of Soprod SmartWatch BLE Service
         ************************************************************************************************************/
        bool updateMotorSpecification();

    protected:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PROTECTED DECLARATION SECTION                                                                            */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Enumeration used to have a unique identifier for every state in the state machine.
         ************************************************************************************************************/
        typedef enum
        {
            ST_UNKOWN = 0,                                                                                              ///< @brief Unkown state
            ST_INIT,                                                                                                    ///< @brief Initial state
            ST_IDLE,                                                                                                    ///< @brief Wait state
            ST_SWITCH_MOTOR,                                                                                            ///< @brief State switching motor id to select the next motor to work with
            ST_PULSE,                                                                                                   ///< @brief State doing a motor step
            ST_NO_MORE_PULSE                                                                                            ///< @brief State skipping a motor's move
        } eMainState;
        eMainState                  _currentState;                                                                      ///< @brief Attribute indicating currently active state of the state machine

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Attributes
        hal::MotorDriverHal         _mDriverHAL;                                                                        // Low level driver
        Motor*                      _motors[NB_MAX_MOTORS];                                                             // Array of references on registered Motor
        Motor*                      _activeMotor;                                                                       // Pointer to the motor currently in use

        uint8_t                     _motorCoilsSize;                                                                    // To store the size of all coils needed by the registered motors. It could be less than NB_MAX_MOTORS
        #if (MOTORS_CONN_SERIES != 0)
            static const uint8_t    _motorCoilsFactor = 2;
            uint8_t                 _motorCoils[2 * NB_MAX_MOTORS + 1];                                                 // To store all GPIO's id used to drive all motor's coils
        #else
            static const uint8_t    _motorCoilsFactor = 3;
            uint8_t                 _motorCoils[3 * NB_MAX_MOTORS];                                                     // To store all GPIO's id used to drive all motor's coils
        #endif

        bool                        _updatePosition;                                                                    // Boolean parameter to know if the change in position MUST be stores
        bool                        _fromCalendarEvt;                                                                   // Just to know where it comes from
        bool                        _fastMoveEnabled;                                                                   // Set to `true` for moving a motor fast.
        bool                        _cancelAnyMove;                                                                     // To cancel a motor's move...
        bool                        _synchronousMode;                                                                   // All motors move synchronously, even if they don't have the same max number of steps
        uint8_t                     _nbRegisteredMotors;                                                                // Total amount of registered Motor
        uint8_t                     _ppdc;                                                                              // Smallest common divisor of all motor's max number of steps
        uint8_t                     _registerdMotorsMask;                                                               // Bitfield with registered motor's ID
        uint8_t                     _motorPositions[2 * NB_MAX_MOTORS];                                                 // Array of all motor's positions. Each motor's position is stored as an 2 * 8 bit value
        uint8_t                     _motorSpecifications[2 * NB_MAX_MOTORS + 1];                                        // specify the number of steps for each motor
        uint16_t                    _motorPositionsSize;                                                                // Size of the array of positions

        uint32_t                    _pulseLen;                                                                          // Duration of the motor's pulse in microseconds
        uint32_t                    _singleGap;                                                                         // Default delay between two motor's pulse applied on the same motor
        uint32_t                    _singleGapTick;                                                                     // Timer timeout tick value to achieve the gap deley specification for pulses on same motor
        uint32_t                    _minGap;                                                                            // Minimal delay allowed between motor's pulses applied on different motors.
        uint32_t                    _minGapTick;                                                                        // Minimal delay expressed in timer's tick.

        uint8_t                     _minuteThreshold;                                                                   // As motor could have more than 60 steps, this parameter set the threshold to move the motor dedicated for representing the minute value of one step
        uint8_t                     _minuteThresholdHalf;                                                               // Half of the previous value ;-) This parameter is used if the minute (or hour) motor won't tick at noon
        uint8_t                     _hourThreshold;                                                                     // Same for the hour ...
        uint8_t                     _hourThresholdHalf;

        uint8_t                     _moveMotorIds;                                                                      // 8bit register where each bit is dedicated to a motor. A bit set to 1 means that the motor identified by this bit MUST move.
        uint16_t                    _moveMotorNbSteps;                                                                  // Number of steps to be applied for the motors
        uint8_t                     _activeMotorIdx;                                                                    // Identify the motor we are currently working with
        uint8_t                     _circularMotorHandlerCounter;                                                       // Circular counter to synchronize rotation speed of all motors

        uint16_t                    _maxMotorSteps;                                                                     // Number MAX of steps ever reached for all registered motor in the driver
        uint16_t                    _minMotorSteps;                                                                     // Number MIN of steps ever reached for all registered motor in the driver

        // Methods
        uint8_t                     _checkMotor(uint8_t motorId, int8_t forceDirection = 0);                            // Check if the motor defined by `motorId` has reached its destination. If direction is forced, the driver won't find the shortest way to reach the destination of the motors.
        Motor*                      _getMotorById(uint8_t motorId) const;                                               // Looks for the motor according to its identifier
        uint8_t                     _handleMirroring(uint8_t mirroring, uint16_t position, Motor* motor);               // Handle the mirroring according to the selected mirroring setup
        void                        _initializeMotorParameters();                                                       // After all motors registration, initialization of the driver parameters
        void                        _notifyStepsEnd();                                                                  // Notify the observer about the end of the movement
        void                        _resetAllMotorsDestination();                                                       // After a call of CancelAllMotorsMove(), we need to reset also all destinations
        void                        _resetMotorMoveParam();                                                             // Reset all needed varaibles to stop moving a motor
        void                        _setDirectionOfMotors(uint8_t motorIds, bool forward, bool asynchroneDirection = false);// Set the direction in one shot for all motors. By setting `forward` to `TRUE`, the direction will be clockwise.
        void                        _setTimerDuration();                                                                // Setup the timer's value for impulse and gap duration
        void                        _setupCoils(uint8_t coilA, uint8_t coilB, uint8_t coilC);                           // Register the motor's coils
        void                        _setupPulseMask();                                                                  // set up the mask to be applied on the GPIOs to drive all motors
        bool                        _setDestination(Motor* motor, uint16_t destination);                                // Setup the timer's value for impulse and gap duration
        void                        _singleStep();                                                                      // Method starting a motor step
        void                        _ST_INIT_Action();                                                                  // Action when the motor's driver is in INIT state
        void                        _switchMotor(bool fromCalendarEvent);                                               // shift the motor's id to select the next motor to move
        void                        _updateNbStepsForAnimation();                                                       // According to the Synchrounous State, it updates the number of steps to be done during default animation (fe: During motor Initialization)
        uint16_t                    _validateNbStepsToMove(uint16_t nbSteps);                                           // Checks that the number of steps to move is really a multiple of _ppdc

        inline bool                 _findNextMotorIdx();                                                                // Find the next index cooresponding to the next motor to move
        inline bool                 _incrActiveMotorIdx(bool fromCalendarEvent);                                        // Increment the circular counter and retrieve its new value
        inline void                 _incrCircularCounter();                                                             // Increment the circular motor index and retrieve if a gap timer should be started
        inline void                 _processGapTick();                                                                  // Compute the duration of a "No Pulse" gap according to the number of motors to move and starts a timer
        inline bool                 _startNoPulseTimer()            { return _mDriverHAL.startTimerNoPulse(); }         // Starts a timer to skip the active motor.
        inline bool                 _startPulseTimer()              { return _mDriverHAL.startTimerPulse(); }           // Starts the timer for a motor's pulse (2.89 ms)
    };

}   // namespace motor

#endif    // #if (USE_MOTORS != 0)

/** \} */   // Group: Motor
