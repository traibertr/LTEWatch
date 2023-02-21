/************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 ************************************************************************************************************/

/************************************************************************************************************/
/* Header files                                                                                             */
/************************************************************************************************************/
#include "motordriverhal.h"
#include "debug-config.h"

#if (USE_MOTORS != 0)

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#if (DEBUG_APP_FUNCTION_MANAGER_BH_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(motor_driver_hal, LOG_LEVEL_DBG);
    #define MHAL_DEBUG(args...)              LOG_DBG(args)
    #define MHAL_ERROR(args...)              LOG_ERR(args) 
#else
    #define MHAL_DEBUG(...)                  {(void) (0);}
    #define MHAL_ERROR(...)                  {(void) (0);}
#endif


#include "factory.h"
/************************************************************************************************************/
/* Namespace's Declaration                                                                                  */
/************************************************************************************************************/
using hal::MotorDriverHal;
//using motor::MotorDriver;

#if(NRFX_RTC_DRIVER != 0)
/************************************************************************************************************/
/* --- Extern "C" Section ---                                                                               */
/*     Callback's section                                                                                   */
/************************************************************************************************************/
//----------------------------------------------------------------------------------------------------------*
void hal::motorTimerHandler(nrfx_rtc_int_type_t intType)
{
    switch(intType)
    {
        case NRFX_RTC_INT_COMPARE0:
            Factory::motorController.clearPulseOnActiveMotor();
            MHAL_DEBUG("motorTimerHandler -> NRFX_RTC_INT_COMPARE0");
            break;

        case NRFX_RTC_INT_COMPARE1:
            break;

        case NRFX_RTC_INT_COMPARE2:
            break;

        case NRFX_RTC_INT_COMPARE3:
            MHAL_DEBUG("motorTimerHandler -> NRFX_RTC_INT_COMPARE3");
            Factory::motorController.notifyMotorTimerEvent();
            break;

        case NRFX_RTC_INT_TICK:
        case NRFX_RTC_INT_OVERFLOW:
            break;
    }
}

#else

#define TIMER_NO_PULSE 0
#define TIMER_PULSE 1

void hal::motorTimerHandler(struct k_timer *timer_id)
{
    if (timer_id == (hal::MotorDriverHal::_pPulseTimer))
    {
        uint32_t*data=static_cast<uint32_t*>(k_timer_user_data_get(timer_id)); 
        if(*data == TIMER_NO_PULSE )
        {
            MHAL_DEBUG(" motorTimerHandler -> NO PULSE TIMER");
        }
        else if(*data == TIMER_PULSE)
        {
            MHAL_DEBUG("motorTimerHandler -> PULSE TIMER");
        }
        //Factory::motorController.clearPulseOnActiveMotor();
    }
    if (timer_id == (hal::MotorDriverHal::_pGapTimer))
    {
        MHAL_DEBUG("motorTimerHandler -> GAP TIMER");
         //Factory::motorController.notifyMotorTimerEvent();
    }
}

// Static member initialization
struct k_timer* hal::MotorDriverHal::_pPulseTimer   = NULL;
struct k_timer* hal::MotorDriverHal::_pGapTimer     = NULL;

#endif

/************************************************************************************************************/
/* CONSTRUCTOR                                                                                              */
/************************************************************************************************************/
// ----------------------------------------------------------------------------------------------------------
MotorDriverHal::MotorDriverHal() : _isTimerInitialized(false), _isTimerPoweredOn(false), _pulseTick(0), _gapTick(0)
{
    
    #if(NRFX_RTC_DRIVER)
        // creating RTC driver instance
        rtc = NRFX_RTC_INSTANCE(MOTOR_RTC_ENABLE_ID);
    #else
            // Constructor
    #endif
}
//-----------------------------------------------------------------------------------------------------------
MotorDriverHal::~MotorDriverHal() {}

/************************************************************************************************************/
/* PUBLIC SECTION                                                                                           */
/************************************************************************************************************/
//-----------------------------------------------------------------------------------------------------------
void MotorDriverHal::setPulseTick(uint32_t pulseLen)
{
    #if(NRFX_RTC_DRIVER != 0)
        _pulseTick = NRFX_RTC_US_TO_TICKS(pulseLen, RTC_INPUT_FREQ) - PULSE_TICK_CORRECTION;
        MHAL_DEBUG("For pulse = %d [us], PulseTick = %d", pulseLen, _pulseTick);
        if (_pulseTick == 0)
        {
            MHAL_DEBUG("ERROR while computing the timer's ticks ...");
            //ASSERT(false);
        }
    #else
        _pulseTick = k_us_to_ticks_near32(pulseLen) - PULSE_TICK_CORRECTION;
        MHAL_DEBUG("For pulse %d [us] PulseTick is %d", pulseLen, _pulseTick);
        if (_pulseTick == 0)
        {
            MHAL_ERROR("ERROR while computing the timer's ticks ...");
        }
    #endif
}

