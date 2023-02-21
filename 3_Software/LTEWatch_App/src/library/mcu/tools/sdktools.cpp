/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "sdktools.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "debug-config.h"

/* **************************************************************************************************************** */
/* LOG CONFIG                                                                                                       */
/* **************************************************************************************************************** */
#if (DEBUG_SDKTOOLS_ENABLE == 1) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(sdktools);
    #define SDK_ERR(args...)                    LOG_ERR(args)
    #define SDK_WRN(args...)                    LOG_WRN(args)
    #define SDK_INF(args...)                    LOG_INF(args)
    #define SDK_DBG(args...)                    LOG_DBG(args)
#else
    #define SDK_ERR(...)                        {(void) (0);}
    #define SDK_WRN(...)                        {(void) (0);}
    #define SDK_INF(...)                        {(void) (0);}
    #define SDK_DBG(...)                        {(void) (0);}
#endif

/* **************************************************************************************************************** */
/* Namespace Declaration                                                                                            */
/* **************************************************************************************************************** */
using tools::SDKTools;

/* **************************************************************************************************************** */
/* Global Variable Declaration                                                                                      */
/* **************************************************************************************************************** */
//static int8_t _advertiserTxPower    = -128;
//static int8_t _connectionTxPower    = -128;
//static int8_t _scanInitTxPower      = -128;


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
// static

uint32_t                    SDKTools::_timeout;
struct k_timer*             SDKTools::_pTimerID;
SDKTools::TimerUserData*    SDKTools::_pUserData;
bool                        SDKTools::_timerMode;

void SDKTools::init()
{
    uint32_t errCode = NRFX_SUCCESS;
    #if (APP_SCHEDULER_ENABLED != 0)
    {
        ble::Scheduler::init();
    }
    #endif
    
    #if (USE_APP_TIMER != 0)
    {
        SDK_DBG("`app_timer` initializing ...");
        errCode = _appTimerInit();
    }
    #endif

    if (errCode == NRFX_SUCCESS)
    {
        SDK_DBG("Initialization of SDK modules done!");
    }
    else
    {
       SDK_ERR("Initialization of SDK modules FAILED ! (errCode: 0x%04x)", errCode);
    }
}

#if (USE_IEEE11073_16 != 0)
    // --------------------------------------------------------------------------------------------------------------
    // static
    uint8_t SDKTools::floatToIEEE11073Float(float value, uint8_t precision, uint8_t* encodedValue)
    {
        uint16_t    tmpResult;
        int8_t      exponent;   ///< Base 10 exponent
        int16_t     mantissa;   ///< Mantissa, should be using only 24 bits

        // Bluetooth FLOAT-TYPE is defined in ISO/IEEE Std. 11073
        // SFLOATs are 16 bits
        // Format [4bit exponent][12bit mantissa]
        if (precision > IEEE11073_16_EXPONENT_MAX)
        {
            precision = IEEE11073_16_EXPONENT_MAX;
        }

        exponent = -precision & IEEE11073_16_EXPONENT_MASK;
        mantissa = (int16_t) floorf(value * powf(10.0, (float) precision));

        if (mantissa > IEEE11073_16_MANTISSA_MAX)
        {
            mantissa = IEEE11073_16_MANTISSA_MAX;
        }
        else if (mantissa < -(IEEE11073_16_MANTISSA_MASK))
        {
            mantissa = -(IEEE11073_16_MANTISSA_MASK);
        }

        tmpResult = (uint16_t)(((int16_t) exponent) << 4) | (mantissa & IEEE11073_16_MANTISSA_MASK);

        encodedValue[0] = (uint8_t)((tmpResult) & 0x00ff);
        encodedValue[1] = (uint8_t)((tmpResult >>  8) & 0x00ff);

        return sizeof(uint16_t);
    }

    // --------------------------------------------------------------------------------------------------------------
    // static
    float SDKTools::IEEE11073ToFloat(uint8_t* ieeeFloat)
    {
        int8_t  exponent;
        int32_t mantissa;

        exponent = (int8_t) ((ieeeFloat[1] >> 4) & IEEE11073_16_EXPONENT_MASK);
        mantissa = (int16_t)  ieeeFloat[0] + (((int16_t)(ieeeFloat[1] & IEEE11073_16_EXPONENT_MASK) << 8));

        if (exponent > IEEE11073_16_EXPONENT_MAX)
        {
            exponent -= IEEE11073_16_EXPONENT_MASK;
        }

        if (mantissa > IEEE11073_16_MANTISSA_MAX)
        {
            mantissa -= IEEE11073_16_MANTISSA_MASK;
        }
        return ((float) mantissa) * ((float) powf(10.0, exponent));
    }
