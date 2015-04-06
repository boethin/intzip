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

#include <string>
#include <iostream>
#include <fstream>

#include "def.h"
#include "io.h"
#include "intzip.h"

#define BUFSIZE 0x10000

using namespace std;

namespace intzip {

  // uint32_t
  template void read_hex(std::vector<uint32_t> &in);
  template void read_hex(const char *path, std::vector<uint32_t> &in);
  template void read_bin(std::vector<uint32_t> &in);
  template void write_hex(const vector<uint32_t> &out);
  template void write_hex(const char *path, const vector<uint32_t> &out);

}

template<class T>
static ___always_inline__(___const__( T unpack(const char *n) ));

template<class T>
static ___always_inline__(___const__( T scan_hex(const char *s) ));

template<class T>
static ___always_inline__(___pure__( void internal_read_hex(istream &is, vector<T> &in) ));

template<class T>
static ___always_inline__(___pure__( void internal_write_hex(ostream &os, const vector<T> &out) ));

template<>
uint32_t scan_hex(const char *s)
{
  uint32_t u;
  sscanf(s,"%x",&u);
  return u;
}

template<>
uint32_t unpack(const char *n)
{
  return
    ( (n[0] << 24) & 0xFF000000) |
    ( (n[1] << 16) & 0xFF0000) |
    ( (n[2] << 8)  & 0xFF00 ) |
    ( (n[3]        & 0xFF) );
}

template<class T>
void internal_read_hex(istream &is, vector<T> &in)
{
  string line;
  while (getline(is,line))
  {
    in.push_back(scan_hex<T>(line.c_str()));
  }
}

template<class T>
void intzip::read_hex(vector<T> &in)
{
  internal_read_hex(cin,in);
}

template<class T>
void intzip::read_hex(const char *path, vector<T> &in)
{
  ifstream infile(path);
  internal_read_hex(infile,in);
}

template<class T>
void intzip::read_bin(vector<T> &in)
{
  const int bytes = sizeof(T);
  char buffer[BUFSIZE];
  streamsize gc;
  do {
    cin.read(buffer,BUFSIZE);
    if (0 < (gc = cin.gcount())) {
      for (streamsize i = 0; i < gc - bytes + 1; i += bytes)
        in.push_back(unpack<T>(buffer + i));
    }
  }
  while (gc > 0);
}

template<class T>
void internal_write_hex(ostream &os, const vector<T> &out)
{
  for (typename vector<T>::const_iterator it = out.begin(); it != out.end(); ++it) {
    os << hex << *it << endl;
  }
}

template<class T>
void intzip::write_hex(const vector<T> &out)
{
  internal_write_hex(cout,out);
}

template<class T>
void intzip::write_hex(const char *path, const std::vector<T> &out)
{
  ofstream outfile(path, ios::out);
  internal_write_hex(outfile,out);
}

