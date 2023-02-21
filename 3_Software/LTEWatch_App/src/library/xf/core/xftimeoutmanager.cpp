/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz & Thomas Sterren
 * Bugs fixes and Zephyr RTOS adaptation by Tristan Traiber 
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "xf-config.h"

#if (XF_TM_PROTECT_ADD_TM != 0)
    #include "critical.h"
    #include <spinlock.h>
    /* Lock to protect the internal state of all work items, work queues,
     * and pending_cancels.*/
    //static struct k_spinlock lock;
#endif // XF_TM_PROTECT_ADD_TM

#include "xftimeoutmanager.h"
#include "ixfreactive.h"
#include "xftimeout.h"
#include <assert.h>

    
/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#define DEBUG_XFTIMEOUTMANAGER_ENABLE       0
#define DEBUG_XFTIMEOUTMANAGER_DEEP_ENABLE  0
#define DEBUG_XFTIMEOUTMANAGER_TM_ENABLE    0

#if ((DEBUG_XFTIMEOUTMANAGER_ENABLE != 0) || (DEBUG_XFTIMEOUTMANAGER_DEEP_ENABLE != 0) || (DEBUG_XFTIMEOUTMANAGER_TM_ENABLE != 0)) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(xf_timeout_manager, CONFIG_XF_TIMEOUT_MANAGER_LOG_LEVEL);
#endif // #if ((DEBUG_XFTIMEOUTMANAGER_ENABLE != 0) || (DEBUG_XFTIMEOUTMANAGER_DEEP_ENABLE != 0) || (DEBUG_XFTIMEOUTMANAGER_TM_ENABLE != 0)) && defined(DEBUG_NRF_USER)

#if (DEBUG_XFTIMEOUTMANAGER_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #define XFTM_DEBUG(args...)             LOG_DBG("XFTimeoutManager : " args) 
#else
    #define XFTM_DEBUG(...)                 {(void) (0);}
#endif

#if (DEBUG_XFTIMEOUTMANAGER_DEEP_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #define XFTMD_DEBUG(args...)            LOG_DBG("XFTimeoutManager : " args) 
#else
    #define XFTMD_DEBUG(...)                {(void) (0);}
#endif

#if (DEBUG_XFTIMEOUTMANAGER_TM_ENABLE != 0) && defined(DEBUG_NRF_USER)
    //#include <critical/critical.h>
    #define XFTMTM_DEBUG(args...)            LOG_DBG("XFTimeoutManager : " args) 
#else
    #define XFTMTM_DEBUG(...)               {(void) (0);}
#endif


/* **************************************************************************************************************** */
/* Declaration section                                                                                              */
/* **************************************************************************************************************** */
// Initialization of static class attributes
XFTimeoutManager* XFTimeoutManager::_pInstance = NULL;

    
/* **************************************************************************************************************** */
/* Constructor | Destructor                                                                                         */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
XFTimeoutManager::XFTimeoutManager()
{
    //ASSERT(!_pInstance);
    _pInstance = this;

    xfTimerInit();
}

// ------------------------------------------------------------------------------------------------------------------
XFTimeoutManager::~XFTimeoutManager()
{
}

/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
XFTimeoutManager* XFTimeoutManager::getInstance()
{
    #if (XF_TM_STATIC_INSTANCE != 0)
        static XFTimeoutManager theXfTimeoutManager;
    #endif // XF_TM_STATIC_INSTANCE

    if (!_pInstance)
    {
        // _pInstance gets already initialized in the constructor. The assignment below gets only done for convenience.
        #if (XF_TM_STATIC_INSTANCE != 0)
            _pInstance = &theXfTimeoutManager;
        #else
            // Create instance on the heap
            _pInstance = new XFTimeoutManager;
        #endif // XF_TM_STATIC_INSTANCE
    }

    return _pInstance;
}

// ------------------------------------------------------------------------------------------------------------------
void XFTimeoutManager::start()
{
    // Nothing to do here
}

