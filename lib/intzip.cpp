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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

// verbose encoding tracing to stderr
#ifdef ENABLE_TRACE
#define __STDC_LIMIT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <stdarg.h>

extern bool enable_trace;

template<class T> struct chunk;
static void printf_tracelog(const char *mark, const char* log);
static void printf_tracelog_args(const char *mark, const char* format, ...);
template<class T> static void printf_tracelog_args(const char *mark, const chunk<T> &chunk, const char* format, ...);
template<class T> static void printf_tracelog_args(const char *mark, const chunk<T> &chunk);

#define TRACE(...) printf_tracelog_args ( __VA_ARGS__ )
#else
#define TRACE(...)
#endif

#include "def.h"
#include "intzip.h"

using namespace std;

// template specializations
namespace intzip {

  // uint32_t
  template void encode(const vector<uint32_t> &in, vector<uint32_t> &enc);
  template void decode(const vector<uint32_t> &enc, vector<uint32_t> &out);

}

template<class T>
struct chunkdata {

  // maximum value of T
  static ___always_inline__(___const__( T maxval(void) ));
  
  // bitsize of T, i.e. sizeof(T)*8
  static ___always_inline__(___const__( uint8_t bitsize(void) ));

  // Bits need for a bit length value, i.e. ceil_log2( sizeof(T)*8 )
  static ___always_inline__(___const__( uint8_t lengthbits(void) ));

  chunkdata(T init = 0)
    : len(0),
      first(init),
      base(maxval()),
      maxdiff(0),
      bits(0)
  {}

  T len;
  T first;
  T base;
  T maxdiff;
  uint8_t bits;
};

template<> uint32_t chunkdata<uint32_t>::maxval() { return UINT32_MAX; }
template<> uint64_t chunkdata<uint64_t>::maxval() { return UINT64_MAX; }

template<> uint8_t chunkdata<uint32_t>::bitsize() { return 32; }
template<> uint8_t chunkdata<uint64_t>::bitsize() { return 64; }

template<> uint8_t chunkdata<uint32_t>::lengthbits() { return 6; }
template<> uint8_t chunkdata<uint64_t>::lengthbits() { return 7; }

// The bit cost of number encoding
template<class T>
static ___inline__(
  int encode_cost(const T val)
);

/* Compress and append an integer */
template<class T>
static ___inline__(
  void encode_append(const T val, vector<T> &enc, size_t &i, uint8_t &off)
);

template<class T>
static ___inline__(
  void encode_append(const T val, const uint8_t bits, vector<T> &enc, size_t &i, uint8_t &off)
);

/* Fetch and decompress a bit-compressed integer */
template<class T>
static ___inline__(
  T decode_fetch(const vector<T> enc, size_t &i, uint8_t &off)
);

template<class T>
static ___inline__(
  T decode_fetch(uint8_t bits, const vector<T> enc, size_t &i, uint8_t &off)
);


/* Calculate the need of bits for an integer */
template<class T>
static ___inline__(___const__( int ceil_log2(T x) ));

/* Whether or not an integer is a power of 2 */
template<class T>
static ___inline__(___const__( bool is_power2(T x) ));


/* Delta Chunk Algorithm */
template<class T>
struct chunk : public chunkdata<T> {

  // default constructor
  chunk(void)
    : chunkdata<T>(),
      cost(0),
      cost_base(0) // uninitialized
  {}

  chunk(T init)
    : chunkdata<T>(init),
      cost(0),
      cost_base( encode_cost<T>(init) )
  {}

  uint64_t ___const__( calculate_cost(void) const )
  {
    return this->cost_base
      + encode_cost<T>(this->len)
      + encode_cost<T>(this->base)
      + this->len * this->bits;
  }

  chunk ___const__( next(T diff) const )
  {
    bool chg = false;
    chunk n(*this);
    n.len++;

    if (diff < n.base)
      n.base = diff, chg = true;

    if (diff > n.maxdiff)
      n.maxdiff = diff, chg = true;

    if (chg) { // re-calculate bits need, cost
      n.bits = ceil_log2<T>(n.maxdiff - n.base); //, n.cost = n.calculate_cost();
    }
//     else {
//       n.cost += n.bits;
//       if (is_power2(n.len)) // increase length bits
//         n.cost++;
//     }
    n.cost = n.calculate_cost();

    return n;
  }

