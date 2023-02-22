/****************************************************************************************************************//**
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2021
 * Created by Patrice Rudaz (patrice.rudaz@hevs.ch)
 * All rights reserved.
 *
 * @file    ibuffer.hpp
 * @class   IBuffer
 * @brief   Template Class handling a buffer for characterisitics that have a data's length greater thant the msx MTU size.
 *
 * Initial author: Patrice Rudaz
 * Creation date: 2019-09-03
 *
 * @author  Patrice Rudaz
 * @date    September 2019
 ********************************************************************************************************************/
#pragma once

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Header files                                                                                                     */
/*                                                                                                                  */
/* **************************************************************************************************************** */
#include <stdint.h>
#include "io/endianess.hpp"


/* **************************************************************************************************************** */
/*                                                                                                                  */
/* NAMESPACE Declaration                                                                                            */
/*                                                                                                                  */
/* **************************************************************************************************************** */
template <size_t S>

/* **************************************************************************************************************** */
/*                                                                                                                  */
/* Class Declaration                                                                                                */
/*                                                                                                                  */
/* **************************************************************************************************************** */
class IBuffer
{
public:
    /* ************************************************************************************************************ */
    /*																										        */
    /* PUBLIC DEFINITION SECTION                                                                                    */
    /*																										        */
    /* ************************************************************************************************************ */
    typedef enum
    {
        BUFFER_SUCCESS = 0,
        BUFFER_ERROR_NULL,
        BUFFER_ERROR_NO_MEM
    } eBufferError;

    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* CONSTRUCTOR SECTION                                                                                          */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    explicit IBuffer()                      { clear(); }
    virtual ~IBuffer()                      {}

    /* ************************************************************************************************************ */
    /*																										        */
    /* PUBLIC DECLARATION SECTION                                                                                   */
    /*																										        */
    /* ************************************************************************************************************ */
    /************************************************************************************************************//**
     * @brief   Append new incoming data.
     *
     * Append new incoming data to the ones already in the buffer. If the pointer to the data to be added is null, 
     * the methods will return BUFFER_ERROR_NULL erro code. If the buffer is full or only a few data could  be added 
     * to the current ones, the method returns BUFFER_ERROR_NO_MEM, otherwise it returns BUFFER_SUCCESS. 
     * In case of BUFFER_ERROR_NO_MEM result, the amount of data that has been added is provided by `size` parameter.
     *
     * @param   data    Pointer to the new values to append
     * @param   size    Amount of data to append to the buffer
     *
     * @retval  BUFFER_SUCCESS          For a successfull process
     * @retval  BUFFER_ERROR_NO_MEM     If there is not enough room keft in the buffer
     * @retval  BUFFER_ERROR_NULL       If the buffer is full
     ****************************************************************************************************************/
    uint32_t append(const uint8_t* data, size_t* size)
    {
        uint32_t retVal = BUFFER_SUCCESS;

        if (data == NULL)
        {
            return BUFFER_ERROR_NULL;
        }
        
        if ((_size + *size) > S)
        {
            *size       = S - _size;
            retVal      = BUFFER_ERROR_NO_MEM;
        }

        if (*size > 0)
        {
            memcpy(&_buffer[_offset], data, *size);
            _size      += *size;
        }
        return retVal;
    }

     /************************************************************************************************************//**
     * @brief   Put a byte at the next free position.
     *
     * Append a byte to the data already in the buffer. If the buffer is full or only a few data could  be added to 
     * the current ones, the method returns BUFFER_ERROR_NO_MEM, otherwise it returns BUFFER_SUCCESS. 
     *
     * @param   value   The value to be put at the end of the buffer.
     *
     * @retval  BUFFER_SUCCESS          For a successfull process
     * @retval  BUFFER_ERROR_NO_MEM     If there is not enough room keft in the buffer
     ****************************************************************************************************************/
    uint32_t put(uint8_t value)
    {
        const uint8_t val   = value;
        size_t len          = sizeof(uint8_t);
        uint32_t errCode    = append((uint8_t*) &val, &len);

        if (errCode != BUFFER_ERROR_NULL) 
        {
            updateOffset(len, S);
        }
        return errCode;
    }

     /************************************************************************************************************//**
     * @brief   Put a byte at the gien position. The old value will be overwrittn.
     *
     * Set the given byte in the buffer at the provided offset.. If the offset is out of bound, the method returns 
     * BUFFER_ERROR_NO_MEM, otherwise it returns BUFFER_SUCCESS. 
     *
     * @param   value   The `byte` value to be set. 
     * @param   offset  The position where to set the new value.
     *
     * @retval  BUFFER_SUCCESS          For a successfull process
     * @retval  BUFFER_ERROR_NO_MEM     If there is not enough room keft in the buffer
     ****************************************************************************************************************/
    uint32_t putAtOffset(uint8_t value, size_t offset) {
        if (offset >= S)
            return BUFFER_ERROR_NO_MEM;

        _buffer[offset] = value;
        return BUFFER_SUCCESS;
    }

     /************************************************************************************************************//**
     * @brief   Put a short (16 bits value) at the next free position.
     *
     * Append a 16bits value to the data already in the buffer. If the buffer is full or only a few data could  be 
     * added to the current ones, the method returns BUFFER_ERROR_NO_MEM, otherwise it returns BUFFER_SUCCESS. 
     *
     * @param   value   The 16bits value to be put at the end of the buffer.
     *
     * @retval  BUFFER_SUCCESS          For a successfull process
     * @retval  BUFFER_ERROR_NO_MEM     If there is not enough room keft in the buffer
     ****************************************************************************************************************/
    uint32_t putShort(uint16_t value)
    {
        const uint16_t val  = com::io::Endianness::swapByteOrder16(value);
        size_t len          = sizeof(uint16_t);
        uint32_t errCode    = append((uint8_t*) &val, &len);

        if (errCode != BUFFER_ERROR_NULL) 
        {
            updateOffset(len, S);
        }
        return errCode;
    }

