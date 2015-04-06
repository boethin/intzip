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

#include <iostream>

#include "def.h"
#include "io.h"
#include "options.h"
#include "intzip.h"

using namespace std;

int main(int argc, char** argv)
{
  string line;
  vector<uint32_t> in, out;
  intzip::options cmd;
  
  cmd.parse(argc,argv);
  
  if (cmd.usage) {
    // display usage and exit
    cout << cmd.get_usage() << endl;
    return 1;
  }

  if (cmd.version) {
    // display version and exit
    cout << cmd.get_version() << endl;
    return 0;
  }

  if (cmd.binary)
  {
    intzip::read_bin(in);
  }
  else
  {
    if (cmd.infile)
      intzip::read_hex(cmd.infile,in);
    else
      intzip::read_hex(in);
  }

  try {
    if (cmd.compress) {
      intzip::encode<uint32_t>(in,out);
    }
    else {
      intzip::decode<uint32_t>(in,out);
    }
  }
  catch (const char* msg) {
    cerr << msg << endl;
    return 1;
  }
  
  if (cmd.outfile) {
    intzip::write_hex(cmd.outfile,out);
  }
  else {
    intzip::write_hex(out);
  }
  
  return 0;
}

