/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    ledhal.h
 *
 * @addtogroup LowHAL
 * @{
 *
 * @class   hal::LedHal
 * @brief   Class of Hardware Abstraction Layer to handle LEDs of device based on the nRf5 series chips.
 *
 * This file contains all necessary functions to handle LEDS on boards based on the nRF5 series and SDK v14.2.0 and
 * above. It starts with the function `initLEDs()` to initialize the GPIOs used for the LEDs.
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2016-06-16
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
#include <stdint.h>


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* APPLICATION defintion                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */

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
    class LedHal
    {
    public:
        /* ******************************************************************************************************** */
        /*																										    */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*																										    */
        /* ******************************************************************************************************** */

        /********************************************************************************************************//**
         * @brief   Enumeration used to have a unique identifier for every button action.
         ************************************************************************************************************/
        static bool initLeds();

        /********************************************************************************************************//**
         * @brief   Initializes the Hardware GPIO assigned to the pin number given for
         *          the LED's index.
         *
         * Set as output the PIN assigned to the LED specified by its index.
         *
         * @param   index   Specify the LED to work with.
         ************************************************************************************************************/
        static bool initLed(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Turn OFF the given LED.
         *
         * Changes the state of GPIO according to the index identifying the LED to turn it OFF.
         *
         * @param   index   The identifier of the LED to work with.
         ************************************************************************************************************/
        static bool setLedOff(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Switch ON the given LED.
         *
         * Changes the state of GPIO according to the index identifying the LED to switch it ON.
         *
         * @param   index   The identifier of the LED to work with.
         ************************************************************************************************************/
        static bool setLedOn(uint8_t index);

        /********************************************************************************************************//**
         * @brief   Toggle the state of the LED specified by its index.
         *
         * Toggles the state of GPIO assigned to the LED specified by the given index.
         *
         * @param   index   The identifier of the LED to work with.
         ************************************************************************************************************/
        static bool toggleLed(uint8_t index);

    protected:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PROTECTED DECLARATION SECTION                                                                            */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        LedHal()  {}
        ~LedHal() {}

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        static bool _switchOnOff(uint8_t index, bool enable);
        static void _switchOn(uint8_t pinNbr);
        static void _switchOff(uint8_t pinNbr);
    };  // Class ledHAL

}   // Namepsace hal

/** @} */
