/* -*-mode:c++; c-style:k&r; c-basic-offset:4; -*- */
/**********************************************************************************************
    Copyright (C) 2008 Albrecht Dre <albrecht.dress@arcor.de>

    This file contains macros for platform-independant access of data stored in little-endian
    format.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    =============================================================
    Why does this file exist, and what should hackers do with it?
    -------------------------------------------------------------

    1. Background

    Garmin stores data in map files and in their devices in little-endian format.  The data
    is usually packed, i.e. it is not guaranteed that multi-byte data is aligned at 16, 32 or
    64 bit boundaries.  This is fine as long as you run QLandkarte (or any derived software)
    on a little-endian machine which accepts accessing unaligned data.

    The `configure' script in the top-level folder tries to detect if your machine is a
    little endian (like Intel or ARM) or a big endian (like PowerPC or Sparc).  In the latter
    case, it defines the macro HAVE_BIGENDIAN.  In another test, it checks if your machine
    supports accessing unaligned memory (like Intel or PowerPC) or if such accesses would fail
    (as on ARM or Sparc).  If unaligned accesses are supported, the macro CAN_UNALIGNED will
    be defined.  Of course, the file config.h from the top-level folder has to be included.

    2. How to access data

    To work around these problems, this file defines a number of access macros.  On machines
    which do not need them, they always expand to nothing, but they are *absolutely* necessary
    on others.  So *never* access multi-byte elements without using these macros.

    2.1 Load data from Garmin

    The following rules apply if you want to access data in a source coming from a Garmin
    device or file:

    (a) the source is a constant or an aligned 16, 32 or 64-bit value

    Always use the macro
        gar_endian(<type>, <source>)
    where type may be int16_t, int32_t, int64_t, uint16_t, uint32_t, uint64_t, float or double.
    The returned value will explicitly be cast'ed to <type>.

    (b) the source is an unaligned 16, 32 or 64-bit value

    Always use the macro
        gar_load(<type>, <source>)
    where type may be int16_t, int32_t, int64_t, uint16_t, uint32_t, uint64_t, float or double.
    The returned value will explicitly be cast'ed to <type>.

    (c) the source is a pointer

    Always use the macro
        gar_ptr_load(<type>, <pointer>)
    where type may be int16_t, int32_t, int64_t, uint16_t, uint32_t, uint64_t, float or double
    or the special Garmin types int24_t or uint24_t.  The returned value will be of type <type>
    except for the Garmin types int24_t or uint24_t which will be int32_t or uint32_t,
    respectively, but have the uppermost 8 bits always set to 0.

    2.2 Store data to Garmin

    (a) the destination is a variable

    For unaligned variables, use the macro
        gar_store(<type>, <destination>, <source>)
    where type may be int16_t, int32_t, int64_t, uint16_t, uint32_t, uint64_t, float or double.
    if the variable is aligned, use "destination = gar_endian(type, source)" which is faster.

    (b) the destination is a pointer

    For unaligned pointer destinations, use the macro
        gar_ptr_store(<type>, <pointer>, <source>)
    where type may be int16_t, int32_t, int64_t, uint16_t, uint32_t, uint64_t, float or double
    or the special Garmin types int24_t or uint24_t.  For a standard type and an aligned pointer
    destination, use "*(type *)(ptr) = gar_endian(type, source)" which is faster.

**********************************************************************************************/
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

// include platform setup (HAVE_BIGENDIAN, CAN_UNALIGNED)
#include "config.h"

// need integer type definitions with fixed width
#ifdef HAVE_INTTYPES_H
#  include <inttypes.h>
#elif HAVE_STDINT_H
#  include <stdint.h>
#elif WIN32
#include <windows.h>

typedef __int8  int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8     uint8_t;
typedef unsigned __int16    uint16_t;
typedef unsigned __int32    uint32_t;
typedef unsigned __int64    uint64_t;

#define isnan(x) _isnan(x)

