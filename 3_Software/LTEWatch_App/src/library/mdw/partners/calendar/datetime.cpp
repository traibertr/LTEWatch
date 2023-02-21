/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * ******************************************************************************************************************/

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "datetime.h"

#include <string.h>
#include <stdio.h>


/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#define DEBUG_DATETIME_ENABLE               0
#if (DEBUG_DATETIME_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(date_time, LOG_LEVEL_DBG);
    #define DT_DEBUG(args...)               LOG_DBG(args)
#else
    #define DT_DEBUG(...)                   {(void) (0);}
#endif


#if (USE_CALENDAR != 0)
/* **************************************************************************************************************** */
/* Namespace's Declaration                                                                                          */
/* **************************************************************************************************************** */
using cal::DateTime;


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void DateTime::initialize(bool hotReset)
{
    if (hotReset)
    {
        if ( _datetime.tm_hour < 0   || _datetime.tm_hour > 23)
        {
            _datetime.tm_hour   = 0;
        }
        if (_datetime.tm_min < 0     || _datetime.tm_min > 59)
        {
            _datetime.tm_min    = 0;
        }
        if (_datetime.tm_sec < 0     || _datetime.tm_sec  > 59)
        {
            _datetime.tm_sec    = 0;
        }
        if (_datetime.tm_mday < 1    || _datetime.tm_mday > 31)
        {
            _datetime.tm_mday   = 1;
        }
        if (_datetime.tm_mon < 0     || _datetime.tm_mon > 11)
        {
            _datetime.tm_mon    = 0;
        }
        if (_datetime.tm_year < 0)
        {
            _datetime.tm_year   = 119;
            _shortYear          = 19;
        }
        if (_weekSetting >= WeekSetting_TopEnum)
        {
            _weekSetting = WeekSetting_ISO;
        }
        _shortYear              = _datetime.tm_year % 100;
        _isLeapYear             = _checkLeapYear(_datetime.tm_year + 1900);
        oneSecondMore();    // Compensate the reset time
    }
    else
    {
        // Initialization of date and Time
        _datetime.tm_hour       = 00;
        _datetime.tm_min        = 00;
        _datetime.tm_sec        = 00;
        _datetime.tm_mday       = 1;
        _datetime.tm_mon        = 0;
        _datetime.tm_year       = 119;
        _weekSetting            = WeekSetting_ISO;
        _shortYear              = 19;
        _isLeapYear             = _checkLeapYear(_datetime.tm_year + 1900);
    }
    
    // Update the time_t variable
    _curDateTime                =  mktime(&_datetime);
    _weekNumber                 = _processWeekNumber(&_datetime);
}

// ------------------------------------------------------------------------------------------------------------------
DateTime::eDayOfWeek DateTime::firstDayOfWeek() const
{
    // Have a look at http://i.imgur.com/eHtAIwP.png
    switch (_weekSetting)
    {
        case WeekSetting_ISO:
        case WeekSetting_ISO_2:
            return Monday;

        case WeekSetting_US:
            return Sunday;

        case WeekSetting_ISLAMIC:
            return Saturday;

        default:
            DT_DEBUG("ERROR: Wring Week Setting %d\r\n", _weekSetting);
            return TopEnumDayOfWeek;
    }
}


// ------------------------------------------------------------------------------------------------------------------
DateTime::eMonth DateTime::month() const
{
    switch (_datetime.tm_mon)
    {
        case  0: return January;
        case  1: return February;
        case  2: return March;
        case  3: return April;
        case  4: return May;
        case  5: return June;
        case  6: return July;
        case  7: return August;
        case  8: return September;
        case  9: return October;
        case 10: return November;
        case 11: return December;
        default: return UnknownMonth;
    }
}

