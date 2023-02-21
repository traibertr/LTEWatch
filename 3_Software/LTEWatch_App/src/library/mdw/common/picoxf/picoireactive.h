#ifndef PICO_IREACTIVE_ONCE
#define PICO_IREACTIVE_ONCE

class PicoEvent;

class PicoIReactive
{
public:
    virtual bool processEvent(PicoEvent* event) = 0;
    void startBehaviour();
};

#endif