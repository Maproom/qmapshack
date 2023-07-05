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
#ifndef _alglibinternal_pkg_h
#define _alglibinternal_pkg_h
#include "ap.h"


/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS COMPUTATIONAL CORE DECLARATIONS (DATATYPES)
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
#if defined(AE_COMPILE_APSERV) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_vector ba0;
    ae_vector ia0;
    ae_vector ia1;
    ae_vector ia2;
    ae_vector ia3;
    ae_vector ra0;
    ae_vector ra1;
    ae_vector ra2;
    ae_vector ra3;
    ae_matrix rm0;
    ae_matrix rm1;
} apbuffers;
typedef struct
{
    ae_bool val;
} sboolean;
typedef struct
{
    ae_vector val;
} sbooleanarray;
typedef struct
{
    ae_int_t val;
} sinteger;
typedef struct
{
    ae_vector val;
} sintegerarray;
typedef struct
{
    double val;
} sreal;
typedef struct
{
    ae_vector val;
} srealarray;
typedef struct
{
    ae_complex val;
} scomplex;
typedef struct
{
    ae_vector val;
} scomplexarray;
typedef struct
{
    ae_int_t n;
    ae_int_t temporariescount;
    ae_shared_pool sourcepool;
    ae_shared_pool temporarypool;
    sbooleanarray seed0;
    sbooleanarray seedn;
} nbpool;
typedef struct
{
    ae_int_t n;
    ae_int_t temporariescount;
    ae_shared_pool sourcepool;
    ae_shared_pool temporarypool;
    sintegerarray seed0;
    sintegerarray seedn;
} nipool;
typedef struct
{
    ae_int_t n;
    ae_int_t temporariescount;
    ae_shared_pool sourcepool;
    ae_shared_pool temporarypool;
    srealarray seed0;
    srealarray seedn;
} nrpool;
typedef struct
{
    double rsum;
    double rcnt;
    double prior;
} savgcounter;
typedef struct
{
    ae_int_t cnt;
    ae_vector elems;
    double prior;
} squantilecounter;
#endif
#if defined(AE_COMPILE_ABLASF) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_HBLAS) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_CREFLECTIONS) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_SBLAS) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_ABLASMKL) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_SCODES) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_TSORT) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_BLAS) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_ROTATIONS) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_BASICSTATOPS) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_APSTRUCT) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t nstored;
    ae_vector items;
    ae_vector locationof;
    ae_int_t iteridx;
} niset;
#endif
#if defined(AE_COMPILE_TRLINSOLVE) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_SAFESOLVE) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_XBLAS) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_LINMIN) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_bool brackt;
    ae_bool stage1;
    ae_int_t infoc;
    double dg;
    double dgm;
    double dginit;
    double dgtest;
    double dgx;
    double dgxm;
    double dgy;
    double dgym;
    double finit;
    double ftest1;
    double fm;
    double fx;
    double fxm;
    double fy;
    double fym;
    double stx;
    double sty;
    double stmin;
    double stmax;
    double width;
    double width1;
    double xtrapf;
} linminstate;
typedef struct
{
    ae_bool needf;
    ae_vector x;
    double f;
    ae_int_t n;
    ae_vector xbase;
    ae_vector s;
    double stplen;
    double fcur;
    double stpmax;
    ae_int_t fmax;
    ae_int_t nfev;
    ae_int_t info;
    rcommstate rstate;
} armijostate;
#endif
#if defined(AE_COMPILE_NEARUNITYUNIT) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_NTHEORY) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_FTBASE) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_matrix entries;
    ae_vector buffer;
    ae_vector precr;
    ae_vector preci;
    ae_shared_pool bluesteinpool;
} fasttransformplan;
#endif
#if defined(AE_COMPILE_HPCCORES) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t chunksize;
    ae_int_t ntotal;
    ae_int_t nin;
    ae_int_t nout;
    ae_int_t wcount;
    ae_vector batch4buf;
    ae_vector hpcbuf;
    ae_matrix xy;
    ae_matrix xy2;
    ae_vector xyrow;
    ae_vector x;
    ae_vector y;
    ae_vector desiredy;
    double e;
    ae_vector g;
    ae_vector tmp0;
} mlpbuffers;
#endif
#if defined(AE_COMPILE_ALGLIBBASICS) || !defined(AE_PARTIAL_BUILD)
#endif

}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS C++ INTERFACE
//
/////////////////////////////////////////////////////////////////////////
namespace alglib
{


}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS COMPUTATIONAL CORE DECLARATIONS (FUNCTIONS)
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
#if defined(AE_COMPILE_APSERV) || !defined(AE_PARTIAL_BUILD)
void seterrorflagdiff(ae_bool* flag,
     double val,
     double refval,
     double tol,
     double s,
     ae_state *_state);
