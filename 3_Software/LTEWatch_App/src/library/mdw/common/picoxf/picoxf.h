#ifndef PICOXF_ONCE
#define PICOXF_ONCE
#include <zephyr.h>
#include "picoevent.h"

#define CONFIG_PICOXF_EVENT_QUEUE_SIZE_MAX 64

using namespace std;

/**
 * @class The PicoXF 
 * @brief Our PicoEvent processing machine. 
 */
class PicoXF
{
public:
    void init();
    static PicoXF* getInstance();
    void pushEvent(PicoEvent* e);
    void execute();
    void unscheduleTM(PicoEvent* e);
private:
    PicoXF(/* args */);
    ~PicoXF();
    PicoEvent* popEvent();
    void scheduleTM(PicoEvent* e);
    static void onTimeout(struct k_timer* t);
    static void onStop(struct k_timer* t);
    static PicoXF thePicoXF;
    //list<PicoEvent*> evQueue;
    // struct k_mutex m;
    struct k_msgq queue;
    char __aligned(4) queue_buffer[CONFIG_PICOXF_EVENT_QUEUE_SIZE_MAX * sizeof(PicoEvent*)];
};

#endif