  static chunk delta(const vector<T> &in, typename vector<T>::const_iterator it)
  {
    assert(!in.empty());
    
    T p = *it;
    chunk c(p), c21, c22;
    TRACE("|start","%#08x",p);

    // handle singleton
    if (++it == in.end()) {
      c.base = 0;
      c.bits = ceil_log2<T>(p);
      c.cost = c.cost_base + 1;
      return c;
    }

    for ( ; it != in.end(); it++)
    {
      // what would be the next
      chunk n = c.next(*it - p), n2;

      if (!(*it > p)) {
        throw "A strictly increasing list is required";
      }

      if (c22.cost_base) {
        n2 = c22.next(*it - p);
      }

      if (n.bits != c.bits && (c.len > 2 && in.end() - it > 2))
      {
        // current sequence may be worth splitting at an earlier stage

        // break immediately when an equidistant sequence ends
        if (c.bits == 0) {
          TRACE("-> equidist",c);
          return c;
        }

        if (c21.cost_base)
        {
          // check previous delta point
          TRACE(" :check",c,"(%u + %u = %u vs. %u)",c21.cost,c22.cost,c21.cost + c22.cost,c.cost);
          if (c21.cost + c22.cost < c.cost) {
            // breaking at the last delta point would be cheaper
            TRACE("-> break",c21);
            return c21;
          }
        }

        // remember this delta point
        TRACE(" :remember",c);
        c21 = c, c22 = chunk(*it), n2.cost_base = 0;
      }

      c = n;
      if (n2.cost_base) {
        c22 = n2;
      }
      
      p = *it;
    }

    // end of list
    if (c.len <= 4) {
      return c;
    }

    if (c21.cost_base)
    {
      // check whether we should break at the last delta point
      TRACE(" :check",c,"(%u + %u = %u vs. %u)",c21.cost,c22.cost,c21.cost + c22.cost,c.cost);
      if (c21.cost + c22.cost < c.cost) {
        // breaking at the last delta point would be cheaper
        TRACE("-> earlier",c21);
        return c21;
      }
    }

    TRACE("-> end",c);
    return c;
  }

  static void encode_header(const chunkdata<T> &c, vector<T> &enc, size_t &i, uint8_t &off)
  {
    encode_append(c.len,enc,i,off);
    encode_append(c.first,enc,i,off);
    encode_append(c.base,enc,i,off);
    encode_append((T)c.bits,chunkdata<T>::lengthbits(),enc,i,off);
  }

  static chunkdata<T> decode_header(const vector<T> &enc, size_t &i, uint8_t &off)
  {
    assert(i < enc.size());

    chunkdata<T> c;
    const uint8_t lb = chunkdata<T>::lengthbits();

    c.len = decode_fetch(enc,i,off);
    c.first = decode_fetch(enc,i,off);
    c.base = decode_fetch(enc,i,off);
    c.bits = (uint8_t)decode_fetch(lb,enc,i,off);
    return c;
  }

#ifdef ENABLE_TRACE
  void to_string(char buf[]) const;
#endif

  // The cost value
  uint64_t cost;

private:
  int cost_base;
};

template<class T>
void intzip::encode(const vector<T> &in, vector<T> &enc)
{
  size_t i = 0;
  uint8_t enc_off = 0;
#ifdef ENABLE_TRACE
  uint64_t total_cost = 0;
#endif

  if (in.empty()) // empty input
    return;

  // append chunks
  for (typename vector<T>::const_iterator it = in.begin(); it != in.end(); )
  {
    chunk<T> c = chunk<T>::delta(in,it);
    TRACE(">> encode",c);

    chunk<T>::encode_header(c,enc,i,enc_off);
    if (c.bits > 0)
    {
      size_t k;
      T p = *it++;
      for (k = 0; k < c.len; k++)
        encode_append(*it - p - c.base, c.bits, enc, i, enc_off), p = *it++;
    }
    else
    {
      it += c.len + 1;
    }

#ifdef ENABLE_TRACE
    total_cost += c.calculate_cost();
#endif

  }
  TRACE("total","cost=%lu",total_cost);
}

template<class T>
void intzip::decode(const vector<T> &enc, vector<T> &out)
{
  size_t i = 0, t = 0;
  uint8_t off = 0;
  T p = 0;

  if (enc.empty()) // empty input
    return;

  for (t = 0; i < enc.size(); t++)
  {
    size_t k;
    
    chunkdata<T> c = chunk<T>::decode_header(enc,i,off);
    if ((c.first == 0 && t > 0)) // halt condition: c.first == 0
      break;
      
    out.push_back(p = c.first); 
    if (c.bits > 0)
    {
      for (k = 0; k < c.len; k++)
        out.push_back(p = p + c.base + decode_fetch(c.bits, enc, i, off));
    }
    else // equidistant seq.
    {
      for (k = 0; k < c.len; k++)
        out.push_back(p = p + c.base);
    }
  }
}


template<class T>
int encode_cost(const T val)
{
  const int bs = chunkdata<T>::bitsize(), lb = bs % 7, u = bs - 7;
  T b = 1;

  for (int c = 0, s = 0; s < bs; c += 8, s += 7)
  {
    if (s > u)
      return c + lb;
    if (val < (b <<= 7))
      return c + 8;
  }

  assert(0); // never reach this point
  return 0;
}

