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
#ifndef ___INTZIP_UINT_H___
#define ___INTZIP_UINT_H___

#include "intzip-stdint.h"
#include "intzip-def.h"

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

namespace intzip {

// uint*_t related
template<typename T>
struct uint {

  // maximum value of T
  static ___always_inline__(___const__( T maxval(void) ));

  // bitsize of T, i.e. sizeof(T)*8
  static ___always_inline__(___const__( uint8_t bitsize(void) ));

  // Bits need for a bit length value, i.e. ceil_log2( sizeof(T)*8 )
  static ___always_inline__(___const__( uint8_t lengthbits(void) ));

  // Calculate the need of bits for an integer
  static ___always_inline__(___const__( int ceil_log2(T x) ));
  
  // Whether or not an integer is a power of 2
  static ___always_inline__(___const__( bool is_power2(T x) ))
  {
    return x && !(x & (x - 1));
  }

  // Create a bitmask
  static ___const__( T bitmask(uint8_t len, uint8_t lshift = 0) )
  {
    assert(len > 0);
    assert(len <= bitsize());
    assert(len + lshift <= maxval());
    return len < bitsize() ? ~(maxval() << len) << lshift : maxval();
  }

};

template<> uint16_t inline uint<uint16_t>::maxval() { return UINT16_MAX; }
template<> uint32_t inline uint<uint32_t>::maxval() { return UINT32_MAX; }
template<> uint64_t inline uint<uint64_t>::maxval() { return UINT64_MAX; }

template<> uint8_t inline uint<uint16_t>::bitsize() { return 16; }
template<> uint8_t inline uint<uint32_t>::bitsize() { return 32; }
template<> uint8_t inline uint<uint64_t>::bitsize() { return 64; }

template<> uint8_t inline uint<uint16_t>::lengthbits() { return 5; }
template<> uint8_t inline uint<uint32_t>::lengthbits() { return 6; }
template<> uint8_t inline uint<uint64_t>::lengthbits() { return 7; }

template<> int inline uint<uint32_t>::ceil_log2(uint32_t n)
{
#ifdef builtin_ceil_log2_uint32 // prefer builtin
  return builtin_ceil_log2_uint32(n);
#else // fallback
  int bits = 0;
  if (n > 0xFFFF)
    n >>= 16,  bits = 0x10;
  if (n > 0xFF)
    n >>= 8, bits |= 8;
  if (n > 0xF)
    n >>= 4, bits |= 4;
  if (n > 3)
    n >>= 2, bits |= 2;
   if (n > 1)
     bits |= 1;
  if (n > 0)
    bits += 1;
  return bits;
#endif
}

template<> int inline uint<uint64_t>::ceil_log2(uint64_t n)
{
#ifdef builtin_ceil_log2_uint64 // prefer builtin
  return builtin_ceil_log2_uint64(n);
#else // fallback
  static const uint64_t t[6] = {
    0xFFFFFFFF00000000,
    0x00000000FFFF0000,
    0x000000000000FF00,
    0x00000000000000F0,
    0x000000000000000C,
    0x0000000000000002
  };

  if (n > 0)
  {
    int y = (((n & (n - 1)) == 0) ? 0 : 1), j = 32, i;
    for (i = 0; i < 6; i++) {
      int k = (((n & t[i]) == 0) ? 0 : j);
      y += k, n >>= k, j >>= 1;
    }
    return y;
  }
  return 0;
#endif
}

template<> int inline uint<uint16_t>::ceil_log2(uint16_t n)
{
  return uint<uint32_t>::ceil_log2((uint32_t)n);
}

}

#endif
