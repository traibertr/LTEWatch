/****************************************************************************************************************//**
 * Copyright (C) MSE Lausanne Hes-so VAUD/WAADT, Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2023
 * Modified by Tristan Traiber
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * \file    platform-config.h
 * @brief   This file is used to enable/disable all the different peripherals and features for PCA10056 project
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2018-01-11
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

// ------------------------------------------------------------------------------------------------------------------
#ifndef UNUSED
    #define UNUSED(x) ((void)0)
#endif // UNUSED

#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
/* **************************************************************************************************************** */
/*                                                                                                                  */
/* APPLICATION defintion                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
#define APP_TIMER_MODE_SINGLE_SHOT      false
#define APP_TIMER_MODE_REPEATED         true

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* COMPONENTS, DRIVERS & LIBRARIES defintion                                                                        */
/*                                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
// XF
#define USE_XF                                                      1

// ------------------------------------------------------------------------------------------------------------------
// Button's component related
#define USE_BUTTONS                                                 1

// ------------------------------------------------------------------------------------------------------------------
// GPIO related
#define USE_GPIOS                                                   0

// ------------------------------------------------------------------------------------------------------------------
// Motor's component related
#define USE_MOTORS                                                  1

#define TICK_AT_NOON                                                1
#define NB_STEPS_FOR_MOTOR_INIT                                     10

// ------------------------------------------------------------------------------------------------------------------
// LEDs related
#define USE_LEDS                                                    0

// ------------------------------------------------------------------------------------------------------------------
// Watchdog's component related
#define USE_WATCHDOG                                                0
#define WATCHDOG_DELAY                                              10000

// ------------------------------------------------------------------------------------------------------------------
// External flash
#define USE_FLASH                                                   0
// ------------------------------------------------------------------------------------------------------------------
// Calendar
#define USE_CALENDAR                                                1
// ------------------------------------------------------------------------------------------------------------------
// SPI
#define USE_SPI                                                     0
// ------------------------------------------------------------------------------------------------------------------
// I2C
#define I2C_DEV                     DEVICE_DT_GET(DT_NODELABEL(i2c1))
#define I2C_SDA_PIN                 DEVICE_DT_GET(DT_PHANDLE_BY_IDX(I2C_DEV, pinctrl_0, 0))
#define I2C_Scl_PIN                 DEVICE_DT_GET(DT_PHANDLE_BY_IDX(I2C_DEV, pinctrl_0, 1))
// ------------------------------------------------------------------------------------------------------------------
// MQTT
# define USE_MQTT                                                   1
// ------------------------------------------------------------------------------------------------------------------
// GNSS
# define USE_GNSS                                                   1

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* BOARD HW CONFIGURATION                                                                                           */
/*                                                                                                                  */
/* **************************************************************************************************************** */

// ------------------------------------------------------------------------------------------------------------------
// Device Name, Model Number, ...

#define DEVICE_NAME_MAX_SIZE  18

#define DEVICE_NAME "nRF9160_DK"

#define NRFX_RTC_DRIVER 1

// ------------------------------------------------------------------------------------------------------------------
// LEDs definitions

#define LEDS_NUMBER                         DT_PROP(DT_PATH(zephyr_user), led_max_number)
#define LED_ACTIVE_LOW                      1

#define LED_1                               DT_GPIO_PIN(DT_ALIAS(led0), gpios)//15 // p0.02 -> nRF91_LED1
#define LED_2                               DT_GPIO_PIN(DT_ALIAS(led1), gpios)//14 // p0.03 -> nRF91_LED2
#define LED_3                               DT_GPIO_PIN(DT_ALIAS(led2), gpios)//13 // p0.04 -> nRF91_LED3

#define LEDS_LIST                           {LED_1, LED_2, LED_3}

// ------------------------------------------------------------------------------------------------------------------
// Button's Component
#define BUTTONS_NUMBER                      DT_PROP(DT_PATH(zephyr_user), button_max_number)
#define BUTTON_PULL_SETUP                   NRF_GPIO_PIN_PULLUP                                                         ///< @brief General configuration for the GPIO's pullup. Here only Pullup or pulldown config MUST be used !!!
#define BTN_IS_ACTIVE_HIGH	            	0

#define BUTTON_1                            DT_GPIO_PIN(DT_ALIAS(bt0), gpios)//19   // p0.06 -> BT0
#define BUTTON_2                            DT_GPIO_PIN(DT_ALIAS(bt1), gpios)//18   // p0.07 -> BT1
#define BUTTON_3                            DT_GPIO_PIN(DT_ALIAS(bt2), gpios)//17   // p0.08 -> BT2

