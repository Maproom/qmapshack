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

#include "kernels_avx2.h"

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



#if !defined(ALGLIB_NO_FAST_KERNELS) && defined(_ALGLIB_HAS_AVX2_INTRINSICS)

double rdotv_avx2(const ae_int_t n,
    /* Real    */ const double* __restrict x,
    /* Real    */ const double* __restrict y,
    const ae_state* __restrict _state)
{
    ae_int_t i;

    const ae_int_t avx2len = n>>2;
    const ae_int_t fmaLen = (avx2len >> 2) << 2;
    const __m256d* __restrict pX = (const __m256d*)(x);
    const __m256d* __restrict pY = (const __m256d*)(y);
    __m256d ans;
    if(fmaLen >= 4) {
        __m256d fmaUnroll[4];
        fmaUnroll[0] = _mm256_mul_pd(pX[0], pY[0]);
        fmaUnroll[1] = _mm256_mul_pd(pX[1], pY[1]);
        fmaUnroll[2] = _mm256_mul_pd(pX[2], pY[2]);
        fmaUnroll[3] = _mm256_mul_pd(pX[3], pY[3]);
        for(i=4; i<fmaLen; i+=4) {
            fmaUnroll[0] = _mm256_add_pd(_mm256_mul_pd(pX[i], pY[i]), fmaUnroll[0]);
            fmaUnroll[1] = _mm256_add_pd(_mm256_mul_pd(pX[i+1], pY[i+1]), fmaUnroll[1]);
            fmaUnroll[2] = _mm256_add_pd(_mm256_mul_pd(pX[i+2], pY[i+2]), fmaUnroll[2]);
            fmaUnroll[3] = _mm256_add_pd(_mm256_mul_pd(pX[i+3], pY[i+3]), fmaUnroll[3]);
        }
        switch(avx2len-fmaLen) {
        case 3:
            fmaUnroll[2] = _mm256_add_pd(_mm256_mul_pd(pX[i+2], pY[i+2]), fmaUnroll[2]);
        case 2:
            fmaUnroll[1] = _mm256_add_pd(_mm256_mul_pd(pX[i+1], pY[i+1]), fmaUnroll[1]);
        case 1:
            fmaUnroll[0] = _mm256_add_pd(_mm256_mul_pd(pX[i], pY[i]), fmaUnroll[0]);
        }
        ans = _mm256_add_pd(
            _mm256_add_pd(fmaUnroll[0], fmaUnroll[1]),
            _mm256_add_pd(fmaUnroll[2], fmaUnroll[3]));
    }
    else {
        ans = _mm256_setzero_pd();
        switch(avx2len) {
        case 3:
            ans = _mm256_mul_pd(pX[2], pY[2]);
        case 2:
            ans = _mm256_add_pd(_mm256_mul_pd(pX[1], pY[1]), ans);
        case 1:
            ans = _mm256_add_pd(_mm256_mul_pd(pX[0], pY[0]), ans);
        }
    }
    const __m128d s = _mm_add_pd(_mm256_extractf128_pd(ans, 0), _mm256_extractf128_pd(ans, 1));
    const double *pComps = (const double*)&s;
    double dot =pComps[0] + pComps[1];
    const ae_int_t tail = avx2len<<2;
    switch(n-tail) {
    case 1: {
        dot += x[tail]*y[tail];
        break;
    }
    case 2: {
        dot += x[tail+0]*y[tail+0];
        dot += x[tail+1]*y[tail+1];
        break;
    }
    case 3: {
        dot += x[tail+0]*y[tail+0];
        dot += x[tail+1]*y[tail+1];
        dot += x[tail+2]*y[tail+2];
        break;
    }
    }
    return dot;
}

double rdotv2_avx2(const ae_int_t n,
    /* Real    */ const double* __restrict x,
    const ae_state* __restrict _state)
{
    ae_int_t i;

    const ae_int_t avx2len = n>>2;
    const ae_int_t fmaLen = (avx2len >> 2) << 2;
    const __m256d* __restrict pX = (const __m256d*)(x);
    __m256d ans;
    if(fmaLen >= 4) {
        //TODO: this can be further unrolled to 8 because AVX(2) provides 16 registers
        __m256d fmaUnroll[4];
        fmaUnroll[0] = _mm256_mul_pd(pX[0], pX[0]);
        fmaUnroll[1] = _mm256_mul_pd(pX[1], pX[1]);
        fmaUnroll[2] = _mm256_mul_pd(pX[2], pX[2]);
        fmaUnroll[3] = _mm256_mul_pd(pX[3], pX[3]);
        for(i=4; i<fmaLen; i+=4) {
            fmaUnroll[0] = _mm256_add_pd(_mm256_mul_pd(pX[i], pX[i]), fmaUnroll[0]);
            fmaUnroll[1] = _mm256_add_pd(_mm256_mul_pd(pX[i+1], pX[i+1]), fmaUnroll[1]);
            fmaUnroll[2] = _mm256_add_pd(_mm256_mul_pd(pX[i+2], pX[i+2]), fmaUnroll[2]);
            fmaUnroll[3] = _mm256_add_pd(_mm256_mul_pd(pX[i+3], pX[i+3]), fmaUnroll[3]);
        }
        switch(avx2len-fmaLen) {
        case 3:
            fmaUnroll[2] = _mm256_add_pd(_mm256_mul_pd(pX[i+2], pX[i+2]), fmaUnroll[2]);
        case 2:
            fmaUnroll[1] = _mm256_add_pd(_mm256_mul_pd(pX[i+1], pX[i+1]), fmaUnroll[1]);
        case 1:
            fmaUnroll[0] = _mm256_add_pd(_mm256_mul_pd(pX[i], pX[i]), fmaUnroll[0]);
        }
        ans = _mm256_add_pd(
            _mm256_add_pd(fmaUnroll[0], fmaUnroll[1]),
            _mm256_add_pd(fmaUnroll[2], fmaUnroll[3]));
    }
    else {
        ans = _mm256_setzero_pd();
        switch(avx2len) {
        case 3:
            ans = _mm256_mul_pd(pX[2], pX[2]);
        case 2:
            ans = _mm256_add_pd(_mm256_mul_pd(pX[1], pX[1]), ans);
        case 1:
            ans = _mm256_add_pd(_mm256_mul_pd(pX[0], pX[0]), ans);
        }
    }
    const __m128d s = _mm_add_pd(_mm256_extractf128_pd(ans, 0), _mm256_extractf128_pd(ans, 1));
    const double *pComps = (const double*)&s;
    double dot =pComps[0] + pComps[1];
    const ae_int_t tail = avx2len<<2;
    switch(n-tail) {
    case 1: {
        dot += x[tail]*x[tail];
        break;
    }
    case 2: {
        dot += x[tail+0]*x[tail+0];
        dot += x[tail+1]*x[tail+1];
        break;
    }
    case 3: {
        dot += x[tail+0]*x[tail+0];
        dot += x[tail+1]*x[tail+1];
        dot += x[tail+2]*x[tail+2];
        break;
    }
    }
    return dot;
}

void rcopyv_avx2(ae_int_t n,
     /* Real    */ const double* __restrict x,
     /* Real    */ double* __restrict y,
     ae_state* __restrict _state)
{
    ae_int_t i;

    const ae_int_t avx2len = n>>2;
    const ae_int_t tail = avx2len<<2;
    const __m256d* __restrict pSrc = (const __m256d*)(x);
    __m256d* __restrict pDest = (__m256d*)(y);
    for(i=0; i<avx2len; i++) {
        pDest[i] = pSrc[i];
    }
    switch(n-tail) {
    case 1:
        *(double*)(pDest+i) = *(const double*)(pSrc+i);
        break;
    case 2:
        *(__m128d*)(pDest+i) = *(const __m128d*)(pSrc+i);
        break;
    case 3:
        *(__m128d*)(pDest+i) = *(const __m128d*)(pSrc+i);
        y[tail+2] = x[tail+2];
        break;
    }
}

void rcopymulv_avx2(const ae_int_t n,
     const double v,
     /* Real    */ const double* __restrict x,
     /* Real    */ double* __restrict y,
     const ae_state* __restrict _state)
{
    ae_int_t i;
    
    const ae_int_t avx2len = n>>2;
    const ae_int_t tail = avx2len<<2;
    const __m256d* __restrict pSrc = (const __m256d*)(x);
    __m256d* __restrict pDest = (__m256d*)(y);
    const __m256d avx2v = _mm256_set1_pd(v);
    for(i=0; i<avx2len; i++) {
        pDest[i] = _mm256_mul_pd(avx2v, pSrc[i]);
    }
    switch(n-tail) {
    case 1:
        *(double*)(pDest+i) = v * (*(const double*)(pSrc+i));
        break;
    case 2:
        *(__m128d*)(pDest+i) = _mm_mul_pd(_mm256_extractf128_pd(avx2v, 0), *(const __m128d*)(pSrc+i));
        break;
    case 3:
        *(__m128d*)(pDest+i) = _mm_mul_pd(_mm256_extractf128_pd(avx2v, 0), *(const __m128d*)(pSrc+i));
        y[tail+2] = v*x[tail+2];
        break;
    }
}

void icopyv_avx2(const ae_int_t n, const ae_int_t* __restrict x,
                ae_int_t* __restrict y, ae_state* __restrict _state)
{
    const ae_int_t tail = (n*sizeof(ae_int_t)) & 31;
    const ae_int_t even = (n*sizeof(ae_int_t)) - tail;
    __m256i *__restrict pDest = (__m256i*)y;
    const __m256i* __restrict pSrc = (const __m256i*)x;
    const ae_int_t nVec = even>>5;
    ae_int_t i;
    for(i=0; i<nVec; i++) {
        pDest[i] = pSrc[i];
    }
    i = even/sizeof(ae_int_t);
    if(tail & 16) {
        *(__m128i*)(y+i) = *(const __m128i*)(x+i);
        i += 16/sizeof(ae_int_t);
    }
    if(tail & 8) {
        *(ae_int64_t*)(y+i) = *(const ae_int64_t*)(x+i);
        i += 8/sizeof(ae_int_t);
    }
    if(tail & 4) {
        *(ae_int32_t*)(y+i) = *(const ae_int32_t*)(x+i);
    }
}

void bcopyv_avx2(const ae_int_t n, const ae_bool* __restrict x,
                ae_bool* __restrict y, ae_state* __restrict _state)
{
    const ae_int_t tail = n & 31;
    const ae_int_t even = n-tail;
    __m256i *__restrict pDest = (__m256i*)y;
    const __m256i* __restrict pSrc = (const __m256i*)x;
    const ae_int_t nVec = even>>5;
    ae_int_t i;
    for(i=0; i<nVec; i++) {
        pDest[i] = pSrc[i];
    }
    i = even;
    if(tail & 16) {
        *(__m128i*)(y+i) = *(const __m128i*)(x+i);
        i += 16;
    }
    if(tail & 8) {
        *(ae_int64_t*)(y+i) = *(const ae_int64_t*)(x+i);
        i += 8;
    }
    if(tail & 4) {
        *(ae_int32_t*)(y+i) = *(const ae_int32_t*)(x+i);
        i += 4;
    }
    if(tail & 2) {
        *(y+i+0) = *(x+i+0);
        *(y+i+1) = *(x+i+1);
        i += 2;
    }
    if(tail & 1) {
        *(y+i) = *(x+i);
    }
}

