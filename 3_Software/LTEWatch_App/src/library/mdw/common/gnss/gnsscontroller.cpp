/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics.
 * Created by Patrice Rudaz
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "gnsscontroller.h"
#include <picoxf.h>
#include <spinlock.h>

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#ifndef CONFIG_GNSS_LOG_LEVEL
    #define CONFIG_GNSS_LOG_LEVEL            0
#endif

#if (CONFIG_GNSS_LOG_LEVEL != 0)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(gnss_controller, CONFIG_GNSS_LOG_LEVEL);

    #define GNSS_DEBUG(args...)              LOG_DBG(args)
    #define GNSS_ERROR(args...)              LOG_ERR(args)
    #define GNSS_INFO(args...)               LOG_INF(args)
    #define GNSS_WRN(args...)                LOG_WRN(args)
#else
    #define GNSS_DEBUG(...)                  {(void) (0);}
    #define GNSS_ERROR(...)                  {(void) (0);}
    #define GNSS_INFO(...)                   {(void) (0);}
    #define GNSS_WRN(...)                    {(void) (0);}
#endif

/* **************************************************************************************************************** */
/* Namespace Declaration                                                                                            */
/* **************************************************************************************************************** */

uLocation_t             GnssController::_gnssData;
uDeviceCfg_t            GnssController::_gDeviceCfg;
uDeviceHandle_t         GnssController::_gnssHandle;
uGnssTransportHandle_t  GnssController::_transportHandle;
uGnssCfgVal_t*          GnssController::_pCfgValList;
uint32_t                GnssController::_keyId;
size_t                  GnssController::_gMessageCount;
uGnssMessageId_t        GnssController::_messageId;
GnssController::gDate_t GnssController::_gnssDate;
int32_t                 GnssController::_length;
bool                    GnssController::_dataValid;
int32_t                 GnssController::_gErrorCode;

/* **************************************************************************************************************** */
/* Static Declaration                                                                                               */
/* **************************************************************************************************************** */
static struct k_spinlock lock;

static const struct gpio_dt_spec gpsRstN = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), gps_rst_gpios);

// Callback function for non-blocking API.
void GnssController::posDateCallback(uDeviceHandle_t gnssHandle,
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
                        uint8_t  secondVal)
{
    if (svs > 0)
    {
        _gnssHandle                         = gnssHandle;
        _gErrorCode                         = errorCode;
        _gnssData.latitudeX1e7              = latitudeX1e7;
        _gnssData.longitudeX1e7             = longitudeX1e7;
        _gnssData.altitudeMillimetres       = altitudeMillimetres;
        _gnssData.radiusMillimetres         = radiusMillimetres;
        _gnssData.speedMillimetresPerSecond = speedMillimetresPerSecond;
        _gnssData.svs                       = svs;
        _gnssData.timeUtc                   = timeUtc;
        _gnssDate.year                      = yearVal;
        _gnssDate.month                     = monthVal;
        _gnssDate.day                       = dayVal;
        _gnssDate.hour                      = hourVal;
        _gnssDate.min                       = minuteVal;
        _gnssDate.sec                       = secondVal;
    }

    //uUbxProtocolUint64Decode
}

