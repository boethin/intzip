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
#ifndef ___INTZIP_TRACE_H___
#define ___INTZIP_TRACE_H___

// verbose encoding tracing to stderr

// TRACE macro
#if ENABLE_TRACE
#define TRACE(...) printf_tracelog_args ( __VA_ARGS__ )
#else
#define TRACE(...)
#endif

#if ENABLE_TRACE

#include "intzip-stdint.h"
#include "intzip-def.h"
#include "intzip-chunk.h"

template<class T>
void printf_tracelog_args(const char *mark, T n);

void printf_tracelog_args(const char *mark, const char* format, ...);

template<class T>
void printf_tracelog_args(const char *mark, const intzip::chunkdata<T> &chunk, const char* format, ...);

template<class T>
void printf_tracelog_args(const char *mark, const intzip::chunk<T> &chunk, const char* format, ...);

template<class T>
void printf_tracelog_args(const char *mark, const intzip::chunkdata<T> &chunk);

template<class T>
void printf_tracelog_args(const char *mark, const intzip::chunk<T> &chunk);

#endif // ENABLE_TRACE

#endif // ___INTZIP_TRACE_H___
