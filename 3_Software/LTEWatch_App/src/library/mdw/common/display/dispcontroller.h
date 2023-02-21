#pragma once

#include "config/disp-config.h"
#include "gnsscontroller.h"

#include <zephyr/zephyr.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/spi.h>

#include <stdio.h>
#include <string.h>
#include <lvgl.h>

class DispController
{
public:

typedef enum{
DISP_MQTT_CONNECTING_1,
DISP_MQTT_CONNECTING_2,
DISP_MQTT_CONNECTED,
DISP_MQTT_DISCONNECT,
DISP_MQTT_SENDING,
DISP_MQTT_ERROR,
}mqttState;

    void init(void);
    void start(void);

    static DispController* getInstance(void);

    void battDisp(uint8_t batteryLevelPercent, uint32_t batteryLevelMv, bool batMvMode, bool isBatteryCharging);
    void gnssDisp(uint8_t svs);
    void timeDisp(uint8_t hour, uint8_t min, uint8_t sec);
    void dateDisp(uint16_t year, uint8_t month, uint8_t day);
    void mqttDisp(uint8_t state);
    void setMotDisp(uint8_t motorId);
    void setGnssAnt(uint8_t gnssAntMode);
    void cleanDisp(void);
    void setModeDisp(char* modeName);

protected:

private:
    virtual ~DispController() {}
    bool _startFlag;
};