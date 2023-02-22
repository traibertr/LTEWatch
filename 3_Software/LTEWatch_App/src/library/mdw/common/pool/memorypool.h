/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    memorypool.h
 *
 * @addtogroup MemoryPool
 * \{
 *
 * @class   MemoryPool
 * @brief   Class which handles the memorypool
 *
 * Initial author: Thomas Sterren
 * Creation date: 2017-04-27
 *
 * \author  Patrice Rudaz
 * \date    November 2021
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include "memorypool-config.h"
#include "objectpool.hpp"

#if (MP_INTERRUPT_SAFE != 0)
    #include "critical.h"
#endif // MP_INTERRUPT_SAFE

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* APPLICATION defintion                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#if (USE_MEMORYPOOL != 0)   // Set USE_MEMORYPOOL to 1 to use/enable memory pool.

#ifndef MP_SMALL_OBJECTS_SIZE
    #define MP_SMALL_OBJECTS_SIZE           8
#endif
#ifndef MP_MEDIUM_OBJECTS_SIZE
    #define MP_MEDIUM_OBJECTS_SIZE          16
#endif
#ifndef MP_BIG_OBJECTS_SIZE
    #define MP_BIG_OBJECTS_SIZE             32
#endif
#ifndef MP_HUGE_OBJECTS_SIZE
    #define MP_HUGE_OBJECTS_SIZE            64
#endif

#ifndef MP_VAST_OBJECTS_SIZE
    #define MP_VAST_OBJECTS_SIZE            256
#endif

#ifndef MP_SMALL_OBJECTS_COUNT
    #define MP_SMALL_OBJECTS_COUNT          4
#endif
#ifndef MP_MEDIUM_OBJECTS_COUNT
    #define MP_MEDIUM_OBJECTS_COUNT         4
#endif
#ifndef MP_BIG_OBJECTS_COUNT
    #define MP_BIG_OBJECTS_COUNT            4
#endif
#ifndef MP_HUGE_OBJECTS_COUNT
    #define MP_HUGE_OBJECTS_COUNT           2
#endif
#ifndef MP_VAST_OBJECTS_COUNT
    #define MP_VAST_OBJECTS_COUNT           0
#endif

#if (MP_INTERRUPT_SAFE != 0)
    // Macro to pass enter and exit critical sections to the object pools
    #define OP_TP_PARAMS_IS ,enterCritical, exitCritical
#else
    // Not using critical sections in memory pool
    #define OP_TP_PARAMS_IS
#endif


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class MemoryPool
{
public:
    typedef ObjectPool<void OP_TP_PARAMS_IS>* PoolPtr;
public:
    static void initialize();
    static void* allocate(size_t sz);
    static void release(void* p);
    static PoolPtr getPoolByNumber(uint16_t poolNumber);
    static PoolPtr getPoolByObjectSize(size_t objectSize);

protected:
    static bool isSmallObject(void* p);
    static bool isMediumObject(void* p);
    static bool isBigObject(void* p);
    static bool isHugeObject(void* p);
    static bool isVastObject(void* p);

    struct SmallObject
    { uint8_t data[MP_SMALL_OBJECTS_SIZE]; };
    struct MediumObject
    { uint8_t data[MP_MEDIUM_OBJECTS_SIZE]; };
    struct BigObject
    { uint8_t data[MP_BIG_OBJECTS_SIZE]; };
    struct HugeObject
    { uint8_t data[MP_HUGE_OBJECTS_SIZE]; };
    struct VastObject
    { uint8_t data[MP_VAST_OBJECTS_SIZE]; };

private:
    static ObjectPool<SmallObject  OP_TP_PARAMS_IS> smallPool;
    static ObjectPool<MediumObject OP_TP_PARAMS_IS> mediumPool;
    static ObjectPool<BigObject    OP_TP_PARAMS_IS> bigPool;
    static ObjectPool<HugeObject   OP_TP_PARAMS_IS> hugePool;
    static ObjectPool<VastObject   OP_TP_PARAMS_IS> vastPool;
};
#endif // USE_MEMORYPOOL

/** \} */   // Group: MemoryPool
