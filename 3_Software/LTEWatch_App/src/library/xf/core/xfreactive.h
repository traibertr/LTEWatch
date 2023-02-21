/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    xfreactive.h
 *
 * \addtogroup XF eXecution Framework
 * \{
 *
 * \class   XFReactive
 * \brief   Reactive class implementing a behaviour.
 *
 * Reactive class implementing a behaviour. This class can be used to implement a state machine behaviour or an 
 * activity behaviour.
 *
 * Override the processEvent() operation in the inherited class to implement your state machine behaviour.
 *
 * Initial author: Thomas Sterren
 * Creation date: 2010-11-26
 *
 * \author  Patrice Rudaz
 * \date    July 2018
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include "xf-config.h"
#include "ixfreactive.h"
#include "xfthread.h"
#include "xfeventstatus.h"
#include "xftimeout.h"
#include "xfnulltransition.h"


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class XFReactive : public IXFReactive
{
public:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* CONSTRUCTOR SECTION                                                                                          */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
     * \brief   Constructor
     *
     * \param   pThread     Reference to thread who will process the behavioural part of this reactive class.
     ****************************************************************************************************************/
    XFReactive(XFThread * pThread = NULL);
    virtual ~XFReactive();


    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC DECLARATION SECTION                                                                                   */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    #if (XF_CRITICAL_QUEUE != 0)
        #define CRITICAL(event) pushCriticalEvent(new event);
    #endif  // XF_CRITICAL_QUEUE
    #define GEN(event) pushEvent(new event);

    /************************************************************************************************************//**
     * \brief   Starts the behaviour, resp. the state machine.
     ****************************************************************************************************************/
    virtual void startBehaviour();

    #if (XF_CRITICAL_QUEUE != 0)
        /********************************************************************************************************//**
         * \brief   Push the event in the critical queue.
         *
         * Push the event in the critical queue. The events contained in this queue will be handled prioritary (FIFO - 
         * first in, first out).
         *
         * \param	pEvent	The event to be pushed in the queue
         ************************************************************************************************************/
        virtual void pushCriticalEvent(IXFEvent * pEvent);
    #endif  // XF_CRITICAL_QUEUE

    /************************************************************************************************************//**
     * \brief   Push the event in the standard queue.
     *
     * Push the event in the standard queue. The events contained in this queue will be handled as soon as the 
     * critical queue is empty. If the critical queue won't be empty, one event of the standard queue will be handled 
     * for each `priorityLevelerThreshold` critical events.
     *
     * \param	pEvent	The event to be pushed in the queue
     *
     * \see 	priorityLevelerThreshold
     ****************************************************************************************************************/
    virtual void pushEvent(IXFEvent * pEvent);

protected:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PROTECTED DECLARATION SECTION                                                                                */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    XFThread*   _pThread;                    ///< \brief Reference to Thread executing this behaviour.
    IXFEvent*   _pCurrentEvent;              ///< \brief Reference to actually processed event.

    /************************************************************************************************************//**
     * \brief   Executes the behaviour of an event.
     *
     * Executes the current event in its implemented behaviour. This method needs to be overridden to implement the 
     * behaviour (i.e. state machine) needed.
     ****************************************************************************************************************/
    virtual EventStatus processEvent();

    /************************************************************************************************************//**
     * \brief   Returns reference to #_pThread.
     ****************************************************************************************************************/
    virtual XFThread* getThread();

    /************************************************************************************************************//**
     * \brief   Returns the current event to be processed in processEvent().
     ****************************************************************************************************************/
    IXFEvent* getCurrentEvent() const      { return _pCurrentEvent; }

    /************************************************************************************************************//**
     * \brief   Returns a reference to the actually processed time-out.
     *
     * \warning Will work only if the current event is of type IXFEvent::Timeout.
     ****************************************************************************************************************/
    XFTimeout* getCurrentTimeout();

private:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PRIVATE DECLARATION SECTION                                                                                  */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /****************************************************************************************************************
     * \brief   Processes the given event.
     *
     * This may change the state of the class according to its implemented behaviour.
     *
     * \warning Must only be called by the associated thread (see #_pThread).
     *
     * \param   pEvent The event to process.
     ****************************************************************************************************************/
    virtual EventStatus process(IXFEvent* pEvent);
};

/** \} */