ae_bool alwaysfalse(ae_state *_state);
void touchboolean(ae_bool* a, ae_state *_state);
void touchint(ae_int_t* a, ae_state *_state);
void touchreal(double* a, ae_state *_state);
double coalesce(double a, double b, ae_state *_state);
ae_int_t coalescei(ae_int_t a, ae_int_t b, ae_state *_state);
double inttoreal(ae_int_t a, ae_state *_state);
double logbase2(double x, ae_state *_state);
ae_bool approxequal(double a, double b, double tol, ae_state *_state);
ae_bool approxequalrel(double a, double b, double tol, ae_state *_state);
void taskgenint1d(double a,
     double b,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void taskgenint1dequidist(double a,
     double b,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void taskgenint1dcheb1(double a,
     double b,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void taskgenint1dcheb2(double a,
     double b,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
ae_bool aredistinct(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state);
ae_bool aresameboolean(ae_bool v1, ae_bool v2, ae_state *_state);
void setlengthzero(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);
void bvectorsetlengthatleast(/* Boolean */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);
void ivectorsetlengthatleast(/* Integer */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);
void rvectorsetlengthatleast(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);
void rmatrixsetlengthatleast(/* Real    */ ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);
void bmatrixsetlengthatleast(/* Boolean */ ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);
void bvectorgrowto(/* Boolean */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);
void ivectorgrowto(/* Integer */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);
void rmatrixgrowrowsto(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_int_t mincols,
     ae_state *_state);
void rmatrixgrowcolsto(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_int_t minrows,
     ae_state *_state);
void rvectorgrowto(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);
void ivectorresize(/* Integer */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);
void rvectorresize(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);
void rmatrixresize(/* Real    */ ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);
void imatrixresize(/* Integer */ ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);
void ivectorappend(/* Integer */ ae_vector* x,
     ae_int_t v,
     ae_state *_state);
ae_bool isfinitevector(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state);
ae_bool isfiniteornanvector(/* Real    */ const ae_vector* x,
     ae_int_t n,
     ae_state *_state);
ae_bool isfinitecvector(/* Complex */ const ae_vector* z,
     ae_int_t n,
     ae_state *_state);
ae_bool apservisfinitematrix(/* Real    */ const ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);
ae_bool apservisfinitecmatrix(/* Complex */ const ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);
ae_bool isfinitecmatrix(/* Complex */ const ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);
ae_bool isfinitertrmatrix(/* Real    */ const ae_matrix* x,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state);
ae_bool apservisfinitectrmatrix(/* Complex */ const ae_matrix* x,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state);
ae_bool isfinitectrmatrix(/* Complex */ const ae_matrix* x,
     ae_int_t n,
     ae_bool isupper,
     ae_state *_state);
ae_bool apservisfiniteornanmatrix(/* Real    */ const ae_matrix* x,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);
double safepythag2(double x, double y, ae_state *_state);
double safepythag3(double x, double y, double z, ae_state *_state);
ae_int_t saferdiv(double x, double y, double* r, ae_state *_state);
double safeminposrv(double x, double y, double v, ae_state *_state);
void apperiodicmap(double* x,
     double a,
     double b,
     double* k,
     ae_state *_state);
double randomnormal(ae_state *_state);
void randomunit(ae_int_t n, /* Real    */ ae_vector* x, ae_state *_state);
void swapi(ae_int_t* v0, ae_int_t* v1, ae_state *_state);
void swapr(double* v0, double* v1, ae_state *_state);
void swaprows(/* Real    */ ae_matrix* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t ncols,
     ae_state *_state);
void swapcols(/* Real    */ ae_matrix* a,
     ae_int_t j0,
     ae_int_t j1,
     ae_int_t nrows,
     ae_state *_state);
void swapentries(/* Real    */ ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t entrywidth,
     ae_state *_state);
void swapentriesb(/* Boolean */ ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t entrywidth,
     ae_state *_state);
void swapelements(/* Real    */ ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state);
void swapelementsi(/* Integer */ ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state);
double maxreal3(double v0, double v1, double v2, ae_state *_state);
void inc(ae_int_t* v, ae_state *_state);
void dec(ae_int_t* v, ae_state *_state);
void threadunsafeinc(ae_int_t* v, ae_state *_state);
void threadunsafeincby(ae_int_t* v, ae_int_t k, ae_state *_state);
void countdown(ae_int_t* v, ae_state *_state);
double possign(double x, ae_state *_state);
double rmul2(double v0, double v1, ae_state *_state);
double rmul3(double v0, double v1, double v2, ae_state *_state);
double rmul4(double v0, double v1, double v2, double v3, ae_state *_state);
ae_int_t idivup(ae_int_t a, ae_int_t b, ae_state *_state);
ae_int_t imin2(ae_int_t i0, ae_int_t i1, ae_state *_state);
ae_int_t imin3(ae_int_t i0, ae_int_t i1, ae_int_t i2, ae_state *_state);
ae_int_t imax2(ae_int_t i0, ae_int_t i1, ae_state *_state);
ae_int_t imax3(ae_int_t i0, ae_int_t i1, ae_int_t i2, ae_state *_state);
double rmax3(double r0, double r1, double r2, ae_state *_state);
double rmaxabs3(double r0, double r1, double r2, ae_state *_state);
double boundval(double x, double b1, double b2, ae_state *_state);
ae_int_t iboundval(ae_int_t x, ae_int_t b1, ae_int_t b2, ae_state *_state);
double rboundval(double x, double b1, double b2, ae_state *_state);
ae_bool bcase2(ae_bool cond, ae_bool v0, ae_bool v1, ae_state *_state);
ae_int_t icase2(ae_bool cond, ae_int_t v0, ae_int_t v1, ae_state *_state);
double rcase2(ae_bool cond, double v0, double v1, ae_state *_state);
ae_int_t countnz1(/* Real    */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state);
ae_int_t countnz2(/* Real    */ const ae_matrix* v,
     ae_int_t m,
     ae_int_t n,
     ae_state *_state);
void alloccomplex(ae_serializer* s, ae_complex v, ae_state *_state);
void serializecomplex(ae_serializer* s, ae_complex v, ae_state *_state);
ae_complex unserializecomplex(ae_serializer* s, ae_state *_state);
void allocrealarray(ae_serializer* s,
     /* Real    */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state);
void allocbooleanarray(ae_serializer* s,
     /* Boolean */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state);
void serializerealarray(ae_serializer* s,
     /* Real    */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state);
void serializebooleanarray(ae_serializer* s,
     /* Boolean */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state);
void unserializerealarray(ae_serializer* s,
     /* Real    */ ae_vector* v,
     ae_state *_state);
void unserializebooleanarray(ae_serializer* s,
     /* Boolean */ ae_vector* v,
     ae_state *_state);
void allocintegerarray(ae_serializer* s,
     /* Integer */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state);
void serializeintegerarray(ae_serializer* s,
     /* Integer */ const ae_vector* v,
     ae_int_t n,
     ae_state *_state);
void unserializeintegerarray(ae_serializer* s,
     /* Integer */ ae_vector* v,
     ae_state *_state);
void allocrealmatrix(ae_serializer* s,
     /* Real    */ const ae_matrix* v,
     ae_int_t n0,
     ae_int_t n1,
     ae_state *_state);
void serializerealmatrix(ae_serializer* s,
     /* Real    */ const ae_matrix* v,
     ae_int_t n0,
     ae_int_t n1,
     ae_state *_state);
void unserializerealmatrix(ae_serializer* s,
     /* Real    */ ae_matrix* v,
     ae_state *_state);
void copybooleanarray(/* Boolean */ const ae_vector* src,
     /* Boolean */ ae_vector* dst,
     ae_state *_state);
void copyintegerarray(/* Integer */ const ae_vector* src,
     /* Integer */ ae_vector* dst,
     ae_state *_state);
void copyrealarray(/* Real    */ const ae_vector* src,
     /* Real    */ ae_vector* dst,
     ae_state *_state);
void copyrealmatrix(/* Real    */ const ae_matrix* src,
     /* Real    */ ae_matrix* dst,
     ae_state *_state);
void unsetintegerarray(/* Integer */ ae_vector* a, ae_state *_state);
void unsetrealarray(/* Real    */ ae_vector* a, ae_state *_state);
void unsetrealmatrix(/* Real    */ ae_matrix* a, ae_state *_state);
void nbpoolinit(nbpool* pool, ae_int_t n, ae_state *_state);
void nbpoolretrieve(nbpool* pool,
     /* Boolean */ ae_vector* a,
     ae_state *_state);
void nbpoolrecycle(nbpool* pool,
     /* Boolean */ ae_vector* a,
     ae_state *_state);
void nipoolinit(nipool* pool, ae_int_t n, ae_state *_state);
void nipoolretrieve(nipool* pool,
     /* Integer */ ae_vector* a,
     ae_state *_state);
void nipoolrecycle(nipool* pool,
     /* Integer */ ae_vector* a,
     ae_state *_state);
void nrpoolinit(nrpool* pool, ae_int_t n, ae_state *_state);
void nrpoolretrieve(nrpool* pool,
     /* Real    */ ae_vector* a,
     ae_state *_state);
void nrpoolrecycle(nrpool* pool,
     /* Real    */ ae_vector* a,
     ae_state *_state);
void tiledsplit(ae_int_t tasksize,
     ae_int_t tilesize,
     ae_int_t* task0,
     ae_int_t* task1,
     ae_state *_state);
ae_int_t recsearch(/* Integer */ const ae_vector* a,
     ae_int_t nrec,
     ae_int_t nheader,
     ae_int_t i0,
     ae_int_t i1,
     /* Integer */ const ae_vector* b,
     ae_state *_state);
void splitlengtheven(ae_int_t tasksize,
     ae_int_t* task0,
     ae_int_t* task1,
     ae_state *_state);
ae_int_t chunkscount(ae_int_t tasksize,
     ae_int_t chunksize,
     ae_state *_state);
double sparselevel2density(ae_state *_state);
ae_int_t matrixtilesizea(ae_state *_state);
ae_int_t matrixtilesizeb(ae_state *_state);
double smpactivationlevel(ae_state *_state);
double spawnlevel(ae_state *_state);
void splitlength(ae_int_t tasksize,
     ae_int_t chunksize,
     ae_int_t* task0,
     ae_int_t* task1,
     ae_state *_state);
void tracevectorautoprec(/* Real    */ const ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state);
void tracerowautoprec(/* Real    */ const ae_matrix* a,
     ae_int_t i,
     ae_int_t j0,
     ae_int_t j1,
     ae_state *_state);
void tracevectorunscaledunshiftedautoprec(/* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* scl,
     ae_bool applyscl,
     /* Real    */ const ae_vector* sft,
     ae_bool applysft,
     ae_state *_state);
void tracerownrm1autoprec(/* Real    */ const ae_matrix* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t j0,
     ae_int_t j1,
     ae_state *_state);
void tracevectore3(/* Real    */ const ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state);
void tracevectore6(/* Real    */ const ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_state *_state);
void tracevectore615(/* Real    */ const ae_vector* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_bool usee15,
     ae_state *_state);
void tracerownrm1e6(/* Real    */ const ae_matrix* a,
     ae_int_t i0,
     ae_int_t i1,
     ae_int_t j0,
     ae_int_t j1,
     ae_state *_state);
void tracespaces(ae_int_t cnt, ae_state *_state);
double minspeedup(ae_state *_state);
#ifdef ALGLIB_NO_FAST_KERNELS
ae_int_t maxconcurrency(ae_state *_state);
#endif
void savgcounterinit(savgcounter* c, double priorvalue, ae_state *_state);
void savgcounterenqueue(savgcounter* c, double v, ae_state *_state);
double savgcounterget(const savgcounter* c, ae_state *_state);
void squantilecounterinit(squantilecounter* c,
     double priorvalue,
     ae_state *_state);
void squantilecounterenqueue(squantilecounter* c,
     double v,
     ae_state *_state);
double squantilecounterget(squantilecounter* c,
     double q,
     ae_state *_state);
void _apbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _apbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _apbuffers_clear(void* _p);
void _apbuffers_destroy(void* _p);
void _sboolean_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sboolean_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sboolean_clear(void* _p);
void _sboolean_destroy(void* _p);
void _sbooleanarray_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sbooleanarray_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sbooleanarray_clear(void* _p);
void _sbooleanarray_destroy(void* _p);
void _sinteger_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sinteger_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sinteger_clear(void* _p);
void _sinteger_destroy(void* _p);
void _sintegerarray_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sintegerarray_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sintegerarray_clear(void* _p);
void _sintegerarray_destroy(void* _p);
void _sreal_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sreal_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sreal_clear(void* _p);
void _sreal_destroy(void* _p);
void _srealarray_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _srealarray_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _srealarray_clear(void* _p);
void _srealarray_destroy(void* _p);
void _scomplex_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _scomplex_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _scomplex_clear(void* _p);
void _scomplex_destroy(void* _p);
void _scomplexarray_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _scomplexarray_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _scomplexarray_clear(void* _p);
void _scomplexarray_destroy(void* _p);
void _nbpool_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nbpool_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nbpool_clear(void* _p);
void _nbpool_destroy(void* _p);
void _nipool_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nipool_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nipool_clear(void* _p);
void _nipool_destroy(void* _p);
void _nrpool_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nrpool_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nrpool_clear(void* _p);
void _nrpool_destroy(void* _p);
void _savgcounter_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _savgcounter_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _savgcounter_clear(void* _p);
void _savgcounter_destroy(void* _p);
void _squantilecounter_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _squantilecounter_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _squantilecounter_clear(void* _p);
void _squantilecounter_destroy(void* _p);
#endif
#if defined(AE_COMPILE_ABLASF) || !defined(AE_PARTIAL_BUILD)
#ifdef ALGLIB_NO_FAST_KERNELS
double rdotv(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
double rdotvr(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
double rdotrr(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
double rdotv2(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void raddv(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmuladdv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rnegmuladdv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rcopymuladdv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* r,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rcopynegmuladdv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ const ae_vector* z,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* r,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void raddvx(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     ae_int_t offsy,
     /* Real    */ ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
#endif
void raddvc(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t colidx,
     ae_state *_state);
#ifdef ALGLIB_NO_FAST_KERNELS
void raddvr(ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmergemulv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmergemulvr(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmergemulrv(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmergedivv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmergedivvr(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmergedivrv(ae_int_t n,
     /* Real    */ const ae_matrix* y,
     ae_int_t rowidx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmergemaxv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmergemaxvr(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmergemaxrv(ae_int_t n,
     /* Real    */ const ae_matrix* x,
     ae_int_t rowidx,
     /* Real    */ ae_vector* y,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmergeminv(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmergeminvr(ae_int_t n,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmergeminrv(ae_int_t n,
     /* Real    */ const ae_matrix* x,
     ae_int_t rowidx,
     /* Real    */ ae_vector* y,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void raddrv(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* y,
     ae_int_t ridx,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void raddrr(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* y,
     ae_int_t ridxsrc,
     /* Real    */ ae_matrix* x,
     ae_int_t ridxdst,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmulv(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmulr(ae_int_t n,
     double v,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rsqrtv(ae_int_t n, /* Real    */ ae_vector* x, ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rsqrtr(ae_int_t n,
     /* Real    */ ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rmulvx(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
double rmaxv(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
double rmaxabsv(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
double rmaxr(ae_int_t n,
     /* Real    */ const ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
double rmaxabsr(ae_int_t n,
     /* Real    */ const ae_matrix* x,
     ae_int_t rowidx,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rsetv(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rsetvx(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_int_t offsx,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void isetv(ae_int_t n,
     ae_int_t v,
     /* Integer */ ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void bsetv(ae_int_t n,
     ae_bool v,
     /* Boolean */ ae_vector* x,
     ae_state *_state);
#endif
void csetv(ae_int_t n,
     ae_complex v,
     /* Complex */ ae_vector* x,
     ae_state *_state);
#ifdef ALGLIB_NO_FAST_KERNELS
void rsetm(ae_int_t m,
     ae_int_t n,
     double v,
     /* Real    */ ae_matrix* a,
     ae_state *_state);
#endif
void rsetallocv(ae_int_t n,
     double v,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void rsetallocm(ae_int_t m,
     ae_int_t n,
     double v,
     /* Real    */ ae_matrix* a,
     ae_state *_state);
void rallocv(ae_int_t n, /* Real    */ ae_vector* x, ae_state *_state);
void callocv(ae_int_t n, /* Complex */ ae_vector* x, ae_state *_state);
void iallocv(ae_int_t n, /* Integer */ ae_vector* x, ae_state *_state);
void ballocv(ae_int_t n, /* Boolean */ ae_vector* x, ae_state *_state);
void rallocm(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_state *_state);
void isetallocv(ae_int_t n,
     ae_int_t v,
     /* Integer */ ae_vector* x,
     ae_state *_state);
void bsetallocv(ae_int_t n,
     ae_bool v,
     /* Boolean */ ae_vector* x,
     ae_state *_state);
void csetallocv(ae_int_t n,
     ae_complex v,
     /* Complex */ ae_vector* x,
     ae_state *_state);
#ifdef ALGLIB_NO_FAST_KERNELS
void rsetr(ae_int_t n,
     double v,
     /* Real    */ ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
#endif
void rsetc(ae_int_t n,
     double v,
     /* Real    */ ae_matrix* a,
     ae_int_t j,
     ae_state *_state);
#ifdef ALGLIB_NO_FAST_KERNELS
void rcopyv(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void bcopyv(ae_int_t n,
     /* Boolean */ const ae_vector* x,
     /* Boolean */ ae_vector* y,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rcopyvx(ae_int_t n,
     /* Real    */ const ae_vector* x,
     ae_int_t offsx,
     /* Real    */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
#endif
void rcopyallocv(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void rcopym(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* x,
     /* Real    */ ae_matrix* y,
     ae_state *_state);
void rcopyallocm(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* x,
     /* Real    */ ae_matrix* y,
     ae_state *_state);
void icopyallocv(ae_int_t n,
     /* Integer */ const ae_vector* x,
     /* Integer */ ae_vector* y,
     ae_state *_state);
void bcopyallocv(ae_int_t n,
     /* Boolean */ const ae_vector* x,
     /* Boolean */ ae_vector* y,
     ae_state *_state);
#ifdef ALGLIB_NO_FAST_KERNELS
void icopyv(ae_int_t n,
     /* Integer */ const ae_vector* x,
     /* Integer */ ae_vector* y,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void icopyvx(ae_int_t n,
     /* Integer */ const ae_vector* x,
     ae_int_t offsx,
     /* Integer */ ae_vector* y,
     ae_int_t offsy,
     ae_state *_state);
#endif
void igrowv(ae_int_t newn, /* Integer */ ae_vector* x, ae_state *_state);
void rgrowv(ae_int_t newn, /* Real    */ ae_vector* x, ae_state *_state);
#ifdef ALGLIB_NO_FAST_KERNELS
void rcopymulv(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rcopymulvr(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* y,
     ae_int_t ridx,
     ae_state *_state);
#endif
void rcopymulvc(ae_int_t n,
     double v,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* y,
     ae_int_t cidx,
     ae_state *_state);
#ifdef ALGLIB_NO_FAST_KERNELS
void rcopyvr(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* a,
     ae_int_t i,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rcopyrv(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rcopyrr(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i,
     /* Real    */ ae_matrix* b,
     ae_int_t k,
     ae_state *_state);
#endif
void rcopyvc(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_matrix* a,
     ae_int_t j,
     ae_state *_state);
void rcopycv(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t j,
     /* Real    */ ae_vector* x,
     ae_state *_state);
#ifdef ALGLIB_NO_FAST_KERNELS
void rgemv(ae_int_t m,
     ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t opa,
     /* Real    */ const ae_vector* x,
     double beta,
     /* Real    */ ae_vector* y,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rgemvx(ae_int_t m,
     ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rger(ae_int_t m,
     ae_int_t n,
     double alpha,
     /* Real    */ const ae_vector* u,
     /* Real    */ const ae_vector* v,
     /* Real    */ ae_matrix* a,
     ae_state *_state);
#endif
#ifdef ALGLIB_NO_FAST_KERNELS
void rtrsvx(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_vector* x,
     ae_int_t ix,
     ae_state *_state);
#endif
ae_bool rmatrixgerf(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     double ralpha,
     /* Real    */ const ae_vector* u,
     ae_int_t iu,
     /* Real    */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state);
ae_bool cmatrixrank1f(ae_int_t m,
     ae_int_t n,
     /* Complex */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Complex */ const ae_vector* u,
     ae_int_t iu,
     /* Complex */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state);
ae_bool rmatrixrank1f(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ const ae_vector* u,
     ae_int_t iu,
     /* Real    */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state);
ae_bool cmatrixrighttrsmf(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Complex */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state);
ae_bool cmatrixlefttrsmf(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Complex */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state);
ae_bool rmatrixrighttrsmf(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state);
ae_bool rmatrixlefttrsmf(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state);
ae_bool cmatrixherkf(ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     /* Complex */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper,
     ae_state *_state);
ae_bool rmatrixsyrkf(ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper,
     ae_state *_state);
ae_bool cmatrixgemmf(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     ae_complex alpha,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     /* Complex */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     ae_complex beta,
     /* Complex */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);
void cmatrixgemmk(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     ae_complex alpha,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     /* Complex */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     ae_complex beta,
     /* Complex */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);
void rmatrixgemmk(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);
void rmatrixgemmk44v00(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);
void rmatrixgemmk44v01(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);
void rmatrixgemmk44v10(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);
void rmatrixgemmk44v11(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_HBLAS) || !defined(AE_PARTIAL_BUILD)
void hermitianmatrixvectormultiply(/* Complex */ const ae_matrix* a,
     ae_bool isupper,
     ae_int_t i1,
     ae_int_t i2,
     /* Complex */ const ae_vector* x,
     ae_complex alpha,
     /* Complex */ ae_vector* y,
     ae_state *_state);
void hermitianrank2update(/* Complex */ ae_matrix* a,
     ae_bool isupper,
     ae_int_t i1,
     ae_int_t i2,
     /* Complex */ const ae_vector* x,
     /* Complex */ const ae_vector* y,
     /* Complex */ ae_vector* t,
     ae_complex alpha,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_CREFLECTIONS) || !defined(AE_PARTIAL_BUILD)
void complexgeneratereflection(/* Complex */ ae_vector* x,
     ae_int_t n,
     ae_complex* tau,
     ae_state *_state);
void complexapplyreflectionfromtheleft(/* Complex */ ae_matrix* c,
     ae_complex tau,
     /* Complex */ const ae_vector* v,
     ae_int_t m1,
     ae_int_t m2,
     ae_int_t n1,
     ae_int_t n2,
     /* Complex */ ae_vector* work,
     ae_state *_state);
void complexapplyreflectionfromtheright(/* Complex */ ae_matrix* c,
     ae_complex tau,
     /* Complex */ ae_vector* v,
     ae_int_t m1,
     ae_int_t m2,
     ae_int_t n1,
     ae_int_t n2,
     /* Complex */ ae_vector* work,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_SBLAS) || !defined(AE_PARTIAL_BUILD)
void symmetricmatrixvectormultiply(/* Real    */ const ae_matrix* a,
     ae_bool isupper,
     ae_int_t i1,
     ae_int_t i2,
     /* Real    */ const ae_vector* x,
     double alpha,
     /* Real    */ ae_vector* y,
     ae_state *_state);
void symmetricrank2update(/* Real    */ ae_matrix* a,
     ae_bool isupper,
     ae_int_t i1,
     ae_int_t i2,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* y,
     /* Real    */ ae_vector* t,
     double alpha,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_ABLASMKL) || !defined(AE_PARTIAL_BUILD)
ae_bool rmatrixgermkl(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     double alpha,
     /* Real    */ const ae_vector* u,
     ae_int_t iu,
     /* Real    */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state);
ae_bool cmatrixrank1mkl(ae_int_t m,
     ae_int_t n,
     /* Complex */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Complex */ const ae_vector* u,
     ae_int_t iu,
     /* Complex */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state);
ae_bool rmatrixrank1mkl(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     /* Real    */ const ae_vector* u,
     ae_int_t iu,
     /* Real    */ const ae_vector* v,
     ae_int_t iv,
     ae_state *_state);
ae_bool cmatrixmvmkl(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     /* Complex */ const ae_vector* x,
     ae_int_t ix,
     /* Complex */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state);
ae_bool rmatrixmvmkl(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     /* Real    */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state);
ae_bool rmatrixgemvmkl(ae_int_t m,
     ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t opa,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state);
ae_bool rmatrixtrsvmkl(ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_vector* x,
     ae_int_t ix,
     ae_state *_state);
ae_bool rmatrixsyrkmkl(ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper,
     ae_state *_state);
ae_bool cmatrixherkmkl(ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     /* Complex */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_bool isupper,
     ae_state *_state);
ae_bool rmatrixgemmmkl(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     /* Real    */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     double beta,
     /* Real    */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);
ae_bool rmatrixsymvmkl(ae_int_t n,
     double alpha,
     /* Real    */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_bool isupper,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state);
ae_bool cmatrixgemmmkl(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     ae_complex alpha,
     /* Complex */ const ae_matrix* a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     /* Complex */ const ae_matrix* b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     ae_complex beta,
     /* Complex */ ae_matrix* c,
     ae_int_t ic,
     ae_int_t jc,
     ae_state *_state);
ae_bool cmatrixlefttrsmmkl(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Complex */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state);
ae_bool cmatrixrighttrsmmkl(ae_int_t m,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Complex */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state);
ae_bool rmatrixlefttrsmmkl(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state);
ae_bool rmatrixrighttrsmmkl(ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t j1,
     ae_bool isupper,
     ae_bool isunit,
     ae_int_t optype,
     /* Real    */ ae_matrix* x,
     ae_int_t i2,
     ae_int_t j2,
     ae_state *_state);
ae_bool spdmatrixcholeskymkl(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t n,
     ae_bool isupper,
     ae_bool* cholresult,
     ae_state *_state);
ae_bool rmatrixplumkl(/* Real    */ ae_matrix* a,
     ae_int_t offs,
     ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_vector* pivots,
     ae_state *_state);
ae_bool rmatrixbdmkl(/* Real    */ ae_matrix* a,
     ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* e,
     /* Real    */ ae_vector* tauq,
     /* Real    */ ae_vector* taup,
     ae_state *_state);
ae_bool rmatrixbdmultiplybymkl(/* Real    */ const ae_matrix* qp,
     ae_int_t m,
     ae_int_t n,
     /* Real    */ const ae_vector* tauq,
     /* Real    */ const ae_vector* taup,
     /* Real    */ ae_matrix* z,
     ae_int_t zrows,
     ae_int_t zcolumns,
     ae_bool byq,
     ae_bool fromtheright,
     ae_bool dotranspose,
     ae_state *_state);
ae_bool rmatrixhessenbergmkl(/* Real    */ ae_matrix* a,
     ae_int_t n,
     /* Real    */ ae_vector* tau,
     ae_state *_state);
ae_bool rmatrixhessenbergunpackqmkl(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     /* Real    */ const ae_vector* tau,
     /* Real    */ ae_matrix* q,
     ae_state *_state);
ae_bool smatrixtdmkl(/* Real    */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* tau,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* e,
     ae_state *_state);
ae_bool smatrixtdunpackqmkl(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_vector* tau,
     /* Real    */ ae_matrix* q,
     ae_state *_state);
ae_bool hmatrixtdmkl(/* Complex */ ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* tau,
     /* Real    */ ae_vector* d,
     /* Real    */ ae_vector* e,
     ae_state *_state);
ae_bool hmatrixtdunpackqmkl(/* Complex */ const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ const ae_vector* tau,
     /* Complex */ ae_matrix* q,
     ae_state *_state);
ae_bool rmatrixbdsvdmkl(/* Real    */ ae_vector* d,
     /* Real    */ ae_vector* e,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_matrix* u,
     ae_int_t nru,
     /* Real    */ ae_matrix* c,
     ae_int_t ncc,
     /* Real    */ ae_matrix* vt,
     ae_int_t ncvt,
     ae_bool* svdresult,
     ae_state *_state);
ae_bool rmatrixinternalschurdecompositionmkl(/* Real    */ ae_matrix* h,
     ae_int_t n,
     ae_int_t tneeded,
     ae_int_t zneeded,
     /* Real    */ ae_vector* wr,
     /* Real    */ ae_vector* wi,
     /* Real    */ ae_matrix* z,
     ae_int_t* info,
     ae_state *_state);
ae_bool rmatrixinternaltrevcmkl(/* Real    */ const ae_matrix* t,
     ae_int_t n,
     ae_int_t side,
     ae_int_t howmny,
     /* Real    */ ae_matrix* vl,
     /* Real    */ ae_matrix* vr,
     ae_int_t* m,
     ae_int_t* info,
     ae_state *_state);
ae_bool smatrixtdevdmkl(/* Real    */ ae_vector* d,
     /* Real    */ ae_vector* e,
     ae_int_t n,
     ae_int_t zneeded,
     /* Real    */ ae_matrix* z,
     ae_bool* evdresult,
     ae_state *_state);
ae_bool sparsegemvcrsmkl(ae_int_t opa,
     ae_int_t arows,
     ae_int_t acols,
     double alpha,
     /* Real    */ const ae_vector* vals,
     /* Integer */ const ae_vector* cidx,
     /* Integer */ const ae_vector* ridx,
     /* Real    */ const ae_vector* x,
     ae_int_t ix,
     double beta,
     /* Real    */ ae_vector* y,
     ae_int_t iy,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_SCODES) || !defined(AE_PARTIAL_BUILD)
ae_int_t getrdfserializationcode(ae_state *_state);
ae_int_t getkdtreeserializationcode(ae_state *_state);
ae_int_t getmlpserializationcode(ae_state *_state);
ae_int_t getmlpeserializationcode(ae_state *_state);
ae_int_t getrbfserializationcode(ae_state *_state);
ae_int_t getspline2dserializationcode(ae_state *_state);
ae_int_t getidwserializationcode(ae_state *_state);
ae_int_t getsparsematrixserializationcode(ae_state *_state);
ae_int_t getspline2dwithmissingnodesserializationcode(ae_state *_state);
ae_int_t getspline1dserializationcode(ae_state *_state);
ae_int_t getknnserializationcode(ae_state *_state);
ae_int_t getlptestserializationcode(ae_state *_state);
#endif
#if defined(AE_COMPILE_TSORT) || !defined(AE_PARTIAL_BUILD)
void tagsort(/* Real    */ ae_vector* a,
     ae_int_t n,
     /* Integer */ ae_vector* p1,
     /* Integer */ ae_vector* p2,
     ae_state *_state);
void tagsortbuf(/* Real    */ ae_vector* a,
     ae_int_t n,
     /* Integer */ ae_vector* p1,
     /* Integer */ ae_vector* p2,
     apbuffers* buf,
     ae_state *_state);
void tagsortfasti(/* Real    */ ae_vector* a,
     /* Integer */ ae_vector* b,
     /* Real    */ ae_vector* bufa,
     /* Integer */ ae_vector* bufb,
     ae_int_t n,
     ae_state *_state);
void tagsortfastr(/* Real    */ ae_vector* a,
     /* Real    */ ae_vector* b,
     /* Real    */ ae_vector* bufa,
     /* Real    */ ae_vector* bufb,
     ae_int_t n,
     ae_state *_state);
void tagsortfast(/* Real    */ ae_vector* a,
     /* Real    */ ae_vector* bufa,
     ae_int_t n,
     ae_state *_state);
void tagsortmiddleir(/* Integer */ ae_vector* a,
     /* Real    */ ae_vector* b,
     ae_int_t offset,
     ae_int_t n,
     ae_state *_state);
void tagsortmiddleri(/* Real    */ ae_vector* a,
     /* Integer */ ae_vector* b,
     ae_int_t offset,
     ae_int_t n,
     ae_state *_state);
void tagsortmiddleii(/* Integer */ ae_vector* a,
     /* Integer */ ae_vector* b,
     ae_int_t offset,
     ae_int_t n,
     ae_state *_state);
void tagsortmiddlei(/* Integer */ ae_vector* a,
     ae_int_t offset,
     ae_int_t n,
     ae_state *_state);
void sortmiddlei(/* Integer */ ae_vector* a,
     ae_int_t offset,
     ae_int_t n,
     ae_state *_state);
void tagheappushi(/* Real    */ ae_vector* a,
     /* Integer */ ae_vector* b,
     ae_int_t* n,
     double va,
     ae_int_t vb,
     ae_state *_state);
void tagheapreplacetopi(/* Real    */ ae_vector* a,
     /* Integer */ ae_vector* b,
     ae_int_t n,
     double va,
     ae_int_t vb,
     ae_state *_state);
void tagheappopi(/* Real    */ ae_vector* a,
     /* Integer */ ae_vector* b,
     ae_int_t* n,
     ae_state *_state);
ae_int_t lowerbound(/* Real    */ const ae_vector* a,
     ae_int_t n,
     double t,
     ae_state *_state);
ae_int_t upperbound(/* Real    */ const ae_vector* a,
     ae_int_t n,
     double t,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_BLAS) || !defined(AE_PARTIAL_BUILD)
double vectornorm2(/* Real    */ const ae_vector* x,
     ae_int_t i1,
     ae_int_t i2,
     ae_state *_state);
ae_int_t vectoridxabsmax(/* Real    */ const ae_vector* x,
     ae_int_t i1,
     ae_int_t i2,
     ae_state *_state);
ae_int_t columnidxabsmax(/* Real    */ const ae_matrix* x,
     ae_int_t i1,
     ae_int_t i2,
     ae_int_t j,
     ae_state *_state);
ae_int_t rowidxabsmax(/* Real    */ const ae_matrix* x,
     ae_int_t j1,
     ae_int_t j2,
     ae_int_t i,
     ae_state *_state);
double upperhessenberg1norm(/* Real    */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t i2,
     ae_int_t j1,
     ae_int_t j2,
     /* Real    */ ae_vector* work,
     ae_state *_state);
void copymatrix(/* Real    */ const ae_matrix* a,
     ae_int_t is1,
     ae_int_t is2,
     ae_int_t js1,
     ae_int_t js2,
     /* Real    */ ae_matrix* b,
     ae_int_t id1,
     ae_int_t id2,
     ae_int_t jd1,
     ae_int_t jd2,
     ae_state *_state);
void inplacetranspose(/* Real    */ ae_matrix* a,
     ae_int_t i1,
     ae_int_t i2,
     ae_int_t j1,
     ae_int_t j2,
     /* Real    */ ae_vector* work,
     ae_state *_state);
void copyandtranspose(/* Real    */ const ae_matrix* a,
     ae_int_t is1,
     ae_int_t is2,
     ae_int_t js1,
     ae_int_t js2,
     /* Real    */ ae_matrix* b,
     ae_int_t id1,
     ae_int_t id2,
     ae_int_t jd1,
     ae_int_t jd2,
     ae_state *_state);
void matrixvectormultiply(/* Real    */ const ae_matrix* a,
     ae_int_t i1,
     ae_int_t i2,
     ae_int_t j1,
     ae_int_t j2,
     ae_bool trans,
     /* Real    */ const ae_vector* x,
     ae_int_t ix1,
     ae_int_t ix2,
     double alpha,
     /* Real    */ ae_vector* y,
     ae_int_t iy1,
     ae_int_t iy2,
     double beta,
     ae_state *_state);
double pythag2(double x, double y, ae_state *_state);
void matrixmatrixmultiply(/* Real    */ const ae_matrix* a,
     ae_int_t ai1,
     ae_int_t ai2,
     ae_int_t aj1,
     ae_int_t aj2,
     ae_bool transa,
     /* Real    */ const ae_matrix* b,
     ae_int_t bi1,
     ae_int_t bi2,
     ae_int_t bj1,
     ae_int_t bj2,
     ae_bool transb,
     double alpha,
     /* Real    */ ae_matrix* c,
     ae_int_t ci1,
     ae_int_t ci2,
     ae_int_t cj1,
     ae_int_t cj2,
     double beta,
     /* Real    */ ae_vector* work,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_ROTATIONS) || !defined(AE_PARTIAL_BUILD)
void applyrotationsfromtheleft(ae_bool isforward,
     ae_int_t m1,
     ae_int_t m2,
     ae_int_t n1,
     ae_int_t n2,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_vector* s,
     /* Real    */ ae_matrix* a,
     /* Real    */ ae_vector* work,
     ae_state *_state);
void applyrotationsfromtheright(ae_bool isforward,
     ae_int_t m1,
     ae_int_t m2,
     ae_int_t n1,
     ae_int_t n2,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_vector* s,
     /* Real    */ ae_matrix* a,
     /* Real    */ ae_vector* work,
     ae_state *_state);
void generaterotation(double f,
     double g,
     double* cs,
     double* sn,
     double* r,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_BASICSTATOPS) || !defined(AE_PARTIAL_BUILD)
void rankx(/* Real    */ ae_vector* x,
     ae_int_t n,
     ae_bool iscentered,
     apbuffers* buf,
     ae_state *_state);
void rankxuntied(/* Real    */ ae_vector* x,
     ae_int_t n,
     apbuffers* buf,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_APSTRUCT) || !defined(AE_PARTIAL_BUILD)
void nisinitemptyslow(ae_int_t n, niset* sa, ae_state *_state);
void niscopy(const niset* ssrc, niset* sdst, ae_state *_state);
void nisaddelement(niset* sa, ae_int_t k, ae_state *_state);
void nissubtract1(niset* sa, const niset* src, ae_state *_state);
void nisclear(niset* sa, ae_state *_state);
ae_int_t niscount(const niset* sa, ae_state *_state);
ae_bool nisequal(const niset* s0, const niset* s1, ae_state *_state);
void nisstartenumeration(niset* sa, ae_state *_state);
ae_bool nisenumerate(niset* sa, ae_int_t* i, ae_state *_state);
void _niset_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _niset_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _niset_clear(void* _p);
void _niset_destroy(void* _p);
#endif
#if defined(AE_COMPILE_TRLINSOLVE) || !defined(AE_PARTIAL_BUILD)
void rmatrixtrsafesolve(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     double* s,
     ae_bool isupper,
     ae_bool istrans,
     ae_bool isunit,
     ae_state *_state);
void safesolvetriangular(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     double* s,
     ae_bool isupper,
     ae_bool istrans,
     ae_bool isunit,
     ae_bool normin,
     /* Real    */ ae_vector* cnorm,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_SAFESOLVE) || !defined(AE_PARTIAL_BUILD)
ae_bool rmatrixscaledtrsafesolve(/* Real    */ const ae_matrix* a,
     double sa,
     ae_int_t n,
     /* Real    */ ae_vector* x,
     ae_bool isupper,
     ae_int_t trans,
     ae_bool isunit,
     double maxgrowth,
     ae_state *_state);
ae_bool cmatrixscaledtrsafesolve(/* Complex */ const ae_matrix* a,
     double sa,
     ae_int_t n,
     /* Complex */ ae_vector* x,
     ae_bool isupper,
     ae_int_t trans,
     ae_bool isunit,
     double maxgrowth,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_XBLAS) || !defined(AE_PARTIAL_BUILD)
void xdot(/* Real    */ const ae_vector* a,
     /* Real    */ const ae_vector* b,
     ae_int_t n,
     /* Real    */ ae_vector* temp,
     double* r,
     double* rerr,
     ae_state *_state);
void xcdot(/* Complex */ const ae_vector* a,
     /* Complex */ const ae_vector* b,
     ae_int_t n,
     /* Real    */ ae_vector* temp,
     ae_complex* r,
     double* rerr,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_LINMIN) || !defined(AE_PARTIAL_BUILD)
void linminnormalized(/* Real    */ ae_vector* d,
     double* stp,
     ae_int_t n,
     ae_state *_state);
void mcsrch(ae_int_t n,
     /* Real    */ ae_vector* x,
     double* f,
     /* Real    */ ae_vector* g,
     /* Real    */ const ae_vector* s,
     double* stp,
     double stpmax,
     double gtol,
     ae_int_t* info,
     ae_int_t* nfev,
     /* Real    */ ae_vector* wa,
     linminstate* state,
     ae_int_t* stage,
     ae_state *_state);
void armijocreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double f,
     /* Real    */ const ae_vector* s,
     double stp,
     double stpmax,
     ae_int_t fmax,
     armijostate* state,
     ae_state *_state);
ae_bool armijoiteration(armijostate* state, ae_state *_state);
void armijoresults(armijostate* state,
     ae_int_t* info,
     double* stp,
     double* f,
     ae_state *_state);
void _linminstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _linminstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _linminstate_clear(void* _p);
void _linminstate_destroy(void* _p);
void _armijostate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _armijostate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _armijostate_clear(void* _p);
void _armijostate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_NEARUNITYUNIT) || !defined(AE_PARTIAL_BUILD)
double nulog1p(double x, ae_state *_state);
double nuexpm1(double x, ae_state *_state);
double nucosm1(double x, ae_state *_state);
#endif
#if defined(AE_COMPILE_NTHEORY) || !defined(AE_PARTIAL_BUILD)
void findprimitiverootandinverse(ae_int_t n,
     ae_int_t* proot,
     ae_int_t* invproot,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_FTBASE) || !defined(AE_PARTIAL_BUILD)
void ftcomplexfftplan(ae_int_t n,
     ae_int_t k,
     fasttransformplan* plan,
     ae_state *_state);
void ftapplyplan(fasttransformplan* plan,
     /* Real    */ ae_vector* a,
     ae_int_t offsa,
     ae_int_t repcnt,
     ae_state *_state);
void ftbasefactorize(ae_int_t n,
     ae_int_t tasktype,
     ae_int_t* n1,
     ae_int_t* n2,
     ae_state *_state);
ae_bool ftbaseissmooth(ae_int_t n, ae_state *_state);
ae_int_t ftbasefindsmooth(ae_int_t n, ae_state *_state);
ae_int_t ftbasefindsmootheven(ae_int_t n, ae_state *_state);
double ftbasegetflopestimate(ae_int_t n, ae_state *_state);
void _fasttransformplan_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _fasttransformplan_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _fasttransformplan_clear(void* _p);
void _fasttransformplan_destroy(void* _p);
#endif
#if defined(AE_COMPILE_HPCCORES) || !defined(AE_PARTIAL_BUILD)
void hpcpreparechunkedgradient(/* Real    */ const ae_vector* weights,
     ae_int_t wcount,
     ae_int_t ntotal,
     ae_int_t nin,
     ae_int_t nout,
     mlpbuffers* buf,
     ae_state *_state);
void hpcfinalizechunkedgradient(const mlpbuffers* buf,
     /* Real    */ ae_vector* grad,
     ae_state *_state);
ae_bool hpcchunkedgradient(/* Real    */ const ae_vector* weights,
     /* Integer */ const ae_vector* structinfo,
     /* Real    */ const ae_vector* columnmeans,
     /* Real    */ const ae_vector* columnsigmas,
     /* Real    */ const ae_matrix* xy,
     ae_int_t cstart,
     ae_int_t csize,
     /* Real    */ ae_vector* batch4buf,
     /* Real    */ ae_vector* hpcbuf,
     double* e,
     ae_bool naturalerrorfunc,
     ae_state *_state);
ae_bool hpcchunkedprocess(/* Real    */ const ae_vector* weights,
     /* Integer */ const ae_vector* structinfo,
     /* Real    */ const ae_vector* columnmeans,
     /* Real    */ const ae_vector* columnsigmas,
     /* Real    */ const ae_matrix* xy,
     ae_int_t cstart,
     ae_int_t csize,
     /* Real    */ ae_vector* batch4buf,
     /* Real    */ ae_vector* hpcbuf,
     ae_state *_state);
void _mlpbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mlpbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mlpbuffers_clear(void* _p);
void _mlpbuffers_destroy(void* _p);
#endif
#if defined(AE_COMPILE_ALGLIBBASICS) || !defined(AE_PARTIAL_BUILD)
#endif

}
#endif

