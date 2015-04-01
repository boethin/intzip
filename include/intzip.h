#ifndef ___DELTACHUNK_H__
#define ___DELTACHUNK_H__

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

using namespace std;

namespace intzip {

  template<class T>
  void encode(const vector<T> &in, vector<T> &enc);

  template<class T>
  void decode(const vector<T> &enc, vector<T> &out);

}

#endif
