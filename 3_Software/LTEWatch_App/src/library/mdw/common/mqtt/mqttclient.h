/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics.
 * Created by Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    mqttclient.h
 *
 * @addtogroup MQTT
 * \{
 *
 * @class   MQTTCleint
 * @brief   Class which handles a client connection using MQTT protocol
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
#include <kernel.h>
#include <net/mqtt.h>
#include <net/socket.h>
#include <string>

#include <isubject.hpp>

#include "imqttclientobserver.h"
#include "config/mqtt-config.h"
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
class MQTTClient : public ISubject<IMQTTClientObserver, CONFIG_MQTT_MAX_OBSERVERS>
{
public:

    typedef enum MQTTEvent
    {
        MQTT_CONNECTED,
        MQTT_DISCONNECTED,
        MQTT_MSG_RECEIVED,
        MQTT_PUBACK,
        MQTT_PUBREC,
        MQTT_PUBREL,
        MQTT_PUBCOMP,
        MQTT_SUBACK,
        MQTT_UNSUBACK,
        MQTT_PINGRESP,
        MQTT_MSG_PUBLISHED
    } eMQTTEvent;


    virtual ~MQTTClient() {};

    static MQTTClient* getInstance();

    static void mqttEventHandler(struct mqtt_client* const client, const struct mqtt_evt* event);

    void initClient();
    bool connectToHost();
    bool disconnectFromHost();

    /**
     * @brief this method polls the TCP stream for incmming MQTT
     * 
     * This method polls the TCP stream for incmming MQTT message and also sends the periodical pings to keep the connection alive.
     */
    bool pollMQTT();
    void waitConnect();
    void waitDisconnect();

    inline void abort()                         { mqtt_abort(&client_); }


    void subscribeTopic(string topic);
    void unsubscribeTopic(string topic);

    bool publish(string topic, string message, mqtt_qos qos = MQTT_QOS_0_AT_MOST_ONCE);
    bool pingBroker();
    bool pingBroker(string pingTopic, string pingMsg, mqtt_qos qos = MQTT_QOS_0_AT_MOST_ONCE);

    inline bool isConnected() const             { return isConnected_; }
    inline void setConnected(bool val)          { isConnected_ = val; }

    inline string hostname() const              { return hostname_; }
    inline void setHostname(string hostname)    { hostname_     = hostname; }

    inline uint16_t port() const                { return port_; }
    inline void setPort(uint16_t port)          { port_         = port; }

    inline string clientId() const              { return clientId_; }
    inline void setClientId(string id)          { clientId_     = id; }

    inline string username() const              { return username_; }
    inline void setUsername(string username)    { username_     = username; }

    inline string password() const              { return password_; }
    inline void setPassword(string password)    { password_     = password; }

    inline bool cleanSession() const            { return cleanSession_; }
    inline void setCleanSession(bool cs)        { cleanSession_ = cs; }
    
    inline void resetPayload()                  { memset(payload_, 0x00, sizeof(payload_)); }
    inline uint8_t* payload()                   { return payload_; }
    inline size_t payloadSize()                 { return sizeof(payload_); }

    inline mqtt_client* client()                { return &client_; }

protected:
    void getPayload(size_t size, uint8_t* payload, const mqtt_topic* topic);

private:
    MQTTClient();

    struct mqtt_client client_;
    struct sockaddr_storage broker_;
    struct pollfd fds_;

    bool isConnected_;
    bool cleanSession_;
    bool isInitialized_;

    string hostname_;
    uint16_t port_;
    string clientId_;
    string username_;
    string password_;

    uint8_t rxBuffer_[CONFIG_MQTT_MESSAGE_BUFFER_SIZE];
    uint8_t txBuffer_[CONFIG_MQTT_MESSAGE_BUFFER_SIZE];
    uint8_t payload_[CONFIG_MQTT_PAYLOAD_BUFFER_SIZE];

    void clearBufferAndResetFlags_();
    bool brokerInit_();
    void clientInit_();
    bool fdsInit_();
    inline uint32_t getRandomMsgId_()       { return k_cycle_get_32(); }

    void notifyOnConnectionStateChanged_(bool connected);
    void notifyOnMessage_(const char* topic, const char* message);
    void notifyOnError_(int error);
};

/** \} */   // Group: MQTT