#else
    // --------------------------------------------------------------------------------------------------------------
    // static
    uint8_t SDKTools::floatToIEEE11073Float(float value, uint8_t precision, uint8_t* encodedValue)
    {
        uint32_t    tmpResult;
        int8_t      exponent;   ///< Base 10 exponent
        int32_t     mantissa;   ///< Mantissa, should be using only 24 bits

        // Bluetooth FLOAT-TYPE is defined in ISO/IEEE Std. 11073
        // FLOATs are 32 bits
        // Format [8bit exponent][24bit mantissa]
        exponent = -precision;
        mantissa = (int32_t) floorf(value * powf(10.0, (float) precision));

        if (mantissa > IEEE11073_MANTISSA_MAX)
        {
            mantissa = IEEE11073_MANTISSA_MAX;
        }
        else if (mantissa < -(IEEE11073_MANTISSA_MASK))
        {
            mantissa = -(IEEE11073_MANTISSA_MASK);
        }

        tmpResult = (uint32_t)(((int32_t) exponent) << 24) | (mantissa & IEEE11073_MANTISSA_MASK);

        encodedValue[0] = (uint8_t)((tmpResult) & 0x000000ff);
        encodedValue[1] = (uint8_t)((tmpResult >>  8) & 0x000000ff);
        encodedValue[2] = (uint8_t)((tmpResult >> 16) & 0x000000ff);
        encodedValue[3] = (uint8_t)((tmpResult >> 24) & 0x000000ff);

        return sizeof(uint32_t);
    }

    // --------------------------------------------------------------------------------------------------------------
    // static
    float SDKTools::IEEE11073ToFloat(uint8_t* ieeeFloat)
    {
        int8_t  exponent;
        int32_t mantissa;

        exponent = (int8_t)  ieeeFloat[3];
        mantissa = (int32_t) ieeeFloat[0] + (((int32_t) ieeeFloat[1]) << 8) + (((int32_t) ieeeFloat[2]) << 16);

        if (mantissa > IEEE11073_MANTISSA_MAX)
        {
            mantissa -= IEEE11073_MANTISSA_MASK;
        }
        return ((float) mantissa) * ((float) powf(10.0, exponent));
    }
#endif // #if (USE_IEEE11073_16 != 0)

// ------------------------------------------------------------------------------------------------------------------
// static
uint8_t SDKTools::floatToUnsignedByteArray(float value, uint8_t* encodedValue)
{
    intFloatUnion_t u;

    u.f = value;
    encodedValue[0] = (uint8_t)((u.i) & 0x000000ff);
    encodedValue[1] = (uint8_t)((u.i >>  8) & 0x000000ff);
    encodedValue[2] = (uint8_t)((u.i >> 16) & 0x000000ff);
    encodedValue[3] = (uint8_t)((u.i >> 24) & 0x000000ff);

    return sizeof(uint32_t);
}

// ------------------------------------------------------------------------------------------------------------------
// static
float SDKTools::unsignedByteArrayToFloat(uint8_t* encodedValue)
{
    intFloatUnion_t u;

    u.i  = ((uint32_t) encodedValue[0]);
    u.i |= ((uint32_t) encodedValue[1]) <<  8;
    u.i |= ((uint32_t) encodedValue[2]) << 16;
    u.i |= ((uint32_t) encodedValue[3]) << 24;

    return u.f;
}