void rsetv_avx2(const ae_int_t n,
     const double v,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state)
{
    ae_int_t i;
    
    const ae_int_t avx2len = n>>2;
    __m256d* __restrict pDest = (__m256d*)(x);
    const __m256d avx2v = _mm256_set1_pd(v);
    for(i=0; i<avx2len; i++) {
        pDest[i] = avx2v;
    }
    const ae_int_t tail = avx2len<<2;
    switch(n-tail) {
    case 1:
        *(double*)(pDest+i) = v;
        break;
    case 2:
        *(__m128d*)(pDest+i) = _mm256_extractf128_pd(avx2v, 0);
        break;
    case 3: 
        *(__m128d*)(pDest+i) = _mm256_extractf128_pd(avx2v, 0);
        x[tail+2] = v;
        break;
    }
}

void rsetvx_avx2(const ae_int_t n, const double v, double* __restrict x,
    const ae_state* __restrict _state)
{
    const ptrdiff_t unal = ((ptrdiff_t)x) & 31;
    if( n<=4 )
    {
        ae_int_t j;
        for(j=0; j<=n-1; j++)
            x[j] = v;
        return;
    }
    switch(unal)
    {
        case 0:
            rsetv_avx2(n, v, x, _state);
            return;
        case 8:
            x[2] = v;
        case 16:
            x[1] = v;
        case 24:
        {
            x[0] = v;
            const ptrdiff_t nDone = 4-(unal>>3);
            rsetv_avx2(n-nDone, v, x+nDone, _state);
            return;
        }
    }
}

void isetv_avx2(const ae_int_t n, const ae_int_t v,
    ae_int_t* __restrict x, ae_state* __restrict _state)
{
    const ae_int_t tail = (n*sizeof(ae_int_t)) & 31;
    const ae_int_t even = (n*sizeof(ae_int_t)) - tail;
    __m256i *__restrict pDest = (__m256i*)x;
    const __m256i avx2v = ((sizeof(v) == 4) ? _mm256_set1_epi32((ae_int32_t)v) : _mm256_set1_epi64x(v));
    const ae_int_t nVec = even>>5;
    ae_int_t i;
    for(i=0; i<nVec; i++) {
        pDest[i] = avx2v;
    }
    memmove(pDest+i, &avx2v, tail);
}

void bsetv_avx2(const ae_int_t n, const ae_bool v, ae_bool* __restrict x,
    ae_state* __restrict _state)
{
    const ae_int_t tail = n & 31;
    const ae_int_t even = n-tail;
    __m256i *__restrict pDest = (__m256i*)x;
    const __m256i avx2v = _mm256_set1_epi8(v);
    const ae_int_t nVec = even>>5;
    ae_int_t i;
    for(i=0; i<nVec; i++) {
        pDest[i] = avx2v;
    }
    /* _mm256_extracti128_si256() has a too high latency on latest processors (Skylake+) */
    memset(x+even, v, tail);
}

void rmulv_avx2(const ae_int_t n, const double v, double* __restrict x,
     const ae_state* __restrict _state)
{
    ae_int_t i;
    
    const ae_int_t avx2len = n>>2;
    __m256d* __restrict pDest = (__m256d*)(x);
    const __m256d avx2v = _mm256_set1_pd(v);
    for(i=0; i<avx2len; i++) {
        pDest[i] = _mm256_mul_pd(avx2v, pDest[i]);
    }
    const ae_int_t tail = avx2len<<2;
    switch(n-tail) {
    case 1:
        *(double*)(pDest+i) = v * (*(const double*)(pDest+i));
        break;
    case 2:
        *(__m128d*)(pDest+i) = _mm_mul_pd(_mm256_extractf128_pd(avx2v, 0), *(const __m128d*)(pDest+i));
        break;
    case 3:
        *(__m128d*)(pDest+i) = _mm_mul_pd(_mm256_extractf128_pd(avx2v, 0), *(const __m128d*)(pDest+i));
        x[tail+2] *= v;
        break;
    }
}

void rsqrtv_avx2(const ae_int_t n, double* __restrict x, const ae_state* __restrict _state)
{
    ae_int_t i;
    
    const ae_int_t avx2len = n>>2;
    const ae_int_t tail = avx2len<<2;
    __m256d* __restrict pDest = (__m256d*)(x);
    for(i=0; i<avx2len; i++)
        pDest[i] = _mm256_sqrt_pd(pDest[i]);
    for(i=tail; i<n; i++)
        x[i] = sqrt(x[i]);
}

void rmulvx_avx2(const ae_int_t n, const double v, double* __restrict x,
    const ae_state* __restrict _state)
{
    const ptrdiff_t unal = ((ptrdiff_t)x) & 31;
    if( n<=4 )
    {
        ae_int_t i;
        for(i=0; i<=n-1; i++)
            x[i] *= v;
        return;
    }
    switch(unal) {
    case 0:
        rmulv_avx2(n, v, x, _state);
        return;
    case 8:
        x[2] = v*x[2];
    case 16:
        x[1] = v*x[1];
    case 24: {
        x[0] = v*x[0];
        const ptrdiff_t nDone = 4-(unal>>3);
        rmulv_avx2(n-nDone, v, x+nDone, _state);
        return;
    }
    }
}

void raddv_avx2(const ae_int_t n,
     const double alpha,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state)
{
    ae_int_t i;
    
    const ae_int_t avx2len = n>>2;
    const __m256d* __restrict pSrc = (const __m256d*)(y);
    __m256d* __restrict pDest = (__m256d*)(x);
    const __m256d avx2alpha = _mm256_set1_pd(alpha);
    for(i=0; i<avx2len; i++) {
        pDest[i] = _mm256_add_pd(_mm256_mul_pd(avx2alpha, pSrc[i]), pDest[i]);
    }
    const ae_int_t tail = avx2len<<2;
    switch(n-tail) {
    case 1:
        *(double*)(pDest+i) = alpha * (*(const double*)(pSrc+i))
            + (*(const double*)(pDest+i));
        break;
    case 2:
        *(__m128d*)(pDest+i) = _mm_add_pd(_mm_mul_pd(_mm256_extractf128_pd(avx2alpha, 0), *(const __m128d*)(pSrc+i)),*(const __m128d*)(pDest+i));
        break;
    case 3:
        *(__m128d*)(pDest+i) = _mm_add_pd(_mm_mul_pd(_mm256_extractf128_pd(avx2alpha, 0), *(const __m128d*)(pSrc+i)),*(const __m128d*)(pDest+i));
        x[tail+2] += alpha*y[tail+2];
        break;
    }
}

void raddvx_avx_xaligned(const ae_int_t n, const double alpha, const double* __restrict y, double* __restrict x, ae_state *_state)
{
    ae_int_t i;
    const ae_int_t vecLen = (n>>2)<<2;
    const __m256d avx2alpha = _mm256_set1_pd(alpha);
    __m256d* __restrict pDest = (__m256d*)x;
    for(i=0; i<vecLen; i+=4)
    {
        const ae_int_t iDest = i>>2;
        pDest[iDest] = _mm256_add_pd(_mm256_mul_pd(avx2alpha, _mm256_loadu_pd(y+i)), pDest[iDest]);
    }
    switch(n-vecLen) {
    case 1:
        x[i] += alpha*y[i];
        break;
    case 2: {
        const ae_int_t iDest = i>>2;
        *(__m128d*)(pDest+iDest) = _mm_add_pd(_mm_mul_pd(_mm256_extractf128_pd(avx2alpha, 0),_mm_loadu_pd(y+i)),*(const __m128d*)(pDest+iDest));
        break;
    }
    case 3:
    {
        const ae_int_t iDest = i>>2;
        *(__m128d*)(pDest+iDest) = _mm_add_pd(_mm_mul_pd(_mm256_extractf128_pd(avx2alpha, 0),_mm_loadu_pd(y+i)),*(const __m128d*)(pDest+iDest));
        x[i+2] += alpha*y[i+2];
        break;
    }
    }
}

void raddvx_avx2(const ae_int_t n, const double alpha, const double* __restrict y,
    double* __restrict x, ae_state *_state)
{
    const ptrdiff_t unal = ((ptrdiff_t)x) & 31;
    if( n<=4 )
    {
        ae_int_t i;
        for(i=0; i<=n-1; i++)
            x[i] += alpha*y[i];
        return;
    }
    switch(unal)
    {
    case 0:
        raddvx_avx_xaligned(n, alpha, y, x, _state);
        return;
    case 8:
        x[2] += alpha*y[2];
    case 16:
        x[1] += alpha*y[1];
    case 24:
        {
            x[0] += alpha*y[0];
            const ptrdiff_t nDone = 4-(unal>>3);
            raddvx_avx_xaligned(n-nDone, alpha, y+nDone, x+nDone, _state);
            return;
        }
    }
}

void rmergemulv_avx2(ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state)
{
    ae_int_t i;

    const ae_int_t avx2len = n>>2;
    const __m256d* __restrict pSrc = (const __m256d*)(y);
    __m256d* __restrict pDest = (__m256d*)(x);
    for(i=0; i<avx2len; i++) {
        pDest[i] = _mm256_mul_pd(pSrc[i], pDest[i]);
    }
    const ae_int_t tail = avx2len<<2;
    switch(n-tail) {
    case 1:
        *(double*)(pDest+i) = *(const double*)(pSrc+i)
            * (*(const double*)(pDest+i));
        break;
    case 2:
        *(__m128d*)(pDest+i) = _mm_mul_pd(
            *(const __m128d*)(pSrc+i), *(const __m128d*)(pDest+i));
        break;
    case 3: {
        *(__m128d*)(pDest+i) = _mm_mul_pd(*(const __m128d*)(pSrc+i), *(const __m128d*)(pDest+i));
        ((double*)(pDest+i))[2] *= ((const double*)(pSrc+i))[2];
        break;
    }
    }
}

void rmergedivv_avx2(ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     const ae_state* __restrict _state)
{
    ae_int_t i;

    const ae_int_t avx2len = n>>2;
    const __m256d* __restrict pSrc = (const __m256d*)(y);
    __m256d* __restrict pDest = (__m256d*)(x);
    for(i=0; i<avx2len; i++) {
        pDest[i] = _mm256_div_pd(pDest[i], pSrc[i]);
    }
    const ae_int_t tail = avx2len<<2;
    switch(n-tail) {
    case 1:
        *(double*)(pDest+i) = (*(const double*)(pDest+i)) / (*(const double*)(pSrc+i));
        break;
    case 2:
        *(__m128d*)(pDest+i) = _mm_div_pd(*(const __m128d*)(pDest+i),  *(const __m128d*)(pSrc+i));
        break;
    case 3: {
        *(__m128d*)(pDest+i) = _mm_div_pd(*(const __m128d*)(pDest+i), *(const __m128d*)(pSrc+i));
        ((double*)(pDest+i))[2] /= ((const double*)(pSrc+i))[2];
        break;
    }
    }
}

