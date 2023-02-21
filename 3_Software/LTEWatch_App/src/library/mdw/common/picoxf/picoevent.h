#ifndef EVENT_ONCE
#define EVENT_ONCE
#include "picoireactive.h"

#include <stdint.h>
#include <zephyr.h>


class PicoEvent
{
public:
    typedef uint8_t evIDT;
    typedef enum evID { evNone, evInitial, evTimeout, evDefault, evPressed, evReleased, evInt,
                        // LTE/MQTT events
                        evConnected, evDisconnected, evPushMsg,
                        //PicoWatch
                        evLongPress, evTimeoutUpdateMOT, evTimeoutUpdateGNSS, evTimeoutUpdateMQTT, evTimeoutLongPress,
                        // Generic
                        evError
                       } evID;
    PicoEvent(/* args */);
    PicoEvent(evID eventID);
    ~PicoEvent();
    
    void set(evID eventID = evNone, PicoIReactive* target = nullptr, bool doNotDelete = true);
    inline void setTarget(PicoIReactive* target)    { this->target = target; }
    inline PicoIReactive* getTarget()               { return target; }
    inline void setId(evIDT eventID)            { id = eventID; }
    inline evIDT getId()                        { return id; }
    inline void setDnd(bool doNotDelete)        { doNotDeleteAfterConsume = doNotDelete; }
    inline bool getDnd()                        { return doNotDeleteAfterConsume; }
    inline void setDelay(int delay)             { this->delay = delay; }
    inline int getDelay()                       { return delay; }
    inline void setT(struct k_timer* timer)     { t = timer; }
    inline struct k_timer* getT()               { return t; }
    inline void setData(int64_t d)              { data = d; }
    inline int64_t getData()                    { return data; }

    static const char* to_string(evIDT id);

private:
    evIDT id;
    PicoIReactive* target;
    struct k_timer* t;
    bool doNotDeleteAfterConsume;
    int delay;
    int64_t data;
};

#endif // EVENT_ONCE