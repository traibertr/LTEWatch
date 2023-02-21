/* Copyright (c) 2022, HES-SO Valais
 * All rights reserved.
 */
#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

#define CONFIG_MQTT_PUB_TOPIC               "v1/devices/me/telemetry"
#define CONFIG_MQTT_ATTR_TOPIC              "v1/devices/me/attributes"
#define CONFIG_MQTT_CLIENT_USERNAME         ""
#define CONFIG_MQTT_CLIENT_PWD              ""
#define CONFIG_MQTT_CLIENT_ID               "LTEWatch_2"
#define CONFIG_MQTT_BROKER_HOSTNAME         "tb.ecs.hevs.ch"
#define CONFIG_MQTT_BROKER_PORT             1884
#define CONFIG_MQTT_KEEPALIVE_SEC           30
#define CONFIG_MQTT_MESSAGE_BUFFER_SIZE     256
#define CONFIG_MQTT_PAYLOAD_BUFFER_SIZE     512
#define CONFIG_MQTT_MAX_OBSERVERS           1
#define CONFIG_MQTT_MAX_DNS_RETRY           3
#define CONFIG_HEI_MQTT_LOG_LEVEL           3U

#endif // MQTT_CONFIG_H
