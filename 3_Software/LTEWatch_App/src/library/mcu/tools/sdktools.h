/****************************************************************************************************************//**
 * Copyright (C) MSE Lausanne Hes-so VAUD/WAADT, Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2023
 * Modified by Tristan Traiber
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    sdktools.h
 *
 * @addtogroup Tools
 * \{
 *
 * @class   tools::SDKTools
 * @brief   Class agregating a couple of tool reéated to the SDK libraries and drivers provided by Nordic.
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2016-06-16
 *
 * @author  Tristan Traiber (tristan.traiber@master.hes-so.ch)
 * @date    December 2022
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <zephyr/zephyr.h>
#include <stdlib.h>
#include <stdint.h>
#include <platform-config.h>
#include "config/sdk-tools-config.h"

#include <string>

#define LOOP_TIMER      true
#define ONE_SHOT_TIMER  false

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* TYPE definition                                                                                                  */
/*                                                                                                                  */
/* **************************************************************************************************************** */
typedef union
{
    int     i;
    float   f;
} intFloatUnion_t;

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* NAMESPACE Declaration                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
using namespace std;

namespace tools
{
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* Class Declaration                                                                                            */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    class SDKTools
    {
    protected:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PROTECTED DECLARATION SECTION                                                                            */
        /* CONSTRUCTOR SECTION                                                                                      */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        SDKTools() {}
        ~SDKTools() {}

    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        typedef struct{
            void        *context;
            uint32_t    timeout;
        } TimerUserData;

        /********************************************************************************************************//**
         * @brief   Initialization the app_timer module
         *
         * @param   Type of function for passing events from the timer module to the scheduler.
         ************************************************************************************************************/
        static void init();

        /********************************************************************************************************//**
         * @brief   Process the conversion from \c float to IEEE-11073 FLOAT (32 bits) or IEEE-11073 SFLOAT (16 bits).
         *
         * This function will convert any `float` value to IEEE-11073 32bit FLOAT (16-bit SFLOAT) and retrieve it as
         * an array of `uint8_t` to be able to sent it through BLE.
         ************************************************************************************************************/
        static uint8_t floatToIEEE11073Float(float value, uint8_t precision, uint8_t* encodedValue);

        /********************************************************************************************************//**
         * @brief   Process the conversion from IEEE-11073 32bits FLOAT or IEEE-11073 16bits SFLOAT to a float value.
         *
         * This function will convert any IEEE-11073 32bits FLOAT (16bits SFLOAT) value to `float` value and retrieve
         * it.
         ************************************************************************************************************/
        static float IEEE11073ToFloat(uint8_t* ieee_float);

        /********************************************************************************************************//**
         * @brief   Process the conversion between float to IEEE-754 32bits float saved as an array of uint8_t.
         *
         * This function will convert any `float` value to IEEE-11073 32bit FLOAT and retrieve it as an array of 4
         * `uint8_t` to be able to sent it through BLE.
         ************************************************************************************************************/
        static uint8_t floatToUnsignedByteArray(float value, uint8_t* encodedValue);

        /********************************************************************************************************//**
         * @brief   Process the conversion from an array of uint8_t value to IEEE-754 32bits float value.
         *
         * This function will convert any array of `uint8_t` to an IEEE-754 32bits float value and retrieve it.
         *
         * @return  Return the given value as an IEEE-754 32bits float
         ************************************************************************************************************/
        static float unsignedByteArrayToFloat(uint8_t* encodedValue);

        /********************************************************************************************************//**
         * @brief   Process the conversion from an unsigned short value to IEEE-754 32bits float value.
         *
         * This function will convert any unsigned short value to an IEEE-754 32bits float and retrieve it.
         *
         * @return  Return the given value as an IEEE-754 32bits float
         ************************************************************************************************************/
        static float unsignedInt16ToFloat(uint16_t value);

        /********************************************************************************************************//**
         * @brief   Process the conversion from an unsigned short value to IEEE-754 32bits float value.
         *
         * This function will convert any unsigned short value to an IEEE-754 32bits float and retrieve it.
         *
         * @return  Return the given value as an IEEE-754 32bits float
         ************************************************************************************************************/
        static inline uint16_t uint16Decode(const uint8_t* pEncodedData) {
            return ((((uint16_t)((uint8_t *)pEncodedData)[0])) |
                    (((uint16_t)((uint8_t *)pEncodedData)[1]) << 8));
        }

        /********************************************************************************************************//**
         * @brief   Function for decoding a uint32 value.
         *
         * @param[in]   pEncodedData   Buffer where the encoded data is stored.
         *
         * @return      Decoded value.
         ************************************************************************************************************/
        static inline uint32_t uint32Decode(const uint8_t * pEncodedData)
        {
            return ((((uint32_t)((uint8_t *)pEncodedData)[0]) << 0)  |
                    (((uint32_t)((uint8_t *)pEncodedData)[1]) << 8)  |
                    (((uint32_t)((uint8_t *)pEncodedData)[2]) << 16) |
                    (((uint32_t)((uint8_t *)pEncodedData)[3]) << 24 ));
        }

        /********************************************************************************************************//**
         * @brief Function for encoding a uint16 value.
         *
         * @param[in]   value           Value to be encoded.
         * @param[out]  pEncodedData    Buffer where the encoded data is to be written.
         *
         * @return      Number of bytes written.
         ************************************************************************************************************/
        static inline uint8_t uint16Encode(uint16_t value, uint8_t* pEncodedData)
        {
            pEncodedData[0] = (uint8_t) ((value & 0x00FF) >> 0);
            pEncodedData[1] = (uint8_t) ((value & 0xFF00) >> 8);
            return sizeof(uint16_t);
        }

        /********************************************************************************************************//**
         * @brief Function for encoding a uint32 value.
         *
         * @param[in]   value           Value to be encoded.
         * @param[out]  pEncodedData    Buffer where the encoded data is to be written.
         *
         * @return      Number of bytes written.
         ************************************************************************************************************/
        static inline uint8_t uint32Encode(uint32_t value, uint8_t * pEncodedData)
        {
            pEncodedData[0] = (uint8_t) ((value & 0x000000FF) >> 0);
            pEncodedData[1] = (uint8_t) ((value & 0x0000FF00) >> 8);
            pEncodedData[2] = (uint8_t) ((value & 0x00FF0000) >> 16);
            pEncodedData[3] = (uint8_t) ((value & 0xFF000000) >> 24);
            return sizeof(uint32_t);
        }


        /********************************************************************************************************//**
         * \name    Arithmetics
         * @brief   Compute the Greatest Common Divisor, rounded devision or ceiled division for integer value of
         *          two numbers
         *
         * @param   a   First number
         * @param   b   Second number
         *
         * \{
         ************************************************************************************************************/
        static uint16_t GCD(uint16_t a, uint16_t b);
        static uint32_t roundedDiv(uint64_t a, uint64_t b);

        /********************************************************************************************************//**
         * @brief   Compute an division and rounded the value to the next unsigned 64 bit integer value.
         *
         * @param   a   First number
         * @param   b   Second number
         ************************************************************************************************************/
        static uint64_t roundedDiv64(uint64_t a, uint64_t b);

        /********************************************************************************************************//**
         * @brief   Compute an division and rounded the value to the higher unsigned integer value.
         *
         * @param   a   First number
         * @param   b   Second number
         ************************************************************************************************************/
        static uint32_t ceiledDiv(uint64_t a, uint64_t b);

        /********************************************************************************************************//**
         * @brief   Compute an division and rounded the value to the higher unsigned integer value (64bits).
         *
         * @param   a   First number
         * @param   b   Second number
         ************************************************************************************************************/
        static uint64_t ceiledDiv64(uint64_t a, uint64_t b);
        /** \} */

        /********************************************************************************************************//**
         * @brief   Checks if the given pointer has a valid address.
         *
         * @return  `true` if the pointed address is invalid and `false` otherwise.
         ************************************************************************************************************/
        static bool isObjectNULL(void* object);

        /********************************************************************************************************//**
         * \name   GetRidOfModulo
         * @brief  Methods to avoid using `modulo` operator.
         *
         * Methods implemented to keep as low as possible the power consumption and avoid using the operator `Modulo`,
         * which consumes a lot of processor power...
         *
         * @param   value   The given value to be operated with modulo
         * @param   divider The value to be used as modulo
         *
         * \{
         ************************************************************************************************************/
        static uint8_t  modulo8(uint8_t value, uint8_t divider);
        static uint16_t modulo16(uint16_t value, uint16_t divider);
        static uint32_t modulo(uint32_t value, uint32_t divider);
        static uint64_t modulo64(uint64_t value, uint64_t divider);

        /********************************************************************************************************//**
         * \name   HandleString
         * @brief  Methods to handle string
         *
         * \{
         ************************************************************************************************************/
        static string strFromUint8(uint8_t* data, size_t size);
        /** \} */


        /** \} */

        #if (USE_APP_TIMER != 0)
            /****************************************************************************************************//**
             * \name    NordicAppTimerWrapper
             * @brief   C++ Wrapper against app_timer utilities provided by Nordic-Semiconductor
             *
             * The following methods let the application create, start and stop an application's timer. The last one
             * gives back the actual value off the internal counter of the timer.
             *
             * \{
             ********************************************************************************************************/
            static uint32_t appTimerCreate(void const* timerId, bool timerMode, void* callback);
            static uint32_t appTimerStart(void* timerId, uint32_t timeout, void* context, uint32_t* rtcValue = NULL);
            static uint32_t appTimerStop(void* timerId, uint32_t* rtcValue = NULL);
            static uint32_t appTimerCounterDiff(uint32_t ticksTo, uint32_t ticksFrom);
            static float appTimerCounterDiffInMs(uint32_t ticksTo, uint32_t ticksFrom);
            //static inline uint32_t appTimerStopAll()    { return app_timer_stop_all(); } only used in "dfuhal.cpp" of MoskitoWatch App
            //static uint32_t appTimerGetCounter();
            static inline uint32_t appTimerGetCounter() { return static_cast<uint32_t>(k_uptime_ticks()); }
            /** \} */
        #endif // #if (USE_APP_TIMER != 0)

        static size_t updateName(char* name, size_t nameLen, uint16_t suffix);

        /********************************************************************************************************//**
         * @brief   For Software reset !
         ************************************************************************************************************/
        static void systemReset()   { NVIC_SystemReset(); }

        /********************************************************************************************************//**
         * \name    DebugPurpose
         * @brief   FOR DEBUG PURPOSE. Returns a string relative to the given parameter...
         *
         * \{
         ************************************************************************************************************/

        static char* debugPrintBuffer(const uint8_t* data, size_t size);
        static char* debugPrintfData(const uint8_t* data, size_t size);
        static char* debugPrintfString(const uint8_t* data, size_t size);
        static char* debugPrintfFloat(const float value);
        /** \} */

        /********************************************************************************************************//**
         * @brief   Checks if there is no leak in HEAP memory.
         ************************************************************************************************************/
        static void memoryTest();

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Iinitialization the app_timer module
         *
         * @param   Type of function for passing events from the timer module to the scheduler.
         *
         * @return  NRFX_SUCCESS If no error occurs
         ************************************************************************************************************/
        static uint32_t _appTimerInit();

        static uint32_t         _timeout;
        static struct k_timer*  _pTimerID;
        static TimerUserData*   _pUserData;
        static bool             _timerMode;

    };  // Class SDKTools

}  // Namepsace tools

/** \} */   // Group: Tools
