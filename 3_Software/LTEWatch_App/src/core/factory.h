/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    factory.h
 * @class   Factory
 * @brief   Where everything starts from...
 *
 * # Factory
 *
 * Here are instantiated and initialized all objects used in the current application.
 * The eXecution Framework's mainloop is also started from this class.
 *
 * @author  Patrice Rudaz (patrice.rudaz@hevs.ch)
 * @date    February 2022
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <platform-config.h>
#include "picowatch.h"
#include "picoxf.h"
#include "lvgl.h"


// ------------------------------------------------------------------------------------------------------------------
// Some checks
#if !defined(USE_LEDS)
    #error "USE_LEDS not defined !"
#elif (USE_LEDS != 0) && defined(LED_1)
    #include "ledcontroller.h"
#endif

#if !defined(USE_WATCHDOG)
    #error "USE_WATCHDOG not defined !"
#endif

#if !defined(USE_GPIOS)
    #error "USE_GPIOS not defined !"
#elif (USE_GPIOS != 0)
    #include "gpio.h"
    #include "gpiocontroller.h"
#endif  // #if !defined(USE_GPIOS)

#if !defined(USE_CALENDAR)
    #error "USE_CALENDAR not defined !"
#elif (USE_CALENDAR != 0)
    #include "calendar.h"
    #include "datetime.h"
#endif  // #if !defined(USE_CALENDAR)

#if !defined(USE_BUTTONS)
    #error "USE_BUTTONS not defined !"
#elif (USE_BUTTONS != 0)
    #include "button.h"
    #include "buttoncontroller.h"
#endif  // #if !defined(USE_BUTTONS)

#if !defined(USE_MOTORS)
    #error "USE_MOTORS not defined !"
#elif (USE_MOTORS != 0)
    #include "motor-config.h"
    #include "motor.h"
    #include "motorcontroller.h"
    #include "motorid.h"
    #include "motorctrlobserver.h"
#endif  // #if !defined(USE_MOTORS)

#if (USE_WATCHDOG != 0)
    #include "watchdog/watchdog.h"
#endif // #if (USE_WATCHDOG != 0)

#if (USE_MQTT != 0)
    #include <lte.h>
    #include "config/lte-config.h"
    #include "config/mqtt-config.h"
#endif

#if (USE_GNSS != 0)
    #include "gnsscontroller.h"
#endif

#include "bathal.h"
#include "batterymanager.h"

#include "dispcontroller.h"

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Global Enum Declaration                                                                                          */
/*                                                                                                                  */
/* **************************************************************************************************************** */
/****************************************************************************************************************//**
 * @brief   Enumeration used to have a unique identifier for every product's state
 ********************************************************************************************************************/
typedef enum
{
    PROD_UNKNOWN_MODE = 0,
    PROD_PCB_TEST_MODE,
    PROD_MVT_TEST_MODE,
    PROD_DEFAULT_MODE,
} eProductMode;

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#if (USE_MQTT != 0)
    class MQTTController;
#endif

class Factory
#if (USE_MOTORS != 0)
    : public motor::MotorCtrlObserver
#endif
{
public:

    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC DECLARATION SECTION                                                                                   */
    /*                                                                                                              */
    /* ************************************************************************************************************ */

    static uint8_t                                          _valSec;

    #if (USE_LEDS != 0) && (LEDS_NUMBER > 0)
        static gpio::LedController                          ledA;
        #if (LEDS_NUMBER > 1)
            static gpio::LedController                      ledB;
        #endif  // #if (LEDS_NUMBER > 1)
        #if (LEDS_NUMBER > 2)
            static gpio::LedController                      ledC;
        #endif  // #if (LEDS_NUMBER > 2)
    #endif  // #if (USE_LEDS != 0) && defined(LED_1)

    #if (USE_BUTTONS != 0)
        static gpio::Button                                 buttonA;
        #if (BUTTONS_NUMBER > 1)
            static gpio::Button                             buttonB;
        #endif  // #if (BUTTONS_NUMBER > 1)
        #if (BUTTONS_NUMBER > 2)
            static gpio::Button                             buttonC;
        #endif  // #if (BUTTONS_NUMBER > 2)
        static gpio::ButtonController                       buttonController;
    #endif  // #if (USE_BUTTONS != 0)


    #if (USE_CALENDAR != 0)
        static cal::Calendar                                calendar;
        static cal::DateTime                                localTime;
    #endif  // #if (USE_CALENDAR != 0)


    #if (USE_MOTORS != 0)
            static motor::Motor                             motorA  __attribute__((section(".noinit")));
            #if (NB_MAX_MOTORS > 1)
                static motor::Motor                         motorB  __attribute__((section(".noinit")));
            #endif
            #if (NB_MAX_MOTORS > 2)
                static motor::Motor                         motorC  __attribute__((section(".noinit")));
            #endif
            #if (NB_MAX_MOTORS > 3)
                static motor::Motor                         motorD  __attribute__((section(".noinit")));
            #endif
            #if (NB_MAX_MOTORS > 4)
                static motor::Motor                         motorE  __attribute__((section(".noinit")));
            #endif
            #if (NB_MAX_MOTORS > 5)
                static motor::Motor                         motorF  __attribute__((section(".noinit")));
            #endif
        static motor::MotorController                       motorController;
    #endif  // #if (USE_MOTORS != 0)

    #if (USE_WATCHDOG != 0)
        static wdt::Watchdog                                watchdog;
    #endif // #if (USE_WATCHDOG != 0)

    static application::PicoWatch* picoWatch()           { return application::PicoWatch::getInstance();}

    #if (USE_MQTT != 0)
        static MQTTController* mqtt()       { return &_mqtt; }
        static LTE* lte()                   { return LTE::getInstance(); }
    #endif

    #if (USE_GNSS != 0)
        static GnssController* gnss()       { return GnssController::getInstance(); }
    #endif

    static PicoXF* picoXF()                 { return PicoXF::getInstance(); }

    static DispController* dispCtrl()               { return DispController::getInstance(); }

    static BatHal*  bathal()                { return BatHal::getInstance(); }

    static BatteryManager* batManager()     { return BatteryManager::getInstance(); }

    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* CONSTRUCTOR SECTION                                                                                          */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    Factory();

    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC DECLARATION SECTION                                                                                   */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
    * @brief   Execute the procedure to start the application in the corresponding mode
    *****************************************************************************************************************/
    void launch(eProductMode mode);

    /************************************************************************************************************//**
     * @brief   Observe when the motor finished the animation
    *****************************************************************************************************************/
    #if (USE_MOTORS != 0)
        void onMotorMoveEnd(motor::MotorController* mCtrl);
    #endif

    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* APPLICATION SECTION                                                                                          */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    void appInit();
    void appBuild(bool hotReset);
    void appBind();
    void appCheck();
    void appPrepare();
    void appStart();

private:
    #if defined(DEBUG_NRF_USER)
        static Factory*                     _pInstance;
    #endif
    static eProductMode                     _mode;

    #if (USE_MQTT != 0)
        static MQTTController               _mqtt;
    #endif
};
