#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "intzip-stdint.h"
#include "intzip-def.h"
#include "intzip-uint.h"

#include <stdio.h>

using namespace std;
using namespace intzip;

// g++ -Wall -I. -Iinclude -DHAVE_CONFIG_H -o uint uint.cpp && ./uint

int main()
{

  uint64_t m1 = uint<uint64_t>::bitmask(64-16,16);
  
  printf("%016" PRIx64 "\n",m1);

	return 0;
}