void rmergemaxv_avx2(ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     ae_state* __restrict _state)
{
    ae_int_t i;

    const ae_int_t avx2len = n>>2;
    const __m256d* __restrict pSrc = (const __m256d*)(y);
    __m256d* __restrict pDest = (__m256d*)(x);
    for(i=0; i<avx2len; i++) {
        pDest[i] = _mm256_max_pd(pSrc[i], pDest[i]);
    }
    const ae_int_t tail = avx2len<<2;
    switch(n-tail) {
    case 1:
        *(double*)(pDest+i) = *(const double*)(pSrc+i)>*(const double*)(pDest+i) ? *(const double*)(pSrc+i) : *(const double*)(pDest+i);
        break;
    case 2:
        *(__m128d*)(pDest+i) = _mm_max_pd(*(const __m128d*)(pSrc+i), *(const __m128d*)(pDest+i));
        break;
    case 3:
    {
        double  s2 = ((const double*)(pSrc+i))[2];
        double *d2 = ((double*)(pDest+i))+2;
        *(__m128d*)(pDest+i) = _mm_max_pd(*(const __m128d*)(pSrc+i), *(const __m128d*)(pDest+i));
        *d2 = s2>*d2 ? s2 : *d2;
        break;
    }
    }
}

void rmergeminv_avx2(ae_int_t n,
     /* Real    */ const double* __restrict y,
     /* Real    */ double* __restrict x,
     ae_state* __restrict _state)
{
    ae_int_t i;

    const ae_int_t avx2len = n>>2;
    const __m256d* __restrict pSrc = (const __m256d*)(y);
    __m256d* __restrict pDest = (__m256d*)(x);
    for(i=0; i<avx2len; i++) {
        pDest[i] = _mm256_min_pd(pSrc[i], pDest[i]);
    }
    const ae_int_t tail = avx2len<<2;
    switch(n-tail) {
    case 1:
        *(double*)(pDest+i) = ae_minreal(*(const double*)(pSrc+i),
            *(const double*)(pDest+i), _state);
        break;
    case 2:
        *(__m128d*)(pDest+i) = _mm_min_pd(
            *(const __m128d*)(pSrc+i), *(const __m128d*)(pDest+i));
        break;
    case 3: {
        double  s2 = ((const double*)(pSrc+i))[2];
        double *d2 = ((double*)(pDest+i))+2;
        *(__m128d*)(pDest+i) = _mm_min_pd(*(const __m128d*)(pSrc+i), *(const __m128d*)(pDest+i));
        *d2 = s2<*d2 ? s2 : *d2;
        break;
    }
    }
}

double rmaxv_avx2(ae_int_t n, /* Real    */ const double* __restrict x, ae_state* __restrict _state)
{
    ae_int_t i;
    const ae_int_t avx2len = n>>2;
    const ae_int_t tail = avx2len<<2;
    const __m256d* __restrict pSrc = (const __m256d*)(x);
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
    __m256d curMax = pSrc[0];
    for(i=1; i<avx2len; i++) {
        curMax = _mm256_max_pd(curMax, pSrc[i]);
    }
    const __m128d sseMax = _mm_max_pd(_mm256_extractf128_pd(curMax, 0), _mm256_extractf128_pd(curMax, 1));
    const double *pComps = (const double *)&sseMax;
    double dMax = (pComps[0] > pComps[1]) ? pComps[0] : pComps[1];
    const double *p_tail = (const double*)(pSrc+i);
    switch(n-tail)
    {
        case 1:
        {
            dMax = p_tail[0]>dMax ? p_tail[0] : dMax;
            break;
        }
        case 2: {
            dMax = p_tail[0]>dMax ? p_tail[0] : dMax;
            dMax = p_tail[1]>dMax ? p_tail[1] : dMax;
            break;
        }
        case 3: {
            dMax = p_tail[0]>dMax ? p_tail[0] : dMax;
            dMax = p_tail[1]>dMax ? p_tail[1] : dMax;
            dMax = p_tail[2]>dMax ? p_tail[2] : dMax;
            break;
        }
    }
    return dMax;
}

double rmaxabsv_avx2(ae_int_t n, /* Real    */ const double* __restrict x, ae_state* __restrict _state)
{
    const __m256d signMask = _mm256_set1_pd(-0.); // -0. = 1 << 63
    const ae_int_t avx2len = n>>2;
    const __m256d* __restrict pSrc = (const __m256d*)(x);
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
    __m256d curMax = _mm256_andnot_pd(signMask, pSrc[0]); // abs
    ae_int_t i;
    for(i=1; i<avx2len; i++) {
        curMax = _mm256_max_pd(curMax, _mm256_andnot_pd(signMask, pSrc[i])); // abs
    }
    const ae_int_t tail = avx2len<<2;
    const __m128d sseMax = _mm_max_pd(_mm256_extractf128_pd(curMax, 0), _mm256_extractf128_pd(curMax, 1));
    const double *p_tail = (const double*)(pSrc+i);
    const double *pComps = (const double *)&sseMax;
    double dMax = (pComps[0] > pComps[1]) ? pComps[0] : pComps[1];
    switch(n-tail)
    {
        case 1:
        {
            double a0 = fabs(p_tail[0]);
            dMax = a0>dMax ? a0 : dMax;
            break;
        }
        case 2:
        {
            double a0 = fabs(p_tail[0]);
            double a1 = fabs(p_tail[1]);
            dMax = a0>dMax ? a0 : dMax;
            dMax = a1>dMax ? a1 : dMax;
            break;
        }
        case 3:
        {
            double a0 = fabs(p_tail[0]);
            double a1 = fabs(p_tail[1]);
            double a2 = fabs(p_tail[2]);
            dMax = a0>dMax ? a0 : dMax;
            dMax = a1>dMax ? a1 : dMax;
            dMax = a2>dMax ? a2 : dMax;
            break;
        }
    }
    return dMax;
}

static void rcopyvx_avx2_xaligned(const ae_int_t n, const double* __restrict x,
    double* __restrict y, ae_state *_state)
{
    ae_int_t i;
    const ae_int_t vecLen = (n>>2)<<2;
    const __m256d* __restrict pSrc = (const __m256d*)x;
    for(i=0; i<vecLen; i+=4) {
        const ae_int_t iSrc = i>>2;
        _mm256_storeu_pd(y+i, pSrc[iSrc]);
    }
    switch(n-vecLen) {
    case 1:
        y[i] = x[i];
        break;
    case 2: {
        const ae_int_t iSrc = i>>2;
        _mm_storeu_pd(y+i, *(const __m128d*)(pSrc+iSrc));
        break;
    }
    case 3: {
        const ae_int_t iSrc = i>>2;
        const __m256d t = pSrc[iSrc];
        _mm_storeu_pd(y+i, _mm256_extractf128_pd(t, 0));
        y[i+2] = *(((const double*)&t)+2);
        break;
    }
    }
}

void rcopyvx_avx2(const ae_int_t n, const double* __restrict x,
    double* __restrict y, ae_state *_state)
{
    const ptrdiff_t unal = ((ptrdiff_t)x) & 31;
    if( n<=4 )
    {
        ae_int_t j;
        for(j=0; j<n; j++)
            y[j] = x[j];
        return;
    }
    switch(unal) {
    case 0:
        rcopyvx_avx2_xaligned(n, x, y, _state);
        return;
    case 8:
        y[2] = x[2];
    case 16:
        y[1] = x[1];
    case 24: {
        y[0] = x[0];
        const ptrdiff_t nDone = 4-(unal>>3);
        rcopyvx_avx2_xaligned(n-nDone, x+nDone, y+nDone, _state);
        return;
    }
    }
}

static void icopyvx_avx2_xaligned(const ae_int_t n, const ae_int_t* __restrict x,
                ae_int_t* __restrict y, ae_state* __restrict _state)
{
    const ae_int_t tail = (n*sizeof(ae_int_t)) & 31;
    const ae_int_t even = (n*sizeof(ae_int_t)) - tail;
    const __m256i* __restrict pSrc = (const __m256i*)x;
    const ae_int_t nVec = even>>5;
    const ae_int_t shift_by = 3-sizeof(ae_int_t)/8;
    ae_int_t i;
    for(i=0; i<nVec; i++) {
        const ae_int_t j = i<<shift_by;
        _mm256_storeu_si256((__m256i*)(y+j), pSrc[i]);
    }
    i = even/sizeof(ae_int_t);
    if(tail & 16) {
        _mm_storeu_si128((__m128i*)(y+i), *(const __m128i*)(x+i));
        i += 16/sizeof(ae_int_t);
    }
    if(tail & 8) {
        *(ae_int64_t*)(y+i) = *(const ae_int64_t*)(x+i);
        i += 8/sizeof(ae_int_t);
    }
    if(tail & 4) {
        *(ae_int32_t*)(y+i) = *(const ae_int32_t*)(x+i);
    }
}

void icopyvx_avx2(const ae_int_t n, const ae_int_t* __restrict x,
                ae_int_t* __restrict y, ae_state* __restrict _state)
{
    const ptrdiff_t unal = ((ptrdiff_t)x) & 31;
    if( n<=8 )
    {
        ae_int_t j;
        for(j=0; j<=n-1; j++)
            y[j] = x[j];
        return;
    }
    if(unal == 0)
    {
        icopyvx_avx2_xaligned(n, x, y, _state);
        return;
    }
    const ae_int_t offset = 32-unal;
    memmove(y, x, offset);
    const ae_int_t nDone = offset / sizeof(ae_int_t);
    icopyvx_avx2_xaligned(n-nDone, x+nDone, y+nDone, _state);
}

