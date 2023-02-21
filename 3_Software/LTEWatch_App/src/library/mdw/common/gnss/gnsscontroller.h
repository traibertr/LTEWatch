#ifndef GNSS_ONCE
#define GNSS_ONCE

#include <zephyr.h>

#include <isubject.hpp>
#include "ignssobserver.h"
#include <picoireactive.h>
#include <picoevent.h>

#include "config/gnss-config.h"
#include "platform-config.h"
#include "ubxlib_custom/u_gnss_pos_date.h"

class GnssController : public PicoIReactive, public ISubject<IGnssObserver, CONFIG_GNSS_MAX_OBSERVERS>
{
public:

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

    virtual ~GnssController() {};

    void initialize();
    void connect();
    void disconnect();
    void update();

    bool processEvent(PicoEvent* event);

    inline void setLNAMode(uint8_t lna_mode) {getInstance()->_lnaMode = lna_mode;}

    uLocation_t getGnssData();
    gDate_t     getGnssDate();

// Singleton
    static GnssController* getInstance();

    static const char* state_to_string(eGnssState state);

// Callback for asynchronous message reception.
    static void callback(uDeviceHandle_t devHandle, const uGnssMessageId_t *pMessageId,
                     int32_t errorCodeOrLength, void *pCallbackParam);

    static void posDateCallback(uDeviceHandle_t gnssHandle,
                            int32_t errorCode,
                            int32_t latitudeX1e7,
                            int32_t longitudeX1e7,
                            int32_t altitudeMillimetres,
                            int32_t radiusMillimetres,
                            int32_t speedMillimetresPerSecond,
                            int32_t svs,
                            int64_t timeUtc,
                            uint16_t yearVal,
                            uint8_t  monthVal,
                            uint8_t  dayVal,
                            uint8_t  hourVal,
                            uint8_t  minuteVal,
                            uint8_t  secondVal);

    char latLongToBits(int32_t thingX1e7, int32_t *pWhole, int32_t *pFraction);
private:
// Singleton pattern
    GnssController();

    void initHW();
    void uninitHW();

    void pushEvent(PicoEvent* event);

    eGnssState _state;

    typedef enum GNSSvents_ {evWaitTimeout=200,
                              evAskData,
                              evReadData,
                              evNext
                              } GNSSEvents;

    PicoEvent                      _evWaitData;
    PicoEvent                      _evAskData;
    PicoEvent                      _evReadData;
    PicoEvent                      _evNextState;

//ubxlib relative
    static uLocation_t              _gnssData;              ///< Data receive from the GNSS module

    static uDeviceCfg_t            _gDeviceCfg;             ///< DEVICE i.e. module/chip configuration: in this case a GNSS
    static uDeviceHandle_t         _gnssHandle;             ///< Device callback: in this case the GNSS
    static uGnssTransportHandle_t  _transportHandle;        ///< Gnss transport callback
    static uGnssCfgVal_t*          _pCfgValList;            ///< Pointer to the GNSS list of configuration values
    static uint32_t                _keyId;                  ///< GNSS config identification key

    static size_t                   _gMessageCount;
    static uGnssMessageId_t         _messageId;
    bool                            _GnssConnect;

    static gDate_t                  _gnssDate;
    static int32_t                  _length;
    static int32_t                  _gErrorCode;
    static bool                     _dataValid;
    uint8_t                         _lnaMode;

    //Convert a lat/long into a whole number and a bit-after-the-decimal-point that can be printed by a version of printf()
    void _notifyObservers(eGnssState state);

    // Print a single configuration value nicely.
    void printCfgVal(uGnssCfgVal_t *pCfgVal);

    // Print an array of uGnssCfgVal_t.
    void printCfgValList(uGnssCfgVal_t *pCfgValList, size_t numItems);

    // Print out the position contained in a UBX-NAV_PVT message
    void printPosition(const char *pBuffer, size_t length);

    // Print out the time contained in a UBX-NAV_PVT message
    void printTimeAndDate(const char *pBuffer, size_t length = (size_t) 32);
};

#endif