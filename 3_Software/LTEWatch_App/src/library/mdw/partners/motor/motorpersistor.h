/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * \file    motorpersistor.h
 *
 * \addtogroup Motor
 * \{
 *
 * \class   motor::MotorPersistor
 * \brief   Class that handles motor's parameters to be saved and restore before and after DFU...
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2017-05-17
 *
 * \author  Patrice Rudaz (patrice.rudaz@hevs.ch)
 * \date    July 2018
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <platform-config.h>

#if (USE_MOTORS != 0)
#include <stdint.h>


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* APPLICATION defintion                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Namespace Declaration                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
namespace motor 
{

    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* Class Declaration                                                                                            */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    class MotorPersistor
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* CONSTRUCTOR SECTION                                                                                      */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        inline MotorPersistor() : _stepCounter(0), _position(0), _alim(1), _reserved(0)    {};


        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * \brief   Serialisation of the class
          ***********************************************************************************************************/
        inline uint32_t*    data()                          { return reinterpret_cast<uint32_t*>(this); }

        /********************************************************************************************************//**
         * \brief   Gives the current position.
         ************************************************************************************************************/
        inline uint16_t     position() const                { return _position; }

        /********************************************************************************************************//**
         * \brief   Returns the counter of steps made during the complete life of the motor.
         ************************************************************************************************************/
        inline uint32_t     stepCounter() const             { return _stepCounter; }

        /********************************************************************************************************//**
         * \brief   Returns the counter of steps made during the complete life of the motor.
         ************************************************************************************************************/
        inline uint8_t      alim() const                    { return _alim & 0x01; }

        /********************************************************************************************************//**
         * \brief   Set the new position of hte motor
         ************************************************************************************************************/
        inline void         setPosition(uint16_t position)  { _position = position; }

        /********************************************************************************************************//**
         * \brief   Toggle the state of the alimentation's controller and increment the counter of steps
         ************************************************************************************************************/
        inline void         toggleAlim()                    { _alim++; _stepCounter++;}

    private:
        // SAV: Counter of steps made during the life of hte motor.
        uint32_t            _stepCounter;

        // to directly get the position in the `mps` buffer when using the Motor Position Service
        uint16_t            _position;

        // To know which impulse has to be applied on the output driven the motor
        uint8_t             _alim;

        // Values to be stored in flash memory MUST have a size which is a multiple of 32 bits.
        uint8_t             _reserved;
    };

}   // Namespace motor

#endif // #if (USE_MOTORS != 0)

/** \} */   // Group: Motor