void rgemv_straight_avx2(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a,
    const double* __restrict x, double* __restrict y, ae_state* _state)
{
    ae_int_t i;
    ae_int_t j;
    const __m256d* __restrict pX = (const __m256d*)x;
    const ae_int_t nVec = n >> 2;
    const ae_int_t nUnroll = nVec >> 3;
    __m256d sum = _mm256_setzero_pd();
    for(i=0; i<m; i++) {
        const __m256d* __restrict pRow = (const __m256d*)a->ptr.pp_double[i];
        if(nUnroll >= 1) {
            __m256d u0 = _mm256_mul_pd(pRow[0], pX[0]);
            __m256d u1 = _mm256_mul_pd(pRow[1], pX[1]);
            __m256d u2 = _mm256_mul_pd(pRow[2], pX[2]);
            __m256d u3 = _mm256_mul_pd(pRow[3], pX[3]);
            __m256d u4 = _mm256_mul_pd(pRow[4], pX[4]);
            __m256d u5 = _mm256_mul_pd(pRow[5], pX[5]);
            __m256d u6 = _mm256_mul_pd(pRow[6], pX[6]);
            __m256d u7 = _mm256_mul_pd(pRow[7], pX[7]);
            for(j=1; j<nUnroll; j++) {
              const ae_int_t at = j<<3;
              u0 = _mm256_add_pd(u0, _mm256_mul_pd(pRow[at], pX[at]));
              u1 = _mm256_add_pd(u1, _mm256_mul_pd(pRow[at+1], pX[at+1]));
              u2 = _mm256_add_pd(u2, _mm256_mul_pd(pRow[at+2], pX[at+2]));
              u3 = _mm256_add_pd(u3, _mm256_mul_pd(pRow[at+3], pX[at+3]));
              u4 = _mm256_add_pd(u4, _mm256_mul_pd(pRow[at+4], pX[at+4]));
              u5 = _mm256_add_pd(u5, _mm256_mul_pd(pRow[at+5], pX[at+5]));
              u6 = _mm256_add_pd(u6, _mm256_mul_pd(pRow[at+6], pX[at+6]));
              u7 = _mm256_add_pd(u7, _mm256_mul_pd(pRow[at+7], pX[at+7]));
            }
            const ae_int_t at = j<<3;
            switch(nVec-at) {
            case 7:
                u6 = _mm256_add_pd(_mm256_mul_pd(pX[at+6], pRow[at+6]), u6);
            case 6:
                u5 = _mm256_add_pd(_mm256_mul_pd(pX[at+5], pRow[at+5]), u5);
            case 5:
                u4 = _mm256_add_pd(_mm256_mul_pd(pX[at+4], pRow[at+4]), u4);
            case 4:
                u3 = _mm256_add_pd(_mm256_mul_pd(pX[at+3], pRow[at+3]), u3);
            case 3:
                u2 = _mm256_add_pd(_mm256_mul_pd(pX[at+2], pRow[at+2]), u2);
            case 2:
                u1 = _mm256_add_pd(_mm256_mul_pd(pX[at+1], pRow[at+1]), u1);
            case 1:
                u0 = _mm256_add_pd(_mm256_mul_pd(pX[at+0], pRow[at+0]), u0);
            }
            sum = _mm256_add_pd(
                _mm256_add_pd(_mm256_add_pd(u0, u1), _mm256_add_pd(u2, u3)),
                _mm256_add_pd(_mm256_add_pd(u4, u5), _mm256_add_pd(u6, u7)));
        }
        else {
            switch(nVec) {
            case 0:
                sum = _mm256_setzero_pd();
                break;
            case 1:
                sum = _mm256_mul_pd(pX[0], pRow[0]);
                break;
            case 2:
                sum = _mm256_add_pd(_mm256_mul_pd(pX[0], pRow[0]), _mm256_mul_pd(pX[1], pRow[1]));
                break;
            case 3:
                sum = _mm256_add_pd(
                    _mm256_add_pd(_mm256_mul_pd(pX[0], pRow[0]), _mm256_mul_pd(pX[1], pRow[1])),
                    _mm256_mul_pd(pX[2], pRow[2]));
                break;
            case 4:
                sum = _mm256_add_pd(
                    _mm256_add_pd(_mm256_mul_pd(pX[0], pRow[0]), _mm256_mul_pd(pX[1], pRow[1])),
                    _mm256_add_pd(_mm256_mul_pd(pX[2], pRow[2]), _mm256_mul_pd(pX[3], pRow[3])));
                break;
            case 5:
                sum = _mm256_add_pd(
                    _mm256_add_pd(
                        _mm256_add_pd(_mm256_mul_pd(pX[0], pRow[0]), _mm256_mul_pd(pX[1], pRow[1])),
                        _mm256_add_pd(_mm256_mul_pd(pX[2], pRow[2]), _mm256_mul_pd(pX[3], pRow[3]))),
                    _mm256_mul_pd(pX[4], pRow[4]));
                break;
            case 6:
                sum = _mm256_add_pd(
                    _mm256_add_pd(
                        _mm256_add_pd(_mm256_mul_pd(pX[0], pRow[0]), _mm256_mul_pd(pX[1], pRow[1])),
                        _mm256_add_pd(_mm256_mul_pd(pX[2], pRow[2]), _mm256_mul_pd(pX[3], pRow[3]))),
                    _mm256_add_pd(_mm256_mul_pd(pX[4], pRow[4]), _mm256_mul_pd(pX[5], pRow[5])));
                break;
            case 7:
                sum = _mm256_add_pd(
                    _mm256_add_pd(
                        _mm256_add_pd(_mm256_mul_pd(pX[0], pRow[0]), _mm256_mul_pd(pX[1], pRow[1])),
                        _mm256_add_pd(_mm256_mul_pd(pX[2], pRow[2]), _mm256_mul_pd(pX[3], pRow[3]))),
                    _mm256_add_pd(
                        _mm256_add_pd(_mm256_mul_pd(pX[4], pRow[4]), _mm256_mul_pd(pX[5], pRow[5])),
                        _mm256_mul_pd(pX[6], pRow[6])));
                break;
            }
        }
        const __m128d t = _mm_add_pd(_mm256_extractf128_pd(sum, 0), _mm256_extractf128_pd(sum, 1));
        const double* pComps = (const double*)&t;
        double ans = pComps[0] + pComps[1];
        const ae_int_t tail = nVec<<2;
        for(j=tail; j<n; j++) {
          ans += a->ptr.pp_double[i][j] * x[j];
        }
        y[i] += alpha*ans;
    }
}

void rgemv_transposed_avx2(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a,
    const double* __restrict x, double* __restrict y, ae_state* _state)
{
    ae_int_t i;
    ae_int_t j;
    __m256d* __restrict pY = (__m256d*)y;
    const ae_int_t nVec = m >> 2;

    for(i=0; i<=n-1; i++)
    {
        const __m256d* __restrict pRow = (const __m256d*)a->ptr.pp_double[i];
        const double v = alpha*x[i];
        const __m256d vV = _mm256_set1_pd(v);
        for(j=0; j<nVec; j++)
        {
            pY[j] = _mm256_add_pd(_mm256_mul_pd(vV, pRow[j]), pY[j]);
        }
        const ae_int_t tail = nVec<<2;
        for(j=tail; j<m; j++) {
            y[j] += v*a->ptr.pp_double[i][j];
        }
    }
}

void rgemvx_straight_avx2_xaligned(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a, const ae_int_t ia,
    const ae_int_t ja, const double* __restrict x,
    double* __restrict y, ae_state* _state)
{
    ae_int_t i;
    ae_int_t j;
    const __m256d* __restrict pX = (const __m256d*)x;
    const ae_int_t nVec = n >> 2;
    const ae_int_t nUnroll = nVec >> 3;
    __m256d sum = _mm256_setzero_pd();
    for(i=0; i<m; i++) {
        const __m256d* __restrict pRow = (const __m256d*)(a->ptr.pp_double[i+ia]+ja);
        if(nUnroll >= 1) {
            __m256d u0 = _mm256_mul_pd(ULOAD256PD(pRow[0]), pX[0]);
            __m256d u1 = _mm256_mul_pd(ULOAD256PD(pRow[1]), pX[1]);
            __m256d u2 = _mm256_mul_pd(ULOAD256PD(pRow[2]), pX[2]);
            __m256d u3 = _mm256_mul_pd(ULOAD256PD(pRow[3]), pX[3]);
            __m256d u4 = _mm256_mul_pd(ULOAD256PD(pRow[4]), pX[4]);
            __m256d u5 = _mm256_mul_pd(ULOAD256PD(pRow[5]), pX[5]);
            __m256d u6 = _mm256_mul_pd(ULOAD256PD(pRow[6]), pX[6]);
            __m256d u7 = _mm256_mul_pd(ULOAD256PD(pRow[7]), pX[7]);
            for(j=1; j<nUnroll; j++) {
              const ae_int_t at = j<<3;
              u0 = _mm256_add_pd(u0, _mm256_mul_pd(ULOAD256PD(pRow[at]), pX[at]));
              u1 = _mm256_add_pd(u1, _mm256_mul_pd(ULOAD256PD(pRow[at+1]), pX[at+1]));
              u2 = _mm256_add_pd(u2, _mm256_mul_pd(ULOAD256PD(pRow[at+2]), pX[at+2]));
              u3 = _mm256_add_pd(u3, _mm256_mul_pd(ULOAD256PD(pRow[at+3]), pX[at+3]));
              u4 = _mm256_add_pd(u4, _mm256_mul_pd(ULOAD256PD(pRow[at+4]), pX[at+4]));
              u5 = _mm256_add_pd(u5, _mm256_mul_pd(ULOAD256PD(pRow[at+5]), pX[at+5]));
              u6 = _mm256_add_pd(u6, _mm256_mul_pd(ULOAD256PD(pRow[at+6]), pX[at+6]));
              u7 = _mm256_add_pd(u7, _mm256_mul_pd(ULOAD256PD(pRow[at+7]), pX[at+7]));
            }
            const ae_int_t at = j<<3;
            switch(nVec-at) {
            case 7:
                u6 = _mm256_add_pd(_mm256_mul_pd(pX[at+6], ULOAD256PD(pRow[at+6])), u6);
            case 6:
                u5 = _mm256_add_pd(_mm256_mul_pd(pX[at+5], ULOAD256PD(pRow[at+5])), u5);
            case 5:
                u4 = _mm256_add_pd(_mm256_mul_pd(pX[at+4], ULOAD256PD(pRow[at+4])), u4);
            case 4:
                u3 = _mm256_add_pd(_mm256_mul_pd(pX[at+3], ULOAD256PD(pRow[at+3])), u3);
            case 3:
                u2 = _mm256_add_pd(_mm256_mul_pd(pX[at+2], ULOAD256PD(pRow[at+2])), u2);
            case 2:
                u1 = _mm256_add_pd(_mm256_mul_pd(pX[at+1], ULOAD256PD(pRow[at+1])), u1);
            case 1:
                u0 = _mm256_add_pd(_mm256_mul_pd(pX[at+0], ULOAD256PD(pRow[at+0])), u0);
            }
            sum = _mm256_add_pd(
                _mm256_add_pd(_mm256_add_pd(u0, u1), _mm256_add_pd(u2, u3)),
                _mm256_add_pd(_mm256_add_pd(u4, u5), _mm256_add_pd(u6, u7)));
        }
        else {
            switch(nVec) {
            case 0:
                sum = _mm256_setzero_pd();
                break;
            case 1:
                sum = _mm256_mul_pd(pX[0], ULOAD256PD(pRow[0]));
                break;
            case 2:
                sum = _mm256_add_pd(
                    _mm256_mul_pd(pX[0], ULOAD256PD(pRow[0])),
                    _mm256_mul_pd(pX[1], ULOAD256PD(pRow[1])));
                break;
            case 3:
                sum = _mm256_add_pd(
                    _mm256_add_pd(
                        _mm256_mul_pd(pX[0], ULOAD256PD(pRow[0])),
                        _mm256_mul_pd(pX[1], ULOAD256PD(pRow[1]))),
                    _mm256_mul_pd(pX[2], ULOAD256PD(pRow[2])));
                break;
            case 4:
                sum = _mm256_add_pd(
                    _mm256_add_pd(
                        _mm256_mul_pd(pX[0], ULOAD256PD(pRow[0])),
                        _mm256_mul_pd(pX[1], ULOAD256PD(pRow[1]))),
                    _mm256_add_pd(
                        _mm256_mul_pd(pX[2], ULOAD256PD(pRow[2])),
                        _mm256_mul_pd(pX[3], ULOAD256PD(pRow[3]))));
                break;
            case 5:
                sum = _mm256_add_pd(
                    _mm256_add_pd(
                        _mm256_add_pd(
                            _mm256_mul_pd(pX[0], ULOAD256PD(pRow[0])),
                            _mm256_mul_pd(pX[1], ULOAD256PD(pRow[1]))),
                        _mm256_add_pd(
                            _mm256_mul_pd(pX[2], ULOAD256PD(pRow[2])),
                            _mm256_mul_pd(pX[3], ULOAD256PD(pRow[3])))),
                    _mm256_mul_pd(pX[4], ULOAD256PD(pRow[4])));
                break;
            case 6:
                sum = _mm256_add_pd(
                    _mm256_add_pd(
                        _mm256_add_pd(
                            _mm256_mul_pd(pX[0], ULOAD256PD(pRow[0])),
                            _mm256_mul_pd(pX[1], ULOAD256PD(pRow[1]))),
                        _mm256_add_pd(
                            _mm256_mul_pd(pX[2], ULOAD256PD(pRow[2])),
                            _mm256_mul_pd(pX[3], ULOAD256PD(pRow[3])))),
                    _mm256_add_pd(
                        _mm256_mul_pd(pX[4], ULOAD256PD(pRow[4])),
                        _mm256_mul_pd(pX[5], ULOAD256PD(pRow[5]))));
                break;
            case 7:
                sum = _mm256_add_pd(
                    _mm256_add_pd(
                        _mm256_add_pd(
                            _mm256_mul_pd(pX[0], ULOAD256PD(pRow[0])),
                            _mm256_mul_pd(pX[1], ULOAD256PD(pRow[1]))),
                        _mm256_add_pd(
                            _mm256_mul_pd(pX[2], ULOAD256PD(pRow[2])),
                            _mm256_mul_pd(pX[3], ULOAD256PD(pRow[3])))),
                    _mm256_add_pd(
                        _mm256_add_pd(
                            _mm256_mul_pd(pX[4], ULOAD256PD(pRow[4])),
                            _mm256_mul_pd(pX[5], ULOAD256PD(pRow[5]))),
                        _mm256_mul_pd(pX[6], ULOAD256PD(pRow[6]))));
                break;
            }
        }
        const __m128d t = _mm_add_pd(_mm256_extractf128_pd(sum, 0), _mm256_extractf128_pd(sum, 1));
        const double* pComps = (const double*)&t;
        double ans = pComps[0] + pComps[1];
        const ae_int_t tail = nVec<<2;
        for(j=tail; j<n; j++) {
          ans += a->ptr.pp_double[i+ia][j+ja] * x[j];
        }
        y[i] += alpha*ans;
    }
}