/************************************************************************************************************/
/* Constructor | Destructor                                                                                 */
/************************************************************************************************************/
// ----------------------------------------------------------------------------------------------------------
GnssController::GnssController()
{
    _gnssHandle     = NULL;
    _pCfgValList    = NULL;
    _keyId          = 0;
    _gMessageCount  = 0;
    _messageId      = {};

    _gnssData.latitudeX1e7              = 0; /**< latitude in ten millionths of a degree. */
    _gnssData.longitudeX1e7             = 0; /**< longitude in ten millionths of a degree. */
    _gnssData.altitudeMillimetres       = 0; /**< altitude in millimetres; if the altitude is unknown INT_MIN will be returned. */
    _gnssData.radiusMillimetres         = 0; /**< radius of location in millimetres; if the radius is unknown -1 will be returned. */
    _gnssData.speedMillimetresPerSecond = 0; /**< the speed (in millimetres per second); if the speed is unknown INT_MIN will be returned. */
    _gnssData.svs                       = 0; /**< the number of space vehicles used in establishing the location.  If the number of space vehicles is unknown or irrelevant -1 will be returned. */
    _gnssData.timeUtc                   = 0; /**< the UTC time at which the location fix was made; if this is not available -1 will be returned. */
    _gnssDate.year                      = 0;
    _gnssDate.month                     = 0;
    _gnssDate.day                       = 0;
    _gnssDate.hour                      = 0;
    _gnssDate.min                       = 0;
    _gnssDate.sec                       = 0;
}

// ----------------------------------------------------------------------------------------------------------
void GnssController::initialize(void)
{
    /* --------- Init Battery Level Enable Output Pin --------- */
    int err = gpio_pin_configure_dt(&gpsRstN, GPIO_OUTPUT_INACTIVE);

	if (err != 0) {
		GNSS_ERROR("Configuring GPS /RST pin failed: %d\n", err);
	}

    _gDeviceCfg = {
    .deviceType = U_DEVICE_TYPE_GNSS,
    .deviceCfg = {
        .cfgGnss = {
            .moduleType = GNSS_MODULE_TYPEM10,                // GNSS chip is a M10 module
            .pinEnablePower = GNSS_PWR_PIN,  // GNSS power enable pin (on/off)
            .pinDataReady = -1,     // Not used
            .i2cAddress = GNSS_I2C_ADDRESS
        },
    },
        .transportType = U_DEVICE_TRANSPORT_TYPE_I2C,
        .transportCfg = {
            .cfgI2c = {
                .i2c = GNSS_I2C_ENABLE,
                .pinSda = U_CFG_APP_PIN_GNSS_SDA,
                .pinScl = U_CFG_APP_PIN_GNSS_SCL,
                // .clockHertz = <I2C_FREQ>,
                // Set clockHertz value if the
                // I2C bus uses a different clock frequency
                // than #U_PORT_I2C_CLOCK_FREQUENCY_HERTZ
                // .alreadyOpen = true
                // Sset alreadyOpen to true if you
                // are already using this I2C HW block,
                // with the native platform APIs,
                // elsewhere in your application code,
                // and you would like the ubxlib code
                // to use the I2C HW block WITHOUT
                // [re]configuring it,
                // if alreadyOpen is set to true then
                // pinSda, pinScl and clockHertz will
                // be ignored.
            },
        },
    };

    _evWaitData.setId(evWaitTimeout);
    _evWaitData.setDnd(true);
    _evWaitData.setTarget(this);
    _evWaitData.setDelay(GNSS_RESPONSE_WAIT_PERIODE);

    _evAskData.setId(evAskData);
    _evAskData.setDnd(true);
    _evAskData.setTarget(this);
    _evAskData.setDelay(0);

    _evReadData.setId(evReadData);
    _evReadData.setDnd(true);
    _evReadData.setTarget(this);
    _evReadData.setDelay(0);

    _evNextState.setId(evNext);
    _evNextState.setDnd(true);
    _evNextState.setTarget(this);
    _evNextState.setDelay(0);
}

