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

#include <cstdlib> // EXIT_SUCCESS
#include <cstdio> // perror
#include <iostream>

#include "intzip-stdint.h"
#include "intzip-def.h"
#include "intzip-io.h"
#include "intzip-options.h"
#include "intzip.h"

using namespace std;

#ifdef ENABLE_TRACE
bool enable_trace = false; // extern
#endif

template<class T>
static ___inline__( void read(const intzip::options &cmd, vector<T> &in) );

template<class T>
static ___inline__( void write(const intzip::options &cmd, vector<T> &out) );

template<class T>
static ___inline__( void process(const intzip::options &cmd) );


int main(int argc, char** argv)
{
  string line;
  vector<uint32_t> in, out;
  intzip::options cmd;
  
  cmd.parse(argc,argv);
  
  if (cmd.usage) { // display usage and exit
    cout << cmd.get_usage() << endl;
    return 1;
  }

  if (cmd.version) { // display version and exit
    cout << cmd.get_version() << endl;
    return EXIT_SUCCESS;
  }

#ifdef ENABLE_TRACE
  enable_trace = cmd.trace;
#endif

  switch (cmd.type)
  {
    case intzip::U16:
      process<uint16_t>(cmd);
      break;
    case intzip::U32:
      process<uint32_t>(cmd);
      break;
    case intzip::U64:
      process<uint64_t>(cmd);
      break;
  }

  return EXIT_SUCCESS;
}

template<class T>
void read(const intzip::options &cmd, vector<T> &in)
{
  if (cmd.binary)
  {
    if (cmd.infile) {
      intzip::read_bin(cmd.infile,in);
    }
    else {
      intzip::read_bin(in);
    }
  }
  else
  {
    if (cmd.infile)
      intzip::read_hex(cmd.infile,in);
    else
      intzip::read_hex(in);
  }
}

template<class T>
void write(const intzip::options &cmd, vector<T> &out)
{
  if (cmd.binary)
  {
    if (cmd.outfile) {
      intzip::write_bin(cmd.outfile,out);
    }
    else {
      intzip::write_bin(out);
    }
  }
  else
  {
    if (cmd.outfile) {
      intzip::write_hex(cmd.outfile,out);
    }
    else {
      intzip::write_hex(out);
    }
  }
}

template<class T>
void process(const intzip::options &cmd)
{
  vector<T> in, out;
  
  read(cmd,in);

  try {
    switch (cmd.action)
    {
      case intzip::options::ENCODE:
        intzip::encode(in,out);
        break;
      case intzip::options::DECODE:
        intzip::decode(in,out);
        break;
      case intzip::options::CONTAINS:
        intzip::contains(in,intzip::scan_hex<T>(cmd.testval));
        break;
    }
  }
  catch (const char* msg) {
    cerr << msg << endl;
    exit(EXIT_FAILURE);
  }

  write(cmd,out);
}