void rgemvx_straight_avx2(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a, const ae_int_t ia,
    const ae_int_t ja, const double* __restrict x,
    double* __restrict y, ae_state* _state)
{
    ae_int_t i;
    ae_int_t j;
    if( n<=3 ) {
        for(i=0; i<m; i++) {
            const double *p_a = a->ptr.pp_double[ia+i]+ja;
            double v = 0.0;
            for(j=0; j<n; j++) {
                v += p_a[j] * x[j];
            }
            y[i] += alpha*v;
        }
        return;
    }

    const ptrdiff_t unal = ((ptrdiff_t)x) & 31;
    if(unal == 0)
    {
        rgemvx_straight_avx2_xaligned(m, n, alpha, a, ia, ja, x, y, _state);
        return;
    }
    const ptrdiff_t shift = 4-(unal>>3);
    for(i=0; i<m; i++) {
        const double *p_a = a->ptr.pp_double[ia+i]+ja;
        double v = 0.0;
        for(j=0; j<shift; j++) {
            v += p_a[j] * x[j];
        }
        y[i] += alpha*v;
    }
    rgemvx_straight_avx2_xaligned(m, n-shift, alpha, a, ia, ja+shift, x+shift, y, _state);
}

void rgemvx_transposed_avx2_yaligned(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a, const ae_int_t ia,
    const ae_int_t ja, const double* __restrict x, double* __restrict y,
    ae_state* _state)
{
    ae_int_t i;
    ae_int_t j;
    __m256d* __restrict pY = (__m256d*)y;
    const ae_int_t nVec = m >> 2;

    for(i=0; i<=n-1; i++)
    {
        const __m256d* __restrict pRow = (const __m256d*)(a->ptr.pp_double[i+ia]+ja);
        const double v = alpha*x[i];
        const __m256d vV = _mm256_set1_pd(v);
        for(j=0; j<nVec; j++)
        {
            pY[j] = _mm256_add_pd(_mm256_mul_pd(vV, ULOAD256PD(pRow[j])), pY[j]);
        }
        const ae_int_t tail = nVec<<2;
        for(j=tail; j<m; j++) {
            y[j] += v*a->ptr.pp_double[i+ia][j+ja];
        }
    }
}

void rgemvx_transposed_avx2(const ae_int_t m, const ae_int_t n,
    const double alpha, const ae_matrix* __restrict a, const ae_int_t ia,
    const ae_int_t ja, const double* __restrict x, double* __restrict y,
    ae_state* _state)
{
    ae_int_t i;
    ae_int_t j;
    if( m<=3 ) {
        for(i=0; i<n; i++) {
            const double *p_a = a->ptr.pp_double[ia+i]+ja;
            const double v = alpha*x[i];
            for(j=0; j<m; j++) {
                y[j] += v*p_a[j];
            }
        }
        return;
    }

    const ptrdiff_t unal = ((ptrdiff_t)y) & 31;
    if(unal == 0)
    {
        rgemvx_transposed_avx2_yaligned(m, n, alpha, a, ia, ja, x, y, _state);
        return;
    }
    const ptrdiff_t shift = 4-(unal>>3);
    for(i=0; i<n; i++) {
        const double *p_a = a->ptr.pp_double[ia+i]+ja;
        const double v = alpha*x[i];
        for(j=0; j<shift; j++) {
            y[j] += v*p_a[j];
        }
    }
    rgemvx_transposed_avx2_yaligned(m-shift, n, alpha, a, ia, ja+shift, x, y+shift, _state);
}

/*************************************************************************
Block packing function for fast rGEMM. Loads long  WIDTH*LENGTH  submatrix
with LENGTH<=BLOCK_SIZE and WIDTH<=MICRO_SIZE into contiguous  MICRO_SIZE*
BLOCK_SIZE row-wise 'horizontal' storage (hence H in the function name).

The matrix occupies first ROUND_LENGTH cols of the  storage  (with  LENGTH
being rounded up to nearest SIMD granularity).  ROUND_LENGTH  is  returned
as result. It is guaranteed that ROUND_LENGTH depends only on LENGTH,  and
that it will be same for all function calls.

Unused rows and columns in [LENGTH,ROUND_LENGTH) range are filled by zeros;
unused cols in [ROUND_LENGTH,BLOCK_SIZE) range are ignored.

* op=0 means that source is an WIDTH*LENGTH matrix stored with  src_stride
  stride. The matrix is NOT transposed on load.
* op=1 means that source is an LENGTH*WIDTH matrix  stored with src_stride
  that is loaded with transposition
* present version of the function supports only MICRO_SIZE=2, the behavior
  is undefined for other micro sizes.
* the target is properly aligned; the source can be unaligned.

Requires AVX2, does NOT check its presense.

The function is present in two versions, one  with  variable  opsrc_length
and another one with opsrc_length==block_size==32.

  -- ALGLIB routine --
     19.07.2021
     Bochkanov Sergey
*************************************************************************/
ae_int_t ablasf_packblkh_avx2(
    const double *src,
    ae_int_t src_stride,
    ae_int_t op,
    ae_int_t opsrc_length,
    ae_int_t opsrc_width,
    double   *dst,
    ae_int_t block_size,
    ae_int_t micro_size)
{
    ae_int_t i;
    
    /*
     * Write to the storage
     */
    if( op==0 )
    {
        /*
         * Copy without transposition
         */
        const ae_int_t len8=(opsrc_length>>3)<<3;
        const double *src1 = src+src_stride;
        double       *dst1 = dst+block_size;
        if( opsrc_width==2 )
        {
            /*
             * Width=2
             */
            for(i=0; i<len8; i+=8)
            {
                _mm256_store_pd(dst+i,    _mm256_loadu_pd(src+i));
                _mm256_store_pd(dst+i+4,  _mm256_loadu_pd(src+i+4));
                _mm256_store_pd(dst1+i,   _mm256_loadu_pd(src1+i));
                _mm256_store_pd(dst1+i+4, _mm256_loadu_pd(src1+i+4));
            }
            for(i=len8; i<opsrc_length; i++)
            {
                dst[i]  = src[i];
                dst1[i] = src1[i];
            }
        }
        else
        {
            /*
             * Width=1, pad by zeros
             */
            __m256d vz = _mm256_setzero_pd();
            for(i=0; i<len8; i+=8)
            {
                _mm256_store_pd(dst+i,    _mm256_loadu_pd(src+i));
                _mm256_store_pd(dst+i+4,  _mm256_loadu_pd(src+i+4));
                _mm256_store_pd(dst1+i,   vz);
                _mm256_store_pd(dst1+i+4, vz);
            }
            for(i=len8; i<opsrc_length; i++)
            {
                dst[i]  = src[i];
                dst1[i] = 0.0;
            }
        }
    }
    else
    {
        /*
         * Copy with transposition
         */
        const ae_int_t stride2 = src_stride<<1;
        const ae_int_t stride3 = src_stride+stride2;
        const ae_int_t stride4 = src_stride<<2;
        const ae_int_t len4=(opsrc_length>>2)<<2;
        const double *srci = src;
        double       *dst1 = dst+block_size;
        if( opsrc_width==2 )
        {
            /*
             * Width=2
             */
            for(i=0; i<len4; i+=4)
            {
                __m128d s0 = _mm_loadu_pd(srci), s1 = _mm_loadu_pd(srci+src_stride);
                __m128d s2 = _mm_loadu_pd(srci+stride2), s3 = _mm_loadu_pd(srci+stride3);
                _mm_store_pd(dst+i,    _mm_unpacklo_pd(s0,s1));
                _mm_store_pd(dst1+i,   _mm_unpackhi_pd(s0,s1));
                _mm_store_pd(dst+i+2,  _mm_unpacklo_pd(s2,s3));
                _mm_store_pd(dst1+i+2, _mm_unpackhi_pd(s2,s3));
                srci += stride4;
            }
            for(i=len4; i<opsrc_length; i++)
            {
                dst[i]  = srci[0];
                dst1[i] = srci[1];
                srci += src_stride;
            }
        }
        else
        {
            /*
             * Width=1, pad by zeros
             */
            __m128d vz = _mm_setzero_pd();
            for(i=0; i<len4; i+=4)
            {
                __m128d s0 = _mm_load_sd(srci), s1 = _mm_load_sd(srci+src_stride);
                __m128d s2 = _mm_load_sd(srci+stride2), s3 = _mm_load_sd(srci+stride3);
                _mm_store_pd(dst+i,    _mm_unpacklo_pd(s0,s1));
                _mm_store_pd(dst+i+2,  _mm_unpacklo_pd(s2,s3));
                _mm_store_pd(dst1+i,   vz);
                _mm_store_pd(dst1+i+2, vz);
                srci += stride4;
            }
            for(i=len4; i<opsrc_length; i++)
            {
                dst[i]  = srci[0];
                dst1[i] = 0.0;
                srci += src_stride;
            }
        }
    }
    
    /*
     * Pad by zeros, if needed
     */
    ae_int_t round_length = ((opsrc_length+3)>>2)<<2;
    for(i=opsrc_length; i<round_length; i++)
    {
        dst[i] = 0;
        dst[i+block_size] = 0;
    }
    return round_length;
}