// ----------------------------------------------------------------------------------------------------------
void GnssController::initHW(void)
{
    int32_t returnCode;
    //int32_t numValues = 0;

    if (_gDeviceCfg.deviceCfg.cfgGnss.moduleType >= U_GNSS_MODULE_TYPE_M9)
    {
        // Initialise the APIs with I2C interface
        uPortI2cInit();
        uDeviceInit();

        uGnssSetUbxMessagePrint(_gnssHandle, false);

        // Open the device
        returnCode = uDeviceOpen(&_gDeviceCfg, &_gnssHandle);
        if (returnCode == 0)
        {
            _GnssConnect = true;
            GNSS_INFO("Succeed to connect to GNSS");

            // Set configuration of the GNSS device

            // See exemple : https://github.com/u-blox/ubxlib/blob/master/example/gnss/cfg_val_main.c
        }
        else
        {
            _GnssConnect = false;
            GNSS_ERROR("Fail to connect to GNSS -> return code %d", returnCode);
        }
    }
    else
    {
        GNSS_DEBUG("The CFGVALXXX API is only supported on M9 modules and later.");
    }

    #if(RESET_GNSS_AFTER_CONNECT != 0)
        GNSS_INFO("Reset Receiver");
        char *pBuffer = (char *) pUPortMalloc(4 + U_UBX_PROTOCOL_OVERHEAD_LENGTH_BYTES);
        int32_t length = uUbxProtocolEncode(0x06, 0x04, NULL, 0, pBuffer);
        uGnssMsgSend(_gnssHandle, pBuffer, length);
        uPortFree(pBuffer);
    #endif

    if (_lnaMode == 2)
    {
        uGnssCfgValSet(_gnssHandle, U_GNSS_CFG_VAL_KEY_ID_HW_ANT_CFG_VOLTCTRL_L, 1, U_GNSS_CFG_VAL_TRANSACTION_NONE, U_GNSS_CFG_VAL_LAYER_RAM);
        uGnssCfgValSet(_gnssHandle, U_GNSS_CFG_VAL_KEY_ID_HW_ANT_SUP_SWITCH_PIN_U1, 7, U_GNSS_CFG_VAL_TRANSACTION_NONE, U_GNSS_CFG_VAL_LAYER_RAM);
        uGnssCfgValSet(_gnssHandle, U_GNSS_CFG_VAL_KEY_ID_HW_ANT_CFG_SHORTDET_L, 1, U_GNSS_CFG_VAL_TRANSACTION_NONE, U_GNSS_CFG_VAL_LAYER_RAM);
        uGnssCfgValSet(_gnssHandle, U_GNSS_CFG_VAL_KEY_ID_HW_ANT_CFG_SHORTDET_POL_L, 0, U_GNSS_CFG_VAL_TRANSACTION_NONE, U_GNSS_CFG_VAL_LAYER_RAM);
        uGnssCfgValSet(_gnssHandle, U_GNSS_CFG_VAL_KEY_ID_HW_ANT_CFG_PWRDOWN_L, 1, U_GNSS_CFG_VAL_TRANSACTION_NONE, U_GNSS_CFG_VAL_LAYER_RAM);
        uGnssCfgValSet(_gnssHandle, U_GNSS_CFG_VAL_KEY_ID_HW_ANT_CFG_PWRDOWN_POL_L, 0, U_GNSS_CFG_VAL_TRANSACTION_NONE, U_GNSS_CFG_VAL_LAYER_RAM);
        uGnssCfgValSet(_gnssHandle, U_GNSS_CFG_VAL_KEY_ID_HW_ANT_CFG_RECOVER_L, 0, U_GNSS_CFG_VAL_TRANSACTION_NONE, U_GNSS_CFG_VAL_LAYER_RAM);

        if (uGnssCfgValSet(_gnssHandle, U_GNSS_CFG_VAL_KEY_ID_HW_ANT_SUP_SHORT_PIN_U1, 5, U_GNSS_CFG_VAL_TRANSACTION_NONE, U_GNSS_CFG_VAL_LAYER_RAM) == 0)
        {
                    GNSS_INFO("Set U_GNSS_CFG_VAL_KEY_ID_HW_ANT_SUP_SWITCH_PIN_U1");
        }
        else
        {
                    GNSS_ERROR("Unable to set U_GNSS_CFG_VAL_KEY_ID_HW_ANT_SUP_SWITCH_PIN_U1");
        }
    }
    #define TEST 0
#if(TEST)
else
{
#endif
    if (uGnssCfgValSet(_gnssHandle, CFG_HW_RF_LNA_MODE, _lnaMode, U_GNSS_CFG_VAL_TRANSACTION_NONE, U_GNSS_CFG_VAL_LAYER_RAM) == 0)
    {
        GNSS_INFO("Successfully Set GNSS LNA mode: %d", _lnaMode);
    }
    else
    {
        GNSS_ERROR("Unable to set GNSS LNA mode!\n");
    }
#if(TEST)
}
#endif

    // Reading a single value from the configuration settings works
    // in the same way, but this time you must either specify the full
    // U_GNSS_CFG_VAL_KEY_ID_XXX value (or write in the 32-bit key ID from
    // the GNSS interface description):

    uint8_t configVal = 10;
    if (uGnssCfgValGet(_gnssHandle, CFG_HW_RF_LNA_MODE, (void *) &configVal, sizeof(configVal), U_GNSS_CFG_VAL_LAYER_RAM) == 0)
    {
        GNSS_INFO("Check GNSS LNA mode value: %d", configVal);
    }
    else
    {
        GNSS_ERROR("Unable to check GNSS LNA mode value...");
    }


    // Make sure we have a 3D fix to get altitude as well
    if(uGnssCfgSetFixMode(_gnssHandle, U_GNSS_FIX_MODE_3D) == 0)
    {
        GNSS_INFO("Set 3D fix on Succeed (to get altitude)");
    }
    else
    {
        GNSS_ERROR("Set 3D fix on fail (to get altitude)");
    }

}
// ----------------------------------------------------------------------------------------------------------
void GnssController::uninitHW(void)
{
    // Note: we don't power the device down here in order
    // to speed up testing; you may prefer to power it off
    // by setting the second parameter to true.
    uDeviceClose(_gnssHandle, false);

    // Calling these will also deallocate all the handles that
    // were allocated above.
    uDeviceDeinit();
    uPortI2cDeinit();
}

