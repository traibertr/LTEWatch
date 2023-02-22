/****************************************************************************************************************//**
 * Copyright (C) MSE Lausanne Hes-so VAUD/WAADT, Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2023
 * Modified by Tristan Traiber
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * \file    gpiohal.h
 *
 * @addtogroup LowHAL
 * @{
 *
 * @class   hal::GpioHal
 * @brief   Class of Hardware Abstraction Layer to handle the different GPIO's of the Nordic nRf5 series chips.
 *
 * Class of Hardware Abstraction Layer to handle the different GPIO's of the nRf5 series chips based on Nordic SDK
 * v15.0.0 and later. This file contains all functions necessary for the use of any GPIO. It starts with the function
 * `gpioInit()` to initialize all GPIO to disable unwanted current consumptions.
 *
 * \author  Tristan Traiber (tristan.traiber@master.hes-so.ch)
 * \date    January 2023
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <stdint.h>
#include <stdlib.h>
#include <drivers/gpio.h>
#include "gpio-config.h"
#include "debug-config.h"

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
    /* ***************************** ZEPHYR GPIO HAL FLAG LIST *****************************
    -----------------------------------------
    - GPIO input/output configuration flags -
    -----------------------------------------
    -- GPIO_INPUT           : Enables pin as input
    -- GPIO_OUTPUT          : Enables pin as output, no change to the output state
    -- GPIO_DISCONNECTED    : Disables pin for both input and output
    -- GPIO_OUTPUT_LOW      : Configures GPIO pin as output and initializes it to a low state
    -- GPIO_OUTPUT_HIGH     : Configures GPIO pin as output and initializes it to a high state
    -- GPIO_OUTPUT_INACTIVE : Configures GPIO pin as output and initializes it to a logic 0
    -- GPIO_OUTPUT_ACTIVE   : Configures GPIO pin as output and initializes it to a logic 1

    --------------------------------------
    - GPIO interrupt configuration flags -
    --------------------------------------
    -- GPIO_INT_DISABLE             : Disables GPIO pin interrupt
    -- GPIO_INT_EDGE_RISING         : Configures GPIO interrupt to be triggered on pin rising edge and enables it
    -- GPIO_INT_EDGE_FALLING        : Configures GPIO interrupt to be triggered on pin falling edge and enables it
    -- GPIO_INT_EDGE_BOTH           : Configures GPIO interrupt to be triggered on pin rising or falling edge and enables it
    -- GPIO_INT_LEVEL_LOW           : Configures GPIO interrupt to be triggered on pin physical level low and enables it
    -- GPIO_INT_LEVEL_HIGH          : Configures GPIO interrupt to be triggered on pin physical level high and enables it
    -- GPIO_INT_EDGE_TO_INACTIVE    : Configures GPIO interrupt to be triggered on pin state change to logical level 0 and enables it
    -- GPIO_INT_EDGE_TO_ACTIVE      : Configures GPIO interrupt to be triggered on pin state change to logical level 1 and enables it
    -- GPIO_INT_LEVEL_INACTIVE      : Configures GPIO interrupt to be triggered on pin logical level 0 and enables it
    -- GPIO_INT_LEVEL_ACTIVE        : Configures GPIO interrupt to be triggered on pin logical level 1 and enables it
    -- GPIO_INT_DEBOUNCE

    -------------------------------
    - GPIO pin active level flags -
    -------------------------------
    -- GPIO_ACTIVE_LOW  : GPIO pin is active (has logical value ‘1’) in low state
    -- GPIO_ACTIVE_HIGH : GPIO pin is active (has logical value ‘1’) in high state.

    ------------------------
    - GPIO pin drive flags -
    ------------------------
    -- GPIO_OPEN_DRAIN  : Configures GPIO output in open drain mode (wired AND)
             -----------------------------------------------------------------------------------
            | ‘Open Drain’ mode also known as ‘Open Collector’ is an output configuration which |
            |  behaves like a switch that is either connected to ground or disconnected.        |
             -----------------------------------------------------------------------------------
    -- GPIO_OPEN_SOURCE : Configures GPIO output in open source mode (wired OR).
             ------------------------------------------------------------------------------------
            | ‘Open Source’ is a term used by software engineers to describe output mode opposite | 
            |  to ‘Open Drain’. It behaves like a switch that is either connected to power supply |
            |  or disconnected. There exist no corresponding hardware schematic and the term is   |
            |  generally unknown to hardware engineers.                                           |
             ------------------------------------------------------------------------------------

    ------------------------
    - GPIO pin bias flags -
    ------------------------
    -- GPIO_PULL_UP     : Enables GPIO pin pull-up
    -- GPIO_PULL_DOWN   : Enable GPIO pin pull-down

    -----------------
    - Unnamed Groug -
    -----------------
    -- GPIO_VOLTAGE_DEFAULT : Set pin at the default voltage level
    -- GPIO_VOLTAGE_1P8     : Set pin voltage level at 1.8 V
    -- GPIO_VOLTAGE_3P3     : Set pin voltage level at 3.3 V
    -- GPIO_VOLTAGE_5P0     : Set pin voltage level at 5.0 V

    ************************************************************************************* */

    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* Class Declaration                                                                                            */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    class GpioHal
    {
    private:
        static struct gpio_dt_spec dev; ///< Container for GPIO pin information specified in devicetree (Zephyr GPIO API)

    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Initialiaze all GPIOs
         *
         * Setup all GPIOs to disable unwated current consumptions. The default value at startup is :
         *  - All GPIO as Input
         *  - disconnected
         *  - pulldown or puulup disabled
         *  - SENSE disabled
         ************************************************************************************************************/
        static void init();

        /********************************************************************************************************//**
         * @brief   Configure the PIN as an input
         *
         * This method configures the PIN specified by `pinNbr` as an input and setup its drive's capability and
         * connection's state.
         *
         * @param   pinNbr      Identify the pin to configure
         * @param   drive       Set the current drive capacity (high or low drain capability).
         * @param   pullSetup   Configure the input with an active pullup, pulldown or no pull at all.
         * @param   connect     Specify if the pin is connected or disconnected.
         ************************************************************************************************************/
        static void cfgPinInput(uint32_t pinNbr, uint32_t drive, uint32_t pullSetup, uint32_t connect);

        /********************************************************************************************************//**
         * @brief   Configure the PIN as an input with neither pullup or pulldown, standard drive and connected.
         *
         * This method configures the PIN specified by `pinNbr` as an input and takes `NOPULL`, `S0S1` and `CONNECTED`
         * as default values. It is dedicated to configure GPIO for button's usage.
         *
         * @param   pinNbr      Identify the pin to configure
         ************************************************************************************************************/
        static void cfgPinInputNoPull(uint32_t pinNbr);

        /********************************************************************************************************//**
         * @brief   Configure the PIN as an input using the pullup on chip, the standard drive and connected.
         *
         * This method configures the PIN specified by `pinNbr` as an input and takes `PULLUP`, `S0S1` and `CONNECT`
         * as default values. It is dedicated to configure GPIO for button's usage.
         *
         * @param   pinNbr      Identify the pin to configure
         ************************************************************************************************************/
        static void cfgPinInputPullup(uint32_t pinNbr);

        /********************************************************************************************************//**
         * @brief   Configure the PIN as an input using the pulldown on chip, the standard drive and connected.
         *
         * This method configures the PIN specified by `pinNbr` as an input and takes `PULLDOWN`, `S0S1` and `CONNECT`
         * as default values. It is dedicated to configure GPIO for button's usage.
         *
         * @param   pinNbr      Identify the pin to configure
         ************************************************************************************************************/
        static void cfgPinInputPullDown(uint32_t pinNbr);

        /********************************************************************************************************//**
         * @brief   Configure the PIN as an output
         *
         * This method configures the PIN specified by `pinNbr` as an output and setup its drive's capability and
         * connection's state.
         *
         * @param   pinNbr      Identify the pin to configure
         * @param   drive       Set the current drive capacity (high or low drain capability).
         * @param   pullSetup   Configure the output with an active pullup, pulldown or idsabled.
         * @param   connect     Specify if the pin is connected or disconnected.
         ************************************************************************************************************/
        static void cfgPinOutput(uint32_t pinNbr, uint32_t drive, uint32_t pullSetup, uint32_t connect);

        /********************************************************************************************************//**
         * @brief   Configure the PIN as an output with pulldown or pullup disabled
         *
         * This method configures the PIN specified by `pinNbr` as an output and takes `PULLDOWN`, the given drive
         * capability and `DISCONNECT` as default values.
         *
         * @param   pinNbr      Identify the pin to configure
         * @param   drive       Set the current drive capacity (high or low drain capability).
         ************************************************************************************************************/
        static void cfgQuickOutput(uint32_t pinNbr, uint32_t drive = GPIO_DEFAULT_DRIVE);

        /********************************************************************************************************//**
         * @brief   Clears the output PINs specified by their mask.
         *
         * @param   pinMask     Identify the pins to clear
         ************************************************************************************************************/
        static void pinMaskClear(uint64_t pinMask);

        /********************************************************************************************************//**
         * @brief   Set the output PINs specified by their mask.
         *
         * @param   pinMask     Identify the pins to set
         ************************************************************************************************************/
        static void pinMaskSet(uint64_t pinMask);

        /********************************************************************************************************//**
         * @brief   Toggle the output  PINs specified by their mask.
         *
         * @param   pinMask     Identify the pins to toggle
         ************************************************************************************************************/
        static void pinMaskToggle(uint64_t pinMask);

        // ZEPHYR -> int gpio_port_toggle_bits(const struct device *port, gpio_port_pins_t pins)

        /********************************************************************************************************//**
         * @brief   Set the output PIN at low level
         *
         * @param   pinNbr      Identify the pin to clear
         * @param   activeHigh  To precise the way the pin should set at low level
         ************************************************************************************************************/
        static void pinClear(uint32_t pinNbr, bool activeHigh = true);


        /********************************************************************************************************//**
         * @brief   Set the output PIN at high level
         *
         * @param   pinNbr      Identify the pin to set
         * @param   activeHigh  To precise the way the pin should set at high level
         ************************************************************************************************************/
        static void pinSet(uint32_t pinNbr, bool activeHigh = true);

        // ZEPHYR -> static inline int gpio_pin_set(const struct device *port, gpio_pin_t pin, 1)

        /********************************************************************************************************//**
         * @brief   Toggle the output PIN
         *
         * @param   pinNbr      Identify the pin to toggle
         ************************************************************************************************************/
        static void pinToggle(uint32_t pinNbr);

        // ZEPHYR -> static inline int gpio_pin_toggle(const struct device *port, gpio_pin_t pin)

        /********************************************************************************************************//**
         * @brief   Write the given value to the output PIN
         *
         * @param   pinNbr      Identify the pin to write to
         * @param   value       The value to write to the output pin.
         ************************************************************************************************************/
        static void pinWrite(uint32_t pinNbr, uint8_t value);

        // ZEPHYR -> static inline int gpio_pin_set(const struct device *port, gpio_pin_t pin, int value)

        /********************************************************************************************************//**
         * @brief   Read the state of the input PIN and returns `true` if the PIN is set and `false` otherwise.
         *
         * @param   pinNbr      Identify the pin to read
         * @param   activeHigh  To know if the pin is active at high level or not. The result will be different...
         *                      For example, if the PIN is active low, the method will return `true` when the signal
         *                      on the PIN is low, whereas if it is set to active high, the method will return
         *                      `false`.
         *                      Default value: `true`
         *
         * @return  `true` if the PIN is set and `false` otherwise.
         ************************************************************************************************************/
        static bool pinRead(uint32_t pinNbr, bool activeHigh = true);

        // ZEPHYR -> static inline int gpio_pin_get(const struct device *port, gpio_pin_t pin)

        /********************************************************************************************************//**
         * @brief   Read the input PINs specified by their mask.
         *
         * @param   pinMask     Identify the pins to read
         ************************************************************************************************************/
        static uint64_t pinMaskRead(uint64_t pinMask);

        // ZEPHYR -> static inline int gpio_port_get(const struct device *port, gpio_port_value_t *value & pinMask)
        #if defined(DEBUG_NRF_USER)
        /********************************************************************************************************//**
         * @brief   Initializes the Hardware GPIO assign to the pin number as an output.
         *
         * For DEBUG usage, this function allows the developer to initialize a specific GPIO as an output.
         *
         * @param   pinNbr      Identify the pin to configure
         ************************************************************************************************************/
        static void initDebugGPIO(uint8_t pinNbr);

        /********************************************************************************************************//**
         * @brief   Toggle the state of the DEBUG GPIO
         *
         * For DEBUG usage, toggles the state of GPIO...
         *
         * @param   pinNbr      Identify the pin to configure
         ************************************************************************************************************/
        static void toggleDebugGPIO(uint8_t pinNbr);
        #endif  // #if defined(DEBUG_NRF_USER)
    protected:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* CONSTRUCTOR SECTION                                                                                      */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        GpioHal()  {}
        ~GpioHal() {}

        static const char* error2msg(int code);

    };  // Class GpioHAL

}   // Namepsace hal

/** @} */
