/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * \file    ilteobserver.h
 *
 * \addtogroup com
 * \{
 *
 * \addtogroup LTE
 * \{
 *
 * \class   ILTEObserver
 * \brief   Interface definition dedicated to the LTE Object...
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2022-10-06
 *
 * \author  Patrice Rudaz (patrice.rudaz@hevs.ch)
 * \date    October 2022
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


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class GnssController;
class IGnssObserver
{
public:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC DECLARATION SECTION                                                                                   */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
     * \name    LTEObservers 
     * \brief   Callback methods for the LTE's event
     * \{
     ****************************************************************************************************************/
    // --------------------------------------------------------------------------------------------------------------
    virtual void onReceive(GnssController* gnss) = 0;

    // --------------------------------------------------------------------------------------------------------------
    virtual void onConnected(GnssController* gnss) = 0;

    // --------------------------------------------------------------------------------------------------------------
    virtual void onDisconnected(GnssController* gnss) = 0;

    // --------------------------------------------------------------------------------------------------------------
    virtual void onError(GnssController* gnss) = 0;
    /** \} */
}; // class LTEObserver

/** \} */   // Group: LTE

/** \} */   // Group: com