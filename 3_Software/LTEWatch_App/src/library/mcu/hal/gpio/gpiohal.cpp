/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include <drivers/gpio.h>
#include <nrfx_gpiote.h>
#include "gpiohal.h"

#include "debug-config.h"
#include "platform-config.h"

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#define DEBUG_APP_FUNCTION_MANAGER_BH_ENABLE 0

#if (DEBUG_APP_FUNCTION_MANAGER_BH_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(gpiohal);
    #define GPIOHAL_DEBUG(args...)              LOG_DBG(args)
    #define GPIOHAL_ERROR(args...)              LOG_ERR(args)
#else
    #define GPIOHAL_DEBUG(...)                  {(void) (0);}
    #define GPIOHAL_ERROR(...)                  {(void) (0);}
#endif

#define PORT led0

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Namespace's Declaration                                                                                          */
/* **************************************************************************************************************** */
using hal::GpioHal;

/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */

// static
struct gpio_dt_spec GpioHal::dev = GPIO_DT_SPEC_GET(DT_NODELABEL(PORT), gpios);

void GpioHal::init()
{
    uint32_t i = 0;
    #if (GPIO_INIT_OUT_AND_IN != 0)
    {
        GPIOHAL_DEBUG("nrf_gpio_cfg() as output for all GPIOS (0..%d)", NUMBER_OF_PINS);
        for (i = 0; i < NUMBER_OF_PINS; i++)
        {
            gpio_pin_configure(GpioHal::dev.port, static_cast<gpio_pin_t>(i),
                GPIO_OUTPUT | GPIO_NO_PULL | GPIO_DISCONNECTED | GPIO_DEFAULT_DRIVE | GPIO_NOSENSE);
        }
        GPIOHAL_DEBUG((char*) "-");
        GPIOHAL_DEBUG("NRF_P0->OUTCLR = 0xffffffff");
        gpio_port_clear_bits(GpioHal::dev.port, 0xffffffff);
        #if (GPIO_COUNT == 2)
        {
            GPIOHAL_DEBUG("NRF_P1->OUTCLR = 0xffffffff");
            gpio_port_clear_bits(GpioHal::dev.port, 0xffffffff);
        }
        #endif
        nrf_delay_ms(GPIO_INIT_OUT_AND_IN_DELAY);
    }
    #endif  // #if (GPIO_INIT_OUT_AND_IN != 0)

    // Initialization of all GPIOs as input or output according to gpio config file (gpio-config.h)
    #if (GPIO_INIT_AS_INPUT != 0)
    {
        GPIOHAL_DEBUG("nrf_gpio_cfg() as input disconnected for all GPIOS (0..%d)", NUMBER_OF_PINS);
        for (i = 0; i < NUMBER_OF_PINS; i++)
        {
            gpio_pin_configure(GpioHal::dev.port, static_cast<gpio_pin_t>(i),
                GPIO_INPUT | GPIO_NO_PULL | GPIO_DISCONNECTED | GPIO_DEFAULT_DRIVE | GPIO_NOSENSE);
        }
    }
    #else
    {
        GPIOHAL_DEBUG("nrf_gpio_cfg() as output for all GPIOS (0..%d)", NUMBER_OF_PINS);
        for (i = 0; i < NUMBER_OF_PINS; i++)
        {
            gpio_pin_configure(GpioHal::dev.port, static_cast<gpio_pin_t>(i),
            GPIO_OUTPUT | GPIO_NO_PULL | GPIO_DISCONNECTED | GPIO_DEFAULT_DRIVE | GPIO_NOSENSE);
        }
    }
    #endif // GPIO_INIT_AS_INPUT

    // DEBUG
    GPIOHAL_DEBUG("Initialization of all GPIO ... Done !");
}

