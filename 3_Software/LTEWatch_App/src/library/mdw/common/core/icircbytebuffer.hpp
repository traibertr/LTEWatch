/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2021
 * Created by Patrice Rudaz
 * All rights reserved.
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2019-09-03
 *
 * @file    icircbytebuffer.h
 * @class   ICircByteBuffer
 * @brief   Template class for a self written circular buffer of Bytes
 *
 * Keeps track of which byte to read and write next. Also, it keeps the information about which memory is 
 * allocated for the buffer and its size.
 *
 * @author  Patrice Rudaz (patrice.rudaz@hevs.ch)
 * @date    September 2019
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <stdint.h>
#include <stdlib.h>
#include <math.h>


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* APPLICATION defintion                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
template <size_t S> 
    
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
class ICircularBuffer
{
public:
    typedef enum
    {
        FIFO_SUCCESS    = 0,
        FIFO_ERROR_NULL,
        FIFO_ERROR_INVALID_LENGTH,
        FIFO_ERROR_NOT_FOUND,
        FIFO_ERROR_NO_MEM
    } eFiFoError;

    /* ************************************************************************************************************ */
    /* CONSTRUCTOR SECTION                                                                                          */
    /* ************************************************************************************************************ */
    inline ICircularBuffer() : _bufferSizeMask(S), _readPos(0), _writePos(0) {}

    /* ************************************************************************************************************ */
    /* PUBLIC DECLARATION SECTION                                                                                   */
    /* ************************************************************************************************************ */
    // --------------------------------------------------------------------------------------------------------------
    uint32_t get(uint8_t* pByte)
    {
        return getAt(0, pByte);
    }
    
    // --------------------------------------------------------------------------------------------------------------
    uint32_t getAt(uint16_t index, uint8_t* pByte)
    {
        if (_length() != 0)
        {
            _peek(index, pByte);
            return FIFO_SUCCESS;
        }
        
        return FIFO_ERROR_NOT_FOUND;
    }
    
    // --------------------------------------------------------------------------------------------------------------
    uint32_t flush()
    {
        _readPos = _writePos;
        return FIFO_SUCCESS;
    }
    
    // --------------------------------------------------------------------------------------------------------------
    uint32_t put(const uint8_t byte)
    {
        if (_checkSize())
        {
            _put(byte);
            return FIFO_SUCCESS;
        }
        
        return FIFO_ERROR_NO_MEM;
    }
    
    // --------------------------------------------------------------------------------------------------------------
    uint32_t read(uint8_t* pByte, uint32_t* pSize)
    {
        if (pSize == NULL)
        {
            return FIFO_ERROR_INVALID_LENGTH;
        }
        
        if (_length() == 0)
        {
            return FIFO_ERROR_NOT_FOUND;
        }
        
        const uint32_t elementCount     = _length();
        const uint32_t requestLength    = *pSize;
        
        // Check if application has requested only the size.
        if (pElement == NULL) 
        {
            (*pSize)                    = elementCount;
            return FIFO_SUCCESS;
        }
        
        uint32_t readSize               = MIN(requestLength, elementCount);
        uint32_t index                  = 0;
        
        // Fetch elements from the FIFO.
        while(index < readSize)
        {
            _peek(0, &pByte[index++]);
        }

        (*pSize)                        = readSize;
        return FIFO_SUCCESS;
    }
    
    // --------------------------------------------------------------------------------------------------------------
    uint32_t write(const uint8_t* pByte, uint32_t* pSize)
    {
        if (pSize == NULL)
        {
            return FIFO_ERROR_INVALID_LENGTH;
        }
        
        const uint32_t availableCount   = _count();

        // Check if the FIFO is FULL.
        if (availableCount == 0)
        {
            return FIFO_ERROR_NO_MEM;
        }
        
        // Check if application has requested only the size.
        if (pElement == NULL)
        {
            (*pSize)                    = availableCount;
            return FIFO_SUCCESS;
        }
        
        const uint32_t requestedLength  = *pSize;
        uint32_t index                  = 0;
        uint32_t writeSize              = MIN(requestedLength, availableCount);
        
        // Pushing element in the buffer
        while(index < writeSize)
        {
            _put(&pByte[index++];
        }
        
        *pSize                          = writeSize;
        return FIFO_SUCCESS;
    }

    // --------------------------------------------------------------------------------------------------------------
    inline size_t count() const             { return _count(); }
    
private:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PRIVATE DECLARATION SECTION                                                                                  */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    // private attributes
    uint8_t     _data[S];                   // Pointer to FIFO buffer memory
    size_t      _bufferSizeMask;            // Read/Write index mask. Also used for size checking
    uint32_t    _readPos;                   // Next read position in the FIFO buffer.
    uint32_t    _writePos;                  // Next write position in the FIFO buffer.
    
    // Private methods
    inline bool _checkSize() const                      { return (_length() <= _bufferSizeMask); }
    inline uint32_t _count() const                      { return _bufferSizeMask - _length() + 1; }
    inline uint32_t _length() const                     { return (_writePos - _readPos) & _bufferSizeMask; }
    inline void _put(const uint8_t byte)                { _data[_writePos & _bufferSizeMask] = byte; _writePos++; }
    inline void _peek(uint16_t index, uint8_t* pByte)   { (*pByte) = _data[(_readPos + index) & _bufferSizeMask]; _readPos++; }

};  // Template Class ICircularBuffer
