/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics.
 * Created by Patrice Rudaz
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "batterymanager.h"
#include <picoxf.h>
#include "bathal.h"
#include "factory.h"
/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#ifndef CONFIG_BAT_LOG_LEVEL
    #define CONFIG_BAT_LOG_LEVEL            3U
#endif

#if (CONFIG_BAT_LOG_LEVEL != 0)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(bat_manager, CONFIG_BAT_LOG_LEVEL);

    #define BAT_DEBUG(args...)              LOG_DBG(args)
    #define BAT_ERROR(args...)              LOG_ERR(args)
    #define BAT_INFO(args...)               LOG_INF(args)
    #define BAT_WARN(args...)               LOG_WRN(args)
#else
    #define BAT_DEBUG(...)                  {(void) (0);}
    #define BAT_ERROR(...)                  {(void) (0);}
    #define BAT_INFO(...)                   {(void) (0);}
    #define BAT_WARN(...)                   {(void) (0);}
#endif

/* **************************************************************************************************************** */
/* Namespace Declaration                                                                                            */
/* **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Static Declaration                                                                                               */
/* **************************************************************************************************************** */
uint8_t                 BatteryManager::_batteryChargerBuffer[DRV_BQ25180_NB_REG + DRV_BQ25180_START_ADDR];
struct gpio_callback    BatteryManager::_BatteryChargerInterruptCallback;
uint8_t                 BatteryManager::_BatChrgReadRegBuff[DRV_BQ25180_NB_REG + DRV_BQ25180_START_ADDR];


void BatteryManager::_BatteryManagerHandler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    getInstance()->pushEvent(&getInstance()->_evInterrupt);
}

void BatteryManager::batterManagerTaskHandler(struct k_work* work_id)
{
    // template
    /*if (work_id == &batTimerWork)
    {
       // Task here
    }*/
}

void BatteryManager::batterManagerTimerHandler(struct k_timer *timer_id)
{
    // template
    /*if (timer_id == &battManagerTimer)
    {
        k_work_submit(&batTimerWork);
    }*/
}

/************************************************************************************************************/
/* Constructor | Destructor                                                                                 */
/************************************************************************************************************/
// ----------------------------------------------------------------------------------------------------------
BatteryManager::BatteryManager()
{
    _evDefault.setId(evDefault);
    _evDefault.setDnd(true);
    _evDefault.setTarget(this);
    _evDefault.setDelay(0);

    _evInterrupt.setId(evInterrupt);
    _evInterrupt.setDnd(true);
    _evInterrupt.setTarget(this);
    _evInterrupt.setDelay(BAT_INTERRUPT_DELAY);

    _evReadADC.setId(evReadADC);
    _evReadADC.setDnd(true);
    _evReadADC.setTarget(this);
    _evReadADC.setDelay(BAT_LVL_PERIODE);
}

// ----------------------------------------------------------------------------------------------------------
void BatteryManager::init(void)
{
    if(!BatHal::getInstance()->isInit())
    {
        BAT_WARN("BatHal was not init -> is init now!");
        BatHal::getInstance()->init();
    }
    _state = ST_START;

    _batLvl = 0;
    _average = 0;
    _nbEch = 0;
}


// ----------------------------------------------------------------------------------------------------------
BatteryManager* BatteryManager::getInstance(void)
{
    static BatteryManager instance;
    return &instance;
}

// ------------------------------------------------------------------------------------------------------------------
bool BatteryManager::registerBatteryInterrupt(gpio_callback_handler_t handler, struct gpio_callback *cb)
{
    Factory::bathal()->registerCallback(handler, cb);
    return true;
}

// ------------------------------------------------------------------------------------------------------------------
void BatteryManager::start(void)
{
    _state = ST_START;
    pushEvent(&_evDefault);
}

