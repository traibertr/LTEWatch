/****************************************************************************************************************//**
 * Copyright (C) MSE Lausanne Hes-so VAUD/WAADT, Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2023
 * Created by Tristan Traiber
 * All rights reserved.
 *
 * @file    bathal.h
 *
 * @addtogroup LowHAL
 * @{
 *
 * @class   BatHal
 * @brief   Class of Hardware Abstraction Layer to handle battery charger BQ25180 (Texas Instrument)
 *
 * This file contains all necessary functions to handle the battery charger BQ25180 (Texas Instrument) for the
 * LTEWatch prototype board developped during MSE Master Thesis SA 2022
 *
 * Initial author: Tristan Traiber
 * Creation date: 06.01.2023
 *
 * @author  Tristan Traiber (tristan.traiber@master.hes-so.ch)
 * @date    January 2023
 ********************************************************************************************************************/

#ifndef BAT_HAL_ONCE
#define BAT_HAL_ONCE

#include <zephyr.h>
#include <zephyr/kernel.h>

#include "batt-config.h"
#include "drv_bq25180.h"

#include <stdint.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/adc.h>

#define FRAM_I2C_ADDR	DRV_BQ25180_CFG_I2C_ADDR

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

/* ************************************************************************************************************ */
/*                                                                                                              */
/* Class Declaration                                                                                            */
/*                                                                                                              */
/* ************************************************************************************************************ */
class BatHal
{
public:
    /* ******************************************************************************************************** */
    /*																										    */
    /* PUBLIC DECLARATION SECTION                                                                               */
    /*																										    */
    /* ******************************************************************************************************** */

    // Singleton
    /********************************************************************************************************//**
    * @brief    Creat/use singleton instance of BatHal
    *
    * @return   BatHal instance pointer
    ************************************************************************************************************/
    static BatHal* getInstance();

    /********************************************************************************************************//**
    * @brief  Initialize BatHal instance
    *
    * @retval  `true`   If successful
    * @retval  `false`  Otherwise
    ************************************************************************************************************/
    static bool init();

    /********************************************************************************************************//**
    * @brief    Check if BatHal is already initialized
    *
    * @retval  `true`   If BatHal already init
    * @retval  `false`  If BatHal not init
    ************************************************************************************************************/
    static bool isInit();

    /********************************************************************************************************//**
    * @brief    Register an interrupt callback
    *
    * @param    handle  Interrupt handler
    * @param    cb      Callback function
    *
    * @retval  `true`   If successful
    * @retval  `false`  Otherwise
    ************************************************************************************************************/
    bool registerCallback(gpio_callback_handler_t handler, struct gpio_callback *cb);

    /********************************************************************************************************//**
    * @brief    Enable ADC unit
    *
    * @param    enable          `true` to enable ADC, `false` to disable ADC
    *
    * @retval   0               If successful
    * @retval   -EIO            I/O error when accessing an external GPIO chip
    * @retval   -EWOULDBLOCK    If operation would block
    ************************************************************************************************************/
    int32_t enableADC(bool enable);

    /********************************************************************************************************//**
    * @brief    Read value of ADC input
    *
    * @retval   -1  If error occured
    * @retval   ADC input value in mV
    ************************************************************************************************************/
    int32_t readADC();

    /********************************************************************************************************//**
    * @brief    Read data on the I2C serial bus
    *
    * @param    data        Data buffer
    * @param    num_bytes   Size of data in byte
    *
    * @retval   0           If successful
    * @retval   -EIO        General input / output error
    ************************************************************************************************************/
    int readI2C(uint8_t *data, uint32_t num_bytes);

    /********************************************************************************************************//**
    * @brief    Write data on the I2C serial bus
    *
    * @param    data        Data buffer
    * @param    num_bytes   Size of data in byte
    *
    * @retval   0           If successful
    * @retval   -EIO        General input / output error
    ************************************************************************************************************/
    int writeI2C(uint8_t *data, uint32_t num_bytes);

    /********************************************************************************************************//**
    * @brief    Read one byte of data on the I2C serial bus
    *
    * @param    i2c_dev     Pointer to the device structure for the I2C controller driver configured
    * @param    addr        Address of the I2C target device
    * @param    data        Data buffer
    * @param    num_bytes   Size of data in byte
    *
    * @retval   0           If successful
    * @retval   -EIO        General input / output error
    ************************************************************************************************************/
    int read_bytes(const struct device *i2c_dev, uint8_t addr, uint8_t *data, uint32_t num_bytes);

    /********************************************************************************************************//**
    * @brief    Write one byte of data on the I2C serial bus
    *
    * @param    i2c_dev     Pointer to the device structure for the I2C controller driver configured
    * @param    addr        Address of the I2C target device
    * @param    data        Data buffer
    * @param    num_bytes   Size of data in byte
    *
    * @retval   0           If successful
    * @retval   -EIO        General input / output error
    ************************************************************************************************************/
    int write_bytes(const struct device *i2c_dev, uint8_t addr, uint8_t *data, uint32_t num_bytes);

    /********************************************************************************************************//**
    * @brief    Config the ADC channel. It is required to call this function and configure the channel before
    *           sending read requests
    *
    * @retval   `true`      If successful
    * @retval   `false`     If an error occured
    ************************************************************************************************************/
    bool configADC();

    /********************************************************************************************************//**
    * @brief    Config the I2C serial bus
    *
    * @retval   `true`      If successful
    * @retval   `false`     If an error occured
    ************************************************************************************************************/
    bool configI2C();

protected:
    /* ******************************************************************************************************** */
    /*                                                                                                          */
    /* PROTECTED DECLARATION SECTION                                                                            */
    /*                                                                                                          */
    /* ******************************************************************************************************** */
    virtual ~BatHal() {};
    // Singleton pattern
    BatHal();

private:
    /* ******************************************************************************************************** */
    /*                                                                                                          */
    /* PRIVATE DECLARATION SECTION                                                                              */
    /*                                                                                                          */
    /* ******************************************************************************************************** */

    static bool _isInit;    ///< BatHal init status flag
};  // Class BatHal

/** @} */

#endif // BATT_MANAGER_ONCE