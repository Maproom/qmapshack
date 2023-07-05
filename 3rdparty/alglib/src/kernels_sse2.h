/*************************************************************************
ALGLIB 4.00.0 (source code generated 2023-05-21)
Copyright (c) Sergey Bochkanov (ALGLIB project).

>>> SOURCE LICENSE >>>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (www.fsf.org); either version 2 of the 
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License is available at
http://www.fsf.org/licensing/licenses
>>> END OF LICENSE >>>
*************************************************************************/
#ifndef _kernels_sse2_h
#define _kernels_sse2_h

#include "ap.h"

#define AE_USE_CPP



namespace alglib_impl
{
#if !defined(ALGLIB_NO_FAST_KERNELS) && defined(_ALGLIB_HAS_SSE2_INTRINSICS)

double rdotv_sse2(ae_int_t n,
     /* Real    */ const double* x,
     /* Real    */ const double* y,
     ae_state *_state);
double rdotv2_sse2(ae_int_t n,
     /* Real    */ const double* x,
     ae_state *_state);
void rcopyv_sse2(const ae_int_t n,
     /* Real    */ const double* __restrict x,
     /* Real    */ double* __restrict y,
     ae_state* __restrict _state);
void rcopymulv_sse2(const ae_int_t n,
     const double v,
     /* Real    */ const double* __restrict x,
     /* Real    */ double* __restrict y,
     const ae_state* __restrict _state);
void icopyv_sse2(const ae_int_t n, const ae_int_t* __restrict x,
                ae_int_t* __restrict y, ae_state* __restrict _state);
void bcopyv_sse2(const ae_int_t n, const ae_bool* __restrict x,
                ae_bool* __restrict y, ae_state* __restrict _state);
void rsetv_sse2(const ae_int_t n,
     const double v,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state);
void rsetvx_sse2(const ae_int_t n,
     const double v,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state);
void isetv_sse2(const ae_int_t n, const ae_int_t v,
    ae_int_t* __restrict x, ae_state* __restrict _state);
void bsetv_sse2(const ae_int_t n, const ae_bool v, ae_bool* __restrict x,
    ae_state* __restrict _state);
void rmulv_sse2(const ae_int_t n, const double v, double* __restrict x,
     const ae_state* __restrict _state);
void rmulvx_sse2(const ae_int_t n, const double v, double* __restrict x,
    const ae_state* __restrict _state);
void raddv_sse2(const ae_int_t n,
     const double alpha,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state);
void raddvx_sse2(const ae_int_t n, const double alpha,
    const double* __restrict y, double* __restrict x, ae_state *_state);
void rmergemulv_sse2(const ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state);
void rmergemaxv_sse2(const ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     ae_state* __restrict _state);
void rmergeminv_sse2(const ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     ae_state* __restrict _state);
double rmaxv_sse2(ae_int_t n, /* Real    */ const double* __restrict x, ae_state* __restrict _state);
double rmaxabsv_sse2(ae_int_t n, /* Real    */ const double* __restrict x, ae_state* __restrict _state);
void rcopyvx_sse2(const ae_int_t n, const double* __restrict x,
    double* __restrict y, ae_state *_state);
void icopyvx_sse2(const ae_int_t n, const ae_int_t* __restrict x,
                ae_int_t* __restrict y, ae_state* __restrict _state);
/* ALGLIB_NO_FAST_KERNELS, _ALGLIB_HAS_SSE2_INTRINSICS */
#endif

}

#endif