// ------------------------------------------------------------------------------------------------------------------
bool BatteryManager::processEvent(PicoEvent* event)
{
    eBatManState oldState = _state;

    // State transition Switch
    switch (_state)
    {
        case ST_START:
        {
            if (event->getId() == evDefault)
            {
                _state = ST_INIT;
            }
            break;
        }
        case ST_INIT:
        {
            if (event->getId() == evDefault)
            {
                _state = ST_CONFIG;
            }
            break;
        }
        case ST_IDLE:
        {
            if (event->getId() == evInterrupt)
            {
                _state = ST_INTERRUPT;
            }
            else if (event->getId() == evReadADC)
            {
                _state = ST_READ_ADC;
            }
            break;
        }
        case ST_CONFIG:
        {
            if (event->getId() == evDefault)
            {
                _state = ST_READ_ADC;
            }
            break;
        }
        case ST_INTERRUPT:
        {
            if (event->getId() == evDefault)
            {
                _state = ST_IDLE;
            }
            break;
        }
        case ST_READ_ADC:
        {
            if (event->getId() == evDefault)
            {
                _state = ST_IDLE;
            }
            break;
        }
        default:
        {
            break;
        }
    }

    if (oldState != _state)
    {
        // Entry action Switch
        switch (_state)
        {
            case ST_INIT:
            {
                BAT_DEBUG("-> ST_INIT");
                if (Factory::bathal()->init())
                {
                    //registerBatteryInterrupt(_BatteryManagerHandler, &_BatteryChargerInterruptCallback);
                    if (Factory::bathal()->configADC())
                    {
                        BAT_DEBUG("ADC config succeed");

                        if (Factory::bathal()->configI2C())
                        {
                            BAT_DEBUG("I2C config succeed");
                        }
                        else
                        {
                            BAT_ERROR("I2C config failed...");
                        }
                    }
                    else
                    {
                        BAT_ERROR("ADC config failed...");
                    }
                    Factory::bathal()->enableADC(true);
                    pushEvent(&_evDefault);
                    }
                    else
                    {
                        BAT_ERROR("Battery hal init failed...");
                        return false;
                    }
                break;
            }
            case ST_IDLE:
            {
                BAT_DEBUG("-> ST_IDLE");
                pushEvent(&_evReadADC);
                break;
            }
            case ST_CONFIG:
            {
                BAT_DEBUG("-> ST_CONFIG");
                configBatteryCharger();
                getBatteryChargerConfig();
                pushEvent(&_evDefault);
                break;
            }
            case ST_INTERRUPT:
            {
                BAT_DEBUG("-> ST_INTERRUPT");
                //getBatteryChargerConfig();
                //configBatteryCharger();
                //clearBatteryChargerFlags();
                _notifyObservers(ST_INTERRUPT);
                pushEvent(&_evDefault);
                break;
            }
            case ST_READ_ADC:
            {
                _nbEch++;
                if (_nbEch <  BAT_LVL_AVERAGE_NB_ECH)
                {
                    if(_nbEch == 1)
                    {
                        PicoXF::getInstance()->unscheduleTM(&_evReadADC);
                        _evReadADC.setDelay(ADC_ECH_PERIODE); // wait 200ms for next ADC read (multiple sample averageing)
                    }
                     _average += Factory::bathal()->readADC();
                }
                else
                {
                    _nbEch   = 0;
                    _batLvl  = _average/BAT_LVL_AVERAGE_NB_ECH;
                    _average = 0;
                    PicoXF::getInstance()->unscheduleTM(&_evReadADC);
                    _evReadADC.setDelay(BAT_LVL_PERIODE); // wait 1000ms for next battery lvl measure
                    //Factory::bathal()->enableADC(false);
                    _notifyObservers(ST_READ_ADC);
                }
                pushEvent(&_evDefault);
                break;
            }
            default:
            {
                BAT_ERROR("Unhandled Action State ! (%d)", _state);
                break;
            }
        }
    }
    return true;
}

// ----------------------------------------------------------------------------------------------------------
void BatteryManager::pushEvent(PicoEvent* event)
{
    PicoXF::getInstance()->pushEvent(event);
}

