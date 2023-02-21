/**
 * @file    drv_bq25180.h
 * @author  Tristan Traiber
 * @date    23.02.2023
 * @brief   Driver interface header for battery charger BQ25180 I2C Controlled,
 *          1-Cell, 1-A Linear Battery Charger with Power Path and Ship Mode
 * @see     https://www.ti.com/lit/ds/symlink/bq25180.pdf
*/

#ifndef _DRV_BQ25180_H_
#define _DRV_BQ25180_H_

/******************************************************************************
 *** INCLUDE FILES
 *****************************************************************************/
#include <zephyr.h>

/******************************************************************************
 *** CONSTANT / DEFINE
 ******************************************************************************/
// The device 7-bit address is 0x6A (8-bit shifted address is 0xD4)
#define DRV_BQ25180_CFG_I2C_ADDR            (0x6A)
#define DRV_BQ25180_CFG_I2C_ADDR_8B         (0xD4)
#define DRV_BQ25180_START_ADDR              0
#define DRV_BQ25180_NB_REG                  13 + DRV_BQ25180_START_ADDR

#define DRV_BQ25180_STAT0_DEFAULT_VAL       (0x00)
#define DRV_BQ25180_STAT1_DEFAULT_VAL       (0x07)
#define DRV_BQ25180_FLAG0_DEFAULT_VAL       (0xFF)
#define DRV_BQ25180_VBAT_CTRL_DEFAULT_VAL   (0x46)
#define DRV_BQ25180_ICHG_CTRL_DEFAULT_VAL   (0x05)
#define DRV_BQ25180_CHARGECTRL0_DEFAULT_VAL (0x2C)
#define DRV_BQ25180_CHARGECTRL1_DEFAULT_VAL (0x56)
#define DRV_BQ25180_IC_CTRL_DEFAULT_VAL     (0x84)
#define DRV_BQ25180_TMR_ILIM_DEFAULT_VAL    (0x4D)
#define DRV_BQ25180_SHIP_RST_DEFAULT_VAL    (0x11)
#define DRV_BQ25180_SYS_REG_DEFAULT_VAL     (0x40)
#define DRV_BQ25180_TS_CONTROL_DEFAULT_VAL  (0x00)
#define DRV_BQ25180_MASK_ID_DEFAULT_VAL     (0xC0)

#define DRV_BQ25180_STAT0_WRITE_MASK        (0x00)
#define DRV_BQ25180_STAT1_WRITE_MASK        (0x07)
#define DRV_BQ25180_FLAG0_WRITE_MASK        (0xFF)
#define DRV_BQ25180_VBAT_CTRL_WRITE_MASK    (0x7F)
#define DRV_BQ25180_ICHG_CTRL_WRITE_MASK    (0xFF)
#define DRV_BQ25180_CHARGECTRL0_WRITE_MASK  (0xFF)
#define DRV_BQ25180_CHARGECTRL1_WRITE_MASK  (0xFF)
#define DRV_BQ25180_IC_CTRL_WRITE_MASK      (0xFF)
#define DRV_BQ25180_TMR_ILIM_WRITE_MASK     (0xFF)
#define DRV_BQ25180_SHIP_RST_WRITE_MASK     (0xFF)
#define DRV_BQ25180_SYS_REG_WRITE_MASK      (0xEF)
#define DRV_BQ25180_TS_CONTROL_WRITE_MASK   (0xFF)
#define DRV_BQ25180_MASK_ID_WRITE_MASK      (0xF0)