// ----------------------------------------------------------------------------------------------------------
void GnssController::connect(void)
{
    GNSS_DEBUG("Connect GNSS Module");
    initHW();
    _notifyObservers(ST_CONNECTED);
}

// ----------------------------------------------------------------------------------------------------------
void GnssController::disconnect(void)
{
    GNSS_DEBUG("Disconnect GNSS Module");
    uninitHW();
    _notifyObservers(ST_DISCONNECTED);
}

// ----------------------------------------------------------------------------------------------------------
void GnssController::update(void)
{
    if (_GnssConnect == true)
    {
        GNSS_DEBUG("GNSS Update start...");
        _state = ST_IDLE;
        pushEvent(&_evAskData);
    }
    else
    {
        GNSS_ERROR("GNSS not connected -> Reconnect...");
        initHW();
    }
}
// ----------------------------------------------------------------------------------------------------------

bool GnssController::processEvent(PicoEvent* event)
{
    // Storing current store...
    eGnssState oldState = _state;
    static uDeviceHandle_t gnssHandle = _gnssHandle;

    static int32_t heapUsed;
    static int64_t startTime;
    static int64_t stopTimeMs;

    // State transition Switch
    switch (_state)
    {
        case ST_IDLE:
        {
            if (event->getId() == evAskData)
            {
                _state = ST_ASK_DATA;
            }
            break;
        }
        case ST_ASK_DATA:
        {
            if (event->getId() == evNext)
            {
                _state = ST_WAIT;
            }
            break;
        }
        case ST_WAIT:
        {
            if (event->getId() == evWaitTimeout)
            {
                _state = ST_CHECK;
            }
            break;
        }
        case ST_CHECK:
        {
            if (event->getId() == evNext)
            {
                _state = ST_WAIT;
            }
            else if (event->getId() == evReadData)
            {
                _state = ST_RECEIVE;
            }
            break;
        }
        case ST_RECEIVE:
        {
            if (event->getId() == evNext)
            {
                _state = ST_IDLE;
            }
            break;
        }
        default:
        {
            GNSS_ERROR("Unhandled Action State ! (%d)", _state);
            break;
        }
    }

    if (oldState != _state)
    {
        // Entry action Switch
        switch (_state)
        {
            case ST_IDLE:
            {
                GNSS_DEBUG("-> ST_IDLE");
                //Nothing to do...
                break;
            }
            case ST_ASK_DATA:
            {
                GNSS_DEBUG("-> ST_ASK_DATA");
                _gErrorCode = 0;

                //k_spinlock_key_t key = k_spin_lock(&lock);

                // Obtain the initial heap size
                heapUsed = uPortGetHeapFree();
                startTime = uPortGetTickTimeMs();
                stopTimeMs = startTime + U_GNSS_POS_TIMEOUT_SECONDS * 1000;
                startTime = uPortGetTickTimeMs();

                _gErrorCode = uGnssPosDateGetStart(gnssHandle, posDateCallback);

                //k_spin_unlock(&lock, key);

                _gErrorCode = 0xFFFFFFFF;
                pushEvent(&_evNextState);
                break;
            }
            case ST_WAIT:
            {
                //k_spinlock_key_t key = k_spin_lock(&lock);
                    _evWaitData.setDelay(GNSS_RESPONSE_WAIT_PERIODE);
                //k_spin_unlock(&lock, key);

                pushEvent(&_evWaitData);
                break;
            }
            case ST_CHECK:
            {
                if((_gErrorCode == (int32_t) 0xFFFFFFFF) && (uPortGetTickTimeMs() < stopTimeMs))
                {
                    pushEvent(&_evNextState);
                }
                else
                {
                    //k_spinlock_key_t key = k_spin_lock(&lock);
                        uGnssPosDateGetStop(gnssHandle);
                    //k_spin_unlock(&lock, key);

                    pushEvent(&_evReadData);
                }
                break;
            }
            case ST_RECEIVE:
            {
                GNSS_DEBUG("-> ST_RECEIVE");
                char prefix[2];
                int32_t whole[2];
                int32_t fraction[2];

                if(_gErrorCode != 0)
                {
                    if (_gErrorCode == U_ERROR_COMMON_TIMEOUT)
                    {
                    GNSS_ERROR("GNSS signal too weak...");
                    }
                    else
                    {
                        GNSS_DEBUG("asynchonous API received error code %d.", _gErrorCode);
                    }
                }
                GNSS_DEBUG("position establishment took %d second(s).", (int32_t) (uPortGetTickTimeMs() - startTime) / 1000);

                //k_spinlock_key_t key = k_spin_lock(&lock);
                    prefix[0] = latLongToBits(_gnssData.latitudeX1e7, &(whole[0]), &(fraction[0]));
                    prefix[1] = latLongToBits(_gnssData.longitudeX1e7, &(whole[1]), &(fraction[1]));
                //k_spin_unlock(&lock, key);

                GNSS_INFO("location %c%d.%07d/%c%d.%07d (radius %d metre(s)), %d metre(s) high,"
                            " moving at %d metre(s)/second, %d satellite(s) visible, time %d.",
                            prefix[0], whole[0], fraction[0], prefix[1], whole[1], fraction[1],
                            _gnssData.radiusMillimetres / 1000, _gnssData.altitudeMillimetres / 1000,
                            _gnssData.speedMillimetresPerSecond / 1000, _gnssData.svs, (int32_t) _gnssData.timeUtc);
                GNSS_WRN("paste this into a browser https://maps.google.com/?q=%c%d.%07d,%c%d.%07d",
                            prefix[0], whole[0], fraction[0], prefix[1], whole[1], fraction[1]);

                GNSS_INFO("Date : %d/%d/%d, Time : %2d:%2d:%2d (UTC)",_gnssDate.day,_gnssDate.month,_gnssDate.year,_gnssDate.hour,_gnssDate.min,_gnssDate.sec);

                //key = k_spin_lock(&lock);
                    _notifyObservers(ST_RECEIVE);
                //k_spin_unlock(&lock, key);

                #if(CONFIG_GNSS_LOG_LEVEL > 3)
                    int y = uGnssMsgReceiveStatStreamLoss(gnssHandle);
                    GNSS_DEBUG("%d byte(s) lost at the input to the ring-buffer during that test.", y);

                    heapUsed -= uPortGetHeapFree();
                    GNSS_DEBUG("we have leaked %d byte(s).", heapUsed);
                #endif
                pushEvent(&_evNextState);
                break;
            }
            default:
            {
                 GNSS_ERROR("Unhandled Action State ! (%d)", _state);
                break;
            }
        }
    }
    return true;
}

