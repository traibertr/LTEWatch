/* Copyright (c) 2018, HES-SO Valais
 * All rights reserved.
 */
#ifndef SDK_TOOLS_CONFIG_H
#define SDK_TOOLS_CONFIG_H

#include <platform-config.h>

#ifndef UNUSED
	#define UNUSED(x) ((void)0)
#endif // UNUSED

// Trying to know the use of "app_timer"
#if (USE_XF != 0) || (USE_GPIOS != 0) || (USE_BUTTONS != 0) || (USE_CALENDAR != 0)
    #define USE_APP_TIMER                    1	// automatic discovery of the use of any app_timer
#else
    #define USE_APP_TIMER                    0   // Or set it manually 
#endif
#define TOOLS_APP_TIMER_MIN_TIMEOUT_TICKS   APP_TIMER_MIN_TIMEOUT_TICKS

// ------------------------------------------------------------------------------------------------------------------
#define DEAD_BEEF                           0xDEADBEEF
    
// ------------------------------------------------------------------------------------------------------------------
// IEEE-11073 FLOAT
#define USE_IEEE11073_16                    1

// ------------------------------------------------------------------------------------------------------------------
// IEEE-11073 FLOAT
#define IEEE11073_MANTISSA_MAX              0x007fffff
#define IEEE11073_MANTISSA_MASK             0x00ffffff

// ------------------------------------------------------------------------------------------------------------------
// IEEE-11073 SFLOAT
#define IEEE11073_16_MANTISSA_MAX           0x07ff
#define IEEE11073_16_MANTISSA_MASK          0x0fff
#define IEEE11073_16_EXPONENT_MAX           0x07
#define IEEE11073_16_EXPONENT_MASK          0x0f


#endif // SDK_TOOLS_CONFIG_H