/***********************************************************************************************
 * MASK BIT             | ACTION
 * ---------------------+-----------------------------------------------------------------------
 *  ILIM_INT_MASK       | Do not issue an /INT pulse when ILIM limiting occurs
 *  VDPM_INT_MASK       | Do not issue an /INT pulse when VINDPM or DDPM is active
 *  TS_INT_MASK         | Do not issue an /INT pulse when any of the TS events have occured.
 *  TREG_INT_MASK       | Do not issue an /INT pulse when TREG is actively reducing the current
 *  PG_INT_MASK         | Do not issue an /INT pulse when VIN meets VIN_PG condition
 *  BAT_INT_MASK        | Do not issue an /INT pulse when BATOCP or BUVLO event is triggered
 *  CHG_STATUS_INT_MASK | Do not send an interrupt anytime there is a charging status change
 ***********************************************************************************************/
#define DRV_BQ25180_MASK_BIT
#define DRV_BQ25180_ILIM_INT_MASK
#define DRV_BQ25180_VDPM_INT_MASK
#define DRV_BQ25180_TS_INT_MASK
#define DRV_BQ25180_TREG_INT_MASK
#define DRV_BQ25180_PG_INT_MASK
#define DRV_BQ25180_BAT_INT_MASK
#define DRV_BQ25180_CHG_STATUS_INT_MASK

/******************************************************************************
 *** DATA TYPE / STRUCTURE DEFINITION / ENUM
 *****************************************************************************/

/** SYS Power Control (SYS_MODE bit control) */
/*********************************************************************************
 * SYS_MODE | DESCRIPTION               | SYS SUPPLY | SYS PULLDOWN
 * ---------+---------------------------+------------+----------------------------
 * 00       | Normal Operation          | IN or BAT  | Off except during HW reset
 * 01       | Force BAT power (IN open) | BAT        | Off except during HW reset
 * 10       | SYS Off – Floating        | None       | Off
 * 11       | SYS Off – Pulled Down     | None       | On
 ********************************************************************************/
typedef enum
{
    E_DRV_BQ25180_SYSPWR_NORMAl_OPERATION   = 0x0,
    E_DRV_BQ25180_SYSPWR_FORCE_BAT_POWER    = 0x1,
    E_DRV_BQ25180_SYSPWR_SYS_OFF_FLOATING   = 0x2,
    E_DRV_BQ25180_SYSPWR_SYS_OFF_PULLDOWN   = 0x3
}   e_drv_bq25180_sys_power_ctrl_mode_t;

/** SYS Voltage Regulation Settings */
typedef enum
{
    E_DRV_BQ25180_VOLT_REG_VBAT     = 0x0, ///< VBAT + 225 mV (3.8 V minimum)
    E_DRV_BQ25180_VOLT_REG_4V4      = 0x1, ///< 4.4 Volt
    E_DRV_BQ25180_VOLT_REG_4V5      = 0x2, ///< 4.5 Volt
    E_DRV_BQ25180_VOLT_REG_4V6      = 0x3, ///< 4.6 Volt
    E_DRV_BQ25180_VOLT_REG_4V7      = 0x4, ///< 4.7 Volt
    E_DRV_BQ25180_VOLT_REG_4V8      = 0x5, ///< 4.8 Volt
    E_DRV_BQ25180_VOLT_REG_4V9      = 0x6, ///< 4.9 Volt
    E_DRV_BQ25180_VOLT_REG_PTHRGT   = 0x7  ///< Pass through
}   e_drv_bq25180_sys_volt_reg_t;

/** Watchdog Settings */
typedef enum
{
    E_DRV_BQ25180_WDOG_ST_RST   = 0x0, ///< Device will only perform a software reset after 160s of the last I2C transaction
    E_DRV_BQ25180_WDOG_HW_RST_1 = 0x1, ///< Device will issue a HW_Reset after 160s of last I2C transation
    E_DRV_BQ25180_WDOG_HW_RST_2 = 0x2, ///< Device will issue a HW_Reset after 40s of the last I2C transaction
    E_DRV_BQ25180_WDOG_DISABLE  = 0x3  ///< Watchdog functionality is completely disabled
}   e_drv_bq25180_watchdog_reg_t;

