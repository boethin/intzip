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

#include <vector>

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

#include "intzip-stdint.h"
#include "intzip-def.h"
#include "intzip-uint.h"
#include "intzip-bitnumber.h"

namespace intzip {

template<class S>
struct bitstore {

  bitstore(S &store)
    : store(store)
  {}

protected:
  S &store;
};


template<typename T, class S>
struct bit_writer : public bitstore<S> {

  bit_writer(S &store)
    : bitstore<S>(store), offset(0)
  {}

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

  static ___const__( int number_cost(const T val) )
  {
    const int bs = uint<T>::bitsize(), lb = bs % 7, u = bs - 7;
    T b = 1;

    for (int c = 0, s = 0; s < bs; c += 8, s += 7)
    {
      if (s > u)
        return c + lb;
      if (val < (b <<= 7))
        return c + 8;
    }

    assert(false); // never reach this point
    return 0;
  }

protected:

  // implementation depends on class S
  virtual void append_bits(T val) = 0;
  virtual void push_bits(T val) = 0;

  uint8_t offset;
};

template<typename T, class S>
struct bit_reader : public bitstore<const S> {

  bit_reader(const S &store)
    : bitstore<const S>(store), offset(0)
  {}

  T fetch(void)
  {
    const int bs = uint<T>::bitsize(), lb = bs % 7, u = bs - 7;
    T val = 0;

    for (int s = 0; s < bs; s += 7)
    {
      int bits = s <= u ? 8 : lb;
      T t = this->fetch(bits);
      val |= ((t & 0x7F) << s);
      if (!(t & 0x80))
        return val;
    }

    assert(false); // never reach this point
    return 0;
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

  uint8_t offset;
};


// -- std::vector<T> implementation --

template<typename T>
struct bitvector_writer : public bit_writer<T,std::vector<T> > {

  bitvector_writer(std::vector<T> &store)
    : bit_writer<T,std::vector<T> >(store)
  {}

protected:

  void append_bits(T val)
  {
    if (this->store.empty())
      this->store.push_back(0);
    T *last = &this->store.back();
    *last |= val;
  }
  
  void push_bits(T val)
  {
    this->store.push_back(val);
  }

};

template<typename T>
struct bitvector_reader : public bit_reader<T,std::vector<T> > {

  bitvector_reader(const std::vector<T> &store)
    : bit_reader<T,std::vector<T> >(store), index(0)
  {}

protected:

  bool ended(void) const
  {
    return this->index >= this->store.size();
  }
  
  T current(void) const
  {
    return this->store[this->index];
  }
  
  bool more(void) const
  {
    return this->index < this->store.size() - 1;
  }
  
  T next(void)
  {
    return this->store[++this->index];
  }

  void inc(void)
  {
    this->index++;
  }

private:
  size_t index;
};

} // namespace intzip

#endif
