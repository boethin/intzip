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
#ifndef ___INTZIP_H___
#define ___INTZIP_H___

#if __cplusplus <= 199711L
// no C++11
// using C Standard Integer Types
#include <stdint.h>
#else
// using std::uint32_t etc.
#include <cstdint>
#endif

#include <cstddef> // std::size_t
#include <vector> // std::vector

namespace intzip {

  template<class T>
  void encode(const std::vector<T> &in, std::vector<T> &enc);

  template<class T>
  void decode(const std::vector<T> &enc, std::vector<T> &out);

  template<class T>
  bool contains(const std::vector<T> &enc, const T test);

}

#endif
