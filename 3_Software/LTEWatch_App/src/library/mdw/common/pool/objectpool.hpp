/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2021
 * Created by Thomas Sterren (thomas.sterren@hevs.ch) & Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * \file    objectpool.h
 *
 * \addtogroup MemoryPool
 * \{
 *
 * \class   ObjectPool
 * \brief   Class which handles the objectpool
 *
 * Initial author: Thomas Sterren
 * Creation date: 2017-04-27
 *
 * \author  Thomas Sterren (thomas.sterren@hevs.ch), Patrice Rudaz (patrice.rudaz@hevs.ch)
 * \date    March 2021
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <stdint.h>
#include <string>
#include <new>
//#include <assert-config.h>
#ifdef HAVE_OBJECTPOOL_CONFIG
	//#include "objectpool-config.h"
#endif
//#include "debug-config.h"
#include <zephyr.h>
#include <logging/log.h>

//LOG_MODULE_REGISTER(OBJECT_POOL);


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* APPLICATION defintion                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#define PRINT_INFO 0

#if (PRINT_INFO == 0)
    #define prink(args...)            {(void) (0);}
#endif

#ifndef DEBUG_OBJECTPOOL_ENABLE
    #define DEBUG_OBJECTPOOL_ENABLE         0
#endif
#if defined(DEBUG_NRF_USER)
    //#include "tools/logtrace.h"
    //#include "tools/config/logtrace-config.h"
#endif
#if (DEBUG_OBJECTPOOL_ENABLE != 0) && defined(DEBUG_NRF_USER)
    #define OPOOL_DEBUG(args...)            LOG_TRACE("ObjectPool", LOG_LEVEL_DEBUG, args)
    #define OPOOL_ERROR(args...)            LOG_TRACE("ObjectPool", LOG_LEVEL_ERROR, args)
    #define OPOOL_INFO(args...)             LOG_TRACE("ObjectPool", LOG_LEVEL_INFO, args)
    #define OPOOL_WARN(args...)             LOG_TRACE("ObjectPool", LOG_LEVEL_WARN, args)
#elif defined(DEBUG_NRF_USER)
    #define OPOOL_ERROR(args...)            LOG_ERR("ObjectPool", args)
    #define OPOOL_DEBUG(...)                {(void) (0);}
    #define OPOOL_INFO(...)                 {(void) (0);}
    #define OPOOL_WARN(...)                 {(void) (0);}
#else
    #define OPOOL_DEBUG(args...)            {(void) (0);}
    #define OPOOL_ERROR(args...)            {(void) (0);}
    #define OPOOL_INFO(args...)             {(void) (0);}
    #define OPOOL_WARN(args...)             {(void) (0);}
#endif


inline void objectPoolEnterCriticalDefault() {}
inline void objectPoolExitCriticalDefault() {}

/****************************************************************************************************************//**
 * @ingroup pool
 * @brief Template to generate a class that can store objects of another class either inside a dynamically allocated
 * 		  memory region or inside a given memory block.
 *
 * It implements a simple reference count system that lets the objects be easily shared. Note that the reference count
 * is manually handled.
 *
 * Providing the appropriate enter and exit critical functions as template parameters the object pool can be made interrupt safe.
 ************************************************************************************************************/
template<typename T, void enterCritical() = objectPoolEnterCriticalDefault, void exitCritical() = objectPoolExitCriticalDefault>
class ObjectPool {
public:
    /********************************************************************************************************//**
     * @brief Creates a new object pool with the given number of objects preallocated either at the given
     * 		  memory region (the memory region pointer will be incremented by the size needed to store all
     * 		  data) or if the pointer to the memory pointer is NULL, the template class uses malloc to
     * 		  allocate memory for his content.
     *
     * @param numberOfObjectsToPreallocate 	The number of objects to hold
     * @param memory Pointer to the memory pointer or NULL if the template should use malloc (new) to allocate memory
     ************************************************************************************************************/
    explicit ObjectPool(size_t numberOfObjectsToPreallocate, uint8_t * * memory = NULL)
    {
        // If the objects should be placed into a given memory region.
        if (memory)
        {
            // Create the object pool.
            _objectPool = new (static_cast<void *>(*memory)) T[numberOfObjectsToPreallocate];
            *memory += numberOfObjectsToPreallocate * sizeof(T);

            // Create the reference count variables.
            _referenceCounts = new (static_cast<void *>(*memory)) int8_t[numberOfObjectsToPreallocate];
            *memory += numberOfObjectsToPreallocate;

            // Remember that we should not delete the memory afterwards.
            _dynamicMemoryUsed = false;
             printk("-ObjectPool -> INFO : %2d objects of %2d bytes preallocated in memory...\r\n", numberOfObjectsToPreallocate, sizeof(T));
        }

        // Use systems malloc()/new() functions to allocate memory.
        else
        {
            // Create the object pool.
            _objectPool = new T[numberOfObjectsToPreallocate];

            // Create the reference count variables.
            _referenceCounts = new int8_t[numberOfObjectsToPreallocate];

            // Remember that we should delete the memory afterwards.s
            _dynamicMemoryUsed = true;
             printk("-ObjectPool -> INFO : %2d objects of %2d bytes preallocated in dynamic memory...\r\n", numberOfObjectsToPreallocate, sizeof(T));
        }

        // Initialize the reference count and total free objects variables on success.
        __ASSERT(_objectPool && _referenceCounts,"Error");
        if (_objectPool && _referenceCounts)
        {
            memset(_referenceCounts, 0, numberOfObjectsToPreallocate);
            _objectCount = numberOfObjectsToPreallocate;
            _availableObjectCount = numberOfObjectsToPreallocate;
             printk("-ObjectPool -> INFO : %d of %d objects available (size: %d)\r\n", _availableObjectCount, _objectCount, sizeof(T));
        }

        // Something went wrong, so we can not offer any object.
        else
        {
            if (_objectPool)
                delete[] _objectPool;
            if (_referenceCounts)
                delete[] _referenceCounts;
            _dynamicMemoryUsed = false;
            _objectCount = 0;
            _availableObjectCount = 0;
        }
    }

    /********************************************************************************************************//**
     * \brief   Releases the resources used by this pool in the case the memory was allocated dynamically.
     ************************************************************************************************************/
    #if !defined(OBJECTPOOL_OMIT_DESTRUCTOR) or (OBJECTPOOL_OMIT_DESTRUCTOR == 0)
        virtual ~ObjectPool()
        {
            // We have to release memory if it was allocated using new().
            if (_dynamicMemoryUsed)
            {
                if (_objectPool)
                    delete[] _objectPool;
                if (_referenceCounts)
                    delete[] _referenceCounts;
            }
        }
    #endif // OBJECTPOLL_OMIT_DESTRUCTOR

    /********************************************************************************************************//**
     * @brief Returns the total number of objects that this pool contains.
     *
     * @return Number of objects in pool.
     ************************************************************************************************************/
    inline size_t objectCount() const           { return _objectCount; }

    /********************************************************************************************************//**
     * @brief Returns the number of objects actually available.
     *
     * @return Number of available objects in pool.
     ************************************************************************************************************/
    inline size_t availableObjectCount() const  { return _availableObjectCount; }

    /********************************************************************************************************//**
     * @brief Tries to allocate a new object from the pool and returns a pointer to that object.
     *
     * If no more objects are left inside the pool, the method returns a NULL pointer.
     *
     * This method will not call the constructor of the returned object. It is up to
     * the caller to call the appropriate constructor of the object. Calling the constructor
     * may be done using a <a href="http://en.wikipedia.org/wiki/Placement_syntax">placement new</a>.
     *
     * @return Pointer to an object or NULL in the case that there are no more objects left inside the pool.
     ************************************************************************************************************/
    T * allocate()
    {
        // Do we have objects left?
        if (_availableObjectCount)
        {
            // Search first available object and return it.
            for (unsigned int i = 0; i < _objectCount; ++i)
            {
                if (_referenceCounts[i] == 0)
                {
                    enterCritical();
                    _referenceCounts[i] = 1;
                    _availableObjectCount--;
                    exitCritical();

                    #if (DEBUG_OBJECTPOOL_ENABLE != 0) && defined(DEBUG_NRF_USER)
                    {
                        if (sizeof(T) == MP_VAST_OBJECTS_SIZE)
                        {
                             printk("-ObjectPool -> ERROR : VAST_OBJECTS > avaialble objects: %d, reference count: %d\r\n", _availableObjectCount, _referenceCounts[i]);
                        }

                    }
                    #endif // #if (DEBUG_OBJECTPOOL_ENABLE != 0) && defined(DEBUG_NRF_USER)

                    return &_objectPool[i];
                }
            }
        }

        // We did not found any unused frame anymore, return NULL pointer.
         printk("-ObjectPool -> ERROR : No more object available of %d (size: %3d bytes)\r\n", _objectCount, sizeof(T));
        #if (DEBUG_OBJECTPOOL_ENABLE != 0) && defined(DEBUG_NRF_USER)
        {
            for (uint_fast8_t i = 0; i < 4096; i++)
            {
                // NOP, just waiting some time here
            }
        }
        #endif // #if (DEBUG_OBJECTPOOL_ENABLE != 0) && defined(DEBUG_NRF_USER)
        __ASSERT(false,"Error");
        return NULL;
    }

    /********************************************************************************************************//**
     * @brief Increments the reference count variable for the given object.
     *
     * @param object Pointer to the object for which the reference count should be incremented
     ************************************************************************************************************/
    void retain(T * object)
    {
        // Calculate index from pointer value.
        size_t index = (reinterpret_cast<uint8_t *>(object) - reinterpret_cast<uint8_t *>(_objectPool)) / sizeof(T);

        // If index is reasonable, increment the reference count for that object.
        if (index < static_cast<int>(_objectCount))
        {
            enterCritical();
            _referenceCounts[index] += 1;
            exitCritical();

            // Ensure that we do not have more references as we can count.
            if (_referenceCounts[index] >= 255)
            {
                 printk("-ObjectPool -> ERROR : Too much references! _referenceCounts[%d]: %d (Object's size: %d)\r\n", index, _referenceCounts[index], sizeof(T));
                __ASSERT(false,"Error");
            }
        }
        else
        {
             printk("-ObjectPool -> INFO : index: %d, _objectCount: %d (Object's size: %d)\r\n", index, static_cast<int>(_objectCount), sizeof(T));
        }
    }

    /********************************************************************************************************//**
     * @brief Decrements the reference count variable for the given object and gives it back to the pool if the
     *        reference count reaches 0.
     *
     * @param object Pointer to the object for which the reference count should be decremented
     * @note Do not release the same object in the pool more than once !
     ************************************************************************************************************/
    void release(T * object)
    {
        // Calculate index from pointer value.
        __ASSERT((reinterpret_cast<uint8_t *>(object) >= reinterpret_cast<uint8_t *>(_objectPool)),"Error");
        int index = (reinterpret_cast<uint8_t *>(object) - reinterpret_cast<uint8_t *>(_objectPool)) / sizeof(T);

        // If index is reasonable, decrement the reference count for that object.
        if (index < static_cast<int>(_objectCount))
        {
            __ASSERT(_referenceCounts[index] > 0,"Error");
            enterCritical();
            _referenceCounts[index] -= 1;

            // If the last released the object, increment the number of available objects.
            if (_referenceCounts[index] <= 0)
                _availableObjectCount++;
            exitCritical();

            #if (DEBUG_OBJECTPOOL_ENABLE != 0) && defined(DEBUG_NRF_USER)
            {
                if (sizeof(T) == MP_VAST_OBJECTS_SIZE)
                {
                    //printk("VAST_OBJECTS > available objects: %d, reference count: %d\r\n", _availableObjectCount, _referenceCounts[index]);
                }

            }
            #endif // #if (DEBUG_OBJECTPOOL_ENABLE != 0) && defined(DEBUG_NRF_USER)
        }
        else
        {
            //printk("index out of bound: %2d, available Object: %2d of %2d, size: %2d\r\n", index, _availableObjectCount, _objectCount, sizeof(T));
        }
    }

private:
    T*      _objectPool;
    int8_t* _referenceCounts;
    size_t  _objectCount;
    size_t  _availableObjectCount;
    bool    _dynamicMemoryUsed;
};

/** \} */   // Group: MemoryPool
