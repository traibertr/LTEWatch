/***************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Michael Clausen, Thierry Hischier, Patrice Rudaz
 * All rights reserved.
 *
 * Initial author: Michael Clausen, Thierry Hischier
 * Creation date: 2018-01-24
 *
 * @file    functionid.h
 *
 * @addtogroup Function
 * \{
 *
 * @class   FunctionId
 * @brief   Class to specifiy the available functions and their identifiers
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
class FunctionId
{
    public:
        /* ******************************************************************************************************** */
        /*                                                                                                          */
        /* PUBLIC SECTION                                                                                           */
        /*                                                                                                          */
        /* ******************************************************************************************************** */
        typedef enum
        {
            FUNCTION_CLOCK              = 1,
            FUNCTION_CLOCK_HANDLE_SET   = 2,
            FUNCTION_TIME_SET           = 3,
//            FCT_NUMBER = 15,         ///< update this when adding functions
            FUNCTION_MENU = 0xFE,
            FUNCTION_UNKNOWN = 0xFF
        } eAppFunctionId;

        /********************************************************************************************************//**
         * @brief   Returns `true` if the function is avaialble for the product and `false` otherewise
         *
         * @param   id    The identifier of the function to checks
         ************************************************************************************************************/
        static bool isAvailable(int id)
        {
            return ((id == FUNCTION_CLOCK)                  ||
                    (id == FUNCTION_CLOCK_HANDLE_SET)       ||
                    (id == FUNCTION_TIME_SET)               ||
                    (id == FUNCTION_MENU));                 
        }

        static eAppFunctionId fromByte(uint8_t id)
        {
            if (isAvailable(id)) 
            {
                return static_cast<eAppFunctionId>(id);
            }

            return FUNCTION_UNKNOWN;
        }

        #if defined(DEBUG_NRF_USER)
            /****************************************************************************************************//**
             * @brief   Returns the string which describes the given function's identifier.
             *
             * @param   state    The identifier of the function to translate in string
             ********************************************************************************************************/
            static const char* to_string(eAppFunctionId fctId)
            {
                switch(fctId)
                {
                    case FUNCTION_CLOCK:                return "FUNCTION_CLOCK";
                    case FUNCTION_CLOCK_HANDLE_SET:     return "FUNCTION_CLOCK_HANDLE_SET";
                    case FUNCTION_TIME_SET:             return "FUNCTION_TIME_SET";
                    case FUNCTION_MENU:                 return "FUNCTION_MENU";
                    default:                            return "Unknown Function";
                }
            }
        #endif // #if defined(DEBUG_NRF_USER)
};

/** \} */   // Group: Function