/** Watchdog Settings */
typedef enum
{
    E_DRV_BQ25180_I2C_REG_OFFSET_STAT0          = 0x0 + DRV_BQ25180_START_ADDR, ///< Charger Status
    E_DRV_BQ25180_I2C_REG_OFFSET_STAT1          = 0x1 + DRV_BQ25180_START_ADDR, ///< Charger Status and Faults
    E_DRV_BQ25180_I2C_REG_OFFSET_FLAG0          = 0x2 + DRV_BQ25180_START_ADDR, ///< Charger Flag Registers
    E_DRV_BQ25180_I2C_REG_OFFSET_VBAT_CTRL      = 0x3 + DRV_BQ25180_START_ADDR, ///< Battery Voltage Control
    E_DRV_BQ25180_I2C_REG_OFFSET_ICHG_CTRL      = 0x4 + DRV_BQ25180_START_ADDR, ///< Fast Charge Current Control
    E_DRV_BQ25180_I2C_REG_OFFSET_CHARGECTRL0    = 0x5 + DRV_BQ25180_START_ADDR, ///< Charger Control 0
    E_DRV_BQ25180_I2C_REG_OFFSET_CHARGECTRL1    = 0x6 + DRV_BQ25180_START_ADDR, ///< Charger Control 1
    E_DRV_BQ25180_I2C_REG_OFFSET_IC_CTRL        = 0x7 + DRV_BQ25180_START_ADDR, ///< IC Control
    E_DRV_BQ25180_I2C_REG_OFFSET_TMR_ILIM       = 0x8 + DRV_BQ25180_START_ADDR, ///< Timer and Input Current Limit Control
    E_DRV_BQ25180_I2C_REG_OFFSET_SHIP_RST       = 0x9 + DRV_BQ25180_START_ADDR, ///< Shipmode, Reset and Pushbutton Control
    E_DRV_BQ25180_I2C_REG_OFFSET_SYS_REG        = 0xA + DRV_BQ25180_START_ADDR, ///< SYS Regulation Voltage Control
    E_DRV_BQ25180_I2C_REG_OFFSET_TS_CONTROL     = 0xB + DRV_BQ25180_START_ADDR, ///< TS Control
    E_DRV_BQ25180_I2C_REG_OFFSET_MASK_ID        = 0xC + DRV_BQ25180_START_ADDR  ///< MASK and Device ID
}   e_drv_bq25180_i2c_reg_offset_t;

/** STAT0 Register */
    typedef struct{
      unsigned VIN_PGOOD_STAT        : 1; ///< VIN Power Good
      unsigned THERMREG_ACTIVE_STAT  : 1; ///< Thermal Regulation Active
      unsigned VINDPM_ACTIVE_STAT    : 1; ///< VINDPM Mode Active
      unsigned VDPPM_ACTIVE_STAT     : 1; ///< VDPPM Mode Active
      unsigned ILIM_ACTIVE_STAT      : 1; ///< Input Curent Limit Active
      unsigned CHG_STAT              : 2; ///< Charging Status Indicator
      unsigned TS_OPEN_STAT          : 1; ///< TS Open Status
    } sRegStat0;

    typedef union
    {
        sRegStat0 item;
        uint8_t reg;
    } stat0_u8;

/** STAT1 Register */
    typedef struct{
      unsigned WAKE2_FLAG               : 1; ///< Wake 2 Timer Flag
      unsigned WAKE1_FLAG               : 1; ///< Wake 1 Timer Flag
      unsigned SAFETY_TMR_FAULT_FLAG    : 1; ///< Safety Timer Expired Fault Cleared only after CE is toggle
      unsigned TS_STAT                  : 2; ///< TS Status
      unsigned RESERVED                 : 1; ///< Reserved
      unsigned BUVLO_STAT               : 1; ///< Battery UVLO Status
      unsigned VIN_OVP_STAT             : 1; ///< VIN_OVP Fault
    } sRegStat1;

    typedef union
    {
        sRegStat1 item;
        uint8_t reg;
    } stat1_u8;

