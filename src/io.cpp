#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <fstream>

// default and compiler dependant definitions
#include "def.h"
#include "gcc.h"
#include "clang.h"

#include "io.h"
#include "intzip.h"

#define BUFSIZE 0x10000

using namespace std;

namespace intzip {

  // uint32_t
  template void read_stdin_hex(std::vector<uint32_t> &in);
  template void read_file_hex(const char *path, std::vector<uint32_t> &in);
  template void read_stdin_bin(std::vector<uint32_t> &in);
  template void write_stdout_hex(const vector<uint32_t> &out);

}

template<class T>
static ___always_inline__(___const__( T unpack(const char *n) ));

template<class T>
static ___always_inline__(___const__( T scan_hex(const char *s) ));

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
void intzip::read_stdin_hex(std::vector<T> &in)
{
  string line;
  while (getline(cin, line))
  {
    in.push_back(scan_hex<T>(line.c_str()));
  }
}

template<class T>
void intzip::read_file_hex(const char *path, std::vector<T> &in)
{
  string line;
  std::ifstream infile(path);
  
  while (getline(infile, line))
  {
    in.push_back(scan_hex<T>(line.c_str()));
  }
}


template<class T>
void intzip::read_stdin_bin(vector<T> &in)
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
void intzip::write_stdout_hex(const vector<T> &out)
{
  for (typename vector<T>::const_iterator it = out.begin(); it != out.end(); ++it) {
    cout << hex << *it << endl;
  }
}


