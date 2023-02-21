/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    initialevent.h
 *
 * \addtogroup XF eXecution Framework
 * \{
 *
 * \class   InitialEvent
 * \brief   Represents the first transition executed in a state machine.
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
#if (XF_USE_MEMORYPOOL_FOR_INITIALEVENTS != 0)
   #include "memorypoolmacros.h"
#endif // XF_USE_MEMORYPOOL_FOR_INITIALEVENTS

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class InitialEvent : public IXFEvent
{
public:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* CONSTRUCTOR SECTION                                                                                          */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    InitialEvent();
    virtual ~InitialEvent();

#if (XF_USE_MEMORYPOOL_FOR_INITIALEVENTS != 0)
    DECLARE_MEMORYPOOL_OPERATORS(InitialEvent)
#endif // XF_USE_MEMORYPOOL_FOR_INITIALEVENTS
};

/** \} */