// ------------------------------------------------------------------------------------------------------------------
void XFTimeoutManager::scheduleTimeout(uint8_t timeoutId, unsigned int interval, IXFReactive* pReactive)
{
    // Create a new timeout out of the given parameters
    XFTimeout* pTimeout = new XFTimeout(timeoutId, interval, pReactive);
    if (pTimeout)
    {
        XFTM_DEBUG("Timeout #%d added successfully (interval: %d) ...", timeoutId, interval);
        addTimeout(pTimeout);
    }
    else
    {
        XFTM_DEBUG("%s","XFTimeout instance is NULL !");
        //ASSERT(false);
    }
}

// ------------------------------------------------------------------------------------------------------------------
void XFTimeoutManager::unscheduleTimeout(uint8_t timeoutId, IXFReactive* pReactive)
{
    // Remove timeout from list
    XFTM_DEBUG("id: %d", timeoutId);
    removeTimeouts(timeoutId, pReactive);
}

// ------------------------------------------------------------------------------------------------------------------
void XFTimeoutManager::timeout()
{
    // Checks if the timeout's list is empty
    if (_timeouts.empty())
    {
        return;
    }
    
    // Get first timeout and inject the timeout back to the behavioral class
    #if (XF_TM_USE_ISLIST != 0)
        XFTimeout* pTimeout = _timeouts.front();
    #else
        XFTimeout* pTimeout = (*_timeouts.begin());
    #endif

    // Inject the timeout back to the behavioral class
    XFTMD_DEBUG("Timeout #%d pushed back (queue size: %u)", pTimeout->getId(), _timeouts.size());
    returnTimeout(pTimeout);
    const unsigned int minGapOverflow = pTimeout->_minGapOverflow;

    // Remove timeout
    _timeouts.pop_front();

    // If there are still timeouts waiting in the timeout list, a new app timer has to be started
    if (!_timeouts.empty())
    {
        #if (XF_TM_USE_ISLIST != 0)
            pTimeout = _timeouts.front();
        #else
            pTimeout = (*_timeouts.begin());
        #endif

        pTimeout->decrRelativeTicks(minGapOverflow);
                
        XFTMD_DEBUG("start XF timer with ticks: %lu", pTimeout->_relTicks);
        xfTimerStart(pTimeout->_relTicks);
    }

    XFTM_DEBUG("_timeouts.size: %d", _timeouts.size());
}


