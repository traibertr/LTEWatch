#ifndef LTE_ONCE
#define LTE_ONCE

#include <zephyr.h>
#include <modem/lte_lc.h>

#include <isubject.hpp>
#include "ilteobserver.h"

#include "config/lte-config.h"

class LTE : public ISubject<ILTEObserver, CONFIG_LTE_MAX_OBSERVERS>
{
public:
    typedef enum LTESTATE
    {
        ST_INITIAL,
        ST_INITIALIZED,
        ST_CONNECTING,
        ST_CONNECTED,
        ST_DISCONNECTING,
        ST_DISCONNECTED
    } eLteState;

    virtual ~LTE() {};

    void initHW();
    void uninitHW();

    bool connect();
    void disconnect();
    void waitForConnection(k_timeout_t timeout);
    void getConnected();
    void lostConnection();
    void error();

    inline bool isConnected() const     { return state_ == ST_CONNECTED; }
    inline eLteState state() const      { return state_; }

    // Event Callback
    static void lteEventHandler(const struct lte_lc_evt *const evt);

    // Singleton
    static LTE* getInstance();

    static const char* state_to_string(eLteState state);

private:
    // Singleton pattern
    LTE();

    struct lte_lc_conn_eval_params params_;
    struct k_sem lteConnectedSem_;
    eLteState state_;

    void _notifyObservers(eLteState state);
};

#endif // LTE_ONCE
