/****************************************************************************************************************//**
 * Copyright (C) MSE Lausanne Hes-so VAUD/WAADT, Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2023
 * Created by Tristan Traiber
 * All rights reserved.
 *
 * @file    gnsscontroller.h
 *
 * @addtogroup GNSS
 * @{
 *
 * @class   GnssController
 * @brief   Class to control LTEWatch GNSS receiver, u-blox `MAX-M10S`
 *
 * Initial author: Tristan Traiber
 * Creation date: 25.01.2023
 *
 * @author  Tristan Traiber (tristan.traiber@master.hes-so.ch)
 * @date    January 2023
 ********************************************************************************************************************/
#ifndef GNSS_ONCE
#define GNSS_ONCE

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <zephyr.h>

#include <isubject.hpp>
#include "ignssobserver.h"
#include <picoireactive.h>
#include <picoevent.h>

#include "config/gnss-config.h"
#include "platform-config.h"
#include "ubxlib_custom/u_gnss_pos_date.h"

/* ************************************************************************************************************ */
/*                                                                                                              */
/* Class Declaration                                                                                            */
/*                                                                                                              */
/* ************************************************************************************************************ */
class GnssController : public PicoIReactive, public ISubject<IGnssObserver, CONFIG_GNSS_MAX_OBSERVERS>
{
public:
/* ******************************************************************************************************** */
/* PUBLIC DECLARATION SECTION                                                                               */
/* ******************************************************************************************************** */
typedef enum GNSSSTATE
    {
        ST_IDLE,
        ST_ASK_DATA,
        ST_WAIT,
        ST_CHECK,
        ST_RECEIVE,
        ST_CONNECTED,
        ST_DISCONNECTED
    } eGnssState;

//ubxlib relative
    struct gDate_t{
        uint16_t    year;
        uint8_t     month;
        uint8_t     day;
        uint8_t     hour;
        uint8_t     min;
        uint8_t     sec;
    };

    /********************************************************************************************************//**
    * @brief  Initialize GnssController instance
    ************************************************************************************************************/
    void initialize(void);

    /********************************************************************************************************//**
    * @brief  Config and connect to the GNSS receiver
    ************************************************************************************************************/
    void connect(void);

    /********************************************************************************************************//**
    * @brief  Disconnect from the GNSS receiver
    ************************************************************************************************************/
    void disconnect(void);

    /********************************************************************************************************//**
    * @brief  Ask GNSS receiver for current position
    ************************************************************************************************************/
    void update(void);

    /********************************************************************************************************//**
    * @brief   Implements state machine behavior
    *
    * @param   event    Event identifier
    *
    * @retval `true`    When event processed successfully
    * @retval `false`   Otherwise
    ************************************************************************************************************/
    bool processEvent(PicoEvent* event);

    /********************************************************************************************************//**
    * @brief   Config GNSS receiver LNA mode
    *
    * @param   lna_mode    GNSS LNA mode
    *                      2 Bypass LNA
    *                      1 Low LNA
    *                      0 Normal LNA
    ************************************************************************************************************/
    inline void setLNAMode(uint8_t lna_mode) {getInstance()->_lnaMode = lna_mode;}

    /********************************************************************************************************//**
    * @brief    Get recieved GNSS data
    *
    * @retval   GNSS data (uLocation_t)
    *
    * \details typedef struct {
    *               uLocationType_t type;
    *               int32_t latitudeX1e7;
    *               int32_t longitudeX1e7;
    *               int32_t altitudeMillimetres;
    *               int32_t radiusMillimetres;
    *               int32_t speedMillimetresPerSecond;
    *               int32_t svs;
    *               int64_t timeUtc;
    *           } uLocation_t;
    ************************************************************************************************************/
    uLocation_t getGnssData(void);