// ------------------------------------------------------------------------------------------------------------------
char* DateTime::strTime() const
{
    static char time[9];
    memset(time, '\0', sizeof(time));
    if (strftime(time, sizeof(time), "%T", &_datetime) != 0)
    {
        return time;
    }
    else
    {
        DT_DEBUG("Error while using `strftime`\r\n");
        return NULL;
    }
}

// ------------------------------------------------------------------------------------------------------------------
char* DateTime::strDate() const
{
    static char date[11];
    memset(date, '\0', sizeof(date));
    if (strftime(date, sizeof(date), "%d.%m.%Y", &_datetime) != 0)
    {
        return date;
    }
    else
    {
        DT_DEBUG("Error while using `strftime`\r\n");
        return NULL;
    }
}

// ------------------------------------------------------------------------------------------------------------------
void DateTime::setSecond(int second, bool update)
{
    _datetime.tm_sec = second;
    if (update)
    {
        _curDateTime =  mktime(&_datetime);
        _updateDateTime();
    }
    DT_DEBUG("%s\r\n", strTime());
}

// ------------------------------------------------------------------------------------------------------------------
void DateTime::oneSecondLess()
{
    _curDateTime -= 1;
    if (_datetime.tm_sec == 0)
    {
        if (_datetime.tm_min == 0)
        {
            if (_datetime.tm_hour == 0)
            {
                _updateDateTime();
            }
            else
            {
                _datetime.tm_hour--;
            }
        }
        else
        {
            _datetime.tm_min--;
        }
    }
    else
    {
        _datetime.tm_sec--;
    }
}

