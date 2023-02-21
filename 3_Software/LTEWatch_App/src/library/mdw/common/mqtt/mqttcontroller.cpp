/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics.
 * Created by Patrice Rudaz & Thomas Sterren
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "mqttcontroller.h"
#include "mqttclient.h"
#include <picoxf.h>


/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#ifndef CONFIG_HEI_MQTT_LOG_LEVEL
    #define CONFIG_HEI_MQTT_LOG_LEVEL       4U
#endif

#if (CONFIG_HEI_MQTT_LOG_LEVEL != 0)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(MQTTController, CONFIG_HEI_MQTT_LOG_LEVEL);

    #define MQTT_CTRL_DEBUG(args...)        LOG_DBG(args)
    #define MQTT_CTRL_ERROR(args...)        LOG_ERR(args)
    #define MQTT_CTRL_INFO(args...)         LOG_INF(args)
    #define MQTT_CTRL_WARN(args...)         LOG_WRN(args)
#else
    #define MQTT_CTRL_DEBUG(...)            {(void) (0);}
    #define MQTT_CTRL_ERROR(...)            {(void) (0);}
    #define MQTT_CTRL_INFO(...)             {(void) (0);}
    #define MQTT_CTRL_WARN(...)             {(void) (0);}
#endif


/* **************************************************************************************************************** */
/* CONSTRUCTOR/DESTRUCTOR SECTION                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
MQTTController::MQTTController() : state_(ST_INITIAL), connectedState_(ST_INITIAL)
{
    messageList_.empty();
    topicList_.empty();

    evConnected_.set(PicoEvent::evConnected, this);
    evDefault_.set(PicoEvent::evDefault, this);
    evDisconnected_.set(PicoEvent::evDisconnected, this);
    evInitial_.set(PicoEvent::evInitial, this);
    evPushMsg_.set(PicoEvent::evPushMsg, this);

    client_ = MQTTClient::getInstance();

    mqttPingDelay_ = (CONFIG_MQTT_KEEPALIVE_SEC - 1);
    if (mqttPingDelay_ < 0)
    {
        mqttPingDelay_ = -mqttPingDelay_;
    }
    else if (mqttPingDelay_ == 0)
    {
        mqttPingDelay_++;
    }
    mqttPingDelay_ *= 1000;
    MQTT_CTRL_WARN("mqttPingDelay_: %d [ms]", mqttPingDelay_);
}


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void MQTTController::init(string userId, string username, string password)
{
    client_->setHostname(CONFIG_MQTT_BROKER_HOSTNAME);
    client_->setPort(CONFIG_MQTT_BROKER_PORT);
    client_->addObserver(this);

    if (userId.size() > 0)
    {
        client_->setClientId(userId);
    }

    if (username.size() > 0)
    {
        client_->setUsername(username);
    }

    if (password.size() > 0)
    {
        // Nothing to do here yet
    }
}

// ------------------------------------------------------------------------------------------------------------------
bool MQTTController::connect()
{
    if (client_->isConnected())
    {
        MQTT_CTRL_DEBUG("MQTT Client already connected !");
        return true;
    }

    if (client_->connectToHost())
    {
        MQTT_CTRL_DEBUG("MQTT Client is connecting to Host...");
        client_->waitConnect();
    }
    else
    {
        MQTT_CTRL_ERROR("MQTT Client Connection process FAILED !");
    }

    return false;
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTController::disconnect()
{
    if (!client_->isConnected())
    {
        MQTT_CTRL_DEBUG("MQTT Client already disconnected !");
    }
    else
    {
        if (client_->disconnectFromHost())
        {
            MQTT_CTRL_DEBUG("MQTT Client is disconnecting from Host...");
            client_->waitDisconnect();
            pushEvent(&evDisconnected_);
        }
        else
        {
            MQTT_CTRL_ERROR("MQTT Client Disconnection process FAILED !");
            state_ = ST_IDLE;
            return;
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTController::startBehaviour()
{
    pushEvent(&evInitial_);
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTController::addMessage(string msg, string topic)
{
    messageList_.add(msg);
    topicList_.add(topic);
    MQTT_CTRL_DEBUG("Adding to msgList_: %s with topic: %s (size: %d)", msg.c_str(), topic.c_str(), messageList_.size());
    pushEvent(&evPushMsg_);
}


/* **************************************************************************************************************** */
/* PROTECTED SECTION                                                                                                */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void MQTTController::pushEvent(PicoEvent* event)
{
    PicoXF::getInstance()->pushEvent(event);
}

