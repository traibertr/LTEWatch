#ifndef BATT_MANAGER_ONCE
#define BATT_MANAGER_ONCE

#include <zephyr.h>
#include "batt-config.h"
#include "ibatteryobserver.h"
#include "drv_bq25180.h"
#include <picoireactive.h>
#include <picoevent.h>
#include <zephyr/drivers/gpio.h>
#include "isubject.hpp"

class BatteryManager : public PicoIReactive, public ISubject<IBatteryObserver, CONFIG_BAT_MAX_OBSERVERS>
{
public:

    typedef struct
    {
        uint8_t     batLvlInPercent;
        uint32_t    batLvlInMV;
        bool        isBatCharging;
    }BatData;

    virtual ~BatteryManager() {};

    void init(void);

    bool processEvent(PicoEvent* event);

    bool configBatteryCharger(void);

    bool getBatteryChargerConfig(void);

    bool clearBatteryChargerFlags(void);

    uint32_t getBatteryLevelInMV(void);

    uint32_t getBatteryLevelPercent(void);

    bool isBatteryCharging(void);

    void start(void);

    void getBatData(BatData* batData);

// Singleton
    static BatteryManager* getInstance();

protected:

private:
// Singleton pattern
    BatteryManager();

    typedef enum BatManState
    {
        ST_START,
        ST_IDLE,
        ST_INIT,
        ST_CONFIG,
        ST_INTERRUPT,
        ST_READ_ADC,
        ST_ERROR,
    } eBatManState;

    eBatManState _state;

    PicoEvent _evDefault;
    PicoEvent _evInterrupt;
    PicoEvent _evReadADC;

    typedef enum BatMngrEvents_ {evInterrupt=125,
                                 evReadADC,
                                 evDefault
                            } BatMngrEvents;

    void pushEvent(PicoEvent* event);

    static uint8_t _batteryChargerBuffer[DRV_BQ25180_NB_REG + DRV_BQ25180_START_ADDR];

    static uint8_t _BatChrgReadRegBuff[DRV_BQ25180_NB_REG + DRV_BQ25180_START_ADDR];

    static struct gpio_callback _BatteryChargerInterruptCallback;

    static void _BatteryManagerHandler(const struct device *dev,
                         struct gpio_callback *cb, uint32_t pins);

    bool registerBatteryInterrupt(gpio_callback_handler_t handler = _BatteryManagerHandler,
        struct gpio_callback *cb = &_BatteryChargerInterruptCallback);

    int32_t     _batLvl;
    int64_t     _average;
    uint16_t    _nbEch;
    int         _errorID;

    void _notifyObservers(eBatManState state);

    // If needed...
    static void batterManagerTaskHandler(struct k_work* work_id);
    static void batterManagerTimerHandler(struct k_timer *timer_id);
};

#endif // BATT_MANAGER_ONCE