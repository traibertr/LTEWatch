/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    basecontroller.h
 *
 * @addtogroup GPIO
 * \{
 *
 * @class   BaseController
 * @brief   This class is the mother class of both Gpio and Button controller.
 *
 * # BaseController
 *
 * This class handles all counters and delays, and their getters and setters, used by the GpioCOntroller and the
 * ButtonController.
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2016-01-09
 *
 * @author  Patrice Rudaz (patrice.rudaz@hevs.ch)
 * @date    July 2018
 ********************************************************************************************************************/
#pragma once


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <stdint.h>

#include "gpio-config.h"

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* NAMESPACE Declaration                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
namespace gpio
{

    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* Class Declaration                                                                                            */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    class BaseController
    {
    protected:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PROTECTED DECLARATION SECTION                                                                            */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Initializes the components needed by this class
         *
         * This method initializes the GPIOTE (hardware) used to handle the gpio events. To define which GPIOTE 
         * are used for a gpio, see gpiohal.h.
         *
         * @warning Must be called before any other method of this class
         ************************************************************************************************************/
        void            initialize();

        /********************************************************************************************************//**
         * @brief   Retrieves the current duration, in ms, of the gpio's gesture.
         *
         * @param   index   The identifier of the gpio.
         *
         * @return  The gesture's duration in [ms] as an `uint32_t`
         ************************************************************************************************************/
        uint32_t        delayCounters(uint8_t index) const;

        /********************************************************************************************************//**
         * @brief   Set the current duration, in ms, of the gpio's gesture.
         *
         * @param   index   The identifier of the gpio.
         *
         * @return  The gesture's duration in [ms] as an `uint32_t`
         ************************************************************************************************************/
        void            setDelay(uint8_t index, uint32_t delay);

        /********************************************************************************************************//**
         * @brief   Retrieves the current duration, in ms, of the gpio's gesture.
         *
         * @param   index   The identifier of the gpio.
         *
         * @return  The gesture's duration in [ms] as an `uint32_t`
         ************************************************************************************************************/
        uint32_t        delay(uint8_t index) const;

        /********************************************************************************************************//**
         * @brief   Reset the durtation of the gpio's gesture.
         *
         * @param   index       The identifier of the gpio.
         * @param   resetVal    Value to be set when resetting the counter value of the debouncer's timer.
         ************************************************************************************************************/
        void            resetDelayCounter(uint8_t index, uint8_t resetVal = 0);

        /********************************************************************************************************//**
         * @brief   Increment the durtation of the gpio's gesture of the `offset` value.
         *
         * @param   index   The identifier of the gpio.
         * @param   offset  The amount of ms to increase the gesture's duration of the gpio.
         ************************************************************************************************************/
        void            incrDelayCounter(uint8_t index, uint32_t offset);

        /********************************************************************************************************//**
         * @brief   Retrieves the MASK used for all registered gpios.
         *
         * @return  The GPIO MASK used by all registered gpios as an `uint32_t`
         ************************************************************************************************************/
        inline uint32_t pinMaskForRegisteredObject() const  { return _pinMaskForRegisteredObject; }

        uint32_t        _delays[NB_MAX_GPIOS];              ///< @brief Array to store the delays for each gpio
        uint32_t        _delayCounters[NB_MAX_GPIOS];       ///< @brief Array to measure the duration of the Button's press
        uint64_t        _pinMaskForRegisteredObject;        ///< @brief Mask of pins for the registered gpios
        uint8_t         _nbRegisteredObject;                ///< @brief Number of registered gpios
        bool            _isHandlerTimerInitialized;         ///< @brief Store the Timer's state...
        bool            _isHandlerTimerActive;
    };

}   // namespace gpio

/** \} */   // Group: GPIO
