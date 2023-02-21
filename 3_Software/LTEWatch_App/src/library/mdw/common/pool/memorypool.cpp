/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz & Thomas Sterren
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "memorypool.h"
#include "objectpool.hpp"
#include "debug-config.h"
//#include <assert.h>

/* **************************************************************************************************************** */
/* DEBUG COMPILATION OPTION & MACRO                                                                                 */
/* **************************************************************************************************************** */
#ifndef DEBUG_MEMORYPOOL_ENABLE
    #define DEBUG_MEMORYPOOL_ENABLE         0
#endif

#if (DEBUG_MEMORYPOOL_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #include <logging/log.h>
    LOG_MODULE_REGISTER(memmory_pool, LOG_LEVEL_DBG);
    #define MEMP_ERROR(args...)             LOG_ERR(args) 
#else
    #define MEMP_ERROR(...)                 {(void) (0);}
#endif

/* **************************************************************************************************************** */
/* Declaration section                                                                                              */
/* **************************************************************************************************************** */
#if (USE_MEMORYPOOL != 0)   // Set USE_MEMORYPOOL to 1 to use/enable memory pool.

#define SMALL_REFCOUNT_SIZE     (MP_SMALL_OBJECTS_COUNT  * sizeof(uint8_t))
#define MEDIUM_REFCOUNT_SIZE    (MP_MEDIUM_OBJECTS_COUNT * sizeof(uint8_t))
#define BIG_REFCOUNT_SIZE       (MP_BIG_OBJECTS_COUNT    * sizeof(uint8_t))
#define HUGE_REFCOUNT_SIZE      (MP_HUGE_OBJECTS_COUNT   * sizeof(uint8_t))
#define VAST_REFCOUNT_SIZE      (MP_VAST_OBJECTS_COUNT   * sizeof(uint8_t))

static const uint32_t SMALL_MEMORY_SIZE     = MP_SMALL_OBJECTS_COUNT  * MP_SMALL_OBJECTS_SIZE;
static const uint32_t MEDIUM_MEMORY_SIZE    = MP_MEDIUM_OBJECTS_COUNT * MP_MEDIUM_OBJECTS_SIZE;
static const uint32_t BIG_MEMORY_SIZE       = MP_BIG_OBJECTS_COUNT    * MP_BIG_OBJECTS_SIZE;
static const uint32_t HUGE_MEMORY_SIZE      = MP_HUGE_OBJECTS_COUNT   * MP_HUGE_OBJECTS_SIZE;
static const uint32_t VAST_MEMORY_SIZE      = MP_VAST_OBJECTS_COUNT   * MP_VAST_OBJECTS_SIZE;

static uint8_t smallMemory[SMALL_MEMORY_SIZE   + SMALL_REFCOUNT_SIZE]  __attribute__((aligned(4)));
static uint8_t mediumMemory[MEDIUM_MEMORY_SIZE + MEDIUM_REFCOUNT_SIZE] __attribute__((aligned(4)));
static uint8_t bigMemory[BIG_MEMORY_SIZE       + BIG_REFCOUNT_SIZE]    __attribute__((aligned(4)));
static uint8_t hugeMemory[HUGE_MEMORY_SIZE     + HUGE_REFCOUNT_SIZE]   __attribute__((aligned(4)));
static uint8_t vastMemory[VAST_MEMORY_SIZE     + VAST_REFCOUNT_SIZE]   __attribute__((aligned(4)));

uint8_t* pSmallMemory   = (uint8_t*)smallMemory;
uint8_t* pMediumMemory  = (uint8_t*)mediumMemory;
uint8_t* pBigMemory     = (uint8_t*)bigMemory;
uint8_t* pHugeMemory    = (uint8_t*)hugeMemory;
uint8_t* pVastMemory    = (uint8_t*)vastMemory;

ObjectPool<MemoryPool::SmallObject  OP_TP_PARAMS_IS> MemoryPool::smallPool( MP_SMALL_OBJECTS_COUNT,  &pSmallMemory);
ObjectPool<MemoryPool::MediumObject OP_TP_PARAMS_IS> MemoryPool::mediumPool(MP_MEDIUM_OBJECTS_COUNT, &pMediumMemory);
ObjectPool<MemoryPool::BigObject    OP_TP_PARAMS_IS> MemoryPool::bigPool(   MP_BIG_OBJECTS_COUNT,    &pBigMemory);
ObjectPool<MemoryPool::HugeObject   OP_TP_PARAMS_IS> MemoryPool::hugePool(  MP_HUGE_OBJECTS_COUNT,   &pHugeMemory);
ObjectPool<MemoryPool::VastObject   OP_TP_PARAMS_IS> MemoryPool::vastPool(  MP_VAST_OBJECTS_COUNT,   &pVastMemory);