//-----------------------------------------------------------------------------------------------------------
void MotorDriverHal::setGapTick(uint32_t gapTick)
{
    _gapTick = gapTick;
    #if(NRFX_RTC_DRIVER != 0)
        if (gapTick < (_pulseTick + RTC_TIMER_MIN_CC_DELAY))
        {
            _gapTick = _pulseTick + RTC_TIMER_MIN_CC_DELAY;
        }

        if (_isTimerInitialized)
        {
            nrfx_err_t errCode = nrfx_rtc_cc_set(&rtc, MOTOR_PULSE_CC_IDX, _pulseTick, true);
            errCode = nrfx_rtc_cc_set(&rtc, MOTOR_GAP_CC_IDX, _gapTick, true);
            if (errCode != NRFX_SUCCESS)
            {
                MHAL_ERROR("startTimerPulse CC1 ERROR: 0x%08x", errCode);
                //MHAL_ERROR("startTimerPulse CC1 ERROR: %s (errCode: 0x%08x)", NRFX_MHAL_ERROROR_STRING_GET(errCode), errCode);
            }
            MHAL_DEBUG("For GapTick = %d", _gapTick);
        }
    #else
        if (gapTick < (_pulseTick + TIMER_MIN_CC_DELAY))
        {
            _gapTick = _pulseTick + TIMER_MIN_CC_DELAY;
        }
    #endif
}

//-----------------------------------------------------------------------------------------------------------
void MotorDriverHal::setGpioMask(uint8_t* motorCoils, size_t size)
{
    // Set up the mask
    _gpioMask = (1ULL << motorCoils[0]);
    for(uint8_t i = 1; i < size; i++)
    {
        _gpioMask |= (1ULL << motorCoils[i]);
    }
}

//-----------------------------------------------------------------------------------------------------------
void MotorDriverHal::setGpioOutputs(uint8_t* motorCoils, size_t size)
{
    // Set as output's GPIO both coilsdriving the motor's step
    for(uint8_t i = 0; i < static_cast<uint8_t>(size); i++)
    {
        hal::GpioHal::cfgPinOutput(motorCoils[i], GPIO_PIN_DRIVE, GPIO_PIN_PULL_CONFIG, GPIO_PIN_CONNECTION);
        MHAL_DEBUG("cfgPinOutput  pin: %d", motorCoils[i]);
    }
}

//-----------------------------------------------------------------------------------------------------------
bool MotorDriverHal::startTimerNoPulse()
{
    #if(NRFX_RTC_DRIVER != 0)
        static nrfx_err_t rtcErrCode = NRFX_SUCCESS;

        nrfx_rtc_counter_clear(&rtc);
        rtcErrCode = nrfx_rtc_cc_set(&rtc, MOTOR_PULSE_CC_IDX, _pulseTick, false);
        if (rtcErrCode != NRFX_SUCCESS)
        {
            MHAL_ERROR("nrfx_rtc_cc_set CC0 ERROR: 0x%08x", rtcErrCode);
            //LOG_ERR("nrfx_rtc_cc_set CC0 ERROR: %s (errCode: 0x%08x)\r\n", NRFX_LOG_ERROR_STRING_GET(rtcErrCode), rtcErrCode);
            return false;
        }
        rtcErrCode = nrfx_rtc_cc_set(&rtc, MOTOR_GAP_CC_IDX, _gapTick, true);
        if (rtcErrCode != NRFX_SUCCESS)
        {
            MHAL_ERROR("nrfx_rtc_cc_set CC1 ERROR: 0x%08x", rtcErrCode);
           // LOG_ERR("nrfx_rtc_cc_set CC1 ERROR: %s (errCode: 0x%08x)\r\n", NRFX_LOG_ERROR_STRING_GET(rtcErrCode), rtcErrCode);
            return false;
        }

        if (!_isTimerPoweredOn)
        {
            nrfx_rtc_enable(&rtc);
            _isTimerPoweredOn = true;
        }
        return true;
    #else
        if (!_isTimerInitialized)
        {
            hal::MotorDriverHal::_timerInit();
        }
        
        static uint32_t data = TIMER_NO_PULSE;

        k_timer_user_data_set(_pPulseTimer,  static_cast<void*>(&data)); 

        k_timer_start(_pPulseTimer, K_TICKS(_pulseTick), K_NO_WAIT);

        return true;
    #endif
}

