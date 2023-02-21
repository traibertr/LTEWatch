/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics.
 * Created by Patrice Rudaz & Thomas Sterren
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "mqttclient.h"

#include <sdktools.h>

#include <errno.h>
#include <zephyr/kernel.h>

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#ifndef CONFIG_HEI_MQTT_LOG_LEVEL
    #define CONFIG_HEI_MQTT_LOG_LEVEL       4U
#endif


#if (CONFIG_HEI_MQTT_LOG_LEVEL != 0)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(MQTTClient, CONFIG_HEI_MQTT_LOG_LEVEL);
    #define MQTT_CLIENT_DEBUG(args...)      LOG_DBG(args) 
    #define MQTT_CLIENT_ERROR(args...)      LOG_ERR(args) 
    #define MQTT_CLIENT_INFO(args...)       LOG_INF(args) 
    #define MQTT_CLIENT_WARN(args...)       LOG_WRN(args) 
#else
    #define MQTT_CLIENT_DEBUG(...)          {(void) (0);}
    #define MQTT_CLIENT_ERROR(...)          {(void) (0);}
    #define MQTT_CLIENT_INFO(...)           {(void) (0);}
    #define MQTT_CLIENT_WARN(...)           {(void) (0);}
#endif

    
/* **************************************************************************************************************** */
/* Application Declaration                                                                                          */
/* **************************************************************************************************************** */
// K_THREAD_STACK_DEFINE(rxMqttThreadStack, 512);


