/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics.
 * Created by Patrice Rudaz
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "bathal.h"
#include "debug-config.h"

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#ifndef CONFIG_BAT_HAL_LOG_LEVEL
    #define CONFIG_BAT_HAL_LOG_LEVEL            3
#endif

#if (CONFIG_BAT_HAL_LOG_LEVEL != 0)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(bat_hal, CONFIG_BAT_HAL_LOG_LEVEL);

    #define BATHAL_DEBUG(args...)              LOG_DBG(args)
    #define BATHAL_ERROR(args...)              LOG_ERR(args)
    #define BATHAL_INFO(args...)               LOG_INF(args)
    #define BATHAL_WRN(args...)                LOG_WRN(args)
#else
    #define BATHAL_DEBUG(...)                  {(void) (0);}
    #define BATHAL_ERROR(...)                  {(void) (0);}
    #define BATHAL_INFO(...)                   {(void) (0);}
    #define BATHAL_WRN(...)                    {(void) (0);}
#endif

/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
// static

BatHal::adc_data2 BatHal::_adc;

static const struct gpio_dt_spec batLvl = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), bat_lvl_gpios);
static const struct gpio_dt_spec batLvlEn = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), batlvl_en_gpios);
static const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c1));

static const struct gpio_dt_spec batChrgInt = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), batchrg_int_gpios);

static const struct device*         adc_dev     = DEVICE_DT_GET(ADC_DEVICE_NODE);
static const struct adc_channel_cfg adc_ch_cfg  = ADC_CHANNEL_CFG_DT(DT_CHILD(ADC_DEVICE_NODE, ADC_1ST_CHANNEL_CHILD));

uint8_t* BatHal::_buf;

bool BatHal::_isInit;

// ------------------------------------------------------------------------------------------------------------------
BatHal::BatHal(void)
{
    _isInit = false;
}

// ------------------------------------------------------------------------------------------------------------------
bool BatHal::init(void)
{
    _isInit = true;
    return true;
}

bool BatHal::isInit()
{
    return _isInit;
}

// ------------------------------------------------------------------------------------------------------------------
BatHal* BatHal::getInstance(void)
{
    static BatHal instance;
    return &instance;
}

// ------------------------------------------------------------------------------------------------------------------
bool BatHal::registerCallback(gpio_callback_handler_t handler, struct gpio_callback *cb)
{
    if (!device_is_ready(batChrgInt.port))
    {
        BATHAL_ERROR("Configuring BAT_CHR_INT pin failed\n");
		return false;
    }

    // configure the interrupt pin as input
    int err = gpio_pin_configure_dt(&batChrgInt, GPIO_INPUT | GPIO_PULL_UP);
    if (err != 0)
    {
        BATHAL_ERROR("Gpio pin config failed : %d", err);
    }
    else
    {
        BATHAL_INFO("Gpio pin config succeed");
    }

#if(1)
    err = gpio_pin_interrupt_configure_dt(&batChrgInt, GPIO_INT_EDGE_TO_ACTIVE);
    if (err != 0)
    {
        BATHAL_ERROR("Gpio interrupt config failed : %d", err);
    }
    else
    {
        BATHAL_INFO("Gpio interrupt config succeed");
    }

    // setup the interrupt gpio callback
    gpio_init_callback(cb, (gpio_callback_handler_t) handler, BIT(batChrgInt.pin));

    err = gpio_add_callback(batChrgInt.port, cb);
    if (err != 0)
    {
        BATHAL_ERROR("Gpio add callback failed : %d", err);
    }
    else
    {
        BATHAL_INFO("Gpio add callback succeed");
    }
#endif

    return true;
}


// ------------------------------------------------------------------------------------------------------------------
bool BatHal::configADC(void)
{
    int err = 0;

/* --------- Init Battery Level Enable Output Pin --------- */
    err = gpio_pin_configure_dt(&batLvlEn, GPIO_OUTPUT_INACTIVE);
    err = gpio_pin_configure_dt(&batLvl, GPIO_INPUT);

	if (err != 0) {
		BATHAL_ERROR("Configuring GPIO pin failed: %d\n", err);
		return false;
	}

/* ----------------------- Init ADC ----------------------- */
    // Config ADC channel
    if (!device_is_ready(adc_dev))
    {
        adc_dev = DEVICE_DT_GET(ADC_DEVICE_NODE);
        BATHAL_ERROR("ADC controller device not ready");
        return false;
	}

    // Config ADC input pin
    err = adc_channel_setup(adc_dev, &adc_ch_cfg);
    if (err < 0)
    {
        BATHAL_ERROR("Could not setup channel #0 (%d)", err);
        return false;
    }

    return true;
}