// ------------------------------------------------------------------------------------------------------------------
// static
void GpioHal::cfgPinInput(uint32_t pinNbr, uint32_t drive, uint32_t pullSetup, uint32_t connect)
{
    // Check if device port is ready
    if (!device_is_ready(GpioHal::dev.port))
    {
	    dev = GPIO_DT_SPEC_GET(DT_NODELABEL(PORT), gpios);
    }

    // Configure gpio pin "pinNbr" as input with "pullSetup"
    int ret = gpio_pin_configure(GpioHal::dev.port, static_cast<gpio_pin_t>(pinNbr), GPIO_INPUT | pullSetup | connect);

    // Check if pin configuration got wrong
    if (ret != 0)
    {
        GPIOHAL_ERROR("gpio_pin_configure() return : %s", GpioHal::error2msg(ret));
    }

    GPIOHAL_DEBUG("Set pin %02u as INPUT, %s, drive %u, connect %u",
            pinNbr, (pullSetup == 0 ? "No Pull":(pullSetup == GPIO_PULL_UP ? "PullUp":"PullDown")),
            drive, connect);
}

// ------------------------------------------------------------------------------------------------------------------
// static
void GpioHal::cfgPinInputNoPull(uint32_t pinNbr)
{
    // Check if device port is ready
    if (!device_is_ready(GpioHal::dev.port))
    {
	    dev = GPIO_DT_SPEC_GET(DT_NODELABEL(PORT), gpios);
    }

    // Configure gpio pin "pinNbr" as input with "pullSetup"
    int ret = gpio_pin_configure(GpioHal::dev.port, static_cast<gpio_pin_t>(pinNbr), GPIO_INPUT | GPIO_NO_PULL | GPIO_CONNECTED);

    // Check if pin configuration got wrong
    if (ret != 0)
    {
        GPIOHAL_ERROR("gpio_pin_configure() return : %s", GpioHal::error2msg(ret));
    }
}

// ------------------------------------------------------------------------------------------------------------------
// static
void GpioHal::cfgPinInputPullup(uint32_t pinNbr)
{
    //cfgPinInput(pinNbr, GPIO_PIN_CNF_DRIVE_S0S1, GPIO_PIN_CNF_PULL_Pullup, GPIO_PIN_CNF_INPUT_Connect);

    // Check if device port is ready
    if (!device_is_ready(GpioHal::dev.port))
    {
	    dev = GPIO_DT_SPEC_GET(DT_NODELABEL(PORT), gpios);
    }

    // Configure gpio pin "pinNbr" as input with "pullSetup"
    int ret = gpio_pin_configure(GpioHal::dev.port, static_cast<gpio_pin_t>(pinNbr), GPIO_INPUT | GPIO_PULL_UP | GPIO_CONNECTED);

    // Check if pin configuration got wrong
    if (ret != 0)
    {
        GPIOHAL_ERROR("gpio_pin_configure() return : %s", GpioHal::error2msg(ret));
    }
}

// ------------------------------------------------------------------------------------------------------------------
// static
void GpioHal::cfgPinInputPullDown(uint32_t pinNbr)
{
    // Check if device port is ready
    if (!device_is_ready(GpioHal::dev.port))
    {
	    dev = GPIO_DT_SPEC_GET(DT_NODELABEL(PORT), gpios);
    }

    // Configure gpio pin "pinNbr" as input with "pullSetup"
    int ret = gpio_pin_configure(GpioHal::dev.port, static_cast<gpio_pin_t>(pinNbr), GPIO_INPUT | GPIO_PULL_UP | GPIO_CONNECTED);

    // Check if pin configuration got wrong
    if (ret != 0)
    {
        GPIOHAL_ERROR("gpio_pin_configure() return : %s", GpioHal::error2msg(ret));
    }
}

// ------------------------------------------------------------------------------------------------------------------
// static
void GpioHal::cfgPinOutput(uint32_t pinNbr, uint32_t drive, uint32_t pullSetup, uint32_t connect)
{
    // Check if device port is ready
    if (!device_is_ready(GpioHal::dev.port))
    {
	    dev = GPIO_DT_SPEC_GET(DT_NODELABEL(PORT), gpios);
    }

    // Configure gpio pin "pinNbr" as input with "pullSetup"
    int ret = gpio_pin_configure(GpioHal::dev.port, static_cast<gpio_pin_t>(pinNbr), GPIO_OUTPUT | drive | connect);

    // Check if pin configuration got wrong
    if (ret != 0)
    {
        GPIOHAL_ERROR("gpio_pin_configure() return : %s", GpioHal::error2msg(ret));
    }

    GPIOHAL_DEBUG("Set pin %02u as OUTPUT, drive %u, connect %u", pinNbr, drive, connect);
}

