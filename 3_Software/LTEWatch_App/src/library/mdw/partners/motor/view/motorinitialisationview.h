/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thierry Hischier, Patrice Rudaz
 * All rights reserved.
 *
 * \file    motorinitialisationviewmodel.h
 *
 * @addtogroup Motor
 * \{
 *
 * @class   MotorInitialisationView
 * @brief   Class updating the view depending on its resources
 *
 * Initial author: Thierry Hischier, Patrice Rudaz
 * Creation date: 2018-09-26
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
#include "core/isubject.hpp"

#include "motor/interface/motorinitialisationviewmodelobserver.h"
#include "motor/motorcontroller.h"
#include "motor/viewmodel/motorinitialisationviewmodel.h"

#include "pool/memorypoolmacros.h"

#include "view/appview.h"
#include "actionid.h"

#if (USE_TONE_MANAGER != 0)
    #include "tonemanager.h"
#endif


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
    class MotorInitialisationController;
    class MotorInitialisationView :
        public application::AppView<MotorInitialisationViewModel>,
        public MotorCtrlObserver,
        public MotorInitialisationViewModelObserver
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* CONSTRUCTOR SECTION                                                                                      */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        MotorInitialisationView();
        virtual ~MotorInitialisationView();

        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        void addObserver();
        DECLARE_MEMORYPOOL_OPERATORS(MotorInitialisationView)

    protected:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PROTECTED DECLARATION SECTION                                                                            */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Attributes

        // Methods
        void onActivated();
        void onDeactivated();
        void onMotorInitialisationStart();
        void onMotorAnimationStart(uint8_t motorId);
        void onMotorAnimationEnd(uint8_t motorId);
        void onMotorStepAction(uint8_t motorId, bool continuesStep, bool clockWise = true);
        void onMotorMoveAction(uint8_t motorId, uint8_t nbSteps, bool clockWise = true);
        void onMotorChange(uint8_t motorId);
        void onSetMotorStepsForFastInit(uint8_t motorId);
        void onCancelMotorInit();
        void onUpdate();

        /********************************************************************************************************//**
         * @brief   Implements Motor Controller Interface.
         ************************************************************************************************************/
        void onMotorMoveEnd(MotorController* mCtrl);

        inline motor::MotorController &getMotorController() { ASSERT(_pMotorController); return *_pMotorController; }

        #if (USE_TONE_MANAGER != 0)
            inline tone::ToneManager &getToneManager() { ASSERT(_pToneManager); return *_pToneManager; }
        #endif

        void notifyObservers(::ActionId::eActionId actionId);

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Attributes

        // Methods
    };
} // namespace watch

#endif // #if (USE_MOTORS != 0) && (0)

/** \} */   // Group: Motor