// ----------------------------------------------------------------------------------------------------------
GnssController* GnssController::getInstance(void)
{
    static GnssController instance;
    return &instance;
}

uLocation_t GnssController::getGnssData(void)
{
    return getInstance()->_gnssData;
}

GnssController::gDate_t GnssController::getGnssDate(void)
{
    return getInstance()->_gnssDate;
}

void GnssController::pushEvent(PicoEvent* event)
{
    PicoXF::getInstance()->pushEvent(event);
}

// ----------------------------------------------------------------------------------------------------------
void GnssController::_notifyObservers(GnssController::eGnssState state)
{
    for (uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != nullptr)
        {
            switch (state)
            {
                case GnssController::ST_CONNECTED:
                {
                    observer(i)->onConnected(this);
                    break;
                }
                case GnssController::ST_DISCONNECTED:
                {
                    observer(i)->onDisconnected(this);
                    break;
                }
                case GnssController::ST_RECEIVE:
                {
                    observer(i)->onReceive(this);
                    break;
                }
                default:
                    break;
            }
        }
    }
}

// ubxlib relative
// ----------------------------------------------------------------------------------------------------------
void GnssController::printCfgVal(uGnssCfgVal_t *pCfgVal)
{
    uGnssCfgValKeySize_t encodedSize = U_GNSS_CFG_VAL_KEY_GET_SIZE(pCfgVal->keyId);

    switch (encodedSize)
    {
        case U_GNSS_CFG_VAL_KEY_SIZE_ONE_BIT:
            if (pCfgVal->value) {
                GNSS_DEBUG("true");
            } else {
                GNSS_DEBUG("false");
            }
            break;
        case U_GNSS_CFG_VAL_KEY_SIZE_ONE_BYTE:
            GNSS_DEBUG("0x%02x", (uint8_t) pCfgVal->value);
            break;
        case U_GNSS_CFG_VAL_KEY_SIZE_TWO_BYTES:
            GNSS_DEBUG("0x%04x", (uint16_t) pCfgVal->value);
            break;
        case U_GNSS_CFG_VAL_KEY_SIZE_FOUR_BYTES:
            GNSS_DEBUG("0x%08x", (uint32_t) pCfgVal->value);
            break;
        case U_GNSS_CFG_VAL_KEY_SIZE_EIGHT_BYTES:
            // Print in two halves as 64-bit integers are not supported
            // by some printf() functions
            GNSS_DEBUG("0x%08x%08x", (uint32_t) (pCfgVal->value >> 32), (uint32_t) (pCfgVal->value));
            break;
        default:
            break;
    }
}

