/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * ******************************************************************************************************************/

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "calendar.h"
#include "sdktools.h"

#include "uicr-config.h"


/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#define DEBUG_CALENDAR_ENABLE           1
#if (DEBUG_CALENDAR_ENABLE == 1) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(calendar, LOG_LEVEL_DBG);
    #define C_DEBUG(args...)              LOG_DBG(args)
    #define C_ERROR(args...)              LOG_ERR(args)
#else
    #define C_DEBUG(...)                    {(void) (0);}
    #define C_ERROR(...)                    {(void) (0);}
#endif

// APP_TIMER / K_TIMER OPTION
#define USE_K_TIMER 1

#if (USE_CALENDAR != 0)
/* **************************************************************************************************************** */
/* Declaration section                                                                                              */
/* **************************************************************************************************************** */
static struct k_timer _calendarTimerId;
static struct k_timer _calendarInhibitionTimerId;

/* **************************************************************************************************************** */
/* Namespace's Declaration                                                                                          */
/* **************************************************************************************************************** */
using cal::Calendar;


/* **************************************************************************************************************** */
/* --- Extern "C" Section ---                                                                                       */
/*     Callback's section                                                                                           */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void cal::calendarTimerEvent(struct k_timer *timer_id)
{
    if (timer_id != NULL)
    {
        void* pContext = k_timer_user_data_get(timer_id);
        (static_cast<Calendar*>(pContext))->updateSecond();
    }
}

// ------------------------------------------------------------------------------------------------------------------
void cal::calendarInhibitionTimerEvent(struct k_timer *timer_id)
{
    if (timer_id != NULL)
    {
        void* pContext = k_timer_user_data_get(timer_id);
        (static_cast<Calendar*>(pContext))->inhibitCrystal();
    }
}

/* **************************************************************************************************************** */
/* CONSTRUCTOR                                                                                                      */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
Calendar::Calendar() {}

// ------------------------------------------------------------------------------------------------------------------
Calendar::~Calendar() {}

/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void Calendar::initialize()
{
    _timerInit();
    _isCalActive = false;
    _calendarSettingActive = false;
    for (uint8_t i = 0; i < APP_NB_DATE_TIME_MAX; i++)
    {
        _dateTime[i] = NULL;
    }

    _nbRegisteredDateTime = 0;
    _inhibitionCounter = 0;
    _inhibitionInterval = UICR_GET_INTERVAL_INHIBITION;
    C_DEBUG("inhibitionInterval from UICR: %d.\r\n", _inhibitionInterval);
    if (_inhibitionInterval == 0xffffffff)
    {
        _inhibitionInterval = 60;
    }
    _inhibitionValue = UICR_GET_INHIBITION_VALUE_23C;
    C_DEBUG("inhibitionValue from UICR: %d.\r\n", _inhibitionValue);
    if (_inhibitionValue == ((int32_t)0xffffffff))
    {
        _inhibitionValue = 56;
    }
    else
    {
        _inhibitionValue -= CALENDAR_TICK_COUNTER_VALUE;
    }
    C_DEBUG("inhibition: Interval = %d and Value = %d.\r\n", _inhibitionInterval, _inhibitionValue);
}

