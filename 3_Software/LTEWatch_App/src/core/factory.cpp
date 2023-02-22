/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "factory.h"

#if (USE_MQTT != 0)
    #include <mqttcontroller.h>
#endif

/* **************************************************************************************************************** */
/* Declaration section                                                                                              */
/* **************************************************************************************************************** */
// Global objects
#if (USE_LEDS != 0) && (LEDS_NUMBER > 0)
    gpio::LedController                     Factory::ledA;
    #if (LEDS_NUMBER > 1)
        gpio::LedController                 Factory::ledB;
    #endif  // #if (LEDS_NUMBER > 1)
    #if (LEDS_NUMBER > 2)
        gpio::LedController                 Factory::ledC;
    #endif  // #if (LEDS_NUMBER > 2)
#endif  // #if (USE_LEDS != 0) && defined(LED_1)

#if (USE_GPIOS != 0)
    gpio::Gpio                              Factory::gpioRX;
    gpio::Gpio                              Factory::gpioTX;

    #if defined(GPIO_MAG_IN)
        gpio::Gpio                          Factory::gpioMagIn;
    #endif // #if defined(GPIO_MAG_IN)

    #if defined(GPIO_INT_ACC)
        gpio::Gpio                          Factory::gpioAccInt;
    #endif // #if defined(GPIO_INT_ACC)
    gpio::GpioController                    Factory::gpioController;
#endif  // #if !defined(USE_GPIOS)

#if (USE_BUTTONS != 0) && (BUTTONS_NUMBER > 0)
    gpio::Button                            Factory::buttonA;
    #if (BUTTONS_NUMBER > 1)
        gpio::Button                        Factory::buttonB;
    #endif  // #if (BUTTONS_NUMBER > 1)
    #if (BUTTONS_NUMBER > 2)
        gpio::Button                        Factory::buttonC;
    #endif  // #if (BUTTONS_NUMBER > 2)
    #if (BUTTONS_NUMBER > 3)
        gpio::Button                        Factory::buttonD;
    #endif  // #if (BUTTONS_NUMBER > 3)
    gpio::ButtonController                  Factory::buttonController;
#endif  // #if (USE_BUTTONS != 0)

#if (USE_CALENDAR != 0)
    cal::Calendar                           Factory::calendar;
    cal::DateTime                           Factory::localTime;
#endif  // #if (USE_CALENDAR != 0)

#if (USE_MOTORS != 0)
    motor::Motor                            Factory::motorA;
    #if (NB_MAX_MOTORS > 1)
        motor::Motor                        Factory::motorB;
    #endif
    #if (NB_MAX_MOTORS > 2)
        motor::Motor                        Factory::motorC;
    #endif
    motor::MotorController                  Factory::motorController;
#endif  // #if (USE_MOTORS != 0)

#if (USE_WATCHDOG != 0)
    wdt::Watchdog                           Factory::watchdog;
#endif // #if (USE_WATCHDOG != 0)

#if (USE_MQTT != 0)
    MQTTController                          Factory::_mqtt;
#endif

/* **************************************************************************************************************** */
/* CONSTRUCTOR                                                                                                      */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
Factory::Factory()
{
    // Nothing...
}

/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void Factory::launch(void)
{
    bool hotReset = false;  // HotReset parameter for motor controller

    appInit();
    appBuild(hotReset);
    appBind();
    appCheck();
    appPrepare();
    appStart();
}

//------------------------------------------------------------------------------------------------------------
#if (USE_MOTORS != 0)
    void Factory::onMotorMoveEnd(motor::MotorController* mCtrl)
    {
        motorController.delObserver(this);
        appStart();
    }
#endif
