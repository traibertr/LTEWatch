/* Copyright (c) 2021, HES-SO Valais
 * All rights reserved.
 */
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */

/********************************************************************************************************************
 *         Register configuration
 *-------------------------------------------------------------------------------------------------------------------
 *-------------------------------------------------------------------------------------------------------------------
 *         Hardware I/O configuration
 *-------------------------------------------------------------------------------------------------------------------
 * ----- Step by Step motor Output -----
 *
 *    A(CW)  B(CCW)
 *     |       |
 *    |¯|     |¯|
 *    | |     | |    <----- Coils
 *    |_|     |_|
 *     |_______|
 *         |
 *     C(common)
 *
 *   Forward Command Signals            Backwards Command Signals
 *     _           _
 * A _| |_________| |_______            _---___---___---___---_
 *                                        _           _
 * B _---___---___---___---_            _| |_________| |_______         --- Tristate
 *           _           _                      _           _
 * C _______| |_________| |_            _______| |_________| |_
 *
 * The duration of an impulsion on the coils must be of 2.89 ms and
 * the gap between 2 signals must not be beneath 3.12 ms.
 *
 * ******************************************************************************************************************/

#include "platform-config.h"

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* DEFINITION                                                                                                       */
/*                                                                                                                  */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
// ISubject Condfiguration
#define MOTOR_MAX_OBSERVERS                 2
#define MOTOR_INIT_VIEW_MAX_OBSERVERS       2
#define MOTOR_INIT_VIEW_MODEL_MAX_OBSERVERS 2
#define MOTOR_INIT_CONTROLLER_MAX_OBSERVERS 2

// ------------------------------------------------------------------------------------------------------------------
#define MOTOR_RTC_ENABLE_ID                 0
#define MOTOR_PULSE_CC_IDX                  0
#define MOTOR_GAP_CC_IDX                    3
#define RTC_TIMER_MIN_CC_DELAY              6

// ------------------------------------------------------------------------------------------------------------------
// Pulse Timer Relative Condfiguration
#define TIMER_MIN_CC_DELAY                  6

// ------------------------------------------------------------------------------------------------------------------
// Motor HAL
#define GPIO_NO_PULL 0
#define GPIO_PIN_PULL_CONFIG                GPIO_NO_PULL
#if(0)
    #define GPIO_PIN_CONNECTION             GPIO_DISCONNECTED
#else
    #define GPIO_PIN_CONNECTION             GPIO_CONNECTED
#endif    
#define GPIO_PIN_DRIVE                      GPIO_DEFAULT_DRIVE

// ------------------------------------------------------------------------------------------------------------------
// MotorDriver HAL (in microseconds)
#define MOTOR_PULSE_LEN                     3000    // Original: 2890
#define MOTOR_SINLGE_GAP_TIME               16600   // Original: 16600
#define MOTOR_MIN_GAP_TIME                  3300    // Original: 3300

// ------------------------------------------------------------------------------------------------------------------
// Motor Driver
#define SMOOTH_MOTOR_MOVE                   0                                                                           // if set to 1, motor position is updated for each step otherwise only at motor's move end
#define PULSE_GAP_CORRECTION                1
#define PULSE_TICK_CORRECTION               1
#define SPEED_UP_SINGLE_MOTOR               0

// ------------------------------------------------------------------------------------------------------------------
// Default Animation
#define NB_STEPS_FOR_ANIMATION_DIVIDER      12                                                                          // During default animation, the motor will move of 1/NB_STEPS_FOR_ANIMATION_DIVIDER of the total number of steps (for example, if hte motor has 60 steps, it will move of 5 steps on each direction)

// ------------------------------------------------------------------------------------------------------------------
// Motor's position when in deep_sleep mode
// - in deep_sleed charged mode, we put the hands at 10:10:37
// - in deep_sleed discharged mode, we put the hands at 08:20:07
#define MOTOR_DEEP_SLEEP_CHARGED_SECOND     37
#define MOTOR_DEEP_SLEEP_CHARGED_MINUTE     10
#define MOTOR_DEEP_SLEEP_CHARGED_HOUR       10
#define MOTOR_DEEP_SLEEP_DISCHARGED_SECOND  7
#define MOTOR_DEEP_SLEEP_DISCHARGED_MINUTE  20
#define MOTOR_DEEP_SLEEP_DISCHARGED_HOUR    8

// ------------------------------------------------------------------------------------------------------------------
// Motor
#define NB_COILS_PER_MOTOR                  3

#define M1CA                                M1_CA_PIO
#define M1CB                                M1_CB_PIO
#define M1CC                                M1_CC_PIO
#define M1STEPS                             M1_STEPS //M1_STEPS for 360° rotation
#define M1NBS                               M1_NBS   //M1_NBS -> 1s = (180/60) = 3 steps
#define M1CCW                               M1_CCW   //M1_CCW

#if (NB_MAX_MOTORS > 1)
    #define M2CA                            M2_CA_PIO
    #define M2CB                            M2_CB_PIO
    #define M2CC                            M2_CC_PIO
    #define M2STEPS                         M2_STEPS
    #define M2NBS                           M2_NBS
    #define M2CCW                           M2_CCW
#endif

#if (NB_MAX_MOTORS > 2)
    #define M3CA                            M3_CA_PIO
    #define M3CB                            M3_CB_PIO
    #define M3CC                            M3_CC_PIO
    #define M3STEPS                         M3_STEPS
    #define M3NBS                           M3_NBS
    #define M3CCW                           M3_CCW
#endif