// ----------------------------------------------------------------------------------------------------------
void GnssController::printCfgValList(uGnssCfgVal_t *pCfgValList, size_t numItems)
{
    for (size_t x = 0; x < numItems; x++)
    {
        GNSS_DEBUG("%5d _keyId 0x%08x = ", x + 1, pCfgValList->keyId);
        printCfgVal(pCfgValList);
        GNSS_DEBUG("");
        pCfgValList++;
        // Pause every few lines so as not to overload logging
        // on some platforms
        if (x % 10 == 9)
        {
            uPortTaskBlock(20);
        }
    }
}

char GnssController::latLongToBits(int32_t thingX1e7, int32_t *pWhole, int32_t *pFraction)
{
    char prefix = '+';

    // Deal with the sign
    if (thingX1e7 < 0) {
        thingX1e7 = -thingX1e7;
        prefix = '-';
    }
    *pWhole = thingX1e7 / 10000000;
    *pFraction = thingX1e7 % 10000000;

    return prefix;
}

void GnssController::printPosition(const char *pBuffer, size_t length)
{
    char prefix[2] = {0};
    int32_t whole[2] = {0};
    int32_t fraction[2] = {0};

    if ((length >= 32) && (*(pBuffer + 21 + U_UBX_PROTOCOL_HEADER_LENGTH_BYTES) & 0x01))
    {
        _gnssData.longitudeX1e7 = uUbxProtocolUint32Decode(pBuffer + 24 + U_UBX_PROTOCOL_HEADER_LENGTH_BYTES);
        _gnssData.latitudeX1e7  = uUbxProtocolUint32Decode(pBuffer + 28 + U_UBX_PROTOCOL_HEADER_LENGTH_BYTES);
        prefix[0] = latLongToBits(_gnssData.longitudeX1e7, &(whole[0]), &(fraction[0]));
        prefix[1] = latLongToBits(_gnssData.latitudeX1e7, &(whole[1]), &(fraction[1]));
        GNSS_INFO("I am here: https://maps.google.com/?q=%c%d.%07d,%c%d.%07d",
                 prefix[1], whole[1], fraction[1], prefix[0], whole[0], fraction[0]);
        _dataValid = true;
    }
    else
    {
        _dataValid = false;
        GNSS_ERROR("Gnss can't get the position...");
    }
}

