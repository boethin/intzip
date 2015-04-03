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
#ifndef ___INTZIP_GCC_H___
#define ___INTZIP_GCC_H___
#ifdef __GNUC__
/* Definitions for: GCC, the GNU Compiler Collection.
 *
 * This header contains optional compiler-specific improvements.
 */

// builtin functions:
// builtin_ceil_log2_uint32, builtin_ceil_log2_uint64

#if UINT_MAX == UINT32_MAX
  // Built-in Function: int __builtin_clz (unsigned int x)
# define builtin_ceil_log2_uint32(x)  ( x > 0 ? 32 - __builtin_clz( (unsigned int)x ) : 0 )
#elif ULONG_MAX == UINT32_MAX
  // Built-in Function: int __builtin_clzl (unsigned long)
# define builtin_ceil_log2_uint32(x)  ( x > 0 ? 32 - __builtin_clzl( (unsigned long)x ) : 0 )
#endif

#if ULONG_MAX == UINT64_MAX
  // Built-in Function: int __builtin_clzl (unsigned long)
# define builtin_ceil_log2_uint64(x)  ( x > 0 ? 64 - __builtin_clzl( (unsigned long)x ) : 0 )
#elif ULLONG_MAX == UINT64_MAX
  // Built-in Function: int __builtin_clzll (unsigned long long)
# define builtin_ceil_log2_uint64(x)  ( x > 0 ? 64 - __builtin_clzll( (unsigned long long)x ) : 0 )
#endif


// GCC __attribute__

// GCC does not inline any functions when not optimizing unless you specify ‘always_inline’
#undef ___always_inline__
#define ___always_inline__(f) inline f __attribute__((always_inline))

// Functions that do not examine any values except their arguments
#undef ___const__
#define ___const__(f) f __attribute__((const))

// like const, but the function may also examine global memory, perhaps via pointer
// (or presumably reference) parameters
#undef ___pure__
#define ___pure__(f) f __attribute__((pure))

#endif
#endif