/** FLAG0 Register */
    typedef struct{
      unsigned BAT_OCP_FAULT        : 1; ///< Battery overcurrent protection
      unsigned BUVLO_FAULT_FLAG     : 1; ///< Battery undervoltage FLAG
      unsigned VIN_OVP_FAULT_FLAG   : 1; ///< VIN_OVP FLAG
      unsigned THERMREG_ACTIVE_FLAG : 1; ///< Thermal Regulation FLAG
      unsigned VINDPM_ACTIVE_FLAG   : 1; ///< VINDPM FLAG
      unsigned VDPPM_ACTIVE_FLAG    : 1; ///< VDPPM FLAG
      unsigned ILIM_ACTIVE_FLAG     : 1; ///< ILIM Active
      unsigned TS_FAULT             : 1; ///< TS_Fault
    } sRegFlag0;

    typedef union
    {
        sRegFlag0 item;
        uint8_t reg;
    } flag0_u8;

/** VBAT_CTRL Register */
    typedef struct{
      unsigned VBATREG  : 7; ///< Battery Regulation Voltage VBATREG= 3.5V + VBATREG_CODE *10mV (Max 4.65V)
      unsigned RESERVED : 1; ///< Reserved
    } sRegVbatCtrl;

    typedef union
    {
        sRegVbatCtrl item;
        uint8_t reg;
    } vbatCtrl_u8;

/** ICHG_CTRL Register */
    typedef struct{
      unsigned ICHG     : 7; ///< For ICHG <= 35mA = ICHGCODE +5mA For ICHG > 35mA = 40+((ICHGCODE-31)*10)mA (Max 1A)
      unsigned CHG_DIS  : 1; ///< Charge Disable
    } sRegIchgCtrl;

    typedef union
    {
        sRegIchgCtrl item;
        uint8_t reg;
    } ichgCtrl_u8;

/** CHARGECTRL0 Register */
    typedef struct{
      unsigned THERM_REG : 2; ///< Thermal Regulation Threshold
      unsigned VINDPM    : 2; ///< VINDPM Level Selection
      unsigned ITERM     : 2; ///< Termination current = % of Icharge
      unsigned IPRECHG   : 1; ///< Precharge current = x times of term
      unsigned RESERVED  : 1; ///< Reserved
    } sRegChrgCtrl0;

    typedef union
    {
        sRegChrgCtrl0 item;
        uint8_t reg;
    } chrgCtrl0_u8;

/** CHARGECTRL1 Register */
    typedef struct{
      unsigned VDPM_INT_MASK        : 1; ///< Mask VINDPM and VDPPM Interrupt
      unsigned ILIM_INT_MASK        : 1; ///< Mask ILIM Fault Interrupt
      unsigned CHG_STATUS_INT_MASK  : 1; ///< Mask Charging Status Interrupt
      unsigned BUVLO                : 3; ///< Battery Undervoltage LockOut Falling Threshold.
      unsigned IBAT_OCP             : 2; ///< Battery Discharge Current Limit
    } sRegChrgCtrl1;

    typedef union
    {
        sRegChrgCtrl1 item;
        uint8_t reg;
    } chrgCtrl1_u8;

/** IC_CTRL Register */
    typedef struct{
      unsigned WATCHDOG_SEL : 2; ///< Watchdog Selection
      unsigned SAFETY_TIMER : 2; ///< Fast Charge Timer
      unsigned TMR2X_EN     : 1; ///< Timer Slow
      unsigned VRCH_0       : 1; ///< Recharge Voltage Threshold
      unsigned VLOWV_SEL    : 1; ///< Precharge Voltage Threshold (VLOWV)
      unsigned TS_EN        : 1; ///< TS Auto Function
    } sRegIcCtrl;

    typedef union
    {
        sRegIcCtrl item;
        uint8_t reg;
    } icCtrl_u8;

