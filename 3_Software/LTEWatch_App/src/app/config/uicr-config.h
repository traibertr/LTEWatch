/* Copyright (c) 2022, HES-SO Valais
 * All rights reserved.
 */
#ifndef UICR_CONFIG_H
#define UICR_CONFIG_H

/****************************************************************************************************************//**
 * UICR — User information configuration registers
 *      The user information configuration registers (UICRs) are non-volatile memory (NVM) registers for
 *      configuring user specific settings.
********************************************************************************************************************/

/****************************************************************************************************************//**
 * Define the address in the flash for the UICR[0] register.
 * This register is reserved for DFU feature
 ********************************************************************************************************************/
#define UICR_DFU_RESERVED_OFFSET                        (0x080)

/****************************************************************************************************************//**
 * Define the address in the flash for the UICR[1] register.
 * This register is reserved for the serial number
 ********************************************************************************************************************/
#define UICR_SERIAL_NUMBER_OFFSET                       (0x084)
#define UICR_GET_SERIAL_NUMBER                          0//(*((uint32_t *) (NRF_UICR_BASE + UICR_SERIAL_NUMBER_OFFSET)))

/****************************************************************************************************************//**
 * Define the address in the flash for the UICR[2] register.
 * This register stores the suffix to add to the device name
 ********************************************************************************************************************/
#define UICR_DEVICE_NAME_SUFFIX_OFFSET                  (0x088)
#define UICR_GET_DEVICE_NAME_SUFFIX                     0//(*((uint32_t *) (NRF_UICR_BASE + UICR_DEVICE_NAME_SUFFIX_OFFSET)))

/****************************************************************************************************************//**
 * Define the address in the flash for the UICR[4] register.
 * This register stores the value to be inhibited, according to the LFCK's measurements at 8 degree celsius
 ********************************************************************************************************************/
#define UICR_BLE_ADVERTISE_MODE_OFFSET                  (0x08C)
#define UICR_GET_BLE_ADVERTISE_MODE                     0//(*((uint32_t *) (NRF_UICR_BASE + UICR_BLE_ADVERTISE_MODE_OFFSET)))

/****************************************************************************************************************//**
 * Define the address in the flash for the UICR[3] register.
 * This register stores the value to be inhibited, according to the LFCK's measurements at 8 degree celsius
 ********************************************************************************************************************/
#define UICR_INHIBITION_VALUE_8C_OFFSET                 (0x090)
#define UICR_GET_INHIBITION_VALUE_8C                    0//(*((uint32_t *) (NRF_UICR_BASE + UICR_INHIBITION_VALUE_8C_OFFSET)))

/****************************************************************************************************************//**
 * Define the address in the flash for the UICR[4] register.
 * This register stores the value to be inhibited, according to the LFCK's measurements at 23 degree celsius
 ********************************************************************************************************************/
#define UICR_INHIBITION_VALUE_23C_OFFSET                (0x094)
#define UICR_GET_INHIBITION_VALUE_23C                   0//(*((uint32_t *) (NRF_UICR_BASE + UICR_INHIBITION_VALUE_23C_OFFSET)))

/****************************************************************************************************************//**
 * Define the address in the flash for the UICR[5] register.
 * This register stores the value to be inhibited, according to the LFCK's measurements at 38 degree celsius
 ********************************************************************************************************************/
#define UICR_INHIBITION_VALUE_38C_OFFSET                (0x098)
#define UICR_GET_INHIBITION_VALUE_38C                   0//(*((uint32_t *) (NRF_UICR_BASE + UICR_INHIBITION_VALUE_38C_OFFSET)))

/****************************************************************************************************************//**
 * Define the address in the flash for the UICR[6] register.
 * This register stores the interval which nust be used to apply the ihnibition's value .
 ********************************************************************************************************************/
#define UICR_INTERVAL_INHIBITION_OFFSET                 (0x09C)
#define UICR_GET_INTERVAL_INHIBITION                    0//(*((uint32_t *) (NRF_UICR_BASE + UICR_INTERVAL_INHIBITION_OFFSET)))

/****************************************************************************************************************//**
 * Define the address in the flash for the UICR[7] register.
 * This register stores the calibration value for the battery .
 ********************************************************************************************************************/
#define UICR_BATTERY_CALIBRATION_OFFSET                 (0x0A0)
#define UICR_GET_BATTERY_CALIBRATION                    0//(*((uint32_t *) (NRF_UICR_BASE + UICR_BATTERY_CALIBRATION_OFFSET)))


#endif // UICR_CONFIG_H
