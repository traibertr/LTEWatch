/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics.
 * Created by Patrice Rudaz
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "lte.h"

#include <string.h>
#include <sdktools.h>

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#ifndef CONFIG_LTE_LOG_LEVEL
    #define CONFIG_LTE_LOG_LEVEL            4
#endif

#if (CONFIG_LTE_LOG_LEVEL != 0)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(lte, CONFIG_LTE_LOG_LEVEL);

    #define LTE_DEBUG(args...)              LOG_DBG(args)
    #define LTE_ERROR(args...)              LOG_ERR(args)
    #define LTE_INFO(args...)               LOG_INF(args)
    #define LTE_WRN(args...)                LOG_WRN(args)
#else
    #define LTE_DEBUG(...)                  {(void) (0);}
    #define LTE_ERROR(...)                  {(void) (0);}
    #define LTE_INFO(...)                   {(void) (0);}
    #define LTE_WRN(...)                    {(void) (0);}
#endif
/* **************************************************************************************************************** */
/* Namespace Declaration                                                                                            */
/* **************************************************************************************************************** */


/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
LTE::LTE() : state_(ST_INITIAL)
{
    memset(&params_, 0x00, sizeof(lte_lc_conn_eval_params));
    k_sem_init(&lteConnectedSem_, 0, 1);
}

// ------------------------------------------------------------------------------------------------------------------
void LTE::_notifyObservers(LTE::eLteState state)
{
    for (uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != nullptr)
        {
            switch (state)
            {
                case LTE::ST_CONNECTED:
                {
                    observer(i)->onConnected(this);
                    break;
                }
                case LTE::ST_INITIAL:
                case LTE::ST_INITIALIZED:
                {
                    observer(i)->onDisconnected(this);
                    break;
                }
                case LTE::ST_DISCONNECTING:
                {
                    observer(i)->onDisconnected(this);
                    break;
                }
                case LTE::ST_DISCONNECTED:
                {
                    observer(i)->onDisconnected(this);
                    break;
                }
                default:
                    break;
            }
        }
    }
}


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void LTE::initHW()
{
    int err;

    err = lte_lc_init();
    if (err) {
        LTE_ERROR("lte_lc_init, error: %d", err);
        return;
    }

    err = lte_lc_psm_req(true);
    if (err) {
        LTE_ERROR("lte_lc_psm_req, error: %d", err);
        return;
    }

    err = lte_lc_edrx_req(true);
    if (err) {
        LTE_ERROR("lte_lc_edrx_req, error: %d", err);
        return;
    }

    LTE_DEBUG("LTE initialization succeeded!");
    state_ = ST_INITIALIZED;
}

// ------------------------------------------------------------------------------------------------------------------
void LTE::uninitHW()
{
    int err = lte_lc_deinit();
    if (err) {
        LTE_ERROR("lte_lc_deinit, error: %d", err);
        return;
    }
    state_ = ST_INITIAL;
}

