/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thierry Hischier & Patrice Rudaz
 * All rights reserved.
 * ******************************************************************************************************************/

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "motorinitialisationviewmodel.h"

#if (USE_MOTORS != 0) && (0)
#include "view/activeviewcontroller.h"


/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#define DEBUG_MOTORINITVIEWMODEL_ENABLE     0
#if (DEBUG_MOTORINITVIEWMODEL_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #include <tools/logtrace.h>
    #include <tools/config/logtrace-config.h>
    #define MVM_DEBUG(args...)              LOG_TRACE("MotorVM", LOG_LEVEL_DEBUG, args)
#else
    #define MVM_DEBUG(...)                  {(void) (0);}
#endif


/* **************************************************************************************************************** */
/* Namespace Declaration                                                                                            */
/* **************************************************************************************************************** */
using motor::MotorInitialisationViewModel;


/* **************************************************************************************************************** */
/* CONSTRUCTOR                                                                                                      */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
MotorInitialisationViewModel::MotorInitialisationViewModel() : _currentIndex(0)
{
}

// ------------------------------------------------------------------------------------------------------------------
MotorInitialisationViewModel::~MotorInitialisationViewModel()
{
}

/* **************************************************************************************************************** */
/* PUBLIC METHODS                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationViewModel::initialize()
{
    // Subscriptions are made in onActivated() method!    
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationViewModel::onActivated()
{
    MVM_DEBUG("onActivated\r\n");
    
    // Tell view controller which view to activate
    for (uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onActivated();
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationViewModel::onDeactivated()
{
    MVM_DEBUG("onDeactivated\r\n");

    // Tell view controller which view to deactivate
    for (uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onDeactivated();
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationViewModel::motorInitialisationStart()
{
    for(uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onMotorInitialisationStart();
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationViewModel::motorAnimationStart(uint8_t motorId)
{
    for(uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onMotorAnimationStart(motorId);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationViewModel::motorAnimationEnd(uint8_t motorId)
{
    for(uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onMotorAnimationEnd(motorId);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationViewModel::motorStepAction(uint8_t motorId, bool continuesStep, bool clockWise)
{
    for(uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onMotorStepAction(motorId, continuesStep, clockWise);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationViewModel::motorMoveAction(uint8_t motorId, uint8_t nbSteps, bool clockWise)
{
    for(uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onMotorMoveAction(motorId, nbSteps, clockWise);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationViewModel::motorChange(uint8_t motorId)
{
    for(uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onMotorChange(motorId);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationViewModel::setMotorStepsForFastInit(uint8_t motorId)
{
    for(uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onSetMotorStepsForFastInit(motorId);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationViewModel::cancelMotorInit()
{
    for(uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onCancelMotorInit();
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationViewModel::update()
{
    for(uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onUpdate();
        }
    }
}

#endif // #if (USE_MOTORS != 0) && (0)
