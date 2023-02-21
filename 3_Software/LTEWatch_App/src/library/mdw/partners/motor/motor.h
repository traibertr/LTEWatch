/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * \file    motor.h
 *
 * \defgroup Motor
 * \{
 *
 * \class   motor::Motor
 * \brief   Class that manages a bidirectional stepper motor
 *
 * Soprod developed bidirectional stepper motors for high-end watch movements.
 *
 * The purpose of this class is to manage one of those bidirectional stepper motors.
 *
 * \image html ./images/motorCoilConf.png "Stepper Motor's Coil Configuration"
 *
 * According to the picture above, the motor has 2 coils, one is used to move CW (clockwise), and the other is used
 * to move CCW (counterclockwise). A, B and C are directly connected to GPIO of the processor.
 *   - When going CW using B1, C must be "in the air", i.e. coil B2 is open.
 *   - When going CCW using B2, A must be "in the air", i.e. coil B1 is open.
 *   - C is always driven
 *
 * \htmlonly <style>div.image img[src="522.png"]{width:280px;}</style> \endhtmlonly
 * \image html ./images/522.png "Caliber SOP915.03"
 *
 * This class handles all informations about one of these bidirectional stepper motors:
 *   - the number of steps required to make a full turn
 *   - the current position
 *   - the position where it has to go (destination)
 *   - the direction to take to reach the destination
 *   - Which impulse must be assign to the outputs to make the next step
 *
 * As the stepper motors are bidirectional and both coils are directly connected on GPIO, we can consider the three 
 * endpoints A, B and C (see picture above) as a parallel output of 3 bits. For each direction, we can switch A & B 
 * together from low to high and keep C alone... for example to go forward, we should toggle endpoints A and C, and 
 * to go backward it's B and C to be toggled. So one pulse is b110 (A-B-C) and the next pulse must be b001.
 * As the GPIO are 8bits port, we can save both of these pulse in an array of 2 `char`. At the initialization of this 
 * class and depending of the hardware configuration, this array of pulse is setup correctly.
 *
 * All these parameters will be explain in detail further.
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
#include "motor-config.h"

#include "motorid.h"
#include "motorpersistor.h"

//#include "flash/interface/persister.h"

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* APPLICATION defintion                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */

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
    class Motor
    #if (USE_FLASH != 0)
        : public flash::Persister
    #endif  // #if (USE_FLASH != 0)
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * \brief   Enumeration of the number of steps available for the Soprod motor.
         ************************************************************************************************************/
        typedef enum
        {
            zeroSteps       =   0,
            SixtySteps      =  60,
            OneTwentySteps  = 120,
            OneEightySteps  = 180,
            TwoFourtySteps  = 240,
            ThreeSixtySteps = 360
        } eNbSteps;

        /********************************************************************************************************//**
         * \brief   Initializes the components needed by this class
         *
         * This initialization method needs some parameters:
         *   - the motor's index to identify the motor,
         *   - the number of steps to make a complete turn,
         *   - the number of steps to a significant indication on the watch
         *   - and some more...
         *
         * This last parameter (`nbStepsForTick`) needs some more explanations. If we have a stepper motor with 180
         * steps and used to display the second on a watch, we have only 60 positions that are significant to display
         * the second. Therefore for each second, the motor has to do three steps and therefore `nbStepsForTick`
         * will have the value of 3.
         *
         * \param   hotReset            To know if a `reset` has occured at runtime...
         * \param   id                  The identifier of the motor to control
         * \param   nbSteps             Number of steps to complete a full rotation, default value is 180
         * \param   nbStepsForTick      The number of steps necessary to give significant information, the default
         *                              value is 1.
         * \param   coilA               The PIN number assign to the connector A of the motor's coils.
         * \param   coilB               The PIN number assign to the connector B of the motor's coils.
         * \param   coilC               The PIN number assign to the connector C of the motor's coils.
         * \param   inverseRotation     This parameter defines the default direction of motor rotation.
         * \param   nbStepsForFastInit  Defines how many steps should be done in fast initialisation mode and
         *                              during the animation when switching motors.
         *
         * \warning Must be called before any other method of this class
         * \note    Requires pre-existing instances of classes listed in parameter list
         ************************************************************************************************************/
        void initialize(bool              hotReset,
                        MotorId::eMotorId id                 = MotorId::UnknownMotor,
                        Motor::eNbSteps   nbSteps            = Motor::OneEightySteps,
                        uint8_t           nbStepsForTick     = 1,
                        uint8_t           coilA              = 0,
                        uint8_t           coilB              = 1,
                        uint8_t           coilC              = 2,
                        bool              inverseRotation    = false,
                        uint8_t           nbStepsForFastInit = 15);
                        

        /********************************************************************************************************//**
         * \brief   Gives the motor's ID
         *
         * This method retrieves the value of the identifier of the motor. This value MUST be unique
         * and contained in the enumerate values `tMotorIdType`
         *
         * \return  The identifier of the motor
         ************************************************************************************************************/
        inline uint8_t id() const                       { return _id; }

        /********************************************************************************************************//**
         * \brief   Gives the motor's index
         *
         * This method retrieves the value of the index of the motor. This value MUST be unique
         * and is a positive number from to 0 to the number of registered motors in the Watch caliber.
         *
         * \return  The index of the motor
         ************************************************************************************************************/
        inline uint8_t motorPositionIndex() const       { return _motorPosIndex; }

        /********************************************************************************************************//**
         * \brief   Gives the current position.
         *
         * This method retrieves the current position of the motor. This value can not exceed the
         * `nbSteps` value.
         *
         * \return  The current position of the motor
         ************************************************************************************************************/
        inline uint16_t position() const                { return _persistentParams.position(); }

        /********************************************************************************************************//**
         * \brief   Gives the destination.
         *
         * This method retrieves the next destination of the current motor. This value can not exceed
         * the `nbSteps` value.
         *
         * \return  The next position of the current motor
         ************************************************************************************************************/
        inline uint16_t destination() const             { return _destination; }

        /********************************************************************************************************//**
         * \brief   Gives the number of steps of the stepper motor
         *
         * This method retrieves the number of steps required to do a complete turn.
         *
         * \return  The number of steps of this motor to complete one turn
         ************************************************************************************************************/
        inline uint16_t nbSteps() const                 { return _nbSteps; }

        /********************************************************************************************************//**
         * \brief   Gives the number of steps of the stepper motor
         *
         * This method retrieves the number of steps required to do a half turn.
         *
         * \return  The number of steps of this motor to complete a half turn
         ************************************************************************************************************/
        inline uint16_t nbStepsHalf() const             { return _nbStepsHalf; }

        /********************************************************************************************************//**
         * \brief   Gives the number of steps of the stepper motor
         *
         * This method retrieves the number of steps required to do a ond and a half turn.
         *
         * \return  The number of steps of this motor to complete one and a half turn
         ************************************************************************************************************/
        inline uint16_t nbStepsThreeHalf() const        { return _nbStepsThreeHalf; }

        /********************************************************************************************************//**
         * \brief   Gives the number of steps necessary to give significant information
         *
         * This method retrieves the number of steps necessary to give significant information.
         *
         * \return  The number of steps necessary to give significant information
         ************************************************************************************************************/
        inline uint8_t nbStepsForTick() const           { return _nbStepsForTick; }

        /********************************************************************************************************//**
         * \brief   Retrieves the number of steps for fast initialization.
         *
         * This method retrieves the number of steps used during the motor's initlialization at high
         * speed. At that speed more than one steps are applied on the motor to reach the initial
         * position.
         *
         * \return  The number of steps for high speed motor's initialization
         ************************************************************************************************************/
        inline uint16_t nbStepsForFastInit() const      { return _nbStepsForFastInit; }

        /********************************************************************************************************//**
         * \brief   Retrieves the number of steps for animation when switching motor in initialization.
         *
         * This method retrieves the number of steps used for the animation during the motor's initlialization when
         * switching between motors. During this animation, more than one steps are applied on the motor on each
         * direction.
         *
         * \return  The number of steps for animation during motor's initialization
         ************************************************************************************************************/
        inline uint16_t nbStepsForAnimation() const     { return _nbStepsForAnimation; }

        /********************************************************************************************************//**
         * \brief   Gives the direction of the motor.
         *
         * This method retrieves the direction expected for the next step of the motor. It returns
         * `TRUE` if the next step is clockwise. If we should go back (counterclockwise), it will
         * return `FALSE`.
         *
         * \return  `TRUE` if the direction expected for the next step is clockwise, `FALSE` otherwise
         ************************************************************************************************************/
        inline bool isGoingForward() const              { return _clockwise; }

        /********************************************************************************************************//**
         * \brief   Gives the coilA PIN number.
         *
         * This method retrieves the number of the PIN assign to the point A of the motor's coils.
         *
         * \return  The PIN number of the point A of the motor's coils.
         ************************************************************************************************************/
        inline uint8_t coilA() const                    { return _coilA; }

        /********************************************************************************************************//**
         * \brief   Gives the coilB PIN number.
         *
         * This method retrieves the number of the PIN assign to the point B of the motor's coils.
         *
         * \return  The PIN number of the point B of the motor's coils.
         ************************************************************************************************************/
        inline uint8_t coilB() const                    { return _coilB; }

        /********************************************************************************************************//**
         * \brief   Gives the coilC PIN number.
         *
         * This method retrieves the number of the PIN assign to the point C of the motor's coils.
         *
         * \return  The PIN number of the point C of the motor's coils.
         ************************************************************************************************************/
        inline uint8_t coilC() const                    { return _coilC; }

        /********************************************************************************************************//**
         * \brief   Retrieves the three coils of the motor.
         *
         * This method retrieves the 3 values which are the pin numbers identifying the GPIOs to be used to
         * drive the coils of the active motor. According to the `Common Coil` configuration (C or B), it retrieves
         * the pin number of GPIO of coil A, C and B or A, B and C.
         *
         * \return  The array of PIN number of the points A, B or C.
         ************************************************************************************************************/
        inline uint8_t* activeCoilEndPoints()          { return _activeCoilEndPoint; }

        /********************************************************************************************************//**
         * \brief   Retrieves the signal to be applied on coils to move the motor in the selected direction.
         *
         * This method retrieves the array of 3 boolean values which are the pulse wave to be used to drive the 
         * coils of the active motor in the desired direction.
         *
         * \return  The pulse of the motor's coils
         ************************************************************************************************************/
        inline const uint8_t* activePulse()
        {
            return _PULSE[(_clockwise && !_inverseRotation) || (!_clockwise && _inverseRotation)]
                   [_persistentParams.alim()];
        }

        /********************************************************************************************************//**
         * \brief   Retrieve the value of `_stepThreshold` private parameter.
         *
         * Watch movement don't always use motors with the same step's number. To give the impression
         * that all motors are rotating simultaneously, the motors with the less of motor's steps
         * should not make a step at each turn... This is store int the `_stepThreshold` parameter.
         * This method retrieves the value for this parameter.
         ************************************************************************************************************/
        inline uint8_t stepThreshold() const            { return _stepThreshold; }

        /********************************************************************************************************//**
         * \brief   Retrieves the reversed confivguration of the motor
         *
         * This method retrieves `TRUE` if the motor is configured to rotate counter clockwise to
         * go forward, due to mechanicla construction and `FALSE` otherwise.
         *
         * \return  The rotation directions inversion due to machanical construction
         ************************************************************************************************************/
        inline bool  isReversed() const                 { return _inverseRotation; }

        /********************************************************************************************************//**
         * \brief   Returns an uint32_t bitfield containing the GPIOs used for the motor's coils.
         *
         * \return  The mask of the GPIOs used to drive the coils of the current motor.
         ************************************************************************************************************/
        inline uint64_t motorGpioMask() const           { return _motorGpioMask; }

        /********************************************************************************************************//**
         * \brief   Returns the delay to wait between two steps of the current motor when it is alone to move.
         *
         * Returns the delay to wait between two steps of the current motor when it is alone to move. The value is
         * already converted in ticks of 32kHz.
         *
         * \return  The number of ticks to wait between two single motor's steps.
         ************************************************************************************************************/
        inline uint32_t singleGapTick() const           { return _singleGapTick; }

        /********************************************************************************************************//**
         * \brief   Returns the number of steps the motor has done during is life...
         ************************************************************************************************************/
        inline uint32_t stepCounter() const             { return _persistentParams.stepCounter(); }

        /********************************************************************************************************//**
         * \brief   Returns wether the direction of the specific motor is forced (-1 for ccw ,1 for cw) or not (0)
         ************************************************************************************************************/
        inline int8_t forceDirection() const            { return _forceDirection; }

        /********************************************************************************************************//**
         * \brief   Set the destination to the actual position
         ************************************************************************************************************/
        inline void resetDestination()                  { _destination = _persistentParams.position(); }
        
        /********************************************************************************************************//**
         * \return  Returns the Motor's identifier to string... For DEBUG purpose
         ************************************************************************************************************/
        inline const char* toString() const             { return MotorId::toString(MotorId::fromInt(_id)); }

        
        /********************************************************************************************************//**
         * \brief   Set the position of the motor
         *
         * This method checks that the given position is in the motor step's range and store it in the persistent 
         * memory. 
         *
         * \param   position. This is the new position to be stored as the actual one.
         ************************************************************************************************************/
        void setPosition(uint16_t position);
        
        /********************************************************************************************************//**
         * \brief   Set the destination of the motor
         *
         * This method computes the next destination. The new given position can be any value, evennegative. This
         * method supports all possible values ??and converts them to a destination between 0 and `nbSteps`.
         *
         * \param   destination This is the goal the motor has to reach.
         ************************************************************************************************************/
        void setDestination(uint16_t destination);

        /********************************************************************************************************//**
         * \brief   Changes the configuration of the direction according to given parameter
         ************************************************************************************************************/
        void setDirectionConfiguration(bool clockwise);

        /********************************************************************************************************//**
         * \brief   Set the direction for the next step.
         *
         * This method update the direction for the next step according the `forward` parameter. A `TRUE` given as
         * parameter means that the next step will be clockwise. For a counterclockwise step, `FALSE` should be
         * passed.
         *
         * \param   clockwise `TRUE` to go clockwise and `FALSE` otherwise.
         ************************************************************************************************************/
        void setDirection(bool clockwise);

        /********************************************************************************************************//**
         * \brief   set the forceDirection parameter of the motor for the next destination
         *
         * This method update the direction behaviour of the motor. It will then be fetched by MotorDriver to know
         * what to call \ref setDirection(bool clockwise) with.
         *
         * \param   forceDirection `-1` to go clockwise and `1` otherwise. `0` means the direction to choose is the
         *                         closest
         ************************************************************************************************************/
        void setForceDirection(int8_t forceDirection);

        /********************************************************************************************************//**
         * \brief   Set the number of steps necessary to give significant information
         *
         * \param   nbStepsForTick The new value to be recorded.
         ************************************************************************************************************/
        void setNbStepsForTick(uint8_t nbStepsForTick);

        /********************************************************************************************************//**
         * \brief   Setup the index used for this motor to update the buffer of motor's position when
         *          using the `Motor Position` characterisitic.
         *
         * \param   index Reference to the motor's index of the motor driver array...
         ************************************************************************************************************/
        void setMotorPositionIndex(uint8_t index);

        /********************************************************************************************************//**
         * \brief   FIXME : Add some comments
         ************************************************************************************************************/
        void setMotorSteps(uint16_t steps);

        /********************************************************************************************************//**
         * \brief   FIXME : Add some comments
         ************************************************************************************************************/
        void setMotorStepsForFastInit(uint16_t stepsForFastInit);

        /********************************************************************************************************//**
         * \brief   Set the new value for `_stepThreshold` private parameter.
         *
         * Watch movement don't always use motors with the same step's number. To give the impression that all motors
         * are rotating simultaneously, the motors with the less of motor's steps should not make a step at each
         * turn... This is store int the `_stepThreshold` parameter.
         *
         * This method sets a new value for this parameter.
         ************************************************************************************************************/
        void setStepThreshold(uint8_t stepThreshold);


        /********************************************************************************************************//**
         * \brief   Toggle the state of the outputs
         *
         * This method is used to switch the state (high or low) of the outputs connected to the coil
         * of the current motor.
         ************************************************************************************************************/
        inline void toggleAlim()                        { _persistentParams.toggleAlim(); }

        /********************************************************************************************************//**
         * \brief   Increase the destination of `nbStepsForTick`.
         *
         * Increase the destination of `nbStepsForTick` to reach the next significant position. As reminder,
         * `nbStepsForTick` represents the number of steps to do to reach a position with any interest... For a
         * stepper motor of 180 steps used to display seconds, significant positions are reached every three steps.
         ************************************************************************************************************/
        void stepForward();

        /********************************************************************************************************//**
         * \brief   Move the motor of one step forward.
         ************************************************************************************************************/
        void singleStepForward();

        /********************************************************************************************************//**
         * \brief   Decrease the destination of `nbStepsForTick`.
         *
         * Decrease the destination of `nbStepsForTick` to reach the previous significant position. As reminder,
         * `nbStepsForTick` represents the number of steps to do to reach a position with any interest... For a
         * stepper motor of 180 steps used to display seconds, significant positions are reached every three steps.
         ************************************************************************************************************/
        void stepBackward();

        /********************************************************************************************************//**
         * \brief   Move the motor of one step forward.
         ************************************************************************************************************/
        void singleStepBackward();

        /********************************************************************************************************//**
         * \brief   Update the number of steps to do for the default animation.
         *
         * This method let change the number of steps to be done in both direction during the default animation.
         * According to the state of the Synchronous Mode, this number should adjusted
         *
         * \param   nbSteps     The new number of steps to be used.
         ************************************************************************************************************/
        void updateNbStepsForAnimation(bool synchronousModeEnabled, uint8_t factor);

        /********************************************************************************************************//**
         * \brief   Update the current position of the motor
         *
         * This method update the current position after a completed step. It takes into account the direction and
         * maximum number of steps `nbSteps`.
         ************************************************************************************************************/
        void updatePosition();

        /********************************************************************************************************//**
         * \brief   Returns the delay to wait between two steps of the current motor when it is alone to move.
         *
         * Returns the delay to wait between two steps of the current motor when it is alone to move. The value is
         * already converted in ticks of 32kHz.
         *
         * \return  The number of ticks to wait between two single motor's steps.
         ************************************************************************************************************/
        void setSingleGapTick(uint32_t gapTick);

    #if (USE_FLASH != 0)
        /********************************************************************************************************//**
         * \name   FlashPersiterInheritance
         * \brief  Inherit from FlashPersiter.
         * \{
         ************************************************************************************************************/
        virtual uint32_t* save(uint16_t* size);
        virtual void restore(uint32_t* data);
        /** \} */
#endif  // #if (USE_FLASH != 0)

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Attributes

        // 3 dimensional array to store the pulse to be applied on the coils. 1st dimension is the direction (CW or
        // CCW), 2nd dimension is the alimentation of the coils (drive by _alim) and 3rd is the state of the 3 GPIOs
        // used to drive the coils.
        static const uint8_t _PULSE[2][2][3];

        // Persistent parameters
        motor::MotorPersistor   _persistentParams;

        // to store the GPIO assign to CoilA, CoilB and CoilC
        uint8_t                 _coilA;
        uint8_t                 _coilB;
        uint8_t                 _coilC;
        uint8_t                 _activeCoilEndPoint[3];

        // to know which motor to work with
        uint8_t                 _id;

        // to know the index of the motor we are working with
        uint8_t                 _motorPosIndex;

        // The next position
        uint16_t                _destination;

        // Number of steps of the stepper motor
        uint16_t                _nbSteps;
        uint16_t                _nbStepsHalf;
        uint16_t                _nbStepsThreeHalf;

        // steps to do to reach the next main position
        uint8_t                 _nbStepsForTick;

        // Watch movement don't always use motors with the same step's number. To give the impression that all motors
        // are rotating simultaneously, the motors with the less of motor's steps should not make a step at each
        // turn... This is store int the variable below
        uint8_t                 _stepThreshold;

        // steps to do when Motor initalisation mode is enabled
        uint16_t                _nbStepsForFastInit;

        // steps to do during animation in initialisation mode
        uint16_t                _nbStepsForAnimation;

        // Mask of the GPIOs used for driving the motor's coils.
        uint64_t                _motorGpioMask;

        // Tick counter for the gap time to wait between two motor's steps when only one motor is moving
        uint32_t                _singleGapTick;

        // Keep the direction of the motor's rotation. TRUE means clockwise and FALSE to rotate counter-clockwise
        bool                    _clockwise;
        int8_t                  _forceDirection;

        // Due to mechanical construction, some motors rotate CCW with a CW impulse... To take care of this fact,
        // we've introduced this variable;
        bool                    _inverseRotation;
        bool                    _clockwiseConfiguration;
    };

}   // Namespace motor

#endif // #if (USE_MOTORS != 0)

/** \} */   // Group: Motor