// ------------------------------------------------------------------------------------------------------------------
int32_t BatHal::enableADC(bool enable)
{
    int err;
    if (enable)
    {
        // Set battery level enable to 1
        err = gpio_pin_set_dt(&batLvlEn, 1);
        if (err != 0) {
            BATHAL_ERROR("Setting GPIO pin level failed: %d\n", err);
            return err;
        }
    }
    else
    {
        // Set battery level enable to 0 when finished
        err = gpio_pin_set_dt(&batLvlEn, 0);
        if (err != 0) {
            BATHAL_ERROR("Setting GPIO pin level failed: %d\n", err);
            return err;
        }
    }
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
int32_t BatHal::readADC(void)
{
    int err;
	int16_t buf;
    int32_t valmV;

    const struct adc_sequence sequence = {
            .channels       = BIT(ADC_1ST_CHANNEL_ID),
            .buffer         = &buf,
            .buffer_size    = sizeof(buf),
            .resolution     = ADC_RESOLUTION,
        };

    // Read _adc input value
    err = adc_read(adc_dev, &sequence);
    if (err < 0)
    {
        BATHAL_ERROR("Could not read (%d)", err);
        valmV = -1;
    }

    // Convert the _adc input value in mV
    uint16_t ref = adc_ref_internal(adc_dev);
    valmV = static_cast<int16_t>(ref) * buf;
    adc_gain_invert(adc_ch_cfg.gain, &valmV);
    valmV = (valmV/4096);

    BATHAL_DEBUG(" = %d mV", valmV);

    return valmV;
}

// ------------------------------------------------------------------------------------------------------------------
bool BatHal::configI2C(void)
{

	if (!device_is_ready(i2c_dev)) {
		BATHAL_ERROR("I2C: Device is not ready.");

		return false;
	}

    return true;
}

// ------------------------------------------------------------------------------------------------------------------
 int BatHal::readI2C(uint8_t *data, uint32_t num_bytes)
{
    if (!device_is_ready(i2c_dev)) {
       return ERROR;
    }
	//return i2c_read(i2c_dev, data, num_bytes, FRAM_I2C_ADDR);
    int err = 0;
    for (size_t i = 0; i < num_bytes; i++)
    {
        //err &= i2c_read(i2c_dev, &data[i], 1, FRAM_I2C_ADDR);
        err &= read_bytes(i2c_dev, i, &data[i], 1);
    }
    return err;
}
// ------------------------------------------------------------------------------------------------------------------
int BatHal::writeI2C(uint8_t *data, uint32_t num_bytes)
{
    if (!device_is_ready(i2c_dev)) {
        return ERROR;
    }

    //return i2c_write(i2c_dev, data, num_bytes, FRAM_I2C_ADDR);
    int err = 0;
    for (size_t i = 0; i < num_bytes; i++)
    {
        //err &= i2c_write(i2c_dev, &data[i], 1, FRAM_I2C_ADDR);
        err &= write_bytes(i2c_dev, i, &data[i], 1);
    }
    return err;
}

// ------------------------------------------------------------------------------------------------------------------
int BatHal::write_bytes(const struct device *i2c_dev, uint8_t addr, uint8_t *data, uint32_t num_bytes)
{
	uint8_t wr_addr[1];
	struct i2c_msg msgs[2];

	/* FRAM address */
	wr_addr[0] = addr & 0xFF;

	/* Setup I2C messages */

	/* Send the address to write to */
	msgs[0].buf = wr_addr;
	msgs[0].len = sizeof(wr_addr);
	msgs[0].flags = I2C_MSG_WRITE;

	/* Data to be written, and STOP after this. */
	msgs[1].buf = data;
	msgs[1].len = num_bytes;
	msgs[1].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

	return i2c_transfer(i2c_dev, &msgs[0], 2, FRAM_I2C_ADDR);
}

// ------------------------------------------------------------------------------------------------------------------
int BatHal::read_bytes(const struct device *i2c_dev, uint8_t addr, uint8_t *data, uint32_t num_bytes)
{
	uint8_t wr_addr[1];
	struct i2c_msg msgs[2];

	/* Now try to read back from FRAM */

/* FRAM address */
	wr_addr[0] = addr & 0xFF;

	/* Setup I2C messages */

	/* Send the address to read from */
	msgs[0].buf = wr_addr;
	msgs[0].len = sizeof(wr_addr);
	msgs[0].flags = I2C_MSG_WRITE;

	/* Read from device. STOP after this. */
	msgs[1].buf = data;
	msgs[1].len = num_bytes;
	msgs[1].flags = I2C_MSG_READ | I2C_MSG_STOP;

	return i2c_transfer(i2c_dev, &msgs[0], 2, FRAM_I2C_ADDR);
}

// ------------------------------------------------------------------------------------------------------------------

uint8_t* BatHal::getBuffer(void)
{
   return _buf;
}

//BAT_LVL_EN_PIN
//BAT_CHRG_NINT