/* **************************************************************************************************************** */
/* PROTECTED SECTION                                                                                                */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void XFTimeoutManager::addTimeout(XFTimeout* pNewTimeout)
{
    if (!_timeouts.empty())
    {
        #if (XF_TM_PROTECT_ADD_TM != 0)
            enterCritical();
            //k_spinlock_key_t key = k_spin_lock(&lock);
        #endif // XF_TM_PROTECT_ADD_TM

        #if (XF_TM_USE_ISLIST != 0)
            TimeoutList::Item* pItem    = _timeouts.getFirst();
            XFTimeout* pTimeout         = pItem->concept;
            //XFTimeout* pLastTimeout     = _timeouts.getEnd()->concept;
            XFTimeout* pLastTimeout     = _timeouts.getLastConcept();
            //XFTimeout* pLastTimeout     = _timeouts.getEnd()->concept;
        #else
            TimeoutList::iterator i     = _timeouts.begin();
            XFTimeout* pTimeout         = (*i);
            XFTimeout* pLastTimeout     = (*(_timeouts.end()));
        #endif  // XF_TM_PROTECT_ADD_TM
     
        #if (XF_TM_PROTECT_ADD_TM != 0)
            //k_spin_unlock(&lock, key);
            exitCritical();
        #endif // XF_TM_PROTECT_ADD_TM
       
        if (pTimeout != NULL)
        {
            // Decrement remaining ticks by elapsed value of timer
            pTimeout->decrRelativeTicks(xfTimerGetElapsedTimerTicks());
            XFTMTM_DEBUG("Item->UpdateTicks: %lu; newTimeout->ticks: %lu", pTimeout->_relTicks, pNewTimeout->_relTicks);

            // Check if new timeout is shorter than actually running one
            if (pNewTimeout->_relTicks <= pTimeout->_relTicks)
            {
                #if (XF_TM_PROTECT_ADD_TM != 0)
                    enterCritical();
                    //k_spinlock_key_t key = k_spin_lock(&lock);
                #endif // XF_TM_PROTECT_ADD_TM

                // A new timeout at the beginning
                _timeouts.push_front(pNewTimeout);

                #if (XF_TM_PROTECT_ADD_TM != 0)
                    //k_spin_unlock(&lock, key);
                    exitCritical();
                #endif // XF_TM_PROTECT_ADD_TM
            
                // Decrement remaining ticks by elapsed value of timer
                pTimeout->decrRelativeTicks(pNewTimeout->_relTicks + pNewTimeout->_minGapOverflow);

                // Restart the XF's timer with new timeout's duration
                XFTMTM_DEBUG("FRONT; Item->UpdateTicks: %lu; newTimeout->ticks: %lu", pTimeout->_relTicks, pNewTimeout->_relTicks);
                xfTimerRestart(pNewTimeout->_relTicks);
            }
            else
            {
                unsigned int index = 0;
                do
                {
                    // Remove time from new timeout
                    pNewTimeout->decrRelativeTicks(pTimeout->_relTicks);
                    XFTMTM_DEBUG("LOOP; Item->UpdateTicks: %lu; newTimeout->ticks: %lu", pNewTimeout->_relTicks, pNewTimeout->_relTicks);
                    
                    // Get next item
                    #if (XF_TM_USE_ISLIST != 0)
                            pItem       = pItem->getNext(); 
                            pTimeout    = (pItem != NULL) ? pItem->concept : NULL;
                    #else
                        i++;
                        pTimeout    = (*i);
                    #endif  // XF_TM_USE_ISLIST
                    index++;    
                }
                while((pTimeout != NULL) && (pTimeout != pLastTimeout) && (pTimeout->_relTicks < pNewTimeout->_relTicks));

                #if (XF_TM_PROTECT_ADD_TM != 0)
                    enterCritical();
                    //k_spinlock_key_t key = k_spin_lock(&lock);
                #endif // XF_TM_PROTECT_ADD_TM

                // Insert new timeout before
                #if (XF_TM_USE_ISLIST != 0)
                    _timeouts.addAt(index, pNewTimeout);
                    XFTMTM_DEBUG("ADD AT: %d; Item added: 0x%08lx", index, pNewTimeout);
                #else
                    _timeouts.insert(i, pNewTimeout);
                    XFTMTM_DEBUG("Item added: 0x%08lx, at position: %d", pNewTimeout, i);
                #endif  // XF_TM_USE_ISLIST

                #if (XF_TM_PROTECT_ADD_TM != 0)
                    //k_spin_unlock(&lock, key);
                    exitCritical();
                #endif // XF_TM_PROTECT_ADD_TM
                
                if (_timeouts.size() > index + 1)
                {
                    // Remove time from following timeout
                    pTimeout->decrRelativeTicks(pNewTimeout->_relTicks - pNewTimeout->_minGapOverflow);
                }
            }
        }
    }
    else
    {
        XFTMD_DEBUG("%s","Empty list of Timeouts ...");
        
        #if (XF_TM_PROTECT_ADD_TM != 0)
            enterCritical();
            //k_spinlock_key_t key = k_spin_lock(&lock);
        #endif // XF_TM_PROTECT_ADD_TM

        // Insert timeout in the list
        _timeouts.push_front(pNewTimeout);
        XFTMD_DEBUG("Item added: 0x%08lx", pNewTimeout);

        #if (XF_TM_PROTECT_ADD_TM != 0)
            //k_spin_unlock(&lock, key);
            exitCritical();
        #endif // XF_TM_PROTECT_ADD_TM

        // Starts the XF's timer again
        xfTimerStart(pNewTimeout->_relTicks);
    }

    XFTM_DEBUG("_timeouts.size: %d", _timeouts.size());
}