ae_int_t ablasf_packblkh32_avx2(
    const double *src,
    ae_int_t src_stride,
    ae_int_t op,
    ae_int_t ignore_opsrc_length,
    ae_int_t opsrc_width,
    double   *dst,
    ae_int_t ignore_block_size,
    ae_int_t micro_size)
{
    ae_int_t i;
    
    /*
     * Write to the storage
     */
    if( op==0 )
    {
        /*
         * Copy without transposition
         */
        const double *src1 = src+src_stride;
        double       *dst1 = dst+32;
        if( opsrc_width==2 )
        {
            /*
             * Width=2
             */
            for(i=0; i<32; i+=8)
            {
                _mm256_store_pd(dst+i,    _mm256_loadu_pd(src+i));
                _mm256_store_pd(dst+i+4,  _mm256_loadu_pd(src+i+4));
                _mm256_store_pd(dst1+i,   _mm256_loadu_pd(src1+i));
                _mm256_store_pd(dst1+i+4, _mm256_loadu_pd(src1+i+4));
            }
        }
        else
        {
            /*
             * Width=1, pad by zeros
             */
            __m256d vz = _mm256_setzero_pd();
            for(i=0; i<32; i+=8)
            {
                _mm256_store_pd(dst+i,    _mm256_loadu_pd(src+i));
                _mm256_store_pd(dst+i+4,  _mm256_loadu_pd(src+i+4));
                _mm256_store_pd(dst1+i,   vz);
                _mm256_store_pd(dst1+i+4, vz);
            }
        }
    }
    else
    {
        /*
         * Copy with transposition
         */
        const ae_int_t stride2 = src_stride<<1;
        const ae_int_t stride3 = src_stride+stride2;
        const ae_int_t stride4 = src_stride<<2;
        const double *srci = src;
        double       *dst1 = dst+32;
        if( opsrc_width==2 )
        {
            /*
             * Width=2
             */
            for(i=0; i<32; i+=4)
            {
                __m128d s0 = _mm_loadu_pd(srci), s1 = _mm_loadu_pd(srci+src_stride);
                __m128d s2 = _mm_loadu_pd(srci+stride2), s3 = _mm_loadu_pd(srci+stride3);
                _mm_store_pd(dst+i,    _mm_unpacklo_pd(s0,s1));
                _mm_store_pd(dst1+i,   _mm_unpackhi_pd(s0,s1));
                _mm_store_pd(dst+i+2,  _mm_unpacklo_pd(s2,s3));
                _mm_store_pd(dst1+i+2, _mm_unpackhi_pd(s2,s3));
                srci += stride4;
            }
        }
        else
        {
            /*
             * Width=1, pad by zeros
             */
            __m128d vz = _mm_setzero_pd();
            for(i=0; i<32; i+=4)
            {
                __m128d s0 = _mm_load_sd(srci), s1 = _mm_load_sd(srci+src_stride);
                __m128d s2 = _mm_load_sd(srci+stride2), s3 = _mm_load_sd(srci+stride3);
                _mm_store_pd(dst+i,    _mm_unpacklo_pd(s0,s1));
                _mm_store_pd(dst+i+2,  _mm_unpacklo_pd(s2,s3));
                _mm_store_pd(dst1+i,   vz);
                _mm_store_pd(dst1+i+2, vz);
                srci += stride4;
            }
        }
    }
    return 32;
}

/*************************************************************************
Computes  product   A*transpose(B)  of two MICRO_SIZE*ROUND_LENGTH rowwise 
'horizontal' matrices, stored with stride=block_size, and writes it to the
row-wise matrix C.

ROUND_LENGTH is expected to be properly SIMD-rounded length,  as  returned
by ablasf_packblkh_avx2().

Present version of the function supports only MICRO_SIZE=2,  the  behavior
is undefined for other micro sizes.

Requires AVX2, does NOT check its presense.

  -- ALGLIB routine --
     19.07.2021
     Bochkanov Sergey
*************************************************************************/
void ablasf_dotblkh_avx2(
    const double *src_a,
    const double *src_b,
    ae_int_t round_length,
    ae_int_t block_size,
    ae_int_t micro_size,
    double *dst,
    ae_int_t dst_stride)
{
    ae_int_t z;
    __m256d r00 = _mm256_setzero_pd(), r01 = _mm256_setzero_pd(), r10 = _mm256_setzero_pd(), r11 = _mm256_setzero_pd();
    if( round_length&0x7 )
    {
        /*
         * round_length is multiple of 4, but not multiple of 8
         */
        for(z=0; z<round_length; z+=4, src_a+=4, src_b+=4)
        {
            __m256d a0 = _mm256_load_pd(src_a);
            __m256d a1 = _mm256_load_pd(src_a+block_size);
            __m256d b0 = _mm256_load_pd(src_b);
            __m256d b1 = _mm256_load_pd(src_b+block_size);
            r00 = _mm256_add_pd(_mm256_mul_pd(a0, b0), r00);
            r01 = _mm256_add_pd(_mm256_mul_pd(a0, b1), r01);
            r10 = _mm256_add_pd(_mm256_mul_pd(a1, b0), r10);
            r11 = _mm256_add_pd(_mm256_mul_pd(a1, b1), r11);
        }
    }
    else
    {
        /*
         * round_length is multiple of 8
         */
        for(z=0; z<round_length; z+=8, src_a+=8, src_b+=8)
        {
            __m256d a0 = _mm256_load_pd(src_a);
            __m256d a1 = _mm256_load_pd(src_a+block_size);
            __m256d b0 = _mm256_load_pd(src_b);
            __m256d b1 = _mm256_load_pd(src_b+block_size);
            __m256d c0 = _mm256_load_pd(src_a+4);
            __m256d c1 = _mm256_load_pd(src_a+block_size+4);
            __m256d d0 = _mm256_load_pd(src_b+4);
            __m256d d1 = _mm256_load_pd(src_b+block_size+4);
            r00 = _mm256_add_pd(_mm256_add_pd(r00, _mm256_mul_pd(a0, b0)), _mm256_mul_pd(c0, d0));
            r01 = _mm256_add_pd(_mm256_add_pd(r01, _mm256_mul_pd(a0, b1)), _mm256_mul_pd(c0, d1));
            r10 = _mm256_add_pd(_mm256_add_pd(r10, _mm256_mul_pd(a1, b0)), _mm256_mul_pd(c1, d0));
            r11 = _mm256_add_pd(_mm256_add_pd(r11, _mm256_mul_pd(a1, b1)), _mm256_mul_pd(c1, d1));
        }
    }
    __m256d sum0 = _mm256_hadd_pd(r00,r01);
    __m256d sum1 = _mm256_hadd_pd(r10,r11);
    _mm_store_pd(dst,            _mm_add_pd(_mm256_castpd256_pd128(sum0), _mm256_extractf128_pd(sum0,1)));
    _mm_store_pd(dst+dst_stride, _mm_add_pd(_mm256_castpd256_pd128(sum1), _mm256_extractf128_pd(sum1,1)));
}

/*************************************************************************
Y := alpha*X + beta*Y

Requires AVX2, does NOT check its presense.

  -- ALGLIB routine --
     19.07.2021
     Bochkanov Sergey
*************************************************************************/
void ablasf_daxpby_avx2(
    ae_int_t     n,
    double       alpha,
    const double *src,
    double       beta,
    double       *dst)
{
    if( beta==1.0 )
    {
        /*
         * The most optimized case: DST := alpha*SRC + DST
         *
         * First, we process leading elements with generic C code until DST is aligned.
         * Then, we process central part, assuming that DST is properly aligned.
         * Finally, we process tail.
         */
        ae_int_t i, n4;
        __m256d avx_alpha = _mm256_set1_pd(alpha);
        while( n>0 && (((ptrdiff_t)dst)&31) )
        {
            *dst += alpha*(*src);
            n--;
            dst++;
            src++;
        }
        n4=(n>>2)<<2;
        for(i=0; i<n4; i+=4)
        {
            __m256d r = _mm256_add_pd(_mm256_mul_pd(avx_alpha, _mm256_loadu_pd(src+i)), _mm256_load_pd(dst+i));
            _mm256_store_pd(dst+i, r);
        }
        for(i=n4; i<n; i++)
            dst[i] = alpha*src[i]+dst[i];
    }
    else if( beta!=0.0 )
    {
        /*
         * Well optimized: DST := alpha*SRC + beta*DST
         */
        ae_int_t i, n4;
        __m256d avx_alpha = _mm256_set1_pd(alpha);
        __m256d avx_beta  = _mm256_set1_pd(beta);
        while( n>0 && (((ptrdiff_t)dst)&31) )
        {
            *dst = alpha*(*src) + beta*(*dst);
            n--;
            dst++;
            src++;
        }
        n4=(n>>2)<<2;
        for(i=0; i<n4; i+=4)
        {
            __m256d r = _mm256_add_pd(
                _mm256_mul_pd(avx_alpha, _mm256_loadu_pd(src+i)),
                _mm256_mul_pd(avx_beta,_mm256_load_pd(dst+i)));
            _mm256_store_pd(dst+i, r);
        }
        for(i=n4; i<n; i++)
            dst[i] = alpha*src[i]+beta*dst[i];
    }
    else
    {
        /*
         * Easy case: DST := alpha*SRC
         */
        ae_int_t i;
        for(i=0; i<n; i++)
            dst[i] = alpha*src[i];
    }
}

