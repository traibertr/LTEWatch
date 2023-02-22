/****************************************************************************************************************//**
 * Copyright (C) MSE Lausanne Hes-so VAUD/WAADT, Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2023
 * Created by Tristan Traiber
 * All rights reserved.
 *
 * @file    batterymanager.h
 *
 * @addtogroup Battery
 * @{
 *
 * @class   BatteryManager
 * @brief   Class to control LTEWatch battery manager module
 *
 * Initial author: Tristan Traiber
 * Creation date: 28.01.2023
 *
 * @author  Tristan Traiber (tristan.traiber@master.hes-so.ch)
 * @date    January 2023
 ********************************************************************************************************************/
#ifndef BATT_MANAGER_ONCE
#define BATT_MANAGER_ONCE

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <zephyr.h>
#include "batt-config.h"
#include "ibatteryobserver.h"
#include "drv_bq25180.h"
#include <picoireactive.h>
#include <picoevent.h>
#include <zephyr/drivers/gpio.h>
#include "isubject.hpp"

/* ************************************************************************************************************ */
/*                                                                                                              */
/* Class Declaration                                                                                            */
/*                                                                                                              */
/* ************************************************************************************************************ */
class BatteryManager : public PicoIReactive, public ISubject<IBatteryObserver, CONFIG_BAT_MAX_OBSERVERS>
{
public:

    typedef struct
    {
        uint8_t     batLvlInPercent;        ///< Battery cell level in percent
        uint32_t    batLvlInMV;             ///< Battery cell level in mV
        bool        isBatCharging;          ///< Battery charging state flag
    }BatData;

    /********************************************************************************************************//**
    * @brief  Initialize BatteryManager instance
    ************************************************************************************************************/
    void init(void);

    /********************************************************************************************************//**
    * @brief   Implements state machine behavior
    *
    * @param   event    Event identifier
    *
    * @retval `true`    When event processed successfully
    * @retval `false`   Otherwise
    ************************************************************************************************************/
    bool processEvent(PicoEvent* event);

    /********************************************************************************************************//**
    * @brief    Send config to the battery charger module (BQ25180)
    *
    * @retval   `true`      If successful
    * @retval   `false`     If an error occured
    ************************************************************************************************************/
    bool configBatteryCharger(void);

    /********************************************************************************************************//**
    * @brief    Print (log) current config of the battery charger module (BQ25180)
    *
    * @retval   `true`      If successful
    * @retval   `false`     If an error occured
    ************************************************************************************************************/
    bool getBatteryChargerConfig(void);

    /********************************************************************************************************//**
    * @brief    Clear battery charger fault flag and status errors registers
    *
    * @retval   `true`      If successful
    * @retval   `false`     If an error occured
    ************************************************************************************************************/
    bool clearBatteryChargerFlags(void);

    /********************************************************************************************************//**
    * @brief    Get battery cell level in mV
    *
    * @retval   -1  If error occured
    * @retval   Battery cell level in mV
    ************************************************************************************************************/
    uint32_t getBatteryLevelInMV(void);

    /********************************************************************************************************//**
    * @brief    Get battery cell level in percent
    *
    * @retval   -1  If error occured
    * @retval   Battery cell level in percent
    ************************************************************************************************************/
    uint32_t getBatteryLevelInPercent(void);

    /********************************************************************************************************//**
    * @brief    Get battery charging status
    *
    * @retval   `true`      If battery is currently charging
    * @retval   `false`     If battery is currently not charging
    ************************************************************************************************************/
    bool isBatteryCharging(void);

    /********************************************************************************************************//**
     * @brief   Starts battery manager executive framework (start behaviour)
    ************************************************************************************************************/
    void start(void);

