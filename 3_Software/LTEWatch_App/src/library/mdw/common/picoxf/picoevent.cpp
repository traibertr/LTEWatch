#include "picoevent.h"

PicoEvent::PicoEvent() : PicoEvent(evNone)
{}

PicoEvent::PicoEvent(evID eventID) {
    id = eventID;
    t = nullptr;
    delay = 0;
    doNotDeleteAfterConsume = false;
    target = nullptr;
    data = 0x0;
}

PicoEvent::~PicoEvent() {
    
}

void PicoEvent::set(evID eventID, PicoIReactive* target, bool doNotDelete)
{
    id                      = eventID;
    this->target            = target;
    this->delay             = delay;
    doNotDeleteAfterConsume = doNotDelete;
}

// Static
const char* PicoEvent::to_string(evIDT id)
{
    switch (id) 
    {
        case evNone:            return "evNone";
        case evInitial:         return "evInitial";
        case evTimeout:         return "evTimeout";
        case evDefault:         return "evDefault";
        case evPressed:         return "evPressed";
        case evReleased:        return "evReleased";
        case evInt:             return "evInt";
        case evConnected:       return "evConnected";
        case evDisconnected:    return "evDisconnected";
        case evPushMsg:         return "evPushMsg";
        default :               return "Unknown PicoEvent ID";
    }
}
