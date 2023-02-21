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
    static BatHal* getInstance();

    static bool init();

    static bool isInit();

    /********************************************************************************************************//**
        * @brief   Initializes
        *
        * Set as output the PIN assigned to the LED specified by its index.
        *
        * @param   index   Specify the LED to work with.
        ************************************************************************************************************/

    bool registerCallback(gpio_callback_handler_t handler, struct gpio_callback *cb);

    int32_t enableADC(bool enable);

    int32_t readADC();

    int readI2C(uint8_t *data, uint32_t num_bytes);

    int writeI2C(uint8_t *data, uint32_t num_bytes);

    int read_bytes(const struct device *i2c_dev, uint8_t addr, uint8_t *data, uint32_t num_bytes);

    int write_bytes(const struct device *i2c_dev, uint8_t addr, uint8_t *data, uint32_t num_bytes);

    bool configADC();

    bool configI2C();

    uint8_t* getBuffer();

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

    static uint8_t* _buf;

    typedef struct{
        struct adc_dt_spec spec;
        struct adc_sequence adc_seq;
        int32_t val_mV;
    } adc_data1;

    typedef struct{
        struct device *dev;
        struct adc_channel_cfg channel_cfg;
        struct adc_sequence sequence;
        int32_t val_mV;
    } adc_data2;

    static adc_data2 _adc;

    static bool _isInit;
};  // Class BatHal

/** @} */

#endif // BATT_MANAGER_ONCE