    /********************************************************************************************************//**
    * @brief    Get battery current data ()
    *
    * @retval   `true`      If battery is currently charging
    * @retval   `false`     If battery is currently not charging
    ************************************************************************************************************/
    void getBatData(BatData* batData);

// Singleton
    /********************************************************************************************************//**
    * @brief    Creat/use singleton instance of Battery Manager
    *
    * @return   Battery Manager instance pointer
    ************************************************************************************************************/
    static BatteryManager* getInstance();

protected:

private:
/* ******************************************************************************************************** */
/*                                                                                                          */
/* PRIVATE DECLARATION SECTION                                                                              */
/*                                                                                                          */
/* ******************************************************************************************************** */
    // Singleton pattern
    BatteryManager();
    virtual ~BatteryManager() {};

    typedef enum BatManState
    {
        ST_START,
        ST_IDLE,
        ST_INIT,
        ST_CONFIG,
        ST_INTERRUPT,
        ST_READ_ADC,
        ST_ERROR,
    } eBatManState;

    eBatManState _state;             ///< Current state of the battery manager

    PicoEvent _evDefault;            ///< default event
    PicoEvent _evInterrupt;          ///< Interrupt event
    PicoEvent _evReadADC;            ///< Read ADC event

    typedef enum BatMngrEvents_ {evInterrupt=125,
                                 evReadADC,
                                 evDefault
                            } BatMngrEvents;

    /********************************************************************************************************//**
    * @brief Push event to the executive framework
    *
    * @param   event     event identifier
    ************************************************************************************************************/
    void pushEvent(PicoEvent* event);

    static uint8_t _batteryChargerBuffer[DRV_BQ25180_NB_REG
                                            + DRV_BQ25180_START_ADDR];  ///< Battery charger reg. buffer

    static uint8_t _BatChrgReadRegBuff[DRV_BQ25180_NB_REG
                                            + DRV_BQ25180_START_ADDR];  ///< Battery charger reg. read buffer

    static struct gpio_callback _BatteryChargerInterruptCallback;   ///< Bat. charger interrupt callback handler

    /********************************************************************************************************//**
    * @brief  Battery Manager interrupt callback function
    *
    * @param   dev      Pointer to the device structure for GPIOTE input
    * @param   cb       Callback function
    * @param   pins     Battery charger interrupt input pin
    ************************************************************************************************************/
    static void _BatteryManagerHandler(const struct device *dev,
                         struct gpio_callback *cb, uint32_t pins);

    /********************************************************************************************************//**
    * @brief  Bind interrupt event from battery charger to battery manager
    *
    * @param   handler  Interrupt handler
    * @param   cb       Callback function
    *
    * @retval `true`    Succefully
    * @retval `false`   Otherwise
    ************************************************************************************************************/
    bool registerBatteryInterrupt(gpio_callback_handler_t handler = _BatteryManagerHandler,
        struct gpio_callback *cb = &_BatteryChargerInterruptCallback);

    int32_t     _batLvl;        ///< Battery cell level value
    int64_t     _average;       ///< Average battery level
    uint16_t    _nbEch;
    int         _errorID;

    /********************************************************************************************************//**
    * @brief   Notify observer about battery manager event
    *
    * The battery manager notify any observer for the following events:
    *
    * `gpioState` can take the values :
    *   - `ST_INTERRUPT`:   Received interrupt from the battery charger
    *   - `ST_READ_ADC`:    Battery level measure is ready
    *   - `ST_ERROR`:       An error occured
    *
    * @param   state        Battery manager event state
    ************************************************************************************************************/
    void _notifyObservers(eBatManState state);

    /********************************************************************************************************//**
    * @brief  Battery Manager k_work callback function
    *
    * @param   work_id     Work identifier
    ************************************************************************************************************/
    static void batterManagerTaskHandler(struct k_work* work_id);

    /********************************************************************************************************//**
    * @brief  Battery Manager k_timer callback function
    *
    * @param   timer_id     Timer identifier
    ************************************************************************************************************/
    static void batterManagerTimerHandler(struct k_timer *timer_id);
};

#endif // BATT_MANAGER_ONCE