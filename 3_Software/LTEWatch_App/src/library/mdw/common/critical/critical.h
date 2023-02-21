/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren, Patrice Rudaz
 * All rights reserved.
 *
 * \file    critical.h
 * \brief   Methods to enter or exit critical section and to know if we already are inside an interrupt service
 *          routine.
 *
 * Initial author: Thomas Sterren
 * Creation date: 2016-06-07
 *
 * \author  Patrice Rudaz
 * \date    November 2021
 ********************************************************************************************************************/
#ifndef CRITICAL_H
#define CRITICAL_H

#if defined(__cplusplus)
extern "C"
{
#endif
    /************************************************************************************************************//**
     * \brief   Returns true if code is executed inside an interrupt service routine (ISR).
     ****************************************************************************************************************/
    int inIsr(void);

    /************************************************************************************************************//**
     * \brief   Enters critical region. May also be called inside an ISR.
     ****************************************************************************************************************/
    void enterCritical(void);

    /************************************************************************************************************//**
     * \brief   Exits critical region. May also be called inside an ISR.
     ****************************************************************************************************************/
    void exitCritical(void);
#if defined(__cplusplus)
}
#endif
#endif /* CRITICAL_H */
