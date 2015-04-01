#ifndef ___DEF_H__
#define ___DEF_H__
// defaults

//
// Make sure we have UINT32_MAX, UINT64_MAX
//

#if __cplusplus <= 199711L
// no C++11
//
// stdint.h:
// /* The ISO C99 standard specifies that in C++ implementations these
//    macros should only be defined if explicitly requested.  */
// #if !defined __cplusplus || defined __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#else
// C++11
//
#include <cstdint> // std::uint32_t etc.
#endif

// fallback

#ifndef UINT32_MAX
#define UINT32_MAX 0xFFFFFFFFUL
#endif

#ifndef UINT64_MAX
#define UINT64_MAX 0xFFFFFFFFFFFFFFFFULL
#endif

//
// Function attributes
//

#ifndef ___always_inline__
#define ___always_inline__(f) inline f
#endif

#ifndef ___const__
#define ___const__(f) f
#endif

#ifndef ___pure__
#define ___pure__(f) f
#endif

#ifndef ___inline__
#define ___inline__(f) inline f
#endif

#endif