void GnssController::printTimeAndDate(const char *pBuffer, size_t length)
{
    if ((length >= 32) && (*(pBuffer + 11 + U_UBX_PROTOCOL_HEADER_LENGTH_BYTES) & 0x03))
    {
        _gnssDate.year  = uUbxProtocolUint16Decode(pBuffer + 4 + U_UBX_PROTOCOL_HEADER_LENGTH_BYTES);
        _gnssDate.month = (uint8_t) *(pBuffer + 6 + U_UBX_PROTOCOL_HEADER_LENGTH_BYTES);
        _gnssDate.day   = (uint8_t) *(pBuffer + 7 + U_UBX_PROTOCOL_HEADER_LENGTH_BYTES);
        _gnssDate.hour  = (uint8_t) *(pBuffer + 8 + U_UBX_PROTOCOL_HEADER_LENGTH_BYTES);
        _gnssDate.min   = (uint8_t) *(pBuffer + 9 + U_UBX_PROTOCOL_HEADER_LENGTH_BYTES);
        _gnssDate.sec   = (uint8_t) *(pBuffer + 10 + U_UBX_PROTOCOL_HEADER_LENGTH_BYTES);
        GNSS_INFO("Date : %d/%d/%d, Time : %2d:%2d:%2d (UTC)",_gnssDate.day,_gnssDate.month,_gnssDate.year,_gnssDate.hour,_gnssDate.min,_gnssDate.sec);
        _dataValid = true;
    }
    else
    {
        _dataValid = false;
        GNSS_ERROR("Gnss can't get the date and time...");
    }
}

// Callback for asynchronous message reception.
void GnssController::callback(uDeviceHandle_t devHandle, const uGnssMessageId_t *pMessageId,
                     int32_t errorCodeOrLength, void *pCallbackParam)
{
    char *pBuffer = (char *) pCallbackParam;

    (void) pMessageId;

    if (errorCodeOrLength >= 0)
    {
        // Read the message into our buffer and print it
        _length = uGnssMsgReceiveCallbackRead(devHandle, pBuffer, errorCodeOrLength);
        if (_length >= 0)
        {
            _gMessageCount++;
        }
        else
        {
            GNSS_ERROR("Empty or bad message received.");
        }
    }
}
