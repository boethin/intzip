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
#ifndef ___INTZIP_IO_H___
#define ___INTZIP_IO_H___

#include <vector>

namespace intzip {

template<class T>
void read_hex(std::vector<T> &in);

template<class T>
void read_hex(const char *path, std::vector<T> &in);

template<class T>
void read_bin(std::vector<T> &in);

template<class T>
void write_hex(const std::vector<T> &out);

template<class T>
void write_hex(const char *path, const std::vector<T> &out);

}

#endif
