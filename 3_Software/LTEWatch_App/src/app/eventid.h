 /****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * All rights reserved.
 *
 * @file    eventid.h
 * @class   EventId
 * @brief   Class to specify the available ID for events
 *
 * This class is responsible to distribute to every event used in the project a different event's
 * identifier.
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2014-04-12
 *
 * @author  Patrice Rudaz (patrice.rudaz@hevs.ch)
 * @date    February 2022
 ********************************************************************************************************************/
#pragma once


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class EventId
{
public:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC SECTION                                                                                               */
    /*                                                                                                              */
    /* ************************************************************************************************************ */

    ///< Enumeration of event's id available in the current application
    typedef enum
    {
        UnknownEvent = 0,               ///< Unknown event (not initialized).
        DefaultEvent,                   ///< Event ID to specify the default event.

        SpiEvent,                       ///< Event ID to identify an event due to an SPI activity...
        SpiNotifyEvent,                 ///< Event ID to notify observers on data ready over SPI.
        SpiErrorEvent,                  ///< Event ID to identify an event dur to a specific error on SPI...

        UartEvent,                      ///< Event ID to identify an event due to an UART activity...
        UartOnDataReadyEvent,           ///< Event ID to identify when some data are ready to be read from UART buffer
        
        UsbdEvent,                      ///< Event ID to identify an event due to an USB activity...
        UsbdOnDataReadyEvent,           ///< Event ID to identify when some data are ready to be read from USB buffer
        
        CalendarEvent,                  ///< Event ID related to a change in date and time

        MotorEvent,                     ///< Event ID dealing with motor's movement
        MotorInitEvent,                 ///< Event ID dealing with motor's movement
        MotorEndEvent,                  ///< Event ID dealing with motor's movement

        NotificationEvent,              ///< Event ID dealing with notification events
        NotificationDisplayEvent,       ///< Event ID dealing with notification Display Timeout events

        ButtonEvent,                    ///< Event ID dealing with button's event

        evNone, evInitial, evTimeout, evDefault, evInt, ///< General Event ID
        // LTE/MQTT events
        evConnected, evDisconnected, evPushMsg,         ///< Event ID dealing with MQTT
        // Generic
        evError,                                        ///< Error Event ID

        TopEnumEventId                  ///< Last entry in the enumeration
    } eEventId;

    #if defined(DEBUG_NRF_USER)
        /********************************************************************************************************//**
         * @brief   Returns the string which describes the given event.
         *
         * @param   state    The identifier of the event to translate in string
         ************************************************************************************************************/
        static const char* to_string(eEventId eventId)
        {
            switch(eventId)
            {
                case DefaultEvent               : return "DefaultEvent"; 
                case SpiEvent                   : return "SpiEvent";
                case SpiNotifyEvent             : return "SpiNotifyEvent";
                case SpiErrorEvent              : return "SpiErrorEvent";
                case UartEvent                  : return "UartEvent";
                case UartOnDataReadyEvent       : return "UartOnDataReadyEvent";
                case UsbdEvent                  : return "UsbdEvent";
                case UsbdOnDataReadyEvent       : return "UsbdOnDataReadyEvent";
                case CalendarEvent              : return "CalendarEvent";
                case MotorEvent                 : return "MotorEvent";
                case MotorInitEvent             : return "MotorInitEvent";
                case MotorEndEvent              : return "MotorEndEvent";
                case NotificationEvent          : return "NotificationEvent";
                case NotificationDisplayEvent   : return "NotificationDisplayEvent";
                case ButtonEvent                : return "ButtonEvent";
                case evNone                     : return "evNone";
                case evInitial                  : return "evInitial";
                case evTimeout                  : return "evTimeout";
                case evDefault                  : return "evDefault";
                case evInt                      : return "evInt";
                case evConnected                : return "evConnected";
                case evDisconnected             : return "evDisconnected";
                case evPushMsg                  : return "evPushMsg";
                case evError                    : return "evError";
                case TopEnumEventId             : return "TopEnumEventId";
                default:                    return "UnknownEvent";
            }
        }
    #endif // #if defined(DEBUG_NRF_USER)
};

        