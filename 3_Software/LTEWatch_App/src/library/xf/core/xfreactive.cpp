/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz & Thomas Sterren
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "xf.h"
#include "xfreactive.h"
#include "xftimeout.h"
#include "initialevent.h"
#include "memorypoolmacros.h"


/* **************************************************************************************************************** */
/* CONSTRUCTOR                                                                                                      */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
XFReactive::XFReactive(XFThread* pThread) 
    : _pThread((pThread) ? pThread : XF::getMainThread())
{}

// ------------------------------------------------------------------------------------------------------------------
XFReactive::~XFReactive()
{}


/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void XFReactive::startBehaviour()
{
    pushEvent(new InitialEvent());
}

#if (XF_CRITICAL_QUEUE != 0)
// ------------------------------------------------------------------------------------------------------------------
void XFReactive::pushCriticalEvent(IXFEvent* pEvent)
{
    if (pEvent)
    {
        // Set owner
        pEvent->setBehaviour(this);
        
        // Push to threads event queue
        getThread()->pushCriticalEvent(pEvent);
    }
}
#endif  // XF_CRITICAL_QUEUE

// ------------------------------------------------------------------------------------------------------------------
void XFReactive::pushEvent(IXFEvent* pEvent)
{
    if (pEvent)
    {
        // Set owner
        pEvent->setBehaviour(this);
        
        // Push to threads event queue
        getThread()->pushEvent(pEvent);
    }
}

// ------------------------------------------------------------------------------------------------------------------
EventStatus XFReactive::process(IXFEvent* pEvent)
{
    _pCurrentEvent          = pEvent;
    EventStatus eventStatus = processEvent();
    _pCurrentEvent          = NULL;
    return eventStatus;
}

// ------------------------------------------------------------------------------------------------------------------
EventStatus XFReactive::processEvent()
{
    // TODO: You should really implement this function in the derived class!
    return EventStatus::Unknown;
}

// ------------------------------------------------------------------------------------------------------------------
XFThread* XFReactive::getThread()
{
    return _pThread;
}

// ------------------------------------------------------------------------------------------------------------------
XFTimeout* XFReactive::getCurrentTimeout()
{
    if (getCurrentEvent() && getCurrentEvent()->getEventType() == IXFEvent::Timeout)
    {
        return (XFTimeout*) getCurrentEvent();
    }
    else
    {
        return NULL;
    }
}
