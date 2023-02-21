/****************************************************************************************************************//**
 * Copyright (c) 2021, Hes-so VALAIS/WALLIS, HEI Sion
 * All rights reserved.
 *
 * \file    hardware-revision.h
 *
 * \author  Patrice Rudaz (patrice.rudaz@hevs.ch)
 * \version 2.0.0
 * \date    October 2021
 ************************************************************************************************************/
#ifndef MEMORYPOOL_CONFIG_H
#define MEMORYPOOL_CONFIG_H

#define USE_MEMORYPOOL              1       ///< Set to 1 if you need to use the memory pool (class MemoryPool).
#define MP_INTERRUPT_SAFE			1       ///< Set to 1 to make the memory pool interrupt safe.

// Object sizes to be handled by the memory pool
#define MP_SMALL_OBJECTS_SIZE       8       ///< Maximum size for small objects
#define MP_MEDIUM_OBJECTS_SIZE      16      ///< Maximum size for medium objects
#define MP_BIG_OBJECTS_SIZE	        32      ///< Maximum size for big objects
#define MP_HUGE_OBJECTS_SIZE	    64      ///< Maximum size for huge objects
#define MP_VAST_OBJECTS_SIZE	    256     ///< Maximum size for vast objects

// Amount of objects to be handled by the memory pool
#define MP_SMALL_OBJECTS_COUNT      24//20      ///< Amount of small objects
#define MP_MEDIUM_OBJECTS_COUNT     16//16      ///< Amount of medium objects
#define MP_BIG_OBJECTS_COUNT        24//20      ///< Amount of big objects
#define MP_HUGE_OBJECTS_COUNT       4//4       ///< Amount of huge objects
#define MP_VAST_OBJECTS_COUNT       16//16      ///< Amount of huge objects

#endif // MEMORYPOOL_CONFIG_H