/* **************************************************************************************************************** */
/* PUBLIC SECTION                                                                                                   */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
void* MemoryPool::allocate(size_t sz)
{
    void* p = NULL;

    if (sz <= MP_SMALL_OBJECTS_SIZE)
    {
        p = smallPool.allocate();
        __ASSERT(p,"Too few small objects!");
    }
    else if (sz <= MP_MEDIUM_OBJECTS_SIZE)
    {
        p = mediumPool.allocate();
        __ASSERT(p,"Too few medium objects!");
    }
    else if (sz <= MP_BIG_OBJECTS_SIZE)
    {
        p = bigPool.allocate();
        __ASSERT(p,"Too few big objects!");
    }
    else if (sz <= MP_HUGE_OBJECTS_SIZE)
    {
        p = hugePool.allocate();
        __ASSERT(p,"Too few huge objects!");
    }
    else if (sz <= MP_VAST_OBJECTS_SIZE)
    {
        p = vastPool.allocate();
        __ASSERT(p,"Too few vast objects!");
    }
    else
    {
        __ASSERT(p,"Object size too big!");
    }
    return p;
}

// ------------------------------------------------------------------------------------------------------------------
void MemoryPool::release(void* p)
{
    if (isSmallObject(p))
    {
        smallPool.release((SmallObject*)p);
    }
    else if (isMediumObject(p))
    {
        mediumPool.release((MediumObject*)p);
    }
    else if (isBigObject(p))
    {
        bigPool.release((BigObject*)p);
    }
    else if (isHugeObject(p))
    {
        hugePool.release((HugeObject*)p);
    }
    else if (isVastObject(p))
    {
        vastPool.release((VastObject*)p);
    }
    else
    {
        MEMP_ERROR("Not origin from this pool!\r\n");
        __ASSERT(false,"Error");
    }
}

// ------------------------------------------------------------------------------------------------------------------
MemoryPool::PoolPtr MemoryPool::getPoolByNumber(uint16_t poolNumber)
{
    switch(poolNumber)
    {
        case 0:
            return reinterpret_cast<MemoryPool::PoolPtr>(&smallPool);
        case 1:
            return reinterpret_cast<MemoryPool::PoolPtr>(&mediumPool);
        case 2:
            return reinterpret_cast<MemoryPool::PoolPtr>(&bigPool);
        case 3:
            return reinterpret_cast<MemoryPool::PoolPtr>(&hugePool);
        case 4:
            return reinterpret_cast<MemoryPool::PoolPtr>(&vastPool);
        default:
            MEMP_ERROR("Unknown pool number: %d !\r\n", poolNumber);
            // Just in case...
            return reinterpret_cast<MemoryPool::PoolPtr>(&bigPool);
    }
}

// ------------------------------------------------------------------------------------------------------------------
MemoryPool::PoolPtr MemoryPool::getPoolByObjectSize(size_t objectSize)
{
    if (objectSize <= MP_SMALL_OBJECTS_SIZE)
    {
        return reinterpret_cast<MemoryPool::PoolPtr>(&smallPool);
    }
    else if (objectSize <= MP_MEDIUM_OBJECTS_SIZE)
    {
        return reinterpret_cast<MemoryPool::PoolPtr>(&mediumPool);
    }
    else if (objectSize <= MP_BIG_OBJECTS_SIZE)
    {
        return reinterpret_cast<MemoryPool::PoolPtr>(&bigPool);
    }
    else if (objectSize <= MP_HUGE_OBJECTS_SIZE)
    {
        return reinterpret_cast<MemoryPool::PoolPtr>(&hugePool);
    }
    else if (objectSize <= MP_VAST_OBJECTS_SIZE)
    {
        return reinterpret_cast<MemoryPool::PoolPtr>(&vastPool);
    }
    else
    {
        MEMP_ERROR("Unknown object size number: %d !\r\n", objectSize);
        // Just in case...
        return reinterpret_cast<MemoryPool::PoolPtr>(&bigPool);
    }
}

// ------------------------------------------------------------------------------------------------------------------
bool MemoryPool::isSmallObject(void* p)
{
    const uint8_t* const pMem = reinterpret_cast<uint8_t*>(p);
    return (pMem >= smallMemory) && (pMem < (smallMemory + SMALL_MEMORY_SIZE));
}

// ------------------------------------------------------------------------------------------------------------------
bool MemoryPool::isMediumObject(void* p)
{
    const uint8_t* const pMem = reinterpret_cast<uint8_t*>(p);
    return (pMem >= mediumMemory) && (pMem < (mediumMemory + MEDIUM_MEMORY_SIZE));
}

// ------------------------------------------------------------------------------------------------------------------
bool MemoryPool::isBigObject(void* p)
{
    const uint8_t* const pMem = reinterpret_cast<uint8_t*>(p);
    return (pMem >= bigMemory) && (pMem < (bigMemory + BIG_MEMORY_SIZE));
}

// ------------------------------------------------------------------------------------------------------------------
bool MemoryPool::isHugeObject(void* p)
{
    const uint8_t* const pMem = reinterpret_cast<uint8_t*>(p);
    return (pMem >= hugeMemory) && (pMem < (hugeMemory + HUGE_MEMORY_SIZE));
}

// ------------------------------------------------------------------------------------------------------------------
bool MemoryPool::isVastObject(void* p)
{
    const uint8_t* const pMem = reinterpret_cast<uint8_t*>(p);
    return (pMem >= vastMemory) && (pMem < (vastMemory + VAST_MEMORY_SIZE));
}

#endif // USE_MEMORYPOOL
