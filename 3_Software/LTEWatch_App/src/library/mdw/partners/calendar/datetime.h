/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    datetime.h
 *
 * @addtogroup Calendar
 * \{
 *
 * @class   DateTime
 * @brief   Contains the informations about date and time.
 *
 * # DateTime
 *
 * This class handles everything about the local date and time. It increments and decrements the
 * date and time as often as the Calendar needs it.
 *
 * @author  Patrice RUDAZ
 * @date    July 2018
 ********************************************************************************************************************/
#pragma once


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <nrf.h>
#include <platform-config.h>
#include <time.h>

#if (USE_CALENDAR != 0)
#include "calendar-config.h"


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* NAMESPACE Declaration                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
namespace cal 
{
    
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* Class Declaration                                                                                            */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    class DateTime
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Day of Week enum type.
         ************************************************************************************************************/
        typedef enum Sunday
        {
            Sunday              = 0,
            Monday              = 1,
            Tuesday             = 2,
            Wednesday           = 3,
            Thursday            = 4,
            Friday              = 5,
            Saturday            = 6,
            TopEnumDayOfWeek    = 7
        } eDayOfWeek;

        /********************************************************************************************************//**
         * @brief   Month enum type.
         ************************************************************************************************************/
        typedef enum
        {
            UnknownMonth        = 0,
            January             = 1,
            February            = 2,
            March               = 3,
            April               = 4,
            May                 = 5,
            June                = 6,
            July                = 7,
            August              = 8,
            September           = 9,
            October             = 10,
            November            = 11,
            December            = 12,
            TopEnumMonth        = 13
        } eMonth;

        /********************************************************************************************************//**
         * @brief   Week Setting's enum type.
         *
         * Some explanation can be found on https://en.wikipedia.org/wiki/Week
         ************************************************************************************************************/
        typedef enum
        {
            WeekSetting_ISO     = 0,            ///< @brief The first week includes the first Thursday of the year and the first day of the week is Monday.
            WeekSetting_ISO_2   = 1,            ///< @brief The first week includes 1. january and the first day of the week is Monday.
            WeekSetting_US      = 2,            ///< @brief The first week includes 1. january and the first day of the week is Sunday.
            WeekSetting_ISLAMIC = 3,            ///< @brief The first week includes 1. january and the first day of the week is Saturday.
            WeekSetting_TopEnum = 4
        } eWeekSetting;

        typedef struct tm tm_t;

        /********************************************************************************************************//**
         * @brief   Initializes the components needed by this class
         *
         * @param   hotReset    To know if a `reset` has occured at runtime...
         *
         * @warning Must be called before any other method of this class
         ************************************************************************************************************/
        void                initialize(bool hotReset = false);

        /********************************************************************************************************//**
         * @brief   Retrieves the second set for the current time
         *
         * This method can be used to know the second of the current time. The value is retrieved in
         * `uint32_t` type.
         *
         * \returns The second of the current time as `uint32_t`
         ************************************************************************************************************/
        inline time_t       dateTimeInSeconds() const           { return _curDateTime; }

        /********************************************************************************************************//**
         * @brief   Retrieves the second set for the current time
         *
         * This method can be used to know the second of the current time. The value is retrieved in
         * `uint32_t` type.
         *
         * \returns The second of the current time as `uint32_t`
         ************************************************************************************************************/
        inline uint8_t      second() const                      { return _datetime.tm_sec; }

        /********************************************************************************************************//**
         * @brief   Retrieves the minute set for the current time
         *
         * This method can be used to know the minute of the current time. The value is retrieved in
         * `uint32_t` type.
         *
         * \returns The minute of the current time as `uint32_t`
         ************************************************************************************************************/
        inline uint8_t      minute() const                      { return _datetime.tm_min; }

        /********************************************************************************************************//**
         * @brief   Retrieves the hour set for the current time
         *
         * This method can be used to know the hour of the current time. The value is retrieved in
         * `uint32_t` type.
         *
         * \returns The hour of the current time as `uint32_t`
         ************************************************************************************************************/
        inline uint8_t      hour() const                        { return _datetime.tm_hour; }

        /********************************************************************************************************//**
         * @brief   Retrieves the day of week  of the current date
         *
         * This method retrieves the current day of week. The value is retrieved in `eDayOfWeek` type.
         *
         * \returns The day of week of the current date as `eDayOfWeek`
         ************************************************************************************************************/
        inline eDayOfWeek   dayOfWeek() const                   { return (DateTime::eDayOfWeek) _datetime.tm_wday; }

        /********************************************************************************************************//**
         * @brief   Retrieves the first day of the week according to the calendar week setting.
         *
         * This method retrieves the first day of the week according to the week setting (ISO, ISO-2, US, ARABIC) of
         * the calendar. The value is retrieved in `eDayOfWeek` type.
         *
         * \returns The first day of the week as `eDayOfWeek`
         ************************************************************************************************************/
        eDayOfWeek          firstDayOfWeek() const;


        /********************************************************************************************************//**
         * @brief   Retrieves the day set  of the current date
         *
         * This method retrieves the current day. The value is retrieved in `uint32_t` type.
         *
         * \returns The day of the current date as `uint32_t`
         ************************************************************************************************************/
        inline uint8_t      day() const                         { return _datetime.tm_mday; }

        /********************************************************************************************************//**
         * @brief   Retrieves the month set  of the current date
         *
         * This method retrieves the current month. The value is retrieved in `eMonth` type.
         *
         * \returns The month of the current date as `eMonth`
         ************************************************************************************************************/
        eMonth              month() const;

        /********************************************************************************************************//**
         * @brief   Retrieves the year  of the current date
         *
         * This method retrieves the current year. The value is retrieved in `uint16_t` type.
         *
         * \returns The year of the current date as `uint16_t`
         ************************************************************************************************************/
        inline uint16_t     year() const                        { return _datetime.tm_year + 1900; }

        /********************************************************************************************************//**
         * @brief   Retrieves the short format of the year of the current date
         *
         * This method retrieves the current year in short format. If we are in 2013, the short format
         * will be `13`. The value is retrieved in `uint8_t` type.
         *
         * \returns The year of the current date in short format as `uint8_t`
         ************************************************************************************************************/
        inline uint8_t      shortYear() const                   { return _shortYear; }

        /********************************************************************************************************//**
         * @brief   Retrieves the number of days in current year until today
         *
         * This method retrieves the number of days passed in the current year until today. The value is retrieved as an
         * `uint16_t`.
         *
         * \returns The number of days of the current year until today as an `uint16_t` value.
         ************************************************************************************************************/
        inline uint16_t     daysInYear() const                  { return _datetime.tm_yday; }

        /********************************************************************************************************//**
         * @brief   Retrieves whether or not the current year is a leap year
         *
         * \returns `true` if the current year is a leap year and `false` otherwise.
         ************************************************************************************************************/
        inline bool         isLeapYear() const                  { return _isLeapYear; }

        /********************************************************************************************************//**
         * @brief   Retrieves whether or not we are in daylight saving time.
         *
         * \returns `true` means we are in daylight saving time and `false` we are not
         ************************************************************************************************************/
        inline bool         isDaylightSavingTime() const        { return _datetime.tm_isdst; }

        /********************************************************************************************************//**
         * @brief   Retrieves the week number for the current date and time
         *
         * This method retrieves the week number for the current date and time.
         * The value is retrieved in `uint8_t` type.
         *
         * \returns The week number for the current date and time as `uint8_t`
         ************************************************************************************************************/
        inline uint8_t      weekNumber() const                  { return _weekNumber; }

        /********************************************************************************************************//**
         * @brief   Retrieves a pointer on the date & time structure.
         *
         * This method retrieves a pointer on the structure which maintains the information about the current date and
         * time.
         *
         * \returns A pointer on the date and time's structure `struct tm`.
         ************************************************************************************************************/
        inline const tm_t &dateTime() const                    { return _datetime; }

        /********************************************************************************************************//**
         * @brief   Retrieves the date set for the current date and time
         *
         * This method can be used to know the current date. The date is retrieved in
         * DD.MM.YYY format.
         *
         * \returns The actual date given in DD.MM.YYYY format.
         ************************************************************************************************************/
        char*               strDate() const;

        /********************************************************************************************************//**
         * @brief   Retrieves the time set for the current date and time
         *
         * This method can be used to know the current time. The time is retrieved in hh:mm:ss format.
         *
         * \returns The actual date given in hh:mm:ss format.
         ************************************************************************************************************/
        char*               strTime() const;

        /********************************************************************************************************//**
         * @brief   Retrieves the current date and time as a string.
         *
         * This method can be used to know the current date & time. The date & time is retrieved in a default format.
         * The default format `Www Mmm dd hh:mm:ss yyyy` where
         * - `Www` is the weekday,
         * - `Mmm` the month (in letters),
         * - `dd` the day of the month,
         * - `hh:mm:ss` the time
         * - `yyyy` the year.
         *
         * \returns The actual date given in `Www Mmm dd hh:mm:ss yyyy` format.
         ************************************************************************************************************/
        inline char*        strDateTime() const                 { return asctime(&_datetime); }

        /********************************************************************************************************//**
         * @brief   Set the second of the time
         *
         * Takes the given `second` as the new value for the second of the current time.
         *
         * @param   second  The new value for the second.
         ************************************************************************************************************/
        void        setSecond(int second, bool update = true);

        /********************************************************************************************************//**
         * @brief   Increment the second of the time by the given value
         *
         * Adds `incrVal` second(s) of the current time.
         *
         * @param   incrVal The amount of second to add
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        void        incrSecond(uint8_t incrVal, bool update)    { setSecond(_datetime.tm_sec + incrVal, update); }

        /********************************************************************************************************//**
         * @brief   Remove some seconds to the time
         *
         * Removes `decrVal` second(s) of the current time.
         *
         * @param   decrVal The amount of seconds to remove
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        void        decrSecond(uint8_t decrVal, bool update)    { setSecond(_datetime.tm_sec - decrVal, update); }

        /********************************************************************************************************//**
         * @brief   Decrement the second of the time of 1 second
         *
         * Removes 1 second of the current time.
         ************************************************************************************************************/
        void        oneSecondLess();

        /********************************************************************************************************//**
         * @brief   Increment the second of the time of 1 second
         *
         * Adds 1 second of the current time.
         ************************************************************************************************************/
        void        oneSecondMore();

        /********************************************************************************************************//**
         * @brief   Set the minute of the time
         *
         * Takes the given `minute` as the new value for the minute of the current time.
         *
         * @param   minute  The new value for the minute.
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        void        setMinute(int minute, bool update = true);

        /********************************************************************************************************//**
         * @brief   Increment the minute of the time of 1 minute
         *
         * Adds `incrVal` minute of the current time.
         *
         * @param   incrVal The amount of minute to add
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        inline void incrMinute(uint8_t incrVal, bool update)    { setMinute(_datetime.tm_min + incrVal, update); }

        /********************************************************************************************************//**
         * @brief   Decrement the minute of the time of 1 minute
         *
         * Removes `decrVal` minute(s) of the current time.
         *
         * @param   decrVal The amount of minutes to remove
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        inline void decrMinute(uint8_t decrVal, bool update)    { setMinute(_datetime.tm_min - decrVal, update); }

        /********************************************************************************************************//**
         * @brief   Set the hour of the time
         *
         * Takes the given `hour` as the new value for the hour of the current time.
         *
         * @param   hour    The new value for the hour.
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        void        setHour(int hour, bool update = true);

        /********************************************************************************************************//**
         * @brief   Increment the hour of the time of 1 hour
         *
         * Adds `incrVal` hour of the current time.
         *
         * @param   incrVal The amount of hours to add
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        inline void incrHour(uint8_t incrVal, bool update)      { setHour(_datetime.tm_hour + incrVal, update); }

        /********************************************************************************************************//**
         * @brief   Decrement the hour of the time of 1 hour
         *
         * Removes `decrVal` hour(s) of the current time.
         *
         * @param   decrVal The amount of hours to remove
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        inline void decrHour(uint8_t decrVal, bool update)      { setHour(_datetime.tm_hour - decrVal, update); }

        /********************************************************************************************************//**
         * @brief   Set the time of the time
         *
         * Takes the given `second`, `minute` and `hour` as the new value for the time of the current
         * time.
         *
         * @param   hour    The new value for the hour.
         * @param   minute  The new value for the minute.
         * @param   second  The new value for the second.
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        void        setTime(uint8_t hour, uint8_t minute, uint8_t second, bool update);

        /********************************************************************************************************//**
         * @brief   Set the day of the time
         *
         * Takes the given `day` as the value for the day of the current date.
         *
         * @param   day     The new value for the local day
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        void        setDay(int day, bool update = true);

        /********************************************************************************************************//**
         * @brief   Increment the day of the time of 1 day
         *
         * Adds `incrVal` day of the current time.
         *
         * @param   incrVal The amount of days to add
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        inline void incrDay(uint8_t incrVal, bool update)       { setDay(_datetime.tm_mday + incrVal, update); }

        /********************************************************************************************************//**
         * @brief   Decrement the day of the time of 1 day
         *
         * Removes `decrVal` day(s) of the current time.
         *
         * @param   decrVal The amount of days to remove
         * @param   update If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        inline void decrDay(uint8_t decrVal, bool update)       { setDay(_datetime.tm_mday - decrVal, update); }

        /********************************************************************************************************//**
         * @brief   Set the month of the local date
         *
         * This method set the month for the local date.
         * Takes the given `month` as the value for the month of the current date.
         *
         * @param   month   The new value for the local month
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        void        setMonth(int month, bool update = true);

        /********************************************************************************************************//**
         * @brief   Set the month of the local date
         *
         * This method set the month for the local date.
         * Takes the given `month` as the value for the month of the current date.
         *
         * @param   month   The new value from the eMonth enumeration for the local month
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        inline void setMonth(eMonth month, bool update)         { setMonth(_getMonthIndex(month), update); }

        /********************************************************************************************************//**
         * @brief   Increment the month of the time of 1 month
         *
         * Adds `incrVal` month of the current time.
         *
         * @param   incrVal The amount of months to add
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        inline void incrMonth(uint8_t incrVal, bool update)     { setMonth(_datetime.tm_mon + incrVal, update); }

        /********************************************************************************************************//**
         * @brief   Decrement the month of the time of 1 month
         *
         * Removes `decrVal` month(s) of the current time.
         *
         * @param   decrVal The amount of month to remove
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        inline void decrMonth(uint8_t decrVal, bool update)     { setMonth(_datetime.tm_mon - decrVal, update); }

        /********************************************************************************************************//**
         * @brief   Set the year of the time
         *
         * Takes the given `year` as the new year for the current time.
         *
         * @param   year    The new value for the local year
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        void        setYear(uint16_t year, bool update = true);

        /********************************************************************************************************//**
         * @brief   Increment the year of the time of 1 year
         *
         * Adds `incrVal` year of the current time.
         *
         * @param   incrVal The amount of year to add
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        inline void incrYear(uint8_t incrVal, bool update)      { setYear(_datetime.tm_year + incrVal, update); }

        /********************************************************************************************************//**
         * @brief   Decrement the year of the time of 1 year
         *
         * Removes `decrVal` year(s) of the current time.
         *
         * @param   decrVal The amount of year to remove
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        inline void decrYear(uint8_t decrVal, bool update)      { setYear(_datetime.tm_year - decrVal, update); }

        /********************************************************************************************************//**
         * @brief   Set the date of the time
         *
         * Takes the given `day`, `month` and `year` as the new date for the current time.
         *
         * @param   day     The day of the date to be set
         * @param   month   The month of the date to be set
         * @param   year    The year of the date to be set
         * @param   update  If set to `TRUE`, the current time is updated
         ************************************************************************************************************/
        void        setDate(uint8_t day, eMonth month, uint16_t year, bool update);

        /********************************************************************************************************//**
         * @brief   Set the week settings
         *
         * Takes the given week setting as the setting for the week number calculation.
         * Possible values: 0 = ISO, 1 = ISO-2, 2 = US, 3 = Arabic
         ************************************************************************************************************/
        bool        setWeekSetting(uint8_t weekSetting);

        /********************************************************************************************************//**
         * @brief   Checks if the given day is in the same week as current day.
         *
         * Checks if the given day is in the same week as current day, according to the week setting's parameter.
         *
         * @param   dayOfMonth  The day to check !
         ************************************************************************************************************/
        bool        isInTheSameWeek(uint8_t dayOfMonth);

        /********************************************************************************************************//**
         * @brief   Update Date and Time from UTC
         *
         * Calculate the new time from the given UTC DateTime with the given offsets
         *
         * @param   utc             Reference to UTC DateTime instance
         * @param   utcHourOffset   Hour Offset from time regarding to UTC
         * @param   utcMinuteOffset Minute Offset from time regarding to UTC
         ************************************************************************************************************/
        void        calculateTimeFromUTC(const DateTime &utc, int8_t utcHourOffset, int8_t utcMinuteOffset);

        /********************************************************************************************************//**
         * @brief   Update Date and Time from the offset relative to the UTC time
         *
         * \detail  Calculate the new time according to the UTC and the different offsets.
         *
         * @param   hourOffset      Hour Offset from time regarding to UTC
         * @param   minuteOffset    Minute Offset from time regarding to UTC
         ************************************************************************************************************/
        void        updateTimeFromOffset(int8_t hourOffset, int8_t minuteOffset);


    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Attributes
        time_t          _curDateTime;
        tm_t            _datetime;
        uint8_t         _shortYear;
        uint8_t         _weekNumber;
        bool            _isLeapYear;
        eWeekSetting    _weekSetting;

        // Methods
        /************************************************************************************************************
         * @brief   Checks of the given year is a leap year or not
         ************************************************************************************************************/
        bool            _checkLeapYear(uint16_t year);

        /************************************************************************************************************
         * @brief   Update the local tTime and tDate variables
         ************************************************************************************************************/
        void            _updateDateTime();

        /************************************************************************************************************
         * @brief   Retrieves 28/29, 30 or 31 according to the current month and `_isLeapYear`.
         ************************************************************************************************************/
        uint8_t         _getMonthIndex(eMonth month);

        /************************************************************************************************************
         * @brief   Calculates the week number using the new algorithm.
         ************************************************************************************************************/
        uint8_t         _processWeekNumber(tm_t* tm);
    };

}  // Namepspace cal

#endif  // #if (USE_CALENDAR != 0)

/** \} */   // Group: Calendar
