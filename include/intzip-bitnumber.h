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
#ifndef ___INTZIP_BITNUMBER_H___
#define ___INTZIP_BITNUMBER_H___

#include "intzip-stdint.h"
#include "intzip-def.h"
#include "intzip-uint.h"

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

namespace intzip {

template<typename T, class S>
struct bit_writer;

template<typename T, class S>
struct bit_reader;

template<typename T>
struct bitnumber {

  static ___optimize__( int cost(const T val) )
  {
    const int bs = uint<T>::bitsize(), lb = bs % 7, u = bs - 7;
    T b = 1;

    // loop expected to be unrolled
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

  template<class S>
  static ___optimize__( void append(const T val, bit_writer<T,S> &writer) )
  {
    // number encoding:
    //
    // An integer is splitted into 7-bit blocks where each block is extended to 8 bit
    // by an additional forward-bit that determines whether or not there are more
    // blocks left. Thus, a number n with 0 <= n < 2^7 takes 8 bit, while in case of
    // 2^7 <= n < 2^14 it takes 16 bit and so forth. 32-bit values above 2^28
    // take 36 bit because of 4 forward-bits.

    const int bs = uint<T>::bitsize(), lb = bs % 7, u = bs - 7;
    T b = 1;

    // loop expected to be unrolled
    for (int s = 0; s < bs; s += 7)
    {
      T t = (val >> s) & 0x7F; // right-most 7 bit
      if (s > u) {
        writer.append(t,lb); // last block
        return;
      }
      if (val < (b <<= 7)) {
        writer.append(t,8); // enough blocks
        return;
      }
      t |= 0x80; // set forward bit
      writer.append(t,8);
    }

    assert(false); // never reach this point
  }

  template<class S>
  static ___optimize__( T fetch(bit_reader<T,S> &reader) )
  {
    const int bs = uint<T>::bitsize(), lb = bs % 7, u = bs - 7;
    T val = 0;

    // loop expected to be unrolled
    for (int s = 0; s < bs; s += 7)
    {
      int bits = s <= u ? 8 : lb;
      T t = reader.fetch(bits);
      val |= ((t & 0x7F) << s);
      if (!(t & 0x80))
        return val;
    }

    assert(false); // never reach this point
    return 0;
  }

};

} // namespace intzip

#endif