//-----------------------------------------------------------------------------------------------------------
bool MotorDriverHal::startTimerPulse()
{
    #if(NRFX_RTC_DRIVER != 0)
        static nrfx_err_t rtcErrCode = NRFX_SUCCESS;
        nrfx_rtc_counter_clear(&rtc);
        rtcErrCode = nrfx_rtc_cc_set(&rtc, MOTOR_GAP_CC_IDX, _gapTick, true);
        if (rtcErrCode != NRFX_SUCCESS)
        {
            MHAL_ERROR("startTimerPulse CC1 ERROR: 0x%08x",rtcErrCode);
            //LOG_ERR("startTimerPulse CC1 ERROR: %s (errCode: 0x%08x)", NRFX_LOG_ERROR_STRING_GET(rtcErrCode), rtcErrCode);
            return false;
        }
        rtcErrCode = nrfx_rtc_cc_set(&rtc, MOTOR_PULSE_CC_IDX, _pulseTick, true);
        if (rtcErrCode != NRFX_SUCCESS)
        {
            MHAL_ERROR("nrfx_rtc_cc_set CC0 ERROR: 0x%08x", rtcErrCode);
            //LOG_ERR("nrfx_rtc_cc_set CC0 ERROR: %s (errCode: 0x%08x)", NRFX_LOG_ERROR_STRING_GET(rtcErrCode), rtcErrCode);
            return false;
        }
        if (!_isTimerPoweredOn)
        {
            nrfx_rtc_enable(&rtc);
            _isTimerPoweredOn = true;
        }
        // Bind the RTC IRQ to System 
        IRQ_DIRECT_CONNECT(RTC0_IRQn, 0, nrfx_rtc_0_irq_handler, 0);
	    irq_enable(RTC0_IRQn);
        
        return true;
    #else
        if (!_isTimerInitialized)
        {
            hal::MotorDriverHal::_timerInit();
        }

        static uint32_t data = TIMER_PULSE;

        k_timer_user_data_set(_pPulseTimer,  static_cast<void*>(&data)); 

        k_timer_start(_pPulseTimer, K_TICKS(_pulseTick),    K_NO_WAIT); /// Start pulse timer
        k_timer_start(_pGapTimer,   K_TICKS(_gapTick),      K_NO_WAIT); /// Start gap timer (pulsen + gaplen)

        return true;
    #endif
}

//-----------------------------------------------------------------------------------------------------------
bool MotorDriverHal::enableGapPulse()
{
    #if(NRFX_RTC_DRIVER != 0)
        return nrfx_rtc_cc_set(&rtc, MOTOR_GAP_CC_IDX, _gapTick, true);
    #else
        if (!_isTimerInitialized)
        {
            hal::MotorDriverHal::_timerInit();
        }

        // Is never use in application
        
        //k_timer_start(_pGapTimer,   K_TICKS(_gapTick),      K_NO_WAIT); /// Start gap timer (pulsen + gaplen)

        return true;
    #endif
}

//-----------------------------------------------------------------------------------------------------------
void MotorDriverHal::disableGapPulse()
{
    #if(NRFX_RTC_DRIVER != 0)
        if (!nrfx_rtc_cc_set(&rtc, MOTOR_GAP_CC_IDX, _gapTick, false))
        {
            MHAL_DEBUG("nrfx_rtc_cc_set returns `false`");
        }
    #else
        if (!_isTimerInitialized)
        {
            hal::MotorDriverHal::_timerInit();
        }
        k_timer_stop(_pGapTimer);
    #endif
}

//-----------------------------------------------------------------------------------------------------------
void MotorDriverHal::stopTimer()
{
    if (_isTimerPoweredOn)
    {
        #if(NRFX_RTC_DRIVER != 0)

            nrfx_rtc_disable(&rtc);
        #else
            k_timer_stop(_pPulseTimer);
            k_timer_stop(_pGapTimer);
        #endif
        _isTimerPoweredOn = false;
    }
}

/************************************************************************************************************/
/* PRIVATE SECTION                                                                                          */
/************************************************************************************************************/
//-----------------------------------------------------------------------------------------------------------
void MotorDriverHal::_timerInit()
{
    MHAL_DEBUG("%s"," Timer init...");
    #if(NRFX_RTC_DRIVER != 0)
        
        _rtcDrvConfig             = NRFX_RTC_DEFAULT_CONFIG; 
        nrfx_err_t errCode        = nrfx_rtc_init(&rtc, &_rtcDrvConfig, hal::motorTimerHandler);

        if (errCode == NRFX_SUCCESS)
        {
            _isTimerInitialized = true;
            MHAL_DEBUG("Succeeded");
        }
        else
        {
            _isTimerInitialized = false;
            MHAL_ERROR("errCode: 0x%08x", errCode);
            //LOG_ERR("errCode: %s (errCode: 0x%08x)", NRFX_LOG_ERROR_STRING_GET(errCode), errCode);
        }
    #else
        if (hal::MotorDriverHal::_pPulseTimer == NULL ) // If timer not initialised
        {
            // Creat a timer
            static struct k_timer pulseTimer;
            hal::MotorDriverHal::_pPulseTimer = &pulseTimer;
            // Initialize it
            k_timer_init (hal::MotorDriverHal::_pPulseTimer, motorTimerHandler, NULL );
        }

        if (hal::MotorDriverHal::_pGapTimer == NULL ) // If timer not initialised
        {
            // Creat a timer
            static struct k_timer gapTimer ;
            hal::MotorDriverHal::_pGapTimer = &gapTimer;

            // Initialize it
            k_timer_init (hal::MotorDriverHal::_pGapTimer, motorTimerHandler, NULL );
        }
    
        if ((hal::MotorDriverHal::_pGapTimer == NULL)||(hal::MotorDriverHal::_pPulseTimer == NULL ))
        {
            _isTimerInitialized = false;
            MHAL_ERROR("Failed");
        }
        else
        {
            _isTimerInitialized = true;
            MHAL_DEBUG("Succeeded");
        }
    #endif
}

#endif  // #if (USE_MOTORS != 0)