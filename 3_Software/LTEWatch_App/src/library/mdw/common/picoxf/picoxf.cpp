#include "picoxf.h"

PicoXF PicoXF::thePicoXF; /**< Singleton instance of the PicoXF >**/

/**
 * @brief Create an PicoXF object
 * 
 */
PicoXF::PicoXF(/* args */) {

}

/**
 * @brief Destroy the PicoXF object
 * 
 */
PicoXF::~PicoXF() {
    
}

/**
 * @brief Initialize the message queue
 * 
 */
void PicoXF::init() {
    k_msgq_init(&queue, queue_buffer, sizeof(PicoEvent*), CONFIG_PICOXF_EVENT_QUEUE_SIZE_MAX);
    // k_mutex_init(&m);
}

/**
 * @brief Get the PicoXF instance
 * 
 * @return PicoXF* 
 */
PicoXF* PicoXF::getInstance() {
    return &thePicoXF;
}

/**
 * @brief Push event into the queue or intiate timer
 * 
 * @param e 
 */
void PicoXF::pushEvent(PicoEvent* e) {
    // Check if Timeout is set
    if (e->getDelay() > 0)
    {
        scheduleTM(e);
    }else
    {
        k_msgq_put(&queue,&e,K_NO_WAIT);
    }
}

/**
 * @brief process event and delete it if needed
 * 
 */
void PicoXF::execute() {
    PicoIReactive* t;
    t = nullptr;

    if (k_msgq_num_used_get(&queue) > 0)
    {
        // Get the oldest PicoEvent in the Queue
        PicoEvent* e = popEvent();
        if (e)
        {
            t = e->getTarget();
            // Check if Target is set
            if (t)
            {
                // Send the event to our target
                t->processEvent(e);
            }
            if (e->getDnd() == false)
            {
                // Delete the event if needed
                k_free(e);
            }
        }
    }
}

/**
 * @brief Function called on a timeout -> push event into queue after timer runs out
 * 
 * @param t 
 */
void PicoXF::onTimeout(struct k_timer* t) {
    PicoEvent* e;
    e = (PicoEvent*) k_timer_user_data_get(t);
    e->setDelay(0);
    PicoXF::getInstance()->pushEvent(e);
    k_free(t);
    e->setT(NULL);
}

/**
 * @brief Stop the set timer
 * 
 * @param t 
 */
void PicoXF::onStop(struct k_timer* t) {
    PicoEvent* e;
    e = (PicoEvent*) k_timer_user_data_get(t);
    if (e->getDnd() == false)
    {
        k_free(e);
    }
    k_free(t);
    e->setT(NULL);
    
}

/**
 * @brief Schedule a timeout
 * 
 * @param e 
 */
void PicoXF::scheduleTM(PicoEvent* e) {
    struct k_timer* t;
    t = (struct k_timer*) k_malloc(sizeof(struct k_timer));
    e->setT(t);
    k_timer_init(t, &PicoXF::onTimeout, &PicoXF::onStop);
    k_timer_user_data_set(t,e);
    k_timer_start(t,K_MSEC(e->getDelay()), K_MSEC(0));
}

/**
 * @brief Unschedule the timeout
 * 
 * @param e 
 */
void PicoXF::unscheduleTM(PicoEvent* e) {
    struct k_timer* t;
    t = e->getT();
    if (t)
    {
        k_timer_stop(t);
    }
}

/**
 * @brief Remove event from queue
 * 
 * @return PicoEvent* 
 */
PicoEvent* PicoXF::popEvent() {
    PicoEvent* e = nullptr;
    k_msgq_get(&queue,&e,K_NO_WAIT);
    return e;
}