// ------------------------------------------------------------------------------------------------------------------
// TODO: There is a bug here when a very tiny timeout is removed ! The
void XFTimeoutManager::removeTimeouts(uint8_t timeoutId, IXFReactive* pReactive)
{
    XFTimeout* pTimeout;
    XFTMTM_DEBUG("| (timeoutId: %d; size: %u)", timeoutId, _timeouts.size());

    //k_spinlock_key_t key = k_spin_lock(&lock);

    #if (XF_TM_USE_ISLIST != 0)
    for(TimeoutList::Item* pItem = _timeouts.getFirst(); pItem != _timeouts.getEnd(); pItem = pItem->getNext())
    {
		pTimeout = pItem->concept;
        XFTMTM_DEBUG("+- (pTimeout: 0x%08x)", pItem->concept);
    #else
    for(TimeoutList::iterator i  = _timeouts.begin(); i != _timeouts.end(); i++)
    {
        pTimeout = *i;
        XFTMTM_DEBUG("+- (pTimeout: 0x%08x)", pItem->concept);
    #endif  // XF_TM_USE_ISLIST
    
        // Check if behavior and timeout id are equal
        if (pTimeout->getId() == timeoutId && pTimeout->getBehaviour() == pReactive)
        {
            XFTMTM_DEBUG(" +-> pTimeout->getId() == timeout.getId() (%d == %d)", pTimeout->getId());

            // Only affect other elements in list if there are any after this one
            // Get iterator of next element in list
            #if (XF_TM_USE_ISLIST != 0)
                XFTimeout* pNextTimeout = (pItem->getNext())->concept;
            #else
                TimeoutList::iterator next = i;
                XFTimeout* pNextTimeout = *(++next);
            #endif
            if (pNextTimeout)
            {
                XFTMTM_DEBUG(" +-> pNextTimeout: %d", pNextTimeout->_relTicks);
                // Check if remaining ticks can be given further
                #if (XF_TM_USE_ISLIST != 0)
                if (pNextTimeout != _timeouts.getEnd()->concept)
                #else
                if (pTimeout != (*_timeouts.end()))
                #endif
                {
                    // Add (remaining) ticks to next timeout in list
                    pNextTimeout->incrRelativeTicks(pTimeout->_relTicks + pTimeout->_minGapOverflow - xfTimerGetElapsedTime());
                }
            }
            else 
            {
                xfTimerStop();
            }

            #if (XF_TM_PROTECT_ADD_TM != 0)
                enterCritical();
            #endif // XF_TM_PROTECT_ADD_TM

            // Erase element and delete pointer
            #if (XF_TM_USE_ISLIST != 0)
                XFTMTM_DEBUG(" +-> size: %d", _timeouts.size());
                _timeouts.removeItem(pItem);
                XFTMTM_DEBUG(" +-> size: %d", _timeouts.size());
            #else
                i = _timeouts.erase(i);
            #endif
            XFTMTM_DEBUG(" \\-> Item removed: id #%d", pTimeout->getId());

            #if (XF_TM_PROTECT_ADD_TM != 0)
                exitCritical();
            #endif // XF_TM_PROTECT_ADD_TM

            delete pTimeout;
            break;
        }
        else 
        {
            XFTMTM_DEBUG("%s"," \\-> *pTimeout != timeout");
        }
    }
    //k_spin_unlock(&lock, key);
    XFTMTM_DEBUG("|");
    XFTM_DEBUG("+- _timeouts.size: %d", _timeouts.size());
}

// ------------------------------------------------------------------------------------------------------------------
void XFTimeoutManager::returnTimeout(XFTimeout* pTimeout)
{
    pTimeout->getBehaviour()->pushEvent(pTimeout);
}


/* **************************************************************************************************************** */
/* PRIVATE SECTION                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void XFTimeoutManager::_startXfTimer()
{
    if (!_timeouts.empty())
    {
        #if (XF_TM_USE_ISLIST != 0)
            unsigned int ticks  = _timeouts.getFirst()->concept->_relTicks;
        #else
            uint32_t ticks  = (*_timeouts.begin())->_relTicks;
        #endif

        XFTMTM_DEBUG("Timer's tick: %lu", ticks);
        xfTimerStart(ticks);
    }
}
