/* Copyright (c) 2022, HES-SO Valais
 * All rights reserved.
 */
#ifndef BATT_CONFIG_H
#define BATT_CONFIG_H

#include <platform-config.h>

/***********************************************************************************************
 * Battery  : 03535 3.8v 580mAh round lipo battery cells
 * Link     : https://www.vatsbattery.com/product/503535-3-8v-580mah-round-lipo-battery-cells
 *
 *  - Cell Dimension        : 5.0±0.2*35±0.5*35±0.5mm
 *  - Charge voltage        : 4.35V±0.03V
 *  - Nominal capacity      : 580mAh
 *  - Fully charge voltage  : 4.35V
 *  - Charge current
 *      - Standard Charging : 0.2C (116mA)
 *      - Rapid charge      : 1.0C (580mA)
 *  - Charging method
 *      1. CC of 0.2c/1.0c to 4.2V,
 *      2. CV 4.2v until Ichrg < 0.01C (5.8mA)
 *  - Discharge cut-off voltage : 3.0V
 *  - Operating environment
 *      - Charging      :  0℃ ~ 45℃
 *      - Discharging   : -20℃ ~ 60℃
 *
************************************************************************************************/
#define CONFIG_BAT_MAX_OBSERVERS 2

// ADC
#include <hal/nrf_saadc.h>
#define ADC_DEVICE_NODE		    DT_INST(0, nordic_nrf_saadc)
#define ADC_RESOLUTION		    12
#define ADC_GAIN		        ADC_GAIN_1_5
#define ADC_REFERENCE		    ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME	ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 20)
#define ADC_1ST_CHANNEL_ID	    0
#define ADC_1ST_CHANNEL_CHILD   channel_0
#define ADC_1ST_CHANNEL_INPUT	NRF_SAADC_INPUT_AIN3

#define PORT                    led0
// Battery level monitoring
#define BAT_LVL_EN_PIN          11
#define BAT_LVL_VAL_PIN         16      //P0.16
#define ADC_ECH_PERIODE         10      //ms
#define BAT_LVL_PERIODE         1000    //ms
// Battery specification
#define BAT_LVL_MAX             (4100)    //mV
#define BAT_LVL_MIN             (3400)    //mV
#define BAT_LVL_80              (BAT_LVL_MAX*0.92)
#define BAT_LVL_50              (BAT_LVL_MAX*0.88)
#define BAT_LVL_20              (BAT_LVL_MAX*0.86)
#define BAT_LVL_10              (BAT_LVL_MAX*0.80)
#define MEASURED_ADC_OFFSET     0      //mV

// Battery level measure voltage divider (resistance values)
#define BAT_LVL_DIVIDER_R1      4020    //kOhm
#define BAT_LVL_DIVIDER_R2      8660    //kOhm

#define BAT_LVL_AVERAGE_NB_ECH  100

// Battery charger interrupt
#define BAT_CHRG_NINT           10 //P0.15
#define BAT_INTERRUPT_DELAY     0 //ms

// Battery hal error macro
#define ERROR                   -5
#endif // BATT_CONFIG_H