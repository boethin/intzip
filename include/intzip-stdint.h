/**
 * This file is part of IntZip.
 *
 * Copyright (C) 2015 Sebastian Boethin <boethin@xn--domain.net>
 *
 * IntZip is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * IntZip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with IntZip. If not, see <http://www.gnu.org/licenses/>.
 *
 * See https://github.com/boethin/intzip/wiki for more information.
 */
#ifndef ___INTZIP_STDINT_H___
#define ___INTZIP_STDINT_H___

// Make sure we have UINT16_MAX, UINT32_MAX, UINT64_MAX

#if __cplusplus <= 199711L
// no C++11

/* C++ implementations should define these macros only when __STDC_LIMIT_MACROS
   is defined before <stdint.h> is included */
// ==> Also, __STDC_LIMIT_MACROS must be defined before include <inttypes.h> <==
#define __STDC_LIMIT_MACROS
#include <stdint.h>

#else
// C++11
#include <cstdint> // std::uint32_t etc.
#endif

// PRIx16, PRIx32, PRIx64, etc.
/* These macros are defined for C programs. They are defined for C++ only when
   __STDC_FORMAT_MACROS is defined before <inttypes.h> is included. */
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#endif
