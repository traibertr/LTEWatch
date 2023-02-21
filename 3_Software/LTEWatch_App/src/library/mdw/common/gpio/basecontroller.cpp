/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "basecontroller.h"

#include <nrfx_gpiote.h>
#include "debug-config.h"

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#define DEBUG_BASE_CONTROLLER_ENABLE        0
#if (DEBUG_BASE_CONTROLLER_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(base_ctrl, LOG_LEVEL_DBG);
    #define BASC_DEBUG(args...)             LOG_DBG(args) 
    #define BASC_ERROR(args...)             LOG_ERR(args) 
#else
    #define BASC_DEBUG(...)                 {(void) (0);}
    #define BASC_ERROR(...)                 {(void) (0);}
#endif


/* **************************************************************************************************************** */
/* Namespace Declaration                                                                                            */
/* **************************************************************************************************************** */
using gpio::BaseController;


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void BaseController::initialize()
{
    // Initializations
    for (int i = 0; i < NB_MAX_GPIOS; i++)
    {
        _delayCounters[i]       = 0;
        _delays[i]              = APP_GPIO_MS_DELAY;
    }
    _nbRegisteredObject         = 0;
    _pinMaskForRegisteredObject = 0;

    _isHandlerTimerActive       = false;
    _isHandlerTimerInitialized  = false;

    if (!nrfx_gpiote_is_init())
    {
        uint32_t errCode        = nrfx_gpiote_init(GPIO_DEFAULT_PRIORITY);
        if (errCode != NRFX_SUCCESS)
        {
            BASC_ERROR("nrfx_gpiote_init() FAILED! (errCode: %s)\r\n", tools::LogTrace::to_string(errCode));
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
uint32_t BaseController::delayCounters(uint8_t index) const
{
    if (index < _nbRegisteredObject)
    {
        return _delayCounters[index];
    }
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
void BaseController::setDelay(uint8_t index, uint32_t delay)
{
    if (index < NB_MAX_GPIOS)
    {
        _delays[index] = delay;
        BASC_DEBUG("new delay for gpio %d is %d\r\n", index, delay);
    }
}

// ------------------------------------------------------------------------------------------------------------------
uint32_t BaseController::delay(uint8_t index) const
{
    if (index < _nbRegisteredObject)
    {
        return _delays[index];
    }
    BASC_DEBUG("index %d is out of bound ! (_nbRegisteredObject = %d)\r\n", index, _nbRegisteredObject);
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
void BaseController::resetDelayCounter(uint8_t index, uint8_t resetVal)
{
    if (index < _nbRegisteredObject)
    {
        _delayCounters[index] = resetVal;
    }
}

// ------------------------------------------------------------------------------------------------------------------
void BaseController::incrDelayCounter(uint8_t index, uint32_t offset)
{
    if (index < _nbRegisteredObject)
    {
        _delayCounters[index] += offset;
    }
}