// ------------------------------------------------------------------------------------------------------------------
// static
float SDKTools::unsignedInt16ToFloat(uint16_t value)
{
    float f;
	if (value >= 0x8000)
    {
        // Applying binary twos complement format
		value = (~value) + 1; 
		f     = ((float) value) * -1;
	} else {
		f     =  (float) value;
	}
    return f;
}

// ------------------------------------------------------------------------------------------------------------------
// static
uint16_t SDKTools::GCD(uint16_t a, uint16_t b)
{
    uint8_t c;
    while(a != 0)
    {
        c = a;
        a = b % a;
        b = c;
    }
    return b;
}

// ------------------------------------------------------------------------------------------------------------------
// static
uint32_t SDKTools::roundedDiv(uint64_t a, uint64_t b)
{
    uint64_t result = roundedDiv64(a, b);
    return (uint32_t)(result & 0xffffffff);
}

// ------------------------------------------------------------------------------------------------------------------
// static
uint64_t SDKTools::roundedDiv64(uint64_t a, uint64_t b)
{
    if (a == 0)
    {
        return 0;
    }
       
    if (b != 0)
    {
        return ((a + (b / 2)) / b);
    }
    else
    {
        //ASSERT(false);
        return 0xffffffffffffffff;
    }
}

// ------------------------------------------------------------------------------------------------------------------
// static
uint32_t SDKTools::ceiledDiv(uint64_t a, uint64_t b)
{
    uint64_t result = ceiledDiv64(a, b);
    return (uint32_t)(result & 0xffffffff);
}

// ------------------------------------------------------------------------------------------------------------------
// static
uint64_t SDKTools::ceiledDiv64(uint64_t a, uint64_t b)
{
    if (a == 0)
    {
        return 0;
    }

    if (b != 0)
    {
        return 1 + ((a - 1) / b);
    }
    else
    {
        //ASSERT(false); 
        return 0xffffffffffffffff;
    }
}

// ------------------------------------------------------------------------------------------------------------------
// static
bool SDKTools::isObjectNULL(void* object)
{
    if (object == NULL)
    {
        #if (DEBUG_SDKTOOLS_ENABLE != 0) && defined(DEBUG_NRF_USER)
            SDK_DBG("Object %lu is NULL !", object);
        #endif // #if (DEBUG_SDKTOOLS_ENABLE != 0) && defined(DEBUG_NRF_USER)
        return true;
    }
    return false;
}


// ------------------------------------------------------------------------------------------------------------------
// static
uint8_t  SDKTools::modulo8(uint8_t value, uint8_t divider)
{
    return (uint8_t) (modulo((uint32_t) value, (uint32_t) divider) & 0x000000ff); 
}

// ------------------------------------------------------------------------------------------------------------------
// static
uint16_t SDKTools::modulo16(uint16_t value, uint16_t divider)
{
    return (uint16_t) (modulo((uint32_t) value, (uint32_t) divider) & 0x0000ffff); 
}

// ------------------------------------------------------------------------------------------------------------------
// static
uint32_t SDKTools::modulo(uint32_t value, uint32_t divider)
{
    while(value >= divider)
    {
        value -= divider;
    }
    return value; 
}

// ------------------------------------------------------------------------------------------------------------------
// static
uint64_t SDKTools::modulo64(uint64_t value, uint64_t divider)
{
    while(value >= divider)
    {
        value -= divider;
    }
    return value;
}

// ------------------------------------------------------------------------------------------------------------------
// static
string SDKTools::strFromUint8(uint8_t* data, size_t size)
{
    string str = "";
    for (size_t i = 0; i < size; i++)
    {
        str += static_cast<char>(data[i]);
    }
    return str;
}


