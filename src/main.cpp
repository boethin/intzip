// http://blog.fourthbit.com/2013/06/18/creating-an-open-source-program-in-c-with-autotools-part-1-of-2

//  g++ -Wall -c intzip.cpp && g++ -Wall -c io.cpp && g++ -Wall -o test test.cpp intzip.o io.o && ./test

#include <string> // sscanf
#include <cstring> // strcmp
#include <cstdio> // getline
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <ext/stdio_filebuf.h>

#include <stdio.h>

#include "intzip.h"
#include "io.h"
#include "options.h"

using namespace std;


int main(int argc, char** argv)
{

  string line;
  vector<uint32_t> in, out;
  intzip::options cmd;
  
  cmd.parse(argc,argv);
  
  if (cmd.binary)
  {
    intzip::read_stdin_bin(in);
  }
  else
  {
    if (cmd.infile)
      intzip::read_file_hex(cmd.infile,in);
    else
      intzip::read_stdin_hex(in);
  }
  

//   while (getline(cin, line))
//   {
//     uint32_t u;
//     sscanf(line.c_str(),"%X",&u);
//     in.push_back(u);
//   }
  
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
  
  intzip::write_stdout_hex(out);

//   for (std::vector<uint32_t>::iterator it = out.begin() ; it != out.end(); ++it) {
//     cout << hex << *it << endl;
//   }

  return 0;
}