// ------------------------------------------------------------------------------------------------------------------
void Calendar::registerDateTime(DateTime* dateTime)
{
    if (_nbRegisteredDateTime < APP_NB_DATE_TIME_MAX)
    {
        _dateTime[_nbRegisteredDateTime++] = dateTime;
        C_DEBUG("A new dateTime registered successfully.\r\n");
    }
    else
    {
        C_DEBUG("registration failed !\r\n");
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Calendar::startCalendar()
{
    _inhibitionCounter = 0;

    #if (USE_K_TIMER)
        uint32_t errCode = NRFX_SUCCESS;
        k_timer_user_data_set(&_calendarTimerId, this);
        k_timer_start(&_calendarTimerId, K_TICKS(CALENDAR_TICK_COUNTER_VALUE), K_TICKS(CALENDAR_TICK_COUNTER_VALUE));         
    #else
        errCode = tools::SDKTools::appTimerStart(&_calendarTimerId, CALENDAR_TICK_COUNTER_VALUE, this);
    #endif

    if (errCode != NRFX_SUCCESS)
    {
        C_ERROR("Calendar timer start FAILED for _calendarTimerId ! (errCode: %s)\r\n", errCode);
        //ASSERT(false);
    }

    #if (USE_K_TIMER)
        k_timer_user_data_set(&_calendarInhibitionTimerId, this);
        k_timer_start(&_calendarInhibitionTimerId, K_TICKS(((CALENDAR_TICK_COUNTER_VALUE * (_inhibitionInterval - 1)) + _inhibitionValue)),
                                                             K_TICKS(((CALENDAR_TICK_COUNTER_VALUE * (_inhibitionInterval - 1)) + _inhibitionValue)));         
    #else
        errCode = tools::SDKTools::appTimerStart(&_calendarInhibitionTimerId, ((CALENDAR_TICK_COUNTER_VALUE * (_inhibitionInterval - 1)) + _inhibitionValue), this);            
    #endif

    if (errCode != NRFX_SUCCESS)
    {
        C_ERROR("alendar timer start FAILED for _calendarInhibitionTimerId ! (errCode: %s)\r\n", errCode);
        //ASSERT(false);
    }

    _isCalActive = true;
}

// ------------------------------------------------------------------------------------------------------------------
void Calendar::stopCalendar()
{
    #if (USE_K_TIMER)
        uint32_t errCode = NRFX_SUCCESS;
        k_timer_stop(&_calendarTimerId);
    #else
        errCode = tools::SDKTools::appTimerStop(&_calendarTimerId);
    #endif

    if (errCode != NRFX_SUCCESS)
    {
        C_ERROR("Calendar timer stop FAILED for _calendarTimerId ! (errCode: %s)\r\n", errCode);
        //ASSERT(false);
    }

    #if (USE_K_TIMER)
        k_timer_stop(&_calendarTimerId);        
    #else
        errCode = tools::SDKTools::appTimerStop(&_calendarInhibitionTimerId);
    #endif
   
    if (errCode != NRFX_SUCCESS)
    {
        C_ERROR("Calendar timer stop FAILED for _calendarInhibitionTimerId ! (errCode: %s)\r\n", errCode);
        //ASSERT(false);
    }

    _isCalActive = false;
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t Calendar::second(uint8_t index)
{
    if (index < _nbRegisteredDateTime)
    {
        return _dateTime[index]->second();
    }
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t Calendar::minute(uint8_t index)
{
    if (index < _nbRegisteredDateTime)
    {
        return _dateTime[index]->minute();
    }
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t Calendar::hour(uint8_t index)
{
    if (index < _nbRegisteredDateTime)
    {
        return _dateTime[index]->hour();
    }
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
cal::DateTime::eDayOfWeek Calendar::dayOfWeek(uint8_t index)
{
    if (index < _nbRegisteredDateTime)
    {
        return _dateTime[index]->dayOfWeek();
    }
    return DateTime::Sunday;
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t Calendar::day(uint8_t index)
{
    if (index < _nbRegisteredDateTime)
    {
        return _dateTime[index]->day();
    }
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
cal::DateTime::eMonth Calendar::month(uint8_t index)
{
    if (index < _nbRegisteredDateTime)
    {
        return _dateTime[index]->month();
    }
    return DateTime::UnknownMonth;
}

// ------------------------------------------------------------------------------------------------------------------
uint16_t Calendar::year(uint8_t index)
{
    if (index < _nbRegisteredDateTime)
    {
        return _dateTime[index]->year();
    }
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t Calendar::shortYear(uint8_t index)
{
    if (index < _nbRegisteredDateTime)
    {
        return _dateTime[index]->shortYear();
    }
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
cal::DateTime* Calendar::getDateTime(uint8_t index)
{
    if (index < _nbRegisteredDateTime)
    {
        return _dateTime[index];
    }
    return NULL;
}

// ------------------------------------------------------------------------------------------------------------------
void Calendar::resetSecond(uint8_t index)
{
    if (index < _nbRegisteredDateTime)
    {
        _dateTime[index]->setSecond(0);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Calendar::setSecond(uint8_t index, uint8_t second)
{
    if (index < _nbRegisteredDateTime)
    {
        _dateTime[index]->setSecond(second);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Calendar::incrMinute(uint8_t index, uint8_t amount)
{
    if (index < _nbRegisteredDateTime)
    {
        _dateTime[index]->incrMinute(amount, true);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Calendar::decrMinute(uint8_t index, uint8_t amount)
{
    if (index < _nbRegisteredDateTime)
    {
        _dateTime[index]->decrMinute(amount, true);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Calendar::setMinute(uint8_t index, uint8_t minute)
{
    if (index < _nbRegisteredDateTime)
    {
        _dateTime[index]->setMinute(minute);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Calendar::setHour(uint8_t index, uint8_t hour)
{
    if (index < _nbRegisteredDateTime)
    {
        _dateTime[index]->setHour(hour);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Calendar::updateSecond()
{
    if (++_inhibitionCounter < (_inhibitionInterval - 1))
    {
        _onSecondChanged();
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Calendar::inhibitCrystal()
{
    #if (USE_K_TIMER)
        uint32_t errCode = NRFX_SUCCESS;
        k_timer_stop(&_calendarTimerId);        
    #else
        uint32_t errCode = tools::SDKTools::appTimerStop(&_calendarTimerId);
    #endif
    
    if (errCode != NRFX_SUCCESS)
    {
        C_ERROR("Calendar timer stop FAILED for _calendarTimerId ! (errCode: %s)\r\n", errCode);
        //ASSERT(false);
    }

    _inhibitionCounter = 0;
    _onSecondChanged();

    #if (USE_K_TIMER)
        k_timer_user_data_set(&_calendarTimerId, this);
        k_timer_start(&_calendarTimerId, K_TICKS(CALENDAR_TICK_COUNTER_VALUE), K_TICKS(CALENDAR_TICK_COUNTER_VALUE));        
    #else
        errCode = tools::SDKTools::appTimerStart(&_calendarTimerId, CALENDAR_TICK_COUNTER_VALUE, this);
    #endif
    
    if (errCode != NRFX_SUCCESS)
    {
        C_ERROR("Calendar timer stop FAILED for _calendarTimerId ! (errCode: %s)\r\n", errCode);
        //ASSERT(false);
    }
}

/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void Calendar::_timerInit()
{
    #if (USE_K_TIMER)
        uint32_t errCode = NRFX_SUCCESS;
        k_timer_init(&_calendarTimerId, cal::calendarTimerEvent, NULL);         
    #else
        uint32_t errCode = tools::SDKTools::appTimerCreate(&_calendarTimerId,
                                                            APP_TIMER_MODE_REPEATED,
                                                            (void*) cal::calendarTimerEvent);               
    #endif
    if (errCode != NRFX_SUCCESS)
    {
        C_ERROR("SDKTools::appTimerCreate() FAILED ! (errCode: %s)\r\n", errCode);
        //ASSERT(false);
    }

    #if (USE_K_TIMER)
        k_timer_init(&_calendarInhibitionTimerId, cal::calendarInhibitionTimerEvent, NULL);         
    #else
        errCode = tools::SDKTools::appTimerCreate(&_calendarInhibitionTimerId,
                                                    APP_TIMER_MODE_REPEATED,
                                                    (void*) cal::calendarInhibitionTimerEvent);             
    #endif
    if (errCode != NRFX_SUCCESS)
    {
        C_ERROR("SDKTools::appTimerCreate() FAILED ! (errCode: %s)\r\n", errCode);
        //ASSERT(false);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void Calendar::_onSecondChanged()
{
    if (!_calendarSettingActive)
    {
        for (uint8_t i = 0; i < _nbRegisteredDateTime; i++)
        {
            _dateTime[i]->oneSecondMore();
        }

        _notifySecond();
        C_DEBUG("UTC date: %s, time: %s, day of week: %d and week number: %d\r\n",
                _dateTime[0]->strDate(), _dateTime[0]->strTime(),
                (uint8_t) _dateTime[0]->dayOfWeek(), (uint8_t) _dateTime[0]->weekNumber());
    }
    else
    {
        C_DEBUG("Calendar is in `Setting` mode. No Update !\r\n");
    }
}


// ------------------------------------------------------------------------------------------------------------------
void Calendar::_notifySecond()
{
    for (uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != NULL)
        {
            observer(i)->onSecond(this);
        }
    }
}

#endif  // USE_CALENDAR
