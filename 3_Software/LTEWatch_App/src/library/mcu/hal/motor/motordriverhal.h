/****************************************************************************************************************//**
 * Copyright (C) MSE Lausanne Hes-so VAUD/WAADT, Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2023
 * Modified by Tristan Traiber
 * Created by Thierry Hischier, Patrice Rudaz
 * All rights reserved.
 *
 * \file    motordriverhal.h
 *
 * @addtogroup LowHAL
 * @{
 *
 * @class   hal::MotorDriverHal
 * @brief   Class of Hardware Abstraction Layer to handle the RTC 2 needed by the motorDriver.
 *
 * This class handles the RTC events and toggle the needed GPIOs to apply some pulse on the motor's coils, based on
 * Nordic SDK v15.0.0 and later...
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2016-06-16
 *
 * \author  Tristan Traiber (tristan.traiber@master.hes-so.ch)
 * \date    December 2022
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <nrfx_rtc.h>
#include "gpiohal.h"

#define USE_MOTORS 1

#if (USE_MOTORS != 0)
#include "motor-config.h"

#if defined(__cplusplus)
extern "C"
{
#endif
    namespace hal
    {
        /********************************************************************************************************//**
         * @brief         Motor's RTC Timer event handler. (Callback Function)
         *
         * This function will be called each time the RTC used for the motors thorw an event.
         *
         * @param   intType     The event type of the used RTC.
         ************************************************************************************************************/
        #if(NRFX_RTC_DRIVER != 0)
            void motorTimerHandler(nrfx_rtc_int_type_t intType);
        #else
            void motorTimerHandler(struct k_timer *timer_id);
        #endif
    }   // namespace motor
#if defined(__cplusplus)
}
#endif

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* NAMESPACE Declaration                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
namespace hal
{
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* Class Declaration                                                                                            */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    class MotorDriverHal
    {
        #if(NRFX_RTC_DRIVER != 0)
            friend void motorTimerHandler(nrfx_rtc_int_type_t intType);
        #else
            friend void motorTimerHandler(struct k_timer *timer_id);
        #endif
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* CONSTRUCTOR SECTION                                                                                      */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        MotorDriverHal();
        ~MotorDriverHal();

        /* ******************************************************************************************************** */
        /*																										    */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*																										    */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Initializes the components needed by this class
         *
         * This method initializes the timers (hardware) used to drive the stepper motors.
         *
         * \warning Must be called before any other method of this class
         ************************************************************************************************************/
        inline void initialize()                        { _timerInit(); }

        /********************************************************************************************************//**
         * @brief   Clear the GPIOs used to do a motor's step.
         *
         * @param   outCoils        The GPIOs to clear.
         ************************************************************************************************************/
        inline void clearMotorPulse()                   { hal::GpioHal::pinMaskClear(_gpioMask); }

        /********************************************************************************************************//**
         * @brief   Applies the pulse on the GPIOs used to drive the motor's coils.
         *
         * @param   pulse           The specific pulse to be applied on the GPIOs.
         ************************************************************************************************************/
        inline void setMotorPulse(uint64_t pulse)       { hal::GpioHal::pinMaskSet(pulse); }

        /********************************************************************************************************//**
         * @brief   Configures the triggers for the timer according to the steppermotor's specs.
         *
         * This method takes care of the stepper-motor's specifications to calculate the trigger to be used with the 
         * rtc for the duration of the pulse to be applied to make on1 step.
         *
         * The precision of the trigger depends on the clock used for it. This clock is defined by 
         * `RTC2_CONFIG_FREQUENCY` (32768 here). The trigger is obtained by the following equation:
         *
         * \f[
         *      trigger = \frac{RTC2\_CONFIG\_FREQUENCY}{1000} * (\frac{duration}{1000});
         * \f]
         *
         * As trigger is an integer value, we should always ceil this result at the next integer. After this first 
         * step, we check that the delay obtained is compatible with the motor's specifications. As long as the delay
         * is below the pulse specification, it's incremented.
         *
         * @param   pulseLen        The duration of the pulse to be applied on the steppermotor's coil in [us]
         ************************************************************************************************************/
        void setPulseTick(uint32_t pulseLen);

        /********************************************************************************************************//**
         * @brief   Set up the GPIO's mask to be used to clear the pulse at the end motor's step
         *
         * @param   motorCoils      All GPIOs used by the motors.
         * @param   size            Amount of GPIOS used by all motors.
         ************************************************************************************************************/
        void setGapTick(uint32_t gapTick);

        /********************************************************************************************************//**
         * @brief   Set up the GPIO's mask to be used to clear the pulse at the end motor's step
         *
         * @param   motorCoils      All GPIOs used by the motors.
         * @param   size            Amount of GPIOS used by all motors.
         ************************************************************************************************************/
        void setGpioMask(uint8_t* motorCoils, size_t size);

        /********************************************************************************************************//**
         * @brief   Set up the GPIOs as outputs (for motor in series)
         *
         * @param   motorCoils      All GPIOs used by the motors.
         * @param   size            Amount of GPIOS used by all motors.
         ************************************************************************************************************/
        void setGpioOutputs(uint8_t* motorCoils, size_t size);

        /********************************************************************************************************//**
         * @brief   Starts the timer dedicated to skip the motors for a pulse's delay.
         *
         * @param   timeoutTicks    The duration of the timer to be used
         ************************************************************************************************************/

        /********************************************************************************************************//**
         * @brief   Starts the timer dedicated for the motor's pulse.
         *
         * @param   gapTicks
         ************************************************************************************************************/
        bool startTimerPulse();

        /********************************************************************************************************//**
         * @brief   Starts the timer dedicated to skip the motors for a pulse's delay.
         *
         * @param   timeoutTicks    The duration of the timer to be used
         ************************************************************************************************************/
        bool startTimerNoPulse();

        /********************************************************************************************************//**
         * @brief   Disable the interrupt of the Gap Delay.
         ************************************************************************************************************/
        bool enableGapPulse();

        /********************************************************************************************************//**
         * @brief   Disable the interrupt of the Gap Delay.
         ************************************************************************************************************/
        void disableGapPulse();

        /********************************************************************************************************//**
         * @brief   Stops the RTC timer/counter used fot the motor's driver.
         ************************************************************************************************************/
        void stopTimer();

    protected:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PROTECTED DECLARATION SECTION                                                                            */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        #if(NRFX_RTC_DRIVER != 0)
            nrfx_rtc_t   rtc;
        #endif

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Attributes
        bool                    _isTimerInitialized;
        bool                    _isTimerPoweredOn;
        uint32_t                _pulseTick;
        uint32_t                _gapTick;
        uint64_t                _gpioMask;
        #if(NRFX_RTC_DRIVER != 0)
            nrfx_rtc_config_t     _rtcDrvConfig;
        #else
            static struct k_timer*  _pPulseTimer;
            static struct k_timer*  _pGapTimer;
        #endif
        // Methods
        void _timerInit();

    };  // Class MotorDriverHal

}  // Namepsace hal

#endif  // #if (USE_MOTORS != 0)

/** @} */
