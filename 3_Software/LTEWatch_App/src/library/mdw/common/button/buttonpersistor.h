/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    buttonpersistor.h
 *
 * @addtogroup GPIO
 * \{
 *
 * @addtogroup Button
 * \{
 *
 * @class   ButtonPersistor
 * @brief   Class that handles button's parameters to be saved and restore before and after DFU...
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2017-08-11
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
#include <stdint.h>

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
    class ButtonPersistor
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* CONSTRUCTOR SECTION                                                                                      */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        inline ButtonPersistor() : 
            _singleClickCounter(0), _doubleClickCounter(0), _tripleClickCounter(1),
            _longPressCounter(0), _doubleLongPressCounter(0), _tripleLongPressCounter(0), _veryLongPressCounter(0), 
            _releaseCounter(0) {};

        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * @brief   Serialisation of the class
         ************************************************************************************************************/
        inline uint32_t* data()                         { return reinterpret_cast<uint32_t*>(this); }

        /********************************************************************************************************//**
         * @return  Retrieves the counter of single-click done on the button.
         ************************************************************************************************************/
        inline uint32_t singleClickCounter() const      { return _singleClickCounter; }

        /********************************************************************************************************//**
         * @return  Retrieves the counter of double-click done on the button.
         ************************************************************************************************************/
        inline uint32_t doubleClickCounter() const      { return _doubleClickCounter; }

        /********************************************************************************************************//**
         * @return  Retrieves the counter of tripple-click done on the button
         ************************************************************************************************************/
        inline uint32_t tripleClickCounter() const      { return _tripleClickCounter; }

        /********************************************************************************************************//**
         * @return  Retrieves the counter of long press done on the button.
         ************************************************************************************************************/
        inline uint32_t longPressCounter() const        { return _longPressCounter; }

        /********************************************************************************************************//**
         * @return  Retrieves the counter of double long press done on the button.
         ************************************************************************************************************/
        inline uint32_t doubleLongPressCounter() const  { return _doubleLongPressCounter; }

        /********************************************************************************************************//**
         * @return  Retrieves the counter of triple long press done on the button.
         ************************************************************************************************************/
        inline uint32_t tripleLongPressCounter() const  { return _tripleLongPressCounter; }

        /********************************************************************************************************//**
         * @return  Retrieves the counter of very long press done on the button.
         ************************************************************************************************************/
        inline uint32_t veryLongPressCounter() const    { return _veryLongPressCounter; }

        /********************************************************************************************************//**
         * @return  Retrieves the counter of release action done on the button.
         ************************************************************************************************************/
        inline uint32_t releaseCounter() const          { return _releaseCounter; }

        /********************************************************************************************************//**
         * @brief   Increments the counter of single-click done on the button.
         ************************************************************************************************************/
        inline void incrSingleClickCounter()            { ++_singleClickCounter; }

        /********************************************************************************************************//**
         * @brief   Increments the counter of single-click done on the button.
         ************************************************************************************************************/
        inline void decrSingleClickCounter()            { --_singleClickCounter; }

        /********************************************************************************************************//**
         * @brief   Increments the counter of double-click done on the button.
         ************************************************************************************************************/
        inline void incrDoubleClickCounter()            { ++_doubleClickCounter; }

        /********************************************************************************************************//**
         * @brief   Increments the counter of double-click done on the button.
         ************************************************************************************************************/
        inline void decrDoubleClickCounter()            { --_doubleClickCounter; }

        /********************************************************************************************************//**
         * @brief   Increments the counter of tripple-click done on the button.
         ************************************************************************************************************/
        inline void incrTripleClickCounter()            { ++_tripleClickCounter; }

        /********************************************************************************************************//**
         * @brief   Increments the counter of triple-click done on the button.
         ************************************************************************************************************/
        inline void decrTripleClickCounter()            { --_tripleClickCounter; }

        /********************************************************************************************************//**
         * @brief   Increments the counter of long press done on the button.
         ************************************************************************************************************/
        inline void incrLongPressCounter()              { ++_longPressCounter; }

        /********************************************************************************************************//**
         * @brief   Increments the counter of long press done on the button.
         ************************************************************************************************************/
        inline void decrLongPressCounter()              { --_longPressCounter; }

        /********************************************************************************************************//**
         * @brief   Increments the counter of double long press done on the button.
         ************************************************************************************************************/
        inline void incrDoubleLongPressCounter()        { ++_doubleLongPressCounter; }

        /********************************************************************************************************//**
         * @brief   Increments the counter of double long press done on the button.
         ************************************************************************************************************/
        inline void decrDoubleLongPressCounter()        { --_doubleLongPressCounter; }

        /********************************************************************************************************//**
         * @brief   Increments the counter of triple long press done on the button.
         ************************************************************************************************************/
        inline void incrTripleLongPressCounter()        { ++_tripleLongPressCounter; }

        /********************************************************************************************************//**
         * @brief   Increments the counter of triple long press done on the button.
         ************************************************************************************************************/
        inline void decrTripleLongPressCounter()        { --_tripleLongPressCounter; }

        /********************************************************************************************************//**
         * @brief   Increments the counter of very long press done on the button.
         ************************************************************************************************************/
        inline void incrVeryLongPressCounter()          { ++_veryLongPressCounter; }

        /********************************************************************************************************//**
         * @brief   Increments the counter of release action done on the button.
         ************************************************************************************************************/
        inline void incrReleaseCounter()                { ++_releaseCounter; }

    private:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PRIVATE DECLARATION SECTION                                                                              */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        // Attributes
        uint32_t    _singleClickCounter;
        uint32_t    _doubleClickCounter;
        uint32_t    _tripleClickCounter;
        uint32_t    _longPressCounter;
        uint32_t    _doubleLongPressCounter;
        uint32_t    _tripleLongPressCounter;
        uint32_t    _veryLongPressCounter;
        uint32_t    _releaseCounter;
    };

}  // Namespace gpio

/** \} */   // Group: Button

/** \} */   // Group: GPIO
