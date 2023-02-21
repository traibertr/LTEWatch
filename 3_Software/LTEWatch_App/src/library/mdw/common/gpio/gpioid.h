/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    gpioid.h
 *
 * @addtogroup GPIO
 * \{
 *
 * @class   GpioId
 * @brief   Class to specify the available ID for instances of GPIO's class.
 *
 * This class is the enumeration of all available identifiers (ID) to be assign to any GPIO's
 * instance.
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
    class GpioId
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * Enumeration of gpio's id available in the current application
         ************************************************************************************************************/
        typedef enum
        {
            UnknownGpioId   = 0x00,
            Gpio1           = 0x01,
            Gpio2           = 0x02,
            Gpio3           = 0x04,
            Gpio4           = 0x08,
            Gpio5           = 0x10,
            Gpio6           = 0x20,
            Gpio7           = 0x40,
            Gpio8           = 0x80,
            TopEnumGpioId   = 0xff
        } eGpioId;

        /********************************************************************************************************//**
         * @brief   Returns the string which describes the given GPIO.
         *
         * @param   id   The given GPIO to translate in string.
         ************************************************************************************************************/
        static const char* to_string(eGpioId id)
        {
            switch(id)
            {
                case Gpio1: return "Gpio 1";
                case Gpio2: return "Gpio 2";
                case Gpio3: return "Gpio 3";
                case Gpio4: return "Gpio 4";
                case Gpio5: return "Gpio 5";
                case Gpio6: return "Gpio 6";
                case Gpio7: return "Gpio 7";
                case Gpio8: return "Gpio 8";
                default:    return "Unknown Gpio";
            }
        }
    };

}   // namespace gpio

/** \} */   // Group: GPIO