// ------------------------------------------------------------------------------------------------------------------
bool MQTTController::processEvent(PicoEvent* event)
{
    if (state_ == ST_CONNECTED)
    {
        if (processInternalEvent(event))
        {
            return true;
        }
    }

    // Storing current store...
    eMQTTState oldState = state_;

    // State transition Switch
    switch (state_)
    {
        case ST_INITIAL:
        {
            if (event->getId() == PicoEvent::evInitial)
            {
                state_ = ST_INIT;
            }
            break;
        }
        case ST_INIT:
        {
            if (event->getId() == PicoEvent::evDefault)
            {
                state_ = ST_IDLE;
            }
            break;
        }
        case ST_IDLE:
        {
            if (event->getId() == PicoEvent::evConnected)
            {
                state_ = ST_CONNECTED;
            }
            break;
        }
        case ST_CONNECTED:
        {
            if (event->getId() == PicoEvent::evDisconnected)
            {
                state_ = ST_DISCONNECTED;
            }
            break;
        }
        case ST_DISCONNECTED:
        {
            if (event->getId() == PicoEvent::evDefault)
            {
                state_ = ST_IDLE;
            }
            break;
        }
        default:
        {
            MQTT_CTRL_DEBUG("Unhandled Transition State ! (%d)", state_);
            break;
        }
    } // end switch

    if (oldState != state_)
    {
        // Exit action switch
        if (oldState == ST_CONNECTED)
        {
            connectedState_ = ST_INITIAL;
        }

        // Entry action Switch
        switch (state_)
        {
            case ST_INIT:
            {
                client_->initClient();
                pushDefaultEvent_();
                break;
            }
            case ST_IDLE:
            {
                // Waiting for connection event (nothing to do)
                break;
            }
            case ST_CONNECTED:
            {
                notifyObservers_(true);

                string connectJson = "{\"gateway_status\":\"connected\"}";
                addMessage(connectJson, CONFIG_MQTT_PUB_TOPIC);
                break;
            }
            case ST_DISCONNECTED:
            {
                notifyObservers_(false);
                pushDefaultEvent_();
                break;
            }
            default:
            {
                 MQTT_CTRL_DEBUG("Unhandled Action State ! (%d)", state_);
                break;
            }
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------------------------------------
bool MQTTController::processInternalEvent(PicoEvent* event)
{
    bool processed = false;

    eMQTTState oldState = connectedState_;
    switch (connectedState_)
    {
        case ST_INITIAL:
        {
            if (event->getId() == PicoEvent::evInitial || event->getId() == PicoEvent::evPushMsg)
            {
                if (messageList_.isEmpty())
                {
                    connectedState_ = ST_CONNECTED_WAIT;
                }
                else
                {
                    connectedState_ = ST_CONNECTED_SEND;
                }
            }
            break;
        }
        case ST_CONNECTED_WAIT:
        {
            if (event->getId() == PicoEvent::evPushMsg)
            {
                connectedState_ = ST_CONNECTED_SEND;
            }
            break;
        }
        case ST_CONNECTED_SEND:
        {
            if (event->getId() == PicoEvent::evDefault)
            {
                connectedState_ = ST_CONNECTED_POP;
            }
            break;
        }
        case ST_CONNECTED_POP:
        {    
            if (event->getId() == PicoEvent::evDefault)
            {
                if (messageList_.isEmpty())
                {
                    connectedState_ = ST_CONNECTED_WAIT;
                }
                else
                {
                    connectedState_ = ST_CONNECTED_SEND;
                }
            }
            break;
        }
        default:
            break;
    }

    if (oldState != connectedState_)
    {
        processed = true;

        // Entry action switch
        switch (connectedState_)
        {
            case ST_CONNECTED_WAIT:
            {
                // Ping the broker at regurlarly interval
                pingBroker_();
                break;
            }
            case ST_CONNECTED_SEND:
            {
                sendNext_();
                break;
            }
            case ST_CONNECTED_POP:
            {
                popMsg_();
                break;
            }
            default:
                break;
        }
    }
    else if (oldState == connectedState_ && oldState == ST_CONNECTED_WAIT) 
    {
        if (event->getId() == PicoEvent::evDefault)
        {
            processed = true;
            pingBroker_();
        }
    }
    return processed;
}


// MQTT Client Interface implementation -------------------------------------------------------------------------
void MQTTController::onConnected(MQTTClient* client)
{
    MQTT_CTRL_DEBUG("MQTT Client get connected");
    pushEvent(&evConnected_);
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTController::onDisconnected(MQTTClient* client)
{
    MQTT_CTRL_DEBUG("MQTT Client has disconnected");
    messageList_.removeAll();
    topicList_.removeAll();

    // To close any opened socket !
    mqtt_abort(client->client());
    pushEvent(&evDisconnected_);
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTController::onMessage(MQTTClient* client, const char* message, const char* topic)
{
    MQTT_CTRL_INFO("MQTT Message: %s %s", topic, message);
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTController::onError(MQTTClient* client, int error)
{
    MQTT_CTRL_WARN("MQTT Error: %d", error); 
    mqtt_abort(client->client());
}


/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void MQTTController::sendNext_()
{
    string topic    = topicList_.front();;
    string msg      = messageList_.front();
    if (client_->publish(topic, msg))
    {
        MQTT_CTRL_WARN("Message published: %s %s (len: %d, size: %d)", topic.c_str(), msg.c_str(), msg.length(), msg.size());
        pushDefaultEvent_();
    }
    else
    {
        pushEvent(&evDisconnected_);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTController::popMsg_()
{
    #if (CONFIG_HEI_MQTT_LOG_LEVEL != 0)
        uint8_t before = messageList_.size();
        uint8_t befor2 = topicList_.size();
    #endif

    messageList_.pop_front();
    topicList_.pop_front();

    #if (CONFIG_HEI_MQTT_LOG_LEVEL != 0)
        uint8_t after =  messageList_.size();
        uint8_t afte2 =  topicList_.size();
        MQTT_CTRL_INFO("Message Queues Size: %d (%d) - %d (%d)", after, before, afte2, befor2);
    #endif

    pushDefaultEvent_();
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTController::pingBroker_()
{
    if (evDefault_.getDelay() != 0)
    {
        PicoXF::getInstance()->unscheduleTM(&evDefault_);
    }

    // if (client_->pingBroker(CONFIG_MQTT_PUB_TOPIC, "\"ping\":true"))
    if (client_->pollMQTT())
    {
        MQTT_CTRL_DEBUG("client_->pingBroker() succeeded!");
        pushDefaultEvent_(mqttPingDelay_);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTController::pushDefaultEvent_(int delay)
{
    if (evDefault_.getDelay() != 0)
    {
        PicoXF::getInstance()->unscheduleTM(&evDefault_);
    }

    evDefault_.setDelay(delay);
    pushEvent(&evDefault_);
}

// ------------------------------------------------------------------------------------------------------------------
void MQTTController::notifyObservers_(bool connected)
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
