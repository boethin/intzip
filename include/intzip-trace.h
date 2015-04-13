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
#ifndef ___INTZIP_TRACE_H___
#define ___INTZIP_TRACE_H___

// verbose encoding tracing to stderr
#ifdef ENABLE_TRACE

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <stdarg.h>

extern bool enable_trace;

template<class T> struct chunk;
static void printf_tracelog(const char *mark, const char* log);
template<class T> void printf_tracelog_args(const char *mark, T n);
static void printf_tracelog_args(const char *mark, const char* format, ...);
template<class T> static void printf_tracelog_args(const char *mark, const chunk<T> &chunk, const char* format, ...);
template<class T> static void printf_tracelog_args(const char *mark, const chunk<T> &chunk);

#define TRACE(...) printf_tracelog_args ( __VA_ARGS__ )
#else
#define TRACE(...)

#endif // ENABLE_TRACE

#endif