/* **************************************************************************************************************** */
/* CONSTRUCTOR/DESTRUCTOR SECTION                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
// Static
MQTTClient* MQTTClient::getInstance()
{
    static MQTTClient instance;
    return &instance;
}

// ------------------------------------------------------------------------------------------------------------------
// static 
void MQTTClient::mqttEventHandler(struct mqtt_client* const client, const struct mqtt_evt* event)
{
    int err = 0;
    MQTTClient* mqtt = MQTTClient::getInstance();

    switch (event->type)
    {
        case MQTT_EVT_CONNACK:
        {
            if (event->result == 0) 
            {
        		MQTT_CLIENT_INFO("MQTT client connected");
                mqtt->setConnected(true);
            } 
            else 
            {
                MQTT_CLIENT_ERROR("CONNACK error: %d", event->result);
            }
            break;
        }
        case MQTT_EVT_DISCONNECT:
        {
            MQTT_CLIENT_INFO("MQTT client disconnected: %d", event->result);
            mqtt->setConnected(false);
            break;
        }
        case MQTT_EVT_PINGRESP:
        {
            if (event->result != 0) 
            {
                MQTT_CLIENT_ERROR("PINGRESP error: %d", event->result);
            }
            break;
        }
        case MQTT_EVT_PUBACK:
        {
            if (event->result == 0) 
            {
                MQTT_CLIENT_DEBUG("PUBACK packet id: %u", event->param.puback.message_id);
            }
            else
            {
                MQTT_CLIENT_ERROR("PUBACK error: %d", event->result);
            }
            break;
        }
        case MQTT_EVT_PUBCOMP:
        {
            if (event->result == 0) 
            {
                MQTT_CLIENT_DEBUG("PUBCOMP packet id: %u", event->param.pubcomp.message_id);
            } 
            else 
            {
                MQTT_CLIENT_ERROR("PUBCOMP error: %d", event->result);
            }
            break;
        }
        case MQTT_EVT_PUBLISH:
        {
            const struct mqtt_publish_param* p = &event->param.publish;
            MQTT_CLIENT_INFO("MQTT PUBLISH result=%d len=%d", event->result, p->message.payload.len);

            if (event->result == 0)
            {
                if (p->message.topic.qos == MQTT_QOS_1_AT_LEAST_ONCE)
                {
                    const struct mqtt_puback_param pp = {
                        .message_id = p->message_id
                    };
                    mqtt_publish_qos1_ack(mqtt->client(), &pp);
                }

                mqtt->resetPayload();
                err = mqtt_read_publish_payload(mqtt->client(), mqtt->payload(), p->message.payload.len);
                if (err >= 0)
                {
                    mqtt->getPayload(err, p->message.payload.data, &p->message.topic);
                }
                else if (err == -EMSGSIZE)
                {
                    MQTT_CLIENT_ERROR("Received payload (%d bytes) is larger than the payload buffer size (%d bytes).", p->message.payload.len, mqtt->payloadSize());
                }
                else
                {
                    MQTT_CLIENT_ERROR("mqtt_read_publish_payload() FAILED ! (err: %d)", err);
                    LOG_WRN("Disconnecting MQTT client...");
                    err = mqtt_disconnect(client);
                    if (err) {
                        MQTT_CLIENT_ERROR("Could not disconnect: %d", err);
                    }
                }
            } 
            else 
            {
                MQTT_CLIENT_ERROR("MQTT PUBLISH error: %d", event->result);
            }
            break;
        }
        case MQTT_EVT_PUBREC:
        {
            if (event->result == 0) 
            {
                struct mqtt_pubrel_param pp;
                pp.message_id = event->param.pubrec.message_id;
                mqtt_publish_qos2_release(mqtt->client(), &pp);
                MQTT_CLIENT_DEBUG("PUBREC packet id: %d", pp.message_id);
            } 
            else 
            {
                MQTT_CLIENT_ERROR("PUBREC error: %d", event->result);
            }
            break;
        }
        case MQTT_EVT_PUBREL:
        {
            if (event->result == 0) 
            {
                struct mqtt_pubcomp_param pp;
                pp.message_id = event->param.pubrel.message_id;
                mqtt_publish_qos2_complete(mqtt->client(), &pp);
                MQTT_CLIENT_DEBUG("PUBREC packet id: %u", event->param.pubrel.message_id);
            } 
            else 
            {
                MQTT_CLIENT_ERROR("PUBREL error: %d", event->result);
            }
           break;
        }
        case MQTT_EVT_SUBACK:
        {
            if (event->result == 0) 
            {
                MQTT_CLIENT_INFO("SUBACK packet id: %d", event->param.suback.message_id);
            } 
            else 
            {
                MQTT_CLIENT_ERROR("SUBACK error: %d", event->result);
            }
            break;
        }
        case MQTT_EVT_UNSUBACK:
        {
            if (event->result == 0) 
            {
                MQTT_CLIENT_INFO("SUBACK packet id: %u", event->param.unsuback.message_id);
            } 
            else 
            {
                MQTT_CLIENT_ERROR("UNSUBACK error: %d", event->result);
            }
            break;
        }
        default:
            break;
    }
}


// ------------------------------------------------------------------------------------------------------------------
void MQTTClient::initClient()
{   
    clearBufferAndResetFlags_();
}

// ------------------------------------------------------------------------------------------------------------------
bool MQTTClient::connectToHost()
{
    // Checks if the client is initialized
    if (!isInitialized_)
    {
        LOG_WRN("Initialization should be done before !");
        // first of all, we have to initialize the client structure
        mqtt_client_init(&client_);

        // Resolving the ip address of the broker
        if (brokerInit_())
        {
            clientInit_();
            isInitialized_ = true;
            MQTT_CLIENT_DEBUG("MQTT Client intialized!");
        }
        else
        {
            MQTT_CLIENT_ERROR("brokerInit_() FAILED !");
        }

        // Check initialization overall process
        if (!isInitialized_)
        {
            MQTT_CLIENT_ERROR("Client intialization overall process FAILED !");
            return false;
        }
    }

    // Launching a connection attempt. If it has worked, we will learn in the mqttEventHandler, when the event
    // MQTT_EVT_CONNACK arrives
    int err = mqtt_connect(&client_);
    if (err != 0)
    {
        MQTT_CLIENT_ERROR("Client connect FAILED ! (err=%d)", err);
        if (err == -ENFILE)
        {
            err = mqtt_abort(&client_);
            MQTT_CLIENT_DEBUG("mqtt_abort() -> %d", err);
            err = mqtt_disconnect(&client_);
            MQTT_CLIENT_DEBUG("mqtt_disconnect() -> %d", err);
        }
        return false;
    }

    // if the connection attempt was done, we still have to initialize the file descriptor structure
    if (!fdsInit_())
    {
        return false;
    };

    return true;
}

// ------------------------------------------------------------------------------------------------------------------
bool MQTTClient::disconnectFromHost()
{
    int err = mqtt_disconnect(&client_);
    if (err != 0)
    {
        MQTT_CLIENT_ERROR("Client disconnect FAILED ! (err=%d)", err);
        return false;
    }
    return true;
}

// ------------------------------------------------------------------------------------------------------------------
bool MQTTClient::pollMQTT()
{
    int err = poll(&fds_, 1, 500);
    if (err < 0)
    {
        MQTT_CLIENT_ERROR("poll FAILED ! (err=%d)", err);
        return false;
    }

    if (pingBroker())
    {
        if ((fds_.revents & POLLIN) == POLLIN)
        {
            err = mqtt_input(&client_);
            if (err != 0)
            {
                MQTT_CLIENT_ERROR("mqtt_input FAILED ! (err=%d)", err);
                return false;
            }
        }

        if ((fds_.revents & POLLERR) == POLLERR)
        {
            MQTT_CLIENT_ERROR("revents error: POLLERR");
            return false;
        }

        if ((fds_.revents & POLLNVAL) == POLLNVAL)
        {
            MQTT_CLIENT_ERROR("revents error: POLLNVAL");
            return false;
        }
        return true;
    }

    return false;
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTClient::waitConnect()
{
    do 
    {
        if (!pollMQTT())
        {
            break; 
        }
    }
    while (!isConnected_);
    notifyOnConnectionStateChanged_(isConnected_);
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTClient::waitDisconnect()
{
    while (isConnected_) 
    {
        if (!pollMQTT())
        {
            break;
        }
    }
    notifyOnConnectionStateChanged_(isConnected_);
}



// ------------------------------------------------------------------------------------------------------------------
void MQTTClient::subscribeTopic(string topic)
{
    struct mqtt_topic subscribe_topic;
    int err;

    subscribe_topic.topic.utf8      = (uint8_t *) topic.c_str();
    subscribe_topic.topic.size      = topic.length();
    subscribe_topic.qos             = MQTT_QOS_1_AT_LEAST_ONCE;

    struct mqtt_subscription_list subscription_list;
    subscription_list.list          = &subscribe_topic;
    subscription_list.list_count    = 1;
    subscription_list.message_id    = getRandomMsgId_();

    err = mqtt_subscribe(&client_, &subscription_list);
    if (err != 0)
    {
        MQTT_CLIENT_ERROR("subscription to topic %s FAILED ! (err=%d)", topic.c_str(), err);
        notifyOnError_(err);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTClient::unsubscribeTopic(string topic)
{
    struct mqtt_topic subscribe_topic;
    int err;

    subscribe_topic.topic.utf8      = (uint8_t *) topic.c_str();
    subscribe_topic.topic.size      = topic.length();
    subscribe_topic.qos             = MQTT_QOS_1_AT_LEAST_ONCE;

    struct mqtt_subscription_list subscription_list;
    subscription_list.list          = &subscribe_topic;
    subscription_list.list_count    = 1;
    subscription_list.message_id    = getRandomMsgId_();

    err = mqtt_unsubscribe(&client_, &subscription_list);
    if (err != 0)
    {
        MQTT_CLIENT_ERROR("unsubscription from topic %s FAILED ! (err=%d)", topic.c_str(), err);
        notifyOnError_(err);
    }
}

// ------------------------------------------------------------------------------------------------------------------
bool MQTTClient::publish(string topic, string message, mqtt_qos qos)
{
    struct mqtt_publish_param param;
    int err;

    param.message.topic.qos = qos;
    param.message.topic.topic.utf8 = (uint8_t*) topic.c_str();
    param.message.topic.topic.size = topic.length();
    param.message.payload.data = (uint8_t*) message.c_str();
    param.message.payload.len = message.length();
    param.message_id = getRandomMsgId_();
    param.dup_flag = 0;
    param.retain_flag = 0;

    MQTT_CLIENT_DEBUG("ID: %d, Topic: %s, msg: %s", param.message_id, param.message.topic.topic.utf8, (char*) param.message.payload.data);
    err = mqtt_publish(&client_, &param);

    if (err != 0)
    {
        MQTT_CLIENT_DEBUG("publish message id %d SUCCEEDED.", param.message_id);
        notifyOnError_(err);
        return false;
    }
    return true;
}

// ------------------------------------------------------------------------------------------------------------------
bool MQTTClient::pingBroker()
{
    int err = mqtt_live(&client_);
    if (err != 0 && (err != -EAGAIN))
    {
        if (err == -ENOTCONN)
        {
            MQTT_CLIENT_WARN("MQTT no more connected ! (err=%d)", err);
            isConnected_ = false;
            notifyOnConnectionStateChanged_(isConnected_);
        }
        else
        {
            MQTT_CLIENT_ERROR("mqtt_live(...) FAILED ! (err=%d)", err);
            notifyOnError_(err);
        }
        return false;
    }
    return true;
}

// ------------------------------------------------------------------------------------------------------------------
bool MQTTClient::pingBroker(string pingTopic, string pingMsg, mqtt_qos qos)
{
    if ((pingTopic.length() == 0) || (pingMsg.length() == 0))
    {
        MQTT_CLIENT_ERROR("Invalid Ping Message !");
        return false;
    }

    MQTT_CLIENT_DEBUG("Send Ping finished...");
    return publish(pingTopic, pingMsg, qos);
}


// ------------------------------------------------------------------------------------------------------------------
// DEBUG
void MQTTClient::getPayload(size_t size, uint8_t* payload, const mqtt_topic* topic)
{
    string strPayload = tools::SDKTools::strFromUint8(payload, size);
    string strTopic   = tools::SDKTools::strFromUint8(const_cast<uint8_t*>(topic->topic.utf8), topic->topic.size);
    MQTT_CLIENT_DEBUG("MSG Received: %s %s", strPayload.c_str(), strTopic.c_str());
    notifyOnMessage_(strTopic.c_str(), strPayload.c_str());
}


/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
// constructor
MQTTClient::MQTTClient()
{
    clearBufferAndResetFlags_();
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTClient::clearBufferAndResetFlags_()
{
    memset(&client_, 0, sizeof(struct mqtt_client));
    memset(&broker_, 0, sizeof(struct sockaddr_storage));
    memset(&fds_, 0, sizeof(struct pollfd));
    
    memset(rxBuffer_, 0, sizeof(uint8_t) * CONFIG_MQTT_MESSAGE_BUFFER_SIZE);
    memset(txBuffer_, 0, sizeof(uint8_t) * CONFIG_MQTT_MESSAGE_BUFFER_SIZE);
    memset(payload_,  0, sizeof(uint8_t) * CONFIG_MQTT_PAYLOAD_BUFFER_SIZE);

    isConnected_    = false;
    cleanSession_   = true;
    isInitialized_  = false;
}

// ------------------------------------------------------------------------------------------------------------------
bool MQTTClient::brokerInit_()
{
    int err;
    struct addrinfo *result;
    struct addrinfo *addr;
    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM
    };

    // this is a new loop to try to reconnect for a maximum of 3 times if the error is EAGAIN
    uint8_t cnt = 0;
    do
    {
        if (hostname_.empty())
        {
            MQTT_CLIENT_WARN("No MQTT hostname set yet !");
            return false;
        }
        else
        {
            err = getaddrinfo(hostname_.c_str(), NULL, &hints, &result);
        }

    } 
    while (err == -EAGAIN && cnt++ < CONFIG_MQTT_MAX_DNS_RETRY);
    
    if (err)
    {
        MQTT_CLIENT_ERROR("getaddrinfo failed: %d", err);
        return false;
    }

    addr = result;
    err = -ENOENT;

    // Look for address of the broker.
    while (addr != NULL)
    {
        /* IPv4 Address. */
        if (addr->ai_addrlen == sizeof(struct sockaddr_in))
        {
            // struct sockaddr_in *broker4 = ((struct sockaddr_in *)&broker_);
            struct sockaddr_in *broker4 = reinterpret_cast<struct sockaddr_in *>(&broker_);
            char ipv4_addr[NET_IPV4_ADDR_LEN];
            // broker4->sin_addr.s_addr = ((struct sockaddr_in *)addr->ai_addr)->sin_addr.s_addr;
            broker4->sin_addr.s_addr = reinterpret_cast<struct sockaddr_in *>(addr->ai_addr)->sin_addr.s_addr;
            broker4->sin_family = AF_INET;
            
            if (port_ == 0)
            {
                LOG_ERR("No port given yet.");
                return false;
            }
            else
            {
                broker4->sin_port = htons(port_);
            }
            
            inet_ntop(AF_INET, &broker4->sin_addr.s_addr, ipv4_addr, sizeof(ipv4_addr));
            MQTT_CLIENT_INFO("IPv4 Address found: %s", ipv4_addr);
            break;
        }
        else
        {
            MQTT_CLIENT_ERROR("ai_addrlen = %d, should be %d or %d", addr->ai_addrlen, sizeof(struct sockaddr_in), sizeof(struct sockaddr_in6));
        }
        addr = addr->ai_next;
    }
    freeaddrinfo(result);
    return true;
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTClient::clientInit_()
{
    MQTT_CLIENT_DEBUG("Internal Initialization ...");
    
    // MQTT client configuration
    client_.keepalive = CONFIG_MQTT_KEEPALIVE_SEC;
    client_.broker = &broker_;
    client_.evt_cb = MQTTClient::mqttEventHandler;
    client_.client_id.utf8 = (uint8_t *)clientId_.c_str();
    client_.client_id.size = clientId_.length();
    MQTT_CLIENT_DEBUG("MQTT -> clientID : %s", clientId_.c_str());
    client_.password = NULL;
    client_.user_name = NULL;
    client_.protocol_version = MQTT_VERSION_3_1_1;
    client_.clean_session = cleanSession_ ? 1 : 0;
    
    // MQTT buffers configuration
    client_.rx_buf = rxBuffer_;
    client_.rx_buf_size = sizeof(rxBuffer_);
    client_.tx_buf = txBuffer_;
    client_.tx_buf_size = sizeof(txBuffer_);
    
    // MQTT User configuration
    if (username_.size() > 0)
    {
        client_.user_name->utf8 = (const uint8_t*) username_.c_str();
        client_.user_name->size = username_.size();
        MQTT_CLIENT_DEBUG("MQTT -> username : %s (%s)", username_.c_str(), client_.user_name->utf8);
    }

    if (password_.size() > 0)
    {
        client_.password->size = password_.size();
        client_.password->utf8 = (const uint8_t*) password_.c_str();
        MQTT_CLIENT_DEBUG("MQTT -> password : %s (%s)", password_.c_str(), client_.password->utf8);
    }

    /* MQTT transport configuration */
    client_.transport.type = MQTT_TRANSPORT_NON_SECURE;
}

// ------------------------------------------------------------------------------------------------------------------
bool MQTTClient::fdsInit_()
{
    if (client_.transport.type == MQTT_TRANSPORT_NON_SECURE)
    {
        fds_.fd = client_.transport.tcp.sock;
    }
    else
    {
        return false;
    }
    fds_.events = POLLIN;
    return true;
}


// ------------------------------------------------------------------------------------------------------------------
void MQTTClient::notifyOnConnectionStateChanged_(bool connected)
{
    for (uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != nullptr)
        {
            if (connected)
            {
                observer(i)->onConnected(this);
            }
            else
            {
                observer(i)->onDisconnected(this);
            }
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTClient::notifyOnMessage_(const char* topic, const char* message)
{
    for (uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != nullptr)
        {
            observer(i)->onMessage(this, message, topic);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTClient::notifyOnError_(int error)
{
    for (uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != nullptr)
        {
            observer(i)->onError(this, error);
        }
    }
}
