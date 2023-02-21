/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    calendar.h
 *
 * \defgroup Calendar
 * \{
 *
 * @class   Calendar
 * @brief   Class to handles the current Date and Time as a calendar...
 *
 * # Calendar
 *
 * This class is the class where everything from the current date and time of the watch is aggregate.
 *
 * # About the version
 *
 * This version implements the horlogical concept of inhibition to improve the time accuracy of the watch. 
 * The value of the inhibition and its interval are both read from the UICR registers.
 *
 * @author  Patrice RUDAZ (patrice.rudaz@hevs.ch)
 * @date    July 2018
 ********************************************************************************************************************/
#pragma once


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <platform-config.h>

#include "isubject.hpp"

#include "datetime.h"
#include "config/calendar-config.h"
#include "interface/calendarobserver.h"
#include "calendarevent.h"

#if (USE_CALENDAR != 0)

#if defined(__cplusplus)
extern "C" 
{
#endif

    namespace cal
    {
        /********************************************************************************************************//**
         * @brief    callback method for SoftDevice's calendar timer.
         *
         * Implementation of the callback method for SoftDevice's calendar timer. 
         * The SoftDevice's app_timer sent an event when its timer reaches the counter's value 
         * specified when calling the `timerStart()` method.
         ************************************************************************************************************/
        void calendarTimerEvent(struct k_timer *timer_id);

        /********************************************************************************************************//**
         * @brief    callback method for calendar's inhibition timer.
         *
         * Implementation of the callback method for calendar's inhibition timer. 
         * The SoftDevice's app_timer sent an event when its timer reaches the counter's value 
         * specified when calling the `timerStart()` method.
         ************************************************************************************************************/
        void calendarInhibitionTimerEvent(struct k_timer *timer_id);
    }

#if defined(__cplusplus)
}
#endif


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
    class Calendar : public ISubject<CalendarObserver, CALENDAR_MAX_OBSERVERS>
    {
        friend void calendarTimerEvent(struct k_timer *timer_id);
        friend void calendarInhibitionTimerEvent(struct k_timer *timer_id);

    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* CONSTRUCTOR SECTION                                                                                      */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        Calendar();
        ~Calendar();

        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Initializes the components needed by this class.
         *
         * @param   localDateTime   A pointer to an instance of DateTime class which is the local date and time.
         *
         * @warning Must be called before any other method of this class
         * \note    Requires existent instances of classes listed in parameter list
         ************************************************************************************************************/
        void                        initialize();

        /********************************************************************************************************//**
         * @brief   Register a new Date and Time in the controller class.
         *
         * This method MUST be used to register a new Time. In fact, this method just
         * save the pointer to the instance of a Date and Time in an array to keep a reference.
         *
         * @param   dateTime        Pointer to the new Date and Time to register.
         ************************************************************************************************************/
        void                       registerDateTime(DateTime* dateTime);

        /********************************************************************************************************//**
         * @brief   Enable the calendar.
         *
         * This method turns ON the calendar. The date and time of the calendar have to be set using the dedicated
         * method such as `Calendar::setDate` or `Calendar::setTime`.
         *
         * This functionality is useful to start the current time precisely on a specific time tick or an external
         * trigger. You can image setting up all the parameters of any date and time and start this new calendar after
         * receiving a tick form an external component.
         ************************************************************************************************************/
        void                        startCalendar();

        /********************************************************************************************************//**
         * @brief   Stops the time.
         *
         * The time won't pass so quickly anymore!
         *
         * This method is used to stop the current time. Very useful to fine-tune a clock. It stops the timer used to
         * tick the time.
         ************************************************************************************************************/
        void                        stopCalendar();

        /********************************************************************************************************//**
         * @brief   Retrieves the number of registered buttons
         ************************************************************************************************************/
        inline uint8_t              nbOfRegisteredDateTime()        { return _nbRegisteredDateTime; }

        /********************************************************************************************************//**
         * @brief   Indicates if the current time is enable or not.
         *
         * This method can be used to know if the calendar is active or not. It returns `FALSE` if
         * timer used to tick at 1 second has stopped and `FALSE` otherwise. In case of `FALSE`, it
         * means that the RTC hardware timer or the `app_timer` is not enable and the time is no
         * more updated.
         *
         * In fact, this method returns the value of the private parameter `_isCalActive` which
         * is BOOLEAN and store the state of the calendar.
         *
         ************************************************************************************************************/
        inline bool                 isActive()                      { return _isCalActive; }

        /********************************************************************************************************//**
         * @brief   Retrieves the second of the current time
         *
         * This method can be used to know the second of the time of the calendar identified by its identifier. The 
         * value is retrieved in `uint8_t` type.
         *
         * \returns The second of the time of the calendar identified by the given index as `uint8_t`
         ************************************************************************************************************/
        uint8_t                     second(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Retrieves the minute of the current time
         *
         * This method can be used to know the minute of the time of the calendar identified by its identifier. The
         * value is retrieved in `uint8_t` type.
         *
         * \returns The minute of the time of the calendar identified by the given index as `uint8_t`
         ************************************************************************************************************/
        uint8_t                     minute(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Retrieves the hour of the current time
         *
         * This method can be used to know the hour of the time of the calendar identified by its identifier. The
         * value is retrieved in `uint8_t` type.
         *
         * \returns The hour of the time of the calendar identified by the given index as `uint8_t`
         ************************************************************************************************************/
        uint8_t                     hour(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Retrieves the day of week of the current date
         *
         * This method retrieves the day of week of the calendar identified by its identifier. The value is retrieved
         * in `eDayOfWeek` type.
         *
         * \returns The day of week of the date of the calendar identified by the given index as `eDayOfWeek`
         ************************************************************************************************************/
        cal::DateTime::eDayOfWeek  dayOfWeek(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Retrieves the day of the current date
         *
         * This method retrieves the day of the calendar identified by its identifier. The value is retrieved in
         * `uint8_t` type.
         *
         * \returns The day of the date of the calendar identified by the given index as `uint8_t`
         ************************************************************************************************************/
        uint8_t                     day(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Retrieves the month of the current date
         *
         * This method retrieves the month of the calendar identified by its identifier. The value is retrieved in
         * `eMonth` type.
         *
         * \returns The month of the date of the calendar identified by the given index as `eMonth`
         ************************************************************************************************************/
        cal::DateTime::eMonth      month(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Retrieves the year of the current date
         *
         * This method retrieves the year of the calendar identified by its identifier. The value is retrieved in
         * `uint16_t` type.
         *
         * \returns The year of the date of the calendar identified by the given index as `uint16_t`
         ************************************************************************************************************/
        uint16_t                    year(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Retrieves `true` if the year is a leap year and `false` otherwise.
         *
         * This method retrieves the year of the calendar identified by its identifier. The value is retrieved in
         * `uint16_t` type.
         *
         * \returns The year of the current date of the calendar identified by the given index as `uint16_t`
         ************************************************************************************************************/
        bool                        isLeapYear(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Retrieves the short format of the year of the current date
         *
         * This method retrieves the year of the calendar identified by its identifier in short format. If we are in
         * 2013, the short format will be `13`. The value is retrieved in `uint8_t` type.
         *
         * \returns The year of the current date in short format as `uint8_t`
         ************************************************************************************************************/
        uint8_t                     shortYear(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Retrieves the pointer to the date and time structure of the given index
         *
         * This method retrieves the pointer to the date and time structure of the given index.
         *
         * \returns The pointer to the date and time structure of the given index.
         ************************************************************************************************************/
        cal::DateTime*              getDateTime(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Reset the second
         *
         * This method can be called to set the second back to 0.
         ************************************************************************************************************/
        void                        resetSecond(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Set a new value for the second
         *
         * This method can be called to set a new value to the second of the current calendar.
         *
         * @param   index           Index of the DateTime object `uint8_t`
         * @param   second          The new value for the second given as an `uint8_t`
         ************************************************************************************************************/
        void                        setSecond(uint8_t index, uint8_t second);

        /********************************************************************************************************//**
         * @brief   Increment the calendar of 1 minute
         *
         * This method can be called to add 1 minute to the current calendar.
         ************************************************************************************************************/
        void                        incrMinute(uint8_t index, uint8_t amount = 1);

        /********************************************************************************************************//**
         * @brief   Remove some minutes to the calendar
         *
         * This method can be called to remove 1 minute to the current calendar.
         ************************************************************************************************************/
        void                        decrMinute(uint8_t index, uint8_t amount = 1);

        /********************************************************************************************************//**
         * @brief   Set a new value for the minute
         *
         * This method can be called to set a new value to the minute of the current calendar.
         *
         * @param   minute The new value for the minute given as an `uint8_t`
         ************************************************************************************************************/
        void                        setMinute(uint8_t index, uint8_t minute);

        /********************************************************************************************************//**
         * @brief   Set a new value for the hour
         *
         * This method can be called to set a new value to the hour of the current calendar.
         *
         * @param   hour The new value for the hour given as an `uint8_t`
         ************************************************************************************************************/
        void                        setHour(uint8_t index, uint8_t hour);

        /********************************************************************************************************//**
         * @brief   Enable calendar
         *
         * This method enables calendar changes
         ************************************************************************************************************/
        inline void                 enableCalendarForSetting()      { _setSettingMode(true); }

        /********************************************************************************************************//**
         * @brief   Disable calendar
         *
         * This method disables calendar changes
         ************************************************************************************************************/
        inline void                 disableCalendarForSetting()     { _setSettingMode(false); }

        /********************************************************************************************************//**
         * @brief   Increment the second of the current time.
         *
         * On each call to this method, the current date and time is increased of one second. Minutes, hours, day,
         * month and year are also updated if necessary. Once this is done, the observer this object is notified of
         * this fact.
         *
         * The observer's notification will raise an event to the main state machine to handle the change of the
         * local date and time.
         ************************************************************************************************************/
        void                        updateSecond();

        /********************************************************************************************************//**
         * @brief   Execute the calendar inhibition.
         *
         * On each call to this method, the measured difference of the current used crystal and its theoretical
         * value is added/removed.
         *
         * The observer's notification will raise an event to the main state machine to handle the change of the
         * local date and time.
         ************************************************************************************************************/
        void                        inhibitCrystal();
        
    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Attributes
        bool                        _isCalActive;                                                                       // to know if the calendar is running
        bool                        _calendarSettingActive;                                                             // Disactivate the changes on the Date/Time during time settings
        cal::DateTime*              _dateTime[APP_NB_DATE_TIME_MAX];                                                    // Reference to the current date and time
        uint8_t                     _nbRegisteredDateTime;                                                              // Number of registered DateTime objects
        int32_t                     _inhibitionValue;                                                                   // Amount of time to add at each interval, expressed in number 0f LFCK ticks
        uint32_t                    _inhibitionInterval;                                                                // The inhibition value must added to the LFCK at this interval [s]
        uint32_t                    _inhibitionCounter;

        // Methods
        /************************************************************************************************************
         * @brief   Changes the `_calendarSettingActive` to the given value
         *
         * @param   enable  The new value for the `_calendarSettingActive` property
         ************************************************************************************************************/
        inline void                 _setSettingMode(bool enable)      { _calendarSettingActive = enable; }

        /************************************************************************************************************
         * @brief   Initialized the `app_timer` used for the calendar
         ************************************************************************************************************/
        void                        _timerInit();

        /************************************************************************************************************
         * @brief   Update all Data en Time sets of one second...
         ************************************************************************************************************/
        void                        _onSecondChanged();

        /************************************************************************************************************
         * @brief   notify all registered observers about calendar fired second
         ************************************************************************************************************/
        void                        _notifySecond();

    };

}   // Namespace cal

#endif  // (USE_CALENDAR != 0)

/** \} */   // Group: Calendar
