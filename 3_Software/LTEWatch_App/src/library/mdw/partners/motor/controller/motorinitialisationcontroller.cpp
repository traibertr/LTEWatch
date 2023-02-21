/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thierry Hischier & Patrice Rudaz
 * All rights reserved.
 * ******************************************************************************************************************/

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "motorinitialisationcontroller.h"

#if (USE_MOTORS != 0) && (0)
#include <app_timer.h>
#include "actionid.h"
    
    #if (SSWS_SUPPORT != 0)
        #include "ble/services/soprod/ssws/sswsdefinition.h"
        #include "ble/services/soprod/ssws/sswsservice.h"
    #endif

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#define DEBUG_MOTORINITIALISATION_ENABLE    0
#if (DEBUG_MOTORINITIALISATION_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(m_init_ctlr, LOG_LEVEL_DBG);
    #define MTI_DEBUG(args...)              LOG_DBG(args) 
    #define MTI_ERROR(args...)              LOG_ERR(args) 
    #define MTI_INFO(args...)               LOG_INF(args) 
    #define MTI_VERBOSE(args...)            LOG_DBG(args) 
    #define MTI_WARN(args...)               LOG_WRN(args) 
#else
    #define MTI_DEBUG(...)                  {(void) (0);}
    #define MTI_ERROR(...)                  {(void) (0);}
    #define MTI_INFO(...)                   {(void) (0);}
    #define MTI_VERBOSE(...)                {(void) (0);}
    #define MTI_WARN(...)                   {(void) (0);}
#endif

#define VIEWMODEL_NAME MotorInitialisationViewModel
#define VIEWMODEL ((VIEWMODEL_NAME*) _viewModel)

/* **************************************************************************************************************** */
/* Namespace's Declaration                                                                                          */
/* **************************************************************************************************************** */
using motor::MotorInitialisationController;


/* **************************************************************************************************************** */
/* Declaration section                                                                                              */
/* **************************************************************************************************************** */


/* **************************************************************************************************************** */
/* CONSTRUCTOR                                                                                                      */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
MotorInitialisationController::MotorInitialisationController() : 
    AppFunction(::FunctionId::FUNCTION_MOTORINITIALISATION),
    _currentState(ST_MOTORINITIALISATION_UNKOWN), 
    _isTimerAnimationEnabled(false),
    _isTimerActivationEnabled(false)
{
    #if (HSS_SUPPORT != 0) && defined(PERIPHERAL)
    {
        _bleDecoder = &com::ble::BleDecoder::getInstance();
        if (_bleDecoder == NULL) 
        {
            MTI_ERROR("Soprod BLE Decoder is NULL!\r\n");
            ASSERT(false);
        }
    }
    #endif // #if (HSS_SUPPORT != 0) && defined(PERIPHERAL)
}