ae_bool spchol_updatekernelabc4_avx2(double* rowstorage,
     ae_int_t offss,
     ae_int_t twidth,
     ae_int_t offsu,
     ae_int_t uheight,
     ae_int_t urank,
     ae_int_t urowstride,
     ae_int_t uwidth,
     const double* diagd,
     ae_int_t offsd,
     const ae_int_t* raw2smap,
     const ae_int_t* superrowidx,
     ae_int_t urbase,
     ae_state *_state)
{
    ae_int_t k;
    ae_int_t targetrow;
    ae_int_t targetcol;
    
    /*
     * Filter out unsupported combinations (ones that are too sparse for the non-SIMD code)
     */
    if( twidth<3||twidth>4 )
    {
        return ae_false;
    }
    if( uwidth<1||uwidth>4 )
    {
        return ae_false;
    }
    if( urank>4 )
    {
        return ae_false;
    }
    
    /*
     * Shift input arrays to the beginning of the working area.
     * Prepare SIMD masks
     */
    __m256i v_rankmask = _mm256_cmpgt_epi64(_mm256_set_epi64x(urank, urank, urank, urank), _mm256_set_epi64x(3, 2, 1, 0));
    double *update_storage = rowstorage+offsu;
    double *target_storage = rowstorage+offss;
    superrowidx += urbase;
    
    /*
     * Load head of the update matrix
     */
    __m256d v_d0123 = _mm256_maskload_pd(diagd+offsd, v_rankmask);
    __m256d u_0_0123 = _mm256_setzero_pd();
    __m256d u_1_0123 = _mm256_setzero_pd();
    __m256d u_2_0123 = _mm256_setzero_pd();
    __m256d u_3_0123 = _mm256_setzero_pd();
    for(k=0; k<=uwidth-1; k++)
    {
        targetcol = raw2smap[superrowidx[k]];
        if( targetcol==0 )
            u_0_0123 = _mm256_mul_pd(v_d0123, _mm256_maskload_pd(update_storage+k*urowstride, v_rankmask));
        if( targetcol==1 )
            u_1_0123 = _mm256_mul_pd(v_d0123, _mm256_maskload_pd(update_storage+k*urowstride, v_rankmask));
        if( targetcol==2 )
            u_2_0123 = _mm256_mul_pd(v_d0123, _mm256_maskload_pd(update_storage+k*urowstride, v_rankmask));
        if( targetcol==3 )
            u_3_0123 = _mm256_mul_pd(v_d0123, _mm256_maskload_pd(update_storage+k*urowstride, v_rankmask));
    }
    
    /*
     * Transpose head
     */
    __m256d u01_lo = _mm256_unpacklo_pd(u_0_0123,u_1_0123);
    __m256d u01_hi = _mm256_unpackhi_pd(u_0_0123,u_1_0123);
    __m256d u23_lo = _mm256_unpacklo_pd(u_2_0123,u_3_0123);
    __m256d u23_hi = _mm256_unpackhi_pd(u_2_0123,u_3_0123);
    __m256d u_0123_0 = _mm256_permute2f128_pd(u01_lo, u23_lo, 0x20);
    __m256d u_0123_1 = _mm256_permute2f128_pd(u01_hi, u23_hi, 0x20);
    __m256d u_0123_2 = _mm256_permute2f128_pd(u23_lo, u01_lo, 0x13);
    __m256d u_0123_3 = _mm256_permute2f128_pd(u23_hi, u01_hi, 0x13);
    
    /*
     * Run update
     */
    if( urank==1 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = raw2smap[superrowidx[k]]*4;
            double *update_row = rowstorage+offsu+k*urowstride;
            _mm256_store_pd(target_storage+targetrow,
                _mm256_sub_pd(_mm256_load_pd(target_storage+targetrow),
                    _mm256_mul_pd(_mm256_broadcast_sd(update_row+0), u_0123_0)));
        }
    }
    if( urank==2 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = raw2smap[superrowidx[k]]*4;
            double *update_row = rowstorage+offsu+k*urowstride;
            _mm256_store_pd(target_storage+targetrow,
                _mm256_sub_pd(_mm256_sub_pd(_mm256_load_pd(target_storage+targetrow),
                    _mm256_mul_pd(_mm256_broadcast_sd(update_row+1), u_0123_1)),
                    _mm256_mul_pd(_mm256_broadcast_sd(update_row+0), u_0123_0)));
        }
    }
    if( urank==3 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = raw2smap[superrowidx[k]]*4;
            double *update_row = rowstorage+offsu+k*urowstride;
            _mm256_store_pd(target_storage+targetrow,
                _mm256_sub_pd(_mm256_sub_pd(_mm256_sub_pd(_mm256_load_pd(target_storage+targetrow),
                    _mm256_mul_pd(_mm256_broadcast_sd(update_row+2), u_0123_2)),
                    _mm256_mul_pd(_mm256_broadcast_sd(update_row+1), u_0123_1)),
                    _mm256_mul_pd(_mm256_broadcast_sd(update_row+0), u_0123_0)));
        }
    }
    if( urank==4 )
    {
        for(k=0; k<=uheight-1; k++)
        {
            targetrow = raw2smap[superrowidx[k]]*4;
            double *update_row = rowstorage+offsu+k*urowstride;
            _mm256_store_pd(target_storage+targetrow,
                _mm256_sub_pd(_mm256_sub_pd(_mm256_sub_pd(_mm256_sub_pd(_mm256_load_pd(target_storage+targetrow),
                    _mm256_mul_pd(_mm256_broadcast_sd(update_row+3), u_0123_3)),
                    _mm256_mul_pd(_mm256_broadcast_sd(update_row+2), u_0123_2)),
                    _mm256_mul_pd(_mm256_broadcast_sd(update_row+1), u_0123_1)),
                    _mm256_mul_pd(_mm256_broadcast_sd(update_row+0), u_0123_0)));
        }
    }
    return ae_true;
}

ae_bool spchol_updatekernel4444_avx2(
     double*  rowstorage,
     ae_int_t offss,
     ae_int_t sheight,
     ae_int_t offsu,
     ae_int_t uheight,
     const double*  diagd,
     ae_int_t offsd,
     const ae_int_t* raw2smap,
     const ae_int_t* superrowidx,
     ae_int_t urbase,
     ae_state *_state)
{
    ae_int_t k;
    ae_int_t targetrow;
    ae_int_t offsk;
    __m256d v_negd_u0, v_negd_u1, v_negd_u2, v_negd_u3, v_negd;
    __m256d v_w0, v_w1, v_w2, v_w3, u01_lo, u01_hi, u23_lo, u23_hi;
    
    /*
     * Compute W = -D*transpose(U[0:3])
     */
    v_negd = _mm256_mul_pd(_mm256_loadu_pd(diagd+offsd),_mm256_set1_pd(-1.0));
    v_negd_u0   = _mm256_mul_pd(_mm256_load_pd(rowstorage+offsu+0*4),v_negd);
    v_negd_u1   = _mm256_mul_pd(_mm256_load_pd(rowstorage+offsu+1*4),v_negd);
    v_negd_u2   = _mm256_mul_pd(_mm256_load_pd(rowstorage+offsu+2*4),v_negd);
    v_negd_u3   = _mm256_mul_pd(_mm256_load_pd(rowstorage+offsu+3*4),v_negd);
    u01_lo = _mm256_unpacklo_pd(v_negd_u0,v_negd_u1);
    u01_hi = _mm256_unpackhi_pd(v_negd_u0,v_negd_u1);
    u23_lo = _mm256_unpacklo_pd(v_negd_u2,v_negd_u3);
    u23_hi = _mm256_unpackhi_pd(v_negd_u2,v_negd_u3);
    v_w0 = _mm256_permute2f128_pd(u01_lo, u23_lo, 0x20);
    v_w1 = _mm256_permute2f128_pd(u01_hi, u23_hi, 0x20);
    v_w2 = _mm256_permute2f128_pd(u23_lo, u01_lo, 0x13);
    v_w3 = _mm256_permute2f128_pd(u23_hi, u01_hi, 0x13);
    
    //
    // Compute update S:= S + row_scatter(U*W)
    //
    if( sheight==uheight )
    {
        /*
         * No row scatter, the most efficient code
         */
        for(k=0; k<=uheight-1; k++)
        {
            __m256d target;
            
            targetrow = offss+k*4;
            offsk = offsu+k*4;
            
            target = _mm256_load_pd(rowstorage+targetrow);
            target = _mm256_add_pd(_mm256_mul_pd(_mm256_broadcast_sd(rowstorage+offsk+0),v_w0),target);
            target = _mm256_add_pd(_mm256_mul_pd(_mm256_broadcast_sd(rowstorage+offsk+1),v_w1),target);
            target = _mm256_add_pd(_mm256_mul_pd(_mm256_broadcast_sd(rowstorage+offsk+2),v_w2),target);
            target = _mm256_add_pd(_mm256_mul_pd(_mm256_broadcast_sd(rowstorage+offsk+3),v_w3),target);
            _mm256_store_pd(rowstorage+targetrow, target);
        }
    }
    else
    {
        /*
         * Row scatter is performed, less efficient code using double mapping to determine target row index
         */
        for(k=0; k<=uheight-1; k++)
        {
            __m256d v_uk0, v_uk1, v_uk2, v_uk3, target;
            
            targetrow = offss+raw2smap[superrowidx[urbase+k]]*4;
            offsk = offsu+k*4;
            
            target = _mm256_load_pd(rowstorage+targetrow);
            v_uk0 = _mm256_broadcast_sd(rowstorage+offsk+0);
            v_uk1 = _mm256_broadcast_sd(rowstorage+offsk+1);
            v_uk2 = _mm256_broadcast_sd(rowstorage+offsk+2);
            v_uk3 = _mm256_broadcast_sd(rowstorage+offsk+3);
            target = _mm256_add_pd(_mm256_mul_pd(v_uk0,v_w0),target);
            target = _mm256_add_pd(_mm256_mul_pd(v_uk1,v_w1),target);
            target = _mm256_add_pd(_mm256_mul_pd(v_uk2,v_w2),target);
            target = _mm256_add_pd(_mm256_mul_pd(v_uk3,v_w3),target);
            _mm256_store_pd(rowstorage+targetrow, target);
        }
    }
    return ae_true;
}


