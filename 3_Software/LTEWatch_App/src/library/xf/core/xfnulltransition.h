/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    xfnulltransition.h
 *
 * \addtogroup XF eXecution Framework
 * \{
 *
 * \class   XFNullTransition
 * \brief   Represents a transition in a state machine having no trigger.
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
#if (XF_USE_MEMORYPOOL_FOR_NULLTRANSITIONS != 0)
   #include "memorypoolmacros.h"
#endif // XF_USE_MEMORYPOOL_FOR_NULLTRANSITIONS

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class XFNullTransition : public IXFEvent
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
     * \param   pBehaviour  Behaviour in which the null transition will be executed.
     ****************************************************************************************************************/
    XFNullTransition(IXFReactive * pBehaviour = NULL);

    virtual ~XFNullTransition();

#if (XF_USE_MEMORYPOOL_FOR_NULLTRANSITIONS != 0)
    DECLARE_MEMORYPOOL_OPERATORS(XFNullTransition)		///< Create custom new and delete operators using memory pool
#endif // XF_USE_MEMORYPOOL_FOR_NULLTRANSITIONS
};

/** \} */
