/***************************************************************************************************************//**
 * Copyright (C) Hes-so, MSE Lausanne, Colaboration with Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Tristan Traiber and based on Patrice Rudaz Source Code from Moskito Project
 * All rights reserved.
 *
 * Initial author: Tristan Traiber
 * Creation date: 06.01.2023
 *
 * @file    picowatch.h
 * @class   PicoWatch
 * @brief   LTEWatch Master Thesis simple LTE/GNSS Hybrid Smart-Watch
 *
 * \mainpage PicoWatch
 *
 * # Moskito
 *
 * This class has been developed for the Specific project called Moskito. It uses the eXecution Framework designed 
 * and developed by the HEI Sion, member of HES-SO Valais.
 *
 * @author  Tristan Traiber (tristan.traiber@master.hes-so.ch)
 * @date    February 2022
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <platform-config.h>

#include <picoireactive.h>
#include <picoevent.h>

#if (USE_BUTTONS != 0)
    #include "button.h"
    #include "buttonobserver.h"
#endif

#if (USE_CALENDAR != 0)
    #include "calendarobserver.h"
#endif  // #if (USE_CALENDAR != 0)


#if (USE_LEDS != 0) && (LEDS_NUMBER > 0)
    #include "ledcontroller.h"
#endif

#if (USE_MQTT != 0)
    #include "ilteobserver.h"
    #include "imqttobserver.h"
    #include "lte.h"
    #include "mqttcontroller.h"
    #include <string.h>
#endif

#if (USE_GNSS != 0)
    #include "gnsscontroller.h"
    #include "ignssobserver.h"
#endif

#include "batterymanager.h"
#include "ibatteryobserver.h"

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* APPLICATION definition                                                                                           */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#define NO_ACTIVITY_DELAY           300     ///< Define the time to wait before going in system off [s]
#define NO_ACTIVITY_TIMEMOUT_STEP   30000   ///< Number of [ms] before a timer_tick
#define NO_ACTIVITY_DECREMENT       30      ///< Value to decrement the counter
#define MY_STACK_SIZE               2048    ///< PicoWatch WorkQueue thread stack area size
#define MY_PRIORITY                 5       ///< PicoWatch WorkQueue thread priority level
#define LONG_PRESS_HOLD_TIMER       500     ///< Hold long press consideration timer in ms
#define DEFAULT_START_UPDATE        3000
#define DEFAULT_UPDATE_MOT_P        1000    ///< Default motor update period in ms
#define DEFAULT_UPDATE_GNSS_P       5000    ///< Default GNSS update period in ms
#define DEFAULT_UPDATE_MQTT_P       5000    ///< Default MQTT update period in ms
#define LTE_CONNECT_MAX             3       ///< Maximum number of LTE connection attempts
#define LTE_CONNECTION_TIMEOUT      15000   ///< Delay between LTE connection attempt

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* NAMESPACE Declaration                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
namespace application
{
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* Class Declaration                                                                                            */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    class PicoWatch : public PicoIReactive
        #if (USE_BUTTONS != 0)
            , public gpio::ButtonObserver
        #endif
        #if (USE_MQTT != 0)
            , public ILTEObserver
            , public IMQTTConnectionObserver
        #endif
        #if (USE_CALENDAR != 0)
            , public cal::CalendarObserver
        #endif  // #if (USE_CALENDAR != 0)
        #if (USE_GNSS != 0)
            , public IGnssObserver
        #endif
            , public IBatteryObserver
    {
        public:

        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Enumeration used to have a unique identifier for every button action.
         ************************************************************************************************************/
        enum
        {
            BTN_INIT,
            BTN_A_CLC,              ///< Bouton BT0 (A) short press (clic)
            BTN_A_DBL,              ///< Bouton BT0 (A) double press
            BTN_A_TRPL,             ///< Bouton BT0 (A) triple press
            BTN_A_LNG,              ///< Bouton BT0 (A) long press
            BTN_A_VLNG,             ///< Bouton BT0 (A) very long press
            BTN_B_CLC,              ///< Bouton BT1 (B) short press (clic)
            BTN_B_DBL,              ///< Bouton BT1 (B) double press
            BTN_B_TRPL,             ///< Bouton BT1 (B) triple press
            BTN_B_LNG,              ///< Bouton BT1 (B) long press
            BTN_B_VLNG,             ///< Bouton BT1 (B) very long press
            BTN_C_CLC,              ///< Bouton BT2 (C) short press (clic)
            BTN_C_DBL,              ///< Bouton BT2 (C) double press
            BTN_C_TRPL,             ///< Bouton BT2 (C) triple press
            BTN_C_LNG,              ///< Bouton BT2 (C) long press
            BTN_C_VLNG,             ///< Bouton BT2 (C) very long press
            BTN_RELEASE,
        } buttonActions;

        /*********************************************************************************************************
         * @brief   Initializes the components needed by this class
         *
         * @warning Must be called before any other method of this class
         ********************************************************************************************************/
        void init();
        void bind(
            #if (USE_LEDS != 0) && (LEDS_NUMBER != 0)
                  gpio::LedController* ledA
                #if (LEDS_NUMBER > 1)
                , gpio::LedController* ledB
                #endif  // #if (LEDS_NUMBER > 1)
                #if (LEDS_NUMBER > 2)
                , gpio::LedController* ledC
                #endif  // #if (LEDS_NUMBER > 2)
                #if (LEDS_NUMBER > 3)
                , gpio::LedController* ledD
                #endif  // #if (LEDS_NUMBER > 3)
            #endif  // #if (USE_LEDS != 0) && (LEDS_NUMBER != 0)
            #if (USE_BUTTONS != 0) && (BUTTONS_NUMBER != 0)
                #if ((USE_LEDS != 0) && (LEDS_NUMBER > 0))
                ,
                #endif
                  gpio::Button* buttonA
                #if (BUTTONS_NUMBER > 1)
                , gpio::Button* buttonB
                #endif  // #if (BUTTONS_NUMBER > 1)
                #if (BUTTONS_NUMBER > 2)
                , gpio::Button* buttonC
                #endif  // #if (BUTTONS_NUMBER > 2)
                #if (BUTTONS_NUMBER > 3)
                , gpio::Button* buttonD
                #endif  // #if (BUTTONS_NUMBER > 3)
            #endif  // #if (USE_BUTTONS != 0) && (BUTTONS_NUMBER != 0)
            );

        bool processEvent(PicoEvent* event);

        void startBehaviour();

        /********************************************************************************************************//**
         * @brief   Get the application Instance (Singleton)
        ************************************************************************************************************/
        // Singleton
        static PicoWatch* getInstance();

        /********************************************************************************************************//**
         * @brief   Zephyr kernel work handler (callback) for application's work
         ************************************************************************************************************/
        static void picoWatchTaskHandler(struct k_work* work);

        /********************************************************************************************************//**
         * @brief   Zephyr kernel timer handler (callback) for application's timeout
         ************************************************************************************************************/
        static void picoWatchTimeoutHandler(struct k_timer *timer_id);

        #if (USE_CALENDAR != 0)
            void onSecond(cal::Calendar* calendar);
        #endif

        #if (USE_MQTT != 0)
            // From ILTEObserver interface
            void onConnected(LTE* lte);
            void onDisconnected(LTE* lte);
            void onError(LTE* lte);

            // From IMQTTObserver
            void onConnected(MQTTController* mqtt);
            void onDisconnected(MQTTController* mqtt);
            void onError(MQTTController* mqtt, int error);
        #endif

        #if (USE_GNSS != 0)
            // From IGnssObserver
            void onReceive(GnssController* gnss);
            void onConnected(GnssController* gnss);
            void onDisconnected(GnssController* gnss);
            void onError(GnssController* gnss);
        #endif

        void onInterrupt(BatteryManager* batterManager);
        void onBatLvlValReady(BatteryManager* batterManager);
        void onError(BatteryManager* batterManager, int error);

protected:
        /********************************************************************************************************//**
         * @brief   Enumeration used to identifiy application's works (k_work_q).
         ************************************************************************************************************/
        enum {
            WRK_INIT,                           ///< Init event
            WRK_UPDATE_MOT,                     ///< Update motor position event
            WRK_UPDATE_GNSS,
            WRK_UPDATE_MQTT,
            WRK_UPDATE_DISP,
            WRK_BTN_RELEASED,                   ///< Button released event
            WRK_BTN_LONG_PRESS,                 ///< Button long press event
            WRK_DONE,                           ///< Next state event (previous state done)
            //...Add works here
            WRK_NR_WORKS                        ///< Used to get the number of works in the enum (do not modify)
        } workId;

        /********************************************************************************************************//**
         * @brief   Enumeration used to identifiy application's timers (k_timer).
         ************************************************************************************************************/
        enum {
            T_UPDATE_MOT,                       ///< Update motor position timeout
            T_LONG_PRESS,                       ///< Button longpress timeout
            T_UPDATE_GNSS,                      ///< GNSS data update timeout
            T_UPDATE_MQTT,                      ///< MQTT tracking update
            T_UPDATE_DISP,
            //...Add timer here
            T_NR_TIMERS                         ///< Used to get the number of timers in the enum (do not modify)
        } timerId;

        /********************************************************************************************************//**
         * @brief   Event type identification
         ************************************************************************************************************/
        enum {
            EV_INIT,                            ///< Init event
            EV_TIMEOUT,                         ///< Timeout event
            EV_BUTTON,                          ///< Button event
            EV_TRANSITION,                      ///< Transition event
            //...Add timer here
            T_NR_EV_TYPE                         ///< Used to get the number of event type in the enum (do not modify)
        } eventType;

        /********************************************************************************************************//**
         * @brief   Enumeration used to have a unique identifier for every state in the state machine.
         ************************************************************************************************************/
        typedef enum PicoWatchState_
        {
            ST_UNKNOWN = 0,
            ST_INITIAL,
            ST_INIT,
            ST_IDLE,
            ST_WAIT,
            ST_SET_MOT_POS,
            ST_SET_GNSS_ANT,
            ST_CONNECT,
            ST_CONNECTION_KEEPALIVE,
            ST_START_MOT,
            ST_START_GNSS,
            ST_START_MQTT,
            ST_UPDATE_LOCAL_TIME,
            ST_GET_BAT_MANAGER_CONFIG,
            ST_SET_BAT_MANAGER_CONFIG,
            ST_CLEAR_BAT_MANAGER_FLAGS,
            ST_GPS_TRACKING_MODE,
            ST_NR_STATE
        } ePicoWatchState;
        /********************************************************************************************************//**
         * @brief   Process a work from the application's workqueue
         *
         * @param   stateId The identificator of the work to be processed
         *
         * @return  true if the work was seccessfully processed
         ************************************************************************************************************/
        void pushEvent(PicoEvent* event);

        void mqttInternalProcessEvent(PicoEvent* event);

        ePicoWatchState _mqttCurrentState;
        ePicoWatchState _mqttOldState;
        ePicoWatchState _currentState;

        PicoEvent evInitial_;
        PicoEvent evDefault_;
        PicoEvent evRelease_;
        PicoEvent evLongPress_;

        #if (USE_MQTT != 0)

            typedef enum MQTTEvents_ {evMQTTinit=100,
                              evMQTTConnected,
                              evMQTTError} MQTTEvents;

            typedef enum LTEEvents_ {evLTEinit=105,
                              evLTEConnectionKeepAlive,
                              evLTEConnected,
                              evLTEError} LTEEvents;

            PicoEvent evConnectionTimeout_;

            uint8_t conAttemptcounter_;

            bool isLteConnectProcessActive_;
            bool isMqttConnectProcessActive_;
            bool isLteActive_;

            void sendMqttLocation();

            void sendMqttTimeDate();

            void sendMqttMessage_(MQTTController* mqtt, string json, string topic);

            void startConnectionTimeout_();

            uint8_t connectionKeepAliveCounter_;
        #endif

        PicoEvent evConnected_;
        PicoEvent evError_;

        struct  EvElement
        {
            uint8_t id;
            uint8_t type;
        };

        EvElement   _currentEvent;
        uint8_t     _currentButtonAction;

        #if (USE_BUTTONS != 0)
            void onButtonSingleClick(gpio::Button* button);
            void onButtonDoubleClick(gpio::Button* button);
            void onButtonTripleClick(gpio::Button* button);
            void onButtonLongPress(gpio::Button* button);
            void onButtonDoubleLongPress(gpio::Button* button);
            void onButtonTripleLongPress(gpio::Button* button);
            void onButtonVeryLongPress(gpio::Button* button);
            void onButtonReleased(gpio::Button* button);

            const gpio::Button* button(int index) const;
            gpio::Button* button(int index);
        #endif  // #if (USE_BUTTONS != 0)

        void nextID(uint8_t* pID, uint8_t MAX, uint8_t MIN = 0);
        void previousID(uint8_t* pID, uint8_t MAX, uint8_t MIN = 0);

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */

        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* CONSTRUCTOR SECTION                                                                                      */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        PicoWatch();
        virtual ~PicoWatch() {}

        // ---- Attributes ------------------------------------------------------------------------------------------

        void pushDefaultEvent_(int delay = 0);

        // Timer and WorQueue
        static struct k_work    _picoWorks[application::PicoWatch::WRK_NR_WORKS];
        static struct k_timer   _picoTimers[application::PicoWatch::T_NR_TIMERS];

        #if (USE_LEDS != 0) && (LEDS_NUMBER != 0)
            gpio::LedController*                _ledA;
            #if (LEDS_NUMBER > 1)
                gpio::LedController*            _ledB;
            #endif  // #if (LEDS_NUMBER > 1)
            #if (LEDS_NUMBER > 2)
                gpio::LedController*            _ledC;
            #endif  // #if (LEDS_NUMBER > 2)
            #if (LEDS_NUMBER > 3)
                gpio::LedController*            _ledD;
            #endif  // #if (LEDS_NUMBER > 3)
            void                                _allLedsOff();
            void                                _allLedsOn();
        #endif  // #if (USE_LEDS != 0) && (LEDS_NUMBER != 0)


        #if (USE_BUTTONS != 0) && (BUTTONS_NUMBER != 0)
            static const uint8_t            _nbrButtons = BUTTONS_NUMBER;
            gpio::Button*                   _buttons[_nbrButtons];
            void setMotorPosition(uint8_t buttonAction);
        #endif

        void setGnssAnt(uint8_t buttonAction);

        #if (USE_MQTT != 0)
            void disconnect_();
            void mqttConnect_();

            static string _mqttData;
        #endif

        #if (USE_GNSS != 0)
            uLocation_t                     _gnssLocation;
            GnssController::gDate_t         _gnssDateTime;
        #endif

        uint32_t                            _updateGnssPeriode;
        uint32_t                            _updateMqttPeriode;

        //flag
        bool                                _isMotorOn;
        bool                                batLvlMvMode_;
        BatteryManager::BatData             batData_;
        // state machine's Actions
        void 		                        _ST_SYSTEM_OFF_Action();
    };

} // namespace application