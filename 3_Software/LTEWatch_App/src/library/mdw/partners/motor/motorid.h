/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * \file    motorid.h
 *
 * @addtogroup Motor
 * \{
 *
 * @class   motor::MotorId
 * @brief   Class to specify the available ID for the motors
 *
 * This class is responsible to distribute to every motor used in the project a different motor's idenfifier.
 *
 * \image html ./images/522.png "Caliber 522"
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2016-08-18
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
#include <stdint.h>


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
    class MotorId
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC SECTION                                                                                           */
        /*                                                                                                          */
        /* ******************************************************************************************************** */

        /**!< Enumeration of event's id available in the current application */
        typedef enum
        {
            UnknownMotor    = 0x00,         ///< @brief Unknown motor (not initialized).
            Motor1          = 0x01,         ///< @brief Motor ID for the first motor.
            Motor2          = 0x02,         ///< @brief Motor ID for the second motor.
            Motor3          = 0x04,         ///< @brief ... and so on
            Motor4          = 0x08,
            Motor5          = 0x10,
            Motor6          = 0x20,
            Motor7          = 0x40,
            Motor8          = 0x80
        } eMotorId;

        static eMotorId fromInt(uint8_t id)
        {
            switch(id)
            {
                case 0x01:  return Motor1;
                case 0x02:  return Motor2;
                case 0x04:  return Motor3;
                case 0x08:  return Motor4;
                case 0x10:  return Motor5;
                case 0x20:  return Motor6;
                case 0x40:  return Motor7;
                case 0x80:  return Motor8;
                default:    return UnknownMotor;
            }
        }

        /********************************************************************************************************//**
        * @brief   Returns the string which describes the given Motor.
        *
        * @param   id   The given Motor to translate in string.
        ************************************************************************************************************/
        static const char* toString(eMotorId id)
        {
            switch(id)
            {
                case Motor1: return "Motor 1";
                case Motor2: return "Motor 2";
                case Motor3: return "Motor 3";
                case Motor4: return "Motor 4";
                case Motor5: return "Motor 5";
                case Motor6: return "Motor 6";
                case Motor7: return "Motor 7";
                case Motor8: return "Motor 8";
                default:    return "Unknown Motor";
            }
        }
    };

}   // Namespace motor

/** \} */   // Group: Motor