// ------------------------------------------------------------------------------------------------------------------
// SPI's Component related
#if(USE_SPI != 0)
    #define SPI_SCK_PIN                         33 //19  // P0.19                                                                // SPI clock GPIO pin number.
    #define SPI_MOSI_PIN                        34 //20  // P0.20                                                                // SPI Master Out Slave In GPIO pin number.
    #define SPI_MISO_PIN                        35 //21  // P0.21                                                                // SPI Master In Slave Out GPIO pin number.
    #define SPI_SS_PIN                          36 //17  // P0.17                                                                // SPI Slave Select GPIO pin number.
    #if !defined(SPI_MASTER)
        #define SPI_IRQ_PIN                     30   // SALEA 4                                                             ///< @brief In slave mode, we need the master to activate SPI transmission if we have somthing to sy... This GPIO is used to notify the SPI master device to read from slave
        #define SPI_CS_PIN                      27   // This is a mirror of the CS line to let know a SPI slave when a SPI communication is active. As the CS line is driven by the SPI hardware itself, we do NOT have access to this GPIO.
        #define SPI_IRQ_PIN_PULL_CONFIG         NRF_GPIO_PIN_NOPULL
    #endif // #if !defined(SPI_MASTER)
    // #define SPI_SLAVE_BUSY_PIN                  31   // A GPIO line to let know the SPI Master that the SPI Salve is busy (setting up its TX buffer, for example)
    #endif
// ------------------------------------------------------------------------------------------------------------------
// Accelerometer related
//#define GPIO_VCC_ACC                        26  // P0.26
//#define GPIO_INT_ACC                        7
//#define GPIO_INT_ACC_PULL_CONFIG            NRF_GPIO_PIN_NOPULL

// ------------------------------------------------------------------------------------------------------------------
// Pin Test related
//#define PIN_TEST_RX                           3   // P0.03
//#define PIN_TEST_TX                           4   // P0.04
#define PIN_TEST_PULL_CONFIG                    NRF_GPIO_PIN_NOPULL

// -----------------------------------------------------------------------------------------------------------
// Motor's Component related
#define MOTORS_CONN_SERIES                  0
#define COMMON_COIL_C                       1
#define NB_MAX_MOTORS                       DT_PROP(DT_PATH(zephyr_user), motor_max_number)

#if (MOTORS_CONN_SERIES != 0)
    #define M1_CA_PIO                       17  // P1.01
    #define M1_CB_PIO                       18  // P1.02
    #define M1_CC_PIO                       16  // P1.03

    #define M2_CA_PIO                       34  // P1.02
    #define M2_CB_PIO                       36  // P1.04
    #define M2_CC_PIO                       37  // P1.05

    #define M3_CA_PIO                       36  // P1.04
    #define M3_CB_PIO                       38  // P1.06
    #define M3_CC_PIO                       39  // P1.07
#else
    #define M1_CA_PIO                       DT_GPIO_PIN(DT_PATH(zephyr_user), motor_d1m1_gpios) //0//17
    #define M1_CB_PIO                       DT_GPIO_PIN(DT_PATH(zephyr_user), motor_d1com_gpios) //1//19
    #define M1_CC_PIO                       DT_GPIO_PIN(DT_PATH(zephyr_user), motor_d1m2_gpios) //2//18

    #define M2_CA_PIO                       DT_GPIO_PIN(DT_PATH(zephyr_user), motor_d2m1_gpios)//5//23
    #define M2_CB_PIO                       DT_GPIO_PIN(DT_PATH(zephyr_user), motor_d2com_gpios)//4//24
    #define M2_CC_PIO                       DT_GPIO_PIN(DT_PATH(zephyr_user), motor_d2m2_gpios)//3//22

    #define M3_CA_PIO                       DT_GPIO_PIN(DT_ALIAS(motord3m1), gpios)//15//27
    #define M3_CB_PIO                       DT_GPIO_PIN(DT_ALIAS(motord3com), gpios)//14//28
    #define M3_CC_PIO                       DT_GPIO_PIN(DT_ALIAS(motord3m2), gpios)//13//26

#endif

#define M1_STEPS                            180
#define M1_NBS                              1
#define M1_CCW                              1

#define M2_STEPS                            180
#define M2_NBS                              1
#define M2_CCW                              1

#define M3_STEPS                            180
#define M3_NBS                              1
#define M3_CCW                              0
