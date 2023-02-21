/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2021
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    isubject.hpp
 * @class   ISubject
 * @brief   Template defining a `ISubject` class.
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2016-08-18
 *
 * @author  Patrice Rudaz (patrice.rudaz@hevs.ch)
 * @date    July 2018
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <stdint.h>
#include <stdlib.h>
//#include <assert.h>

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* APPLICATION defintion                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#define DEBUG_ISUBJECT_ENABLE               0
#if (DEBUG_ISUBJECT_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #include "tools/logtrace.h"
    #include "tools/config/logtrace-config.h"
    #define ISUBJECT_DEBUG(args...)         LOG_TRACE("ISubject", LOG_LEVEL_DEBUG, args) 
    #define ISUBJECT_ERROR(args...)         LOG_TRACE("ISubject", LOG_LEVEL_ERROR, args) 
    #define ISUBJECT_INFO(args...)          LOG_TRACE("ISubject", LOG_LEVEL_INFO, args) 
#else
    #define ISUBJECT_DEBUG(...)             {(void) (0);}
    #define ISUBJECT_ERROR(...)             {(void) (0);}
    #define ISUBJECT_INFO(...)              {(void) (0);}
#endif
    
#define ISUBJECT_USE_CRITICL_SECTION        0
#if (ISUBJECT_USE_CRITICL_SECTION != 0)
    #include <critical/critical.h>
#endif


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* NAMESPACE Declaration                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
// template <class T, int MAX_OBSERVERS = 12> class ISubject
template <class T, int MAX_OBSERVERS> class ISubject
{
public:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* CONSTRUCTOR SECTION                                                                                          */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    ISubject()
    {
        for(int i = 0; i < MAX_OBSERVERS; i++)
        {
            _observers[i] = NULL;
        }
        _nbRegisteredObservers = 0;
    }
    ~ISubject() {}

    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC DECLARATION SECTION                                                                                   */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
     * @brief   Subscribes an Onbserver to this Subject
     *
     * @param   pObserver   Pointer on the Object that requires notification.
     ****************************************************************************************************************/
    bool addObserver(T* pObserver)
    {
        for(int i = 0; i < MAX_OBSERVERS; i++)
        {
            if (_observers[i] == pObserver)
            {
                ISUBJECT_INFO("Observer 0x%08x already in the list...\r\n", pObserver);
                return true;
            }
            
            if (_observers[i] == NULL)
            {
                _observers[i] = pObserver;
                _nbRegisteredObservers++;
                return true;
            }
        }
        ISUBJECT_ERROR("Observer's list is FULLL ! (size: %d)\r\n", _nbRegisteredObservers);
       // __assert(false);
        return false;
    }


    /************************************************************************************************************//**
     * @brief   Unsubscribes the Observer.
     *
     * @param   pObserver   Pointer on the Object to unsubscribe.
     ************************************************************************************************************/
    void delObserver(T* pObserver)
    {
        T* pToMove = NULL;
        ISUBJECT_DEBUG("Observer 0x%08x to be removed\r\n", pObserver);
        #if (ISUBJECT_USE_CRITICL_SECTION != 0)
        enterCritical();
        #endif
        for(int i = _nbRegisteredObservers; i > 0; i--)
        {
            if (_observers[i - 1] != NULL)
            {
                if (_observers[i - 1] == pObserver)
                {
                    _observers[i - 1] = pToMove;
                    _nbRegisteredObservers--;
                    ISUBJECT_DEBUG("Observer 0x%08x removed at %d\r\n", pObserver, i-1);
                    #if (ISUBJECT_USE_CRITICL_SECTION != 0)
                    exitCritical();
                    #endif
                    return;
                }
                else if (pToMove == NULL)
                {
                    pToMove = _observers[i - 1];
                    _observers[i - 1] = NULL;
                }
            }
        }
        
        if (pToMove != NULL) 
        {
            ISUBJECT_DEBUG("Observer 0x%08x was not in list\r\n", pObserver);
            _observers[_nbRegisteredObservers - 1] = pToMove;
        }
        #if (ISUBJECT_USE_CRITICL_SECTION != 0)
        exitCritical();
        #endif
    }

    
    /************************************************************************************************************//**
     * @brief Returns the number of registered observers
     ****************************************************************************************************************/
    inline uint8_t observersCount() const   { return _nbRegisteredObservers; }

    /************************************************************************************************************//**
     * @brief Returns the pointer of the observer selected by the given index...
     ****************************************************************************************************************/
    inline T* observer(uint8_t index) const { /*__assert(index < observersCount());*/ return _observers[index]; }

protected:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PROTECTED DECLARATION SECTION                                                                                */
    /*                                                                                                              */
    /* ************************************************************************************************************ */

private:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PRIVATE DECLARATION SECTION                                                                                  */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    // Attributes
    T*  _observers[MAX_OBSERVERS];
    uint8_t _nbRegisteredObservers;

    // Methods
};