/* **************************************************************************************************************** */
/* PUBLIC METHODS                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationController::initialize()
{
    _currentState           = ST_MOTORINITIALISATION_INIT;

    _motorController        = NULL;
    _deviceController       = NULL;

    _clockWise              = true;
    _currentMotorId         = motor::MotorId::Motor1;
    _fastMotorInit          = false;
    MTI_DEBUG("Initialise done...\r\n");
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationController::bind(
    MotorController* motorController,
    MotorInitialisationViewModel* viewModel
)
{
    if (viewModel == NULL)
    {
        MTI_DEBUG("viewModel not available \r\n");
        ASSERT(false);
    }
    _viewModel              = viewModel;
    
    if (motorController == NULL)
    {
        MTI_DEBUG("motorController not available \r\n");
        ASSERT(false);
    }
    _motorController        = motorController;
}

//------------------------------------------------------------------------------------------------------------
void MotorInitialisationController::handle(FunctionId id, const FunctionProviderContext& context)
{
    if (!_isReady)
        return;

    switch(id)
    {
        case MOTOR_INITIALISATION_SINGLE_STEP:
        {
            MTI_DEBUG("handle() > MOTOR_INITIALISATION_SINGLE_STEP, _currentState: %d...\r\n", _currentState);
            VIEWMODEL->cancelMotorInit();
            if (_currentState == ST_MOTORINITIALISATION_INIT_MOTOR || 
               _currentState == ST_MOTORINITIALISATION_INIT_MOTOR_CONFIRM)
            {
                _currentState = ST_MOTORINITIALISATION_INIT_MOTOR_STEP;
                MTI_DEBUG("_currentState -> ST_MOTORINITIALISATION_INIT_MOTOR_STEP...\r\n");
                VIEWMODEL->motorStepAction(_currentMotorId, false);
            }
            break;
        }
        case MOTOR_INITIALISATION_CONTINUES_STEPS:
        {
            MTI_DEBUG("handle() > MOTOR_INITIALISATION_CONTINUES_STEPS, _currentState: %d...\r\n", _currentState);
            if (_currentState == ST_MOTORINITIALISATION_INIT_MOTOR || 
               _currentState == ST_MOTORINITIALISATION_INIT_MOTOR_CONFIRM)
            {
                _currentState = ST_MOTORINITIALISATION_INIT_MOTOR_STEP;
                MTI_DEBUG("_currentState -> ST_MOTORINITIALISATION_INIT_MOTOR_STEP...\r\n");
                VIEWMODEL->motorStepAction(_currentMotorId, true);
            }
            break;
        }
        case MOTOR_INITIALISATION_CONTINUES_STOP:
        {
            MTI_DEBUG("handle() > MOTOR_INITIALISATION_CONTINUES_STOP, _currentState: %d...\r\n", _currentState);
            VIEWMODEL->cancelMotorInit();
            _currentState = ST_MOTORINITIALISATION_INIT_MOTOR;
            MTI_DEBUG("_currentState -> ST_MOTORINITIALISATION_INIT_MOTOR...\r\n");
            break;
        }
        case MOTOR_INITIALISATION_CHANGE_MOTOR:
        {
            MTI_DEBUG("handle() > MOTOR_INITIALISATION_CHANGE_MOTOR, _currentState: %d...\r\n", _currentState);
            if (_currentState == ST_MOTORINITIALISATION_INIT_MOTOR || 
               _currentState == ST_MOTORINITIALISATION_INIT_MOTOR_CONFIRM)
            {
                _currentMotorId = static_cast<uint8_t>(_motorController->nextMotorId(_currentMotorId));
                _currentState = ST_MOTORINITIALISATION_INIT_MOTOR_CHANGE;
                MTI_DEBUG("_currentState -> ST_MOTORINITIALISATION_INIT_MOTOR_CHANGE...\r\n");
                VIEWMODEL->motorChange(_currentMotorId);
            }
            break;
        }
        default:
        {
            MTI_WARN("State %d is not handled !\r\n", id);
            break;
        }
    }
}
//------------------------------------------------------------------------------------------------------------
void MotorInitialisationController::activate(AppFunctionContext& context)
{
    AppFunction::activate(context);

    _currentMotorId = MotorId::Motor1;
    _currentState = ST_MOTORINITIALISATION_VIEW_READY_WAIT;
    VIEWMODEL->onActivated();
    MTI_DEBUG("Function activated ...\r\n");

}

//------------------------------------------------------------------------------------------------------------
void MotorInitialisationController::deactivate(AppFunctionContext& context)
{
    if (_isReady)
    {
        _isReady = false;
        context.clearFunction(gpio::ButtonId::Button1, gpio::Button::ButtonSingleClick,   false);
        context.clearFunction(gpio::ButtonId::Button1, gpio::Button::ButtonDoubleClick,   false);
        context.clearFunction(gpio::ButtonId::Button1, gpio::Button::ButtonTripleClick,   false);
        context.clearFunction(gpio::ButtonId::Button1, gpio::Button::ButtonLongPress,     false);
        context.clearFunction(gpio::ButtonId::Button1, gpio::Button::ButtonLongPress,     true);
        context.clearFunction(gpio::ButtonId::Button1, gpio::Button::ButtonVeryLongPress, true);
        context.clearFunction(gpio::ButtonId::Button3, gpio::Button::ButtonSingleClick,   false);
        MTI_DEBUG("Buttons unassigned...\r\n");
    }

    _isTimerAnimationEnabled  = false;
    _isTimerActivationEnabled = false;
    _currentState             = ST_MOTORINITIALISATION_IDLE;
    
    if (_fastMotorInit)
    {
        MTI_DEBUG("Cancel running motor init movement while fastMotorInit is true...\r\n");
        _fastMotorInit = false;
        VIEWMODEL->cancelMotorInit();
    }


    VIEWMODEL->onDeactivated();
    MTI_DEBUG("Function disactivated ...\r\n");
}

//------------------------------------------------------------------------------------------------------------
void MotorInitialisationController::onViewActionChanged(::ActionId::eActionId actionId)
{
    MTI_DEBUG("Triggered by %s !\r\n", ::ActionId::to_string(actionId));
    if (actionId == ::ActionId::ACTION_MOTOR_MOVE_END)
    {
        _onMotorMoveEnd();
    }
}


/* **************************************************************************************************************** */
/* Protected section                                                                                                */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
EventStatus MotorInitialisationController::processEvent()
{
    switch(getCurrentEvent()->getEventType())
    {
        /* ******************************************************************************************************** */
        /* Timeout event                                                                                            */
        /* ******************************************************************************************************** */
        case IXFEvent::Timeout:
        {
            eTimeoutId timeoutId = static_cast<eTimeoutId>(getCurrentTimeout()->getId());
            MTI_DEBUG("Timeout Event received, %s (id: %d)...\r\n", _timeoutId_to_string(timeoutId), timeoutId);
            if (timeoutId == TimeoutAnimationId && _isTimerAnimationEnabled)
            {
                _isTimerAnimationEnabled = false;
                _currentState = ST_MOTORINITIALISATION_INIT_MOTOR_CHANGE;
                _currentMotorId = MotorId::Motor1;
                MTI_DEBUG("Starts animating Motor1\r\n");
                _isReady = false;
                VIEWMODEL->motorAnimationStart(_currentMotorId);
            }
            else if (timeoutId == TimeoutActivationId && _isTimerActivationEnabled)
            {
                _isTimerActivationEnabled = false;
                if (_currentState == ST_MOTORINITIALISATION_VIEW_READY_WAIT)
                {
                    _currentState = ST_MOTORINITIALISATION_INIT_MOTOR_WAIT;
                    _startTimeoutAnimation();
                }
            }
            break;
        }

        /* ******************************************************************************************************** */
        /* Initial event                                                                                            */
        /* ******************************************************************************************************** */
        case IXFEvent::Initial:
        {
            MTI_DEBUG("Initiale Event received...\r\n");
            if (_currentState == ST_MOTORINITIALISATION_INIT)
            {
                // Update State
                _currentState = ST_MOTORINITIALISATION_IDLE;
            }
            else
            {
                MTI_ERROR("_currentState %s is wrong !\r\n", _currentState_to_string());
                ASSERT(false);
            }
            break;
        }
        default:
            break;
    }

    return EventStatus::Consumed;
}


