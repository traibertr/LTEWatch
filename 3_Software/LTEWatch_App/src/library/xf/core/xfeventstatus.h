/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    xfeventstatus.h
 *
 * \addtogroup XF eXecution Framework
 * \{
 *
 * \class   EventStatus
 * \brief   The EventStatus is used after consumption of an event.
 *
 * The EventStatus is used after consumption of an event. It gives an information on how the event was consumed and 
 * how the framework should react.
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

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class EventStatus
{
public:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC TYPE DECLARATION SECTION                                                                              */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
     * \brief   Status values about the event processed.
     * \see     XFReactive::process
     ****************************************************************************************************************/
    typedef enum
    {
        Unknown			= 0x00,
        Consumed		= 0x01,
        NotConsumed		= 0x02,
        Terminate		= 0x04
    } eEventStatus;


    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* CONSTRUCTOR SECTION                                                                                          */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
     * \brief   Constructor
     *
     * \param   eventStatus Initial value of the event status.
     ****************************************************************************************************************/
    EventStatus(eEventStatus eventStatus = Unknown) : _status(eventStatus) {}


    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC DECLARATION SECTION                                                                                   */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
     * \brief   Checks if both variables contain the same event status.
     ****************************************************************************************************************/
    bool operator == (const eEventStatus & eventStatus) const
    {
        return (this->_status == eventStatus);
    }

public:
    // Attributes
    eEventStatus _status;                   ///< \brief Attribute containing the status of the event.
};

typedef EventStatus::eEventStatus eEventStatus;

/** \} */
