/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thierry Hischier, Patrice Rudaz
 * All rights reserved.
 *
 * Initial author: Thierry Hischier
 * Creation date: 2018-03-23
 *
 * \file    motorinitialisation.h
 *
 * \defgroup Motor
 * \{
 *
 * \class   MotorInitialisation
 * \brief   Class which handles the initialisation of the different motors
 *
 * # MotorInitialisation
 *
 * This class handles all actions on the MotorInitialisation class. It allows to initialise the different
 * motors due moving to a given position (mostly 00:00:00)
 *
 * \author  Patrice Rudaz (patrice.rudaz@hevs.ch)
 * \date    December 2019
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <platform-config.h>

#if (USE_MOTORS != 0) && (0)
#include "xf-config.h"
#include "isubject.hpp"
#include "xfreactive.h"

#include "appfunction.h"

#include "motor-config.h"
#include "motorcontroller.h"
#include "motorinitialisationviewmodel.h"


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* APPLICATION definition                                                                                           */
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
    class MotorInitialisationController : public XFReactive
        , public AppFunction
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* CONSTRUCTOR SECTION                                                                                      */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        MotorInitialisationController();
        virtual ~MotorInitialisationController() {}

        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * \brief   Enumeration used to have a unique identifier for every button action.
         ************************************************************************************************************/
        enum buttonActions
        {
            MOTOR_INITIALISATION_SINGLE_STEP = 1, 
            MOTOR_INITIALISATION_CONTINUES_STEPS,
            MOTOR_INITIALISATION_CONTINUES_STOP,
            MOTOR_INITIALISATION_CHANGE_MOTOR
        };
        
        void initialize();
        void bind(MotorController* motorController,
                  MotorInitialisationViewModel* viewModel
                 );

        void handle(FunctionId id, const FunctionProviderContext& context);
        void activate(AppFunctionContext& context);
        void deactivate(AppFunctionContext& context);

    protected:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PROTECTED DECLARATION SECTION                                                                            */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * \brief   Enumeration used to have a unique identifier for every state in the state machine.
         ************************************************************************************************************/
        typedef enum
        {
            ST_MOTORINITIALISATION_UNKOWN = 0,
            ST_MOTORINITIALISATION_INIT,
            ST_MOTORINITIALISATION_IDLE,
            ST_MOTORINITIALISATION_VIEW_READY_WAIT,
            ST_MOTORINITIALISATION_INIT_MOTOR_WAIT,
            ST_MOTORINITIALISATION_INIT_MOTOR,
            ST_MOTORINITIALISATION_INIT_MOTOR_CONFIRM,
            ST_MOTORINITIALISATION_INIT_MOTOR_STEP,
            ST_MOTORINITIALISATION_INIT_MOTOR_CHANGE,
            ST_MOTORINITIALISATION_INIT_MOTOR_CHANGE_END,
            ST_MOTORINITIALISATION_TOP_ENUM
        } eState;

        
        /********************************************************************************************************//**
         * \brief   Timeout identifier(s) for this state machine
         ************************************************************************************************************/
        typedef enum
        {
            TimeoutAnimationId  = 1,
            TimeoutActivationId, 
            TimeoutTopEnumId
        } eTimeoutId;

        /********************************************************************************************************//**
         * \brief   Implements state machine behavior.
         ************************************************************************************************************/
        EventStatus processEvent();

        /********************************************************************************************************//**
         * \brief   Implements Function View Interface.
         ************************************************************************************************************/
        void onViewActionChanged(::ActionId::eActionId actionId);

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Attributes
        MotorController*                        _motorController;

        eState                                  _currentState;
        bool                                    _isTimerAnimationEnabled;
        bool                                    _isTimerActivationEnabled;
        bool                                    _clockWise;
        uint8_t                                 _currentMotorId;
        bool                                    _fastMotorInit;

        // Methods  
        void                                    _startTimeoutAnimation();
        void                                    _onMotorMoveEnd();

        // DEBUG
        const char*                             _currentState_to_string();
        const char*                             _timeoutId_to_string(eTimeoutId timeoutId);
    };

} // namespace motorinitialisation

#endif // #if (USE_MOTORS != 0)

/** \} */   // Group: Motor