#else
#  error neither inttypes.h nor stdint.h are available
#endif
#include <string.h>

// --------------------------------------------------------------------------------------------
// macros to fix the endianess of the constant or properly aligned argument x of type t

#if !defined(HAVE_BIGENDIAN)

// little endian platform: just return the argument
#define gar_endian(t, x)            (t)(x)

#else

// big endian platform
#define gar_endian(t, x)            (__gar_endian_ ## t(x))

// define swapping
static inline uint16_t
__gar_endian_uint16_t(uint16_t x)
{
    return ((x >> 8) & 0xffu) | ((x & 0xffu) << 8);
}


static inline uint32_t
__gar_endian_uint32_t(uint32_t x)
{
    return (((x & 0xff000000u) >> 24) |
        ((x & 0x00ff0000u) >>  8) |
        ((x & 0x0000ff00u) <<  8) |
        ((x & 0x000000ffu) << 24));
}


static inline uint64_t
__gar_endian_uint64_t(uint64_t x)
{
    return (((x & 0xff00000000000000ull) >> 56) |
        ((x & 0x00ff000000000000ull) >> 40) |
        ((x & 0x0000ff0000000000ull) >> 24) |
        ((x & 0x000000ff00000000ull) >>  8) |
        ((x & 0x00000000ff000000ull) <<  8) |
        ((x & 0x0000000000ff0000ull) << 24) |
        ((x & 0x000000000000ff00ull) << 40) |
        ((x & 0x00000000000000ffull) << 56));
}


static inline int16_t
__gar_endian_int16_t(int16_t x)
{
    return ((x >> 8) & 0xffu) | ((x & 0xffu) << 8);
}


static inline int32_t
__gar_endian_int32_t(int32_t x)
{
    return (((x & 0xff000000u) >> 24) |
        ((x & 0x00ff0000u) >>  8) |
        ((x & 0x0000ff00u) <<  8) |
        ((x & 0x000000ffu) << 24));
}


static inline int64_t
__gar_endian_int64_t(int64_t x)
{
    return (((x & 0xff00000000000000ull) >> 56) |
        ((x & 0x00ff000000000000ull) >> 40) |
        ((x & 0x0000ff0000000000ull) >> 24) |
        ((x & 0x000000ff00000000ull) >>  8) |
        ((x & 0x00000000ff000000ull) <<  8) |
        ((x & 0x0000000000ff0000ull) << 24) |
        ((x & 0x000000000000ff00ull) << 40) |
        ((x & 0x00000000000000ffull) << 56));
}


static inline float
__gar_endian_float(float x)
{
    union
    {
        uint32_t _u;
        float _f;
    } _v;

    _v._f = x;
    _v._u = gar_endian(uint32_t, _v._u);
    return _v._f;
}


static inline double
__gar_endian_double(double x)
{
    union
    {
        uint64_t _u;
        double _d;
    } _v;

    _v._d = x;
    _v._u = gar_endian(uint64_t, _v._u);
    return _v._d;
}
#endif                           // HAVE_BIGENDIAN

// --------------------------------------------------------------------------------------------
// macros to deal with pointers or unaligned arguments

// load argument of type t from pointer p
#define gar_ptr_load(t, p)          __gar_ptr_load_ ## t((const uint8_t *)(p))

// store argument src of type t in in the location to which the pointer p points
#define gar_ptr_store(t, p, src)    __gar_ptr_store_ ## t((uint8_t *)(p), (src))

#if defined(CAN_UNALIGNED) && !defined(HAVE_BIGENDIAN)

// load argument x of type t - noop with proper cast
#define gar_load(t, x)              (t)(x)

// store argument src of type t in the variable dst of type t - just assign
#define gar_store(t, dst, src)      (dst) = (src)

