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
#ifndef ___INTZIP_DEF_H___
#define ___INTZIP_DEF_H___
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
