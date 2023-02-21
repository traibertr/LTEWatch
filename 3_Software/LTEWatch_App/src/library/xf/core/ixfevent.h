/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    ixfevent.h
 *
 * \addtogroup XF eXecution Framework
 * \{
 *
 * \class   IXFEvent
 * \brief   Interface for an event.
 *
 * Interface for an event. The abstract methods defined in this interface need to be reimplemented in its derived
 * classes. These methods are used by other classes in the framework (ex. XFThread, XFReactive).
 *
 * Requirements:
 * - Needs to hold a property which allows to distinguish between different types of events (#_eventType).
 * - Use an enumeration to define the type for the event type property (#eEventType).
 * - The event type property needs to be set at initialization time and cannot be changed afterwards.
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
#include <stdint.h>


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class IXFReactive;
class IXFEvent
{
public :
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC TYPE DECLARATION SECTION                                                                              */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
     * \brief   Defines the types of events.
     ****************************************************************************************************************/
    typedef enum
    {
        Unknown         =  0,               ///< Unknown state (not initialized).
        Initial         =  1,               ///< Initial (pseudo) state.
        NullTransition  =  2,               ///< Event generated by the state machine to define a null transition.
        Event           =  3,               ///< Normal event.
        Timeout         =  4,               ///< Defines a time-out event.
        Terminate       = -1                ///< Defines a terminate state. Will end the state machine.
    } eEventType;
    
    
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* CONSTRUCTOR SECTION                                                                                          */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
     * \brief   Default class constructor
     ****************************************************************************************************************/
    IXFEvent(eEventType eventType, uint8_t id, IXFReactive * pBehaviour, bool deleteMe = true) 
        : _eventType(eventType), _id(id), _pBehaviour(pBehaviour), _deleteMe( deleteMe ) {}

    /************************************************************************************************************//**
     * \brief   Class destructor
     ****************************************************************************************************************/
    virtual ~IXFEvent() {}

        
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PUBLIC DECLARATION SECTION                                                                                   */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
     * \brief   Returns the type of the event.
     *          Can be used to distinguish between an event or a time-out.
     ****************************************************************************************************************/
    inline eEventType getEventType() const              { return _eventType; }

    /************************************************************************************************************//**
     * \brief   Returns pointer to behavioural class.
     *          Returns a pointer to the behavioural class processing the event (see #_pBehaviour).
     ****************************************************************************************************************/
    inline IXFReactive* getBehaviour() const            { return _pBehaviour; }

    /************************************************************************************************************//**
     * \brief   Returns pointer to behavioural class.
     *          Returns a pointer to the behavioural class processing the event (see #_pBehaviour).
     ****************************************************************************************************************/
    inline bool mustBeDeleted() const                   { return _deleteMe; }

    /************************************************************************************************************//**
     * \brief   Sets pointer to behavioural class (see #_pBehaviour).
     *          Sets the behaviour in which the event should be executed.
     ****************************************************************************************************************/
    inline void setBehaviour(IXFReactive* pBehaviour)   { _pBehaviour = pBehaviour; }

    /************************************************************************************************************//**
     * \brief   Returns #_id identifying the event in the behaviours context.
     ****************************************************************************************************************/
    inline int getId() const                            { return _id; }

protected:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PROTECTED DECLARATION SECTION                                                                                */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    // Attributes
    const eEventType    _eventType;         ///< \brief Holds the type of the event.
    uint8_t             _id;                ///< \brief Event id

    IXFReactive*        _pBehaviour;        ///< \brief Pointer to behavioural class processing the event.
    bool                _deleteMe;          ///< \brief To know if the event has to be deleted after consumption
};

/** \} */