/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    gpioobserver.h
 *
 * @addtogroup GPIO
 * \{
 *
 * @class   GpioObserver
 * @brief   Interface definition dedicated to the GPIO Object...
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2016-01-09
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

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* NAMESPACE Declaration                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
namespace gpio 
{

    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* Class Declaration                                                                                            */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    class Gpio;
    class GpioObserver
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * \name    GpioObservers 
         * @brief   Callback methods for the GPIO's event
         * \{
         ************************************************************************************************************/
        // ----------------------------------------------------------------------------------------------------------
        virtual void onFallingEdge(Gpio* gpio) = 0;

        // ----------------------------------------------------------------------------------------------------------
        virtual void onRisingEdge(Gpio* gpio) = 0;
        /** \} */
    };

}   // namespace gpio

/** \} */   // Group: GPIO