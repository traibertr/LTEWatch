/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thierry Hischier, Patrice Rudaz
 * All rights reserved.
 *
 * \file    motorinitialisationviewmodel.h
 *
 * \addtogroup Motor
 * \{
 *
 * \class   MotorInitialisationViewModel
 * \brief   Class which contains all data for the MotorInitialisation-Feature
 *
 * Initial author: Thierry Hischier, Patrice Rudaz
 * Creation date: 2018-09-26
 *
 * \author  Patrice Rudaz
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
#include "motor/config/motor-config.h"
#include "motor/interface/motorinitialisationviewmodelobserver.h"

#include "viewmodel/appviewmodel.h"
#include "core/isubject.hpp"


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
    class MotorInitialisationViewModel : 
        public application::AppViewModel, 
        public ISubject<MotorInitialisationViewModelObserver, MOTOR_INIT_VIEW_MODEL_MAX_OBSERVERS> 
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* CONSTRUCTOR SECTION                                                                                      */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        MotorInitialisationViewModel();
        virtual ~MotorInitialisationViewModel();

        /********************************************************************************************************/
        /*                                                                                                      */
        /*                                       PUBLIC DECLARATION SECTION                                     */
        /*                                                                                                      */
        /********************************************************************************************************/
        void initialize();
    
        // ViewModel Interface
        void onActivated();
        void onDeactivated();
        
        void motorInitialisationStart();
        void motorAnimationStart(uint8_t motorId);
        void motorAnimationEnd(uint8_t motorId);
        void motorStepAction(uint8_t motorId, bool continuesStep, bool clockWise = true);
        void motorMoveAction(uint8_t motorId, uint8_t nbSteps, bool clockWise = true);
        void motorChange(uint8_t motorId);
        void setMotorStepsForFastInit(uint8_t motorId);
        void cancelMotorInit();
        void update();
        
    private:
        /********************************************************************************************************/
        /*                                                                                                      */
        /*                                       PRIVATE DECLARATION SECTION                                    */
        /*                                                                                                      */
        /********************************************************************************************************/
        //Attributes
        uint8_t _currentIndex;
    
        // Methods
    };

} // namespace application

#endif // #if (USE_MOTORS != 0) && (0)
/** \} */   // Group: Motor