    /********************************************************************************************************//**
    * @brief    Get recieved GNSS date and time
    *
    * @retval   GNSS data (gDate_t)
    *   struct gDate_t{
    *       uint16_t    year;
    *       uint8_t     month;
    *       uint8_t     day;
    *       uint8_t     hour;
    *       uint8_t     min;
    *       uint8_t     sec;
    *   };
    ************************************************************************************************************/
    gDate_t     getGnssDate(void);

// Singleton
    /********************************************************************************************************//**
    * @brief    Creat/use singleton instance of GNSS Controller
    *
    * @return   GNSS Controller instance pointer
    ************************************************************************************************************/
    static GnssController* getInstance();

    /********************************************************************************************************//**
    * @brief  Callback function for asynchronous message reception of the GNSS receiver
    *
    * @param   devHandle         I2C driver device instance
    * @param   pMessageId        Message identifier
    * @param   errorCodeOrLength Error Code or length of the message recieved
    * @param   pCallbackParam    Message reception buffer
    ************************************************************************************************************/
    static void callback(uDeviceHandle_t devHandle, const uGnssMessageId_t *pMessageId,
                     int32_t errorCodeOrLength, void *pCallbackParam);

 uLocationType_t type; /**< the location mechanism that was used. */
    int32_t latitudeX1e7; /**< latitude in ten millionths of a degree. */
    int32_t longitudeX1e7; /**< longitude in ten millionths of a degree. */
    int32_t altitudeMillimetres; /**< altitude in millimetres; if the
                                      altitude is unknown INT_MIN will be
                                      returned. */
    int32_t radiusMillimetres; /**< radius of location in millimetres;
                                    if the radius is unknown -1
                                    will be returned. */
    int32_t speedMillimetresPerSecond; /**< the speed (in millimetres
                                            per second); if the speed
                                            is unknown INT_MIN will be
                                            returned. */
    int32_t svs;                       /**< the number of space vehicles
                                            used in establishing the
                                            location.  If the number of
                                            space vehicles is unknown or
                                            irrelevant -1 will be
                                            returned. */
    int64_t timeUtc; /**< the UTC time at which the location fix was made;
                          if this is not available -1 will be returned. */

    /********************************************************************************************************//**
    * @brief  Callback function to register GNSS position, date and time data received
    *
    * @param    gnssHandle                  GNSS instance
    * @param    errorCode                   Error code
    * @param    latitudeX1e7                Latitude in ten millionths of a degree
    * @param    longitudeX1e7               Longitude in ten millionths of a degree
    * @param    altitudeMillimetres         Altitude in millimetres
    * @param    radiusMillimetres           Radius of location in millimetres
    * @param    speedMillimetresPerSecond   The speed (in millimetres per second)
    * @param    svs                         The number of space vehicles used for establishing the location
    * @param    timeUtc                     The UTC time at which the location fix was made
    * @param    yearVal                     UTC year value
    * @param    monthVal                    UTC month value
    * @param    dayVal                      UTC day value
    * @param    hourVal                     UTC hour value
    * @param    minuteVal                   UTC minute value
    * @param    secondVal                   UTC second value
    ************************************************************************************************************/
    static void posDateCallback(uDeviceHandle_t gnssHandle,
                            int32_t     errorCode,
                            int32_t     latitudeX1e7,
                            int32_t     longitudeX1e7,
                            int32_t     altitudeMillimetres,
                            int32_t     radiusMillimetres,
                            int32_t     speedMillimetresPerSecond,
                            int32_t     svs,
                            int64_t     timeUtc,
                            uint16_t    yearVal,
                            uint8_t     monthVal,
                            uint8_t     dayVal,
                            uint8_t     hourVal,
                            uint8_t     minuteVal,
                            uint8_t     secondVal);

    /********************************************************************************************************//**
    * @brief  Convert latitude and longitude values in bits
    *
    * @param    thingX1e7   Latitude or longitude value to convert
    * @param    pWhole      Whole part of the convertersion
    * @param    pFraction   Fraction part of the conversion
    *
    * @retval   `+`         If converted value >= 0
    * @retval   `-`         If converted value < 0
    ************************************************************************************************************/
    char latLongToBits(int32_t thingX1e7, int32_t *pWhole, int32_t *pFraction);
private:
/* ******************************************************************************************************** */
/* PRIVATE DECLARATION SECTION                                                                              */
/* ******************************************************************************************************** */
    // Singleton pattern
    GnssController();
    virtual ~GnssController() {};

