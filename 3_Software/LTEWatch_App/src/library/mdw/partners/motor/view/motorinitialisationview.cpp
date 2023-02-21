/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thierry Hischier & Patrice Rudaz
 * All rights reserved.
 * ******************************************************************************************************************/

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "motorinitialisationview.h"

#if (USE_MOTORS != 0) && (0)
#include "motor/motorid.h"
#include "actionid.h"


/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#define DEBUG_MOTOR_INIT_VIEW_ENABLE        0
#if (DEBUG_MOTOR_INIT_VIEW_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #include <tools/logtrace.h>
    #include <tools/config/logtrace-config.h>
    #define MINITV_ERROR(args...)           LOG_TRACE("MotInitV", LOG_LEVEL_ERROR, args)
    #define MINITV_DEBUG(args...)           LOG_TRACE("MotInitV", LOG_LEVEL_DEBUG, args)
#else
    #define MINITV_ERROR(...)               {(void) (0);}
    #define MINITV_DEBUG(...)               {(void) (0);}
#endif


/* **************************************************************************************************************** */
/* Namespace's Declaration                                                                                          */
/* **************************************************************************************************************** */
using motor::MotorInitialisationView;

IMPLEMENT_MEMORYPOOL_OPERATORS(MotorInitialisationView)


/* **************************************************************************************************************** */
/* CONSTRUCTOR                                                                                                      */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
MotorInitialisationView::MotorInitialisationView() : AppView(application::VT_MOTOR_INIT)
{}

// ------------------------------------------------------------------------------------------------------------------
MotorInitialisationView::~MotorInitialisationView()
{
    if (_viewModel)
    {
        _viewModel->delObserver(this);
    }
}


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationView::addObserver()
{
    if (_viewModel)
    {
        if (!_viewModel->addObserver(this))
        {
            MINITV_ERROR("addObserver() FAILED !\r\n");
        }
    }
}


/* **************************************************************************************************************** */
/* PROTECTED SECTION                                                                                                */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationView::onActivated()
{
    MINITV_DEBUG("Activated!\r\n");
    getMotorController().addObserver(this);
    onMotorInitialisationStart();
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationView::onDeactivated()
{
    MINITV_DEBUG("Deactivated!\r\n");
    getMotorController().delObserver(this);
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationView::onMotorInitialisationStart()
{
    MINITV_DEBUG("Starting initialisation, all motors should move to their `zero` position...\r\n");
    uint16_t destinations[NB_MAX_MOTORS];
    memset(destinations, 0x00, NB_MAX_MOTORS * sizeof(uint16_t));
    getMotorController().setDestinations(destinations, NB_MAX_MOTORS, true);
    getMotorController().raiseMotorCmdEvent();
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationView::onMotorAnimationStart(uint8_t motorId)
{
    MINITV_DEBUG("Animation starts for motorId: 0x%04x\r\n", motorId);
    getMotorController().setStepsForInitAnimation(motorId);
    getMotorController().raiseMotorInitEvent(static_cast<motor::MotorId::eMotorId>(motorId), true, true, true);
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationView::onMotorAnimationEnd(uint8_t motorId)
{
    MINITV_DEBUG("Animation stops for motorId: 0x%04x\r\n", motorId);
    getMotorController().raiseMotorInitEvent(static_cast<motor::MotorId::eMotorId>(motorId), false, true, true);
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationView::onMotorStepAction(uint8_t motorId, bool continuesStep, bool clockWise)
{
    MINITV_DEBUG("Step action for motorId: 0x%04x, continuesStep: %s, clockWise: %s\r\n", motorId, continuesStep ? "YES":"NO", clockWise ? "YES":"NO");
    if (continuesStep)
    {
        getMotorController().setStepsForFastInit(motorId);
    }
    getMotorController().raiseMotorInitEvent(static_cast<motor::MotorId::eMotorId>(motorId), clockWise, continuesStep);
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationView::onMotorMoveAction(uint8_t motorId, uint8_t nbSteps, bool clockWise)
{
    MINITV_DEBUG("Move action for motorId: 0x%04x, nbSteps: %d, clockwise: %s\r\n", motorId, nbSteps, clockWise ? "YES":"NO");
    getMotorController().setStepsForFastInit(motorId, nbSteps);
    getMotorController().raiseMotorInitEvent(static_cast<motor::MotorId::eMotorId>(motorId), clockWise, true);
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationView::onMotorChange(uint8_t motorId)
{
    MINITV_DEBUG("motor change for motorId: 0x%04x\r\n", motorId);
    getMotorController().setStepsForInitAnimation(motorId);
    getMotorController().raiseMotorInitEvent(static_cast<motor::MotorId::eMotorId>(motorId), true, true, true);
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationView::onSetMotorStepsForFastInit(uint8_t motorId)
{
    MINITV_DEBUG("Fast init for motorId: 0x%04x\r\n", motorId);
    getMotorController().setStepsForFastInit(motorId);
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationView::onCancelMotorInit()
{
    getMotorController().cancelMotorInit();
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationView::onUpdate()
{
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationView::onMotorMoveEnd(MotorController* mCtrl)
{
    notifyObservers(::ActionId::ACTION_MOTOR_MOVE_END);
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationView::notifyObservers(::ActionId::eActionId actionId)
{
    MINITV_DEBUG("_notifyObservers with %s!\r\n", ::ActionId::to_string(actionId));
    for (uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onViewActionChanged(actionId);
        }
    }
}


/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------

#endif // #if (USE_MOTORS != 0) && (0)