// ----------------------------------------------------------------------------------------------------------
bool BatteryManager::isBatteryCharging(void)
{
    getBatteryChargerConfig();

    stat0_u8 stat0Reg;
    stat0Reg.reg = _BatChrgReadRegBuff[E_DRV_BQ25180_I2C_REG_OFFSET_STAT0];

    if ((stat0Reg.item.CHG_STAT == 1)||(stat0Reg.item.CHG_STAT == 2))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// ----------------------------------------------------------------------------------------------------------
void BatteryManager::getBatData(BatData* batData)
{
    batData->batLvlInPercent    = getBatteryLevelInPercent();
    batData->batLvlInMV         = getBatteryLevelInMV();
    batData->isBatCharging      = isBatteryCharging();
}

// ----------------------------------------------------------------------------------------------------------
bool BatteryManager::getBatteryChargerConfig(void)
{
    int err = Factory::bathal()->readI2C(_BatChrgReadRegBuff, DRV_BQ25180_NB_REG);
    if(err == 0)
    {
        BAT_DEBUG("STAT0: 0x%02X", _BatChrgReadRegBuff[E_DRV_BQ25180_I2C_REG_OFFSET_STAT0]);
        BAT_DEBUG("STAT1: 0x%02X", _BatChrgReadRegBuff[E_DRV_BQ25180_I2C_REG_OFFSET_STAT1]);
        BAT_DEBUG("FLAG0: 0x%02X", _BatChrgReadRegBuff[E_DRV_BQ25180_I2C_REG_OFFSET_FLAG0]);
        BAT_DEBUG("VBAT_CTRL: 0x%02X", _BatChrgReadRegBuff[E_DRV_BQ25180_I2C_REG_OFFSET_VBAT_CTRL]);
        BAT_DEBUG("ICHG_CTRL: 0x%02X", _BatChrgReadRegBuff[E_DRV_BQ25180_I2C_REG_OFFSET_ICHG_CTRL]);
        BAT_DEBUG("CHARGECTRL0: 0x%02X", _BatChrgReadRegBuff[E_DRV_BQ25180_I2C_REG_OFFSET_CHARGECTRL0]);
        BAT_DEBUG("CHARGECTRL1: 0x%02X", _BatChrgReadRegBuff[E_DRV_BQ25180_I2C_REG_OFFSET_CHARGECTRL1]);
        BAT_DEBUG("IC_CTRL: 0x%02X", _BatChrgReadRegBuff[E_DRV_BQ25180_I2C_REG_OFFSET_IC_CTRL]);
        BAT_DEBUG("TMR_ILIM: 0x%02X", _BatChrgReadRegBuff[E_DRV_BQ25180_I2C_REG_OFFSET_TMR_ILIM]);
        BAT_DEBUG("SHIP_RST: 0x%02X", _BatChrgReadRegBuff[E_DRV_BQ25180_I2C_REG_OFFSET_SHIP_RST]);
        BAT_DEBUG("SYS_REG: 0x%02X", _BatChrgReadRegBuff[E_DRV_BQ25180_I2C_REG_OFFSET_SYS_REG]);
        BAT_DEBUG("TS_CONTROL: 0x%02X", _BatChrgReadRegBuff[E_DRV_BQ25180_I2C_REG_OFFSET_TS_CONTROL]);
        BAT_DEBUG("MASK_ID: 0x%02X", _BatChrgReadRegBuff[E_DRV_BQ25180_I2C_REG_OFFSET_MASK_ID]);
        return true;
    }
    else
    {
        BAT_ERROR("Read battery charger's config failed, err: %d", err);
        return false;
    }
}

// ----------------------------------------------------------------------------------------------------------
bool BatteryManager::clearBatteryChargerFlags(void)
{
    s_drv_bq25180_i2c_reg_t batteryChargerConfig;
    // Set a config buffer to clear all flags of the battery charger
    batteryChargerConfig.STAT1.reg = 0xff;
    batteryChargerConfig.FLAG0.reg = 0xff;

    //Creat the Buffer to send (write) to battery charger
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_STAT1] = batteryChargerConfig.STAT1.reg & DRV_BQ25180_STAT1_WRITE_MASK;
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_FLAG0] = batteryChargerConfig.FLAG0.reg & DRV_BQ25180_FLAG0_WRITE_MASK;

    // Send "clear all flags" command (2 bytes command)
    int ret = Factory::bathal()->writeI2C(&_batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_STAT1], 2);
    if (ret == 0)
    {
        BAT_INFO("Battery charger flags clear Succeed");
        return true;
    }
    else
    {
        BAT_ERROR("Battery charger config failed:%d", ret);
        return false;
    }
}

// ----------------------------------------------------------------------------------------------------------
uint32_t BatteryManager::getBatteryLevelInMV(void)
{
    BAT_DEBUG("Start reading battery level");

    int32_t adcVal = _batLvl;

    // Correct ADC value to be "castable" in uint32_t
    if(adcVal < 0) adcVal *= (-1);

    // Cast to uint32 to have more available space for calcul
    uint32_t batLvlMilliVolt = static_cast<uint32_t>(adcVal + MEASURED_ADC_OFFSET);

    batLvlMilliVolt = (batLvlMilliVolt * (BAT_LVL_DIVIDER_R1 + BAT_LVL_DIVIDER_R2)) / BAT_LVL_DIVIDER_R2; // VB = VA*R2/(R1+R2) -> VA = VB*(R1+R2)/R2
    BAT_DEBUG("Battery voltage: %dmV", batLvlMilliVolt);
    // Operation order is important to not work with float
    return batLvlMilliVolt;
}

