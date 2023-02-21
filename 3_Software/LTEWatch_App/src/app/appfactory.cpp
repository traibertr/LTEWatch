/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 * ******************************************************************************************************************/

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "factory.h"
#include <sdktools.h>
#include <xf.h>

#include "gpiohal.h"
#include "debug-config.h"
#include <zephyr.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_factory, 3);
/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#if (DEBUG_FACTORY_ENABLE == 1) && defined(DEBUG_NRF_USER)
    #define F_ERROR(args...)                    LOG_ERR(args)
    #define F_WARN(args...)                     LOG_WRN(args)
    #define F_INFO(args...)                     LOG_INF(args)
    #define F_DEBUG(args...)                    LOG_DBG(args)
#else
    #define F_ERROR(...)                        {(void) (0);}
    #define F_WARN(...)                         {(void) (0);}
    #define F_INFO(...)                         {(void) (0);}
    #define F_DEBUG(...)                        {(void) (0);}
#endif

/* **************************************************************************************************************** */
/* OBJECT'S DECLARATION                                                                                             */
/* **************************************************************************************************************** */
// Main class of the application

/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void Factory::appInit()
{
    printk("_______________________________________________\n");
    printk("______ _           _    _       _       _     \n");
    printk("| ___ (_)         | |  | |     | |     | |    \n");
    printk("| |_/ /_  ___ ___ | |  | | __ _| |_ ___| |__  \n");
    printk("|  __/| |/ __/ _ \\| |/\\| |/ _` | __/ __| '_ \\ \n");
    printk("| |   | | (_| (_) \\  /\\  / (_| | || (__| | | |\n");
    printk("\\_|   |_|\\___\\___/ \\/  \\/ \\__,_|\\__\\___|_| |_|\n");
    printk("_______________________________________________\n");
    F_DEBUG("Init done");

}

// ------------------------------------------------------------------------------------------------------------------
void Factory::appBuild(bool hotReset)
{
    F_DEBUG("Building the application");

    // Initialize the SDK
    tools::SDKTools::init();
    picoXF()->init();
    picoWatch()->init();
    dispCtrl()->init();
    bathal()->init();
    batManager()->init();

    #if (USE_BUTTONS != 0) && (BUTTONS_NUMBER > 0)
        buttonA.initialize(BUTTON_1, gpio::ButtonId::Button1);
        #if (BUTTONS_NUMBER > 1)
            buttonB.initialize(BUTTON_2, gpio::ButtonId::Button2);
        #endif  // #if (BUTTONS_NUMBER > 1)
        #if (BUTTONS_NUMBER > 2)
            buttonC.initialize(BUTTON_3, gpio::ButtonId::Button3);
        #endif  // #if (BUTTONS_NUMBER > 2)
        buttonController.initialize();
    #endif

    #if (USE_CALENDAR != 0)
        localTime.initialize();
        calendar.initialize();
    #endif

    #if (USE_LEDS != 0) && (LEDS_NUMBER > 0)
        ledA.initialize(0);
        #if (LEDS_NUMBER > 1)
            ledB.initialize(1);
        #endif  // #if (LEDS_NUMBER > 1)
        #if (LEDS_NUMBER > 2)
            ledC.initialize(2);
        #endif  // #if (LEDS_NUMBER > 2)
    #endif

    #if (USE_MOTORS != 0)
        motorA.initialize(hotReset, motor::MotorId::Motor1, (motor::Motor::eNbSteps) M1STEPS, M1NBS, M1CA, M1CB, M1CC, M1CCW);
        #if (NB_MAX_MOTORS > 1)
            motorB.initialize(hotReset, motor::MotorId::Motor2, (motor::Motor::eNbSteps) M2STEPS, M2NBS, M2CA, M2CB, M2CC, M2CCW);
        #endif // #if (NB_MAX_MOTORS > 1)
        #if (NB_MAX_MOTORS > 2)
            motorC.initialize(hotReset, motor::MotorId::Motor3, (motor::Motor::eNbSteps) M3STEPS, M3NBS, M3CA, M3CB, M3CC, M3CCW);
        #endif // #if (NB_MAX_MOTORS > 2)
        #if (NB_MAX_MOTORS > 3)
            motorD.initialize(hotReset, motor::MotorId::Motor4, (motor::Motor::eNbSteps) M4STEPS, M4NBS, M4CA, M4CB, M4CC, M4CCW, M4NBS);
        #endif // #if (NB_MAX_MOTORS > 3)
        motorController.initialize(hotReset);
    #endif // #if (USE_MOTORS != 0)

    #if (USE_WATCHDOG != 0)
       // watchdog.initialize();
    #endif

    #if (USE_MQTT != 0)
        mqtt()->init(CONFIG_MQTT_CLIENT_ID, CONFIG_MQTT_CLIENT_USERNAME, CONFIG_MQTT_CLIENT_PWD);
    #endif

    #if (USE_GNSS != 0)
        gnss()->initialize();
    #endif
}