     /************************************************************************************************************//**
     * @brief   Put a long (32 bits value) at the next free position.
     *
     * Append a 32 bits value to the data already in the buffer. If the buffer is full or only a few data could  be 
     * added to the current ones, the method returns BUFFER_ERROR_NO_MEM, otherwise it returns BUFFER_SUCCESS. 
     *
     * @param   value   The 32 bits value to be put at the end of the buffer.
     *
     * @retval  BUFFER_SUCCESS          For a successfull process
     * @retval  BUFFER_ERROR_NO_MEM     If there is not enough room keft in the buffer
     ****************************************************************************************************************/
    uint32_t putLong(uint32_t value)
    {
        const uint32_t val  = com::io::Endianness::swapByteOrder32(value);
        size_t len          = sizeof(uint32_t);
        uint32_t errCode    = append((uint8_t*) &val, &len);

        if (errCode != BUFFER_ERROR_NULL) 
        {
            updateOffset(len, S);
        }
        return errCode;
    }

    /************************************************************************************************************//**
     * @brief   Returns how many bytes can still be stored in the buffer.
     ****************************************************************************************************************/
    inline size_t availableSize() const     { return S - _size; }
    
    /************************************************************************************************************//**
     * @brief   Returns the byte at current position in the buffer.
     ****************************************************************************************************************/
    inline uint8_t byteAtOffset() const     { return _buffer[_offset]; }

    /************************************************************************************************************//**
     * @brief   Returns the size of the buffer
     ****************************************************************************************************************/
    inline size_t capacity() const          { return S; }

    /************************************************************************************************************//**
     * @brief   Clear the buffer, its size and offset position.
     ****************************************************************************************************************/
    inline void clear()                     { _offset = 0; _size = 0; memset(_buffer, 0, S); }
    
    /************************************************************************************************************//**
     * @brief   Returns the last byte actually in the buffer.
     ****************************************************************************************************************/
    inline uint8_t last() const             { return _buffer[_size - 1]; } 

    /************************************************************************************************************//**
     * @brief   Retrieves the pointer to the buffer.
     *
     * This method points to the first bytes of the buffer.
     ****************************************************************************************************************/
    inline uint8_t* array()                 { return _buffer; }

    /************************************************************************************************************//**
     * @brief   Retrieves the pointer to the buffer's value from the given offset.
     *
     * This method points to the first bytes of the buffer.
     ****************************************************************************************************************/
    inline uint8_t* arrayAt(uint8_t offset) { return &_buffer[offset]; }

    /************************************************************************************************************//**
     * @brief   Retrieves the pointer to the next chunck of the buffer.
     *
     * This method points to the first bytes of the buffer which has not yet been handled.
     ****************************************************************************************************************/
    inline uint8_t* nextChunk()             { return &_buffer[_offset]; }
    
    /************************************************************************************************************//**
     * @brief   Retrieves the offset position currently in use to access bytes.
     ****************************************************************************************************************/
    inline size_t offset() const            { return _offset; } 
    
    /************************************************************************************************************//**
     * @brief   Returns the byte at the given position. If `at` value is out of bound, it returns '0'.
     * 
     * @param   at  The given position where to read the byte value...    
     ****************************************************************************************************************/
    inline uint8_t peek(size_t at) const    { if (at < _size) return _buffer[at]; return 0x00; }  

    /************************************************************************************************************//**
     * @brief   Retrieves the current size of the buffer, the amount of bytes stored in it.
     ****************************************************************************************************************/
    inline size_t size() const              { return _size; }
    
    /************************************************************************************************************//**
     * @brief   Returns the byte at top position in the buffer.
     ****************************************************************************************************************/
    inline uint8_t top() const              { return _buffer[0]; }  

    /************************************************************************************************************//**
     * @brief   According to the given parameters, this method updates to offset of the buffer
     *
     * It updates the current offset position of the buffer with the quantity of handled bytes `len`. According 
     * to the MTU max size of the current connection, the returns the size of the next chunk to deal with.
     *
     * @param   len         The last amount of byte which has been successfully handled.
     * @param   mtuSize     The GATT MTU max size of the current connection.
     *
     * @return  The available size of the next chunk.
     ****************************************************************************************************************/
    size_t updateOffset(size_t len, size_t mtuSize)
    {
        _offset         += len;
        const size_t tmp = _size - _offset;
        return (tmp > mtuSize) ? mtuSize : tmp;
    }

    /************************************************************************************************************//**
     * @brief   Returns the error code to a string
     * 
     * For DEBUG purpose only
     *
     * @return  A string related to the given error's code.
     ****************************************************************************************************************/
    static const char* error_to_string(uint32_t error) {
        switch (error) {
            case BUFFER_SUCCESS:        return "BUFFER_SUCCESS";
            case BUFFER_ERROR_NULL:     return "BUFFER_ERROR_NULL";
            case BUFFER_ERROR_NO_MEM:   return "BUFFER_ERROR_NO_MEM";
            default:                    return "UNKNOWN ERROR";

        }
    }

private:
    /* ************************************************************************************************************ */
    /*                                                                                                              */
    /* PRIVATE DECLARATION SECTION                                                                                  */
    /*                                                                                                              */
    /* ************************************************************************************************************ */
    // Attributes
    uint8_t _buffer[S];
    size_t  _size;
    size_t  _offset;

};  // Template Class IBuffer