#if (USE_APP_TIMER != 0)
    // --------------------------------------------------------------------------------------------------------------
    // static
    uint32_t SDKTools::appTimerCreate(void const* timerId, bool timerMode, void* callback)
    {
        if(timerId != NULL)
        {
            k_timer_init((k_timer*) timerId, (k_timer_expiry_t) callback, NULL);
            k_timer_user_data_set((k_timer*) timerId, (void*) timerMode);
            if (timerMode == LOOP_TIMER)
            {
                _timerMode = LOOP_TIMER;
            }
            else if(timerMode == ONE_SHOT_TIMER)
            {
                _timerMode = ONE_SHOT_TIMER;
            }
            else
            {
                return NRFX_ERROR_INVALID_PARAM;
            }
            return NRFX_SUCCESS;
        }
        else
        {
            SDK_DBG("Timer ID NULL !!!");
            return NRFX_ERROR_INVALID_PARAM;
        }
    }

    // --------------------------------------------------------------------------------------------------------------
    // static
    uint32_t SDKTools::appTimerStart(void* timerId, uint32_t timeout, void* context, uint32_t* rtcValue)
    {
        bool timerMode = (bool) k_timer_user_data_get((k_timer*) timerId);
        if (context != NULL)
        {
            // Transfer user data to the timer
            k_timer_user_data_set((k_timer*) timerId, context);
        }

        // Start one shot Timer
        if(timerMode == LOOP_TIMER)
        {
            k_timer_start((k_timer*) timerId, K_TICKS(timeout), K_TICKS(timeout));
        }
        else
        {
            k_timer_start((k_timer*) timerId, K_TICKS(timeout), K_NO_WAIT);
        }

        if (rtcValue != NULL) // Check if rtcValue pointer exist
        {
            //Get current timer value in ticks
            *rtcValue = static_cast<uint32_t>(k_uptime_ticks());
        }

        return NRFX_SUCCESS;
    }

    // --------------------------------------------------------------------------------------------------------------
    // static
    uint32_t SDKTools::appTimerStop(void* timerId, uint32_t* rtcValue)
    {
        k_timer_stop((k_timer*) timerId);
        
        //_pUserData = (TimerUserData*) k_timer_user_data_get((k_timer*) timerId);

        if (rtcValue != NULL) // Check if rtcValue pointer exist
        {
            //Get current timer value in ticks
            *rtcValue = static_cast<uint32_t>(k_uptime_ticks());
        }
            
        return NRFX_SUCCESS;
    }

    // --------------------------------------------------------------------------------------------------------------
    // static
    uint32_t SDKTools::appTimerCounterDiff(uint32_t ticksTo, uint32_t ticksFrom)
    {
        if (ticksFrom > ticksTo)
        {
            return 0x00FFFFFF - ticksFrom + ticksTo;
        }
        else
        {
            return ticksTo - ticksFrom;
        }
    }

    // --------------------------------------------------------------------------------------------------------------
    // static
    float SDKTools::appTimerCounterDiffInMs(uint32_t ticksTo, uint32_t ticksFrom)
    {
        uint32_t diff = appTimerCounterDiff(ticksTo, ticksFrom);
        return diff / k_ms_to_ticks_ceil32(1); // t_in_ms = nb_ticks / nb_ticks_per_ms 
        // -> ms = [tick]/([tick]*[ms^(-1)]) -> 1/ms^(-1) = ms
    }
#endif // #if (USE_APP_TIMER != 0)

// ------------------------------------------------------------------------------------------------------------------
// static
size_t SDKTools::updateName(char* name, size_t nameLen, uint16_t suffix)
{
    // Set up the device's name
    uint8_t localNameSize   = nameLen;
    uint8_t extendNameSize  = sizeof("#xxxxx") - 1;
    if ((localNameSize + extendNameSize) > DEVICE_NAME_MAX_SIZE)
    {
        localNameSize       = DEVICE_NAME_MAX_SIZE - extendNameSize;
    }

    memcpy(name, DEVICE_NAME, localNameSize);
    sprintf(&name[localNameSize], "#%05d", suffix);
    return localNameSize + extendNameSize;
}

