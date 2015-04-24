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
#ifndef ___INTZIP_BITVECTOR_H___
#define ___INTZIP_BITVECTOR_H___

#include "intzip-stdint.h"
#include "intzip-def.h"
#include "intzip-bitstore.h"

#include <vector>

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

namespace intzip {

//
// std::vector<T> implementation of bit_writer, bit_reader
//
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
  std::size_t index;
};

} // namespace intzip

#endif
