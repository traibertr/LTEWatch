/***************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Michael Clausen, Thierry Hischier, Patrice Rudaz
 * All rights reserved.
 *
 * Initial author: Michael Clausen, Thierry Hischier
 * Creation date: 2017-11-20
 *
 * @file    functioncall.h
 *
 * @addtogroup Function
 * \{
 *
 * @class   FunctionCall
 * @brief   Interface for the new function call
 *
 * # FunctionCall
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

#include "functionprovider.h"


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
class FunctionCall
{
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* CONSTRUCTOR SECTION                                                                                      */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        FunctionCall(): _subject(NULL), _id(0){}
        FunctionCall(FunctionProvider* subject, FunctionProvider::FunctionId id): _subject(subject), _id(id){}
        FunctionCall(FunctionProvider& subject, FunctionProvider::FunctionId id): _subject(&subject), _id(id){}

        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        inline void operator()(const FunctionProviderContext & context) const 
        {
            if (_subject)
            {
                _subject->handle(_id, context);
            }
        }
        
        inline bool isValid() const {
            return _subject;
        }

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        FunctionProvider* _subject;
        FunctionProvider::FunctionId _id;
    };

/** \} */   // Group: Function
