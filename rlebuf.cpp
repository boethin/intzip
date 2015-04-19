

// g++ -Wall -I. -Iinclude -DHAVE_CONFIG_H -o rlebuf rlebuf.cpp && ./rlebuf


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "intzip-stdint.h"
#include "intzip-trace.h"
#include "intzip-def.h"
#include "intzip-uint.h"
#include "intzip-bitstore.h"
#include "intzip-rlebuf.h"

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

#include <vector>
#include <iostream>

using namespace std;
using namespace intzip;

int test(const uint32_t t[], size_t length, uint8_t bits)
{
  vector<uint32_t> v;
  bitvector_writer<uint32_t> bit_writer(v);
  bitvector_reader<uint32_t> bit_reader(v);
  
  {
    rlebuf_writer<uint32_t,vector<uint32_t> >rle_writer(bit_writer,length,bits);
    for (size_t i = 0; i < length; i++) {
      rle_writer.append(t[i]);
    }
  }

  cout << "--" << endl;

  {
    rlebuf_reader<uint32_t,vector<uint32_t> >rle_reader(bit_reader,length,bits);
    for (size_t i = 0; i < length; i++) {
      uint32_t f = rle_reader.fetch();
      cout << f << endl;
      if (f != t[i]) {
        cerr << "failed" << endl;
        return 0;
      }
    }
  }

  return 1;
}

int main()
{
  if (!test( (const uint32_t[]){
      1,2,3,4,3,4,3,4,3,4
    }, 10, 3 )) {
    return 1;
  }

//   vector< uint32_t > enc;
//   bitvector_writer<uint32_t> bit_writer(enc);
//   bitvector_reader<uint32_t> bit_reader(enc);
//   uint8_t bits = 3;
//   size_t length = 16;
//
//   rlebuf_writer<uint32_t,vector<uint32_t> >rle_writer(bit_writer,length,bits);
//
//   rle_writer.append(1);
//   rle_writer.append(2);
//   rle_writer.append(5);
//   rle_writer.append(3);
//   rle_writer.append(2);
//   rle_writer.append(5);
//   rle_writer.append(3);
//   rle_writer.append(2);
//   rle_writer.append(5);
//   rle_writer.append(3);
//   rle_writer.append(2);
//   rle_writer.append(5);
//   rle_writer.append(3);
//   rle_writer.append(0);
//   rle_writer.append(7);
//   rle_writer.append(6);
//
//   cout << "--" << endl;
//   rlebuf_reader<uint32_t,vector<uint32_t> >rle_reader(bit_reader,length,bits);
//
//   for (size_t i = 0; i < length; i++) {
//     cout << rle_reader.fetch() << endl;
//   }


	return 0;
}