// ------------------------------------------------------------------------------------------------------------------
// static
char* SDKTools::debugPrintBuffer(const uint8_t* data, size_t size)
{
    #if defined(DEBUG_NRF_USER)
    {
        static char bufDebug[3 * 255 + 1];
        char* pBuf = bufDebug;
        size_t len = size > 255 ? 255:size;
        memset(bufDebug, 0x00, sizeof(bufDebug));

        for (uint8_t i = 0; i < len; i++)
        {
            pBuf += sprintf(pBuf, " %02x", data[i]);
        }
        return bufDebug;
    }
    #else
        return NULL;
    #endif // #if defined(DEBUG_NRF_USER)
}

// ------------------------------------------------------------------------------------------------------------------
// static
char* SDKTools::debugPrintfData(const uint8_t* data, size_t size)
{
    #if defined(DEBUG_NRF_USER)&&(false)
    {
        // if you receive 4, you will get 0x04 as returned char*
        static char strDebug[5 * 512 + 1];
        char* pStr = strDebug;
        size_t len = size > 512 ? 512:size;
        memset(strDebug, 0x00, sizeof(strDebug));
        
        for (uint8_t i = 0; i < len; i++)
        {
            pStr += sprintf(pStr, "0x%02x ", data[i]);
        }
        
        return strDebug;
        return NULL;
    }
    #else
        return NULL;
    #endif // #if defined(DEBUG_NRF_USER)
}

// ------------------------------------------------------------------------------------------------------------------
// static
char* SDKTools::debugPrintfString(const uint8_t* data, size_t size)
{
    #if defined(DEBUG_NRF_USER)&&(false)
    {
        // if you receive 0x41, you will get "A" as returned char*
        static char strDebug[512 + 1];
        char* pStr = strDebug;
        size_t len = size > 512 ? 512:size;
        memset(strDebug, 0x00, sizeof(strDebug));
        
        for (uint8_t i = 0; i < len; i++)
        {
            if (data[i] != 0)
            {
                pStr += sprintf(pStr, "%c", data[i]);
            }
        }
        
        return strDebug;
        return NULL;
    }
    #else
        return NULL;
    #endif // #if defined(DEBUG_NRF_USER)
}

// ------------------------------------------------------------------------------------------------------------------
// static
char* SDKTools::debugPrintfFloat(const float value)
{
    #if defined(CONFIG_LOG) && (CONFIG_LOG == true)
    {
        static char strFloat[16];
        char* pStr = strFloat;
        memset(strFloat, 0x00, 16);

        pStr += sprintf(pStr, "%5.3f", value);
        return strFloat;
    }    
    #else
        return NULL;
    #endif // #if defined(CONFIG_LOG) && (CONFIG_LOG == true)
}

// ------------------------------------------------------------------------------------------------------------------
// static
void SDKTools::memoryTest()
{
    #if defined(false)
    {
        static bool initialized = false;
        static uint8_t* pHeapTest = new uint8_t[__HEAP_SIZE - sizeof(uint32_t)];
        
        if (!initialized) {
            //ASSERT(pHeapTest);
            memset(pHeapTest, 0xaa, __HEAP_SIZE - sizeof(uint32_t));
            initialized = true;
        }
        
        if (pHeapTest[__HEAP_SIZE - sizeof(uint32_t) - 1] != 0xaa) 
        {
            SDK_DBG("STACK overrides HEAP !");
            //ASSERT(false);
        }
        else
        {
            uint32_t* pTest = new uint32_t;
            uint32_t stackTest;
            SDK_DBG("HEAP start: 0x%08x, end: 0x%08x, size: %d. STACK addr: 0x%08x", pHeapTest, pTest, (uint32_t)pTest - (uint32_t)pHeapTest, &stackTest);
            delete pTest;
        }
    }
    #endif // #if defined(DEBUG_NRF_USER)
}


/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
// static
uint32_t SDKTools::_appTimerInit()
{
    #if (USE_APP_TIMER != 0)
        
        // K_TIMER module doesn't require to be init

        return NRFX_SUCCESS; 
    #endif
}