/*************************************************************************
Fast kernel for biharmonic panel with NY=1

INPUT PARAMETERS:
    D0, D1, D2      -   evaluation point minus (Panel.C0,Panel.C1,Panel.C2)

OUTPUT PARAMETERS:
    F               -   model value
    InvPowRPPlus1   -   1/(R^(P+1))

  -- ALGLIB --
     Copyright 26.08.2022 by Sergey Bochkanov
*************************************************************************/
ae_bool rbfv3farfields_bhpaneleval1fastkernel16_avx2(double d0,
     double d1,
     double d2,
     const double* pnma,
     const double* pnmb,
     const double* pmmcdiag,
     const double* ynma,
     const double* tblrmodmn,
     double* f,
     double* invpowrpplus1,
     ae_state *_state)
{
    ae_int_t n;
    double r, r2, r01, invr;
    double sintheta, costheta;
    ae_complex expiphi, expiphi2, expiphi3, expiphi4;
    ae_int_t jj;
    ae_bool result;

    *f = 0.0;
    *invpowrpplus1 = 0.0;
    result = ae_true;
    
    /*
     *Convert to spherical polar coordinates.
     *
     * NOTE: we make sure that R is non-zero by adding extremely small perturbation
     */
    r2 = d0*d0+d1*d1+d2*d2+ae_minrealnumber;
    r = ae_sqrt(r2, _state);
    r01 = ae_sqrt(d0*d0+d1*d1+ae_minrealnumber, _state);
    costheta = d2/r;
    sintheta = r01/r;
    expiphi.x = d0/r01;
    expiphi.y = d1/r01;
    invr = (double)1/r;
    
    /*
     * prepare precomputed quantities
     */
    double powsintheta2 = sintheta*sintheta;
    double powsintheta3 = powsintheta2*sintheta;
    double powsintheta4 = powsintheta2*powsintheta2;
    expiphi2.x = expiphi.x*expiphi.x-expiphi.y*expiphi.y;
    expiphi2.y = 2*expiphi.x*expiphi.y;
    expiphi3.x = expiphi2.x*expiphi.x-expiphi2.y*expiphi.y;
    expiphi3.y = expiphi2.x*expiphi.y+expiphi.x*expiphi2.y;
    expiphi4.x = expiphi2.x*expiphi2.x-expiphi2.y*expiphi2.y;
    expiphi4.y = 2*expiphi2.x*expiphi2.y;
    
    /*
     * Compute far field expansion for a cluster of basis functions f=r
     *
     * NOTE: the original paper by Beatson et al. uses f=r as the basis function,
     *       whilst ALGLIB uses f=-r due to conditional positive definiteness requirement.
     *       We will perform conversion later.
     */
    __m256d v_costheta      = _mm256_set1_pd(costheta);
    __m256d v_r2            = _mm256_set1_pd(r2);
    __m256d v_f             = _mm256_setzero_pd();
    __m256d v_invr          = _mm256_set1_pd(invr);
    __m256d v_powsinthetaj  = _mm256_set_pd(powsintheta3, powsintheta2, sintheta, 1.0);
    __m256d v_powsintheta4  = _mm256_set1_pd(powsintheta4);
    __m256d v_expijphix     = _mm256_set_pd(expiphi3.x, expiphi2.x, expiphi.x, 1.0);
    __m256d v_expijphiy     = _mm256_set_pd(expiphi3.y, expiphi2.y, expiphi.y, 0.0);
    __m256d v_expi4phix     = _mm256_set1_pd(expiphi4.x);
    __m256d v_expi4phiy     = _mm256_set1_pd(expiphi4.y);
    *f = (double)(0);
    for(jj=0; jj<4; jj++)
    {
        __m256d pnm_cur         = _mm256_setzero_pd(), pnm_prev = _mm256_setzero_pd(), pnm_new;
        __m256d v_powrminusj1   = _mm256_set1_pd(invr);
        for(n=0; n<jj*4; n++)
            v_powrminusj1 = _mm256_mul_pd(v_powrminusj1, v_invr);
        for(n=jj*4; n<16; n++)
        {
            ae_int_t j0=jj*4;
            
            
            pnm_new = _mm256_mul_pd(v_powsinthetaj, _mm256_load_pd(pmmcdiag+n*16+j0));
            pnm_new = _mm256_add_pd(pnm_new, _mm256_mul_pd(v_costheta,_mm256_mul_pd(pnm_cur,_mm256_load_pd(pnma+n*16+j0))));
            pnm_new = _mm256_add_pd(pnm_new, _mm256_mul_pd(pnm_prev,_mm256_load_pd(pnmb+n*16+j0)));
            pnm_prev = pnm_cur;
            pnm_cur  = pnm_new;
            
            __m256d v_tmp = _mm256_mul_pd(pnm_cur, _mm256_load_pd(ynma+n*16+j0));
            __m256d v_sphericalx = _mm256_mul_pd(v_tmp, v_expijphix);
            __m256d v_sphericaly = _mm256_mul_pd(v_tmp, v_expijphiy);
            
            __m256d v_summnx = _mm256_add_pd(_mm256_mul_pd(v_r2,_mm256_load_pd(tblrmodmn+n*64+j0+32)),_mm256_load_pd(tblrmodmn+n*64+j0));
            __m256d v_summny = _mm256_add_pd(_mm256_mul_pd(v_r2,_mm256_load_pd(tblrmodmn+n*64+j0+48)),_mm256_load_pd(tblrmodmn+n*64+j0+16));
            
            __m256d v_z = _mm256_sub_pd(_mm256_mul_pd(v_sphericalx,v_summnx),_mm256_mul_pd(v_sphericaly,v_summny));
            
            v_f = _mm256_add_pd(v_f, _mm256_mul_pd(v_powrminusj1, v_z));
            v_powrminusj1 = _mm256_mul_pd(v_powrminusj1, v_invr);
        }
        __m256d v_expijphix_new = _mm256_sub_pd(_mm256_mul_pd(v_expijphix,v_expi4phix),_mm256_mul_pd(v_expijphiy,v_expi4phiy));
        __m256d v_expijphiy_new = _mm256_add_pd(_mm256_mul_pd(v_expijphix,v_expi4phiy),_mm256_mul_pd(v_expijphiy,v_expi4phix));
        v_powsinthetaj = _mm256_mul_pd(v_powsinthetaj, v_powsintheta4);
        v_expijphix = v_expijphix_new;
        v_expijphiy = v_expijphiy_new;
    }
            
    double ttt[4];
    _mm256_storeu_pd(ttt, v_f);
    for(int k=0; k<4; k++)
        *f += ttt[k];
    
    double r4 = r2*r2;
    double r8 = r4*r4;
    double r16 = r8*r8;
    *invpowrpplus1 = 1/r16;
    
    return result;
}


/*************************************************************************
Fast kernel for biharmonic panel with general NY

INPUT PARAMETERS:
    D0, D1, D2      -   evaluation point minus (Panel.C0,Panel.C1,Panel.C2)

OUTPUT PARAMETERS:
    F               -   array[NY], model value
    InvPowRPPlus1   -   1/(R^(P+1))

  -- ALGLIB --
     Copyright 26.08.2022 by Sergey Bochkanov
*************************************************************************/
ae_bool rbfv3farfields_bhpanelevalfastkernel16_avx2(double d0,
     double d1,
     double d2,
     ae_int_t ny,
     const double* pnma,
     const double* pnmb,
     const double* pmmcdiag,
     const double* ynma,
     const double* tblrmodmn,
     double* f,
     double* invpowrpplus1,
     ae_state *_state)
{
    ae_int_t n;
    double r, r2, r01, invr;
    double sintheta, costheta;
    ae_complex expiphi, expiphi2, expiphi3, expiphi4;
    ae_int_t jj;
    
    /*
     * Precomputed buffer which is enough for NY up to 16
     */
    __m256d v_f[16];
    if( ny>16 )
        return ae_false;
    for(int k=0; k<ny; k++)
    {
        v_f[k] = _mm256_setzero_pd();
        f[k] = 0.0;
    }
    
    /*
     *Convert to spherical polar coordinates.
     *
     * NOTE: we make sure that R is non-zero by adding extremely small perturbation
     */
    r2 = d0*d0+d1*d1+d2*d2+ae_minrealnumber;
    r = ae_sqrt(r2, _state);
    r01 = ae_sqrt(d0*d0+d1*d1+ae_minrealnumber, _state);
    costheta = d2/r;
    sintheta = r01/r;
    expiphi.x = d0/r01;
    expiphi.y = d1/r01;
    invr = (double)1/r;
    
    /*
     * prepare precomputed quantities
     */
    double powsintheta2 = sintheta*sintheta;
    double powsintheta3 = powsintheta2*sintheta;
    double powsintheta4 = powsintheta2*powsintheta2;
    expiphi2.x = expiphi.x*expiphi.x-expiphi.y*expiphi.y;
    expiphi2.y = 2*expiphi.x*expiphi.y;
    expiphi3.x = expiphi2.x*expiphi.x-expiphi2.y*expiphi.y;
    expiphi3.y = expiphi2.x*expiphi.y+expiphi.x*expiphi2.y;
    expiphi4.x = expiphi2.x*expiphi2.x-expiphi2.y*expiphi2.y;
    expiphi4.y = 2*expiphi2.x*expiphi2.y;
    
    /*
     * Compute far field expansion for a cluster of basis functions f=r
     *
     * NOTE: the original paper by Beatson et al. uses f=r as the basis function,
     *       whilst ALGLIB uses f=-r due to conditional positive definiteness requirement.
     *       We will perform conversion later.
     */
    __m256d v_costheta      = _mm256_set1_pd(costheta);
    __m256d v_r2            = _mm256_set1_pd(r2);
    __m256d v_invr          = _mm256_set1_pd(invr);
    __m256d v_powsinthetaj  = _mm256_set_pd(powsintheta3, powsintheta2, sintheta, 1.0);
    __m256d v_powsintheta4  = _mm256_set1_pd(powsintheta4);
    __m256d v_expijphix     = _mm256_set_pd(expiphi3.x, expiphi2.x, expiphi.x, 1.0);
    __m256d v_expijphiy     = _mm256_set_pd(expiphi3.y, expiphi2.y, expiphi.y, 0.0);
    __m256d v_expi4phix     = _mm256_set1_pd(expiphi4.x);
    __m256d v_expi4phiy     = _mm256_set1_pd(expiphi4.y);
    *f = (double)(0);
    for(jj=0; jj<4; jj++)
    {
        __m256d pnm_cur         = _mm256_setzero_pd(), pnm_prev = _mm256_setzero_pd(), pnm_new;
        __m256d v_powrminusj1   = _mm256_set1_pd(invr);
        for(n=0; n<jj*4; n++)
            v_powrminusj1 = _mm256_mul_pd(v_powrminusj1, v_invr);
        for(n=jj*4; n<16; n++)
        {
            ae_int_t j0=jj*4;
            
            pnm_new = _mm256_mul_pd(v_powsinthetaj, _mm256_load_pd(pmmcdiag+n*16+j0));
            pnm_new = _mm256_add_pd(pnm_new, _mm256_mul_pd(v_costheta,_mm256_mul_pd(pnm_cur,_mm256_load_pd(pnma+n*16+j0))));
            pnm_new = _mm256_add_pd(pnm_new, _mm256_mul_pd(pnm_prev,_mm256_load_pd(pnmb+n*16+j0)));
            pnm_prev = pnm_cur;
            pnm_cur  = pnm_new;
            
            __m256d v_tmp = _mm256_mul_pd(pnm_cur, _mm256_load_pd(ynma+n*16+j0));
            __m256d v_sphericalx = _mm256_mul_pd(v_tmp, v_expijphix);
            __m256d v_sphericaly = _mm256_mul_pd(v_tmp, v_expijphiy);
            
            const double *p_rmodmn = tblrmodmn+n*64+j0;
            for(int k=0; k<ny; k++)
            {
                __m256d v_summnx = _mm256_add_pd(_mm256_mul_pd(v_r2,_mm256_load_pd(p_rmodmn+32)),_mm256_load_pd(p_rmodmn));
                __m256d v_summny = _mm256_add_pd(_mm256_mul_pd(v_r2,_mm256_load_pd(p_rmodmn+48)),_mm256_load_pd(p_rmodmn+16));
                __m256d v_z = _mm256_sub_pd(_mm256_mul_pd(v_sphericalx,v_summnx),_mm256_mul_pd(v_sphericaly,v_summny));
                v_f[k] = _mm256_add_pd(v_f[k], _mm256_mul_pd(v_powrminusj1, v_z));
                p_rmodmn += 1024;
            }
            v_powrminusj1 = _mm256_mul_pd(v_powrminusj1, v_invr);
        }
        __m256d v_expijphix_new = _mm256_sub_pd(_mm256_mul_pd(v_expijphix,v_expi4phix),_mm256_mul_pd(v_expijphiy,v_expi4phiy));
        __m256d v_expijphiy_new = _mm256_add_pd(_mm256_mul_pd(v_expijphix,v_expi4phiy),_mm256_mul_pd(v_expijphiy,v_expi4phix));
        v_powsinthetaj = _mm256_mul_pd(v_powsinthetaj, v_powsintheta4);
        v_expijphix = v_expijphix_new;
        v_expijphiy = v_expijphiy_new;
    }
            
for(int t=0; t<ny; t++)
{
    double ttt[4];
    _mm256_storeu_pd(ttt, v_f[t]);
    for(int k=0; k<4; k++)
        f[t] += ttt[k];
}
    
    double r4 = r2*r2;
    double r8 = r4*r4;
    double r16 = r8*r8;
    *invpowrpplus1 = 1/r16;
    
    return ae_true;
}

/* ALGLIB_NO_FAST_KERNELS, _ALGLIB_HAS_AVX2_INTRINSICS */
#endif


}

