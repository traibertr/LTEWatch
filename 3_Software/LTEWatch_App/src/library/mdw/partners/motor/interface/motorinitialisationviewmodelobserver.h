/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thierry Hischier, Patrice Rudaz
 * All rights reserved.
 *
 * \file    motorinitialisationviewmodelobserver.h
 *
 * \addtogroup Motor
 * \{
 *
 * \class   MotorInitialisationViewModelObserver
 * \brief   Interface reacting on the MotorInitialisation-Controller
 *
 * Initial author: Thierry Hischier, Patrice Rudaz
 * Creation date: 2018-09-26
 *
 * \author  Patrice Rudaz (patrice.rudaz@hevs.ch)
 * \date    January 2019
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <stdint.h>

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
    class MotorInitialisationViewModelObserver
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        virtual void onActivated() = 0;
        virtual void onDeactivated() = 0;
        virtual void onMotorInitialisationStart() = 0;
        virtual void onMotorAnimationStart(uint8_t motorId) = 0;
        virtual void onMotorAnimationEnd(uint8_t motorId) = 0;
        virtual void onMotorStepAction(uint8_t motorId, bool continuesStep, bool clockWise) = 0;
        virtual void onMotorMoveAction(uint8_t motorId, uint8_t nbSteps, bool clockWise) = 0;
        virtual void onMotorChange(uint8_t motorId) = 0;
        virtual void onSetMotorStepsForFastInit(uint8_t motorId) = 0;
        virtual void onCancelMotorInit() = 0;
        virtual void onUpdate() = 0;
    
        virtual ~MotorInitialisationViewModelObserver() {}
    
    protected:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PROTECTED DECLARATION SECTION                                                                            */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        MotorInitialisationViewModelObserver() {}         // Interfaces are not instantiable.
            
    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Attributes
    
        // Methods
    };
    
} // namespace motor

/** \} */   // Group: Motor