// load from pointer - simply map memory
#define __gar_ptr_load_int16_t(p)   (*((int16_t *)(p)))
#define __gar_ptr_load_int32_t(p)   (*((int32_t *)(p)))
#define __gar_ptr_load_int64_t(p)   (*((int64_t *)(p)))
#define __gar_ptr_load_uint16_t(p)  (*((uint16_t *)(p)))
#define __gar_ptr_load_uint32_t(p)  (*((uint32_t *)(p)))
#define __gar_ptr_load_uint64_t(p)  (*((uint64_t *)(p)))
#define __gar_ptr_load_float(p)     (*((float *)(p)))
#define __gar_ptr_load_double(p)    (*((double *)(p)))
// special Garmin types - map memory and clear extra bits
#define __gar_ptr_load_uint24_t(p)  (__gar_ptr_load_uint32_t(p) & 0x00FFFFFFu)
#define __gar_ptr_load_int24_t(p)   (__gar_ptr_load_int32_t(p) & 0x00FFFFFFu)

// store data to pointer - just assign after a proper cast
#define __gar_ptr_store_int16_t(p, src)     (*((int16_t *)(p))) = (src)
#define __gar_ptr_store_int32_t(p, src)     (*((int32_t *)(p))) = (src)
#define __gar_ptr_store_int64_t(p, src)     (*((int64_t *)(p))) = (src)
#define __gar_ptr_store_uint16_t(p, src)    (*((uint16_t *)(p))) = (src)
#define __gar_ptr_store_uint32_t(p, src)    (*((uint32_t *)(p))) = (src)
#define __gar_ptr_store_uint64_t(p, src)    (*((uint64_t *)(p))) = (src)
#define __gar_ptr_store_float(p, src)       (*((float *)(p))) = (src)
#define __gar_ptr_store_double(p, src)      (*((double *)(p))) = (src)
// special Garmin types - use memcpy
static inline void
__gar_ptr_store_int24_t(uint8_t * p, int32_t src)
{
    __gar_ptr_store_uint16_t(p, src & 0xffffu);
    p[2] = src >> 16;
}


static inline void
__gar_ptr_store_uint24_t(uint8_t * p, uint32_t src)
{
    __gar_ptr_store_uint16_t(p, src & 0xffffu);
    p[2] = src >> 16;
}


#else                            // machine is either Big Endian or does not support unaligned accesses

// load argument x of type t - call pointer load macro
#define gar_load(t, x)              gar_ptr_load(t, (uint8_t *)&(x))

// store argument src of type t in the variable dst of type t - call pointer store macro
#define gar_store(t, dst, src)      gar_ptr_store(t, (uint8_t *)&(dst), src)

// load from pointer - read'n'shift bytes
// use Byte-Reverse operations for PowerPC
static inline uint16_t
__gar_ptr_load_uint16_t(const uint8_t *p)
{
#ifdef __powerpc__
    register uint16_t temp;

    asm __volatile__ ("lhbrx %0,0,%1" : "=r" (temp) : "b" (p), "m" (*p));
    return temp;
#else
    return (uint16_t)(p[0] | (p[1] << 8));
#endif
}


static inline uint32_t
__gar_ptr_load_uint24_t(const uint8_t *p)
{
#ifdef __powerpc__
    register uint32_t temp;

    asm __volatile__ ("lwbrx %0,0,%1"       : "=r" (temp) : "b" (p), "m" (*p));
    asm __volatile__ ("rlwinm %0,%1,0,8,31" : "=r" (temp) : "r" (temp));
    return temp;
#else
    return (uint32_t)(p[0] | (p[1] << 8) | (p[2] << 16));
#endif
}


