/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2021
 * Created by Patrice Rudaz & Thomas Sterren
 * All rights reserved.
 * ******************************************************************************************************************/

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "critical.h"
#include <zephyr.h>

/* **************************************************************************************************************** */
/* Declaration section                                                                                              */
/* **************************************************************************************************************** */
volatile int bOMEnterCriticalRegionNested = 0;

/* **************************************************************************************************************** */
/* --- Extern "C" Section ---                                                                                       */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------

int inIsr()
{
        // Check MCU in interrupt state
        return k_is_in_isr();
        
    return 0;
}

// ------------------------------------------------------------------------------------------------------------------
void enterCritical()
{
    #if (APP_SCHEDULER_ENABLED == 0) || (NRF_SDH_DISPATCH_MODEL != 1)
    {
        // Only disable interrupts when not calling from an ISR
        if (!inIsr()) 
        {
            if (bOMEnterCriticalRegionNested == 0) 
            {
                    // Disable interrupts
                    __disable_irq();
            }
            bOMEnterCriticalRegionNested++;
        }
    }
    #endif // #if (APP_SCHEDULER_ENABLED == 0) || (NRF_SDH_DISPATCH_MODEL != 1)
}

// ------------------------------------------------------------------------------------------------------------------
void exitCritical()
{
    #if (APP_SCHEDULER_ENABLED == 0) || (NRF_SDH_DISPATCH_MODEL != 1)
    {
        // Only enable interrupts when not calling from an ISR
        if (!inIsr()) 
        {
            bOMEnterCriticalRegionNested--;
            if (!bOMEnterCriticalRegionNested) 
            {
                    // Enable interrupts
                    __enable_irq();
            }
        }
    }
    #endif // #if (APP_SCHEDULER_ENABLED == 0) || (NRF_SDH_DISPATCH_MODEL != 1)
}
