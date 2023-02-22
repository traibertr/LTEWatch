/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * @file    buttonid.h
 *
 * @addtogroup GPIO
 * \{
 *
 * @addtogroup Button
 * \{
 *
 * @class   ButtonId
 * @brief   Class to specify the available ID for instances of Button's class.
 *
 * This class is the enumeration of all available identifiers (ID) to be assign to any Button's
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
#include "gpioid.h"


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
    class ButtonId
    {
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC DECLARATION SECTION                                                                               */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        /********************************************************************************************************//**
         * Enumeration of button's id available in the current application
         ************************************************************************************************************/
        typedef enum
        {
            UnknownButtonId = 0x00,      ///< Unknown button (not initialized).

            Button1         = 0x01,      ///< Button ID for the first button.
            Button2         = 0x02,      ///< Button ID for the second button.
            Button3         = 0x04,      ///< Button ID for the third button.
            Button4         = 0x08,      ///< Button ID for the fourth button.
            Button5         = 0x10,      ///< Button ID for the fifth button.
            Button6         = 0x20,      // ... and so on ...
            Button7         = 0x40,
            Button8         = 0x80,

            TopEnumButtonId = 0xff      ///< Last entry of the enum, should not be used !
        } eButtonId;

        /********************************************************************************************************//**
        * @brief   Returns the GpioID matching the given button's identifier
        *
        * @param   id   The Identifier of the button to find a match.
        *************************************************************************************************************/
        static GpioId::eGpioId toGpioId(eButtonId id)
        {
            switch(id)
            {
                case Button1:   return GpioId::Gpio1;
                case Button2:   return GpioId::Gpio2;
                case Button3:   return GpioId::Gpio3;
                case Button4:   return GpioId::Gpio4;
                case Button5:   return GpioId::Gpio5;
                case Button6:   return GpioId::Gpio6;
                case Button7:   return GpioId::Gpio7;
                case Button8:   return GpioId::Gpio8;
                default:        return GpioId::UnknownGpioId;
            }
        }

        /********************************************************************************************************//**
        * @brief   Returns the string which describes the given BUTTON.
        *
        * @param   id   The given BUTTON to translate in string.
        *************************************************************************************************************/
        static const char* toString(eButtonId id)
        {
            switch(id)
            {
                case Button1: return "Button 1";
                case Button2: return "Button 2";
                case Button3: return "Button 3";
                case Button4: return "Button 4";
                case Button5: return "Button 5";
                case Button6: return "Button 6";
                case Button7: return "Button 7";
                case Button8: return "Button 8";
                default:    return "Unknown Button";
            }
        }
    };

}   // Namespace gpio

/** \} */   // Group: Button

/** \} */   // Group: GPIO