// ------------------------------------------------------------------------------------------------------------------
bool LTE::connect()
{
    if (state_ == ST_INITIAL) {
        LTE_DEBUG("Initializing HW first ...");
        initHW();
    }

    if (state_ == ST_INITIALIZED) {
        int err = lte_lc_connect_async(LTE::lteEventHandler);
        if (err) {
            LTE_ERROR("Connecting to LTE network failed, error: %d", err);
            return false;
        }
        state_ = ST_CONNECTING;
        return true;
    } else {
        LTE_WRN("Invalid State (state: %d)", state_);
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------------------
void LTE::disconnect()
{
    if (state_ > ST_INITIALIZED) {
        int err = lte_lc_func_mode_set(LTE_LC_FUNC_MODE_DEACTIVATE_LTE);
        if (err) {
            LTE_ERROR("lte_lc_func_mode_set(LTE_LC_FUNC_MODE_DEACTIVATE_LTE) failed, error: %d", err);
        } else {
            state_ = ST_DISCONNECTING;
        }
    }
    _notifyObservers(state_);
}

// ------------------------------------------------------------------------------------------------------------------
void LTE::lostConnection()
{
    LTE_DEBUG("> state_ = %s", state_to_string(state_));
    state_ = ST_DISCONNECTED;
    uninitHW();
    LTE_DEBUG("< state_ = %s", state_to_string(state_));
    _notifyObservers(state_);
}

// ------------------------------------------------------------------------------------------------------------------
void LTE::error()
{
    LTE_ERROR("LTE on ERROR!");
    k_sem_give(&lteConnectedSem_);
    int err = lte_lc_func_mode_set(LTE_LC_FUNC_MODE_DEACTIVATE_LTE);
    if (err) {
        LTE_ERROR("lte_lc_func_mode_set(LTE_LC_FUNC_MODE_DEACTIVATE_LTE) failed, error: %d", err);
    }
    uninitHW();

    for (uint8_t i = 0; i < observersCount(); i++)
    {
        if (observer(i) != nullptr)
        {
            observer(i)->onError(this);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
void LTE::waitForConnection(k_timeout_t timeout)
{
    k_sem_take(&lteConnectedSem_, timeout);
}

// ------------------------------------------------------------------------------------------------------------------
void LTE::getConnected()
{
    k_sem_give(&lteConnectedSem_);
    state_ = ST_CONNECTED;
    _notifyObservers(state_);
}

// ------------------------------------------------------------------------------------------------------------------
// static
const char* LTE::state_to_string(eLteState state)
{
    switch (state)
    {
        case ST_CONNECTED:      return "ST_CONNECTED";
        case ST_CONNECTING:     return "ST_CONNECTING";
        case ST_DISCONNECTED:   return "ST_DISCONNECTED";
        case ST_DISCONNECTING:  return "ST_DISCONNECTING";
        case ST_INITIAL:        return "ST_INITIAL";
        case ST_INITIALIZED:    return "ST_INITIALIZED";
        default :               return "Unknown state";
    }
}


// ------------------------------------------------------------------------------------------------------------------
// Event Callback
void LTE::lteEventHandler(const struct lte_lc_evt *const evt)
{
    switch (evt->type) {
    case LTE_LC_EVT_NW_REG_STATUS:
    {
        LTE_DEBUG("LTE_LC_EVT_NW_REG_STATUS: %d", evt->nw_reg_status);
        if (evt->nw_reg_status == LTE_LC_NW_REG_UICC_FAIL)
        {
            LTE::getInstance()->error();
            break;
        }

        if ((evt->nw_reg_status != LTE_LC_NW_REG_REGISTERED_HOME) &&
            (evt->nw_reg_status != LTE_LC_NW_REG_REGISTERED_ROAMING)) {
            break;
        }

        LTE_INFO("Connected to: %s network", evt->nw_reg_status == LTE_LC_NW_REG_REGISTERED_HOME ? "home" : "roaming");
        LTE::getInstance()->getConnected();
        break;
    }
    case LTE_LC_EVT_PSM_UPDATE:
    {
        LTE_DEBUG("LTE_LC_EVT_PSM_UPDATE: tau %d, active time: %d", evt->psm_cfg.tau, evt->psm_cfg.active_time);
        break;
    }
    case LTE_LC_EVT_EDRX_UPDATE:
    {
        LTE_DEBUG("LTE_LC_EVT_EDRX_UPDATE: mode=%d, edrx=%s [s], window=%s [s]",
                evt->edrx_cfg.mode, tools::SDKTools::debugPrintfFloat(evt->edrx_cfg.edrx), tools::SDKTools::debugPrintfFloat(evt->edrx_cfg.ptw));
        break;
    }
    case LTE_LC_EVT_RRC_UPDATE:
    {
        LTE_DEBUG("LTE_LC_EVT_RRC_UPDATE: mode = %s", evt->rrc_mode == LTE_LC_RRC_MODE_IDLE ? "Idle":"Connected");
        break;
    }
    case LTE_LC_EVT_CELL_UPDATE:
    {
        LTE_DEBUG("LTE_LC_EVT_CELL_UPDATE: tac %d, cell ID %d", evt->cell.tac, evt->cell.id);
        break;
    }
    case LTE_LC_EVT_LTE_MODE_UPDATE:
    {
        switch (evt->lte_mode) {
            case LTE_LC_LTE_MODE_NONE:  LTE_DEBUG("LTE_LC_EVT_LTE_MODE_UPDATE: mode = LTE_LC_LTE_MODE_NONE");  break;
            case LTE_LC_LTE_MODE_LTEM:  LTE_DEBUG("LTE_LC_EVT_LTE_MODE_UPDATE: mode = LTE_LC_LTE_MODE_LTEM");  break;
            case LTE_LC_LTE_MODE_NBIOT: LTE_DEBUG("LTE_LC_EVT_LTE_MODE_UPDATE: mode = LTE_LC_LTE_MODE_NBIOT"); break;
            default: LTE_WRN("Unhandled mode here");
        }

        if (evt->lte_mode == LTE_LC_LTE_MODE_NONE) {
            LTE_INFO("LTE disconnected !");
            LTE::getInstance()->lostConnection();
        }
        break;
    }
    case LTE_LC_EVT_TAU_PRE_WARNING:
    {
        LTE_DEBUG("LTE_LC_EVT_TAU_PRE_WARNING");
        break;
    }
    case LTE_LC_EVT_NEIGHBOR_CELL_MEAS:
    {
        LTE_DEBUG("LTE_LC_EVT_NEIGHBOR_CELL_MEAS");
        break;
    }
    case LTE_LC_EVT_MODEM_SLEEP_EXIT_PRE_WARNING:
    {
        LTE_DEBUG("LTE_LC_EVT_MODEM_SLEEP_EXIT_PRE_WARNING");
        break;
    }
    case LTE_LC_EVT_MODEM_SLEEP_EXIT:
    {
        LTE_DEBUG("LTE_LC_EVT_MODEM_SLEEP_EXIT");
        break;
    }
    case LTE_LC_EVT_MODEM_SLEEP_ENTER:
        /* Callback events carrying LTE link data */
        LTE_DEBUG("LTE_LC_EVT_MODEM_SLEEP_ENTER");
        break;
    default:
        break;
    }
}

LTE* LTE::getInstance()
{
    static LTE instance;
    return &instance;
}