template<class T>
void encode_append(const T val, vector<T> &enc, size_t &i, uint8_t &off)
{
  // number encoding:
  //
  // An integer is splitted into 7-bit blocks where each block is extended to 8 bit
  // by an additional forward-bit that determines whether or not there are more
  // blocks left. Thus, a number n with 0 <= n < 2^7 takes 8 bit, while in case of
  // 2^7 <= n < 2^14 it takes 16 bit and so forth. 32-bit values above 2^28
  // take 36 bit because of 4 forward-bits.
  
  const int bs = chunkdata<T>::bitsize(), lb = bs % 7, u = bs - 7;
  T b = 1;

  for (int s = 0; s < bs; s += 7)
  {
    T t = (val >> s) & 0x7F; // right-most 7 bit
    if (s > u) {
      encode_append(t,lb,enc,i,off); // last block
      return;
    }
    if (val < (b <<= 7)) {
      encode_append(t,8,enc,i,off); // enough blocks
      return;
    }
    t |= 0x80; // set forward bit
    encode_append(t,8,enc,i,off);
  }

  assert(0); // never reach this point
}

template<class T>
void encode_append(const T val, const uint8_t bits, vector<T> &enc, size_t &i, uint8_t &off)
{
  // assume: 0 <= off < bitsize, 0 < bits <= bitsize
  assert(off <= chunkdata<T>::bitsize());
  assert(bits > 0);
  assert(bits <= chunkdata<T>::bitsize());

  const uint8_t bs = chunkdata<T>::bitsize(), len = off + bits;

  if (enc.empty())
    enc.push_back(0);
  T *last = &enc.back();

  if (len < bs)
  {
    *last |= (val << (bs - len));
    off += bits;
  }
  else if (len > bs)
  {
    *last |= (val >> (len - bs));
    enc.push_back(val << (2*bs - len));
    off = len % bs;
  }
  else {
    *last |= val;
    off = bs;
  }
}

template<class T>
T decode_fetch(const vector<T> enc, size_t &i, uint8_t &off)
{
  const int bs = chunkdata<T>::bitsize(), lb = bs % 7, u = bs - 7;
  T val = 0;

  for (int s = 0; s < bs; s += 7)
  {
    int bits = s <= u ? 8 : lb;
    T t = decode_fetch(bits,enc,i,off);
    val |= ((t & 0x7F) << s);
    if (!(t & 0x80))
      return val;
  }

  assert(0); // never reach this point
  return 0;
}

template<class T>
T decode_fetch(const uint8_t bits, const vector<T> enc, size_t &i, uint8_t &off)
{
  // assume: 0 <= off < bs, 0 < bits <= bitsize
  assert(off <= chunkdata<T>::bitsize());
  assert(bits > 0);
  assert(bits <= chunkdata<T>::bitsize());

  if (i >= enc.size())
    return 0;

  const uint8_t bs = chunkdata<T>::bitsize(), len = off + bits;
  T val = enc[i] << off;

  if (len > bs)
  {
    if (i >= enc.size() - 1)
      return 0;
    val |= enc[++i] >> (bs - off);
  }
  else if (len == bs)
  {
    i++;
  }

  off = len % bs;
  return val >> (bs - bits);
}

template<>
int ceil_log2(uint32_t n)
{
  // prefer builtin
#ifdef builtin_ceil_log2_uint32
  return builtin_ceil_log2_uint32(n);
#else
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

template<>
int ceil_log2(uint64_t n)
{
  // prefer builtin
#ifdef builtin_ceil_log2_uint64
  return builtin_ceil_log2_uint64(n);
#else
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

template<class T>
bool is_power2(T x)
{
  return x && !(x & (x - 1));
}

// -- TRACE --

#ifdef ENABLE_TRACE

template<class T>
void chunk<T>::to_string(char buf[]) const
{
  sprintf(buf,"%#08x [base=%u, maxdiff=%u, %lu*%d bits: cost=%u]",
    this->first,this->base,this->maxdiff,this->len,this->bits,this->calculate_cost());
}

void printf_tracelog(const char *mark, const char* log)
{
  if (enable_trace)
    fprintf(stderr,"# %-12s %s\n",mark,log);
}

void printf_tracelog_args(const char *mark, const char* format, ...)
{
  va_list args;
  char buf[0x1000];

  va_start(args, format);
  vsprintf(buf, format, args);
  va_end(args);

  printf_tracelog(mark,buf);
}

template<class T>
void printf_tracelog_args(const char *mark, const chunk<T> &chunk, const char* format, ...)
{
  va_list args;
  char buf[0x1000], buf2[0x1000], buf3[0x1000];

  chunk.to_string(buf);

  va_start(args, format);
  vsprintf(buf2, format, args);
  va_end(args);

  sprintf(buf3,"%s %s",buf,buf2);
  
  printf_tracelog(mark,buf3);
}

template<class T>
void printf_tracelog_args(const char *mark, const chunk<T> &chunk)
{
  char buf[0x1000];

  chunk.to_string(buf);
  printf_tracelog(mark,buf);
}

#endif
