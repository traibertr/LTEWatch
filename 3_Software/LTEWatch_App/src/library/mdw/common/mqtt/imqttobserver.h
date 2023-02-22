/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics.
 * Created by Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file        imqttclientobserver.h
 *
 * @addtogroup  MQTT
 * \{
 *
 * \interface   IMQTTClientObserver
 * @brief       Interface of any MQTT Client
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
#include <string>

using namespace std;

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class MQTTController;

class IMQTTObserver
{
public:
    virtual void onConnected(MQTTController* mqtt) = 0;
    virtual void onDisconnected(MQTTController* mqtt) = 0;
    virtual void onMessage(MQTTController* mqtt, const char* message, const char* topic) = 0;
    virtual void onError(MQTTController* mqtt, int error) = 0;
};

class IMQTTConnectionObserver : public IMQTTObserver
{
public:
    void onMessage(MQTTController* mqtt, const char* message, const char* topic) {};
};

class IMQTTMessageObserver : public IMQTTObserver
{
public:
    void onConnected(MQTTController* mqtt) {};
    void onDisconnected(MQTTController* mqtt) {};
};



/** \} */   // Group: MQTT