// ------------------------------------------------------------------------------------------------------------------
// static
void GpioHal::cfgQuickOutput(uint32_t pinNbr, uint32_t drive)
{
    cfgPinOutput(pinNbr, drive, GPIO_PIN_CNF_PULL_Pulldown, GPIO_PIN_CNF_INPUT_Disconnect);

    // Check if device port is ready
    if (!device_is_ready(GpioHal::dev.port))
    {
	    dev = GPIO_DT_SPEC_GET(DT_NODELABEL(PORT), gpios);
    }

    // Configure gpio pin "pinNbr" as input with "pullSetup"
    int ret = gpio_pin_configure(GpioHal::dev.port, static_cast<gpio_pin_t>(pinNbr), GPIO_OUTPUT | drive);

    // Check if pin configuration got wrong
    if (ret != 0)
    {
        GPIOHAL_ERROR("gpio_pin_configure() return : %s", GpioHal::error2msg(ret));
    }
}

// ------------------------------------------------------------------------------------------------------------------
// static
void GpioHal::pinMaskClear(uint64_t pinMask)
{
    int ret = gpio_port_clear_bits(GpioHal::dev.port, static_cast<gpio_port_pins_t>((uint32_t) (pinMask & 0x00000000ffffffff)));

    // Check if pin configuration got wrong
    if (ret != 0)
    {
        GPIOHAL_ERROR("gpio_port_clear_bits() return : %s", GpioHal::error2msg(ret));
    }

    // DEBUG
     GPIOHAL_DEBUG("pinMask : 0x%016x", (uint32_t) pinMask);
}

// ------------------------------------------------------------------------------------------------------------------
// static
void GpioHal::pinMaskSet(uint64_t pinMask)
{
    int ret = gpio_port_set_bits(GpioHal::dev.port, static_cast<gpio_port_pins_t>((uint32_t) (pinMask & 0x00000000ffffffff)));

    // Check if pin configuration got wrong
    if (ret != 0)
    {
        GPIOHAL_ERROR("gpio_port_set_bits() return : %s", GpioHal::error2msg(ret));
    }

    // DEBUG
     GPIOHAL_DEBUG("pinMask : 0x%016x", (uint32_t) pinMask);
}

// ------------------------------------------------------------------------------------------------------------------
// static
void GpioHal::pinMaskToggle(uint64_t pinMask)
{
    int ret = gpio_port_toggle_bits(GpioHal::dev.port, static_cast<gpio_port_pins_t>(pinMask));

    // Check if pin configuration got wrong
    if (ret != 0)
    {
        GPIOHAL_ERROR("gpio_port_toggle_bits() return : %s", GpioHal::error2msg(ret));
    }

    // DEBUG
    GPIOHAL_DEBUG("pinMask : 0x%016x", (uint32_t) pinMask);
}

// ------------------------------------------------------------------------------------------------------------------
// static
void GpioHal::pinClear(uint32_t pinNbr, bool activeHigh)
{
    int ret = gpio_pin_set(GpioHal::dev.port, static_cast<gpio_pin_t>(pinNbr), (activeHigh == true ? 0 : 1));

    // Check if pin configuration got wrong
    if (ret != 0)
    {
        GPIOHAL_ERROR("gpio_pin_set() return : %s", GpioHal::error2msg(ret));
    }

    // DEBUG
    GPIOHAL_DEBUG("pin %i", pinNbr);
}

