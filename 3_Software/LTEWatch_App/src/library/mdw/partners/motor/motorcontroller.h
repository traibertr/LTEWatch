/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * \file    motorcontroller.h
 *
 * @addtogroup Motor
 * \{
 *
 * @class   motor::MotorController
 * @brief   This class manages the motor's movement.
 *
 * # MotorController
 *
 * This is the state machine that handles the motor's movement...
 *
 * \image html ./images/motorControllerStateMachine.png "Motor Controller State Machine"
 *
 * The way this state machine works is very simple. The different states of the transition are the following:
 *   - <i>ST_INIT:</i>
 *     This is the first state where every state machine MUST pass through and where every object and
 *     elements are initialized.
 *
 *   - <i>ST_MOTOR_INIT:</i>
 *     Initialize all motors by moving of one step on each directions.
 *
 *   - <i>ST_IDLE:</i>
 *     The state where nothing is done. Should be the the state in which the watch is most often.
 *
 *   - <i>ST_CHECK:</i>
 *     When the motor controller enter this state, the position of every motor are compared to their destination.
 *     The Motor Controller checks every motor and save in a register the index of the motor that must still move.
 *
 *     If no move is needed, a `DefaultEvent` is raised and the state machine runs in `ST_Idle` state. If one or more
 *     motors have to move, a `MotorEvent` is generated and the state machine goes in `ST_Move`.
 *
 *   - <i>ST_MOVE:</i>
 *     In this state, one step is applied to the motors that need to move. After the step, a `DefaultEvent` is raised
 *     and the state machine goes back to `ST_Check` state.
 *
 *   - <i>ST_CHECK_INIT:</i>
 *     State checking if motor's init steps are needed
 *
 *   - <i>ST_MOVE_INIT:</i>
 *     State doing motor's init step
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
#include <platform-config.h>

#if (USE_MOTORS != 0)

#include "isubject.hpp"
#include "xfreactive.h"

#include "motordriver.h"
#include "motor-config.h"
#include "motorctrlobserver.h"
#include "motordriverobserver.h"
#include "motorevent.h"
#include "motorendevent.h"
#include "motorinitevent.h"

#if (USE_CALENDAR != 0)
    #include "datetime.h"
#endif  // #if (USE_CALENDAR != 0)

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
    class MotorController :
        public XFReactive,
        public motor::MotorDriverObserver,
        public ISubject<MotorCtrlObserver, MOTOR_MAX_OBSERVERS>
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* CONSTRUCTOR SECTION                                                                                      */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        MotorController();
        virtual ~MotorController();

        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   The MotorDriver notify the controller that it has finished handling all motors.
         *
         * @param   hotReset    To know if a `reset` has occured at runtime...
         ************************************************************************************************************/
        void initialize(bool hotReset);

        /********************************************************************************************************//**
         * @brief   Enable the Synchronous Mode. All motors will use the same maximum speed.
         ************************************************************************************************************/
        inline void enableSynchronousMode()                             { _mDriver.handleSynchronousMode(true); }

        /********************************************************************************************************//**
         * @brief   Disable the Synchronous Mode. All motors will use their own maximum speed (60Hz).
         ************************************************************************************************************/
        inline void disableSynchronousMode()                            { _mDriver.handleSynchronousMode(false); }

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
        inline bool addMotor(Motor* motor)                              { return _mDriver.addMotor(motor); }

        /********************************************************************************************************//**
         * @brief   Retrieves the number of steps for the motor identified by its motorId.
         *
         * @param   motorId    The identifier of the motor to work with.
         ************************************************************************************************************/
        inline uint16_t motorNbSteps(MotorId::eMotorId motorId) const   { return _mDriver.nbStepsByMotorId(motorId); }

        /********************************************************************************************************//**
         * @brief   Retruns the the number of steps for each of all registered motors in an array of bytes.
         ************************************************************************************************************/
        inline uint8_t* motorSpecifications()                           { return _mDriver.motorSpecifications(); }

        /********************************************************************************************************//**
         * @brief   Returns the next available motorId
         *
         * @param   motorId The current motor's identifier.
         ************************************************************************************************************/
        inline MotorId::eMotorId nextMotorId(uint8_t motorId) const     { return _mDriver.nextMotorId(motorId); }

        /********************************************************************************************************//**
         * @brief   Returns the last available motorId
         ************************************************************************************************************/
        inline MotorId::eMotorId firstMotorId() const                   { return _mDriver.firstMotorId(); }

        /********************************************************************************************************//**
         * @brief   Returns the last available motorId
         ************************************************************************************************************/
        inline MotorId::eMotorId lastMotorId() const                    { return _mDriver.lastMotorId(); }

        /********************************************************************************************************//**
         * @brief   Returns the motor driver
         ************************************************************************************************************/
        inline MotorDriver motorDriver() const                          { return _mDriver; }

        /********************************************************************************************************//**
         * @brief   This methd clears the GPIOs used to drive the motors.
         *
         * This method is used to clear the GPIOs used to drive the motors to end any step on a motor. As the
         * nRF51 has onyl 2 hardware RTCTimers, the motor's driver must use an `app_timer` to get the different
         * timing needed.
         ************************************************************************************************************/
        inline void clearPulseOnActiveMotor()                           { _mDriver.clearPulse(); }

        /********************************************************************************************************//**
         * @brief   Send a MotorTimerEvent to the main event critical queue of the MotorDriver.
         ************************************************************************************************************/
        inline void notifyMotorTimerEvent()                             { _mDriver.motorTimerEvent(); }

        /********************************************************************************************************//**
         * @brief   Cancel any motor's initialization.
         ************************************************************************************************************/
        inline void cancelMotorInit()                                   { _mDriver.cancelMotorInitMove(); }

        /********************************************************************************************************//**
         * @brief   Cancel any movement of all motors.
         ************************************************************************************************************/
        inline void cancelAllMotorsMove()                               { _mDriver.cancelAllMotorsMove(); }

        /********************************************************************************************************//**
         * @brief   Retrieves the number of steps for animation when switching motor in initialization.
         *
         * Retrieves the number of steps for the animation done when switching motors in initalistion's mode. The
         * motor is selected by the given `motorId`, which must be one of the MotorId::eMotorId (see image below).
         * If `motorId` is not in the enum of motor's identifier, the method returns `0`, otherwise it should return
         * the desired number of steps to do.
         *
         * \image   html ./images/motorIdx.png "Stepper Motor's 8bits Identifier"
         *
         * @param   motorId     Identifier of the motor (as shown in the figure above).
         *
         * @return  The number of steps to do according the moto to work with or `0` if the given motorId is NOT part
         *          of the enumeration of motor's identifiers and `false` otherwise.
         ************************************************************************************************************/
        uint16_t getStepsForInitAnimation(uint8_t motorId);

        /********************************************************************************************************//**
         * @brief   Cancel any motor's fast move.
         ************************************************************************************************************/
        void cancelFastMove();

        /********************************************************************************************************//**
         * @brief   Send a default event to the main event queue.
         *
         * This method send an event to the main event's queue. This event is due to a request from an extern module
         * to check the motor's destination and decide if a motor step is needed. Nothing else is needed here. Every
         * parameter about motor and motor steps is handled by the motor's driver.
         *
         * @param   calendarEvent   Boolean parameter to identify a motor's move due to a calendar Event.
         *                          Default value is `false`
         *
         * \see     MotorCmdEvent
         ************************************************************************************************************/
        void raiseMotorCmdEvent(bool calendarEvent = false);

        /********************************************************************************************************//**
         * @brief   Send a default event to the main event queue.
         *
         * This method send an event to the main event's queue to start a motor initialization. The motor's
         * intialization is defined as the action to move every motor to a defined position which will be recognized
         * as the start position, the `zero` position. Any further move will be referenced to this position.
         *
         * @param   motorId         The identifier of the motor to work with.
         * @param   forward         The direction of the rotation: `true` means clockwise.
         * @param   fast            If set to `true`, more than 1 step is done. The number of step is defined during
         *                           themotor's initialization. \see Motor::initialize()
         * @param   updatePosition  To know if the new position should be saved or not.
         ************************************************************************************************************/
        void raiseMotorInitEvent(motor::MotorId::eMotorId motorId, bool forward, bool fast, bool updatePosition = false);

        /********************************************************************************************************//**
         * @brief   Set the new Motor Direction Configuration for a specific motor.
         *
         * This method forward the new Motor Direction Configuration to the MotorDriver
         *
         * @param   motorId         The identifier of the motor to work with.
         * @param   clockwise       The direction of the rotation: `true` means clockwise.
         ************************************************************************************************************/
        inline void setMotorDirectionConfiguration(motor::MotorId::eMotorId motorId, bool clockwise)
        {
            _mDriver.setMotorDirectionConfiguration(motorId, clockwise);
        }

        /********************************************************************************************************//**
         * @brief   Set the new Motor Direction Configuration for all motors.
         *
         * This method forward the new Motor Direction Configuration for all motors to the MotorDriver
         *
         * @param   clockwise       The direction of the rotation: `true` means clockwise.
         ************************************************************************************************************/
        inline void setAllMotorsDirectionConfiguration(bool clockwise)
        {
            _mDriver.setAllMotorsDirectionConfiguration(clockwise);
        }

        /********************************************************************************************************//**
         * @brief   Set the destination to a specific motor.
         *
         * \image   html ./images/motorIdx.png "Stepper Motor's 8bits Identifier"
         *
         * @param   motorId         Identifier of the motor (as shown in the figure above).
         * @param   destination     The new destination given as an `int` value.
         * @param   forcedDirection If set to `0`, the direction is not forced and the shortest way will be choosen by
         *                          MotorDriver. If set to `+1`, `-1`, the direction is forced to be clockwise,
         *                          respectively counter-clockwise. Default value: `0`.
         * @param   autoStart       If set to `true`, the motor will start moving automatically.
         *                          Default value is `false`.
         * @param   calendarEvent   Boolean parameter to identify a motor's move due to a calendar Event.
         *                          Default value is `false`
         *
         * @return  The method returns `true` only if a motor's step is needed, otherwise `false` is returned.
         ************************************************************************************************************/
        bool setDestination(MotorId::eMotorId motorId,
                            uint16_t destination,
                            int8_t forcedDirection = 0,
                            bool autoStart = false,
                            bool calendarEvent = false);

        /********************************************************************************************************//**
         * @brief   Set the destination to a specific motor.
         *
         * @param   motorId         Identifier of the motor (as shown in the figure above).
         * @param   tick            The new tick's value to be display by the motor.
         * @param   forcedDirection If set to `0`, the direction is not forced and the shortest way will be choosen by
         *                          MotorDriver. If set to `+1`, `-1`, the direction is forced to be clockwise,
         *                          respectively counter clockwise. Default value: `0`.
         * @param   autoStart       If set to `true`, the motor will start moving automatically.
         *                          Default value is `false`.
         * @param   calendarEvent   Boolean parameter to identify a motor's move due to a calendar Event.
         *                          Default value is `false`
         *
         * @return  The method returns `true` only if a motor's step is needed, otherwise `false` is returned.
         ************************************************************************************************************/
        bool setDestinationForTicks(MotorId::eMotorId motorId,
                                    uint8_t tick,
                                    int8_t forcedDirection = 0,
                                    bool autoStart = false,
                                    bool calendarEvent = false);

        /********************************************************************************************************//**
         * @brief   Set the destination to the motor driving the second's hand.
         *
         * @param   motorId         Identifier of the motor of the second's hand.
         * @param   second          The new second's position where the motor needs to go to.
         * @param   updatePosition  Flag to know if the position has to be updated or not.
         * @param   mirroring       Flag to know if a mirroring filter should be applied to the position... Filter's
         *                          values are:
         *                            - 0: No mirror
         *                            - 1: Vertical mirror
         *                            - 2: Vertical mirror and hands set in opposite direction
         *                            - 3: Horizontal mirror
         *                            - 4: Horizontal mirror and hands set in opposite direction
         * @param   forcedDirection If set to `0`, the direction is not forced and the shortest way will be choosen by
         *                          MotorDriver. If set to `+1`, `-1`, the direction is forced to be clockwise,
         *                          respectively counter clockwise. Default value: `0`.
         * @param   autoStart       If set to `true`, the motor will start moving automatically.
         *                          Default value is `false`.
         * @param   calendarEvent   Boolean parameter to identify a motor's move due to a calendar Event.
         *                          Default value is `false`
         *
         * @return  The method returns `true` only if a motor's step is needed, otherwise `false` is returned.
         ************************************************************************************************************/
        bool setDestForSecond(MotorId::eMotorId motorId,
                              uint8_t second,
                              bool updatePosition = true,
                              uint8_t mirroring = 0,
                              int8_t forcedDirection = 0,
                              bool autoStart = false,
                              bool calendarEvent = false);

        /********************************************************************************************************//**
         * @brief   Get the position of the motor representing the second's hand for a given value.
         *
         * @param   motorId         Identifier of the motor of the second's hand.
         * @param   second          The new second's value where the motor needs to go to.
         *
         * @return  The method returns the position of the motor for the given second value.
         ************************************************************************************************************/
        inline uint16_t getDestForSecond(MotorId::eMotorId motorId, uint8_t second) { return _mDriver.getDestForSecond(motorId, second); }

        /********************************************************************************************************//**
         * @brief   Set the destination to the motor driving the minute's hand. According to the actual second.
         *
         * This method set the destination of the motor driving the minute's hand to the current time. According to the
         * second, it slightliy move the minute's hand to give the same behaviour as a mechanical watch.
         *
         * @param   motorId         Identifier of the motor of the minute's hand.
         * @param   minute          The new minute's position where the motor needs to go to.
         * @param   second          The actual second to adjust the position of the minute's hand.
         * @param   updatePosition  Flag to know if the position has to be updated or not.
         * @param   mirroring       Flag to know if a mirroring filter should be applied to the position... Filter's
         *                          values are:
         *                            - 0: No mirror
         *                            - 1: Vertical mirror
         *                            - 2: Vertical mirror and hands set in opposite direction
         *                            - 3: Horizontal mirror
         *                            - 4: Horizontal mirror and hands set in opposite direction
         * @param   forcedDirection If set to `0`, the direction is not forced and the shortest way will be choosen by
         *                          MotorDriver. If set to `+1`, `-1`, the direction is forced to be clockwise,
         *                          respectively counter clockwise. Default value: `0`.
         * @param   autoStart       If set to `true`, the motor will start moving automatically.
         *                          Default value is `false`.
         * @param   calendarEvent   Boolean parameter to identify a motor's move due to a calendar Event.
         *                          Default value is `false`
         *
         * @return  The method returns `true` only if a motor's step is needed, otherwise `false` is returned.
         ************************************************************************************************************/
        bool setDestForMinute(MotorId::eMotorId motorId,
                              uint8_t minute,
                              uint8_t second,
                              bool updatePosition = true,
                              uint8_t mirroring = 0,
                              int8_t forcedDirection = 0,
                              bool autoStart = false,
                              bool calendarEvent = false);

        /********************************************************************************************************//**
         * @brief   Get the position of the motor representing the minute's hand for a given value.
         *
         * @param   motorId         Identifier of the motor of the second's hand.
         * @param   minute          The new minute's value where the motor needs to go to.
         * @param   second          The actual second to adjust the position of the minute's hand.
         *
         * @return  The method returns the position of the motor for the given minute value.
         ************************************************************************************************************/
        inline uint16_t getDestForMinute(MotorId::eMotorId motorId, uint8_t minute, uint8_t second) { return _mDriver.getDestForMinute(motorId, minute, second); }

        /********************************************************************************************************//**
         * @brief   Set the destination to the motor driving the hour's hand. According to the actual second.
         *
         * This method set the destination of the motor driving the hour's hand to the current time. According to the
         * minute, it slightliy move the hour's hand to give the same behaviour as a mechanical watch.
         *
         * @param   motorId         Identifier of the motor of the hour's hand.
         * @param   hour            The new hour's destination where the motor needs to go to.
         * @param   minute          The actual minute to adjust the position of the hour's hand.
         * @param   updatePosition  Flag to know if the position has to be updated or not.
         * @param   mirroring       Flag to know if a mirroring filter should be applied to the position... Filter's
         *                          values are:
         *                            - 0: No mirror
         *                            - 1: Vertical mirror
         *                            - 2: Vertical mirror and hands set in opposite direction
         *                            - 3: Horizontal mirror
         *                            - 4: Horizontal mirror and hands set in opposite direction
         * @param   forcedDirection If set to `0`, the direction  is not forced and the shortest way will choosen by
         *                          MotorDriver. If set to `+1`, `-1`, the direction is forced to be clockwise,
         *                          respectively counter clockwise. Default value: `0`.
         * @param   autoStart       If set to `true`, the motor will start moving automatically.
         *                          Default value is `false`.
         * @param   calendarEvent   Boolean parameter to identify a motor's move due to a calendar Event.
         *                          Default value is `false`
         *
         * @return  The method returns `true` only if a motor's step is needed, otherwise `false` is returned.
         ************************************************************************************************************/
        bool setDestForHour(MotorId::eMotorId motorId,
                            uint16_t hour,
                            uint8_t minute,
                            bool updatePosition = true,
                            uint8_t mirroring = 0,
                            int8_t forcedDirection = 0,
                            bool autoStart = false,
                            bool calendarEvent = false);

        /********************************************************************************************************//**
         * @brief   Get the position of the motor representing the hour's hand for a given value.
         *
         * @param   motorId         Identifier of the motor of the second's hand.
         * @param   hour            The new hours's value where the motor needs to go to.
         * @param   minute          The actual minute to adjust the position of the minute's hand.
         *
         * @return  The method returns the position of the motor for the given hour value.
         ************************************************************************************************************/
        inline uint16_t getDestForHour(MotorId::eMotorId motorId, uint8_t hour, uint8_t minute) { return _mDriver.getDestForHour(motorId, hour, minute); }

        /********************************************************************************************************//**
         * @brief   Set the destination to motors.
         *
         * \image   html ./images/motorIdx.png "Stepper Motor's 8bits Identifier"
         *
         * @param   destinations    The new destinations given as an array of `uint16_t` value.
         * @param   size            The number of destination
         * @param   updatePosition  Flag to know if the position has to be updated or not.
         * @param   forcedDirection If set to `0`, the direction  is not forced and the shortest way will choosen by
         *                          MotorDriver. If set to `+1`, `-1`, the direction is forced to be clockwise,
         *                          respectively counter clockwise. Default value: `0`.
         * @param   autoStart       If set to `true`, the motor will start moving automatically.
         *                          Default value is `false`.
         * @param   calendarEvent   Boolean parameter to identify a motor's move due to a calendar Event.
         *                          Default value is `false`
         *
         * @return  The method returns `true` only if a motor's step is needed, otherwise `false` is returned.
         ************************************************************************************************************/
        bool setDestinations(uint16_t* destinations,
                             uint8_t size,
                             bool updatePosition = true,
                             int8_t forcedDirection = 0,
                             bool autoStart = false,
                             bool calendarEvent = false);

        #if (USE_CALENDAR != 0)
            /****************************************************************************************************//**
            * @brief   Set the destination to motors according to a given date and time.
            *
            * @param   time                 The new date and time to display with hands (motor's positions).
            * @param   adjustSecond         To know if the seconds should be displayed also.
            * @param   displayWeekNumber    To know if the week's number should be displayed instead of the day 
            *                               number.
            * @param   mirroring            Flag to know if a mirroring filter should be applied to the position... 
            *                               Filter's values are:
            *                                - 0: No mirror
            *                                - 1: Vertical mirror
            *                                - 2: Vertical mirror and hands set in opposite direction
            *                                - 3: Horizontal mirror
            *                                - 4: Horizontal mirror and hands set in opposite direction
            * @param   forcedDirection      If set to `0`, the direction  is not forced and the shortest way will 
            *                               choosen by MotorDriver. If set to `+1`, `-1`, the direction is forced to 
            *                               be clockwise, respectively counter clockwise. Default value: `0`.
            * @param   batteryLevel         Value to be displayed with one of the motors. The value can be set 
            *                               between 0 and 100, representing the charge level of the battery in 
            *                               percent. If it's set to -1, this means that it should not be displayed.
            * @param   autoStart            If set to `true`, the motor will start moving automatically.
            *                               Default value is `false`.
            * @param   calendarEvent        Boolean parameter to identify a motor's move due to a calendar Event.
            *                               Default value is `true`
            *
            * @param   secondAtNoon         On specific battery's state, we can decide to let the second's hand at 
            *                               noon. This parameter set to `true` will force to move the motor driving
            *                               the second to the position 0.
            *
            * @return  The method returns `true` only if a motor's step is needed, otherwise `false` is returned.
            ********************************************************************************************************/
            bool setDestinationsForTime(cal::DateTime* time,
                                        bool    adjustSecond,
                                        bool    secondAtNoon = false,
                                        bool    displayWeekNumber = false,
                                        int8_t  forcedDirection = 0,
                                        int8_t  batteryLevel = -1,
                                        uint8_t mirroring = 0,
                                        bool    autoStart = false,
                                        bool    calendarEvent = true);
        #endif  // #if (USE_CALENDAR != 0)

        /********************************************************************************************************//**
         * @brief   Set the destination to motors according to the given hour, minute, second and day or weeknumber.
         *
         * @param   adjustSecond    To know if the seconds should be displayed also.
         * @param   secondAtNoon    To know if the second's hand must be set to noon (position 0).
         * @param   second          The new destination for the motor displaying the seconds.
         * @param   minute          The new destination for the motor displaying the minutes.
         * @param   hour            The new destination for the motor displaying the hours.
         * @param   dayOrWeek       The new destination for the motor displaying the date or the week number.
         * @param   batteryLevel    Value to be displayed with one of the motors. The value can be set between 0 and
         *                          100, representing the charge level of the battery in percent. If it's set to -1, 
         *                          this means that it should not be displayed.
         * @param   forcedDirection If set to `0`, the direction  is not forced and the shortest way will choosen by
         *                          MotorDriver. If set to `+1`, `-1`, the direction is forced to be clockwise,
         *                          respectively counter clockwise. Default value: `0`.
         * @param   mirroring       Flag to know if a mirroring filter should be applied to the position... Filter's
         *                          values are:
         *                            - 0: No mirror
         *                            - 1: Vertical mirror
         *                            - 2: Vertical mirror and hands set in opposite direction
         *                            - 3: Horizontal mirror
         *                            - 4: Horizontal mirror and hands set in opposite direction
         * @param   autoStart       If set to `true`, the motor will start moving automatically.
         *                          Default value is `false`.
         * @param   calendarEvent   Boolean parameter to identify a motor's move due to a calendar Event.
         *                          Default value is `true`
         *
         * @return  The method returns `true` only if a motor's step is needed, otherwise `false` is returned.
         ************************************************************************************************************/
        bool setDestinationsForTime(bool    adjustSecond,
                                    bool    secondAtNoon,
                                    uint8_t second,
                                    uint8_t minute,
                                    uint8_t hour,
                                    uint8_t dayOrWeek,
                                    int8_t  batteryLevel = -1,
                                    int8_t  forcedDirection = 0,
                                    uint8_t mirroring = 0,
                                    bool autoStart = false,
                                    bool calendarEvent = true);

        /********************************************************************************************************//**
         * @brief   Set the threshold for the minutes.
         *
         * In high-end watch movement, it's usual that the hand displaying the minutes move not only at the second 0.
         * According to the number of steps available on the motor of the minutes, one can process how many time in a
         * minute the hand should move. This threshold is processed by this method with the following algorithm:
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
         * @brief   Set the max number of steps for fast init for a single specific motorto a specific motor.
         *
         * Set the max number of steps for fast init for a single specific motor. The motor is selected by the given
         * `motorId`, which must be one of the MotorId::eMotorId (see image below). If `motorId` is not in the enum
         * of motor's identifier, the method returns `false`, otherwise it should return `true`.
         *
         * \image   html ./images/motorIdx.png "Stepper Motor's 8bits Identifier"
         *
         * @param   motorId     Identifier of the motor (as shown in the figure above).
         * @param   nbSteps     The number of steps the motor should make in fast initialization's mode. If this 
         *                      number is zero, then the max number of step available for the given motor will be
         *                      used.
         *
         * @return  `true` if the given motorId is part of the enumeration of motor's identifiers and `false` 
         *          otherwise.
         ************************************************************************************************************/
        bool setStepsForFastInit(uint8_t motorId, uint16_t nbSteps = 0);

        /********************************************************************************************************//**
         * @brief   Set the number of steps for animation when switching motor in initialization.
         *
         * Set the number of steps for the animation done when switching motors in initalistion's mode. The motor is
         * selected by the given `motorId`, which must be one of the MotorId::eMotorId (see image below). If `motorId`
         * is not in the enum of motor's identifier, the method returns `false`, otherwise it should return `true`.
         *
         * \image   html ./images/motorIdx.png "Stepper Motor's 8bits Identifier"
         *
         * @param   motorId     Identifier of the motor (as shown in the figure above).
         *
         * @return  `true` if the given motorId is part of the enumeration of motor's identifiers and `false` 
         *          otherwise.
         ************************************************************************************************************/
        bool setStepsForInitAnimation(uint8_t motorId);

        /********************************************************************************************************//**
         * @brief   Initializes all motors by doing one step on each direction (CW and CCW).
         *
         * This class knows after the initialization how many motors are registered. For all this motors two steps
         * on each direction are made. Those two steps are needed to be sure that the rotor is well aligned with the
         * stator.
         *
         * In fact, when a watch's movement is getting power, we can not be sure that all motors are in a correct
         * starting position. By doing those both steps on each direction, all motors will be synchronized with the
         * impulse that will excite the coil to the next step.
         *
         * @param   forward     Defines the direction of the inititliazation. `true` means clockwise.
         * @param   nbSteps     Number of steps to be applied on all motors for initialization. Default value: 1.
         * @param   autoStart   If set to `true`, the motor will start moving automatically. Default value is `false`.
         ************************************************************************************************************/
        void initializeAllMotors(bool forward, uint16_t nbSteps = 1);

        /********************************************************************************************************//**
         * @brief   Testing method for all motors going in the same direction for the given number of steps.
         *
         * This method is used for internal tests of the motors during production. With this method, all registered
         * motors will move in the given direction for the given number of steps. It also update the motor's position
         * if it's needed.
         *
         * @param   forward         Specify the direction of the move. `true` means clockwise move and `false` for
         *                          the other way.
         * @param   nbSteps         The number of steps to move...
         * @param   unpdatePosition If set to `true`, on each step saves the motor's driver saves the new position.
         *                          If set to `false`, the new position is not saved.
         ************************************************************************************************************/
        void acceleratedAgingSynchrone(bool forward,  uint16_t nbSteps, bool updatePosition);

        /********************************************************************************************************//**
         * @brief   Testing method for all motors going in an alternate direction for the given number of steps.
         *
         * This method is used for internal tests of the motors during production. With this method, all registered
         * motors will move for the given number of steps in an alternate direction according to the given one. It
         * also update the motor's position if it's needed.
         *
         * @param   forward         Specify the direction of the first registered motor. `true` means clockwise move
         *                          and `false` for the other way.
         * @param   nbSteps         The number of steps to move...
         * @param   unpdatePosition If set to `true`, on each step saves the motor's driver saves the new position.
         *                          If set to `false`, the new position is not saved.
         ************************************************************************************************************/
        void acceleratedAgingAsynchrone(bool forward, uint16_t nbSteps, bool updatePosition);

        /********************************************************************************************************//**
         * @brief   Implmentation of the xfReactive Interface's... Starts the behaviour, resp. the state machine.
         ************************************************************************************************************/
        void startBehaviour();

        /********************************************************************************************************//**
         * @brief   Implementation of the MotorDriverObserver Interface
         ************************************************************************************************************/
        void onStepsEnd(MotorDriver* mDriver);

    protected:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PROTECTED DECLARATION SECTION                                                                            */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Implements state machine behavior.
         ************************************************************************************************************/
        EventStatus processEvent();

        /********************************************************************************************************//**
         * @brief   Enumeration used to have a unique identifier for every state in the state machine.
         ************************************************************************************************************/
        typedef enum
        {
            ST_UNKOWN,                      ///< @brief Unkown state
            ST_INIT,                        ///< @brief Initial state
            ST_INIT_HOT,                    ///< @brief Initial state for Hot Reset
            ST_MOTOR_INIT_CW,               ///< @brief Initialize all motors clockwise
            ST_MOTOR_INIT_CCW,              ///< @brief Initialize all motors in the other direction
            ST_IDLE,                        ///< @brief No motor's move needed, just wait
            ST_CHECK,                       ///< @brief State checking if motor's steps are needed
            ST_MOVE,                        ///< @brief State doing motor's step
            ST_CHECK_INIT,                  ///< @brief State checking if motor's initi steps are needed
            ST_MOVE_INIT,                   ///< @brief State doing motor's init step
            ST_ACCEL_SYNC,                  ///< @brief Special state for AcceleratAgingSynchrone() function
            ST_ACCEL_ASYNC                  ///< @brief Special state for AcceleratAgingAsynchrone() function
        } eMainState;
        eMainState _currentState;           ///< @brief Attribute indicating currently active state of the state machine

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Attributes
        uint8_t                                 _motorIds;
        MotorDriver                             _mDriver;

        // Static event for motors (XF related)
        MotorEvent                              _motorEvent;
        MotorInitEvent                          _motorInitEvent;
        MotorEndEvent                           _motorEndEvent;

        // Methods
        void _checkStateAction(bool initMotor = false, bool calendarEvent = false); // Action to perform when `_currentState` goes in `ST_CHECK` or `ST_CHECK_INIT` state.
        void _autoStartMove(bool autostart, bool fromCalendar);
        void _notifyMotorMoveEnd();
        const char* _state_to_string(eMainState state = ST_UNKOWN);
    };

}  // namespace motor

#endif // #if (USE_MOTORS != 0)

/** \} */   // Group: Motor