/** TMR_ILIM Register */
    typedef struct{
      unsigned ILIM         : 3; ///< Input Current Limit Setting
      unsigned AUTOWAKE     : 2; ///< Auto Wake Up Timer Restart
      unsigned MR_RESET_VIN : 1; ///< Hardware reset condition
      unsigned MR_LPRESS    : 2; ///< Push button Long Press duration timer
    } sRegTmrILim;

    typedef union
    {
        sRegTmrILim item;
        uint8_t reg;
    } tmrILim_u8;

/** SHIP_RST Register */
    typedef struct{
      unsigned EN_PUSH          : 1; ///< Enable Push Button and Reset Function on Battery Only
      unsigned WAKE2_TMR        : 1; ///< Wake 2 Timer item
      unsigned WAKE1_TMR        : 1; ///< Wake 1 Timer item
      unsigned PB_LPRESS_ACTION : 2; ///< Pushbutton long press action
      unsigned EN_RST_SHIP      : 2; ///< Shipmode Enable and Hardware Reset
      unsigned REG_RST          : 1; ///< Software Reset

    } sRegShipRst;

    typedef union
    {
        sRegShipRst item;
        uint8_t reg;
    } shipRst_u8;


/** SYS_REG Register */
    typedef struct{
      unsigned VDPPM_DIS        : 1; ///< Disable VDPPM
      unsigned WATCHDOG_15S_EN  : 1; ///< I2C Watchdog
      unsigned SYS_MODE         : 2; ///< Sets how SYS is powered in any state, except SHIPMODE
      unsigned RESERVED         : 1; ///< Reserved
      unsigned SYS_REG_CTRL     : 3; ///< SYS Regulation Voltgage
    } sRegSysReg;

    typedef union
    {
        sRegSysReg item;
        uint8_t reg;
    } sysReg_u8;

/** TS_CONTROL Register */
    typedef struct{
      unsigned TS_VRCG  : 1; ///< Reduced target battery voltage during Warm
      unsigned TS_ICHG  : 1; ///< Fast charge current when decreased by TS function
      unsigned TS_COOL  : 1; ///< TS Cool threshold register
      unsigned TS_WARM  : 1; ///< TS Warm threshold
      unsigned TS_COLD  : 2; ///< TS Cold threshold register
      unsigned TS_HOT   : 2; ///< TS Hot threshold register
    } sRegTsCtrl;

    typedef union
    {
        sRegTsCtrl item;
        uint8_t reg;
    } tsCtrl_u8;

/** MASK_ID Register */
    typedef struct{
      unsigned Device_ID     : 4; ///< Device ID
      unsigned PG_INT_MASK   : 1; ///< Mask PG and VINOVP
      unsigned BAT_INT_MASK  : 1; ///< Mask BATOCP and BUVLO
      unsigned TREG_INT_MASK : 1; ///< Mask TREG
      unsigned TS_INT_MASK   : 1; ///< Mask TS
    } sRegMaskId;

    typedef union
    {
        sRegMaskId item;
        uint8_t reg;
    } maskId_u8;

typedef struct
{
    stat0_u8 STAT0;
    stat1_u8 STAT1;
    flag0_u8 FLAG0;
    vbatCtrl_u8 VBAT_CTRL;
    ichgCtrl_u8 ICHG_CTRL;
    chrgCtrl0_u8 CHARGECTRL0;
    chrgCtrl1_u8 CHARGECTRL1;
    icCtrl_u8 IC_CTRL;
    tmrILim_u8 TMR_ILIM;
    shipRst_u8 SHIP_RST;
    sysReg_u8 SYS_REG;
    tsCtrl_u8 TS_CONTROL;
    maskId_u8 MASK_ID;
}   s_drv_bq25180_i2c_reg_t;

#endif    // END of _DRV_BQ25180_H_