static inline uint32_t
__gar_ptr_load_uint32_t(const uint8_t *p)
{
#ifdef __powerpc__
    register uint32_t temp;

    asm __volatile__ ("lwbrx %0,0,%1" : "=r" (temp) : "b" (p), "m" (*p));
    return temp;
#else
    return (uint32_t)(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
#endif
}


static inline uint64_t
__gar_ptr_load_uint64_t(const uint8_t *p)
{
    return ((uint64_t)__gar_ptr_load_uint32_t(p) |
        ((uint64_t)__gar_ptr_load_uint32_t(p + 4) << 32));
}


static inline int16_t
__gar_ptr_load_int16_t(const uint8_t *p)
{
#ifdef __powerpc__
    register int16_t temp;

    asm __volatile__ ("lhbrx %0,0,%1" : "=r" (temp) : "b" (p), "m" (*p));
    return temp;
#else
    return (int16_t)(p[0] | (p[1] << 8));
#endif
}


static inline int32_t
__gar_ptr_load_int24_t(const uint8_t *p)
{
#ifdef __powerpc__
    register int32_t temp;

    asm __volatile__ ("lwbrx %0,0,%1"       : "=r" (temp) : "b" (p), "m" (*p));
    asm __volatile__ ("rlwinm %0,%1,0,8,31" : "=r" (temp) : "r" (temp));
    return temp;
#else
    return p[0] | (p[1] << 8) | (p[2] << 16);
#endif
}


static inline int32_t
__gar_ptr_load_int32_t(const uint8_t *p)
{
#ifdef __powerpc__
    register int32_t temp;

    asm __volatile__ ("lwbrx %0,0,%1" : "=r" (temp) : "b" (p), "m" (*p));
    return temp;
#else
    return (int32_t)(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
#endif
}


static inline int64_t
__gar_ptr_load_int64_t(const uint8_t *p)
{
    return ((int64_t)__gar_ptr_load_uint32_t(p) |
        ((int64_t)__gar_ptr_load_int32_t(p + 4) << 32));
}


static inline float
__gar_ptr_load_float(const uint8_t * p)
{
    union
    {
        uint32_t _u;
        float _f;
    } _v;

    _v._u = gar_ptr_load(uint32_t, p);
    return _v._f;
}


static inline double
__gar_ptr_load_double(const uint8_t * p)
{
    union
    {
        uint64_t _u;
        double _d;
    } _v;

    _v._u = gar_ptr_load(uint64_t, p);
    return _v._d;
}


// macros to store data - use memcpy to store data to pointer
static inline void
__gar_ptr_store_uint16_t(uint8_t *p, uint16_t src)
{
    p[0] = src & 0xffu;
    p[1] = (src >> 8) & 0xffu;
}


static inline void
__gar_ptr_store_uint24_t(uint8_t *p, uint32_t src)
{
    p[0] = src & 0xffu;
    p[1] = (src >> 8) & 0xffu;
    p[2] = (src >> 16) & 0xffu;
}


static inline void
__gar_ptr_store_uint32_t(uint8_t *p, uint32_t src)
{
    p[0] = src & 0xffu;
    p[1] = (src >> 8) & 0xffu;
    p[2] = (src >> 16) & 0xffu;
    p[3] = (src >> 24) & 0xffu;
}


static inline void
__gar_ptr_store_uint64_t(uint8_t *p, uint64_t src)
{
    __gar_ptr_store_uint32_t(p, src & 0xffffffffu);
    __gar_ptr_store_uint32_t(p + 4, src >> 32);
}


#define __gar_ptr_store_int16_t(p, src) __gar_ptr_store_uint16_t(p, (uint16_t)src)
#define __gar_ptr_store_int24_t(p, src) __gar_ptr_store_uint24_t(p, (uint32_t)src)
#define __gar_ptr_store_int32_t(p, src) __gar_ptr_store_uint32_t(p, (uint32_t)src)
#define __gar_ptr_store_int64_t(p, src) __gar_ptr_store_uint64_t(p, (uint64_t)src)

static inline void
__gar_ptr_store_float(uint8_t *p, float src)
{
    float __fv = gar_endian(float, src);
    memcpy(p, &__fv, 4);
}


static inline void
__gar_ptr_store_double(uint8_t *p, double src)
{
    double __dv = gar_endian(double, src);
    memcpy(p, &__dv, 8);
}
#endif                           // cannot unaligned or big endian
#endif                           // __PLATFORM_H__
