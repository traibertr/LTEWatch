/* Copyright (c) 2021, HES-SO Valais
 * All rights reserved.
 */
#pragma once

#include <stdint.h>
#include <string>
#include <new>

template<class _Tp, uint8_t MAX_SIZE = 10>
class queue {
public:

  /**
   * @brief Create a queue base on a ringbuffer of `MAX_SIZE` elements.
   *
   * The oldest element is lost if the queue is full.
   * The queue memory is allocated on the stack and filled with null (empty) objects.
   *
   * @param nullObject the null/empty object to indicates the queue is free
   */
  queue(const _Tp & nullObject) :
      _size(0), _pushIndex(0), _popIndex(0), _nullObject(nullObject) {

    // The placement new is needed in order to initialize the pool with empty/null object.
    // The ringbuffer is allocated on the stack
    for(int i = 0; i < MAX_SIZE; i++)
      new(_elementMemory + i * sizeof(_Tp)) serialcon::Msg(0xFF);

    _ringBuffer = (_Tp *)_elementMemory;
  }

  /** @return `true` if no elements are queued */
  bool empty() const {
    return (_size == 0);
  }

  /** @return the actual number of queued elements */
  size_t size() const {
    return _size;
  }

  /** @return the maximum possible elements to be queued */
  inline size_t maxSize() const {
    return MAX_SIZE;
  }

  _Tp front();                                      ///< Access next element (oldest).
  const _Tp front() const;                          ///< Access next element (oldest).

  _Tp back();                                       ///< Access last element (newest).
  const _Tp back() const;                           ///< Access last element (newest).

  void push(const _Tp x);                           ///< Add element on the end of the queue.
  void pop();                                       ///< Remove element from top of the queue (oldest).

protected:
  void construct(_Tp __p, const _Tp& __val) { /*::new(__p) _Tp(__val);*/
  }

  void destroy(_Tp __p) { /*__p->~_Tp();*/
  }

  inline uint8_t prevIndex(const uint8_t & index) const {
    return ((index + maxSize() - 1) % maxSize());
  }
  inline uint8_t nextIndex(const uint8_t & index) const {
    return ((index + 1) % maxSize());
  }

private:

  /** @brief Ringbuffer holding the queued elements. */
  uint8_t _elementMemory[MAX_SIZE * sizeof(_Tp)];
  _Tp * _ringBuffer;

  uint8_t _size;                      ///< Actual count of elements in queue.
  uint8_t _pushIndex;                 ///< Index pointing on the next empty element where to push the next element.
  uint8_t _popIndex;                  ///< Points on element to be unqueued next.

  _Tp _nullObject;                    ///< Null/empty object used when an element is pop
};

template<class _Tp, uint8_t MAX_SIZE>
_Tp queue<_Tp, MAX_SIZE>::front() {
  return _ringBuffer[_popIndex];
}

template<class _Tp, uint8_t MAX_SIZE>
const _Tp queue<_Tp, MAX_SIZE>::front() const {
  return _ringBuffer[_popIndex];
}

template<class _Tp, uint8_t MAX_SIZE>
_Tp queue<_Tp, MAX_SIZE>::back() {
  return _ringBuffer[prevIndex(_pushIndex)];
}

template<class _Tp, uint8_t MAX_SIZE>
const _Tp queue<_Tp, MAX_SIZE>::back() const {
  return _ringBuffer[prevIndex(_pushIndex)];
}

template<class _Tp, uint8_t MAX_SIZE>
void queue<_Tp, MAX_SIZE>::push(const _Tp x) {
  // Checks
  if (!empty()) {
    assert(_pushIndex != _popIndex);            // Overrun. Queue is too small.
  }

  if (_size < MAX_SIZE) {
    _size++;
    _ringBuffer[_pushIndex] = x;
    _pushIndex = nextIndex(_pushIndex);         // Move on. Prevent overflow
  }
}

template<class _Tp, uint8_t MAX_SIZE>
void queue<_Tp, MAX_SIZE>::pop() {
  if (!empty()) {
    _size--;

    // Call destructor
    _ringBuffer[_popIndex] = _nullObject;
    _popIndex = nextIndex(_popIndex);           // Move on. Prevent overflow
  }
}