// ----------------------------------------------------------------------------------------------------------
uint32_t BatteryManager::getBatteryLevelInPercent(void)
{
    BAT_DEBUG("Start reading battery level");

    int32_t temp = getBatteryLevelInMV();
    uint32_t batLvlPercent = 0;

    if(temp < BAT_LVL_MIN) temp = BAT_LVL_MIN;
    if(temp > BAT_LVL_MAX) temp = BAT_LVL_MAX;

    if (temp < BAT_LVL_10)
    {
        batLvlPercent = 10/((BAT_LVL_10)/(temp));
    }
    else if (temp < BAT_LVL_20)
    {
        batLvlPercent = 10 + 10/((BAT_LVL_20 - BAT_LVL_10)/(temp - BAT_LVL_10));
    }
    else if (temp < BAT_LVL_50)
    {
        batLvlPercent = 20 + 30/((BAT_LVL_50 - BAT_LVL_20)/(temp - BAT_LVL_20));
    }
    else if (temp < BAT_LVL_80)
    {
        batLvlPercent = 50 + 30/((BAT_LVL_80 - BAT_LVL_50)/(temp - BAT_LVL_50));
    }
    else
    {
        batLvlPercent = 80 + 20/((BAT_LVL_MAX - BAT_LVL_80)/(temp - BAT_LVL_80));
    }

    BAT_DEBUG("Battery lvl: %d%%, Full Bat: %dmV, Empty Bat: %dmV", batLvlPercent, BAT_LVL_MAX, BAT_LVL_MIN);

    return batLvlPercent;
}

