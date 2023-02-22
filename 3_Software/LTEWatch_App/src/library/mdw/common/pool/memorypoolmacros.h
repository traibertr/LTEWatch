/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    memorypoolmacros.h
 *
 * @addtogroup MemoryPool
 * \{
 *
 * @brief   Defines MACROS impementing the `new` operator using the MemoryPool
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
#include "memorypool.h"


#if (USE_MEMORYPOOL != 0)


#define DECLARE_MEMORYPOOL_OPERATORS(ClassName)                     \
    static void * operator new(std::size_t sz);                     \
    static void operator delete(void * pObject);

#define IMPLEMENT_MEMORYPOOL_OPERATORS(ClassName)                   \
    void * ClassName::operator new(std::size_t sz)                  \
    {                                                               \
        return MemoryPool::allocate(sz);                            \
    }                                                               \
    \
    void ClassName::operator delete(void * pObject)                 \
    {                                                               \
        MemoryPool::release(pObject);                               \
    }

#else
#define DECLARE_MEMORYPOOL_OPERATORS(ClassName)

#define IMPLEMENT_MEMORYPOOL_OPERATORS(ClassName)
#endif  // (USE_MEMORYPOOL != 0)

/** \} */   // Group: MemoryPool