/* **************************************************************************************************************** */
/* Private section                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationController::_startTimeoutAnimation()
{
    if (!_isTimerAnimationEnabled)
    {
        MTI_DEBUG("Schedule an Animation's timeout of 100ms\r\n");
        this->getThread()->scheduleTimeout(TimeoutAnimationId, 250, this);
        _isTimerAnimationEnabled = true;
    }
    else
    {
        MTI_WARN("Timeout Animation already enabled !\r\n");
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MotorInitialisationController::_onMotorMoveEnd()
{
    switch(_currentState)
    {
        case ST_MOTORINITIALISATION_INIT_MOTOR_STEP:
        {
            _deviceController->resetHighReactivityCounter();
            _currentState = ST_MOTORINITIALISATION_INIT_MOTOR;
            break;
        }
        case ST_MOTORINITIALISATION_VIEW_READY_WAIT:
        {
            MTI_INFO("ST_MOTORINITIALISATION_VIEW_READY_WAIT\r\n");
            if (!_isReady)
            {
                _isReady      = true;
                _appContext->registerFunction(gpio::ButtonId::Button1, gpio::Button::ButtonSingleClick,   FunctionCall(this, MOTOR_INITIALISATION_SINGLE_STEP),     false);
                _appContext->registerFunction(gpio::ButtonId::Button1, gpio::Button::ButtonDoubleClick,   FunctionCall(this, MOTOR_INITIALISATION_SINGLE_STEP),     false);
                _appContext->registerFunction(gpio::ButtonId::Button1, gpio::Button::ButtonTripleClick,   FunctionCall(this, MOTOR_INITIALISATION_SINGLE_STEP),     false);
                _appContext->registerFunction(gpio::ButtonId::Button1, gpio::Button::ButtonLongPress,     FunctionCall(this, MOTOR_INITIALISATION_CONTINUES_STEPS), false);
                _appContext->registerFunction(gpio::ButtonId::Button1, gpio::Button::ButtonLongPress,     FunctionCall(this, MOTOR_INITIALISATION_CONTINUES_STOP),  true);
                _appContext->registerFunction(gpio::ButtonId::Button1, gpio::Button::ButtonVeryLongPress, FunctionCall(this, MOTOR_INITIALISATION_CONTINUES_STOP),  true);
                _appContext->registerFunction(gpio::ButtonId::Button3, gpio::Button::ButtonSingleClick,   FunctionCall(this, MOTOR_INITIALISATION_CHANGE_MOTOR),    false);
                MTI_DEBUG("Button assigned...\r\n");

                // Run a timeout here before starting motor's animation !!!
                if (!_isTimerActivationEnabled)
                {
                    MTI_DEBUG("Schedule an Activation's timeout of 100ms\r\n");
                    this->getThread()->scheduleTimeout(TimeoutActivationId, 100, this);
                    MTI_DEBUG("Timeout scheduled\r\n");
                    _isTimerActivationEnabled = true;
                }
                else
                {
                    MTI_WARN("Timeout Activation already enabled !\r\n");
                }
            }
            break;
        }
        case ST_MOTORINITIALISATION_INIT_MOTOR_WAIT:
        {
            _startTimeoutAnimation();
            break;
        }
        case ST_MOTORINITIALISATION_INIT_MOTOR_CHANGE:
        {
            MTI_DEBUG("motor 0x%02x should go backwards...\r\n", _currentMotorId);
            _currentState = ST_MOTORINITIALISATION_INIT_MOTOR_CHANGE_END;
            VIEWMODEL->motorAnimationEnd(_currentMotorId);
            break;
        }
        case ST_MOTORINITIALISATION_INIT_MOTOR_CHANGE_END:
        {
            MTI_DEBUG("Ready to init motor 0x%02x\r\n", _currentMotorId);
            _isReady = true;
            VIEWMODEL->setMotorStepsForFastInit(_currentMotorId);
            _currentState = ST_MOTORINITIALISATION_INIT_MOTOR_CONFIRM;
            break;
        }
        default:
            break;
    }
}

// ------------------------------------------------------------------------------------------------------------------
const char* MotorInitialisationController::_currentState_to_string()
{
    switch (_currentMotorId)
    {
        case ST_MOTORINITIALISATION_UNKOWN:                 return "ST_MOTORINITIALISATION_UNKOWN";
        case ST_MOTORINITIALISATION_INIT:                   return "ST_MOTORINITIALISATION_INIT";
        case ST_MOTORINITIALISATION_IDLE:                   return "ST_MOTORINITIALISATION_IDLE";
        case ST_MOTORINITIALISATION_VIEW_READY_WAIT:        return "ST_MOTORINITIALISATION_VIEW_READY_WAIT";
        case ST_MOTORINITIALISATION_INIT_MOTOR_WAIT:        return "ST_MOTORINITIALISATION_INIT_MOTOR_WAIT";
        case ST_MOTORINITIALISATION_INIT_MOTOR:             return "ST_MOTORINITIALISATION_INIT_MOTOR";
        case ST_MOTORINITIALISATION_INIT_MOTOR_CONFIRM:     return "ST_MOTORINITIALISATION_INIT_MOTOR_CONFIRM";
        case ST_MOTORINITIALISATION_INIT_MOTOR_STEP:        return "ST_MOTORINITIALISATION_INIT_MOTOR_STEP";
        case ST_MOTORINITIALISATION_INIT_MOTOR_CHANGE:      return "ST_MOTORINITIALISATION_INIT_MOTOR_CHANGE";
        case ST_MOTORINITIALISATION_INIT_MOTOR_CHANGE_END:  return "ST_MOTORINITIALISATION_INIT_MOTOR_CHANGE_END";
        default:                                            return "ST_MOTORINITIALISATION_TOP_ENUM";
    }
}

// ------------------------------------------------------------------------------------------------------------------
const char* MotorInitialisationController::_timeoutId_to_string(eTimeoutId timeoutId)
{
    switch (timeoutId) 
    {
        case TimeoutAnimationId:    return "TimeoutAnimationId";
        case TimeoutActivationId:   return "TimeoutActivationId";
        case TimeoutTopEnumId:      return "TimeoutTopEnumId";
        default:                    return "Unknown TimeoutId";
    }
}

#endif