// ----------------------------------------------------------------------------------------------------------
bool  BatteryManager::configBatteryCharger(void)
{
    s_drv_bq25180_i2c_reg_t batteryChargerConfig;

    /* BQ25180 I2C Config -> see "drv_bq25180.h" or "https://www.ti.com/lit/ds/symlink/bq25180.pdf" */

    BAT_DEBUG("Start setting the battery charger config");
    // Read only register
    batteryChargerConfig.STAT0.reg = DRV_BQ25180_STAT0_DEFAULT_VAL;
    batteryChargerConfig.STAT1.reg = DRV_BQ25180_STAT1_DEFAULT_VAL;
    batteryChargerConfig.FLAG0.reg = DRV_BQ25180_FLAG0_DEFAULT_VAL;
    // Battery Regulation Voltage
    batteryChargerConfig.VBAT_CTRL.reg = DRV_BQ25180_VBAT_CTRL_DEFAULT_VAL;
    batteryChargerConfig.VBAT_CTRL.item.VBATREG = 0b1000110; // VBATREG_CODE = (VBAT_FULL_CHARGE-3.5V)*100 = 70mV -> VBAT_FULL_CHARGE = 4.2V
    // Battery Charge Disable
    batteryChargerConfig.ICHG_CTRL.reg = DRV_BQ25180_ICHG_CTRL_DEFAULT_VAL;
    batteryChargerConfig.ICHG_CTRL.item.CHG_DIS = 0b0; // 0b = Battery Charging Enabled
    // Battery Charging current limit
    batteryChargerConfig.ICHG_CTRL.item.ICHG = 0b1001101; // ICHG = 500mA -> For ICHG > 35mA = 40 + ((ICHGCODE-31)*10)mA ->  ICHGCODE = ((500-40)/10)+31 = 77 = 0b1001101
    // Precharge current = 2*TERM, Termination current = 10% Ichrg, VINDPM Level Selection = disable, Thermal Regulation Threshold = 100C
    batteryChargerConfig.CHARGECTRL0.reg = DRV_BQ25180_CHARGECTRL0_DEFAULT_VAL;
    // Battery Discharge Current Limit
    batteryChargerConfig.CHARGECTRL1.reg = DRV_BQ25180_CHARGECTRL1_DEFAULT_VAL;
    batteryChargerConfig.CHARGECTRL1.item.IBAT_OCP = 0b00; // 00b = 500mA, 01b = 1000mA, 10b = 1500mA, 11b = Disabled
    // Battery Undervoltage LockOut Falling Threshold.
    batteryChargerConfig.CHARGECTRL1.item.BUVLO = 0b001; // = 3.2V -> 000b = 3.4V -> 111b = 2.0V
    // Mask Charging Status Interrupt
    batteryChargerConfig.CHARGECTRL1.item.CHG_STATUS_INT_MASK = 0b0; // Enable Charging Status Interrupt (anytime charging status change)
    //Mask ILIM Fault Interrupt
    batteryChargerConfig.CHARGECTRL1.item.ILIM_INT_MASK = 0b1; // Mask ILIM Interrupt
    //Mask VINDPM and VDPPM Interrupt
    batteryChargerConfig.CHARGECTRL1.item.VDPM_INT_MASK = 0b1; // Mask VINDPM and VDPPM Interrupt
    // TS Auto Function enabled , Precharge Voltage Threshold (VLOWV) = 3V, Recharge Voltage Threshold 100mV, Timer Slow disable, Fast Charge Timer = 6h, Watchdog default Selection
    batteryChargerConfig.IC_CTRL.reg = DRV_BQ25180_IC_CTRL_DEFAULT_VAL;
    batteryChargerConfig.IC_CTRL.item.WATCHDOG_SEL = 0b11;
    // Push button Long Press duration timer = 10s, Hardware reset condition -> long press duration is met, Auto Wake Up Timer Restart = 1s, Input Current Limit Setting = 500mA
    batteryChargerConfig.TMR_ILIM.reg = DRV_BQ25180_TMR_ILIM_DEFAULT_VAL;
    batteryChargerConfig.TMR_ILIM.item.ILIM = 0b110; // Input current limit to 700mA
    // Default ship reset register config
    batteryChargerConfig.SHIP_RST.reg = 0x00;
    // Default system register config
    batteryChargerConfig.SYS_REG.reg = 0x00;
    batteryChargerConfig.SYS_REG.item.SYS_REG_CTRL = 0b001; //SYS Regulation Voltgage = 4.4V
    // Default thermal regulation register config
    batteryChargerConfig.TS_CONTROL.reg = DRV_BQ25180_TS_CONTROL_DEFAULT_VAL;
    // Default MASK_ID register config
    batteryChargerConfig.MASK_ID.reg = 0xF0;

    BAT_DEBUG("Start creating the config buffer for the battery charger");
        //Creat the Buffer to send (write) to battery charger
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_STAT0]       = batteryChargerConfig.STAT0.reg & DRV_BQ25180_STAT0_WRITE_MASK;
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_STAT1]       = batteryChargerConfig.STAT1.reg & DRV_BQ25180_STAT1_WRITE_MASK;
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_FLAG0]       = batteryChargerConfig.FLAG0.reg & DRV_BQ25180_FLAG0_WRITE_MASK;
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_VBAT_CTRL]   = batteryChargerConfig.VBAT_CTRL.reg & DRV_BQ25180_VBAT_CTRL_WRITE_MASK;
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_ICHG_CTRL]   = batteryChargerConfig.ICHG_CTRL.reg & DRV_BQ25180_ICHG_CTRL_WRITE_MASK;
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_CHARGECTRL0] = batteryChargerConfig.CHARGECTRL0.reg & DRV_BQ25180_CHARGECTRL0_WRITE_MASK;
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_CHARGECTRL1] = batteryChargerConfig.CHARGECTRL1.reg & DRV_BQ25180_CHARGECTRL1_WRITE_MASK;
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_IC_CTRL]     = batteryChargerConfig.IC_CTRL.reg & DRV_BQ25180_IC_CTRL_WRITE_MASK;
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_TMR_ILIM]    = batteryChargerConfig.TMR_ILIM.reg & DRV_BQ25180_TMR_ILIM_WRITE_MASK;
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_SHIP_RST]    = batteryChargerConfig.SHIP_RST.reg & DRV_BQ25180_SHIP_RST_WRITE_MASK;
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_SYS_REG]     = batteryChargerConfig.SYS_REG.reg & DRV_BQ25180_SYS_REG_WRITE_MASK;
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_TS_CONTROL]  = batteryChargerConfig.TS_CONTROL.reg & DRV_BQ25180_TS_CONTROL_WRITE_MASK;
    _batteryChargerBuffer[E_DRV_BQ25180_I2C_REG_OFFSET_MASK_ID]     = batteryChargerConfig.MASK_ID.reg & DRV_BQ25180_MASK_ID_WRITE_MASK;

    LOG_HEXDUMP_INF(_batteryChargerBuffer, DRV_BQ25180_NB_REG, "Send the config to the battery charger (i2c):");
    int ret = Factory::bathal()->writeI2C(_batteryChargerBuffer, DRV_BQ25180_NB_REG);
    if (ret == 0)
    {
        BAT_INFO("Battery charger config succeed");
        return true;
    }
    else
    {
        BAT_ERROR("Battery charger config failed:%d", ret);
        return false;
    }
}

// ------------------------------------------------------------------------------------------------------------------
void BatteryManager::_notifyObservers(eBatManState state)
{
    for (uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != nullptr)
        {
            switch (state)
            {
                case ST_INTERRUPT:
                {
                    observer(i)->onInterrupt(this);
                    break;
                }
                case ST_READ_ADC:
                {
                    observer(i)->onBatLvlValReady(this);
                    break;
                }
                case ST_ERROR:
                {
                    observer(i)->onError(this, _errorID);
                    break;
                }
                default:
                    break;
            }
        }
    }
}