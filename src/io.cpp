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

// macros for printf and scanf format specifiers
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "def.h"
#include "io.h"
#include "intzip.h"

#define BUFSIZE 0x10000

using namespace std;

namespace intzip {

  // uint16_t
  template void read_hex(std::vector<uint16_t> &in);
  template void read_hex(const char *path, std::vector<uint16_t> &in);
  template void read_bin(std::vector<uint16_t> &in);
  template void read_bin(const char *path, std::vector<uint16_t> &in);
  template void write_hex(const vector<uint16_t> &out);
  template void write_hex(const char *path, const vector<uint16_t> &out);
  template void write_bin(const vector<uint16_t> &out);
  template void write_bin(const char *path, const vector<uint16_t> &out);

  // uint32_t
  template void read_hex(std::vector<uint32_t> &in);
  template void read_hex(const char *path, std::vector<uint32_t> &in);
  template void read_bin(std::vector<uint32_t> &in);
  template void read_bin(const char *path, std::vector<uint32_t> &in);
  template void write_hex(const vector<uint32_t> &out);
  template void write_hex(const char *path, const vector<uint32_t> &out);
  template void write_bin(const vector<uint32_t> &out);
  template void write_bin(const char *path, const vector<uint32_t> &out);

  // uint64_t
  template void read_hex(std::vector<uint64_t> &in);
  template void read_hex(const char *path, std::vector<uint64_t> &in);
  template void read_bin(std::vector<uint64_t> &in);
  template void read_bin(const char *path, std::vector<uint64_t> &in);
  template void write_hex(const vector<uint64_t> &out);
  template void write_hex(const char *path, const vector<uint64_t> &out);
  template void write_bin(const vector<uint64_t> &out);
  template void write_bin(const char *path, const vector<uint64_t> &out);

}

template<class T>
static ___always_inline__(___pure__( void pack(T n, char buf[]) ));

template<class T>
static ___always_inline__(___const__( T unpack(const char *n) ));

template<class T>
static ___always_inline__(___const__( T scan_hex(const char *s) ));

template<class T>
static ___always_inline__(___pure__( void internal_read_hex(istream &is, vector<T> &in) ));

template<class T>
static ___always_inline__(___pure__( void internal_read_bin(istream &is, vector<T> &in) ));

template<class T>
static ___always_inline__(___pure__( void internal_write_hex(ostream &os, const vector<T> &out) ));

template<class T>
static ___always_inline__(___pure__( void internal_write_bin(ostream &os, const vector<T> &out) ));

template<>
uint16_t scan_hex(const char *s)
{
  uint16_t u;
  sscanf(s,"%" SCNx16,&u);
  return u;
}

template<>
uint32_t scan_hex(const char *s)
{
  uint32_t u;
  sscanf(s,"%" SCNx32,&u);
  return u;
}

template<>
uint64_t scan_hex(const char *s)
{
  uint64_t u;
  sscanf(s,"%" SCNx64,&u);
  return u;
}

template<>
void pack(uint16_t n, char buf[])
{
  buf[0] = (n & 0xFF00) >> 8;
  buf[1] = (n & 0xFF);
}

template<>
void pack(uint32_t n, char buf[])
{
  buf[0] = (n & 0xFF000000) >> 24;
  buf[1] = (n & 0xFF0000) >> 16;
  buf[2] = (n & 0xFF00) >> 8;
  buf[3] = (n & 0xFF);
}

template<>
void pack(uint64_t n, char buf[])
{
  buf[0] = (n & 0xFF00000000000000) >> 56;
  buf[1] = (n & 0xFF000000000000) >> 48;
  buf[2] = (n & 0xFF0000000000) >> 40;
  buf[3] = (n & 0xFF00000000) >> 32;
  buf[4] = (n & 0xFF000000) >> 24;
  buf[5] = (n & 0xFF0000) >> 16;
  buf[6] = (n & 0xFF00) >> 8;
  buf[7] = (n & 0xFF);
}

template<>
uint16_t unpack(const char *n)
{
  return
    ( ((uint16_t)n[0] << 8)  & 0xFF00 ) |
    ( ((uint16_t)n[1]        & 0xFF ) );
}

template<>
uint32_t unpack(const char *n)
{
  return
    ( ((uint32_t)n[0] << 24) & 0xFF000000 ) |
    ( ((uint32_t)n[1] << 16) & 0xFF0000 ) |
    ( ((uint32_t)n[2] << 8)  & 0xFF00 ) |
    ( ((uint32_t)n[3]        & 0xFF) );
}

template<>
uint64_t unpack(const char *n)
{
  return
    ( ((uint64_t)n[0] << 56) & 0xFF00000000000000 ) |
    ( ((uint64_t)n[1] << 48) & 0xFF000000000000 ) |
    ( ((uint64_t)n[2] << 40) & 0xFF0000000000 ) |
    ( ((uint64_t)n[3] << 32) & 0xFF00000000 ) |
    ( ((uint64_t)n[4] << 24) & 0xFF000000) |
    ( ((uint64_t)n[5] << 16) & 0xFF0000) |
    ( ((uint64_t)n[6] << 8)  & 0xFF00 ) |
    ( ((uint64_t)n[7]        & 0xFF) );
}

// -- read --

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
  infile.close();
}

template<class T>
void internal_read_bin(istream &is, vector<T> &in)
{
  char buffer[BUFSIZE];
  streamsize gc;
  do {
    is.read(buffer,BUFSIZE);
    if (0 < (gc = is.gcount())) {
      for (streamsize i = 0; i < (streamsize)(gc - sizeof(T) + 1); i += sizeof(T))
        in.push_back(unpack<T>(buffer + i));
    }
  }
  while (gc > 0);
}

template<class T>
void intzip::read_bin(vector<T> &in)
{
  internal_read_bin(cin,in);
}

template<class T>
void intzip::read_bin(const char *path, vector<T> &in)
{
  ifstream infile(path);
  internal_read_bin(infile,in);
  infile.close();
}

// -- write --

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
  outfile.close();
}

template<class T>
void internal_write_bin(ostream &os, const vector<T> &out)
{
  char buf[sizeof(T)];
  for (typename vector<T>::const_iterator it = out.begin(); it != out.end(); ++it) {
    pack(*it,buf);
    os.write(buf,sizeof(T));
  }
}

template<class T>
void intzip::write_bin(const std::vector<T> &out)
{
  internal_write_bin(cout,out);
}

template<class T>
void intzip::write_bin(const char *path, const std::vector<T> &out)
{
  ofstream outfile(path, ios::out | ios::binary);
  internal_write_bin(outfile,out);
  outfile.close();
}

