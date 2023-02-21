/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    xfevent.h
 *
 * \addtogroup XF eXecution Framework
 * \{
 *
 * \class   XFEvent
 * \brief   Base class for all events.
 *
 * Base class for all events. When intending to signal an event, derive a new class from this class.
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
#include "xf.h"
#include "ixfevent.h"
#if (XF_USE_MEMORYPOOL_FOR_DEFAULTEVENTS != 0)
    #include "memorypoolmacros.h"
#endif // XF_USE_MEMORYPOOL_FOR_DEFAULTEVENTS

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class XFEvent : public IXFEvent
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
     * \param   id          Identifier uniquely identifying the event in the context of the behaviour.
     * \param   pBehaviour  The behaviour for which the event was constructed (will consume the event).
     ****************************************************************************************************************/
    XFEvent( uint8_t id, IXFReactive * pBehaviour = NULL, bool deleteMe = true );
    virtual ~XFEvent();

#if (XF_USE_MEMORYPOOL_FOR_DEFAULTEVENTS != 0)
    DECLARE_MEMORYPOOL_OPERATORS(XFEvent)		///< Create custom new and delete operators using memory pool
#endif // XF_USE_MEMORYPOOL_FOR_DEFAULTEVENTS
};

/** \} */