// ------------------------------------------------------------------------------------------------------------------
void DateTime::oneSecondMore()
{
    _curDateTime++;
    _datetime.tm_sec++;
    if (_datetime.tm_sec == 60)
    {
        _datetime.tm_sec = 0;
        _datetime.tm_min++;
        if (_datetime.tm_min == 60)
        {
            _datetime.tm_min = 0;
            _datetime.tm_hour++;
            if (_datetime.tm_hour == 24)
            {
                _updateDateTime();
            }
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void DateTime::setMinute(int minute, bool update)
{
    _datetime.tm_min = minute;
    if (update)
    {
        _curDateTime =  mktime(&_datetime);
    }
    DT_DEBUG("%s\r\n", strTime());
}

// ------------------------------------------------------------------------------------------------------------------
void DateTime::setHour(int hour, bool update)
{
    _datetime.tm_hour = hour;
    if (update)
    {
        _curDateTime =  mktime(&_datetime);
        _updateDateTime();
    }
    DT_DEBUG("%s\r\n", strTime());
}

// ------------------------------------------------------------------------------------------------------------------
void DateTime::setTime(uint8_t hour, uint8_t minute, uint8_t second, bool update)
{
    setSecond(second, false);
    setMinute(minute, false);
    setHour(hour, update);

    DT_DEBUG("%s\r\n", strTime());
}

// ------------------------------------------------------------------------------------------------------------------
void DateTime::setDay(int day, bool update)
{
    _datetime.tm_mday = day;
    if (update)
    {
        _curDateTime =  mktime(&_datetime);
        _updateDateTime();
    }
    DT_DEBUG("%s\r\n", strDate());
}

// ------------------------------------------------------------------------------------------------------------------
void DateTime::setMonth(int month, bool update)
{
    _datetime.tm_mon = month;
    if (update)
    {
        _curDateTime =  mktime(&_datetime);
        _updateDateTime();
    }
    DT_DEBUG("%s\r\n", strDate());
}

// ------------------------------------------------------------------------------------------------------------------
void DateTime::setYear(uint16_t year, bool update)
{
    if (year > 1899)                                                                                                     // Year start at 1900 and the given value MUST be an increment the number from 1900
    {
        DT_DEBUG("%d: year MUST be an increment of 1900...\r\n", year);
        _isLeapYear = _checkLeapYear(year);                                                                             // Checks if it's a leap year
        year -= 1900;                                                                                                   // Update the given year's value, if needed;
        DT_DEBUG("Year's value becomes %d\r\n", year);
    }
    else
    {
        _isLeapYear = _checkLeapYear(year + 1900);                                                                      // Checks if it's a leap year
    }

    _datetime.tm_year = year;                                                                                           // Update the Year in struct tm
    while(year > 999)
    {
        year -= 1000;                                                                                                   // Avoiding modulo calculation
    }
    while(year >  99)
    {
        year -=  100;                                                                                                   // for the `short year`
    }
    _shortYear  = year;                                                                                                 // Stores the short format
    if (update)
    {
        _curDateTime =  mktime(&_datetime);
        _updateDateTime();
    }
    DT_DEBUG("%s\r\n", strDate());
}

// ------------------------------------------------------------------------------------------------------------------
void DateTime::setDate(uint8_t day, eMonth month, uint16_t year, bool update)
{
    int mon = month - 1;
    setYear(year, false);
    setMonth(mon, false);
    setDay(day, update);
    DT_DEBUG("%s, Day of week: %d, Week number: %d\r\n", strDate(), _datetime.tm_wday, _weekNumber);
}

// ------------------------------------------------------------------------------------------------------------------
bool DateTime::setWeekSetting(uint8_t weekSetting)
{
    if ((weekSetting == (uint8_t) WeekSetting_ISO)   ||
       (weekSetting == (uint8_t) WeekSetting_ISO_2) ||
       (weekSetting == (uint8_t) WeekSetting_US)    ||
       (weekSetting == (uint8_t) WeekSetting_ISLAMIC))
    {
        _weekSetting = (eWeekSetting) weekSetting;
    }
    else
    {
        DT_DEBUG("Unknown Setting (%d)\r\n", weekSetting);
        return false;
    }

    #if (DEBUG_DATETIME_ENABLE != 0)
    switch (_weekSetting)
    {
        case 0:
            DT_DEBUG("weekSettin: %d -> ISO\r\n", weekSetting);
            break;

        case 1:
            DT_DEBUG("weekSettin: %d -> ISO-2\r\n", weekSetting);
            break;

        case 2:
            DT_DEBUG("weekSettin: %d -> US\r\n", weekSetting);
            break;

        case 3:
            DT_DEBUG("weekSettin: %d -> ARABIC\r\n", weekSetting);
            break;

        default:
            break;
    }
    #endif
    return true;
}

// ------------------------------------------------------------------------------------------------------------------
bool DateTime::isInTheSameWeek(uint8_t dayOfMonth)
{
    uint8_t fDOW = (uint8_t) firstDayOfWeek();                                                                          // Get first day of the week according to calendar week setting
    int8_t dDiff = dayOfMonth - _datetime.tm_mday;                                                                      // Calculate the difference between given day and today
    int8_t next  = _datetime.tm_wday + dDiff;                                                                           // Get the next day of week without modulo
    if (dDiff >= 0)                                                                                                      // If it's a positive difference
    {
        if (_datetime.tm_wday < fDOW)                                                                                    // ... and the current day of week is less than the day of the begining of the week
        {
            return (next < fDOW);                                                                                       // we will be still in the same week if the next day of week is less than the day odf the begining of the week
        }
        else                                                                                                            // ... and if the current day of week is greater than the day of the begining of the week
        {
            return (next < (fDOW + 7));                                                                                 // we need to add 7 days to the day of the begining of the week to be able to check if the next day is still in the same week
        }
    }
    else                                                                                                                // If the difference is a negative value, the algo is almost the same...
    {
        if (_datetime.tm_wday > fDOW)
        {
            return (next >= fDOW);
        }
        else
        {
            return (next >= (((int8_t)fDOW) - 7));
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void DateTime::calculateTimeFromUTC(const DateTime &utc, int8_t utcHourOffset, int8_t utcMinuteOffset)
{
    setTime(utc.hour(), utc.minute(), utc.second(), false);
    setDate(utc.day(),  utc.month(),  utc.year(),   true);

    DT_DEBUG("UTC Date and Time: %s, %s\r\n", utc.strDate(), utc.strTime());
    DT_DEBUG("Hour Offset: %d, Minutes Offset: %d\r\n", utcHourOffset, utcMinuteOffset);

    updateTimeFromOffset(utcHourOffset, utcMinuteOffset);
}


// ------------------------------------------------------------------------------------------------------------------
void DateTime::updateTimeFromOffset(int8_t hourOffset, int8_t minuteOffset)
{
    if (minuteOffset >= 0)
    {
        incrMinute(minuteOffset, false);
    }
    else
    {
        decrMinute(-minuteOffset, false);
    }

    if (hourOffset >= 0)
    {
        incrHour(hourOffset, true);
    }
    else
    {
        decrHour(-hourOffset, true);
    }
}


/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
bool DateTime::_checkLeapYear(uint16_t year)
{
    if ((year == 1900) || (year == 2100) || (year == 2200) || (year == 2300))                                            // Some tests are made here to avoid MODULO calculation
    {
        return false;                                                                                                   // 1900, 2100, 2200 and 2300 aren't Leap year.
    }
    if (year == 2400)
    {
        return true;                                                                                                    // Year 2400 is a leap year
    }
    if (year > 1900 && year < 2400)
    {
        return ((year & 0x0003) == 0);                                                                                  // Between 1900 and 2100, all 4 years are leap year
    }
    return ((((year & 0x0003) == 0) && (year % 100 != 0)) || (year % 400 == 0));                                        // for other years, we'll check the leap year with the full formula
}

// ------------------------------------------------------------------------------------------------------------------
void DateTime::_updateDateTime()
{
    struct tm* tmpTime = localtime(&_curDateTime);
    if (tmpTime != NULL)
    {
        _datetime.tm_hour   = tmpTime->tm_hour;
        _datetime.tm_isdst  = tmpTime->tm_isdst;
        _datetime.tm_mday   = tmpTime->tm_mday;
        _datetime.tm_min    = tmpTime->tm_min;
        _datetime.tm_mon    = tmpTime->tm_mon;
        _datetime.tm_sec    = tmpTime->tm_sec;
        _datetime.tm_wday   = tmpTime->tm_wday;
        _datetime.tm_yday   = tmpTime->tm_yday;
        _datetime.tm_year   = tmpTime->tm_year;

        _isLeapYear = _checkLeapYear(_datetime.tm_year + 1900);
        _weekNumber         = _processWeekNumber(&_datetime);
    }
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t DateTime::_getMonthIndex(eMonth month)
{
    switch (month)
    {
        case January:   return 0;
        case February:  return 1;
        case March:     return 2;
        case April:     return 3;
        case May:       return 4;
        case June:      return 5;
        case July:      return 6;
        case August:    return 7;
        case September: return 8;
        case October:   return 9;
        case November:  return 10;
        case December:  return 11;
        default:        return 0;
    }
}

// ------------------------------------------------------------------------------------------------------------------
uint8_t DateTime::_processWeekNumber(tm_t* tm)
{
    if (tm == NULL)
    {
        return 0;
    }                                                                                                                   // The calculation:
    uint8_t wOffset = 11;                                                                                               // 11 because tm_yday is 0 to 365 (instead of 1 to 366).
    mktime(tm);                                                                                       					// 1. For ISO & ISO-2
    uint8_t weekday = (tm->tm_wday == 0) ? 7 : tm->tm_wday; // <- Basic DayOfWeek needed for the algo                   //    weekNumber = (DayOfYear + 10 - DayOfWeek) / 7;
    uint8_t weekNbr = 0;                                                                                                // 2. For US & ISLAMIC
                                                                                                                        //    weekNumber = (DayOfYear + 13 - DayOfWeek) / 7;
    switch (_weekSetting)                                                                                                // where DayOfYear is [1..366] and DayOfWeek is [1..7]
    {
        case WeekSetting_US:                                                                                            // For US
            wOffset = 14;                                                                                               // - week Offset: 14 because tm_yday is 0 to 365.
            weekday = (++weekday < 8) ? weekday : (weekday - 7);                                                        // - DayOfWeek: (Adding 1) % 7 because Sunday is the first day of week Sun=1, Mon=2, ...
            break;                                                                                                      //
        case WeekSetting_ISLAMIC:                                                                                       // For ISLAMIC (ARABIC)
            weekday = ((weekday + 2) < 8) ? (weekday + 2) : (weekday - 5);                                              // - DayOfWeek: (Adding 2) % 7 because Sunday is the first day of week Sat=1, Sun=2, Mon=3, ...
            wOffset = 14;                                                                                               // - week Offset: 14 because tm_yday is 0 to 365.
            break;
        default:
            break;
    }
    weekNbr = (tm->tm_yday + wOffset - weekday) / 7;                                                                    // Calculation for weekNbr

                                                                                                                        // Now deal with special cases:
    if (weekNbr == 0)                                                                                                    // A) If calculated week is zero, then it is part of the last week of the previous year.
    {                                                                                                                   //    - We need to find out if there are 53 weeks in previous year.
        struct tm lastDay;                                                                                              //    - Unfortunately to do so we have to call mktime again to get the information we require.
        memset(&lastDay, 0, sizeof(lastDay));
        lastDay.tm_mday   = 31;                                                                                         //    - Here we can use a slight cheat - reuse this function!
        lastDay.tm_mon    = 11;                                                                                         //      (This won't end up in a loop, because there's no way week will be zero again with these
        lastDay.tm_year   = tm->tm_year - 1;                                                                            //       values).
        lastDay.tm_hour   = 12;                                                                                         //     - We set time to sometime during the day (midday seems to make sense)
        weekNbr = _processWeekNumber(&lastDay);                                                                         //       so that we don't get problems with daylight saving time.
    }                                                                                                                   //
    else if (weekNbr == 53 && (_weekNumber == WeekSetting_ISO || _weekSetting == WeekSetting_ISO_2))                     // B) If calculated week is 53, then we need to determine if there really are 53 weeks in
    {
                                                                                                                        //    current year or if this is actually week one of the next year.
                                                                                                                        //    - We need to find out if there really are 53 weeks in this year...
                                                                                                                        //    - There must be 53 weeks in the year if :
                                                                                                                        //      a) it ends on Thurs (year also starts on Thurs, or Wed on leap year).
                                                                                                                        //      b) it ends on Friday and starts on Thurs (a leap year).
        int lastDay = tm->tm_wday + 31 - tm->tm_mday;                                                                   // In order not to call mktime again, we can work this out from what we already know!
        if (lastDay == 5)                                                                                                // Last day of the year is Friday
        {                                                                                                               // How many days in the year?
            int daysInYear = tm->tm_yday + 32 - tm->tm_mday;                                                            // add 32 because yday is 0 to 365
            if (daysInYear < 366)                                                                                        // If 365 days in year, then the year started on Friday
            {
                weekNbr = 1;                                                                                            // so there are only 52 weeks, and this is week one of next year.
            }
        }
        else if (lastDay != 4)                                                                                           // Last day is NOT Thursday
        {
            weekNbr = 1;                                                                                                // This must be the first week of next year
        }                                                                                                               // Otherwise we really have 53 weeks!
    }

    else if (weekNbr == 54 && (_weekNumber == WeekSetting_ISO || _weekSetting == WeekSetting_ISO_2))                     // C) If calculated 54 and we are in ISO or ISO_2
    {
        weekNbr = 1;                                                                                                    //    - The real week number is 1
    }
    DT_DEBUG("week number: %d\r\n", _weekNumber);
    return weekNbr;
}

#endif  // #if (USE_CALENDAR != 0)
