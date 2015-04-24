/**
 * This file is part of intzip.
 *
 * Copyright (C) 2015 Sebastian Boethin <boethin@xn--domain.net>
 *
 * intzip is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * intzip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with intzip.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef ___INTZIP_BITSTORE_H___
#define ___INTZIP_BITSTORE_H___

#include "intzip-stdint.h"
#include "intzip-def.h"
#include "intzip-uint.h"
#include "intzip-bitnumber.h"

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

namespace intzip {

//
// abstract base class for bit_writer, bit_reader
//
template<class S>
class bitstore {
protected:

  bitstore(S &store)
    : store(store), offset(0)
  {}
  
  virtual ~bitstore() {}

  S &store;
  uint8_t offset;
};

//
// bit_writer
//
template<typename T, class S>
class bit_writer : public bitstore<S> {
public:

  bit_writer(S &store) : bitstore<S>(store) {}
  
  virtual ~bit_writer() {}

  void append(const T val)
  {
    bitnumber<T>::append(val,*this);
  }

  void append(const T val, const uint8_t bits)
  {
    // assume: 0 <= this->offset < bitsize, 0 < bits <= bitsize
    assert(this->offset <= uint<T>::bitsize());
    assert(bits > 0);
    assert(bits <= uint<T>::bitsize());

    const uint8_t bs = uint<T>::bitsize(), len = this->offset + bits;

    if (len < bs)
    {
      this->append_bits(val << (bs - len));
      this->offset += bits;
    }
    else if (len > bs)
    {
      this->append_bits(val >> (len - bs));
      this->push_bits(val << (2*bs - len));
      this->offset = len % bs;
    }
    else {
      this->append_bits(val);
      this->offset = bs;
    }
  }

protected:
  // implementation depends on class S
  virtual void append_bits(T val) = 0;
  virtual void push_bits(T val) = 0;
};

//
// bit_reader
//
template<typename T, class S>
class bit_reader : public bitstore<const S> {
public:

  bit_reader(const S &store) : bitstore<const S>(store) {}
  
  virtual ~bit_reader() {}

  T fetch(void)
  {
    return bitnumber<T>::fetch(*this);
  }

  T fetch(const uint8_t bits)
  {
    // assume: 0 <= this->offset < bitsize, 0 < bits <= bitsize
    assert(this->offset <= uint<T>::bitsize());
    assert(bits > 0);
    assert(bits <= uint<T>::bitsize());

    if (this->ended())
      return 0;

    const uint8_t bs = uint<T>::bitsize(), len = this->offset + bits;
    T val = this->current() << this->offset;

    if (len > bs)
    {
      if (!this->more())
        return 0;
      val |= this->next() >> (bs - this->offset);
    }
    else if (len == bs)
    {
      this->inc();
    }

    this->offset = len % bs;
    return val >> (bs - bits);
  }

protected:
  // implementation depends on class S
  virtual bool ended(void) const = 0;
  virtual T current(void) const = 0;
  virtual bool more(void) const = 0;
  virtual T next(void) = 0;
  virtual void inc(void) = 0;
};

} // namespace intzip

#endif
