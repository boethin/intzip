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
#include "intzip.h"

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



// The bit cost of number encoding
template<class T>
static ___inline__(___const__(
  int encode_cost(const T val)
));


template<class T>
struct chunkdata {

  chunkdata(T init = 0)
    : len(0),
      first(init),
      base(uint<T>::maxval()),
      maxdiff(0),
      bits(0)
  {}

  T len;
  T first;
  T base;
  T maxdiff;
  uint8_t bits;
};


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
      cost_base(bitnumber<T>::cost(init))
  {}

  T ___const__( calculate_cost(void) const )
  {
    return this->cost_base
      + bitnumber<T>::cost(this->len)
      + bitnumber<T>::cost(this->base)
      + this->len * this->bits;
  }

  chunk ___const__( next(T diff) const )
  {
    bool chg = false;
    chunk n(*this); // copy this
    n.len++;

    if (diff < n.base) 
      n.base = diff, chg = true;

    if (diff > n.maxdiff)
      n.maxdiff = diff, chg = true;

    if (chg) { // re-calculate bits need, cost
      n.bits = uint<T>::ceil_log2(n.maxdiff - n.base), n.cost = n.calculate_cost();
    }
    else {
      // cost may be only changed only by len
      if (uint<T>::is_power2(n.len)) {
        n.cost = n.calculate_cost();
      }
      else if (n.bits > 0) {
        n.cost += n.bits;
      }
    }

    assert(n.cost == n.calculate_cost());
    return n;
  }

  static chunk delta(const vector<T> &in, typename vector<T>::const_iterator it)
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

  template<class S>
  static void encode_header(const chunkdata<T> &c, bit_writer<T,S> &appender)
  {
    appender.append(c.len);
    appender.append(c.first);
    appender.append(c.base);
    appender.append((T)c.bits,uint<T>::lengthbits());
  }

  template<class S>
  static chunkdata<T> decode_header(bit_reader<T,S> &reader)
  {
    chunkdata<T> c;
    c.len = reader.fetch();
    c.first = reader.fetch();
    c.base = reader.fetch();
    c.bits = (uint8_t)reader.fetch(uint<T>::lengthbits());
    return c;
  }

#ifdef ENABLE_TRACE
  void to_string(char buf[]) const;
#endif

  // The cost value
  T cost;

private:
  T cost_base;
};


template<class T>
void intzip::encode(const vector<T> &in, vector<T> &enc)
{
  bitvector_writer<T> appender(enc);
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

    chunk<T>::encode_header(c,appender);
    if (c.bits > 0)
    {
      size_t k;
      T p = *it++;
      for (k = 0; k < c.len; k++)
        appender.append((T)(*it - p - c.base), c.bits), p = *it++;
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
  bitvector_reader<T> reader(enc);
  size_t t = 0;
  T p = 0;

  if (enc.empty()) // empty input
    return;

  while (true)
  {
    size_t k;
    chunkdata<T> c = chunk<T>::decode_header(reader);
    if ((c.first == 0 && t > 0)) // halt condition: c.first == 0
      break;
      
    out.push_back(p = c.first); 
    if (c.bits > 0)
    {
      for (k = 0; k < c.len; k++)
        out.push_back(p = p + c.base + reader.fetch(c.bits));
    }
    else // equidistant seq.
    {
      for (k = 0; k < c.len; k++)
        out.push_back(p = p + c.base);
    }
    
    t++;
  }
}


// -- TRACE --

#ifdef ENABLE_TRACE

#define TRACE_BUFSIZE 0x100 // must be large enough for any log line

template<>
void chunk<uint16_t>::to_string(char buf[]) const
{
  sprintf(buf,"%#04" PRIx16 " [base=%" PRIu16 ", maxdiff=%" PRIu16 ", %" PRIu16 "*%" PRId8 " bits: cost=%" PRIu16 "]",
    this->first,this->base,this->maxdiff,this->len,this->bits,this->calculate_cost());
}

template<>
void chunk<uint32_t>::to_string(char buf[]) const
{
  sprintf(buf,"%#08" PRIx32 " [base=%" PRIu32 ", maxdiff=%" PRIu32 ", %" PRIu32 "*%" PRId8 " bits: cost=%" PRIu32 "]",
    this->first,this->base,this->maxdiff,this->len,this->bits,this->calculate_cost());
}

template<>
void chunk<uint64_t>::to_string(char buf[]) const
{
  sprintf(buf,"%#016" PRIx64 " [base=%" PRIu64 ", maxdiff=%" PRIu64 ", %" PRIu64 "*%" PRId8 " bits: cost=%" PRIu64 "]",
    this->first,this->base,this->maxdiff,this->len,this->bits,this->calculate_cost());
}

void printf_tracelog(const char *mark, const char* log)
{
  if (enable_trace)
    fprintf(stderr,"# %-12s %s\n",mark,log);
}

template<>
void printf_tracelog_args(const char *mark, uint16_t n)
{
  char buf[TRACE_BUFSIZE];
  
  sprintf(buf,"%#04" PRIx16,n);
  printf_tracelog(mark,buf);
}

template<>
void printf_tracelog_args(const char *mark, uint32_t n)
{
  char buf[TRACE_BUFSIZE];
  
  sprintf(buf,"%#08" PRIx32,n);
  printf_tracelog(mark,buf);
}

template<>
void printf_tracelog_args(const char *mark, uint64_t n)
{
  char buf[TRACE_BUFSIZE];
  
  sprintf(buf,"%#016" PRIx64,n);
  printf_tracelog(mark,buf);
}

void printf_tracelog_args(const char *mark, const char* format, ...)
{
  va_list args;
  char buf[TRACE_BUFSIZE];

  va_start(args, format);
  vsprintf(buf, format, args);
  va_end(args);

  printf_tracelog(mark,buf);
}

template<class T>
void printf_tracelog_args(const char *mark, const chunk<T> &chunk, const char* format, ...)
{
  va_list args;
  char buf[TRACE_BUFSIZE], buf2[TRACE_BUFSIZE], buf3[TRACE_BUFSIZE];

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
  char buf[TRACE_BUFSIZE];

  chunk.to_string(buf);
  printf_tracelog(mark,buf);
}

#endif
