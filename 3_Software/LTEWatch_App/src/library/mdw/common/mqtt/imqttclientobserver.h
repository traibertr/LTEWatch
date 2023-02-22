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
class MQTTClient;
class MQTTMessage;

class IMQTTClientObserver
{
public:
    virtual void onConnected(MQTTClient* client) = 0;
    virtual void onDisconnected(MQTTClient* client) = 0;
    virtual void onMessage(MQTTClient* client, const char* message, const char* topic) = 0;
    virtual void onError(MQTTClient* client, int error) = 0;
};

/** \} */   // Group: MQTT
