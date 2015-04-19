#ifndef ___INTZIP_CHUNK_H___
#define ___INTZIP_CHUNK_H___

#include <vector>

#include "intzip-stdint.h"
#include "intzip-trace.h"
#include "intzip-def.h"
#include "intzip-uint.h"
#include "intzip-bitnumber.h"
#include "intzip-bitstore.h"

using namespace std;

namespace intzip {

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

  T calculate_cost(void) const
  {
    return this->cost_base
      + bitnumber<T>::cost(this->len)
      + bitnumber<T>::cost(this->base)
      + this->len * this->bits;
  }

private:
  chunk next(T diff) const
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

public:
  static chunk delta(const vector<T> &in, typename vector<T>::const_iterator it);

  template<class S>
  static inline void encode_header(const chunkdata<T> &c, bit_writer<T,S> &appender)
  {
    appender.append(c.len);
    appender.append(c.first);
    appender.append(c.base);
    appender.append((T)c.bits,uint<T>::lengthbits());
  }

  template<class S>
  static inline chunkdata<T> decode_header(bit_reader<T,S> &reader)
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

}

#endif
