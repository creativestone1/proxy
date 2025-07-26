#pragma once
#include <stdint.h>
#include <string.h>
template <typename _T> class CBuffer {
public:
  ~CBuffer() { delete[] _buffer; }
  CBuffer() : _buffer(nullptr), _capacity(0), _length(0) {}
  void init(size_t capacity) {
    _buffer = new _T[capacity]();
    _capacity = capacity;
  }
  _T *get() const { return _buffer; }
  size_t length() const { return _length; }
  size_t capacity() const { return _capacity; }
  void addLength(size_t l) { _length += l; }
  void removeLength(size_t l) {
    if (l >= _length) {
      _length = 0;
      return;
    }

    memmove(_buffer, &_buffer[l], _length - l);

    _length -= l;
  }

protected:
  _T *_buffer;
  size_t _capacity;
  size_t _length;
};