// ------------------------------------------------------------------------------------------------------------------
// static
void GpioHal::pinSet(uint32_t pinNbr, bool activeHigh)
{
    int ret = gpio_pin_set(GpioHal::dev.port, static_cast<gpio_pin_t>(pinNbr), (activeHigh == true ? 1 : 0));

    // Check if pin configuration got wrong
    if (ret != 0)
    {
        GPIOHAL_ERROR("gpio_pin_set() return : %s", GpioHal::error2msg(ret));
    }

    // DEBUG
    GPIOHAL_DEBUG("pin %i", pinNbr);
}

// ------------------------------------------------------------------------------------------------------------------
// static
void GpioHal::pinToggle(uint32_t pinNbr)
{
    int ret = gpio_pin_toggle(GpioHal::dev.port, static_cast<gpio_pin_t>(pinNbr));

    if (ret != 0)
    {
        GPIOHAL_ERROR("gpio_pin_toggle() return : %s", GpioHal::error2msg(ret));
    }

    // DEBUG
    GPIOHAL_DEBUG("Toggle pin %i", pinNbr);
}

// ------------------------------------------------------------------------------------------------------------------
// static
void GpioHal::pinWrite(uint32_t pinNbr, uint8_t value)
{
    int ret = gpio_pin_set(GpioHal::dev.port, static_cast<gpio_pin_t>(pinNbr), value);

    if (ret != 0)
    {
        GPIOHAL_ERROR("gpio_pin_set() return : %s", GpioHal::error2msg(ret));
    }
    
    // DEBUG
    GPIOHAL_DEBUG("Set %i on pin %i", value, pinNbr);
}

// ------------------------------------------------------------------------------------------------------------------
// static
bool GpioHal::pinRead(uint32_t pinNbr, bool activeHigh)
{   
    int reg = gpio_pin_get(GpioHal::dev.port, static_cast<gpio_pin_t>(pinNbr));

    if (activeHigh)
    {
        GPIOHAL_DEBUG("pin %02u is %d", pinNbr, (reg = 1 ? 1 : 0));
        return (reg = 1 ? true : false);
    }
    else
    {
        GPIOHAL_DEBUG("pin %02u is %d", pinNbr, (reg = 1 ? 0 : 1));
        return (reg = 1 ? false : true);
    }
}

// ------------------------------------------------------------------------------------------------------------------
// static
uint64_t GpioHal::pinMaskRead(uint64_t pinMask)
{
    static gpio_port_value_t value;

    int ret = gpio_port_get(GpioHal::dev.port, &value);

    if (ret != 0)
    {
        GPIOHAL_ERROR("gpio_pin_set() return : %s", GpioHal::error2msg(ret));
    }

    GPIOHAL_DEBUG("pinMask 0x 0x%08x is 0x 0x%08x", (uint32_t) pinMask, ((uint32_t) (value)) & ((uint32_t) pinMask));
    
    return ((value) & ((uint32_t) pinMask));
}

#if defined(DEBUG_NRF_USER)
    // ------------------------------------------------------------------------------------------------------------------
    // static
    void GpioHal::initDebugGPIO(uint8_t pinNbr)
    {
        cfgQuickOutput(pinNbr, GPIO_PIN_CNF_DRIVE_S0S1);
    }

    // ------------------------------------------------------------------------------------------------------------------
    // static
    void GpioHal::toggleDebugGPIO(uint8_t pinNbr)
    {
        pinToggle(pinNbr);
    }
#endif

const char* GpioHal::error2msg(int code)
{
    static struct  ErrorCode { int value;
                const char* name;}error_codes[5];
                
    error_codes[0] = { ENOSYS, "ENOSYS : Current pin configuration is not implemented by the driver" };
    error_codes[1] = { EINVAL, "EINVAL :Invalid argument" };
    error_codes[2] = { EIO, "EIO : I/O error when accessing an external GPIO chip" };
    error_codes[3] = { EWOULDBLOCK, "EWOULDBLOCK : Operation would block" };
    error_codes[4] = { 0, 0 };

    for (int i = 0; error_codes[i].name; ++i)
    {
        if (error_codes[i].value == code) return error_codes[i].name;
    }
    return "unknown";
}