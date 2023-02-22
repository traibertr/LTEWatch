/****************************************************************************************************************//**
 * Copyright (C) MSE Lausanne Hes-so VAUD/WAADT, Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2023
 * Created by Tristan Traiber
 * All rights reserved.
 *
 * @file    dispcontroller.h
 *
 * @addtogroup Display
 * @{
 *
 * @class   DispController
 * @brief   Class to drive the LTEWatch display, SHARP (Memmory In Pixel) LCD `LS011B7DH03`
 *
 * Initial author: Tristan Traiber
 * Creation date: 20.01.2023
 *
 * @author  Tristan Traiber (tristan.traiber@master.hes-so.ch)
 * @date    January 2023
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include "config/disp-config.h"
#include "gnsscontroller.h"

#include <zephyr/zephyr.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/spi.h>

#include <stdio.h>
#include <string.h>
#include <lvgl.h>

/* ************************************************************************************************************ */
/*                                                                                                              */
/* Class Declaration                                                                                            */
/*                                                                                                              */
/* ************************************************************************************************************ */
class DispController
{
public:
/* ************************************************************************************************************ */
/* Public Declaration                                                                                           */
/* ************************************************************************************************************ */

typedef enum{
    DISP_MQTT_CONNECTING_1,
    DISP_MQTT_CONNECTING_2,
    DISP_MQTT_CONNECTED,
    DISP_MQTT_DISCONNECT,
    DISP_MQTT_SENDING,
    DISP_MQTT_ERROR,
}mqttState;

    /********************************************************************************************************//**
    * @brief  Initialize DispController instance
    ************************************************************************************************************/
    void init(void);

    /********************************************************************************************************//**
     * @brief   Starts display controller executive framework (start behaviour)
    ************************************************************************************************************/
    void start(void);

    // Singleton
    /********************************************************************************************************//**
    * @brief    Creat/use singleton instance of Display Controller
    *
    * @return   Display Controller instance pointer
    ************************************************************************************************************/
    static DispController* getInstance(void);

    /********************************************************************************************************//**
    * @brief  Display battery information data on LDC
    *
    * @param   batteryLevelPercent  Battery cell level in percent
    * @param   batteryLevelMv       Battery cell level in mV
    * @param   batMvMode            Display battery level in mV/percent on LCD
    * @param   isBatteryCharging    Battery charging status flag
    ************************************************************************************************************/
    void battDisp(uint8_t batteryLevelPercent, uint32_t batteryLevelMv, bool batMvMode, bool isBatteryCharging);

    /********************************************************************************************************//**
    * @brief  Display GNSS `svs` on LDC. `svs`: number of connected satellites
    *
    * @param   svs  Number of satellites connected to the GNSS
    ************************************************************************************************************/
    void gnssDisp(uint8_t svs);

    /********************************************************************************************************//**
    * @brief  Display time on LCD
    *
    * @param   hour     Hour value
    * @param   min      Minute value
    * @param   sec      Second value
    ************************************************************************************************************/
    void timeDisp(uint8_t hour, uint8_t min, uint8_t sec);

    /********************************************************************************************************//**
    * @brief  Display date on LCD
    *
    * @param   year     Year value
    * @param   month    Month value
    * @param   day      Day value
    ************************************************************************************************************/
    void dateDisp(uint16_t year, uint8_t month, uint8_t day);

    /********************************************************************************************************//**
    * @brief  MQTT state on LCD
    *
    * @param   state    MQTT client state
    ************************************************************************************************************/
    void mqttDisp(uint8_t state);

    /********************************************************************************************************//**
    * @brief  Display `SET MOTOR POSITION` menu on LCD
    *
    * @param   motorId    Motor identifier
    ************************************************************************************************************/
    void setMotDisp(uint8_t motorId);

    /********************************************************************************************************//**
    * @brief  Display `SET GNSS ANTENNA` menu on LCD
    *
    * @param   gnssAntMode    GNSS antenna selected mode (on-board/external)
    ************************************************************************************************************/
    void setGnssAnt(uint8_t gnssAntMode);

    /********************************************************************************************************//**
    * @brief  Clean LCD screen (all white screen)
    ************************************************************************************************************/
    void cleanDisp(void);

    /********************************************************************************************************//**
    * @brief  Display `LTEWatch` application running mode
    *
    * @param   modeName    Name of currently running mode
    ************************************************************************************************************/
    void setModeDisp(char* modeName);

protected:
/* ************************************************************************************************************ */
/*  Protected Declaration                                                                                       */
/* ************************************************************************************************************ */
private:
/* ************************************************************************************************************ */
/*  Private Declaration                                                                                         */
/* ************************************************************************************************************ */
    virtual ~DispController() {}
    bool _startFlag;
};