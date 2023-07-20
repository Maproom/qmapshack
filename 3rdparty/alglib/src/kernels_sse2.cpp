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
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "stdafx.h"

//
// Must be defined before we include kernel header
//
#define _ALGLIB_IMPL_DEFINES
#define _ALGLIB_INTEGRITY_CHECKS_ONCE

#include "kernels_sse2.h"

// disable some irrelevant warnings
#if (AE_COMPILER==AE_MSVC) && !defined(AE_ALL_WARNINGS)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#pragma warning(disable:4611)
#pragma warning(disable:4702)
#pragma warning(disable:4996)
#endif

namespace alglib_impl
{



#if !defined(ALGLIB_NO_FAST_KERNELS) && defined(_ALGLIB_HAS_SSE2_INTRINSICS)

double rdotv_sse2(ae_int_t n,
     /* Real    */ const double* x,
     /* Real    */ const double* y,
     ae_state *_state)
{
    ae_int_t i;

    const ae_int_t sse2len = n>>1;
    const ae_int_t unrollLen = (sse2len>>3)<<3;
    const __m128d* __restrict pX = (const __m128d*)(x);
    const __m128d* __restrict pY = (const __m128d*)(y);
    __m128d ans;
    if(unrollLen >= 8) {
        __m128d unroll0 = _mm_mul_pd(pX[0], pY[0]);
        __m128d unroll1 = _mm_mul_pd(pX[1], pY[1]);
        __m128d unroll2 = _mm_mul_pd(pX[2], pY[2]);
        __m128d unroll3 = _mm_mul_pd(pX[3], pY[3]);
        __m128d unroll4 = _mm_mul_pd(pX[4], pY[4]);
        __m128d unroll5 = _mm_mul_pd(pX[5], pY[5]);
        __m128d unroll6 = _mm_mul_pd(pX[6], pY[6]);
        __m128d unroll7 = _mm_mul_pd(pX[7], pY[7]);
        for(i=8; i<unrollLen; i+=8) {
            unroll0 = _mm_add_pd(_mm_mul_pd(pX[i], pY[i]), unroll0);
            unroll1 = _mm_add_pd(_mm_mul_pd(pX[i+1], pY[i+1]), unroll1);
            unroll2 = _mm_add_pd(_mm_mul_pd(pX[i+2], pY[i+2]), unroll2);
            unroll3 = _mm_add_pd(_mm_mul_pd(pX[i+3], pY[i+3]), unroll3);
            unroll4 = _mm_add_pd(_mm_mul_pd(pX[i+4], pY[i+4]), unroll4);
            unroll5 = _mm_add_pd(_mm_mul_pd(pX[i+5], pY[i+5]), unroll5);
            unroll6 = _mm_add_pd(_mm_mul_pd(pX[i+6], pY[i+6]), unroll6);
            unroll7 = _mm_add_pd(_mm_mul_pd(pX[i+7], pY[i+7]), unroll7);
        }
        switch(sse2len-unrollLen) {
        case 7:
            unroll6 = _mm_add_pd(_mm_mul_pd(pX[i+6], pY[i+6]), unroll6);
        case 6:
            unroll5 = _mm_add_pd(_mm_mul_pd(pX[i+5], pY[i+5]), unroll5);
        case 5:
            unroll4 = _mm_add_pd(_mm_mul_pd(pX[i+4], pY[i+4]), unroll4);
        case 4:
            unroll3 = _mm_add_pd(_mm_mul_pd(pX[i+3], pY[i+3]), unroll3);
        case 3:
            unroll2 = _mm_add_pd(_mm_mul_pd(pX[i+2], pY[i+2]), unroll2);
        case 2:
            unroll1 = _mm_add_pd(_mm_mul_pd(pX[i+1], pY[i+1]), unroll1);
        case 1:
            unroll0 = _mm_add_pd(_mm_mul_pd(pX[i+0], pY[i+0]), unroll0);
        }
        ans = _mm_add_pd(
            _mm_add_pd(_mm_add_pd(unroll0, unroll1), _mm_add_pd(unroll2, unroll3)),
            _mm_add_pd(_mm_add_pd(unroll4, unroll5), _mm_add_pd(unroll6, unroll7)));
    }
    else {
        switch(sse2len) {
        case 0:
            if(n == 0) {
                return 0;
            } else {
                return x[0]*y[0];
            }
        case 1:
            ans = _mm_mul_pd(pX[0], pY[0]);
            break;
        case 2:
            ans = _mm_add_pd(_mm_mul_pd(pX[0], pY[0]), _mm_mul_pd(pX[1], pY[1]));
            break;
        case 3:
            ans = _mm_add_pd(
                _mm_add_pd(_mm_mul_pd(pX[0], pY[0]), _mm_mul_pd(pX[1], pY[1])),
                _mm_mul_pd(pX[2], pY[2]));
            break;
        case 4:
            ans = _mm_add_pd(
                _mm_add_pd(_mm_mul_pd(pX[0], pY[0]), _mm_mul_pd(pX[1], pY[1])),
                _mm_add_pd(_mm_mul_pd(pX[2], pY[2]), _mm_mul_pd(pX[3], pY[3])));
            break;
        case 5:
            ans = _mm_add_pd(
                _mm_add_pd(
                    _mm_add_pd(_mm_mul_pd(pX[0], pY[0]), _mm_mul_pd(pX[1], pY[1])),
                    _mm_add_pd(_mm_mul_pd(pX[2], pY[2]), _mm_mul_pd(pX[3], pY[3]))),
                _mm_mul_pd(pX[4], pY[4]));
            break;
        case 6:
            ans = _mm_add_pd(
                _mm_add_pd(
                    _mm_add_pd(_mm_mul_pd(pX[0], pY[0]), _mm_mul_pd(pX[1], pY[1])),
                    _mm_add_pd(_mm_mul_pd(pX[2], pY[2]), _mm_mul_pd(pX[3], pY[3]))),
                _mm_add_pd(_mm_mul_pd(pX[4], pY[4]), _mm_mul_pd(pX[5], pY[5])));
            break;
        case 7:
            ans = _mm_add_pd(
                _mm_add_pd(
                    _mm_add_pd(_mm_mul_pd(pX[0], pY[0]), _mm_mul_pd(pX[1], pY[1])),
                    _mm_add_pd(_mm_mul_pd(pX[2], pY[2]), _mm_mul_pd(pX[3], pY[3]))),
                _mm_add_pd(
                    _mm_add_pd(_mm_mul_pd(pX[4], pY[4]), _mm_mul_pd(pX[5], pY[5])),
                    _mm_mul_pd(pX[6], pY[6])));
            break;
        }
    }

    const double *pComps = (const double*)&ans;
    double scalar = pComps[0] + pComps[1];
    const ae_int_t tail = sse2len<<1;
    if(n-tail) {
        return scalar + x[tail]*y[tail];
    } else {
        return scalar;
    }
}

double rdotv2_sse2(ae_int_t n,
     /* Real    */ const double* x,
     ae_state *_state)
{
    ae_int_t i;

    const ae_int_t sse2len = n>>1;
    const ae_int_t unrollLen = (sse2len>>3)<<3;
    const __m128d* __restrict pX = (const __m128d*)(x);
    __m128d ans;
    if(unrollLen >= 8) {
        __m128d unroll0 = _mm_mul_pd(pX[0], pX[0]);
        __m128d unroll1 = _mm_mul_pd(pX[1], pX[1]);
        __m128d unroll2 = _mm_mul_pd(pX[2], pX[2]);
        __m128d unroll3 = _mm_mul_pd(pX[3], pX[3]);
        __m128d unroll4 = _mm_mul_pd(pX[4], pX[4]);
        __m128d unroll5 = _mm_mul_pd(pX[5], pX[5]);
        __m128d unroll6 = _mm_mul_pd(pX[6], pX[6]);
        __m128d unroll7 = _mm_mul_pd(pX[7], pX[7]);
        for(i=8; i<unrollLen; i+=8) {
            unroll0 = _mm_add_pd(_mm_mul_pd(pX[i], pX[i]), unroll0);
            unroll1 = _mm_add_pd(_mm_mul_pd(pX[i+1], pX[i+1]), unroll1);
            unroll2 = _mm_add_pd(_mm_mul_pd(pX[i+2], pX[i+2]), unroll2);
            unroll3 = _mm_add_pd(_mm_mul_pd(pX[i+3], pX[i+3]), unroll3);
            unroll4 = _mm_add_pd(_mm_mul_pd(pX[i+4], pX[i+4]), unroll4);
            unroll5 = _mm_add_pd(_mm_mul_pd(pX[i+5], pX[i+5]), unroll5);
            unroll6 = _mm_add_pd(_mm_mul_pd(pX[i+6], pX[i+6]), unroll6);
            unroll7 = _mm_add_pd(_mm_mul_pd(pX[i+7], pX[i+7]), unroll7);
        }
        switch(sse2len-unrollLen) {
        case 7:
            unroll6 = _mm_add_pd(_mm_mul_pd(pX[i+6], pX[i+6]), unroll6);
        case 6:
            unroll5 = _mm_add_pd(_mm_mul_pd(pX[i+5], pX[i+5]), unroll5);
        case 5:
            unroll4 = _mm_add_pd(_mm_mul_pd(pX[i+4], pX[i+4]), unroll4);
        case 4:
            unroll3 = _mm_add_pd(_mm_mul_pd(pX[i+3], pX[i+3]), unroll3);
        case 3:
            unroll2 = _mm_add_pd(_mm_mul_pd(pX[i+2], pX[i+2]), unroll2);
        case 2:
            unroll1 = _mm_add_pd(_mm_mul_pd(pX[i+1], pX[i+1]), unroll1);
        case 1:
            unroll0 = _mm_add_pd(_mm_mul_pd(pX[i+0], pX[i+0]), unroll0);
        }
        ans = _mm_add_pd(
            _mm_add_pd(_mm_add_pd(unroll0, unroll1), _mm_add_pd(unroll2, unroll3)),
            _mm_add_pd(_mm_add_pd(unroll4, unroll5), _mm_add_pd(unroll6, unroll7)));
    }
    else {
        switch(sse2len) {
        case 0:
            if(n == 0) {
                return 0;
            } else {
                return x[0]*x[0];
            }
        case 1:
            ans = _mm_mul_pd(pX[0], pX[0]);
            break;
        case 2:
            ans = _mm_add_pd(_mm_mul_pd(pX[0], pX[0]), _mm_mul_pd(pX[1], pX[1]));
            break;
        case 3:
            ans = _mm_add_pd(
                _mm_add_pd(_mm_mul_pd(pX[0], pX[0]), _mm_mul_pd(pX[1], pX[1])),
                _mm_mul_pd(pX[2], pX[2]));
            break;
        case 4:
            ans = _mm_add_pd(
                _mm_add_pd(_mm_mul_pd(pX[0], pX[0]), _mm_mul_pd(pX[1], pX[1])),
                _mm_add_pd(_mm_mul_pd(pX[2], pX[2]), _mm_mul_pd(pX[3], pX[3])));
            break;
        case 5:
            ans = _mm_add_pd(
                _mm_add_pd(
                    _mm_add_pd(_mm_mul_pd(pX[0], pX[0]), _mm_mul_pd(pX[1], pX[1])),
                    _mm_add_pd(_mm_mul_pd(pX[2], pX[2]), _mm_mul_pd(pX[3], pX[3]))),
                _mm_mul_pd(pX[4], pX[4]));
            break;
        case 6:
            ans = _mm_add_pd(
                _mm_add_pd(
                    _mm_add_pd(_mm_mul_pd(pX[0], pX[0]), _mm_mul_pd(pX[1], pX[1])),
                    _mm_add_pd(_mm_mul_pd(pX[2], pX[2]), _mm_mul_pd(pX[3], pX[3]))),
                _mm_add_pd(_mm_mul_pd(pX[4], pX[4]), _mm_mul_pd(pX[5], pX[5])));
            break;
        case 7:
            ans = _mm_add_pd(
                _mm_add_pd(
                    _mm_add_pd(_mm_mul_pd(pX[0], pX[0]), _mm_mul_pd(pX[1], pX[1])),
                    _mm_add_pd(_mm_mul_pd(pX[2], pX[2]), _mm_mul_pd(pX[3], pX[3]))),
                _mm_add_pd(
                    _mm_add_pd(_mm_mul_pd(pX[4], pX[4]), _mm_mul_pd(pX[5], pX[5])),
                    _mm_mul_pd(pX[6], pX[6])));
            break;
        }
    }

    const double *pComps = (const double*)&ans;
    double scalar = pComps[0] + pComps[1];
    const ae_int_t tail = sse2len<<1;
    if(n-tail) {
        return scalar + x[tail]*x[tail];
    } else {
        return scalar;
    }
}

void rcopyv_sse2(const ae_int_t n,
     /* Real    */ const double* __restrict x,
     /* Real    */ double* __restrict y,
     ae_state* __restrict _state)
{
    ae_int_t i;
    const ae_int_t sse2len = n>>1;
    const ae_int_t tail = sse2len<<1;
    const __m128d* __restrict pSrc = (const __m128d*)(x);
    __m128d* __restrict pDest = (__m128d*)(y);
    
    for(i=0; i<sse2len; i++)
        pDest[i] = pSrc[i];
    if( n-tail )
        *(double*)(pDest+i) = *(const double*)(pSrc+i);
}

void rcopymulv_sse2(const ae_int_t n,
     const double v,
     /* Real    */ const double* __restrict x,
     /* Real    */ double* __restrict y,
     const ae_state* __restrict _state)
{
    ae_int_t i;
    
    const ae_int_t sse2len = n>>1;
    const __m128d* __restrict pSrc = (const __m128d*)(x);
    __m128d* __restrict pDest = (__m128d*)(y);
    const __m128d sse2v = _mm_set1_pd(v);
    const ae_int_t tail = sse2len<<1;
    for(i=0; i<sse2len; i++) {
        pDest[i] = _mm_mul_pd(sse2v, pSrc[i]);
    }
    if(n-tail) {
        *(double*)(pDest+i) = v * (*(const double*)(pSrc+i));
    }
}

void icopyv_sse2(const ae_int_t n, const ae_int_t* __restrict x,
                ae_int_t* __restrict y, ae_state* __restrict _state)
{
    const ae_int_t tail = (n*sizeof(ae_int_t)) & 15;
    const ae_int_t even = (n*sizeof(ae_int_t)) - tail;
    __m128i *__restrict pDest = (__m128i*)y;
    const __m128i* __restrict pSrc = (const __m128i*)x;
    const ae_int_t nVec = even>>4;
    ae_int_t i;
    for(i=0; i<nVec; i++) {
        pDest[i] = pSrc[i];
    }
    i = even/sizeof(ae_int_t);
    if(tail & 8) {
        *(ae_int64_t*)(y+i) = *(const ae_int64_t*)(x+i);
        i += 8/sizeof(ae_int_t);
    }
    if(tail & 4) {
        *(ae_int32_t*)(y+i) = *(const ae_int32_t*)(x+i);
    }
}

void bcopyv_sse2(const ae_int_t n, const ae_bool* __restrict x,
                ae_bool* __restrict y, ae_state* __restrict _state)
{
    const ae_int_t tail = n & 15;
    const ae_int_t even = n-tail;
    __m128i *__restrict pDest = (__m128i*)y;
    const __m128i* __restrict pSrc = (const __m128i*)x;
    const ae_int_t nVec = even>>4;
    ae_int_t i;
    for(i=0; i<nVec; i++) {
        pDest[i] = pSrc[i];
    }
    i = even;
    if(tail & 8) {
        *(ae_int64_t*)(y+i) = *(const ae_int64_t*)(x+i);
        i += 8;
    }
    if(tail & 4) {
        *(ae_int32_t*)(y+i) = *(const ae_int32_t*)(x+i);
        i += 4;
    }
    if(tail & 2)
    {
        *(y+i+0) = *(x+i+0);
        *(y+i+1) = *(x+i+1);
        i += 2;
    }
    if(tail & 1) {
        *(y+i) = *(x+i);
    }
}

void rsetv_sse2(const ae_int_t n,
     const double v,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state)
{
    ae_int_t i;
    
    const ae_int_t sse2len = n>>1;
    __m128d* __restrict pDest = (__m128d*)(x);
    const __m128d sse2v = _mm_set1_pd(v);
    for(i=0; i<sse2len; i++) {
        pDest[i] = sse2v;
    }
    const ae_int_t tail = sse2len<<1;
    if(n-tail) {
        *(double*)(pDest+i) = v;
    }
}

void rsetvx_sse2(const ae_int_t n,
     const double v,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state)
{
    if( n<=4 )
    {
        ae_int_t j;
        for(j=0; j<=n-1; j++)
            x[j] = v;
        return;
    }
    if((((ptrdiff_t)x) & 15) == 0)
    {
        rsetv_sse2(n, v, x, _state);
        return;
    }
    x[0] = v;
    rsetv_sse2(n-1, v, x+1, _state);
}

void isetv_sse2(const ae_int_t n, const ae_int_t v,
    ae_int_t* __restrict x, ae_state* __restrict _state)
{
    const ae_int_t tail = (n*sizeof(ae_int_t)) & 15;
    const ae_int_t even = (n*sizeof(ae_int_t)) - tail;
    __m128i *__restrict pDest = (__m128i*)x;
    const ae_int_t v2[2] = {v, v};
    const __m128i sse2v = ((sizeof(v) == 4) ? _mm_set1_epi32((ae_int32_t)v) : _mm_loadu_si128((const __m128i*)(&v2[0])));
    const ae_int_t nVec = even>>4;
    ae_int_t i;
    for(i=0; i<nVec; i++) {
        pDest[i] = sse2v;
    }
    memmove(pDest+i, &sse2v, tail);
}

void bsetv_sse2(const ae_int_t n, const ae_bool v, ae_bool* __restrict x,
    ae_state* __restrict _state)
{
    const ae_int_t tail = n & 15;
    const ae_int_t even = n-tail;
    __m128i *__restrict pDest = (__m128i*)x;
    const __m128i sse2v = _mm_set1_epi8(v);
    const ae_int_t nVec = even>>4;
    ae_int_t i;
    for(i=0; i<nVec; i++) {
        pDest[i] = sse2v;
    }
    /* _mm_storel_epi64() has a too high latency and too low throughput on the recent (Skylake+) processors */
    memset(x+even, v, tail);
}

void rmulv_sse2(const ae_int_t n, const double v, double* __restrict x,
     const ae_state* __restrict _state)
{
    ae_int_t i;
    
    const ae_int_t sse2len = n>>1;
    __m128d* __restrict pDest = (__m128d*)(x);
    const __m128d sse2v = _mm_set1_pd(v);
    for(i=0; i<sse2len; i++) {
        pDest[i] = _mm_mul_pd(sse2v, pDest[i]);
    }
    const ae_int_t tail = sse2len<<1;
    if(n-tail) {
        *(double*)(pDest+i) = v * (*(const double*)(pDest+i));
    }
}

void rmulvx_sse2(const ae_int_t n, const double v, double* __restrict x,
    const ae_state* __restrict _state)
{
    if( n<=4 )
    {
        ae_int_t i;
        for(i=0; i<=n-1; i++)
            x[i] *= v;
        return;
    }
    if((((ptrdiff_t)x) & 15) == 0)
    {
        rmulv_sse2(n, v, x, _state);
        return;
    }
    x[0] = v * x[0];
    rmulv_sse2(n-1, v, x+1, _state);
}

void raddv_sse2(const ae_int_t n,
     const double alpha,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state)
{
    ae_int_t i;
    
    const ae_int_t sse2len = n>>1;
    const __m128d* __restrict pSrc = (const __m128d*)(y);
    __m128d* __restrict pDest = (__m128d*)(x);
    const __m128d sse2alpha = _mm_set1_pd(alpha);
    for(i=0; i<sse2len; i++) {
        pDest[i] = _mm_add_pd(_mm_mul_pd(sse2alpha, pSrc[i]), pDest[i]);
    }
    const ae_int_t tail = sse2len<<1;
    if(n-tail) {
        *(double*)(pDest+i) = alpha * (*(const double*)(pSrc+i))
            + (*(const double*)(pDest+i));
    }
}

void raddvx_sse2_xaligned(const ae_int_t n, const double alpha,
    const double* __restrict y, double* __restrict x, ae_state *_state)
{
    ae_int_t i;
    
    const ae_int_t vecLen = (n>>1)<<1;
    const __m128d sse2alpha = _mm_set1_pd(alpha);
    __m128d * __restrict pDest = (__m128d*)x;
    for(i=0; i<vecLen; i+=2)
    {
        const ae_int_t iDest = i>>1;
        pDest[iDest] = _mm_add_pd(_mm_mul_pd(sse2alpha, _mm_loadu_pd(y+i)), pDest[iDest]);
    }
    if(n-vecLen)
        x[i] += alpha*y[i];
}

void raddvx_sse2(const ae_int_t n, const double alpha,
    const double* __restrict y, double* __restrict x, ae_state *_state)
{
    if( n<=4 )
    {
        ae_int_t i;
        for(i=0; i<=n-1; i++)
            x[i] += alpha*y[i];
        return;
    }
    if((((ptrdiff_t)x) & 15) == 0)
    {
        raddvx_sse2_xaligned(n, alpha, y, x, _state);
        return;
    }
    x[0] += alpha*y[0];
    raddvx_sse2_xaligned(n-1, alpha, y+1, x+1, _state);
}

void rmergemulv_sse2(const ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state)
{
    ae_int_t i;
    
    const ae_int_t sse2len = n>>1;
    const __m128d* __restrict pSrc = (const __m128d*)(y);
    __m128d* __restrict pDest = (__m128d*)(x);
    for(i=0; i<sse2len; i++) {
        pDest[i] = _mm_mul_pd(pSrc[i], pDest[i]);
    }
    const ae_int_t tail = sse2len<<1;
    if(n-tail) {
        *(double*)(pDest+i) = (*(const double*)(pSrc+i))
            * (*(const double*)(pDest+i));
    }
}

void rmergemaxv_sse2(const ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     ae_state* __restrict _state)
{
    ae_int_t i;
    
    const ae_int_t sse2len = n>>1;
    const __m128d* __restrict pSrc = (const __m128d*)(y);
    __m128d* __restrict pDest = (__m128d*)(x);
    for(i=0; i<sse2len; i++) {
        pDest[i] = _mm_max_pd(pSrc[i], pDest[i]);
    }
    const ae_int_t tail = sse2len<<1;
    if(n-tail) {
        *(double*)(pDest+i) = ae_maxreal(*(const double*)(pSrc+i),
            *(const double*)(pDest+i), _state);
    }
}

void rmergeminv_sse2(const ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     ae_state* __restrict _state)
{
    ae_int_t i;
    
    const ae_int_t sse2len = n>>1;
    const __m128d* __restrict pSrc = (const __m128d*)(y);
    __m128d* __restrict pDest = (__m128d*)(x);
    for(i=0; i<sse2len; i++) {
        pDest[i] = _mm_min_pd(pSrc[i], pDest[i]);
    }
    const ae_int_t tail = sse2len<<1;
    if(n-tail) {
        *(double*)(pDest+i) = ae_minreal(*(const double*)(pSrc+i),
            *(const double*)(pDest+i), _state);
    }
}

double rmaxv_sse2(ae_int_t n, /* Real    */ const double* __restrict x, ae_state* __restrict _state)
{
    ae_int_t i;
    
    const ae_int_t sse2len = n>>1;
    const __m128d* __restrict pSrc = (const __m128d*)(x);
    if( n<=4 )
    {
        double result;
        if(n == 0)
            return 0.0;
        result = x[0];
        for(i=1; i<=n-1; i++)
        {
            double v = x[i];
            if( v>result )
                result = v;
        }
        return result;
    }
    __m128d curMax = pSrc[0];
    for(i=1; i<sse2len; i++) {
        curMax = _mm_max_pd(curMax, pSrc[i]);
    }
    const double *pComps = (const double *)&curMax;
    const double dMax = (pComps[0] > pComps[1]) ? pComps[0] : pComps[1];
    const ae_int_t tail = sse2len<<1;
    if(n-tail) {
        const double candidate = *(const double*)(pSrc+i);
        return (candidate > dMax) ? candidate : dMax;
    }
    else {
      return dMax;
    }
}

double rmaxabsv_sse2(ae_int_t n, /* Real    */ const double* __restrict x, ae_state* __restrict _state)
{
    const __m128d signMask = _mm_set1_pd(-0.); // -0. = 1 << 63
    const ae_int_t sse2len = n>>1;
    const __m128d* __restrict pSrc = (const __m128d*)(x);
    if( n<=4 )
    {
        double result;
        ae_int_t i;
        result = 0;
        for(i=0; i<=n-1; i++)
        {
            double v = fabs(x[i]);
            if( v>result )
                result = v;
        }
        return result;
    }
    __m128d curMax = _mm_andnot_pd(signMask, pSrc[0]); // abs
    ae_int_t i;
    for(i=1; i<sse2len; i++)
        curMax = _mm_max_pd(curMax, _mm_andnot_pd(signMask, pSrc[i])); // abs
    const double *pComps = (const double *)&curMax;
    const double dMax = (pComps[0] > pComps[1]) ? pComps[0] : pComps[1];
    const ae_int_t tail = sse2len<<1;
    if(n-tail) {
        const double candidate = ae_fabs(*(const double*)(pSrc+i), _state);
        return (candidate > dMax) ? candidate : dMax;
    }
    else {
      return dMax;
    }
}

static void rcopyvx_sse2_xaligned(const ae_int_t n, const double* __restrict x,
    double* __restrict y, ae_state *_state)
{
    ae_int_t i;
    
    const ae_int_t vecLen = (n>>1)<<1;
    const __m128d * __restrict pSrc = (const __m128d*)x;
    for(i=0; i<vecLen; i+=2) {
        const ae_int_t iSrc = i>>1;
        _mm_storeu_pd(y+i, pSrc[iSrc]);
    }
    if(n-vecLen) {
        y[i] = x[i];
    }
}

void rcopyvx_sse2(const ae_int_t n, const double* __restrict x,
    double* __restrict y, ae_state *_state)
{
    if((((ptrdiff_t)x) & 15) == 0)
    {
        rcopyvx_sse2_xaligned(n, x, y, _state);
        return;
    }
    y[0] = x[0];
    rcopyvx_sse2_xaligned(n-1, x+1, y+1, _state);
}

static void icopyvx_sse2_xaligned(const ae_int_t n, const ae_int_t* __restrict x,
                ae_int_t* __restrict y, ae_state* __restrict _state)
{
    const ae_int_t tail = (n*sizeof(ae_int_t)) & 15;
    const ae_int_t even = (n*sizeof(ae_int_t)) - tail;
    const __m128i* __restrict pSrc = (const __m128i*)x;
    const ae_int_t nVec = even>>4;
    const ae_int_t shift_by = 2-sizeof(ae_int_t)/8;
    ae_int_t i;
    for(i=0; i<nVec; i++) {
        const ae_int_t j = i<<shift_by;
        _mm_storeu_si128((__m128i*)(y+j), pSrc[i]);
    }
    i = even/sizeof(ae_int_t);
    if(tail & 8) {
        *(ae_int64_t*)(y+i) = *(const ae_int64_t*)(x+i);
        i += 8/sizeof(ae_int_t);
    }
    if(tail & 4) {
        *(ae_int32_t*)(y+i) = *(const ae_int32_t*)(x+i);
    }
}

void icopyvx_sse2(const ae_int_t n, const ae_int_t* __restrict x,
                ae_int_t* __restrict y, ae_state* __restrict _state)
{
    const ptrdiff_t unal = ((ptrdiff_t)x) & 15;
    if( n<=8 )
    {
        ae_int_t j;
        for(j=0; j<=n-1; j++)
            y[j] = x[j];
        return;
    }
    if(unal == 0)
    {
        icopyvx_sse2_xaligned(n, x, y, _state);
        return;
    }
    const ae_int_t offset = 16-unal;
    memmove(y, x, offset);
    const ae_int_t nDone = offset / sizeof(ae_int_t);
    icopyvx_sse2_xaligned(n-nDone, x+nDone, y+nDone, _state);
}

/* ALGLIB_NO_FAST_KERNELS, _ALGLIB_HAS_SSE2_INTRINSICS */
#endif


}