// ------------------------------------------------------------------------------------------------------------------
void Factory::appBind()
{
    F_DEBUG("Binding the application");

    picoWatch()->bind(&buttonA, &buttonB, &buttonC);

    F_DEBUG("Bind PicoWatch Application");

    #if (USE_BUTTONS != 0) && (BUTTONS_NUMBER > 0)
        bool ret = false;
        ret = buttonController.registerButton(&buttonA);
        if(ret)
        {
            F_INFO("Register Button BT0");
        }
        else
        {
            F_ERROR("Failed to Register Button BT0");
        }
        #if (BUTTONS_NUMBER > 1)
        ret = buttonController.registerButton(&buttonB);
        if(ret)
        {
            F_INFO("Register Button BT1");
        }
        else
        {
            F_ERROR("Failed to Register Button BT1");
        }
        #endif  // #if (BUTTONS_NUMBER > 1)
        #if (BUTTONS_NUMBER > 2)
        ret = buttonController.registerButton(&buttonC);
        if(ret)
        {
            F_INFO("Register Button BT2");
        }
        else
        {
            F_ERROR("Failed to Register BT2");
        }
        #endif  // #if (BUTTONS_NUMBER > 2)
        #if (BUTTONS_NUMBER > 3)
            buttonController.registerButton(&buttonD);
            F_DEBUG("Register Button D");
        #endif  // #if (BUTTONS_NUMBER > 3)
    #endif

    #if (USE_CALENDAR != 0)
        calendar.registerDateTime(&localTime);
        calendar.addObserver(application::PicoWatch::getInstance());
        F_DEBUG("Register Calender");
    #endif // #if (USE_CALENDAR != 0)

    #if (USE_MOTORS != 0)
        motorController.addMotor(&motorA);
        F_DEBUG("Add Motor A");
        #if (NB_MAX_MOTORS > 1)
            motorController.addMotor(&motorB);
            F_DEBUG("Add Motor B");
        #endif // #if (NB_MAX_MOTORS > 1)
        #if (NB_MAX_MOTORS > 2)
            motorController.addMotor(&motorC);
            F_DEBUG("Add Motor C");
        #endif // #if (NB_MAX_MOTORS > 2)
    #endif

    #if (USE_MQTT != 0)
       lte()->addObserver(picoWatch());
       mqtt()->addObserver(picoWatch());
    #endif

    #if (USE_GNSS != 0)
        gnss()->addObserver(picoWatch());
    #endif

    batManager()->addObserver(picoWatch());
}

// ------------------------------------------------------------------------------------------------------------------
void Factory::appCheck()
{
   F_DEBUG("Checking the application");
}

// ------------------------------------------------------------------------------------------------------------------
void Factory::appPrepare()
{
    F_DEBUG("Preparing the application");

    #if (USE_LEDS != 0)  && (LEDS_NUMBER > 0)
    {
        ledA.startBehaviour();
        F_DEBUG("Start LedA Behaviour");
        #if (LEDS_NUMBER > 1)
            ledB.startBehaviour();
            F_DEBUG("Start LedB Behaviour");
        #endif  // #if (LEDS_NUMBER > 1)
        #if (LEDS_NUMBER > 2)
            ledC.startBehaviour();
            F_DEBUG("Start LedC Behaviour");
        #endif  // #if (LEDS_NUMBER > 2)
    }
    #endif

    #if (USE_BUTTONS != 0)
        buttonController.enableController();
        F_DEBUG("Enable Button Controller");
    #endif


    #if (USE_MOTORS != 0)
    {
        #if (NB_MAX_MOTORS > 1)
            motorController.setThresholdForMinute(motor::MotorId::Motor2);
        #endif // #if (NB_MAX_MOTORS > 1)
        #if (NB_MAX_MOTORS > 2)
            motorController.setThresholdForHour(motor::MotorId::Motor3);
        #endif // #if (NB_MAX_MOTORS > 2)

        F_DEBUG("starting motorController");
        motorController.addObserver(this);
        motorController.startBehaviour();

        #if (USE_MQTT != 0)
            mqtt()->startBehaviour();
        #endif

        F_DEBUG("Start PicoWatch Behaviour");
        picoWatch()->startBehaviour();

    }
    #endif // #if (USE_MOTORS != 0)
}

// ------------------------------------------------------------------------------------------------------------------
void Factory::appStart()
{
    // nothing yet
}