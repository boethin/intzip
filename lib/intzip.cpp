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

#include "intzip-stdint.h"
#include "intzip-trace.h"
#include "intzip-def.h"
#include "intzip-uint.h"
#include "intzip-bitnumber.h"
#include "intzip-bitstore.h"
#include "intzip-bitvector.h"
#include "intzip-rlebuf.h"
#include "intzip-chunk.h"
#include "intzip.h"

#include <stdio.h>

using namespace std;
using namespace intzip;

// template specializations
namespace intzip {

  // uint16_t
  template void encode(const vector<uint16_t> &in, vector<uint16_t> &enc);
  template void decode(const vector<uint16_t> &enc, vector<uint16_t> &out);

  // uint32_t
  template void encode(const vector<uint32_t> &in, vector<uint32_t> &enc);
  template void decode(const vector<uint32_t> &enc, vector<uint32_t> &out);

  // uint64_t
  template void encode(const vector<uint64_t> &in, vector<uint64_t> &enc);
  template void decode(const vector<uint64_t> &enc, vector<uint64_t> &out);

}

template<typename T>
chunk<T> chunk<T>::delta(const vector<T> &in, typename vector<T>::const_iterator it)
{
  assert(!in.empty());

  T p = *it;
  chunk c(p), c_left, c_right;
  TRACE("|start",p);

  // handle singleton
  if (++it == in.end()) {
    c.base = 0;
    c.bits = uint<T>::ceil_log2(p);
    c.cost = c.cost_base + 1;
    TRACE("->single",c);
    return c;
  }

  for ( ; it != in.end(); it++)
  {
    // what would be the next
    chunk n = c.next(*it - p), n2;

    if (!(*it > p)) {
      throw "A strictly increasing list is required";
    }

    if (c_right.cost_base) {
      n2 = c_right.next(*it - p);
    }

    if (n.bits > c.bits && (c.len > 0 && in.end() - it > 1) )
    {
      // current sequence may be worth splitting at an earlier stage

      // break immediately when an equidistant sequence ends
      if (c.len > 1 && c.bits == 0) {
        TRACE("-> equidist",c);
        return c;
      }

      if (c_left.cost_base && c_right.cost > 0)
      {
        // check previous delta point
        assert(c_left.cost == c_left.calculate_cost());
        assert(c_right.cost == c_right.calculate_cost());

        TRACE(" :check",c,"(%u + %u = %u vs. %u)",c_left.cost,c_right.cost,c_left.cost + c_right.cost,c.cost);
        if (c_left.cost + c_right.cost < c.cost) {
          // breaking at the last delta point would be cheaper
          TRACE("-> break",c_left);
          return c_left;
        }
      }

      // remember this delta point
      TRACE(" :remember",c);
      c_left = c, c_right = chunk(*it), n2.cost_base = 0;
    }

    c = n;
    if (n2.cost_base) { // move c_right to next
      c_right = n2;
    }

    p = *it;
  }

  // end of list
  if (c_left.cost_base)
  {
    // check whether we should break at the last delta point
    TRACE(" :check",c,"(%u + %u = %u vs. %u)",c_left.cost,c_right.cost,c_left.cost + c_right.cost,c.cost);
    if (c_left.cost + c_right.cost < c.cost) {
      // breaking at the last delta point would be cheaper
      TRACE("-> backtrace",c_left);
      return c_left;
    }
  }

  TRACE("-> end",c);
  return c;
}




template<class T>
void intzip::encode(const vector<T> &in, vector<T> &enc)
{
  bitvector_writer<T> appender(enc);
#if ENABLE_TRACE
  uint64_t total_cost = 0;
#endif

  if (in.empty()) // empty input
    return;

  // append chunks
  for (typename vector<T>::const_iterator it = in.begin(); it != in.end(); )
  {
    chunk<T> c = chunk<T>::delta(in,it);
    TRACE(">> encode",c);

    chunk<T>::encode_header(c,appender);
    if (c.len > 0 && c.bits > 0) {
      // apply RLE buffering
      rlebuf_writer<T,vector<T> >rle_writer(appender,c.len,c.bits);
      T p = *it++;
      for (size_t k = 0; k < c.len; k++) {
        rle_writer.append((T)(*it - p - c.base)), p = *it++;
      }
    }
    else
    {
      it += c.len + 1;
    }

#if ENABLE_TRACE
    total_cost += c.calculate_cost();
#endif

  }
  TRACE("total","cost=%lu",total_cost);
}

template<class T>
void intzip::decode(const vector<T> &enc, vector<T> &out)
{
  bitvector_reader<T> reader(enc);
  size_t t = 0;
  T p = 0;

  if (enc.empty()) // empty input
    return;

  while (true)
  {
    chunkdata<T> c = chunk<T>::decode_header(reader);
    if ((c.first == 0 && t > 0)) // halt condition: c.first == 0
      break;
      
    out.push_back(p = c.first);
    if (c.len > 0)
    {
      if (c.bits > 0)
      {
        // apply RLE buffering
        rlebuf_reader<T,vector<T> >rle_reader(reader,c.len,c.bits);
        for (size_t k = 0; k < c.len; k++) {
          out.push_back(p = p + c.base + rle_reader.fetch());
        }
      }
      else // equidistant seq.
      {
        for (size_t k = 0; k < c.len; k++) {
          out.push_back(p = p + c.base);
        }
      }
    }
    
    t++;
  }
}

template<class T>
bool intzip::contains(const std::vector<T> &enc, const T value)
{

  return false;
}
