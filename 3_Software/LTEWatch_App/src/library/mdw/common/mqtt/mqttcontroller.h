/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics.
 * Created by Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    mqttcontroller.h
 *
 * @addtogroup MQTT
 * \{
 *
 * @class   MQTTContoller
 * @brief   PicoIReactive Class to control the MQTT Client one
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2022-10-05
 *
 * \author  Patrice Rudaz
 * \date    October 2022
 ********************************************************************************************************************/
#pragma once


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include "imqttobserver.h"
#include "imqttclientobserver.h"

#include <islist.hpp>
#include <isubject.hpp> 
#include <picoireactive.h>
#include <picoevent.h>

#include "config/mqtt-config.h"

#include <string>

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Namespace Declaration                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
using namespace std;


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class MQTTClient;

class MQTTController : public PicoIReactive, public IMQTTClientObserver, public ISubject<IMQTTObserver, CONFIG_MQTT_MAX_OBSERVERS>
{
public:
    MQTTController();
    virtual ~MQTTController() {}

    void init(string userId, string username = "", string password = "");
    bool connect();
    void disconnect();
    bool processEvent(PicoEvent* event);
    void startBehaviour();
    void addMessage(string msg, string topic);

    inline bool isConnected() const { return state_ == ST_CONNECTED; }

protected:
    typedef enum MQTTState_
    {
        ST_INITIAL,
        ST_INIT,
        ST_IDLE,
        ST_CONNECTED,
        ST_CONNECTED_WAIT,
        ST_CONNECTED_SEND,
        ST_CONNECTED_POP,
        ST_DISCONNECTED
    } eMQTTState;

    void pushEvent(PicoEvent* event);
    bool processInternalEvent(PicoEvent* event);

    // MQTT Client Interface implementation -------------------------------------------------------------------------
    void onConnected(MQTTClient* client);
    void onDisconnected(MQTTClient* client);
    void onMessage(MQTTClient* client, const char* message, const char* topic);
    void onError(MQTTClient* client, int error);

private:
    eMQTTState state_;
    eMQTTState connectedState_;
    IsList<string> messageList_;
    IsList<string> topicList_;

    PicoEvent evConnected_;
    PicoEvent evDefault_;
    PicoEvent evDisconnected_;
    PicoEvent evInitial_;
    PicoEvent evPushMsg_;

    MQTTClient* client_;
    int mqttPingDelay_;

    void sendNext_();
    void popMsg_();
    void pingBroker_();
    void pushDefaultEvent_(int delay = 0);

    void notifyObservers_(bool connected);
};

/** \} */   // Group: MQTT