    /********************************************************************************************************//**
    * @brief  Initialize GNSS receiver hardware (Power ON, I2C, etc...)
    ************************************************************************************************************/
    void initHW(void);

    /********************************************************************************************************//**
    * @brief  Unitialize GNSS receiver hardware (Power ON, I2C, etc...)
    ************************************************************************************************************/
    void uninitHW(void);

    /********************************************************************************************************//**
    * @brief Push event to the executive framework
    *
    * @param   event     event identifier
    ************************************************************************************************************/
    void pushEvent(PicoEvent* event);

    eGnssState _state;  ///< Current state of the GNSS controller

    typedef enum GNSSvents_ {evWaitTimeout=200,
                              evAskData,
                              evReadData,
                              evNext
                              } GNSSEvents;

    PicoEvent                      _evWaitData;             ///< Wait for GNSS answear event
    PicoEvent                      _evAskData;              ///< Ask GNSS for position event
    PicoEvent                      _evReadData;             ///< Read received data event
    PicoEvent                      _evNextState;            ///< Next state event

//ubxlib relative
    static uLocation_t              _gnssData;              ///< Data receive from the GNSS module

    static uDeviceCfg_t            _gDeviceCfg;             ///< DEVICE i.e. module/chip configuration: in this case a GNSS
    static uDeviceHandle_t         _gnssHandle;             ///< Device callback: in this case the GNSS
    static uGnssTransportHandle_t  _transportHandle;        ///< Gnss transport callback
    static uGnssCfgVal_t*          _pCfgValList;            ///< Pointer to the GNSS list of configuration values
    static uint32_t                _keyId;                  ///< GNSS config identification key

    static size_t                   _gMessageCount;         ///< Number of messages
    static uGnssMessageId_t         _messageId;             ///< Message identifier
    bool                            _GnssConnect;           ///< GNSS connection status flag

    static gDate_t                  _gnssDate;              ///< GNSS recieved date data
    static int32_t                  _length;                ///< Message length
    static int32_t                  _gErrorCode;            ///< Error code
    static bool                     _dataValid;             ///< Data valide status flag
    uint8_t                         _lnaMode;               ///< GNSS receiver LNA mode

    /********************************************************************************************************//**
    * @brief   Notify observer about GNSS controler events
    *
    * @param   state        GNSS Controller event state
    ************************************************************************************************************/
    void _notifyObservers(eGnssState state);

    /********************************************************************************************************//**
    * @brief   Print a single configuration value nicely
    *
    * @param   pCfgVal      GNSS receiver coinfiguration value
    ************************************************************************************************************/
    void printCfgVal(uGnssCfgVal_t *pCfgVal);

    /********************************************************************************************************//**
    * @brief   Print an array of uGnssCfgVal_t
    *
    * @param   pCfgValList  GNSS receiver list of configuration values
    * @param   numItems     Number of list's items
    ************************************************************************************************************/
    void printCfgValList(uGnssCfgVal_t *pCfgValList, size_t numItems);

    /********************************************************************************************************//**
    * @brief   Print out the position contained in a UBX-NAV_PVT message
    *
    * @param   pBuffer      Message buffer
    * @param   length       Message length in `Bytes`
    ************************************************************************************************************/
    void printPosition(const char *pBuffer, size_t length);

    /********************************************************************************************************//**
    * @brief    Print out the time contained in a UBX-NAV_PVT message
    *
    * @param   pBuffer      Message buffer
    * @param   length       Message length in `Bytes`
    ************************************************************************************************************/
    void printTimeAndDate(const char *pBuffer, size_t length = (size_t) 32);
};

#endif