/***************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Michael Clausen, Thierry Hischier, Patrice Rudaz
 * All rights reserved.
 *
 * Initial author: Michael Clausen, Thierry Hischier
 * Creation date: 2017-11-20
 *
 * @file    functionprovider.h
 *
 * @addtogroup Function
 * \{
 *
 * @class   FunctionProvider
 * @brief   Interface for the new function provider
 *
 * # FunctionProvider
 *
 * @author  Patrice Rudaz (patrice.rudaz@hevs.ch)
 * @date    February 2022
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <platform-config.h>

#include "functionprovidercontext.h"


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* APPLICATION defintion                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */


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
class FunctionProvider
{
    public:
        /* ******************************************************************************************************** */
        /* CONSTRUCTOR SECTION                                                                                      */
        /* ******************************************************************************************************** */
        FunctionProvider() {}
        virtual ~FunctionProvider() {}

        /* ******************************************************************************************************** */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /* ******************************************************************************************************** */
        typedef int FunctionId;

        virtual void handle(FunctionId id, const FunctionProviderContext& context) = 0;
};

/** \} */   // Group: Function
