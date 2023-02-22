/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    ixfreactive.h
 *
 * @addtogroup XF eXecution Framework
 * \{
 *
 * @class   IXFReactive
 * @brief   Interface that needs to be implemented by every class having a behaviour.
 *
 * Interface that needs to be implemented by every class having a behaviour. Interface methods are needed by the 
 * other parts of the XF.
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

#include "ixfevent.h"
#include "xfeventstatus.h"


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class IXFReactive
{
public:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* CONSTRUCTOR SECTION                                                                                          */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    IXFReactive() {}
    virtual ~IXFReactive() {}

    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC DECLARATION SECTION                                                                                   */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    virtual void startBehaviour() = 0;                  ///< @brief Starts the behaviour.
    virtual void pushEvent(IXFEvent* pEvent) = 0;       ///< @brief Injects an event into the class.
    virtual EventStatus process(IXFEvent* pEvent) = 0;  ///< @brief Called by the thread to process an event.
};

/** \} */
