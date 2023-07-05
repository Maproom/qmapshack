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
#include "solvers.h"

// disable some irrelevant warnings
#if (AE_COMPILER==AE_MSVC) && !defined(AE_ALL_WARNINGS)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#pragma warning(disable:4611)
#pragma warning(disable:4702)
#pragma warning(disable:4996)
#endif

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS IMPLEMENTATION OF C++ INTERFACE
//
/////////////////////////////////////////////////////////////////////////
namespace alglib
{

#if defined(AE_COMPILE_POLYNOMIALSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_DIRECTDENSESOLVERS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_DIRECTSPARSESOLVERS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_ITERATIVESPARSE) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_LINCG) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_LINLSQR) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_NLEQ) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_POLYNOMIALSOLVER) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************

*************************************************************************/
_polynomialsolverreport_owner::_polynomialsolverreport_owner()
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_polynomialsolverreport_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    p_struct = (alglib_impl::polynomialsolverreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::polynomialsolverreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::polynomialsolverreport));
    alglib_impl::_polynomialsolverreport_init(p_struct, &_state, ae_false);
    ae_state_clear(&_state);
}

_polynomialsolverreport_owner::_polynomialsolverreport_owner(const _polynomialsolverreport_owner &rhs)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_polynomialsolverreport_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: polynomialsolverreport copy constructor failure (source is not initialized)", &_state);
    p_struct = (alglib_impl::polynomialsolverreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::polynomialsolverreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::polynomialsolverreport));
    alglib_impl::_polynomialsolverreport_init_copy(p_struct, const_cast<alglib_impl::polynomialsolverreport*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
}

_polynomialsolverreport_owner& _polynomialsolverreport_owner::operator=(const _polynomialsolverreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return *this;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    alglib_impl::ae_assert(p_struct!=NULL, "ALGLIB: polynomialsolverreport assignment constructor failure (destination is not initialized)", &_state);
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: polynomialsolverreport assignment constructor failure (source is not initialized)", &_state);
    alglib_impl::_polynomialsolverreport_destroy(p_struct);
    memset(p_struct, 0, sizeof(alglib_impl::polynomialsolverreport));
    alglib_impl::_polynomialsolverreport_init_copy(p_struct, const_cast<alglib_impl::polynomialsolverreport*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
    return *this;
}

_polynomialsolverreport_owner::~_polynomialsolverreport_owner()
{
    if( p_struct!=NULL )
    {
        alglib_impl::_polynomialsolverreport_destroy(p_struct);
        ae_free(p_struct);
    }
}

alglib_impl::polynomialsolverreport* _polynomialsolverreport_owner::c_ptr()
{
    return p_struct;
}

const alglib_impl::polynomialsolverreport* _polynomialsolverreport_owner::c_ptr() const
{
    return p_struct;
}
polynomialsolverreport::polynomialsolverreport() : _polynomialsolverreport_owner() ,maxerr(p_struct->maxerr)
{
}

polynomialsolverreport::polynomialsolverreport(const polynomialsolverreport &rhs):_polynomialsolverreport_owner(rhs) ,maxerr(p_struct->maxerr)
{
}

polynomialsolverreport& polynomialsolverreport::operator=(const polynomialsolverreport &rhs)
{
    if( this==&rhs )
        return *this;
    _polynomialsolverreport_owner::operator=(rhs);
    return *this;
}

polynomialsolverreport::~polynomialsolverreport()
{
}

/*************************************************************************
Polynomial root finding.

This function returns all roots of the polynomial
    P(x) = a0 + a1*x + a2*x^2 + ... + an*x^n
Both real and complex roots are returned (see below).

INPUT PARAMETERS:
    A       -   array[N+1], polynomial coefficients:
                * A[0] is constant term
                * A[N] is a coefficient of X^N
    N       -   polynomial degree

OUTPUT PARAMETERS:
    X       -   array of complex roots:
                * for isolated real root, X[I] is strictly real: IMAGE(X[I])=0
                * complex roots are always returned in pairs - roots occupy
                  positions I and I+1, with:
                  * X[I+1]=Conj(X[I])
                  * IMAGE(X[I]) > 0
                  * IMAGE(X[I+1]) = -IMAGE(X[I]) < 0
                * multiple real roots may have non-zero imaginary part due
                  to roundoff errors. There is no reliable way to distinguish
                  real root of multiplicity 2 from two  complex  roots  in
                  the presence of roundoff errors.
    Rep     -   report, additional information, following fields are set:
                * Rep.MaxErr - max( |P(xi)| )  for  i=0..N-1.  This  field
                  allows to quickly estimate "quality" of the roots  being
                  returned.

NOTE:   this function uses companion matrix method to find roots. In  case
        internal EVD  solver  fails  do  find  eigenvalues,  exception  is
        generated.

NOTE:   roots are not "polished" and  no  matrix  balancing  is  performed
        for them.

  -- ALGLIB --
     Copyright 24.02.2014 by Bochkanov Sergey
*************************************************************************/
void polynomialsolve(const real_1d_array &a, const ae_int_t n, complex_1d_array &x, polynomialsolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::polynomialsolve(a.c_ptr(), n, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

#if defined(AE_COMPILE_DIRECTDENSESOLVERS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************

*************************************************************************/
_densesolverreport_owner::_densesolverreport_owner()
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_densesolverreport_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    p_struct = (alglib_impl::densesolverreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::densesolverreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::densesolverreport));
    alglib_impl::_densesolverreport_init(p_struct, &_state, ae_false);
    ae_state_clear(&_state);
}

_densesolverreport_owner::_densesolverreport_owner(const _densesolverreport_owner &rhs)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_densesolverreport_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: densesolverreport copy constructor failure (source is not initialized)", &_state);
    p_struct = (alglib_impl::densesolverreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::densesolverreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::densesolverreport));
    alglib_impl::_densesolverreport_init_copy(p_struct, const_cast<alglib_impl::densesolverreport*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
}

_densesolverreport_owner& _densesolverreport_owner::operator=(const _densesolverreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return *this;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    alglib_impl::ae_assert(p_struct!=NULL, "ALGLIB: densesolverreport assignment constructor failure (destination is not initialized)", &_state);
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: densesolverreport assignment constructor failure (source is not initialized)", &_state);
    alglib_impl::_densesolverreport_destroy(p_struct);
    memset(p_struct, 0, sizeof(alglib_impl::densesolverreport));
    alglib_impl::_densesolverreport_init_copy(p_struct, const_cast<alglib_impl::densesolverreport*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
    return *this;
}

_densesolverreport_owner::~_densesolverreport_owner()
{
    if( p_struct!=NULL )
    {
        alglib_impl::_densesolverreport_destroy(p_struct);
        ae_free(p_struct);
    }
}

alglib_impl::densesolverreport* _densesolverreport_owner::c_ptr()
{
    return p_struct;
}

const alglib_impl::densesolverreport* _densesolverreport_owner::c_ptr() const
{
    return p_struct;
}
densesolverreport::densesolverreport() : _densesolverreport_owner() ,terminationtype(p_struct->terminationtype),r1(p_struct->r1),rinf(p_struct->rinf)
{
}

densesolverreport::densesolverreport(const densesolverreport &rhs):_densesolverreport_owner(rhs) ,terminationtype(p_struct->terminationtype),r1(p_struct->r1),rinf(p_struct->rinf)
{
}

densesolverreport& densesolverreport::operator=(const densesolverreport &rhs)
{
    if( this==&rhs )
        return *this;
    _densesolverreport_owner::operator=(rhs);
    return *this;
}

densesolverreport::~densesolverreport()
{
}


/*************************************************************************

*************************************************************************/
_densesolverlsreport_owner::_densesolverlsreport_owner()
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_densesolverlsreport_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    p_struct = (alglib_impl::densesolverlsreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::densesolverlsreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::densesolverlsreport));
    alglib_impl::_densesolverlsreport_init(p_struct, &_state, ae_false);
    ae_state_clear(&_state);
}

_densesolverlsreport_owner::_densesolverlsreport_owner(const _densesolverlsreport_owner &rhs)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_densesolverlsreport_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: densesolverlsreport copy constructor failure (source is not initialized)", &_state);
    p_struct = (alglib_impl::densesolverlsreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::densesolverlsreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::densesolverlsreport));
    alglib_impl::_densesolverlsreport_init_copy(p_struct, const_cast<alglib_impl::densesolverlsreport*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
}

_densesolverlsreport_owner& _densesolverlsreport_owner::operator=(const _densesolverlsreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return *this;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    alglib_impl::ae_assert(p_struct!=NULL, "ALGLIB: densesolverlsreport assignment constructor failure (destination is not initialized)", &_state);
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: densesolverlsreport assignment constructor failure (source is not initialized)", &_state);
    alglib_impl::_densesolverlsreport_destroy(p_struct);
    memset(p_struct, 0, sizeof(alglib_impl::densesolverlsreport));
    alglib_impl::_densesolverlsreport_init_copy(p_struct, const_cast<alglib_impl::densesolverlsreport*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
    return *this;
}

_densesolverlsreport_owner::~_densesolverlsreport_owner()
{
    if( p_struct!=NULL )
    {
        alglib_impl::_densesolverlsreport_destroy(p_struct);
        ae_free(p_struct);
    }
}

alglib_impl::densesolverlsreport* _densesolverlsreport_owner::c_ptr()
{
    return p_struct;
}

const alglib_impl::densesolverlsreport* _densesolverlsreport_owner::c_ptr() const
{
    return p_struct;
}
densesolverlsreport::densesolverlsreport() : _densesolverlsreport_owner() ,terminationtype(p_struct->terminationtype),r2(p_struct->r2),cx(&p_struct->cx),n(p_struct->n),k(p_struct->k)
{
}

densesolverlsreport::densesolverlsreport(const densesolverlsreport &rhs):_densesolverlsreport_owner(rhs) ,terminationtype(p_struct->terminationtype),r2(p_struct->r2),cx(&p_struct->cx),n(p_struct->n),k(p_struct->k)
{
}

densesolverlsreport& densesolverlsreport::operator=(const densesolverlsreport &rhs)
{
    if( this==&rhs )
        return *this;
    _densesolverlsreport_owner::operator=(rhs);
    return *this;
}

densesolverlsreport::~densesolverlsreport()
{
}

/*************************************************************************
Dense solver for A*x=b with N*N real matrix A and N*1 real vectorx  x  and
b. This is "slow-but-feature rich" version of the  linear  solver.  Faster
version is RMatrixSolveFast() function.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations. It is also very significant on small matrices.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, RMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, the following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolve(const real_2d_array &a, const ae_int_t n, const real_1d_array &b, real_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::rmatrixsolve(a.c_ptr(), n, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver for A*x=b with N*N real matrix A and N*1 real vectorx  x  and
b. This is "slow-but-feature rich" version of the  linear  solver.  Faster
version is RMatrixSolveFast() function.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations. It is also very significant on small matrices.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, RMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, the following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void rmatrixsolve(const real_2d_array &a, const real_1d_array &b, real_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'rmatrixsolve': looks like one of arguments has wrong size");
    n = a.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::rmatrixsolve(a.c_ptr(), n, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where A is NxN non-denegerate
real matrix, x  and  b  are  vectors.  This is a "fast" version of  linear
solver which does NOT provide  any  additional  functions  like  condition
number estimation or iterative refinement.

Algorithm features:
* efficient algorithm O(N^3) complexity
* no performance overhead from additional functionality

If you need condition number estimation or iterative refinement, use  more
feature-rich version - RMatrixSolve().

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 16.03.2015 by Bochkanov Sergey
*************************************************************************/
bool rmatrixsolvefast(const real_2d_array &a, const ae_int_t n, real_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::rmatrixsolvefast(a.c_ptr(), n, b.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where A is NxN non-denegerate
real matrix, x  and  b  are  vectors.  This is a "fast" version of  linear
solver which does NOT provide  any  additional  functions  like  condition
number estimation or iterative refinement.

Algorithm features:
* efficient algorithm O(N^3) complexity
* no performance overhead from additional functionality

If you need condition number estimation or iterative refinement, use  more
feature-rich version - RMatrixSolve().

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 16.03.2015 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool rmatrixsolvefast(const real_2d_array &a, real_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'rmatrixsolvefast': looks like one of arguments has wrong size");
    n = a.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::rmatrixsolvefast(a.c_ptr(), n, b.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Dense solver.

Similar to RMatrixSolve() but solves task with multiple right parts (where
b and x are NxM matrices). This is  "slow-but-robust"  version  of  linear
solver with additional functionality  like  condition  number  estimation.
There also exists faster version - RMatrixSolveMFast().

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* optional iterative refinement
* O(N^3+M*N^2) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations. It also very significant on small matrices.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, RMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolvem(const real_2d_array &a, const ae_int_t n, const real_2d_array &b, const ae_int_t m, const bool rfs, real_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::rmatrixsolvem(a.c_ptr(), n, b.c_ptr(), m, rfs, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver.

Similar to RMatrixSolve() but solves task with multiple right parts (where
b and x are NxM matrices). This is  "slow-but-robust"  version  of  linear
solver with additional functionality  like  condition  number  estimation.
There also exists faster version - RMatrixSolveMFast().

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* optional iterative refinement
* O(N^3+M*N^2) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations. It also very significant on small matrices.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, RMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void rmatrixsolvem(const real_2d_array &a, const real_2d_array &b, const bool rfs, real_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'rmatrixsolvem': looks like one of arguments has wrong size");
    n = a.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::rmatrixsolvem(a.c_ptr(), n, b.c_ptr(), m, rfs, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver.

Similar to RMatrixSolve() but solves task with multiple right parts (where
b and x are NxM matrices). This is "fast" version of linear  solver  which
does NOT offer additional functions like condition  number  estimation  or
iterative refinement.

Algorithm features:
* O(N^3+M*N^2) complexity
* no additional functionality, highest performance

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True for well-conditioned matrix
    False for extremely badly conditioned or exactly singular problem

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
bool rmatrixsolvemfast(const real_2d_array &a, const ae_int_t n, real_2d_array &b, const ae_int_t m, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::rmatrixsolvemfast(a.c_ptr(), n, b.c_ptr(), m, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Dense solver.

Similar to RMatrixSolve() but solves task with multiple right parts (where
b and x are NxM matrices). This is "fast" version of linear  solver  which
does NOT offer additional functions like condition  number  estimation  or
iterative refinement.

Algorithm features:
* O(N^3+M*N^2) complexity
* no additional functionality, highest performance

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True for well-conditioned matrix
    False for extremely badly conditioned or exactly singular problem

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool rmatrixsolvemfast(const real_2d_array &a, real_2d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'rmatrixsolvemfast': looks like one of arguments has wrong size");
    n = a.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::rmatrixsolvemfast(a.c_ptr(), n, b.c_ptr(), m, &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where A is NxN non-denegerate
real matrix given by its LU decomposition, x and b are real vectors.  This
is "slow-but-robust" version of the linear LU-based solver. Faster version
is RMatrixLUSolveFast() function.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! RMatrixLUSolveFast() function.

INPUT PARAMETERS
    LUA     -   array[N,N], LU decomposition, RMatrixLU result
    P       -   array[N], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[N], right part

OUTPUT PARAMETERS
    Rep     -   additional report, the following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros


  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixlusolve(const real_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const real_1d_array &b, real_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::rmatrixlusolve(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where A is NxN non-denegerate
real matrix given by its LU decomposition, x and b are real vectors.  This
is "slow-but-robust" version of the linear LU-based solver. Faster version
is RMatrixLUSolveFast() function.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! RMatrixLUSolveFast() function.

INPUT PARAMETERS
    LUA     -   array[N,N], LU decomposition, RMatrixLU result
    P       -   array[N], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[N], right part

OUTPUT PARAMETERS
    Rep     -   additional report, the following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros


  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void rmatrixlusolve(const real_2d_array &lua, const integer_1d_array &p, const real_1d_array &b, real_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (lua.rows()!=lua.cols()) || (lua.rows()!=p.length()) || (lua.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'rmatrixlusolve': looks like one of arguments has wrong size");
    n = lua.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::rmatrixlusolve(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where A is NxN non-denegerate
real matrix given by its LU decomposition, x and b are real vectors.  This
is "fast-without-any-checks" version of the linear LU-based solver. Slower
but more robust version is RMatrixLUSolve() function.

Algorithm features:
* O(N^2) complexity
* fast algorithm without ANY additional checks, just triangular solver

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system


  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
bool rmatrixlusolvefast(const real_2d_array &lua, const integer_1d_array &p, const ae_int_t n, real_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::rmatrixlusolvefast(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where A is NxN non-denegerate
real matrix given by its LU decomposition, x and b are real vectors.  This
is "fast-without-any-checks" version of the linear LU-based solver. Slower
but more robust version is RMatrixLUSolve() function.

Algorithm features:
* O(N^2) complexity
* fast algorithm without ANY additional checks, just triangular solver

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system


  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool rmatrixlusolvefast(const real_2d_array &lua, const integer_1d_array &p, real_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (lua.rows()!=lua.cols()) || (lua.rows()!=p.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'rmatrixlusolvefast': looks like one of arguments has wrong size");
    n = lua.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::rmatrixlusolvefast(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Dense solver.

Similar to RMatrixLUSolve() but solves  task  with  multiple  right  parts
(where b and x are NxM matrices). This  is  "robust-but-slow"  version  of
LU-based solver which performs additional  checks  for  non-degeneracy  of
inputs (condition number estimation). If you need  best  performance,  use
"fast-without-any-checks" version, RMatrixLUSolveMFast().

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver.
           !
           ! This performance penalty is especially apparent when you  use
           ! ALGLIB parallel capabilities (condition number estimation  is
           ! inherently  sequential).  It   also   becomes significant for
           ! small-scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! RMatrixLUSolveMFast() function.

INPUT PARAMETERS
    LUA     -   array[N,N], LU decomposition, RMatrixLU result
    P       -   array[N], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixlusolvem(const real_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const real_2d_array &b, const ae_int_t m, real_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::rmatrixlusolvem(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver.

Similar to RMatrixLUSolve() but solves  task  with  multiple  right  parts
(where b and x are NxM matrices). This  is  "robust-but-slow"  version  of
LU-based solver which performs additional  checks  for  non-degeneracy  of
inputs (condition number estimation). If you need  best  performance,  use
"fast-without-any-checks" version, RMatrixLUSolveMFast().

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver.
           !
           ! This performance penalty is especially apparent when you  use
           ! ALGLIB parallel capabilities (condition number estimation  is
           ! inherently  sequential).  It   also   becomes significant for
           ! small-scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! RMatrixLUSolveMFast() function.

INPUT PARAMETERS
    LUA     -   array[N,N], LU decomposition, RMatrixLU result
    P       -   array[N], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void rmatrixlusolvem(const real_2d_array &lua, const integer_1d_array &p, const real_2d_array &b, real_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (lua.rows()!=lua.cols()) || (lua.rows()!=p.length()) || (lua.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'rmatrixlusolvem': looks like one of arguments has wrong size");
    n = lua.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::rmatrixlusolvem(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver.

Similar to RMatrixLUSolve() but solves  task  with  multiple  right parts,
where b and x are NxM matrices.  This is "fast-without-any-checks" version
of LU-based solver. It does not estimate  condition number  of  a  system,
so it is extremely fast. If you need better detection  of  near-degenerate
cases, use RMatrixLUSolveM() function.

Algorithm features:
* O(M*N^2) complexity
* fast algorithm without ANY additional checks, just triangular solver

INPUT PARAMETERS:
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS:
    B       -   array[N,M]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
bool rmatrixlusolvemfast(const real_2d_array &lua, const integer_1d_array &p, const ae_int_t n, real_2d_array &b, const ae_int_t m, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::rmatrixlusolvemfast(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), m, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Dense solver.

Similar to RMatrixLUSolve() but solves  task  with  multiple  right parts,
where b and x are NxM matrices.  This is "fast-without-any-checks" version
of LU-based solver. It does not estimate  condition number  of  a  system,
so it is extremely fast. If you need better detection  of  near-degenerate
cases, use RMatrixLUSolveM() function.

Algorithm features:
* O(M*N^2) complexity
* fast algorithm without ANY additional checks, just triangular solver

INPUT PARAMETERS:
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS:
    B       -   array[N,M]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool rmatrixlusolvemfast(const real_2d_array &lua, const integer_1d_array &p, real_2d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (lua.rows()!=lua.cols()) || (lua.rows()!=p.length()) || (lua.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'rmatrixlusolvemfast': looks like one of arguments has wrong size");
    n = lua.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::rmatrixlusolvemfast(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), m, &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where BOTH ORIGINAL A AND ITS
LU DECOMPOSITION ARE KNOWN. You can use it if for some  reasons  you  have
both A and its LU decomposition.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixmixedsolve(const real_2d_array &a, const real_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const real_1d_array &b, real_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::rmatrixmixedsolve(a.c_ptr(), lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where BOTH ORIGINAL A AND ITS
LU DECOMPOSITION ARE KNOWN. You can use it if for some  reasons  you  have
both A and its LU decomposition.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void rmatrixmixedsolve(const real_2d_array &a, const real_2d_array &lua, const integer_1d_array &p, const real_1d_array &b, real_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (a.rows()!=a.cols()) || (a.rows()!=lua.rows()) || (a.rows()!=lua.cols()) || (a.rows()!=p.length()) || (a.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'rmatrixmixedsolve': looks like one of arguments has wrong size");
    n = a.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::rmatrixmixedsolve(a.c_ptr(), lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver.

Similar to RMatrixMixedSolve() but  solves task with multiple right  parts
(where b and x are NxM matrices).

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixmixedsolvem(const real_2d_array &a, const real_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const real_2d_array &b, const ae_int_t m, real_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::rmatrixmixedsolvem(a.c_ptr(), lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver.

Similar to RMatrixMixedSolve() but  solves task with multiple right  parts
(where b and x are NxM matrices).

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void rmatrixmixedsolvem(const real_2d_array &a, const real_2d_array &lua, const integer_1d_array &p, const real_2d_array &b, real_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (a.rows()!=a.cols()) || (a.rows()!=lua.rows()) || (a.rows()!=lua.cols()) || (a.rows()!=p.length()) || (a.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'rmatrixmixedsolvem': looks like one of arguments has wrong size");
    n = a.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::rmatrixmixedsolvem(a.c_ptr(), lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Complex dense solver for A*X=B with N*N  complex  matrix  A,  N*M  complex
matrices  X  and  B.  "Slow-but-feature-rich"   version   which   provides
additional functions, at the cost of slower  performance.  Faster  version
may be invoked with CMatrixSolveMFast() function.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3+M*N^2) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, CMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixsolvem(const complex_2d_array &a, const ae_int_t n, const complex_2d_array &b, const ae_int_t m, const bool rfs, complex_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::cmatrixsolvem(a.c_ptr(), n, b.c_ptr(), m, rfs, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Complex dense solver for A*X=B with N*N  complex  matrix  A,  N*M  complex
matrices  X  and  B.  "Slow-but-feature-rich"   version   which   provides
additional functions, at the cost of slower  performance.  Faster  version
may be invoked with CMatrixSolveMFast() function.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3+M*N^2) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, CMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void cmatrixsolvem(const complex_2d_array &a, const complex_2d_array &b, const bool rfs, complex_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'cmatrixsolvem': looks like one of arguments has wrong size");
    n = a.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::cmatrixsolvem(a.c_ptr(), n, b.c_ptr(), m, rfs, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Complex dense solver for A*X=B with N*N  complex  matrix  A,  N*M  complex
matrices  X  and  B.  "Fast-but-lightweight" version which  provides  just
triangular solver - and no additional functions like iterative  refinement
or condition number estimation.

Algorithm features:
* O(N^3+M*N^2) complexity
* no additional time consuming functions

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS:
    B       -   array[N,M]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 16.03.2015 by Bochkanov Sergey
*************************************************************************/
bool cmatrixsolvemfast(const complex_2d_array &a, const ae_int_t n, complex_2d_array &b, const ae_int_t m, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::cmatrixsolvemfast(a.c_ptr(), n, b.c_ptr(), m, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Complex dense solver for A*X=B with N*N  complex  matrix  A,  N*M  complex
matrices  X  and  B.  "Fast-but-lightweight" version which  provides  just
triangular solver - and no additional functions like iterative  refinement
or condition number estimation.

Algorithm features:
* O(N^3+M*N^2) complexity
* no additional time consuming functions

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS:
    B       -   array[N,M]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 16.03.2015 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool cmatrixsolvemfast(const complex_2d_array &a, complex_2d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'cmatrixsolvemfast': looks like one of arguments has wrong size");
    n = a.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::cmatrixsolvemfast(a.c_ptr(), n, b.c_ptr(), m, &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Complex dense solver for A*x=B with N*N complex matrix A and  N*1  complex
vectors x and b. "Slow-but-feature-rich" version of the solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, CMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixsolve(const complex_2d_array &a, const ae_int_t n, const complex_1d_array &b, complex_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::cmatrixsolve(a.c_ptr(), n, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Complex dense solver for A*x=B with N*N complex matrix A and  N*1  complex
vectors x and b. "Slow-but-feature-rich" version of the solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, CMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void cmatrixsolve(const complex_2d_array &a, const complex_1d_array &b, complex_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'cmatrixsolve': looks like one of arguments has wrong size");
    n = a.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::cmatrixsolve(a.c_ptr(), n, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Complex dense solver for A*x=B with N*N complex matrix A and  N*1  complex
vectors x and b. "Fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^3) complexity
* no additional time consuming features, just triangular solver

INPUT PARAMETERS:
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS:
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
bool cmatrixsolvefast(const complex_2d_array &a, const ae_int_t n, complex_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::cmatrixsolvefast(a.c_ptr(), n, b.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Complex dense solver for A*x=B with N*N complex matrix A and  N*1  complex
vectors x and b. "Fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^3) complexity
* no additional time consuming features, just triangular solver

INPUT PARAMETERS:
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS:
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool cmatrixsolvefast(const complex_2d_array &a, complex_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'cmatrixsolvefast': looks like one of arguments has wrong size");
    n = a.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::cmatrixsolvefast(a.c_ptr(), n, b.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Dense solver for A*X=B with N*N complex A given by its  LU  decomposition,
and N*M matrices X and B (multiple right sides).   "Slow-but-feature-rich"
version of the solver.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use CMatrixSolve or CMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver.
           !
           ! This performance penalty is especially apparent when you  use
           ! ALGLIB parallel capabilities (condition number estimation  is
           ! inherently  sequential).  It   also   becomes significant for
           ! small-scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! CMatrixLUSolveMFast() function.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixlusolvem(const complex_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const complex_2d_array &b, const ae_int_t m, complex_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::cmatrixlusolvem(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver for A*X=B with N*N complex A given by its  LU  decomposition,
and N*M matrices X and B (multiple right sides).   "Slow-but-feature-rich"
version of the solver.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use CMatrixSolve or CMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver.
           !
           ! This performance penalty is especially apparent when you  use
           ! ALGLIB parallel capabilities (condition number estimation  is
           ! inherently  sequential).  It   also   becomes significant for
           ! small-scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! CMatrixLUSolveMFast() function.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void cmatrixlusolvem(const complex_2d_array &lua, const integer_1d_array &p, const complex_2d_array &b, complex_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (lua.rows()!=lua.cols()) || (lua.rows()!=p.length()) || (lua.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'cmatrixlusolvem': looks like one of arguments has wrong size");
    n = lua.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::cmatrixlusolvem(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver for A*X=B with N*N complex A given by its  LU  decomposition,
and N*M matrices X and B (multiple  right  sides).  "Fast-but-lightweight"
version of the solver.

Algorithm features:
* O(M*N^2) complexity
* no additional time-consuming features

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[N,M]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
bool cmatrixlusolvemfast(const complex_2d_array &lua, const integer_1d_array &p, const ae_int_t n, complex_2d_array &b, const ae_int_t m, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::cmatrixlusolvemfast(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), m, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Dense solver for A*X=B with N*N complex A given by its  LU  decomposition,
and N*M matrices X and B (multiple  right  sides).  "Fast-but-lightweight"
version of the solver.

Algorithm features:
* O(M*N^2) complexity
* no additional time-consuming features

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[N,M]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool cmatrixlusolvemfast(const complex_2d_array &lua, const integer_1d_array &p, complex_2d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (lua.rows()!=lua.cols()) || (lua.rows()!=p.length()) || (lua.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'cmatrixlusolvemfast': looks like one of arguments has wrong size");
    n = lua.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::cmatrixlusolvemfast(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), m, &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Complex dense linear solver for A*x=b with complex N*N A  given  by its LU
decomposition and N*1 vectors x and b. This is  "slow-but-robust"  version
of  the  complex  linear  solver  with  additional  features   which   add
significant performance overhead. Faster version  is  CMatrixLUSolveFast()
function.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation

No iterative refinement is provided because exact form of original matrix
is not known to subroutine. Use CMatrixSolve or CMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! CMatrixLUSolveFast() function.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixlusolve(const complex_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const complex_1d_array &b, complex_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::cmatrixlusolve(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Complex dense linear solver for A*x=b with complex N*N A  given  by its LU
decomposition and N*1 vectors x and b. This is  "slow-but-robust"  version
of  the  complex  linear  solver  with  additional  features   which   add
significant performance overhead. Faster version  is  CMatrixLUSolveFast()
function.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation

No iterative refinement is provided because exact form of original matrix
is not known to subroutine. Use CMatrixSolve or CMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! CMatrixLUSolveFast() function.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void cmatrixlusolve(const complex_2d_array &lua, const integer_1d_array &p, const complex_1d_array &b, complex_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (lua.rows()!=lua.cols()) || (lua.rows()!=p.length()) || (lua.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'cmatrixlusolve': looks like one of arguments has wrong size");
    n = lua.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::cmatrixlusolve(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Complex dense linear solver for A*x=b with N*N complex A given by  its  LU
decomposition and N*1 vectors x and b. This is  fast  lightweight  version
of solver, which is significantly faster than CMatrixLUSolve(),  but  does
not provide additional information (like condition numbers).

Algorithm features:
* O(N^2) complexity
* no additional time-consuming features, just triangular solver

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

NOTE: unlike  CMatrixLUSolve(),  this   function   does   NOT   check  for
      near-degeneracy of input matrix. It  checks  for  EXACT  degeneracy,
      because this check is easy to do. However,  very  badly  conditioned
      matrices may went unnoticed.


  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
bool cmatrixlusolvefast(const complex_2d_array &lua, const integer_1d_array &p, const ae_int_t n, complex_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::cmatrixlusolvefast(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Complex dense linear solver for A*x=b with N*N complex A given by  its  LU
decomposition and N*1 vectors x and b. This is  fast  lightweight  version
of solver, which is significantly faster than CMatrixLUSolve(),  but  does
not provide additional information (like condition numbers).

Algorithm features:
* O(N^2) complexity
* no additional time-consuming features, just triangular solver

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

NOTE: unlike  CMatrixLUSolve(),  this   function   does   NOT   check  for
      near-degeneracy of input matrix. It  checks  for  EXACT  degeneracy,
      because this check is easy to do. However,  very  badly  conditioned
      matrices may went unnoticed.


  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool cmatrixlusolvefast(const complex_2d_array &lua, const integer_1d_array &p, complex_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (lua.rows()!=lua.cols()) || (lua.rows()!=p.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'cmatrixlusolvefast': looks like one of arguments has wrong size");
    n = lua.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::cmatrixlusolvefast(lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Dense solver. Same as RMatrixMixedSolveM(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixmixedsolvem(const complex_2d_array &a, const complex_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const complex_2d_array &b, const ae_int_t m, complex_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::cmatrixmixedsolvem(a.c_ptr(), lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver. Same as RMatrixMixedSolveM(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void cmatrixmixedsolvem(const complex_2d_array &a, const complex_2d_array &lua, const integer_1d_array &p, const complex_2d_array &b, complex_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (a.rows()!=a.cols()) || (a.rows()!=lua.rows()) || (a.rows()!=lua.cols()) || (a.rows()!=p.length()) || (a.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'cmatrixmixedsolvem': looks like one of arguments has wrong size");
    n = a.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::cmatrixmixedsolvem(a.c_ptr(), lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver. Same as RMatrixMixedSolve(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixmixedsolve(const complex_2d_array &a, const complex_2d_array &lua, const integer_1d_array &p, const ae_int_t n, const complex_1d_array &b, complex_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::cmatrixmixedsolve(a.c_ptr(), lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver. Same as RMatrixMixedSolve(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void cmatrixmixedsolve(const complex_2d_array &a, const complex_2d_array &lua, const integer_1d_array &p, const complex_1d_array &b, complex_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (a.rows()!=a.cols()) || (a.rows()!=lua.rows()) || (a.rows()!=lua.cols()) || (a.rows()!=p.length()) || (a.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'cmatrixmixedsolve': looks like one of arguments has wrong size");
    n = a.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::cmatrixmixedsolve(a.c_ptr(), lua.c_ptr(), p.c_ptr(), n, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A,  and
N*M vectors X and B. It is "slow-but-feature-rich" version of the solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant   performance   penalty  when
           ! compared with "fast" version  which  just  performs  Cholesky
           ! decomposition and calls triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, SPDMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixsolvem(const real_2d_array &a, const ae_int_t n, const bool isupper, const real_2d_array &b, const ae_int_t m, real_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::spdmatrixsolvem(a.c_ptr(), n, isupper, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A,  and
N*M vectors X and B. It is "slow-but-feature-rich" version of the solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant   performance   penalty  when
           ! compared with "fast" version  which  just  performs  Cholesky
           ! decomposition and calls triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, SPDMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void spdmatrixsolvem(const real_2d_array &a, const bool isupper, const real_2d_array &b, real_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'spdmatrixsolvem': looks like one of arguments has wrong size");
    n = a.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::spdmatrixsolvem(a.c_ptr(), n, isupper, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A,  and
N*M vectors X and B. It is "fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming features

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[N,M], it contains:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
bool spdmatrixsolvemfast(const real_2d_array &a, const ae_int_t n, const bool isupper, real_2d_array &b, const ae_int_t m, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::spdmatrixsolvemfast(a.c_ptr(), n, isupper, b.c_ptr(), m, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A,  and
N*M vectors X and B. It is "fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming features

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[N,M], it contains:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool spdmatrixsolvemfast(const real_2d_array &a, const bool isupper, real_2d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'spdmatrixsolvemfast': looks like one of arguments has wrong size");
    n = a.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::spdmatrixsolvemfast(a.c_ptr(), n, isupper, b.c_ptr(), m, &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Dense linear solver for A*x=b with N*N real  symmetric  positive  definite
matrix A,  N*1 vectors x and b.  "Slow-but-feature-rich"  version  of  the
solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant   performance   penalty  when
           ! compared with "fast" version  which  just  performs  Cholesky
           ! decomposition and calls triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, SPDMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixsolve(const real_2d_array &a, const ae_int_t n, const bool isupper, const real_1d_array &b, real_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::spdmatrixsolve(a.c_ptr(), n, isupper, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense linear solver for A*x=b with N*N real  symmetric  positive  definite
matrix A,  N*1 vectors x and b.  "Slow-but-feature-rich"  version  of  the
solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant   performance   penalty  when
           ! compared with "fast" version  which  just  performs  Cholesky
           ! decomposition and calls triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, SPDMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void spdmatrixsolve(const real_2d_array &a, const bool isupper, const real_1d_array &b, real_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'spdmatrixsolve': looks like one of arguments has wrong size");
    n = a.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::spdmatrixsolve(a.c_ptr(), n, isupper, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense linear solver for A*x=b with N*N real  symmetric  positive  definite
matrix A,  N*1 vectors x and  b.  "Fast-but-lightweight"  version  of  the
solver.

Algorithm features:
* O(N^3) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming features like condition number estimation

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N], it contains:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
bool spdmatrixsolvefast(const real_2d_array &a, const ae_int_t n, const bool isupper, real_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::spdmatrixsolvefast(a.c_ptr(), n, isupper, b.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Dense linear solver for A*x=b with N*N real  symmetric  positive  definite
matrix A,  N*1 vectors x and  b.  "Fast-but-lightweight"  version  of  the
solver.

Algorithm features:
* O(N^3) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming features like condition number estimation

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N], it contains:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool spdmatrixsolvefast(const real_2d_array &a, const bool isupper, real_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'spdmatrixsolvefast': looks like one of arguments has wrong size");
    n = a.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::spdmatrixsolvefast(a.c_ptr(), n, isupper, b.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*M vectors X and B. It  is  "slow-but-
feature-rich" version of the solver which estimates  condition  number  of
the system.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver. Amount of  overhead  introduced  depends  on  M  (the
           ! larger - the more efficient).
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! SPDMatrixCholeskySolveMFast() function.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N]:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixcholeskysolvem(const real_2d_array &cha, const ae_int_t n, const bool isupper, const real_2d_array &b, const ae_int_t m, real_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::spdmatrixcholeskysolvem(cha.c_ptr(), n, isupper, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*M vectors X and B. It  is  "slow-but-
feature-rich" version of the solver which estimates  condition  number  of
the system.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver. Amount of  overhead  introduced  depends  on  M  (the
           ! larger - the more efficient).
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! SPDMatrixCholeskySolveMFast() function.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N]:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void spdmatrixcholeskysolvem(const real_2d_array &cha, const bool isupper, const real_2d_array &b, real_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (cha.rows()!=cha.cols()) || (cha.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'spdmatrixcholeskysolvem': looks like one of arguments has wrong size");
    n = cha.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::spdmatrixcholeskysolvem(cha.c_ptr(), n, isupper, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*M vectors X and B. It  is  "fast-but-
lightweight" version of  the  solver  which  just  solves  linear  system,
without any additional functions.

Algorithm features:
* O(M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional functionality

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[N,M], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
bool spdmatrixcholeskysolvemfast(const real_2d_array &cha, const ae_int_t n, const bool isupper, real_2d_array &b, const ae_int_t m, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::spdmatrixcholeskysolvemfast(cha.c_ptr(), n, isupper, b.c_ptr(), m, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*M vectors X and B. It  is  "fast-but-
lightweight" version of  the  solver  which  just  solves  linear  system,
without any additional functions.

Algorithm features:
* O(M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional functionality

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[N,M], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool spdmatrixcholeskysolvemfast(const real_2d_array &cha, const bool isupper, real_2d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (cha.rows()!=cha.cols()) || (cha.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'spdmatrixcholeskysolvemfast': looks like one of arguments has wrong size");
    n = cha.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::spdmatrixcholeskysolvemfast(cha.c_ptr(), n, isupper, b.c_ptr(), m, &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Dense solver for A*x=b with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*1 real vectors x and b. This is "slow-
but-feature-rich"  version  of  the  solver  which,  in  addition  to  the
solution, performs condition number estimation.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! SPDMatrixCholeskySolveFast() function.

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[N], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N]:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixcholeskysolve(const real_2d_array &cha, const ae_int_t n, const bool isupper, const real_1d_array &b, real_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::spdmatrixcholeskysolve(cha.c_ptr(), n, isupper, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver for A*x=b with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*1 real vectors x and b. This is "slow-
but-feature-rich"  version  of  the  solver  which,  in  addition  to  the
solution, performs condition number estimation.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! SPDMatrixCholeskySolveFast() function.

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[N], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N]:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void spdmatrixcholeskysolve(const real_2d_array &cha, const bool isupper, const real_1d_array &b, real_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (cha.rows()!=cha.cols()) || (cha.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'spdmatrixcholeskysolve': looks like one of arguments has wrong size");
    n = cha.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::spdmatrixcholeskysolve(cha.c_ptr(), n, isupper, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver for A*x=b with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*1 real vectors x and b. This is "fast-
but-lightweight" version of the solver.

Algorithm features:
* O(N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional features

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[N], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
bool spdmatrixcholeskysolvefast(const real_2d_array &cha, const ae_int_t n, const bool isupper, real_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::spdmatrixcholeskysolvefast(cha.c_ptr(), n, isupper, b.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Dense solver for A*x=b with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*1 real vectors x and b. This is "fast-
but-lightweight" version of the solver.

Algorithm features:
* O(N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional features

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[N], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool spdmatrixcholeskysolvefast(const real_2d_array &cha, const bool isupper, real_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (cha.rows()!=cha.cols()) || (cha.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'spdmatrixcholeskysolvefast': looks like one of arguments has wrong size");
    n = cha.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::spdmatrixcholeskysolvefast(cha.c_ptr(), n, isupper, b.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Dense solver for A*X=B, with N*N Hermitian positive definite matrix A  and
N*M  complex  matrices  X  and  B.  "Slow-but-feature-rich" version of the
solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver.
           !
           ! This performance penalty is especially apparent when you  use
           ! ALGLIB parallel capabilities (condition number estimation  is
           ! inherently  sequential).  It   also   becomes significant for
           ! small-scale problems (N<100).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! HPDMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixsolvem(const complex_2d_array &a, const ae_int_t n, const bool isupper, const complex_2d_array &b, const ae_int_t m, complex_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::hpdmatrixsolvem(a.c_ptr(), n, isupper, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver for A*X=B, with N*N Hermitian positive definite matrix A  and
N*M  complex  matrices  X  and  B.  "Slow-but-feature-rich" version of the
solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver.
           !
           ! This performance penalty is especially apparent when you  use
           ! ALGLIB parallel capabilities (condition number estimation  is
           ! inherently  sequential).  It   also   becomes significant for
           ! small-scale problems (N<100).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! HPDMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void hpdmatrixsolvem(const complex_2d_array &a, const bool isupper, const complex_2d_array &b, complex_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'hpdmatrixsolvem': looks like one of arguments has wrong size");
    n = a.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::hpdmatrixsolvem(a.c_ptr(), n, isupper, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver for A*X=B, with N*N Hermitian positive definite matrix A  and
N*M complex matrices X and B. "Fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming features like condition number estimation

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[0..N-1]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
bool hpdmatrixsolvemfast(const complex_2d_array &a, const ae_int_t n, const bool isupper, complex_2d_array &b, const ae_int_t m, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::hpdmatrixsolvemfast(a.c_ptr(), n, isupper, b.c_ptr(), m, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Dense solver for A*X=B, with N*N Hermitian positive definite matrix A  and
N*M complex matrices X and B. "Fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming features like condition number estimation

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[0..N-1]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool hpdmatrixsolvemfast(const complex_2d_array &a, const bool isupper, complex_2d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'hpdmatrixsolvemfast': looks like one of arguments has wrong size");
    n = a.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::hpdmatrixsolvemfast(a.c_ptr(), n, isupper, b.c_ptr(), m, &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Dense solver for A*x=b, with N*N Hermitian positive definite matrix A, and
N*1 complex vectors  x  and  b.  "Slow-but-feature-rich"  version  of  the
solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant   performance   penalty  when
           ! compared with "fast" version  which  just  performs  Cholesky
           ! decomposition and calls triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, HPDMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixsolve(const complex_2d_array &a, const ae_int_t n, const bool isupper, const complex_1d_array &b, complex_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::hpdmatrixsolve(a.c_ptr(), n, isupper, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver for A*x=b, with N*N Hermitian positive definite matrix A, and
N*1 complex vectors  x  and  b.  "Slow-but-feature-rich"  version  of  the
solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant   performance   penalty  when
           ! compared with "fast" version  which  just  performs  Cholesky
           ! decomposition and calls triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, HPDMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void hpdmatrixsolve(const complex_2d_array &a, const bool isupper, const complex_1d_array &b, complex_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'hpdmatrixsolve': looks like one of arguments has wrong size");
    n = a.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::hpdmatrixsolve(a.c_ptr(), n, isupper, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver for A*x=b, with N*N Hermitian positive definite matrix A, and
N*1 complex vectors  x  and  b.  "Fast-but-lightweight"  version  of   the
solver without additional functions.

Algorithm features:
* O(N^3) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming functions

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[0..N-1]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
bool hpdmatrixsolvefast(const complex_2d_array &a, const ae_int_t n, const bool isupper, complex_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::hpdmatrixsolvefast(a.c_ptr(), n, isupper, b.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Dense solver for A*x=b, with N*N Hermitian positive definite matrix A, and
N*1 complex vectors  x  and  b.  "Fast-but-lightweight"  version  of   the
solver without additional functions.

Algorithm features:
* O(N^3) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming functions

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[0..N-1]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool hpdmatrixsolvefast(const complex_2d_array &a, const bool isupper, complex_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (a.rows()!=a.cols()) || (a.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'hpdmatrixsolvefast': looks like one of arguments has wrong size");
    n = a.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::hpdmatrixsolvefast(a.c_ptr(), n, isupper, b.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Dense solver for A*X=B with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition and N*M complex matrices X  and  B.  This is
"slow-but-feature-rich" version of the solver which, in  addition  to  the
solution, estimates condition number of the system.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver. Amount of  overhead  introduced  depends  on  M  (the
           ! larger - the more efficient).
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large Cholesky decomposition.  However,  if  you call
           ! this  function  many  times  for  the same  left  side,  this
           ! overhead BECOMES significant. It  also   becomes  significant
           ! for small-scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! HPDMatrixCholeskySolveMFast() function.


INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                HPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[N,M], right part
    M       -   right part size

OUTPUT PARAMETERS:
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixcholeskysolvem(const complex_2d_array &cha, const ae_int_t n, const bool isupper, const complex_2d_array &b, const ae_int_t m, complex_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::hpdmatrixcholeskysolvem(cha.c_ptr(), n, isupper, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver for A*X=B with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition and N*M complex matrices X  and  B.  This is
"slow-but-feature-rich" version of the solver which, in  addition  to  the
solution, estimates condition number of the system.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver. Amount of  overhead  introduced  depends  on  M  (the
           ! larger - the more efficient).
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large Cholesky decomposition.  However,  if  you call
           ! this  function  many  times  for  the same  left  side,  this
           ! overhead BECOMES significant. It  also   becomes  significant
           ! for small-scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! HPDMatrixCholeskySolveMFast() function.


INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                HPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[N,M], right part
    M       -   right part size

OUTPUT PARAMETERS:
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void hpdmatrixcholeskysolvem(const complex_2d_array &cha, const bool isupper, const complex_2d_array &b, complex_2d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (cha.rows()!=cha.cols()) || (cha.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'hpdmatrixcholeskysolvem': looks like one of arguments has wrong size");
    n = cha.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::hpdmatrixcholeskysolvem(cha.c_ptr(), n, isupper, b.c_ptr(), m, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver for A*X=B with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition and N*M complex matrices X  and  B.  This is
"fast-but-lightweight" version of the solver.

Algorithm features:
* O(M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time-consuming features

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                HPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[N,M], right part
    M       -   right part size

OUTPUT PARAMETERS:
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system

  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
bool hpdmatrixcholeskysolvemfast(const complex_2d_array &cha, const ae_int_t n, const bool isupper, complex_2d_array &b, const ae_int_t m, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::hpdmatrixcholeskysolvemfast(cha.c_ptr(), n, isupper, b.c_ptr(), m, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Dense solver for A*X=B with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition and N*M complex matrices X  and  B.  This is
"fast-but-lightweight" version of the solver.

Algorithm features:
* O(M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time-consuming features

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                HPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[N,M], right part
    M       -   right part size

OUTPUT PARAMETERS:
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system

  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool hpdmatrixcholeskysolvemfast(const complex_2d_array &cha, const bool isupper, complex_2d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    ae_int_t m;
    if( (cha.rows()!=cha.cols()) || (cha.rows()!=b.rows()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'hpdmatrixcholeskysolvemfast': looks like one of arguments has wrong size");
    n = cha.rows();
    m = b.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::hpdmatrixcholeskysolvemfast(cha.c_ptr(), n, isupper, b.c_ptr(), m, &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Dense solver for A*x=b with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition, and N*1 complex vectors x and  b.  This  is
"slow-but-feature-rich" version of the solver  which  estimates  condition
number of the system.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! HPDMatrixCholeskySolveFast() function.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixcholeskysolve(const complex_2d_array &cha, const ae_int_t n, const bool isupper, const complex_1d_array &b, complex_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::hpdmatrixcholeskysolve(cha.c_ptr(), n, isupper, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver for A*x=b with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition, and N*1 complex vectors x and  b.  This  is
"slow-but-feature-rich" version of the solver  which  estimates  condition
number of the system.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! HPDMatrixCholeskySolveFast() function.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void hpdmatrixcholeskysolve(const complex_2d_array &cha, const bool isupper, const complex_1d_array &b, complex_1d_array &x, densesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (cha.rows()!=cha.cols()) || (cha.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'hpdmatrixcholeskysolve': looks like one of arguments has wrong size");
    n = cha.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::hpdmatrixcholeskysolve(cha.c_ptr(), n, isupper, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
Dense solver for A*x=b with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition, and N*1 complex vectors x and  b.  This  is
"fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time-consuming features

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system

  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
bool hpdmatrixcholeskysolvefast(const complex_2d_array &cha, const ae_int_t n, const bool isupper, complex_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::hpdmatrixcholeskysolvefast(cha.c_ptr(), n, isupper, b.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
Dense solver for A*x=b with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition, and N*1 complex vectors x and  b.  This  is
"fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time-consuming features

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system

  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
bool hpdmatrixcholeskysolvefast(const complex_2d_array &cha, const bool isupper, complex_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;
    if( (cha.rows()!=cha.cols()) || (cha.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'hpdmatrixcholeskysolvefast': looks like one of arguments has wrong size");
    n = cha.rows();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::hpdmatrixcholeskysolvefast(cha.c_ptr(), n, isupper, b.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return *(reinterpret_cast<bool*>(&result));
}
#endif

/*************************************************************************
Dense solver.

This subroutine finds solution of the linear system A*X=B with non-square,
possibly degenerate A.  System  is  solved in the least squares sense, and
general least squares solution  X = X0 + CX*y  which  minimizes |A*X-B| is
returned. If A is non-degenerate, solution in the usual sense is returned.

Algorithm features:
* automatic detection (and correct handling!) of degenerate cases
* iterative refinement
* O(N^3) complexity

INPUT PARAMETERS
    A       -   array[0..NRows-1,0..NCols-1], system matrix
    NRows   -   vertical size of A
    NCols   -   horizontal size of A
    B       -   array[0..NCols-1], right part
    Threshold-  a number in [0,1]. Singular values  beyond  Threshold*Largest are
                considered  zero.  Set  it to 0.0, if you don't understand
                what it means, so the solver will choose good value on its
                own.

OUTPUT PARAMETERS
    Rep     -   solver report, see below for more info
    X       -   array[0..N-1,0..M-1], it contains:
                * solution of A*X=B (even for singular A)
                * zeros, if SVD subroutine failed

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* TerminationType is set to:
            * -4 for SVD failure
            * >0 for success
* R2        reciprocal of condition number: 1/cond(A), 2-norm.
* N         = NCols
* K         dim(Null(A))
* CX        array[0..N-1,0..K-1], kernel of A.
            Columns of CX store such vectors that A*CX[i]=0.

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 24.08.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolvels(const real_2d_array &a, const ae_int_t nrows, const ae_int_t ncols, const real_1d_array &b, const double threshold, real_1d_array &x, densesolverlsreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::rmatrixsolvels(a.c_ptr(), nrows, ncols, b.c_ptr(), threshold, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Dense solver.

This subroutine finds solution of the linear system A*X=B with non-square,
possibly degenerate A.  System  is  solved in the least squares sense, and
general least squares solution  X = X0 + CX*y  which  minimizes |A*X-B| is
returned. If A is non-degenerate, solution in the usual sense is returned.

Algorithm features:
* automatic detection (and correct handling!) of degenerate cases
* iterative refinement
* O(N^3) complexity

INPUT PARAMETERS
    A       -   array[0..NRows-1,0..NCols-1], system matrix
    NRows   -   vertical size of A
    NCols   -   horizontal size of A
    B       -   array[0..NCols-1], right part
    Threshold-  a number in [0,1]. Singular values  beyond  Threshold*Largest are
                considered  zero.  Set  it to 0.0, if you don't understand
                what it means, so the solver will choose good value on its
                own.

OUTPUT PARAMETERS
    Rep     -   solver report, see below for more info
    X       -   array[0..N-1,0..M-1], it contains:
                * solution of A*X=B (even for singular A)
                * zeros, if SVD subroutine failed

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* TerminationType is set to:
            * -4 for SVD failure
            * >0 for success
* R2        reciprocal of condition number: 1/cond(A), 2-norm.
* N         = NCols
* K         dim(Null(A))
* CX        array[0..N-1,0..K-1], kernel of A.
            Columns of CX store such vectors that A*CX[i]=0.

  ! FREE EDITION OF ALGLIB:
  !
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  !
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  !
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 24.08.2009 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void rmatrixsolvels(const real_2d_array &a, const real_1d_array &b, const double threshold, real_1d_array &x, densesolverlsreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t nrows;
    ae_int_t ncols;
    if( (a.rows()!=b.length()))
        _ALGLIB_CPP_EXCEPTION("Error while calling 'rmatrixsolvels': looks like one of arguments has wrong size");
    nrows = a.rows();
    ncols = a.cols();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::rmatrixsolvels(a.c_ptr(), nrows, ncols, b.c_ptr(), threshold, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif
#endif

#if defined(AE_COMPILE_DIRECTSPARSESOLVERS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This structure is a sparse solver report (both direct and iterative solvers
use this structure).

Following fields can be accessed by users:
* TerminationType (specific error codes depend on the solver  being  used,
  with positive values ALWAYS signaling  that something useful is returned
  in X, and negative values ALWAYS meaning critical failures.
* NMV - number of matrix-vector products performed (0 for direct solvers)
* IterationsCount - inner iterations count (0 for direct solvers)
* R2 - squared residual
*************************************************************************/
_sparsesolverreport_owner::_sparsesolverreport_owner()
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_sparsesolverreport_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    p_struct = (alglib_impl::sparsesolverreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::sparsesolverreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::sparsesolverreport));
    alglib_impl::_sparsesolverreport_init(p_struct, &_state, ae_false);
    ae_state_clear(&_state);
}

_sparsesolverreport_owner::_sparsesolverreport_owner(const _sparsesolverreport_owner &rhs)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_sparsesolverreport_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: sparsesolverreport copy constructor failure (source is not initialized)", &_state);
    p_struct = (alglib_impl::sparsesolverreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::sparsesolverreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::sparsesolverreport));
    alglib_impl::_sparsesolverreport_init_copy(p_struct, const_cast<alglib_impl::sparsesolverreport*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
}

_sparsesolverreport_owner& _sparsesolverreport_owner::operator=(const _sparsesolverreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return *this;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    alglib_impl::ae_assert(p_struct!=NULL, "ALGLIB: sparsesolverreport assignment constructor failure (destination is not initialized)", &_state);
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: sparsesolverreport assignment constructor failure (source is not initialized)", &_state);
    alglib_impl::_sparsesolverreport_destroy(p_struct);
    memset(p_struct, 0, sizeof(alglib_impl::sparsesolverreport));
    alglib_impl::_sparsesolverreport_init_copy(p_struct, const_cast<alglib_impl::sparsesolverreport*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
    return *this;
}

_sparsesolverreport_owner::~_sparsesolverreport_owner()
{
    if( p_struct!=NULL )
    {
        alglib_impl::_sparsesolverreport_destroy(p_struct);
        ae_free(p_struct);
    }
}

alglib_impl::sparsesolverreport* _sparsesolverreport_owner::c_ptr()
{
    return p_struct;
}

const alglib_impl::sparsesolverreport* _sparsesolverreport_owner::c_ptr() const
{
    return p_struct;
}
sparsesolverreport::sparsesolverreport() : _sparsesolverreport_owner() ,terminationtype(p_struct->terminationtype),nmv(p_struct->nmv),iterationscount(p_struct->iterationscount),r2(p_struct->r2)
{
}

sparsesolverreport::sparsesolverreport(const sparsesolverreport &rhs):_sparsesolverreport_owner(rhs) ,terminationtype(p_struct->terminationtype),nmv(p_struct->nmv),iterationscount(p_struct->iterationscount),r2(p_struct->r2)
{
}

sparsesolverreport& sparsesolverreport::operator=(const sparsesolverreport &rhs)
{
    if( this==&rhs )
        return *this;
    _sparsesolverreport_owner::operator=(rhs);
    return *this;
}

sparsesolverreport::~sparsesolverreport()
{
}

/*************************************************************************
Sparse linear solver for A*x=b with N*N  sparse  real  symmetric  positive
definite matrix A, N*1 vectors x and b.

This solver  converts  input  matrix  to  SKS  format,  performs  Cholesky
factorization using  SKS  Cholesky  subroutine  (works  well  for  limited
bandwidth matrices) and uses sparse triangular solvers to get solution  of
the original system.

INPUT PARAMETERS
    A       -   sparse matrix, must be NxN exactly
    IsUpper -   which half of A is provided (another half is ignored)
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate or non-SPD system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsespdsolvesks(const sparsematrix &a, const bool isupper, const real_1d_array &b, real_1d_array &x, sparsesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsespdsolvesks(a.c_ptr(), isupper, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Sparse linear solver for A*x=b with N*N  sparse  real  symmetric  positive
definite matrix A, N*1 vectors x and b.

This solver  converts  input  matrix  to  CRS  format,  performs  Cholesky
factorization using supernodal Cholesky  decomposition  with  permutation-
reducing ordering and uses sparse triangular solver to get solution of the
original system.

INPUT PARAMETERS
    A       -   sparse matrix, must be NxN exactly
    IsUpper -   which half of A is provided (another half is ignored)
    B       -   array[N], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate or non-SPD system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsespdsolve(const sparsematrix &a, const bool isupper, const real_1d_array &b, real_1d_array &x, sparsesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsespdsolve(a.c_ptr(), isupper, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Sparse linear solver for A*x=b with N*N real  symmetric  positive definite
matrix A given by its Cholesky decomposition, and N*1 vectors x and b.

IMPORTANT: this solver requires input matrix to be in  the  SKS  (Skyline)
           or CRS (compressed row storage) format. An  exception  will  be
           generated if you pass matrix in some other format.

INPUT PARAMETERS
    A       -   sparse NxN matrix stored in CRs or SKS format, must be NxN
                exactly
    IsUpper -   which half of A is provided (another half is ignored)
    B       -   array[N], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate or non-SPD system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsespdcholeskysolve(const sparsematrix &a, const bool isupper, const real_1d_array &b, real_1d_array &x, sparsesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsespdcholeskysolve(a.c_ptr(), isupper, b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Sparse linear solver for A*x=b with general (nonsymmetric) N*N sparse real
matrix A, N*1 vectors x and b.

This solver converts input matrix to CRS format, performs LU factorization
and uses sparse triangular solvers to get solution of the original system.

INPUT PARAMETERS
    A       -   sparse matrix, must be NxN exactly, any storage format
    N       -   size of A, N>0
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsesolve(const sparsematrix &a, const real_1d_array &b, real_1d_array &x, sparsesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolve(a.c_ptr(), b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Sparse linear solver for A*x=b with general (nonsymmetric) N*N sparse real
matrix A given by its LU factorization, N*1 vectors x and b.

IMPORTANT: this solver requires input matrix  to  be  in  the  CRS  sparse
           storage format. An exception will  be  generated  if  you  pass
           matrix in some other format (HASH or SKS).

INPUT PARAMETERS
    A       -   LU factorization of the sparse matrix, must be NxN exactly
                in CRS storage format
    P, Q    -   pivot indexes from LU factorization
    N       -   size of A, N>0
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparselusolve(const sparsematrix &a, const integer_1d_array &p, const integer_1d_array &q, const real_1d_array &b, real_1d_array &x, sparsesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparselusolve(a.c_ptr(), p.c_ptr(), q.c_ptr(), b.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

#if defined(AE_COMPILE_ITERATIVESPARSE) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This object stores state of the sparse linear solver object.

You should use ALGLIB functions to work with this object.
Never try to access its fields directly!
*************************************************************************/
_sparsesolverstate_owner::_sparsesolverstate_owner()
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_sparsesolverstate_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    p_struct = (alglib_impl::sparsesolverstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::sparsesolverstate), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::sparsesolverstate));
    alglib_impl::_sparsesolverstate_init(p_struct, &_state, ae_false);
    ae_state_clear(&_state);
}

_sparsesolverstate_owner::_sparsesolverstate_owner(const _sparsesolverstate_owner &rhs)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_sparsesolverstate_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: sparsesolverstate copy constructor failure (source is not initialized)", &_state);
    p_struct = (alglib_impl::sparsesolverstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::sparsesolverstate), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::sparsesolverstate));
    alglib_impl::_sparsesolverstate_init_copy(p_struct, const_cast<alglib_impl::sparsesolverstate*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
}

_sparsesolverstate_owner& _sparsesolverstate_owner::operator=(const _sparsesolverstate_owner &rhs)
{
    if( this==&rhs )
        return *this;
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return *this;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    alglib_impl::ae_assert(p_struct!=NULL, "ALGLIB: sparsesolverstate assignment constructor failure (destination is not initialized)", &_state);
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: sparsesolverstate assignment constructor failure (source is not initialized)", &_state);
    alglib_impl::_sparsesolverstate_destroy(p_struct);
    memset(p_struct, 0, sizeof(alglib_impl::sparsesolverstate));
    alglib_impl::_sparsesolverstate_init_copy(p_struct, const_cast<alglib_impl::sparsesolverstate*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
    return *this;
}

_sparsesolverstate_owner::~_sparsesolverstate_owner()
{
    if( p_struct!=NULL )
    {
        alglib_impl::_sparsesolverstate_destroy(p_struct);
        ae_free(p_struct);
    }
}

alglib_impl::sparsesolverstate* _sparsesolverstate_owner::c_ptr()
{
    return p_struct;
}

const alglib_impl::sparsesolverstate* _sparsesolverstate_owner::c_ptr() const
{
    return p_struct;
}
sparsesolverstate::sparsesolverstate() : _sparsesolverstate_owner() 
{
}

sparsesolverstate::sparsesolverstate(const sparsesolverstate &rhs):_sparsesolverstate_owner(rhs) 
{
}

sparsesolverstate& sparsesolverstate::operator=(const sparsesolverstate &rhs)
{
    if( this==&rhs )
        return *this;
    _sparsesolverstate_owner::operator=(rhs);
    return *this;
}

sparsesolverstate::~sparsesolverstate()
{
}

/*************************************************************************
Solving sparse symmetric linear system A*x=b using GMRES(k) method. Sparse
symmetric A is given by its lower or upper triangle.

NOTE: use SparseSolveGMRES() to solve system with nonsymmetric A.

This function provides convenience API for an 'expert' interface  provided
by SparseSolverState class. Use SparseSolver  API  if  you  need  advanced
functions like providing initial point, using out-of-core API and so on.

INPUT PARAMETERS:
    A       -   sparse symmetric NxN matrix in any sparse storage  format.
                Using CRS format is recommended because it avoids internal
                conversion.
                An exception will be generated if  A  is  not  NxN  matrix
                (where  N  is  a  size   specified  during  solver  object
                creation).
    IsUpper -   whether upper or lower triangle of A is used:
                * IsUpper=True  => only upper triangle is used and lower
                                   triangle is not referenced at all
                * IsUpper=False => only lower triangle is used and upper
                                   triangle is not referenced at all
    B       -   right part, array[N]
    K       -   k parameter for  GMRES(k), k>=0.  Zero  value  means  that
                algorithm will choose it automatically.
    EpsF    -   stopping condition, EpsF>=0. The algorithm will stop  when
                residual will decrease below EpsF*|B|. Having EpsF=0 means
                that this stopping condition is ignored.
    MaxIts  -   stopping condition, MaxIts>=0.  The  algorithm  will  stop
                after performing MaxIts iterations. Zero  value  means  no
                limit.

NOTE: having both EpsF=0 and MaxIts=0 means that stopping criteria will be
      chosen automatically.

OUTPUT PARAMETERS:
    X       -   array[N], the solution
    Rep     -   solution report:
                * Rep.TerminationType completion code:
                    * -5    CG method was used for a matrix which  is  not
                            positive definite
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                    *  8    the  algorithm  was  terminated   early  with
                            SparseSolverRequestTermination() being called
                            from other thread.
                * Rep.IterationsCount contains iterations count
                * Rep.NMV contains number of matrix-vector calculations
                * Rep.R2 contains squared residual

  -- ALGLIB --
     Copyright 25.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolvesymmetricgmres(const sparsematrix &a, const bool isupper, const real_1d_array &b, const ae_int_t k, const double epsf, const ae_int_t maxits, real_1d_array &x, sparsesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolvesymmetricgmres(a.c_ptr(), isupper, b.c_ptr(), k, epsf, maxits, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Solving sparse linear system A*x=b using GMRES(k) method.

This function provides convenience API for an 'expert' interface  provided
by SparseSolverState class. Use SparseSolver  API  if  you  need  advanced
functions like providing initial point, using out-of-core API and so on.

INPUT PARAMETERS:
    A       -   sparse NxN matrix in any sparse storage format. Using  CRS
                format   is   recommended   because   it  avoids  internal
                conversion.
                An exception will be generated if  A  is  not  NxN  matrix
                (where  N  is  a  size   specified  during  solver  object
                creation).
    B       -   right part, array[N]
    K       -   k parameter for  GMRES(k), k>=0.  Zero  value  means  that
                algorithm will choose it automatically.
    EpsF    -   stopping condition, EpsF>=0. The algorithm will stop  when
                residual will decrease below EpsF*|B|. Having EpsF=0 means
                that this stopping condition is ignored.
    MaxIts  -   stopping condition, MaxIts>=0.  The  algorithm  will  stop
                after performing MaxIts iterations. Zero  value  means  no
                limit.

NOTE: having both EpsF=0 and MaxIts=0 means that stopping criteria will be
      chosen automatically.

OUTPUT PARAMETERS:
    X       -   array[N], the solution
    Rep     -   solution report:
                * Rep.TerminationType completion code:
                    * -5    CG method was used for a matrix which  is  not
                            positive definite
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                    *  8    the  algorithm  was  terminated   early  with
                            SparseSolverRequestTermination() being called
                            from other thread.
                * Rep.IterationsCount contains iterations count
                * Rep.NMV contains number of matrix-vector calculations
                * Rep.R2 contains squared residual

  -- ALGLIB --
     Copyright 25.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolvegmres(const sparsematrix &a, const real_1d_array &b, const ae_int_t k, const double epsf, const ae_int_t maxits, real_1d_array &x, sparsesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolvegmres(a.c_ptr(), b.c_ptr(), k, epsf, maxits, x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function initializes sparse linear iterative solver object.

This solver can be used  to  solve  nonsymmetric  and  symmetric  positive
definite NxN (square) linear systems.

The solver provides  'expert'  API  which  allows  advanced  control  over
algorithms being used, including ability to get progress report, terminate
long-running solver from other thread, out-of-core solution and so on.

NOTE: there are also convenience  functions  that  allows  quick  one-line
      access to the solvers:
      * SparseSolveCG() to solve SPD linear systems
      * SparseSolveGMRES() to solve unsymmetric linear systems.

NOTE: if you want to solve MxN (rectangular) linear problem  you  may  use
      LinLSQR solver provided by ALGLIB.

USAGE (A is given by the SparseMatrix structure):

    1. User initializes algorithm state with SparseSolverCreate() call
    2. User  selects   algorithm  with one of the SparseSolverSetAlgo???()
       functions. By default, GMRES(k) is used with automatically chosen k
    3. Optionally, user tunes solver parameters, sets starting point, etc.
    4. Depending on whether system is symmetric or not, user calls:
       * SparseSolverSolveSymmetric() for a  symmetric system given by its
         lower or upper triangle
       * SparseSolverSolve() for a nonsymmetric system or a symmetric  one
         given by the full matrix
    5. User calls SparseSolverResults() to get the solution

    It is possible to call SparseSolverSolve???() again to  solve  another
    task with same dimensionality but different matrix and/or  right  part
    without reinitializing SparseSolverState structure.

USAGE (out-of-core mode):

    1. User initializes algorithm state with SparseSolverCreate() call
    2. User  selects   algorithm  with one of the SparseSolverSetAlgo???()
       functions. By default, GMRES(k) is used with automatically chosen k
    3. Optionally, user tunes solver parameters, sets starting point, etc.
    4. After that user should work with out-of-core interface  in  a  loop
       like one given below:

        > alglib.sparsesolveroocstart(state)
        > while alglib.sparsesolverooccontinue(state) do
        >     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
        >     alglib.sparsesolveroocgetrequestdata(state, out X)
        >     if RequestType=0 then
        >         [calculate  Y=A*X, with X=R^N]
        >     alglib.sparsesolveroocsendresult(state, in Y)
        > alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    N       -   problem dimensionality (fixed at start-up)

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolvercreate(const ae_int_t n, sparsesolverstate &state, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolvercreate(n, state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function sets the solver algorithm to GMRES(k).

NOTE: if you do not need advanced functionality of the  SparseSolver  API,
      you   may   use   convenience   functions   SparseSolveGMRES()   and
      SparseSolveSymmetricGMRES().

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    K       -   GMRES parameter, K>=0:
                * recommended values are in 10..100 range
                * larger values up to N are possible but have little sense
                  - the algorithm will be slower than any dense solver.
                * values above N are truncated down to N
                * zero value means that  default  value  is  chosen.  This
                  value is 50 in the current version, but  it  may  change
                  in future ALGLIB releases.

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversetalgogmres(sparsesolverstate &state, const ae_int_t k, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolversetalgogmres(state.c_ptr(), k, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function sets starting point.
By default, zero starting point is used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    X       -   starting point, array[N]

OUTPUT PARAMETERS:
    State   -   new starting point was set

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversetstartingpoint(sparsesolverstate &state, const real_1d_array &x, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolversetstartingpoint(state.c_ptr(), x.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function sets stopping criteria.

INPUT PARAMETERS:
    EpsF    -   algorithm will be stopped if norm of residual is less than
                EpsF*||b||.
    MaxIts  -   algorithm will be stopped if number of iterations is  more
                than MaxIts.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
If  both  EpsF  and  MaxIts  are  zero then small EpsF will be set to small
value.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void sparsesolversetcond(sparsesolverstate &state, const double epsf, const ae_int_t maxits, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolversetcond(state.c_ptr(), epsf, maxits, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Procedure for  the  solution of A*x=b with sparse symmetric A given by its
lower or upper triangle.

This function will work with any solver algorithm  being   used,  SPD  one
(like CG) or not (like GMRES). Using unsymmetric solvers (like  GMRES)  on
SPD problems is suboptimal, but still possible.

NOTE: the  solver  behavior is ill-defined  for  a  situation  when a  SPD
      solver is used on indefinite matrix. It  may solve the problem up to
      desired precision (sometimes, rarely)  or  return  with  error  code
      signalling violation of underlying assumptions.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse symmetric NxN matrix in any sparse storage  format.
                Using CRS format is recommended because it avoids internal
                conversion.
                An exception will be generated if  A  is  not  NxN  matrix
                (where  N  is  a  size   specified  during  solver  object
                creation).
    IsUpper -   whether upper or lower triangle of A is used:
                * IsUpper=True  => only upper triangle is used and lower
                                   triangle is not referenced at all
                * IsUpper=False => only lower triangle is used and upper
                                   triangle is not referenced at all
    B       -   right part, array[N]

RESULT:
    This function returns no result.
    You can get the solution by calling SparseSolverResults()

  -- ALGLIB --
     Copyright 25.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversolvesymmetric(sparsesolverstate &state, const sparsematrix &a, const bool isupper, const real_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolversolvesymmetric(state.c_ptr(), a.c_ptr(), isupper, b.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Procedure for the solution of A*x=b with sparse nonsymmetric A

IMPORTANT: this function will work with any solver algorithm  being  used,
           symmetric solver like CG,  or  not.  However,  using  symmetric
           solvers on nonsymmetric problems is  dangerous.  It  may  solve
           the problem up  to  desired  precision  (sometimes,  rarely) or
           terminate with error code signalling  violation  of  underlying
           assumptions.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse NxN matrix in any sparse storage  format.
                Using CRS format is recommended because it avoids internal
                conversion.
                An exception will be generated if  A  is  not  NxN  matrix
                (where  N  is  a  size   specified  during  solver  object
                creation).
    B       -   right part, array[N]

RESULT:
    This function returns no result.
    You can get the solution by calling SparseSolverResults()

  -- ALGLIB --
     Copyright 25.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversolve(sparsesolverstate &state, const sparsematrix &a, const real_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolversolve(state.c_ptr(), a.c_ptr(), b.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Sparse solver results.

This function must be called after calling one of the SparseSolverSolve()
functions.

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution
    Rep     -   solution report:
                * Rep.TerminationType completion code:
                    * -5    CG method was used for a matrix which  is  not
                            positive definite
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                    *  8    the  algorithm  was  terminated   early  with
                            SparseSolverRequestTermination() being called
                            from other thread.
                * Rep.IterationsCount contains iterations count
                * Rep.NMV contains number of matrix-vector calculations
                * Rep.R2 contains squared residual
s
  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void sparsesolverresults(sparsesolverstate &state, real_1d_array &x, sparsesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolverresults(state.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function turns on/off reporting during out-of-core processing.

When the solver works in the out-of-core mode, it  can  be  configured  to
report its progress by returning current location. These location  reports
are implemented as a special kind of the out-of-core request:
* SparseSolverOOCGetRequestInfo() returns -1
* SparseSolverOOCGetRequestData() returns current location
* SparseSolverOOCGetRequestData1() returns squared norm of the residual
* SparseSolverOOCSendResult() shall NOT be called

This function has no effect when SparseSolverSolve() is used because  this
function has no method of reporting its progress.

NOTE: when used with GMRES(k), this function reports progress  every  k-th
      iteration.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

  -- ALGLIB --
     Copyright 01.10.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversetxrep(sparsesolverstate &state, const bool needxrep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolversetxrep(state.c_ptr(), needxrep, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function initiates out-of-core mode of the sparse solver.  It  should
be used in conjunction with other out-of-core-related  functions  of  this
subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State       -   solver object

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocstart(sparsesolverstate &state, const real_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolveroocstart(state.c_ptr(), b.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function performs iterative solution of  the  linear  system  in  the
out-of-core mode. It should be used in conjunction with other out-of-core-
related functions of this subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
bool sparsesolverooccontinue(sparsesolverstate &state, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::sparsesolverooccontinue(state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}

/*************************************************************************
This function is used to retrieve information  about  out-of-core  request
sent by the solver:
* RequestType=0  means that matrix-vector products A*x is requested
* RequestType=-1 means that solver reports its progress; this  request  is
  returned only when reports are activated wit SparseSolverSetXRep().

This function returns just request type; in order  to  get contents of the
trial vector, use sparsesolveroocgetrequestdata().

It should be used in conjunction with other out-of-core-related  functions
of this subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode

OUTPUT PARAMETERS:
    RequestType     -   type of the request to process:
                        * 0   for matrix-vector product A*x, with A  being
                          NxN system matrix  and X being N-dimensional
                          vector
                        *-1   for location and residual report


  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocgetrequestinfo(sparsesolverstate &state, ae_int_t &requesttype, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolveroocgetrequestinfo(state.c_ptr(), &requesttype, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function is used  to  retrieve  vector  associated  with  out-of-core
request sent by the solver to user code. Depending  on  the  request  type
(returned by the SparseSolverOOCGetRequestInfo()) this  vector  should  be
multiplied by A or subjected to another processing.

It should be used in conjunction with other out-of-core-related  functions
of this subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode
    X               -   possibly  preallocated   storage;  reallocated  if
                        needed, left unchanged, if large enough  to  store
                        request data.

OUTPUT PARAMETERS:
    X               -   array[N] or larger, leading N elements are  filled
                        with vector X.


  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocgetrequestdata(sparsesolverstate &state, real_1d_array &x, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolveroocgetrequestdata(state.c_ptr(), x.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function is used to retrieve scalar value associated with out-of-core
request sent by the solver to user code. In  the  current  ALGLIB  version
this function is used to retrieve squared residual  norm  during  progress
reports.

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode

OUTPUT PARAMETERS:
    V               -   scalar value associated with the current request


  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocgetrequestdata1(sparsesolverstate &state, double &v, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolveroocgetrequestdata1(state.c_ptr(), &v, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function is used to send user reply to out-of-core  request  sent  by
the solver. Usually it is product A*x for vector X returned by the solver.

It should be used in conjunction with other out-of-core-related  functions
of this subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode
    AX              -   array[N] or larger, leading N elements contain A*x


  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocsendresult(sparsesolverstate &state, const real_1d_array &ax, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolveroocsendresult(state.c_ptr(), ax.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This  function  finalizes out-of-core mode of the linear solver. It should
be used in conjunction with other out-of-core-related  functions  of  this
subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State       -   solver state

OUTPUT PARAMETERS:
    X       -   array[N], the solution.
                Zero-filled on the failure (Rep.TerminationType<0).
    Rep     -   report with additional info:
                * Rep.TerminationType completion code:
                    * -5    CG method was used for a matrix which  is  not
                            positive definite
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                    *  8    the  algorithm  was  terminated   early  with
                            SparseSolverRequestTermination() being called
                            from other thread.
                * Rep.IterationsCount contains iterations count
                * Rep.NMV contains number of matrix-vector calculations
                * Rep.R2 contains squared residual

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocstop(sparsesolverstate &state, real_1d_array &x, sparsesolverreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolveroocstop(state.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This subroutine submits request for termination of the running solver.  It
can be called from some other thread which wants the   solver to terminate
or when processing an out-of-core request.

As result, solver  stops  at  point  which  was  "current  accepted"  when
the termination request was submitted and returns error code 8 (successful
termination).  Such   termination   is  a smooth  process  which  properly
deallocates all temporaries.

INPUT PARAMETERS:
    State   -   solver structure

NOTE: calling this function on solver which is NOT running  will  have  no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

NOTE: solver clears termination flag on its start, it means that  if  some
      other thread will request termination too soon, its request will went
      unnoticed.

  -- ALGLIB --
     Copyright 01.10.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolverrequesttermination(sparsesolverstate &state, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::sparsesolverrequesttermination(state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

#if defined(AE_COMPILE_LINCG) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This object stores state of the linear CG method.

You should use ALGLIB functions to work with this object.
Never try to access its fields directly!
*************************************************************************/
_lincgstate_owner::_lincgstate_owner()
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_lincgstate_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    p_struct = (alglib_impl::lincgstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::lincgstate), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::lincgstate));
    alglib_impl::_lincgstate_init(p_struct, &_state, ae_false);
    ae_state_clear(&_state);
}

_lincgstate_owner::_lincgstate_owner(const _lincgstate_owner &rhs)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_lincgstate_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: lincgstate copy constructor failure (source is not initialized)", &_state);
    p_struct = (alglib_impl::lincgstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::lincgstate), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::lincgstate));
    alglib_impl::_lincgstate_init_copy(p_struct, const_cast<alglib_impl::lincgstate*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
}

_lincgstate_owner& _lincgstate_owner::operator=(const _lincgstate_owner &rhs)
{
    if( this==&rhs )
        return *this;
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return *this;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    alglib_impl::ae_assert(p_struct!=NULL, "ALGLIB: lincgstate assignment constructor failure (destination is not initialized)", &_state);
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: lincgstate assignment constructor failure (source is not initialized)", &_state);
    alglib_impl::_lincgstate_destroy(p_struct);
    memset(p_struct, 0, sizeof(alglib_impl::lincgstate));
    alglib_impl::_lincgstate_init_copy(p_struct, const_cast<alglib_impl::lincgstate*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
    return *this;
}

_lincgstate_owner::~_lincgstate_owner()
{
    if( p_struct!=NULL )
    {
        alglib_impl::_lincgstate_destroy(p_struct);
        ae_free(p_struct);
    }
}

alglib_impl::lincgstate* _lincgstate_owner::c_ptr()
{
    return p_struct;
}

const alglib_impl::lincgstate* _lincgstate_owner::c_ptr() const
{
    return p_struct;
}
lincgstate::lincgstate() : _lincgstate_owner() 
{
}

lincgstate::lincgstate(const lincgstate &rhs):_lincgstate_owner(rhs) 
{
}

lincgstate& lincgstate::operator=(const lincgstate &rhs)
{
    if( this==&rhs )
        return *this;
    _lincgstate_owner::operator=(rhs);
    return *this;
}

lincgstate::~lincgstate()
{
}


/*************************************************************************

*************************************************************************/
_lincgreport_owner::_lincgreport_owner()
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_lincgreport_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    p_struct = (alglib_impl::lincgreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::lincgreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::lincgreport));
    alglib_impl::_lincgreport_init(p_struct, &_state, ae_false);
    ae_state_clear(&_state);
}

_lincgreport_owner::_lincgreport_owner(const _lincgreport_owner &rhs)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_lincgreport_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: lincgreport copy constructor failure (source is not initialized)", &_state);
    p_struct = (alglib_impl::lincgreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::lincgreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::lincgreport));
    alglib_impl::_lincgreport_init_copy(p_struct, const_cast<alglib_impl::lincgreport*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
}

_lincgreport_owner& _lincgreport_owner::operator=(const _lincgreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return *this;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    alglib_impl::ae_assert(p_struct!=NULL, "ALGLIB: lincgreport assignment constructor failure (destination is not initialized)", &_state);
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: lincgreport assignment constructor failure (source is not initialized)", &_state);
    alglib_impl::_lincgreport_destroy(p_struct);
    memset(p_struct, 0, sizeof(alglib_impl::lincgreport));
    alglib_impl::_lincgreport_init_copy(p_struct, const_cast<alglib_impl::lincgreport*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
    return *this;
}

_lincgreport_owner::~_lincgreport_owner()
{
    if( p_struct!=NULL )
    {
        alglib_impl::_lincgreport_destroy(p_struct);
        ae_free(p_struct);
    }
}

alglib_impl::lincgreport* _lincgreport_owner::c_ptr()
{
    return p_struct;
}

const alglib_impl::lincgreport* _lincgreport_owner::c_ptr() const
{
    return p_struct;
}
lincgreport::lincgreport() : _lincgreport_owner() ,iterationscount(p_struct->iterationscount),nmv(p_struct->nmv),terminationtype(p_struct->terminationtype),r2(p_struct->r2)
{
}

lincgreport::lincgreport(const lincgreport &rhs):_lincgreport_owner(rhs) ,iterationscount(p_struct->iterationscount),nmv(p_struct->nmv),terminationtype(p_struct->terminationtype),r2(p_struct->r2)
{
}

lincgreport& lincgreport::operator=(const lincgreport &rhs)
{
    if( this==&rhs )
        return *this;
    _lincgreport_owner::operator=(rhs);
    return *this;
}

lincgreport::~lincgreport()
{
}

/*************************************************************************
This function initializes linear CG Solver. This solver is used  to  solve
symmetric positive definite problems. If you want  to  solve  nonsymmetric
(or non-positive definite) problem you may use LinLSQR solver provided  by
ALGLIB.

USAGE:
1. User initializes algorithm state with LinCGCreate() call
2. User tunes solver parameters with  LinCGSetCond() and other functions
3. Optionally, user sets starting point with LinCGSetStartingPoint()
4. User  calls LinCGSolveSparse() function which takes algorithm state and
   SparseMatrix object.
5. User calls LinCGResults() to get solution
6. Optionally, user may call LinCGSolveSparse()  again  to  solve  another
   problem  with different matrix and/or right part without reinitializing
   LinCGState structure.

INPUT PARAMETERS:
    N       -   problem dimension, N>0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgcreate(const ae_int_t n, lincgstate &state, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::lincgcreate(n, state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function sets starting point.
By default, zero starting point is used.

INPUT PARAMETERS:
    X       -   starting point, array[N]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetstartingpoint(lincgstate &state, const real_1d_array &x, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::lincgsetstartingpoint(state.c_ptr(), x.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This  function  changes  preconditioning  settings  of  LinCGSolveSparse()
function. By default, SolveSparse() uses diagonal preconditioner,  but  if
you want to use solver without preconditioning, you can call this function
which forces solver to use unit matrix for preconditioning.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void lincgsetprecunit(lincgstate &state, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::lincgsetprecunit(state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This  function  changes  preconditioning  settings  of  LinCGSolveSparse()
function.  LinCGSolveSparse() will use diagonal of the  system  matrix  as
preconditioner. This preconditioning mode is active by default.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void lincgsetprecdiag(lincgstate &state, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::lincgsetprecdiag(state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function sets stopping criteria.

INPUT PARAMETERS:
    EpsF    -   algorithm will be stopped if norm of residual is less than
                EpsF*||b||.
    MaxIts  -   algorithm will be stopped if number of iterations is  more
                than MaxIts.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
If  both  EpsF  and  MaxIts  are  zero then small EpsF will be set to small
value.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetcond(lincgstate &state, const double epsf, const ae_int_t maxits, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::lincgsetcond(state.c_ptr(), epsf, maxits, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Procedure for solution of A*x=b with sparse A.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse matrix in the CRS format (you MUST contvert  it  to
                CRS format by calling SparseConvertToCRS() function).
    IsUpper -   whether upper or lower triangle of A is used:
                * IsUpper=True  => only upper triangle is used and lower
                                   triangle is not referenced at all
                * IsUpper=False => only lower triangle is used and upper
                                   triangle is not referenced at all
    B       -   right part, array[N]

RESULT:
    This function returns no result.
    You can get solution by calling LinCGResults()

NOTE: this function uses lightweight preconditioning -  multiplication  by
      inverse of diag(A). If you want, you can turn preconditioning off by
      calling LinCGSetPrecUnit(). However, preconditioning cost is low and
      preconditioner  is  very  important  for  solution  of  badly scaled
      problems.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsolvesparse(lincgstate &state, const sparsematrix &a, const bool isupper, const real_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::lincgsolvesparse(state.c_ptr(), a.c_ptr(), isupper, b.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
CG-solver: results.

This function must be called after LinCGSolve

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -5    input matrix is either not positive definite,
                            too large or too small
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                * Rep.IterationsCount contains iterations count
                * NMV countains number of matrix-vector calculations

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgresults(const lincgstate &state, real_1d_array &x, lincgreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::lincgresults(state.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function sets restart frequency. By default, algorithm  is  restarted
after N subsequent iterations.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetrestartfreq(lincgstate &state, const ae_int_t srf, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::lincgsetrestartfreq(state.c_ptr(), srf, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function sets frequency of residual recalculations.

Algorithm updates residual r_k using iterative formula,  but  recalculates
it from scratch after each 10 iterations. It is done to avoid accumulation
of numerical errors and to stop algorithm when r_k starts to grow.

Such low update frequence (1/10) gives very  little  overhead,  but  makes
algorithm a bit more robust against numerical errors. However, you may
change it

INPUT PARAMETERS:
    Freq    -   desired update frequency, Freq>=0.
                Zero value means that no updates will be done.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetrupdatefreq(lincgstate &state, const ae_int_t freq, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::lincgsetrupdatefreq(state.c_ptr(), freq, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinCGOptimize().

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetxrep(lincgstate &state, const bool needxrep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::lincgsetxrep(state.c_ptr(), needxrep, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

#if defined(AE_COMPILE_LINLSQR) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This object stores state of the LinLSQR method.

You should use ALGLIB functions to work with this object.
*************************************************************************/
_linlsqrstate_owner::_linlsqrstate_owner()
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_linlsqrstate_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    p_struct = (alglib_impl::linlsqrstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::linlsqrstate), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::linlsqrstate));
    alglib_impl::_linlsqrstate_init(p_struct, &_state, ae_false);
    ae_state_clear(&_state);
}

_linlsqrstate_owner::_linlsqrstate_owner(const _linlsqrstate_owner &rhs)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_linlsqrstate_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: linlsqrstate copy constructor failure (source is not initialized)", &_state);
    p_struct = (alglib_impl::linlsqrstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::linlsqrstate), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::linlsqrstate));
    alglib_impl::_linlsqrstate_init_copy(p_struct, const_cast<alglib_impl::linlsqrstate*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
}

_linlsqrstate_owner& _linlsqrstate_owner::operator=(const _linlsqrstate_owner &rhs)
{
    if( this==&rhs )
        return *this;
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return *this;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    alglib_impl::ae_assert(p_struct!=NULL, "ALGLIB: linlsqrstate assignment constructor failure (destination is not initialized)", &_state);
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: linlsqrstate assignment constructor failure (source is not initialized)", &_state);
    alglib_impl::_linlsqrstate_destroy(p_struct);
    memset(p_struct, 0, sizeof(alglib_impl::linlsqrstate));
    alglib_impl::_linlsqrstate_init_copy(p_struct, const_cast<alglib_impl::linlsqrstate*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
    return *this;
}

_linlsqrstate_owner::~_linlsqrstate_owner()
{
    if( p_struct!=NULL )
    {
        alglib_impl::_linlsqrstate_destroy(p_struct);
        ae_free(p_struct);
    }
}

alglib_impl::linlsqrstate* _linlsqrstate_owner::c_ptr()
{
    return p_struct;
}

const alglib_impl::linlsqrstate* _linlsqrstate_owner::c_ptr() const
{
    return p_struct;
}
linlsqrstate::linlsqrstate() : _linlsqrstate_owner() 
{
}

linlsqrstate::linlsqrstate(const linlsqrstate &rhs):_linlsqrstate_owner(rhs) 
{
}

linlsqrstate& linlsqrstate::operator=(const linlsqrstate &rhs)
{
    if( this==&rhs )
        return *this;
    _linlsqrstate_owner::operator=(rhs);
    return *this;
}

linlsqrstate::~linlsqrstate()
{
}


/*************************************************************************

*************************************************************************/
_linlsqrreport_owner::_linlsqrreport_owner()
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_linlsqrreport_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    p_struct = (alglib_impl::linlsqrreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::linlsqrreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::linlsqrreport));
    alglib_impl::_linlsqrreport_init(p_struct, &_state, ae_false);
    ae_state_clear(&_state);
}

_linlsqrreport_owner::_linlsqrreport_owner(const _linlsqrreport_owner &rhs)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_linlsqrreport_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: linlsqrreport copy constructor failure (source is not initialized)", &_state);
    p_struct = (alglib_impl::linlsqrreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::linlsqrreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::linlsqrreport));
    alglib_impl::_linlsqrreport_init_copy(p_struct, const_cast<alglib_impl::linlsqrreport*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
}

_linlsqrreport_owner& _linlsqrreport_owner::operator=(const _linlsqrreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return *this;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    alglib_impl::ae_assert(p_struct!=NULL, "ALGLIB: linlsqrreport assignment constructor failure (destination is not initialized)", &_state);
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: linlsqrreport assignment constructor failure (source is not initialized)", &_state);
    alglib_impl::_linlsqrreport_destroy(p_struct);
    memset(p_struct, 0, sizeof(alglib_impl::linlsqrreport));
    alglib_impl::_linlsqrreport_init_copy(p_struct, const_cast<alglib_impl::linlsqrreport*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
    return *this;
}

_linlsqrreport_owner::~_linlsqrreport_owner()
{
    if( p_struct!=NULL )
    {
        alglib_impl::_linlsqrreport_destroy(p_struct);
        ae_free(p_struct);
    }
}

alglib_impl::linlsqrreport* _linlsqrreport_owner::c_ptr()
{
    return p_struct;
}

const alglib_impl::linlsqrreport* _linlsqrreport_owner::c_ptr() const
{
    return p_struct;
}
linlsqrreport::linlsqrreport() : _linlsqrreport_owner() ,iterationscount(p_struct->iterationscount),nmv(p_struct->nmv),terminationtype(p_struct->terminationtype)
{
}

linlsqrreport::linlsqrreport(const linlsqrreport &rhs):_linlsqrreport_owner(rhs) ,iterationscount(p_struct->iterationscount),nmv(p_struct->nmv),terminationtype(p_struct->terminationtype)
{
}

linlsqrreport& linlsqrreport::operator=(const linlsqrreport &rhs)
{
    if( this==&rhs )
        return *this;
    _linlsqrreport_owner::operator=(rhs);
    return *this;
}

linlsqrreport::~linlsqrreport()
{
}

/*************************************************************************
This function initializes linear LSQR Solver. This solver is used to solve
non-symmetric (and, possibly, non-square) problems. Least squares solution
is returned for non-compatible systems.

USAGE:
1. User initializes algorithm state with LinLSQRCreate() call
2. User tunes solver parameters with  LinLSQRSetCond() and other functions
3. User  calls  LinLSQRSolveSparse()  function which takes algorithm state
   and SparseMatrix object.
4. User calls LinLSQRResults() to get solution
5. Optionally, user may call LinLSQRSolveSparse() again to  solve  another
   problem  with different matrix and/or right part without reinitializing
   LinLSQRState structure.

INPUT PARAMETERS:
    M       -   number of rows in A
    N       -   number of variables, N>0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE: see also linlsqrcreatebuf()  for  version  which  reuses  previously
      allocated place as much as possible.

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrcreate(const ae_int_t m, const ae_int_t n, linlsqrstate &state, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::linlsqrcreate(m, n, state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function initializes linear LSQR Solver.  It  provides  exactly  same
functionality as linlsqrcreate(), but reuses  previously  allocated  space
as much as possible.

INPUT PARAMETERS:
    M       -   number of rows in A
    N       -   number of variables, N>0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 14.11.2018 by Bochkanov Sergey
*************************************************************************/
void linlsqrcreatebuf(const ae_int_t m, const ae_int_t n, linlsqrstate &state, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::linlsqrcreatebuf(m, n, state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This  function  changes  preconditioning  settings of LinLSQQSolveSparse()
function. By default, SolveSparse() uses diagonal preconditioner,  but  if
you want to use solver without preconditioning, you can call this function
which forces solver to use unit matrix for preconditioning.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetprecunit(linlsqrstate &state, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::linlsqrsetprecunit(state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This  function  changes  preconditioning  settings  of  LinCGSolveSparse()
function.  LinCGSolveSparse() will use diagonal of the  system  matrix  as
preconditioner. This preconditioning mode is active by default.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetprecdiag(linlsqrstate &state, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::linlsqrsetprecdiag(state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function sets optional Tikhonov regularization coefficient.
It is zero by default.

INPUT PARAMETERS:
    LambdaI -   regularization factor, LambdaI>=0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetlambdai(linlsqrstate &state, const double lambdai, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::linlsqrsetlambdai(state.c_ptr(), lambdai, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
Procedure for solution of A*x=b with sparse A.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse M*N matrix in the CRS format (you MUST contvert  it
                to CRS format  by  calling  SparseConvertToCRS()  function
                BEFORE you pass it to this function).
    B       -   right part, array[M]

RESULT:
    This function returns no result.
    You can get solution by calling LinCGResults()

NOTE: this function uses lightweight preconditioning -  multiplication  by
      inverse of diag(A). If you want, you can turn preconditioning off by
      calling LinLSQRSetPrecUnit(). However, preconditioning cost is   low
      and preconditioner is very important for solution  of  badly  scaled
      problems.

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsolvesparse(linlsqrstate &state, const sparsematrix &a, const real_1d_array &b, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::linlsqrsolvesparse(state.c_ptr(), a.c_ptr(), b.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function sets stopping criteria.

INPUT PARAMETERS:
    EpsA    -   algorithm will be stopped if ||A^T*Rk||/(||A||*||Rk||)<=EpsA.
    EpsB    -   algorithm will be stopped if ||Rk||<=EpsB*||B||
    MaxIts  -   algorithm will be stopped if number of iterations
                more than MaxIts.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE: if EpsA,EpsB,EpsC and MaxIts are zero then these variables will
be setted as default values.

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetcond(linlsqrstate &state, const double epsa, const double epsb, const ae_int_t maxits, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::linlsqrsetcond(state.c_ptr(), epsa, epsb, maxits, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
LSQR solver: results.

This function must be called after LinLSQRSolve

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    *  1    ||Rk||<=EpsB*||B||
                    *  4    ||A^T*Rk||/(||A||*||Rk||)<=EpsA
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            X contains best point found so far.
                            (sometimes returned on singular systems)
                    *  8    user requested termination via calling
                            linlsqrrequesttermination()
                * Rep.IterationsCount contains iterations count
                * NMV countains number of matrix-vector calculations

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrresults(const linlsqrstate &state, real_1d_array &x, linlsqrreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::linlsqrresults(state.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinCGOptimize().

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetxrep(linlsqrstate &state, const bool needxrep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::linlsqrsetxrep(state.c_ptr(), needxrep, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function is used to peek into LSQR solver and get  current  iteration
counter. You can safely "peek" into the solver from another thread.

INPUT PARAMETERS:
    S           -   solver object

RESULT:
    iteration counter, in [0,INF)

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
ae_int_t linlsqrpeekiterationscount(const linlsqrstate &s, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::ae_int_t result = alglib_impl::linlsqrpeekiterationscount(s.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return ae_int_t(result);
}

/*************************************************************************
This subroutine submits request for termination of the running solver.  It
can be called from some other thread which wants LSQR solver to  terminate
(obviously, the  thread  running  LSQR  solver can not request termination
because it is already busy working on LSQR).

As result, solver  stops  at  point  which  was  "current  accepted"  when
termination  request  was  submitted  and returns error code 8 (successful
termination).  Such   termination   is  a smooth  process  which  properly
deallocates all temporaries.

INPUT PARAMETERS:
    State   -   solver structure

NOTE: calling this function on solver which is NOT running  will  have  no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

NOTE: solver clears termination flag on its start, it means that  if  some
      other thread will request termination too soon, its request will went
      unnoticed.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void linlsqrrequesttermination(linlsqrstate &state, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::linlsqrrequesttermination(state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

#if defined(AE_COMPILE_NLEQ) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************

*************************************************************************/
_nleqstate_owner::_nleqstate_owner()
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_nleqstate_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    p_struct = (alglib_impl::nleqstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::nleqstate), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::nleqstate));
    alglib_impl::_nleqstate_init(p_struct, &_state, ae_false);
    ae_state_clear(&_state);
}

_nleqstate_owner::_nleqstate_owner(const _nleqstate_owner &rhs)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_nleqstate_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: nleqstate copy constructor failure (source is not initialized)", &_state);
    p_struct = (alglib_impl::nleqstate*)alglib_impl::ae_malloc(sizeof(alglib_impl::nleqstate), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::nleqstate));
    alglib_impl::_nleqstate_init_copy(p_struct, const_cast<alglib_impl::nleqstate*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
}

_nleqstate_owner& _nleqstate_owner::operator=(const _nleqstate_owner &rhs)
{
    if( this==&rhs )
        return *this;
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return *this;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    alglib_impl::ae_assert(p_struct!=NULL, "ALGLIB: nleqstate assignment constructor failure (destination is not initialized)", &_state);
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: nleqstate assignment constructor failure (source is not initialized)", &_state);
    alglib_impl::_nleqstate_destroy(p_struct);
    memset(p_struct, 0, sizeof(alglib_impl::nleqstate));
    alglib_impl::_nleqstate_init_copy(p_struct, const_cast<alglib_impl::nleqstate*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
    return *this;
}

_nleqstate_owner::~_nleqstate_owner()
{
    if( p_struct!=NULL )
    {
        alglib_impl::_nleqstate_destroy(p_struct);
        ae_free(p_struct);
    }
}

alglib_impl::nleqstate* _nleqstate_owner::c_ptr()
{
    return p_struct;
}

const alglib_impl::nleqstate* _nleqstate_owner::c_ptr() const
{
    return p_struct;
}
nleqstate::nleqstate() : _nleqstate_owner() ,needf(p_struct->needf),needfij(p_struct->needfij),xupdated(p_struct->xupdated),f(p_struct->f),fi(&p_struct->fi),j(&p_struct->j),x(&p_struct->x)
{
}

nleqstate::nleqstate(const nleqstate &rhs):_nleqstate_owner(rhs) ,needf(p_struct->needf),needfij(p_struct->needfij),xupdated(p_struct->xupdated),f(p_struct->f),fi(&p_struct->fi),j(&p_struct->j),x(&p_struct->x)
{
}

nleqstate& nleqstate::operator=(const nleqstate &rhs)
{
    if( this==&rhs )
        return *this;
    _nleqstate_owner::operator=(rhs);
    return *this;
}

nleqstate::~nleqstate()
{
}


/*************************************************************************

*************************************************************************/
_nleqreport_owner::_nleqreport_owner()
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_nleqreport_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    p_struct = (alglib_impl::nleqreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::nleqreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::nleqreport));
    alglib_impl::_nleqreport_init(p_struct, &_state, ae_false);
    ae_state_clear(&_state);
}

_nleqreport_owner::_nleqreport_owner(const _nleqreport_owner &rhs)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
        if( p_struct!=NULL )
        {
            alglib_impl::_nleqreport_destroy(p_struct);
            alglib_impl::ae_free(p_struct);
        }
        p_struct = NULL;
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    p_struct = NULL;
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: nleqreport copy constructor failure (source is not initialized)", &_state);
    p_struct = (alglib_impl::nleqreport*)alglib_impl::ae_malloc(sizeof(alglib_impl::nleqreport), &_state);
    memset(p_struct, 0, sizeof(alglib_impl::nleqreport));
    alglib_impl::_nleqreport_init_copy(p_struct, const_cast<alglib_impl::nleqreport*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
}

_nleqreport_owner& _nleqreport_owner::operator=(const _nleqreport_owner &rhs)
{
    if( this==&rhs )
        return *this;
    jmp_buf _break_jump;
    alglib_impl::ae_state _state;
    
    alglib_impl::ae_state_init(&_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_state.error_msg);
        return *this;
#endif
    }
    alglib_impl::ae_state_set_break_jump(&_state, &_break_jump);
    alglib_impl::ae_assert(p_struct!=NULL, "ALGLIB: nleqreport assignment constructor failure (destination is not initialized)", &_state);
    alglib_impl::ae_assert(rhs.p_struct!=NULL, "ALGLIB: nleqreport assignment constructor failure (source is not initialized)", &_state);
    alglib_impl::_nleqreport_destroy(p_struct);
    memset(p_struct, 0, sizeof(alglib_impl::nleqreport));
    alglib_impl::_nleqreport_init_copy(p_struct, const_cast<alglib_impl::nleqreport*>(rhs.p_struct), &_state, ae_false);
    ae_state_clear(&_state);
    return *this;
}

_nleqreport_owner::~_nleqreport_owner()
{
    if( p_struct!=NULL )
    {
        alglib_impl::_nleqreport_destroy(p_struct);
        ae_free(p_struct);
    }
}

alglib_impl::nleqreport* _nleqreport_owner::c_ptr()
{
    return p_struct;
}

const alglib_impl::nleqreport* _nleqreport_owner::c_ptr() const
{
    return p_struct;
}
nleqreport::nleqreport() : _nleqreport_owner() ,iterationscount(p_struct->iterationscount),nfunc(p_struct->nfunc),njac(p_struct->njac),terminationtype(p_struct->terminationtype)
{
}

nleqreport::nleqreport(const nleqreport &rhs):_nleqreport_owner(rhs) ,iterationscount(p_struct->iterationscount),nfunc(p_struct->nfunc),njac(p_struct->njac),terminationtype(p_struct->terminationtype)
{
}

nleqreport& nleqreport::operator=(const nleqreport &rhs)
{
    if( this==&rhs )
        return *this;
    _nleqreport_owner::operator=(rhs);
    return *this;
}

nleqreport::~nleqreport()
{
}

/*************************************************************************
                LEVENBERG-MARQUARDT-LIKE NONLINEAR SOLVER

DESCRIPTION:
This algorithm solves system of nonlinear equations
    F[0](x[0], ..., x[n-1])   = 0
    F[1](x[0], ..., x[n-1])   = 0
    ...
    F[M-1](x[0], ..., x[n-1]) = 0
with M/N do not necessarily coincide.  Algorithm  converges  quadratically
under following conditions:
    * the solution set XS is nonempty
    * for some xs in XS there exist such neighbourhood N(xs) that:
      * vector function F(x) and its Jacobian J(x) are continuously
        differentiable on N
      * ||F(x)|| provides local error bound on N, i.e. there  exists  such
        c1, that ||F(x)||>c1*distance(x,XS)
Note that these conditions are much more weaker than usual non-singularity
conditions. For example, algorithm will converge for any  affine  function
F (whether its Jacobian singular or not).


REQUIREMENTS:
Algorithm will request following information during its operation:
* function vector F[] and Jacobian matrix at given point X
* value of merit function f(x)=F[0]^2(x)+...+F[M-1]^2(x) at given point X


USAGE:
1. User initializes algorithm state with NLEQCreateLM() call
2. User tunes solver parameters with  NLEQSetCond(),  NLEQSetStpMax()  and
   other functions
3. User  calls  NLEQSolve()  function  which  takes  algorithm  state  and
   pointers (delegates, etc.) to callback functions which calculate  merit
   function value and Jacobian.
4. User calls NLEQResults() to get solution
5. Optionally, user may call NLEQRestartFrom() to  solve  another  problem
   with same parameters (N/M) but another starting  point  and/or  another
   function vector. NLEQRestartFrom() allows to reuse already  initialized
   structure.


INPUT PARAMETERS:
    N       -   space dimension, N>1:
                * if provided, only leading N elements of X are used
                * if not provided, determined automatically from size of X
    M       -   system size
    X       -   starting point


OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state


NOTES:
1. you may tune stopping conditions with NLEQSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use NLEQSetStpMax() function to bound algorithm's steps.
3. this  algorithm  is  a  slightly  modified implementation of the method
   described  in  'Levenberg-Marquardt  method  for constrained  nonlinear
   equations with strong local convergence properties' by Christian Kanzow
   Nobuo Yamashita and Masao Fukushima and further  developed  in  'On the
   convergence of a New Levenberg-Marquardt Method'  by  Jin-yan  Fan  and
   Ya-Xiang Yuan.


  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
void nleqcreatelm(const ae_int_t n, const ae_int_t m, const real_1d_array &x, nleqstate &state, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::nleqcreatelm(n, m, x.c_ptr(), state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
                LEVENBERG-MARQUARDT-LIKE NONLINEAR SOLVER

DESCRIPTION:
This algorithm solves system of nonlinear equations
    F[0](x[0], ..., x[n-1])   = 0
    F[1](x[0], ..., x[n-1])   = 0
    ...
    F[M-1](x[0], ..., x[n-1]) = 0
with M/N do not necessarily coincide.  Algorithm  converges  quadratically
under following conditions:
    * the solution set XS is nonempty
    * for some xs in XS there exist such neighbourhood N(xs) that:
      * vector function F(x) and its Jacobian J(x) are continuously
        differentiable on N
      * ||F(x)|| provides local error bound on N, i.e. there  exists  such
        c1, that ||F(x)||>c1*distance(x,XS)
Note that these conditions are much more weaker than usual non-singularity
conditions. For example, algorithm will converge for any  affine  function
F (whether its Jacobian singular or not).


REQUIREMENTS:
Algorithm will request following information during its operation:
* function vector F[] and Jacobian matrix at given point X
* value of merit function f(x)=F[0]^2(x)+...+F[M-1]^2(x) at given point X


USAGE:
1. User initializes algorithm state with NLEQCreateLM() call
2. User tunes solver parameters with  NLEQSetCond(),  NLEQSetStpMax()  and
   other functions
3. User  calls  NLEQSolve()  function  which  takes  algorithm  state  and
   pointers (delegates, etc.) to callback functions which calculate  merit
   function value and Jacobian.
4. User calls NLEQResults() to get solution
5. Optionally, user may call NLEQRestartFrom() to  solve  another  problem
   with same parameters (N/M) but another starting  point  and/or  another
   function vector. NLEQRestartFrom() allows to reuse already  initialized
   structure.


INPUT PARAMETERS:
    N       -   space dimension, N>1:
                * if provided, only leading N elements of X are used
                * if not provided, determined automatically from size of X
    M       -   system size
    X       -   starting point


OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state


NOTES:
1. you may tune stopping conditions with NLEQSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use NLEQSetStpMax() function to bound algorithm's steps.
3. this  algorithm  is  a  slightly  modified implementation of the method
   described  in  'Levenberg-Marquardt  method  for constrained  nonlinear
   equations with strong local convergence properties' by Christian Kanzow
   Nobuo Yamashita and Masao Fukushima and further  developed  in  'On the
   convergence of a New Levenberg-Marquardt Method'  by  Jin-yan  Fan  and
   Ya-Xiang Yuan.


  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
void nleqcreatelm(const ae_int_t m, const real_1d_array &x, nleqstate &state, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;    
    ae_int_t n;

    n = x.length();
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::nleqcreatelm(n, m, x.c_ptr(), state.c_ptr(), &_alglib_env_state);

    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif

/*************************************************************************
This function sets stopping conditions for the nonlinear solver

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsF    -   >=0
                The subroutine finishes  its work if on k+1-th iteration
                the condition ||F||<=EpsF is satisfied
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsF=0 and MaxIts=0 simultaneously will lead to  automatic
stopping criterion selection (small EpsF).

NOTES:

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void nleqsetcond(nleqstate &state, const double epsf, const ae_int_t maxits, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::nleqsetcond(state.c_ptr(), epsf, maxits, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to NLEQSolve().

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void nleqsetxrep(nleqstate &state, const bool needxrep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::nleqsetxrep(state.c_ptr(), needxrep, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when target function  contains  exp()  or  other  fast
growing functions, and algorithm makes  too  large  steps  which  lead  to
overflow. This function allows us to reject steps that are too large  (and
therefore expose us to the possible overflow) without actually calculating
function value at the x+stp*d.

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void nleqsetstpmax(nleqstate &state, const double stpmax, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::nleqsetstpmax(state.c_ptr(), stpmax, &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool nleqiteration(nleqstate &state, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return 0;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    ae_bool result = alglib_impl::nleqiteration(state.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return bool(result);
}


void nleqsolve(nleqstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr), 
    void *ptr,
    const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::ae_assert(func!=NULL, "ALGLIB: error in 'nleqsolve()' (func is NULL)", &_alglib_env_state);
    alglib_impl::ae_assert(jac!=NULL,  "ALGLIB: error in 'nleqsolve()' (jac is NULL)", &_alglib_env_state);
    while( alglib_impl::nleqiteration(state.c_ptr(), &_alglib_env_state) )
    {
        _ALGLIB_CALLBACK_EXCEPTION_GUARD_BEGIN
                if( state.needf )
                {
                    func(state.x, state.f, ptr);
                    continue;
                }
                if( state.needfij )
                {
                    jac(state.x, state.fi, state.j, ptr);
                    continue;
                }
        if( state.xupdated )
        {
            if( rep!=NULL )
                rep(state.x, state.f, ptr);
            continue;
        }
        goto lbl_no_callback;
        _ALGLIB_CALLBACK_EXCEPTION_GUARD_END
    lbl_no_callback:
        alglib_impl::ae_assert(ae_false, "ALGLIB: error in 'nleqsolve' (some derivatives were not provided?)", &_alglib_env_state);
    }
    alglib_impl::ae_state_clear(&_alglib_env_state);
}



/*************************************************************************
NLEQ solver results

INPUT PARAMETERS:
    State   -   algorithm state.

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -4    ERROR:  algorithm   has   converged   to   the
                            stationary point Xf which is local minimum  of
                            f=F[0]^2+...+F[m-1]^2, but is not solution  of
                            nonlinear system.
                    *  1    sqrt(f)<=EpsF.
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible
                * Rep.IterationsCount contains iterations count
                * NFEV countains number of function calculations
                * ActiveConstraints contains number of active constraints

  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
void nleqresults(const nleqstate &state, real_1d_array &x, nleqreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::nleqresults(state.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
NLEQ solver results

Buffered implementation of NLEQResults(), which uses pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
void nleqresultsbuf(const nleqstate &state, real_1d_array &x, nleqreport &rep, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::nleqresultsbuf(state.c_ptr(), x.c_ptr(), rep.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}

/*************************************************************************
This  subroutine  restarts  CG  algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used for reverse communication previously
                allocated with MinCGCreate call.
    X       -   new starting point.
    BndL    -   new lower bounds
    BndU    -   new upper bounds

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void nleqrestartfrom(nleqstate &state, const real_1d_array &x, const xparams _xparams)
{
    jmp_buf _break_jump;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    if( setjmp(_break_jump) )
    {
#if !defined(AE_NO_EXCEPTIONS)
        _ALGLIB_CPP_EXCEPTION(_alglib_env_state.error_msg);
#else
        _ALGLIB_SET_ERROR_FLAG(_alglib_env_state.error_msg);
        return;
#endif
    }
    ae_state_set_break_jump(&_alglib_env_state, &_break_jump);
    if( _xparams.flags!=(alglib_impl::ae_uint64_t)0x0 )
        ae_state_set_flags(&_alglib_env_state, _xparams.flags);
    alglib_impl::nleqrestartfrom(state.c_ptr(), x.c_ptr(), &_alglib_env_state);
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return;
}
#endif
}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS IMPLEMENTATION OF COMPUTATIONAL CORE
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
#if defined(AE_COMPILE_POLYNOMIALSOLVER) || !defined(AE_PARTIAL_BUILD)


#endif
#if defined(AE_COMPILE_DIRECTDENSESOLVERS) || !defined(AE_PARTIAL_BUILD)
static void directdensesolvers_rmatrixlusolveinternal(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_bool havea,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state);
static void directdensesolvers_spdmatrixcholeskysolveinternal(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_matrix* a,
     ae_bool havea,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state);
static void directdensesolvers_cmatrixlusolveinternal(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_bool havea,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state);
static void directdensesolvers_hpdmatrixcholeskysolveinternal(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ const ae_matrix* a,
     ae_bool havea,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state);
static ae_int_t directdensesolvers_densesolverrfsmax(ae_int_t n,
     double r1,
     double rinf,
     ae_state *_state);
static ae_int_t directdensesolvers_densesolverrfsmaxv2(ae_int_t n,
     double r2,
     ae_state *_state);
static void directdensesolvers_rbasiclusolve(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ ae_vector* xb,
     ae_state *_state);
static void directdensesolvers_spdbasiccholeskysolve(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* xb,
     ae_state *_state);
static void directdensesolvers_cbasiclusolve(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ ae_vector* xb,
     ae_state *_state);
static void directdensesolvers_hpdbasiccholeskysolve(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* xb,
     ae_state *_state);


#endif
#if defined(AE_COMPILE_DIRECTSPARSESOLVERS) || !defined(AE_PARTIAL_BUILD)


#endif
#if defined(AE_COMPILE_ITERATIVESPARSE) || !defined(AE_PARTIAL_BUILD)
static ae_bool iterativesparse_sparsesolveriteration(sparsesolverstate* state,
     ae_state *_state);
static void iterativesparse_clearrequestfields(sparsesolverstate* state,
     ae_state *_state);
static void iterativesparse_clearreportfields(sparsesolverstate* state,
     ae_state *_state);


#endif
#if defined(AE_COMPILE_LINCG) || !defined(AE_PARTIAL_BUILD)
static double lincg_defaultprecision = 1.0E-6;
static void lincg_clearrfields(lincgstate* state, ae_state *_state);
static void lincg_updateitersdata(lincgstate* state, ae_state *_state);


#endif
#if defined(AE_COMPILE_LINLSQR) || !defined(AE_PARTIAL_BUILD)
static double linlsqr_atol = 1.0E-6;
static double linlsqr_btol = 1.0E-6;
static void linlsqr_clearrfields(linlsqrstate* state, ae_state *_state);


#endif
#if defined(AE_COMPILE_NLEQ) || !defined(AE_PARTIAL_BUILD)
static void nleq_clearrequestfields(nleqstate* state, ae_state *_state);
static ae_bool nleq_increaselambda(double* lambdav,
     double* nu,
     double lambdaup,
     ae_state *_state);
static void nleq_decreaselambda(double* lambdav,
     double* nu,
     double lambdadown,
     ae_state *_state);


#endif

#if defined(AE_COMPILE_POLYNOMIALSOLVER) || !defined(AE_PARTIAL_BUILD)


/*************************************************************************
Polynomial root finding.

This function returns all roots of the polynomial
    P(x) = a0 + a1*x + a2*x^2 + ... + an*x^n
Both real and complex roots are returned (see below).

INPUT PARAMETERS:
    A       -   array[N+1], polynomial coefficients:
                * A[0] is constant term
                * A[N] is a coefficient of X^N
    N       -   polynomial degree

OUTPUT PARAMETERS:
    X       -   array of complex roots:
                * for isolated real root, X[I] is strictly real: IMAGE(X[I])=0
                * complex roots are always returned in pairs - roots occupy
                  positions I and I+1, with:
                  * X[I+1]=Conj(X[I])
                  * IMAGE(X[I]) > 0
                  * IMAGE(X[I+1]) = -IMAGE(X[I]) < 0
                * multiple real roots may have non-zero imaginary part due
                  to roundoff errors. There is no reliable way to distinguish
                  real root of multiplicity 2 from two  complex  roots  in
                  the presence of roundoff errors.
    Rep     -   report, additional information, following fields are set:
                * Rep.MaxErr - max( |P(xi)| )  for  i=0..N-1.  This  field
                  allows to quickly estimate "quality" of the roots  being
                  returned.

NOTE:   this function uses companion matrix method to find roots. In  case
        internal EVD  solver  fails  do  find  eigenvalues,  exception  is
        generated.

NOTE:   roots are not "polished" and  no  matrix  balancing  is  performed
        for them.

  -- ALGLIB --
     Copyright 24.02.2014 by Bochkanov Sergey
*************************************************************************/
void polynomialsolve(/* Real    */ const ae_vector* _a,
     ae_int_t n,
     /* Complex */ ae_vector* x,
     polynomialsolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector a;
    ae_matrix c;
    ae_matrix vl;
    ae_matrix vr;
    ae_vector wr;
    ae_vector wi;
    ae_int_t i;
    ae_int_t j;
    ae_bool status;
    ae_int_t nz;
    ae_int_t ne;
    ae_complex v;
    ae_complex vv;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    memset(&c, 0, sizeof(c));
    memset(&vl, 0, sizeof(vl));
    memset(&vr, 0, sizeof(vr));
    memset(&wr, 0, sizeof(wr));
    memset(&wi, 0, sizeof(wi));
    ae_vector_init_copy(&a, _a, _state, ae_true);
    ae_vector_clear(x);
    _polynomialsolverreport_clear(rep);
    ae_matrix_init(&c, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&vl, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&vr, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&wr, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&wi, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "PolynomialSolve: N<=0", _state);
    ae_assert(a.cnt>=n+1, "PolynomialSolve: Length(A)<N+1", _state);
    ae_assert(isfinitevector(&a, n+1, _state), "PolynomialSolve: A contains infitite numbers", _state);
    ae_assert(ae_fp_neq(a.ptr.p_double[n],(double)(0)), "PolynomialSolve: A[N]=0", _state);
    
    /*
     * Prepare
     */
    ae_vector_set_length(x, n, _state);
    
    /*
     * Normalize A:
     * * analytically determine NZ zero roots
     * * quick exit for NZ=N
     * * make residual NE-th degree polynomial monic
     *   (here NE=N-NZ)
     */
    nz = 0;
    while(nz<n&&ae_fp_eq(a.ptr.p_double[nz],(double)(0)))
    {
        nz = nz+1;
    }
    ne = n-nz;
    for(i=nz; i<=n; i++)
    {
        a.ptr.p_double[i-nz] = a.ptr.p_double[i]/a.ptr.p_double[n];
    }
    
    /*
     * For NZ<N, build companion matrix and find NE non-zero roots
     */
    if( ne>0 )
    {
        ae_matrix_set_length(&c, ne, ne, _state);
        for(i=0; i<=ne-1; i++)
        {
            for(j=0; j<=ne-1; j++)
            {
                c.ptr.pp_double[i][j] = (double)(0);
            }
        }
        c.ptr.pp_double[0][ne-1] = -a.ptr.p_double[0];
        for(i=1; i<=ne-1; i++)
        {
            c.ptr.pp_double[i][i-1] = (double)(1);
            c.ptr.pp_double[i][ne-1] = -a.ptr.p_double[i];
        }
        status = rmatrixevd(&c, ne, 0, &wr, &wi, &vl, &vr, _state);
        ae_assert(status, "PolynomialSolve: inernal error - EVD solver failed", _state);
        for(i=0; i<=ne-1; i++)
        {
            x->ptr.p_complex[i].x = wr.ptr.p_double[i];
            x->ptr.p_complex[i].y = wi.ptr.p_double[i];
        }
    }
    
    /*
     * Remaining NZ zero roots
     */
    for(i=ne; i<=n-1; i++)
    {
        x->ptr.p_complex[i] = ae_complex_from_i(0);
    }
    
    /*
     * Rep
     */
    rep->maxerr = (double)(0);
    for(i=0; i<=ne-1; i++)
    {
        v = ae_complex_from_i(0);
        vv = ae_complex_from_i(1);
        for(j=0; j<=ne; j++)
        {
            v = ae_c_add(v,ae_c_mul_d(vv,a.ptr.p_double[j]));
            vv = ae_c_mul(vv,x->ptr.p_complex[i]);
        }
        rep->maxerr = ae_maxreal(rep->maxerr, ae_c_abs(v, _state), _state);
    }
    ae_frame_leave(_state);
}


void _polynomialsolverreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    polynomialsolverreport *p = (polynomialsolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _polynomialsolverreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    polynomialsolverreport       *dst = (polynomialsolverreport*)_dst;
    const polynomialsolverreport *src = (const polynomialsolverreport*)_src;
    dst->maxerr = src->maxerr;
}


void _polynomialsolverreport_clear(void* _p)
{
    polynomialsolverreport *p = (polynomialsolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _polynomialsolverreport_destroy(void* _p)
{
    polynomialsolverreport *p = (polynomialsolverreport*)_p;
    ae_touch_ptr((void*)p);
}


#endif
#if defined(AE_COMPILE_DIRECTDENSESOLVERS) || !defined(AE_PARTIAL_BUILD)


/*************************************************************************
Dense solver for A*x=b with N*N real matrix A and N*1 real vectorx  x  and
b. This is "slow-but-feature rich" version of the  linear  solver.  Faster
version is RMatrixSolveFast() function.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations. It is also very significant on small matrices.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, RMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, the following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolve(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "RMatrixSolve: N<=0", _state);
    ae_assert(a->rows>=n, "RMatrixSolve: rows(A)<N", _state);
    ae_assert(a->cols>=n, "RMatrixSolve: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "RMatrixSolve: length(B)<N", _state);
    ae_assert(apservisfinitematrix(a, n, n, _state), "RMatrixSolve: A contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "RMatrixSolve: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&bm, n, 1, _state);
    rcopyvc(n, b, &bm, 0, _state);
    rmatrixsolvem(a, n, &bm, 1, ae_true, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    rcopycv(n, &xm, 0, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where A is NxN non-denegerate
real matrix, x  and  b  are  vectors.  This is a "fast" version of  linear
solver which does NOT provide  any  additional  functions  like  condition
number estimation or iterative refinement.

Algorithm features:
* efficient algorithm O(N^3) complexity
* no performance overhead from additional functionality

If you need condition number estimation or iterative refinement, use  more
feature-rich version - RMatrixSolve().

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros
                
RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 16.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixsolvefast(/* Real    */ const ae_matrix* _a,
     ae_int_t n,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_int_t i;
    ae_int_t j;
    ae_vector p;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    memset(&p, 0, sizeof(p));
    ae_matrix_init_copy(&a, _a, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "RMatrixSolveFast: N<=0", _state);
    ae_assert(a.rows>=n, "RMatrixSolveFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "RMatrixSolveFast: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "RMatrixSolveFast: length(B)<N", _state);
    ae_assert(apservisfinitematrix(&a, n, n, _state), "RMatrixSolveFast: A contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "RMatrixSolveFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    rmatrixlu(&a, n, n, &p, _state);
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_eq(a.ptr.pp_double[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                b->ptr.p_double[j] = 0.0;
            }
            result = ae_false;
            ae_frame_leave(_state);
            return result;
        }
    }
    directdensesolvers_rbasiclusolve(&a, &p, n, b, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Dense solver.

Similar to RMatrixSolve() but solves task with multiple right parts (where
b and x are NxM matrices). This is  "slow-but-robust"  version  of  linear
solver with additional functionality  like  condition  number  estimation.
There also exists faster version - RMatrixSolveMFast().

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* optional iterative refinement
* O(N^3+M*N^2) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations. It also very significant on small matrices.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, RMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolvem(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     ae_bool rfs,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix da;
    ae_matrix emptya;
    ae_vector p;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&da, 0, sizeof(da));
    memset(&emptya, 0, sizeof(emptya));
    memset(&p, 0, sizeof(p));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&da, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&emptya, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "RMatrixSolveM: N<=0", _state);
    ae_assert(m>0, "RMatrixSolveM: M<=0", _state);
    ae_assert(a->rows>=n, "RMatrixSolveM: rows(A)<N", _state);
    ae_assert(a->cols>=n, "RMatrixSolveM: cols(A)<N", _state);
    ae_assert(b->rows>=n, "RMatrixSolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "RMatrixSolveM: cols(B)<M", _state);
    ae_assert(apservisfinitematrix(a, n, n, _state), "RMatrixSolveM: A contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "RMatrixSolveM: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&da, n, n, _state);
    
    /*
     * 1. factorize matrix
     * 3. solve
     */
    for(i=0; i<=n-1; i++)
    {
        ae_v_move(&da.ptr.pp_double[i][0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
    }
    rmatrixlu(&da, n, n, &p, _state);
    if( rfs )
    {
        directdensesolvers_rmatrixlusolveinternal(&da, &p, n, a, ae_true, b, m, x, rep, _state);
    }
    else
    {
        directdensesolvers_rmatrixlusolveinternal(&da, &p, n, &emptya, ae_false, b, m, x, rep, _state);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

Similar to RMatrixSolve() but solves task with multiple right parts (where
b and x are NxM matrices). This is "fast" version of linear  solver  which
does NOT offer additional functions like condition  number  estimation  or
iterative refinement.

Algorithm features:
* O(N^3+M*N^2) complexity
* no additional functionality, highest performance

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros
                
RETURNS:
    True for well-conditioned matrix
    False for extremely badly conditioned or exactly singular problem

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixsolvemfast(/* Real    */ const ae_matrix* _a,
     ae_int_t n,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    double v;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_vector p;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    memset(&p, 0, sizeof(p));
    ae_matrix_init_copy(&a, _a, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "RMatrixSolveMFast: N<=0", _state);
    ae_assert(m>0, "RMatrixSolveMFast: M<=0", _state);
    ae_assert(a.rows>=n, "RMatrixSolveMFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "RMatrixSolveMFast: cols(A)<N", _state);
    ae_assert(b->rows>=n, "RMatrixSolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "RMatrixSolveMFast: cols(B)<M", _state);
    ae_assert(apservisfinitematrix(&a, n, n, _state), "RMatrixSolveMFast: A contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "RMatrixSolveMFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    rmatrixlu(&a, n, n, &p, _state);
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_eq(a.ptr.pp_double[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                for(k=0; k<=m-1; k++)
                {
                    b->ptr.pp_double[j][k] = 0.0;
                }
            }
            result = ae_false;
            ae_frame_leave(_state);
            return result;
        }
    }
    
    /*
     * Solve with TRSM()
     */
    for(i=0; i<=n-1; i++)
    {
        if( p.ptr.p_int[i]!=i )
        {
            for(j=0; j<=m-1; j++)
            {
                v = b->ptr.pp_double[i][j];
                b->ptr.pp_double[i][j] = b->ptr.pp_double[p.ptr.p_int[i]][j];
                b->ptr.pp_double[p.ptr.p_int[i]][j] = v;
            }
        }
    }
    rmatrixlefttrsm(n, m, &a, 0, 0, ae_false, ae_true, 0, b, 0, 0, _state);
    rmatrixlefttrsm(n, m, &a, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where A is NxN non-denegerate
real matrix given by its LU decomposition, x and b are real vectors.  This
is "slow-but-robust" version of the linear LU-based solver. Faster version
is RMatrixLUSolveFast() function.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! RMatrixLUSolveFast() function.

INPUT PARAMETERS
    LUA     -   array[N,N], LU decomposition, RMatrixLU result
    P       -   array[N], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[N], right part

OUTPUT PARAMETERS
    Rep     -   additional report, the following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

    
  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixlusolve(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "RMatrixLUSolve: N<=0", _state);
    ae_assert(lua->rows>=n, "RMatrixLUSolve: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "RMatrixLUSolve: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "RMatrixLUSolve: length(P)<N", _state);
    ae_assert(b->cnt>=n, "RMatrixLUSolve: length(B)<N", _state);
    ae_assert(apservisfinitematrix(lua, n, n, _state), "RMatrixLUSolve: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "RMatrixLUSolve: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "RMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    rcopyvc(n, b, &bm, 0, _state);
    rmatrixlusolvem(lua, p, n, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    rcopycv(n, &xm, 0, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where A is NxN non-denegerate
real matrix given by its LU decomposition, x and b are real vectors.  This
is "fast-without-any-checks" version of the linear LU-based solver. Slower
but more robust version is RMatrixLUSolve() function.

Algorithm features:
* O(N^2) complexity
* fast algorithm without ANY additional checks, just triangular solver

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system


  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixlusolvefast(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_bool result;


    ae_assert(n>0, "RMatrixLUSolveFast: N<=0", _state);
    ae_assert(lua->rows>=n, "RMatrixLUSolveFast: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "RMatrixLUSolveFast: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "RMatrixLUSolveFast: length(P)<N", _state);
    ae_assert(b->cnt>=n, "RMatrixLUSolveFast: length(B)<N", _state);
    ae_assert(apservisfinitematrix(lua, n, n, _state), "RMatrixLUSolveFast: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "RMatrixLUSolveFast: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "RMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_eq(lua->ptr.pp_double[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                b->ptr.p_double[j] = 0.0;
            }
            result = ae_false;
            return result;
        }
    }
    result = ae_true;
    directdensesolvers_rbasiclusolve(lua, p, n, b, _state);
    return result;
}


/*************************************************************************
Dense solver.

Similar to RMatrixLUSolve() but solves  task  with  multiple  right  parts
(where b and x are NxM matrices). This  is  "robust-but-slow"  version  of
LU-based solver which performs additional  checks  for  non-degeneracy  of
inputs (condition number estimation). If you need  best  performance,  use
"fast-without-any-checks" version, RMatrixLUSolveMFast().

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver.
           !
           ! This performance penalty is especially apparent when you  use
           ! ALGLIB parallel capabilities (condition number estimation  is
           ! inherently  sequential).  It   also   becomes significant for
           ! small-scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! RMatrixLUSolveMFast() function.
  
INPUT PARAMETERS
    LUA     -   array[N,N], LU decomposition, RMatrixLU result
    P       -   array[N], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixlusolvem(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix emptya;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&emptya, 0, sizeof(emptya));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&emptya, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "RMatrixLUSolveM: N<=0", _state);
    ae_assert(m>0, "RMatrixLUSolveM: M<=0", _state);
    ae_assert(lua->rows>=n, "RMatrixLUSolveM: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "RMatrixLUSolveM: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "RMatrixLUSolveM: length(P)<N", _state);
    ae_assert(b->rows>=n, "RMatrixLUSolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "RMatrixLUSolveM: cols(B)<M", _state);
    ae_assert(apservisfinitematrix(lua, n, n, _state), "RMatrixLUSolveM: LUA contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "RMatrixLUSolveM: LUA contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "RMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    directdensesolvers_rmatrixlusolveinternal(lua, p, n, &emptya, ae_false, b, m, x, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

Similar to RMatrixLUSolve() but solves  task  with  multiple  right parts,
where b and x are NxM matrices.  This is "fast-without-any-checks" version
of LU-based solver. It does not estimate  condition number  of  a  system,
so it is extremely fast. If you need better detection  of  near-degenerate
cases, use RMatrixLUSolveM() function.

Algorithm features:
* O(M*N^2) complexity
* fast algorithm without ANY additional checks, just triangular solver

INPUT PARAMETERS:
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS:
    B       -   array[N,M]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool rmatrixlusolvemfast(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    double v;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_bool result;


    
    /*
     * Check for exact degeneracy
     */
    ae_assert(n>0, "RMatrixLUSolveMFast: N<=0", _state);
    ae_assert(m>0, "RMatrixLUSolveMFast: M<=0", _state);
    ae_assert(lua->rows>=n, "RMatrixLUSolveMFast: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "RMatrixLUSolveMFast: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "RMatrixLUSolveMFast: length(P)<N", _state);
    ae_assert(b->rows>=n, "RMatrixLUSolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "RMatrixLUSolveMFast: cols(B)<M", _state);
    ae_assert(apservisfinitematrix(lua, n, n, _state), "RMatrixLUSolveMFast: LUA contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "RMatrixLUSolveMFast: LUA contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "RMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_eq(lua->ptr.pp_double[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                for(k=0; k<=m-1; k++)
                {
                    b->ptr.pp_double[j][k] = 0.0;
                }
            }
            result = ae_false;
            return result;
        }
    }
    result = ae_true;
    
    /*
     * Solve with TRSM()
     */
    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]!=i )
        {
            for(j=0; j<=m-1; j++)
            {
                v = b->ptr.pp_double[i][j];
                b->ptr.pp_double[i][j] = b->ptr.pp_double[p->ptr.p_int[i]][j];
                b->ptr.pp_double[p->ptr.p_int[i]][j] = v;
            }
        }
    }
    rmatrixlefttrsm(n, m, lua, 0, 0, ae_false, ae_true, 0, b, 0, 0, _state);
    rmatrixlefttrsm(n, m, lua, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    return result;
}


/*************************************************************************
Dense solver.

This  subroutine  solves  a  system  A*x=b,  where BOTH ORIGINAL A AND ITS
LU DECOMPOSITION ARE KNOWN. You can use it if for some  reasons  you  have
both A and its LU decomposition.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixmixedsolve(/* Real    */ const ae_matrix* a,
     /* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "RMatrixMixedSolve: N<=0", _state);
    ae_assert(a->rows>=n, "RMatrixMixedSolve: rows(A)<N", _state);
    ae_assert(a->cols>=n, "RMatrixMixedSolve: cols(A)<N", _state);
    ae_assert(lua->rows>=n, "RMatrixMixedSolve: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "RMatrixMixedSolve: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "RMatrixMixedSolve: length(P)<N", _state);
    ae_assert(b->cnt>=n, "RMatrixMixedSolve: length(B)<N", _state);
    ae_assert(apservisfinitematrix(a, n, n, _state), "RMatrixMixedSolve: A contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(lua, n, n, _state), "RMatrixMixedSolve: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "RMatrixMixedSolve: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "RMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    rcopyvc(n, b, &bm, 0, _state);
    rmatrixmixedsolvem(a, lua, p, n, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    rcopycv(n, &xm, 0, x, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver.

Similar to RMatrixMixedSolve() but  solves task with multiple right  parts
(where b and x are NxM matrices).

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void rmatrixmixedsolvem(/* Real    */ const ae_matrix* a,
     /* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_int_t i;

    ae_matrix_clear(x);
    _densesolverreport_clear(rep);

    ae_assert(n>0, "RMatrixMixedSolveM: N<=0", _state);
    ae_assert(m>0, "RMatrixMixedSolveM: M<=0", _state);
    ae_assert(a->rows>=n, "RMatrixMixedSolve: rows(A)<N", _state);
    ae_assert(a->cols>=n, "RMatrixMixedSolve: cols(A)<N", _state);
    ae_assert(lua->rows>=n, "RMatrixMixedSolve: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "RMatrixMixedSolve: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "RMatrixMixedSolve: length(P)<N", _state);
    ae_assert(b->rows>=n, "RMatrixMixedSolve: rows(B)<N", _state);
    ae_assert(b->cols>=m, "RMatrixMixedSolve: cols(B)<M", _state);
    ae_assert(apservisfinitematrix(a, n, n, _state), "RMatrixMixedSolve: A contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(lua, n, n, _state), "RMatrixMixedSolve: LUA contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "RMatrixMixedSolve: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "RMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    directdensesolvers_rmatrixlusolveinternal(lua, p, n, a, ae_true, b, m, x, rep, _state);
}


/*************************************************************************
Complex dense solver for A*X=B with N*N  complex  matrix  A,  N*M  complex
matrices  X  and  B.  "Slow-but-feature-rich"   version   which   provides
additional functions, at the cost of slower  performance.  Faster  version
may be invoked with CMatrixSolveMFast() function.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3+M*N^2) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, CMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size
    RFS     -   iterative refinement switch:
                * True - refinement is used.
                  Less performance, more precision.
                * False - refinement is not used.
                  More performance, less precision.

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixsolvem(/* Complex */ const ae_matrix* a,
     ae_int_t n,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     ae_bool rfs,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix da;
    ae_matrix emptya;
    ae_vector p;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&da, 0, sizeof(da));
    memset(&emptya, 0, sizeof(emptya));
    memset(&p, 0, sizeof(p));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&da, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&emptya, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "CMatrixSolveM: N<=0", _state);
    ae_assert(m>0, "CMatrixSolveM: M<=0", _state);
    ae_assert(a->rows>=n, "CMatrixSolveM: rows(A)<N", _state);
    ae_assert(a->cols>=n, "CMatrixSolveM: cols(A)<N", _state);
    ae_assert(b->rows>=n, "CMatrixSolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "CMatrixSolveM: cols(B)<M", _state);
    ae_assert(isfinitecmatrix(a, n, n, _state), "CMatrixSolveM: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "CMatrixSolveM: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&da, n, n, _state);
    
    /*
     * factorize, solve
     */
    for(i=0; i<=n-1; i++)
    {
        ae_v_cmove(&da.ptr.pp_complex[i][0], 1, &a->ptr.pp_complex[i][0], 1, "N", ae_v_len(0,n-1));
    }
    cmatrixlu(&da, n, n, &p, _state);
    if( rfs )
    {
        directdensesolvers_cmatrixlusolveinternal(&da, &p, n, a, ae_true, b, m, x, rep, _state);
    }
    else
    {
        directdensesolvers_cmatrixlusolveinternal(&da, &p, n, &emptya, ae_false, b, m, x, rep, _state);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Complex dense solver for A*X=B with N*N  complex  matrix  A,  N*M  complex
matrices  X  and  B.  "Fast-but-lightweight" version which  provides  just
triangular solver - and no additional functions like iterative  refinement
or condition number estimation.

Algorithm features:
* O(N^3+M*N^2) complexity
* no additional time consuming functions

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS:
    B       -   array[N,M]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 16.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixsolvemfast(/* Complex */ const ae_matrix* _a,
     ae_int_t n,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_complex v;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_vector p;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    memset(&p, 0, sizeof(p));
    ae_matrix_init_copy(&a, _a, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "CMatrixSolveMFast: N<=0", _state);
    ae_assert(m>0, "CMatrixSolveMFast: M<=0", _state);
    ae_assert(a.rows>=n, "CMatrixSolveMFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "CMatrixSolveMFast: cols(A)<N", _state);
    ae_assert(b->rows>=n, "CMatrixSolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "CMatrixSolveMFast: cols(B)<M", _state);
    ae_assert(isfinitecmatrix(&a, n, n, _state), "CMatrixSolveMFast: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "CMatrixSolveMFast: B contains infinite or NaN values!", _state);
    
    /*
     * Check for exact degeneracy
     */
    cmatrixlu(&a, n, n, &p, _state);
    for(i=0; i<=n-1; i++)
    {
        if( ae_c_eq_d(a.ptr.pp_complex[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                for(k=0; k<=m-1; k++)
                {
                    b->ptr.pp_complex[j][k] = ae_complex_from_d(0.0);
                }
            }
            result = ae_false;
            ae_frame_leave(_state);
            return result;
        }
    }
    result = ae_true;
    
    /*
     * Solve with TRSM()
     */
    for(i=0; i<=n-1; i++)
    {
        if( p.ptr.p_int[i]!=i )
        {
            for(j=0; j<=m-1; j++)
            {
                v = b->ptr.pp_complex[i][j];
                b->ptr.pp_complex[i][j] = b->ptr.pp_complex[p.ptr.p_int[i]][j];
                b->ptr.pp_complex[p.ptr.p_int[i]][j] = v;
            }
        }
    }
    cmatrixlefttrsm(n, m, &a, 0, 0, ae_false, ae_true, 0, b, 0, 0, _state);
    cmatrixlefttrsm(n, m, &a, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Complex dense solver for A*x=B with N*N complex matrix A and  N*1  complex
vectors x and b. "Slow-but-feature-rich" version of the solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^3) complexity

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear  system
           ! and  performs  iterative   refinement,   which   results   in
           ! significant performance penalty  when  compared  with  "fast"
           ! version  which  just  performs  LU  decomposition  and  calls
           ! triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, CMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixsolve(/* Complex */ const ae_matrix* a,
     ae_int_t n,
     /* Complex */ const ae_vector* b,
     /* Complex */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "CMatrixSolve: N<=0", _state);
    ae_assert(a->rows>=n, "CMatrixSolve: rows(A)<N", _state);
    ae_assert(a->cols>=n, "CMatrixSolve: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "CMatrixSolve: length(B)<N", _state);
    ae_assert(isfinitecmatrix(a, n, n, _state), "CMatrixSolve: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "CMatrixSolve: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    cmatrixsolvem(a, n, &bm, 1, ae_true, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Complex dense solver for A*x=B with N*N complex matrix A and  N*1  complex
vectors x and b. "Fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^3) complexity
* no additional time consuming features, just triangular solver

INPUT PARAMETERS:
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS:
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixsolvefast(/* Complex */ const ae_matrix* _a,
     ae_int_t n,
     /* Complex */ ae_vector* b,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_int_t i;
    ae_int_t j;
    ae_vector p;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    memset(&p, 0, sizeof(p));
    ae_matrix_init_copy(&a, _a, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    ae_assert(n>0, "CMatrixSolveFast: N<=0", _state);
    ae_assert(a.rows>=n, "CMatrixSolveFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "CMatrixSolveFast: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "CMatrixSolveFast: length(B)<N", _state);
    ae_assert(isfinitecmatrix(&a, n, n, _state), "CMatrixSolveFast: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "CMatrixSolveFast: B contains infinite or NaN values!", _state);
    cmatrixlu(&a, n, n, &p, _state);
    for(i=0; i<=n-1; i++)
    {
        if( ae_c_eq_d(a.ptr.pp_complex[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                b->ptr.p_complex[j] = ae_complex_from_d(0.0);
            }
            result = ae_false;
            ae_frame_leave(_state);
            return result;
        }
    }
    directdensesolvers_cbasiclusolve(&a, &p, n, b, _state);
    result = ae_true;
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Dense solver for A*X=B with N*N complex A given by its  LU  decomposition,
and N*M matrices X and B (multiple right sides).   "Slow-but-feature-rich"
version of the solver.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation

No iterative refinement  is provided because exact form of original matrix
is not known to subroutine. Use CMatrixSolve or CMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver.
           !
           ! This performance penalty is especially apparent when you  use
           ! ALGLIB parallel capabilities (condition number estimation  is
           ! inherently  sequential).  It   also   becomes significant for
           ! small-scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! CMatrixLUSolveMFast() function.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixlusolvem(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix emptya;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&emptya, 0, sizeof(emptya));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&emptya, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "CMatrixLUSolveM: N<=0", _state);
    ae_assert(m>0, "CMatrixLUSolveM: M<=0", _state);
    ae_assert(lua->rows>=n, "CMatrixLUSolveM: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "CMatrixLUSolveM: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "CMatrixLUSolveM: length(P)<N", _state);
    ae_assert(b->rows>=n, "CMatrixLUSolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "CMatrixLUSolveM: cols(B)<M", _state);
    ae_assert(isfinitecmatrix(lua, n, n, _state), "CMatrixLUSolveM: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "CMatrixLUSolveM: LUA contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "CMatrixLUSolveM: P contains values outside of [0,N)", _state);
    }
    directdensesolvers_cmatrixlusolveinternal(lua, p, n, &emptya, ae_false, b, m, x, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*X=B with N*N complex A given by its  LU  decomposition,
and N*M matrices X and B (multiple  right  sides).  "Fast-but-lightweight"
version of the solver.

Algorithm features:
* O(M*N^2) complexity
* no additional time-consuming features

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, RMatrixLU result
    P       -   array[0..N-1], pivots array, RMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[N,M]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixlusolvemfast(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    ae_complex v;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_bool result;


    ae_assert(n>0, "CMatrixLUSolveMFast: N<=0", _state);
    ae_assert(m>0, "CMatrixLUSolveMFast: M<=0", _state);
    ae_assert(lua->rows>=n, "CMatrixLUSolveMFast: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "CMatrixLUSolveMFast: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "CMatrixLUSolveMFast: length(P)<N", _state);
    ae_assert(b->rows>=n, "CMatrixLUSolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "CMatrixLUSolveMFast: cols(B)<M", _state);
    ae_assert(isfinitecmatrix(lua, n, n, _state), "CMatrixLUSolveMFast: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "CMatrixLUSolveMFast: LUA contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "RMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    
    /*
     * Check for exact degeneracy
     */
    for(i=0; i<=n-1; i++)
    {
        if( ae_c_eq_d(lua->ptr.pp_complex[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                for(k=0; k<=m-1; k++)
                {
                    b->ptr.pp_complex[j][k] = ae_complex_from_d(0.0);
                }
            }
            result = ae_false;
            return result;
        }
    }
    result = ae_true;
    
    /*
     * Solve with TRSM()
     */
    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]!=i )
        {
            for(j=0; j<=m-1; j++)
            {
                v = b->ptr.pp_complex[i][j];
                b->ptr.pp_complex[i][j] = b->ptr.pp_complex[p->ptr.p_int[i]][j];
                b->ptr.pp_complex[p->ptr.p_int[i]][j] = v;
            }
        }
    }
    cmatrixlefttrsm(n, m, lua, 0, 0, ae_false, ae_true, 0, b, 0, 0, _state);
    cmatrixlefttrsm(n, m, lua, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    return result;
}


/*************************************************************************
Complex dense linear solver for A*x=b with complex N*N A  given  by its LU
decomposition and N*1 vectors x and b. This is  "slow-but-robust"  version
of  the  complex  linear  solver  with  additional  features   which   add
significant performance overhead. Faster version  is  CMatrixLUSolveFast()
function.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation

No iterative refinement is provided because exact form of original matrix
is not known to subroutine. Use CMatrixSolve or CMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems.
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! CMatrixLUSolveFast() function.

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixlusolve(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ const ae_vector* b,
     /* Complex */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "CMatrixLUSolve: N<=0", _state);
    ae_assert(lua->rows>=n, "CMatrixLUSolve: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "CMatrixLUSolve: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "CMatrixLUSolve: length(P)<N", _state);
    ae_assert(b->cnt>=n, "CMatrixLUSolve: length(B)<N", _state);
    ae_assert(isfinitecmatrix(lua, n, n, _state), "CMatrixLUSolve: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "CMatrixLUSolve: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "CMatrixLUSolve: P contains values outside of [0,N)", _state);
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    cmatrixlusolvem(lua, p, n, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Complex dense linear solver for A*x=b with N*N complex A given by  its  LU
decomposition and N*1 vectors x and b. This is  fast  lightweight  version
of solver, which is significantly faster than CMatrixLUSolve(),  but  does
not provide additional information (like condition numbers).

Algorithm features:
* O(N^2) complexity
* no additional time-consuming features, just triangular solver

INPUT PARAMETERS
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros
    
NOTE: unlike  CMatrixLUSolve(),  this   function   does   NOT   check  for
      near-degeneracy of input matrix. It  checks  for  EXACT  degeneracy,
      because this check is easy to do. However,  very  badly  conditioned
      matrices may went unnoticed.


  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool cmatrixlusolvefast(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_bool result;


    ae_assert(n>0, "CMatrixLUSolveFast: N<=0", _state);
    ae_assert(lua->rows>=n, "CMatrixLUSolveFast: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "CMatrixLUSolveFast: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "CMatrixLUSolveFast: length(P)<N", _state);
    ae_assert(b->cnt>=n, "CMatrixLUSolveFast: length(B)<N", _state);
    ae_assert(isfinitecmatrix(lua, n, n, _state), "CMatrixLUSolveFast: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "CMatrixLUSolveFast: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "CMatrixLUSolveFast: P contains values outside of [0,N)", _state);
    }
    for(i=0; i<=n-1; i++)
    {
        if( ae_c_eq_d(lua->ptr.pp_complex[i][i],(double)(0)) )
        {
            for(j=0; j<=n-1; j++)
            {
                b->ptr.p_complex[j] = ae_complex_from_d(0.0);
            }
            result = ae_false;
            return result;
        }
    }
    directdensesolvers_cbasiclusolve(lua, p, n, b, _state);
    result = ae_true;
    return result;
}


/*************************************************************************
Dense solver. Same as RMatrixMixedSolveM(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(M*N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixmixedsolvem(/* Complex */ const ae_matrix* a,
     /* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_int_t i;

    ae_matrix_clear(x);
    _densesolverreport_clear(rep);

    ae_assert(n>0, "CMatrixMixedSolveM: N<=0", _state);
    ae_assert(m>0, "CMatrixMixedSolveM: M<=0", _state);
    ae_assert(a->rows>=n, "CMatrixMixedSolveM: rows(A)<N", _state);
    ae_assert(a->cols>=n, "CMatrixMixedSolveM: cols(A)<N", _state);
    ae_assert(lua->rows>=n, "CMatrixMixedSolveM: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "CMatrixMixedSolveM: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "CMatrixMixedSolveM: length(P)<N", _state);
    ae_assert(b->rows>=n, "CMatrixMixedSolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "CMatrixMixedSolveM: cols(B)<M", _state);
    ae_assert(isfinitecmatrix(a, n, n, _state), "CMatrixMixedSolveM: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(lua, n, n, _state), "CMatrixMixedSolveM: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "CMatrixMixedSolveM: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "CMatrixMixedSolveM: P contains values outside of [0,N)", _state);
    }
    directdensesolvers_cmatrixlusolveinternal(lua, p, n, a, ae_true, b, m, x, rep, _state);
}


/*************************************************************************
Dense solver. Same as RMatrixMixedSolve(), but for complex matrices.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* iterative refinement
* O(N^2) complexity

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    LUA     -   array[0..N-1,0..N-1], LU decomposition, CMatrixLU result
    P       -   array[0..N-1], pivots array, CMatrixLU result
    N       -   size of A
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or exactly singular matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void cmatrixmixedsolve(/* Complex */ const ae_matrix* a,
     /* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ const ae_vector* b,
     /* Complex */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;
    ae_int_t i;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "CMatrixMixedSolve: N<=0", _state);
    ae_assert(a->rows>=n, "CMatrixMixedSolve: rows(A)<N", _state);
    ae_assert(a->cols>=n, "CMatrixMixedSolve: cols(A)<N", _state);
    ae_assert(lua->rows>=n, "CMatrixMixedSolve: rows(LUA)<N", _state);
    ae_assert(lua->cols>=n, "CMatrixMixedSolve: cols(LUA)<N", _state);
    ae_assert(p->cnt>=n, "CMatrixMixedSolve: length(P)<N", _state);
    ae_assert(b->cnt>=n, "CMatrixMixedSolve: length(B)<N", _state);
    ae_assert(isfinitecmatrix(a, n, n, _state), "CMatrixMixedSolve: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(lua, n, n, _state), "CMatrixMixedSolve: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "CMatrixMixedSolve: B contains infinite or NaN values!", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=0&&p->ptr.p_int[i]<n, "CMatrixMixedSolve: P contains values outside of [0,N)", _state);
    }
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    cmatrixmixedsolvem(a, lua, p, n, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A,  and
N*M vectors X and B. It is "slow-but-feature-rich" version of the solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant   performance   penalty  when
           ! compared with "fast" version  which  just  performs  Cholesky
           ! decomposition and calls triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, SPDMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N,M], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixsolvem(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix da;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j1;
    ae_int_t j2;

    ae_frame_make(_state, &_frame_block);
    memset(&da, 0, sizeof(da));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&da, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "SPDMatrixSolveM: N<=0", _state);
    ae_assert(m>0, "SPDMatrixSolveM: M<=0", _state);
    ae_assert(a->rows>=n, "SPDMatrixSolveM: rows(A)<N", _state);
    ae_assert(a->cols>=n, "SPDMatrixSolveM: cols(A)<N", _state);
    ae_assert(b->rows>=n, "SPDMatrixSolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "SPDMatrixSolveM: cols(B)<M", _state);
    ae_assert(isfinitertrmatrix(a, n, isupper, _state), "SPDMatrixSolveM: A contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "SPDMatrixSolveM: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&da, n, n, _state);
    
    /*
     * factorize
     * solve
     */
    for(i=0; i<=n-1; i++)
    {
        if( isupper )
        {
            j1 = i;
            j2 = n-1;
        }
        else
        {
            j1 = 0;
            j2 = i;
        }
        ae_v_move(&da.ptr.pp_double[i][j1], 1, &a->ptr.pp_double[i][j1], 1, ae_v_len(j1,j2));
    }
    if( !spdmatrixcholesky(&da, n, isupper, _state) )
    {
        ae_matrix_set_length(x, n, m, _state);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_double[i][j] = (double)(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    rep->terminationtype = 1;
    directdensesolvers_spdmatrixcholeskysolveinternal(&da, n, isupper, a, ae_true, b, m, x, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A,  and
N*M vectors X and B. It is "fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming features

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[N,M], it contains:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool spdmatrixsolvemfast(/* Real    */ const ae_matrix* _a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_int_t i;
    ae_int_t j;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    ae_matrix_init_copy(&a, _a, _state, ae_true);

    ae_assert(n>0, "SPDMatrixSolveMFast: N<=0", _state);
    ae_assert(a.rows>=n, "SPDMatrixSolveMFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "SPDMatrixSolveMFast: cols(A)<N", _state);
    ae_assert(b->rows>=n, "SPDMatrixSolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "SPDMatrixSolveMFast: cols(B)<M", _state);
    ae_assert(isfinitertrmatrix(&a, n, isupper, _state), "SPDMatrixSolveMFast: A contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "SPDMatrixSolveMFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    if( !spdmatrixcholesky(&a, n, isupper, _state) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                b->ptr.pp_double[i][j] = 0.0;
            }
        }
        result = ae_false;
        ae_frame_leave(_state);
        return result;
    }
    if( isupper )
    {
        rmatrixlefttrsm(n, m, &a, 0, 0, ae_true, ae_false, 1, b, 0, 0, _state);
        rmatrixlefttrsm(n, m, &a, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    }
    else
    {
        rmatrixlefttrsm(n, m, &a, 0, 0, ae_false, ae_false, 0, b, 0, 0, _state);
        rmatrixlefttrsm(n, m, &a, 0, 0, ae_false, ae_false, 1, b, 0, 0, _state);
    }
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Dense linear solver for A*x=b with N*N real  symmetric  positive  definite
matrix A,  N*1 vectors x and b.  "Slow-but-feature-rich"  version  of  the
solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant   performance   penalty  when
           ! compared with "fast" version  which  just  performs  Cholesky
           ! decomposition and calls triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, SPDMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixsolve(/* Real    */ const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "SPDMatrixSolve: N<=0", _state);
    ae_assert(a->rows>=n, "SPDMatrixSolve: rows(A)<N", _state);
    ae_assert(a->cols>=n, "SPDMatrixSolve: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "SPDMatrixSolve: length(B)<N", _state);
    ae_assert(isfinitertrmatrix(a, n, isupper, _state), "SPDMatrixSolve: A contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "SPDMatrixSolve: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_move(&bm.ptr.pp_double[0][0], bm.stride, &b->ptr.p_double[0], 1, ae_v_len(0,n-1));
    spdmatrixsolvem(a, n, isupper, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_move(&x->ptr.p_double[0], 1, &xm.ptr.pp_double[0][0], xm.stride, ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense linear solver for A*x=b with N*N real  symmetric  positive  definite
matrix A,  N*1 vectors x and  b.  "Fast-but-lightweight"  version  of  the
solver.

Algorithm features:
* O(N^3) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming features like condition number estimation

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N], it contains:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool spdmatrixsolvefast(/* Real    */ const ae_matrix* _a,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_int_t i;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    ae_matrix_init_copy(&a, _a, _state, ae_true);

    ae_assert(n>0, "SPDMatrixSolveFast: N<=0", _state);
    ae_assert(a.rows>=n, "SPDMatrixSolveFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "SPDMatrixSolveFast: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "SPDMatrixSolveFast: length(B)<N", _state);
    ae_assert(isfinitertrmatrix(&a, n, isupper, _state), "SPDMatrixSolveFast: A contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "SPDMatrixSolveFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    if( !spdmatrixcholesky(&a, n, isupper, _state) )
    {
        for(i=0; i<=n-1; i++)
        {
            b->ptr.p_double[i] = 0.0;
        }
        result = ae_false;
        ae_frame_leave(_state);
        return result;
    }
    directdensesolvers_spdbasiccholeskysolve(&a, n, isupper, b, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*M vectors X and B. It  is  "slow-but-
feature-rich" version of the solver which estimates  condition  number  of
the system.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver. Amount of  overhead  introduced  depends  on  M  (the
           ! larger - the more efficient).
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! SPDMatrixCholeskySolveMFast() function.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N]:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixcholeskysolvem(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix emptya;

    ae_frame_make(_state, &_frame_block);
    memset(&emptya, 0, sizeof(emptya));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&emptya, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "SPDMatrixCholeskySolveM: N<=0", _state);
    ae_assert(m>0, "SPDMatrixCholeskySolveM: M<=0", _state);
    ae_assert(cha->rows>=n, "SPDMatrixCholeskySolveM: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "SPDMatrixCholeskySolveM: cols(CHA)<N", _state);
    ae_assert(b->rows>=n, "SPDMatrixCholeskySolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "SPDMatrixCholeskySolveM: cols(B)<M", _state);
    ae_assert(isfinitertrmatrix(cha, n, isupper, _state), "SPDMatrixCholeskySolveM: LUA contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "SPDMatrixCholeskySolveM: LUA contains infinite or NaN values!", _state);
    directdensesolvers_spdmatrixcholeskysolveinternal(cha, n, isupper, &emptya, ae_false, b, m, x, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*X=B with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*M vectors X and B. It  is  "fast-but-
lightweight" version of  the  solver  which  just  solves  linear  system,
without any additional functions.

Algorithm features:
* O(M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional functionality

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[N,M], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system
    
  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool spdmatrixcholeskysolvemfast(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_bool result;


    ae_assert(n>0, "SPDMatrixCholeskySolveMFast: N<=0", _state);
    ae_assert(m>0, "SPDMatrixCholeskySolveMFast: M<=0", _state);
    ae_assert(cha->rows>=n, "SPDMatrixCholeskySolveMFast: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "SPDMatrixCholeskySolveMFast: cols(CHA)<N", _state);
    ae_assert(b->rows>=n, "SPDMatrixCholeskySolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "SPDMatrixCholeskySolveMFast: cols(B)<M", _state);
    ae_assert(isfinitertrmatrix(cha, n, isupper, _state), "SPDMatrixCholeskySolveMFast: LUA contains infinite or NaN values!", _state);
    ae_assert(apservisfinitematrix(b, n, m, _state), "SPDMatrixCholeskySolveMFast: LUA contains infinite or NaN values!", _state);
    result = ae_true;
    for(k=0; k<=n-1; k++)
    {
        if( ae_fp_eq(cha->ptr.pp_double[k][k],0.0) )
        {
            for(i=0; i<=n-1; i++)
            {
                for(j=0; j<=m-1; j++)
                {
                    b->ptr.pp_double[i][j] = 0.0;
                }
            }
            result = ae_false;
            return result;
        }
    }
    if( isupper )
    {
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 1, b, 0, 0, _state);
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    }
    else
    {
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 0, b, 0, 0, _state);
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 1, b, 0, 0, _state);
    }
    return result;
}


/*************************************************************************
Dense solver for A*x=b with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*1 real vectors x and b. This is "slow-
but-feature-rich"  version  of  the  solver  which,  in  addition  to  the
solution, performs condition number estimation.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! SPDMatrixCholeskySolveFast() function.

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[N], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1    condition number in 1-norm
                * rep.rinf  condition number in inf-norm
    X       -   array[N]:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void spdmatrixcholeskysolve(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_REAL, _state, ae_true);

    ae_assert(n>0, "SPDMatrixCholeskySolve: N<=0", _state);
    ae_assert(cha->rows>=n, "SPDMatrixCholeskySolve: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "SPDMatrixCholeskySolve: cols(CHA)<N", _state);
    ae_assert(b->cnt>=n, "SPDMatrixCholeskySolve: length(B)<N", _state);
    ae_assert(isfinitertrmatrix(cha, n, isupper, _state), "SPDMatrixCholeskySolve: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "SPDMatrixCholeskySolve: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_move(&bm.ptr.pp_double[0][0], bm.stride, &b->ptr.p_double[0], 1, ae_v_len(0,n-1));
    spdmatrixcholeskysolvem(cha, n, isupper, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_move(&x->ptr.p_double[0], 1, &xm.ptr.pp_double[0][0], xm.stride, ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*x=b with N*N symmetric positive definite matrix A given
by its Cholesky decomposition, and N*1 real vectors x and b. This is "fast-
but-lightweight" version of the solver.

Algorithm features:
* O(N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional features

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[N], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or exactly singular system
    
  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
ae_bool spdmatrixcholeskysolvefast(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_bool result;


    ae_assert(n>0, "SPDMatrixCholeskySolveFast: N<=0", _state);
    ae_assert(cha->rows>=n, "SPDMatrixCholeskySolveFast: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "SPDMatrixCholeskySolveFast: cols(CHA)<N", _state);
    ae_assert(b->cnt>=n, "SPDMatrixCholeskySolveFast: length(B)<N", _state);
    ae_assert(isfinitertrmatrix(cha, n, isupper, _state), "SPDMatrixCholeskySolveFast: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, n, _state), "SPDMatrixCholeskySolveFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    for(k=0; k<=n-1; k++)
    {
        if( ae_fp_eq(cha->ptr.pp_double[k][k],0.0) )
        {
            for(i=0; i<=n-1; i++)
            {
                b->ptr.p_double[i] = 0.0;
            }
            result = ae_false;
            return result;
        }
    }
    directdensesolvers_spdbasiccholeskysolve(cha, n, isupper, b, _state);
    return result;
}


/*************************************************************************
Dense solver for A*X=B, with N*N Hermitian positive definite matrix A  and
N*M  complex  matrices  X  and  B.  "Slow-but-feature-rich" version of the
solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver.
           !
           ! This performance penalty is especially apparent when you  use
           ! ALGLIB parallel capabilities (condition number estimation  is
           ! inherently  sequential).  It   also   becomes significant for
           ! small-scale problems (N<100).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! HPDMatrixSolveMFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixsolvem(/* Complex */ const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix da;
    ae_int_t i;
    ae_int_t j;
    ae_int_t j1;
    ae_int_t j2;

    ae_frame_make(_state, &_frame_block);
    memset(&da, 0, sizeof(da));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&da, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "HPDMatrixSolveM: N<=0", _state);
    ae_assert(m>0, "HPDMatrixSolveM: M<=0", _state);
    ae_assert(a->rows>=n, "HPDMatrixSolveM: rows(A)<N", _state);
    ae_assert(a->cols>=n, "HPDMatrixSolveM: cols(A)<N", _state);
    ae_assert(b->rows>=n, "HPDMatrixSolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "HPDMatrixSolveM: cols(B)<M", _state);
    ae_assert(isfinitectrmatrix(a, n, isupper, _state), "HPDMatrixSolveM: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "HPDMatrixSolveM: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&da, n, n, _state);
    
    /*
     * factorize matrix, solve
     */
    for(i=0; i<=n-1; i++)
    {
        if( isupper )
        {
            j1 = i;
            j2 = n-1;
        }
        else
        {
            j1 = 0;
            j2 = i;
        }
        ae_v_cmove(&da.ptr.pp_complex[i][j1], 1, &a->ptr.pp_complex[i][j1], 1, "N", ae_v_len(j1,j2));
    }
    if( !hpdmatrixcholesky(&da, n, isupper, _state) )
    {
        ae_matrix_set_length(x, n, m, _state);
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_complex[i][j] = ae_complex_from_i(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    rep->terminationtype = 1;
    directdensesolvers_hpdmatrixcholeskysolveinternal(&da, n, isupper, a, ae_true, b, m, x, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*X=B, with N*N Hermitian positive definite matrix A  and
N*M complex matrices X and B. "Fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^3+M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming features like condition number estimation

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1,0..M-1], right part
    M       -   right part size

OUTPUT PARAMETERS
    B       -   array[0..N-1]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system
    
  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool hpdmatrixsolvemfast(/* Complex */ const ae_matrix* _a,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_int_t i;
    ae_int_t j;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    ae_matrix_init_copy(&a, _a, _state, ae_true);

    ae_assert(n>0, "HPDMatrixSolveMFast: N<=0", _state);
    ae_assert(a.rows>=n, "HPDMatrixSolveMFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "HPDMatrixSolveMFast: cols(A)<N", _state);
    ae_assert(b->rows>=n, "HPDMatrixSolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "HPDMatrixSolveMFast: cols(B)<M", _state);
    ae_assert(isfinitectrmatrix(&a, n, isupper, _state), "HPDMatrixSolveMFast: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "HPDMatrixSolveMFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    if( !hpdmatrixcholesky(&a, n, isupper, _state) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                b->ptr.pp_complex[i][j] = ae_complex_from_d(0.0);
            }
        }
        result = ae_false;
        ae_frame_leave(_state);
        return result;
    }
    if( isupper )
    {
        cmatrixlefttrsm(n, m, &a, 0, 0, ae_true, ae_false, 2, b, 0, 0, _state);
        cmatrixlefttrsm(n, m, &a, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    }
    else
    {
        cmatrixlefttrsm(n, m, &a, 0, 0, ae_false, ae_false, 0, b, 0, 0, _state);
        cmatrixlefttrsm(n, m, &a, 0, 0, ae_false, ae_false, 2, b, 0, 0, _state);
    }
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Dense solver for A*x=b, with N*N Hermitian positive definite matrix A, and
N*1 complex vectors  x  and  b.  "Slow-but-feature-rich"  version  of  the
solver.

Algorithm features:
* automatic detection of degenerate cases
* condition number estimation
* O(N^3) complexity
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant   performance   penalty  when
           ! compared with "fast" version  which  just  performs  Cholesky
           ! decomposition and calls triangular solver.
           !
           ! This  performance  penalty  is  especially  visible  in   the
           ! multithreaded mode, because both condition number  estimation
           ! and   iterative    refinement   are   inherently   sequential
           ! calculations.
           !
           ! Thus, if you need high performance and if you are pretty sure
           ! that your system is well conditioned, we  strongly  recommend
           ! you to use faster solver, HPDMatrixSolveFast() function.

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   same as in RMatrixSolve
    X       -   same as in RMatrixSolve

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixsolve(/* Complex */ const ae_matrix* a,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ const ae_vector* b,
     /* Complex */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "HPDMatrixSolve: N<=0", _state);
    ae_assert(a->rows>=n, "HPDMatrixSolve: rows(A)<N", _state);
    ae_assert(a->cols>=n, "HPDMatrixSolve: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "HPDMatrixSolve: length(B)<N", _state);
    ae_assert(isfinitectrmatrix(a, n, isupper, _state), "HPDMatrixSolve: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "HPDMatrixSolve: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    hpdmatrixsolvem(a, n, isupper, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*x=b, with N*N Hermitian positive definite matrix A, and
N*1 complex vectors  x  and  b.  "Fast-but-lightweight"  version  of   the
solver without additional functions.

Algorithm features:
* O(N^3) complexity
* matrix is represented by its upper or lower triangle
* no additional time consuming functions

INPUT PARAMETERS
    A       -   array[0..N-1,0..N-1], system matrix
    N       -   size of A
    IsUpper -   what half of A is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[0..N-1]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 17.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool hpdmatrixsolvefast(/* Complex */ const ae_matrix* _a,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* b,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix a;
    ae_int_t i;
    ae_bool result;

    ae_frame_make(_state, &_frame_block);
    memset(&a, 0, sizeof(a));
    ae_matrix_init_copy(&a, _a, _state, ae_true);

    ae_assert(n>0, "HPDMatrixSolveFast: N<=0", _state);
    ae_assert(a.rows>=n, "HPDMatrixSolveFast: rows(A)<N", _state);
    ae_assert(a.cols>=n, "HPDMatrixSolveFast: cols(A)<N", _state);
    ae_assert(b->cnt>=n, "HPDMatrixSolveFast: length(B)<N", _state);
    ae_assert(isfinitectrmatrix(&a, n, isupper, _state), "HPDMatrixSolveFast: A contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "HPDMatrixSolveFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    if( !hpdmatrixcholesky(&a, n, isupper, _state) )
    {
        for(i=0; i<=n-1; i++)
        {
            b->ptr.p_complex[i] = ae_complex_from_d(0.0);
        }
        result = ae_false;
        ae_frame_leave(_state);
        return result;
    }
    directdensesolvers_hpdbasiccholeskysolve(&a, n, isupper, b, _state);
    ae_frame_leave(_state);
    return result;
}


/*************************************************************************
Dense solver for A*X=B with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition and N*M complex matrices X  and  B.  This is
"slow-but-feature-rich" version of the solver which, in  addition  to  the
solution, estimates condition number of the system.

Algorithm features:
* automatic detection of degenerate cases
* O(M*N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which  results  in  significant  performance   penalty   when
           ! compared with "fast"  version  which  just  calls  triangular
           ! solver. Amount of  overhead  introduced  depends  on  M  (the
           ! larger - the more efficient).
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large Cholesky decomposition.  However,  if  you call
           ! this  function  many  times  for  the same  left  side,  this
           ! overhead BECOMES significant. It  also   becomes  significant
           ! for small-scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! HPDMatrixCholeskySolveMFast() function.


INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                HPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[N,M], right part
    M       -   right part size

OUTPUT PARAMETERS:
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixcholeskysolvem(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix emptya;

    ae_frame_make(_state, &_frame_block);
    memset(&emptya, 0, sizeof(emptya));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&emptya, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "HPDMatrixCholeskySolveM: N<=0", _state);
    ae_assert(m>0, "HPDMatrixCholeskySolveM: M<=0", _state);
    ae_assert(cha->rows>=n, "HPDMatrixCholeskySolveM: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "HPDMatrixCholeskySolveM: cols(CHA)<N", _state);
    ae_assert(b->rows>=n, "HPDMatrixCholeskySolveM: rows(B)<N", _state);
    ae_assert(b->cols>=m, "HPDMatrixCholeskySolveM: cols(B)<M", _state);
    ae_assert(isfinitectrmatrix(cha, n, isupper, _state), "HPDMatrixCholeskySolveM: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "HPDMatrixCholeskySolveM: LUA contains infinite or NaN values!", _state);
    directdensesolvers_hpdmatrixcholeskysolveinternal(cha, n, isupper, &emptya, ae_false, b, m, x, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*X=B with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition and N*M complex matrices X  and  B.  This is
"fast-but-lightweight" version of the solver.

Algorithm features:
* O(M*N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time-consuming features

INPUT PARAMETERS
    CHA     -   array[N,N], Cholesky decomposition,
                HPDMatrixCholesky result
    N       -   size of CHA
    IsUpper -   what half of CHA is provided
    B       -   array[N,M], right part
    M       -   right part size

OUTPUT PARAMETERS:
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system

  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool hpdmatrixcholeskysolvemfast(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_matrix* b,
     ae_int_t m,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_bool result;


    ae_assert(n>0, "HPDMatrixCholeskySolveMFast: N<=0", _state);
    ae_assert(m>0, "HPDMatrixCholeskySolveMFast: M<=0", _state);
    ae_assert(cha->rows>=n, "HPDMatrixCholeskySolveMFast: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "HPDMatrixCholeskySolveMFast: cols(CHA)<N", _state);
    ae_assert(b->rows>=n, "HPDMatrixCholeskySolveMFast: rows(B)<N", _state);
    ae_assert(b->cols>=m, "HPDMatrixCholeskySolveMFast: cols(B)<M", _state);
    ae_assert(isfinitectrmatrix(cha, n, isupper, _state), "HPDMatrixCholeskySolveMFast: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecmatrix(b, n, m, _state), "HPDMatrixCholeskySolveMFast: LUA contains infinite or NaN values!", _state);
    result = ae_true;
    for(k=0; k<=n-1; k++)
    {
        if( ae_fp_eq(cha->ptr.pp_complex[k][k].x,0.0)&&ae_fp_eq(cha->ptr.pp_complex[k][k].y,0.0) )
        {
            for(i=0; i<=n-1; i++)
            {
                for(j=0; j<=m-1; j++)
                {
                    b->ptr.pp_complex[i][j] = ae_complex_from_d(0.0);
                }
            }
            result = ae_false;
            return result;
        }
    }
    if( isupper )
    {
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 2, b, 0, 0, _state);
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 0, b, 0, 0, _state);
    }
    else
    {
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 0, b, 0, 0, _state);
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 2, b, 0, 0, _state);
    }
    return result;
}


/*************************************************************************
Dense solver for A*x=b with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition, and N*1 complex vectors x and  b.  This  is
"slow-but-feature-rich" version of the solver  which  estimates  condition
number of the system.

Algorithm features:
* automatic detection of degenerate cases
* O(N^2) complexity
* condition number estimation
* matrix is represented by its upper or lower triangle

No iterative refinement is provided because such partial representation of
matrix does not allow efficient calculation of extra-precise  matrix-vector
products for large matrices. Use RMatrixSolve or RMatrixMixedSolve  if  you
need iterative refinement.

IMPORTANT: ! this function is NOT the most efficient linear solver provided
           ! by ALGLIB. It estimates condition  number  of  linear system,
           ! which results in 10-15x  performance  penalty  when  compared
           ! with "fast" version which just calls triangular solver.
           !
           ! This performance penalty is insignificant  when compared with
           ! cost of large LU decomposition.  However,  if you  call  this
           ! function many times for the same  left  side,  this  overhead
           ! BECOMES significant. It  also  becomes significant for small-
           ! scale problems (N<50).
           !
           ! In such cases we strongly recommend you to use faster solver,
           ! HPDMatrixCholeskySolveFast() function.

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    Rep     -   additional report, following fields are set:
                * rep.terminationtype   >0 for success
                                        -3 for badly conditioned or indefinite matrix
                * rep.r1                condition number in 1-norm
                * rep.rinf              condition number in inf-norm
    X       -   array[N], it contains:
                * rep.terminationtype>0  => solution
                * rep.terminationtype=-3 => filled by zeros

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
void hpdmatrixcholeskysolve(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ const ae_vector* b,
     /* Complex */ ae_vector* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_matrix bm;
    ae_matrix xm;

    ae_frame_make(_state, &_frame_block);
    memset(&bm, 0, sizeof(bm));
    memset(&xm, 0, sizeof(xm));
    ae_vector_clear(x);
    _densesolverreport_clear(rep);
    ae_matrix_init(&bm, 0, 0, DT_COMPLEX, _state, ae_true);
    ae_matrix_init(&xm, 0, 0, DT_COMPLEX, _state, ae_true);

    ae_assert(n>0, "HPDMatrixCholeskySolve: N<=0", _state);
    ae_assert(cha->rows>=n, "HPDMatrixCholeskySolve: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "HPDMatrixCholeskySolve: cols(CHA)<N", _state);
    ae_assert(b->cnt>=n, "HPDMatrixCholeskySolve: length(B)<N", _state);
    ae_assert(isfinitectrmatrix(cha, n, isupper, _state), "HPDMatrixCholeskySolve: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "HPDMatrixCholeskySolve: B contains infinite or NaN values!", _state);
    ae_matrix_set_length(&bm, n, 1, _state);
    ae_v_cmove(&bm.ptr.pp_complex[0][0], bm.stride, &b->ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    hpdmatrixcholeskysolvem(cha, n, isupper, &bm, 1, &xm, rep, _state);
    ae_vector_set_length(x, n, _state);
    ae_v_cmove(&x->ptr.p_complex[0], 1, &xm.ptr.pp_complex[0][0], xm.stride, "N", ae_v_len(0,n-1));
    ae_frame_leave(_state);
}


/*************************************************************************
Dense solver for A*x=b with N*N Hermitian positive definite matrix A given
by its Cholesky decomposition, and N*1 complex vectors x and  b.  This  is
"fast-but-lightweight" version of the solver.

Algorithm features:
* O(N^2) complexity
* matrix is represented by its upper or lower triangle
* no additional time-consuming features

INPUT PARAMETERS
    CHA     -   array[0..N-1,0..N-1], Cholesky decomposition,
                SPDMatrixCholesky result
    N       -   size of A
    IsUpper -   what half of CHA is provided
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    B       -   array[N]:
                * result=true    =>  overwritten by solution
                * result=false   =>  filled by zeros

RETURNS:
    True, if the system was solved
    False, for an extremely badly conditioned or indefinite system

  -- ALGLIB --
     Copyright 18.03.2015 by Bochkanov Sergey
*************************************************************************/
ae_bool hpdmatrixcholeskysolvefast(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* b,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t k;
    ae_bool result;


    ae_assert(n>0, "HPDMatrixCholeskySolveFast: N<=0", _state);
    ae_assert(cha->rows>=n, "HPDMatrixCholeskySolveFast: rows(CHA)<N", _state);
    ae_assert(cha->cols>=n, "HPDMatrixCholeskySolveFast: cols(CHA)<N", _state);
    ae_assert(b->cnt>=n, "HPDMatrixCholeskySolveFast: length(B)<N", _state);
    ae_assert(isfinitectrmatrix(cha, n, isupper, _state), "HPDMatrixCholeskySolveFast: LUA contains infinite or NaN values!", _state);
    ae_assert(isfinitecvector(b, n, _state), "HPDMatrixCholeskySolveFast: B contains infinite or NaN values!", _state);
    result = ae_true;
    for(k=0; k<=n-1; k++)
    {
        if( ae_fp_eq(cha->ptr.pp_complex[k][k].x,0.0)&&ae_fp_eq(cha->ptr.pp_complex[k][k].y,0.0) )
        {
            for(i=0; i<=n-1; i++)
            {
                b->ptr.p_complex[i] = ae_complex_from_d(0.0);
            }
            result = ae_false;
            return result;
        }
    }
    directdensesolvers_hpdbasiccholeskysolve(cha, n, isupper, b, _state);
    return result;
}


/*************************************************************************
Dense solver.

This subroutine finds solution of the linear system A*X=B with non-square,
possibly degenerate A.  System  is  solved in the least squares sense, and
general least squares solution  X = X0 + CX*y  which  minimizes |A*X-B| is
returned. If A is non-degenerate, solution in the usual sense is returned.

Algorithm features:
* automatic detection (and correct handling!) of degenerate cases
* iterative refinement
* O(N^3) complexity

INPUT PARAMETERS
    A       -   array[0..NRows-1,0..NCols-1], system matrix
    NRows   -   vertical size of A
    NCols   -   horizontal size of A
    B       -   array[0..NCols-1], right part
    Threshold-  a number in [0,1]. Singular values  beyond  Threshold*Largest are
                considered  zero.  Set  it to 0.0, if you don't understand
                what it means, so the solver will choose good value on its
                own.
                
OUTPUT PARAMETERS
    Rep     -   solver report, see below for more info
    X       -   array[0..N-1,0..M-1], it contains:
                * solution of A*X=B (even for singular A)
                * zeros, if SVD subroutine failed

SOLVER REPORT

Subroutine sets following fields of the Rep structure:
* TerminationType is set to:
            * -4 for SVD failure
            * >0 for success
* R2        reciprocal of condition number: 1/cond(A), 2-norm.
* N         = NCols
* K         dim(Null(A))
* CX        array[0..N-1,0..K-1], kernel of A.
            Columns of CX store such vectors that A*CX[i]=0.

  ! FREE EDITION OF ALGLIB:
  ! 
  ! Free Edition of ALGLIB supports following important features for  this
  ! function:
  ! * C++ version: x64 SIMD support using C++ intrinsics
  ! * C#  version: x64 SIMD support using NET5/NetCore hardware intrinsics
  !
  ! We  recommend  you  to  read  'Compiling ALGLIB' section of the ALGLIB
  ! Reference Manual in order  to  find  out  how to activate SIMD support
  ! in ALGLIB.

  ! COMMERCIAL EDITION OF ALGLIB:
  ! 
  ! Commercial Edition of ALGLIB includes following important improvements
  ! of this function:
  ! * high-performance native backend with same C# interface (C# version)
  ! * multithreading support (C++ and C# versions)
  ! * hardware vendor (Intel) implementations of linear algebra primitives
  !   (C++ and C# versions, x86/x64 platform)
  ! 
  ! We recommend you to read 'Working with commercial version' section  of
  ! ALGLIB Reference Manual in order to find out how to  use  performance-
  ! related features provided by commercial edition of ALGLIB.

  -- ALGLIB --
     Copyright 24.08.2009 by Bochkanov Sergey
*************************************************************************/
void rmatrixsolvels(/* Real    */ const ae_matrix* a,
     ae_int_t nrows,
     ae_int_t ncols,
     /* Real    */ const ae_vector* b,
     double threshold,
     /* Real    */ ae_vector* x,
     densesolverlsreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_vector sv;
    ae_matrix u;
    ae_matrix vt;
    ae_vector rp;
    ae_vector utb;
    ae_vector sutb;
    ae_vector tmp;
    ae_vector ta;
    ae_vector tx;
    ae_vector buf;
    ae_vector w;
    ae_int_t i;
    ae_int_t j;
    ae_int_t nsv;
    ae_int_t kernelidx;
    double v;
    double verr;
    ae_bool svdfailed;
    ae_bool zeroa;
    ae_int_t rfs;
    ae_int_t nrfs;
    ae_bool terminatenexttime;
    ae_bool smallerr;

    ae_frame_make(_state, &_frame_block);
    memset(&sv, 0, sizeof(sv));
    memset(&u, 0, sizeof(u));
    memset(&vt, 0, sizeof(vt));
    memset(&rp, 0, sizeof(rp));
    memset(&utb, 0, sizeof(utb));
    memset(&sutb, 0, sizeof(sutb));
    memset(&tmp, 0, sizeof(tmp));
    memset(&ta, 0, sizeof(ta));
    memset(&tx, 0, sizeof(tx));
    memset(&buf, 0, sizeof(buf));
    memset(&w, 0, sizeof(w));
    ae_vector_clear(x);
    _densesolverlsreport_clear(rep);
    ae_vector_init(&sv, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&u, 0, 0, DT_REAL, _state, ae_true);
    ae_matrix_init(&vt, 0, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&rp, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&utb, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&sutb, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tmp, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&ta, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tx, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&buf, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&w, 0, DT_REAL, _state, ae_true);

    ae_assert(nrows>0, "RMatrixSolveLS: NRows<=0", _state);
    ae_assert(ncols>0, "RMatrixSolveLS: NCols<=0", _state);
    ae_assert(ae_isfinite(threshold, _state)&&ae_fp_greater_eq(threshold,(double)(0)), "RMatrixSolveLS: Threshold<0 or is infinite", _state);
    ae_assert(a->rows>=nrows, "RMatrixSolveLS: rows(A)<NRows", _state);
    ae_assert(a->cols>=ncols, "RMatrixSolveLS: cols(A)<NCols", _state);
    ae_assert(b->cnt>=nrows, "RMatrixSolveLS: length(B)<NRows", _state);
    ae_assert(apservisfinitematrix(a, nrows, ncols, _state), "RMatrixSolveLS: A contains infinite or NaN values!", _state);
    ae_assert(isfinitevector(b, nrows, _state), "RMatrixSolveLS: B contains infinite or NaN values!", _state);
    if( ae_fp_eq(threshold,(double)(0)) )
    {
        threshold = (double)1000*ae_machineepsilon;
    }
    
    /*
     * Factorize A first
     */
    svdfailed = !rmatrixsvd(a, nrows, ncols, 1, 2, 2, &sv, &u, &vt, _state);
    zeroa = ae_fp_eq(sv.ptr.p_double[0],(double)(0));
    if( svdfailed||zeroa )
    {
        if( svdfailed )
        {
            rep->terminationtype = -4;
        }
        else
        {
            rep->terminationtype = 1;
        }
        ae_vector_set_length(x, ncols, _state);
        for(i=0; i<=ncols-1; i++)
        {
            x->ptr.p_double[i] = (double)(0);
        }
        rep->n = ncols;
        rep->k = ncols;
        ae_matrix_set_length(&rep->cx, ncols, ncols, _state);
        for(i=0; i<=ncols-1; i++)
        {
            for(j=0; j<=ncols-1; j++)
            {
                if( i==j )
                {
                    rep->cx.ptr.pp_double[i][j] = (double)(1);
                }
                else
                {
                    rep->cx.ptr.pp_double[i][j] = (double)(0);
                }
            }
        }
        rep->r2 = (double)(0);
        ae_frame_leave(_state);
        return;
    }
    nsv = ae_minint(ncols, nrows, _state);
    if( nsv==ncols )
    {
        rep->r2 = sv.ptr.p_double[nsv-1]/sv.ptr.p_double[0];
    }
    else
    {
        rep->r2 = (double)(0);
    }
    rep->n = ncols;
    rep->terminationtype = 1;
    
    /*
     * Iterative refinement of xc combined with solution:
     * 1. xc = 0
     * 2. calculate r = bc-A*xc using extra-precise dot product
     * 3. solve A*y = r
     * 4. update x:=x+r
     * 5. goto 2
     *
     * This cycle is executed until one of two things happens:
     * 1. maximum number of iterations reached
     * 2. last iteration decreased error to the lower limit
     */
    ae_vector_set_length(&utb, nsv, _state);
    ae_vector_set_length(&sutb, nsv, _state);
    ae_vector_set_length(x, ncols, _state);
    ae_vector_set_length(&tmp, ncols, _state);
    ae_vector_set_length(&ta, ncols+1, _state);
    ae_vector_set_length(&tx, ncols+1, _state);
    ae_vector_set_length(&buf, ncols+1, _state);
    for(i=0; i<=ncols-1; i++)
    {
        x->ptr.p_double[i] = (double)(0);
    }
    kernelidx = nsv;
    for(i=0; i<=nsv-1; i++)
    {
        if( ae_fp_less_eq(sv.ptr.p_double[i],threshold*sv.ptr.p_double[0]) )
        {
            kernelidx = i;
            break;
        }
    }
    rep->k = ncols-kernelidx;
    nrfs = directdensesolvers_densesolverrfsmaxv2(ncols, rep->r2, _state);
    terminatenexttime = ae_false;
    ae_vector_set_length(&rp, nrows, _state);
    for(rfs=0; rfs<=nrfs; rfs++)
    {
        if( terminatenexttime )
        {
            break;
        }
        
        /*
         * calculate right part
         */
        if( rfs==0 )
        {
            ae_v_move(&rp.ptr.p_double[0], 1, &b->ptr.p_double[0], 1, ae_v_len(0,nrows-1));
        }
        else
        {
            smallerr = ae_true;
            for(i=0; i<=nrows-1; i++)
            {
                ae_v_move(&ta.ptr.p_double[0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,ncols-1));
                ta.ptr.p_double[ncols] = (double)(-1);
                ae_v_move(&tx.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,ncols-1));
                tx.ptr.p_double[ncols] = b->ptr.p_double[i];
                xdot(&ta, &tx, ncols+1, &buf, &v, &verr, _state);
                rp.ptr.p_double[i] = -v;
                smallerr = smallerr&&ae_fp_less(ae_fabs(v, _state),(double)4*verr);
            }
            if( smallerr )
            {
                terminatenexttime = ae_true;
            }
        }
        
        /*
         * solve A*dx = rp
         */
        for(i=0; i<=ncols-1; i++)
        {
            tmp.ptr.p_double[i] = (double)(0);
        }
        for(i=0; i<=nsv-1; i++)
        {
            utb.ptr.p_double[i] = (double)(0);
        }
        for(i=0; i<=nrows-1; i++)
        {
            v = rp.ptr.p_double[i];
            ae_v_addd(&utb.ptr.p_double[0], 1, &u.ptr.pp_double[i][0], 1, ae_v_len(0,nsv-1), v);
        }
        for(i=0; i<=nsv-1; i++)
        {
            if( i<kernelidx )
            {
                sutb.ptr.p_double[i] = utb.ptr.p_double[i]/sv.ptr.p_double[i];
            }
            else
            {
                sutb.ptr.p_double[i] = (double)(0);
            }
        }
        for(i=0; i<=nsv-1; i++)
        {
            v = sutb.ptr.p_double[i];
            ae_v_addd(&tmp.ptr.p_double[0], 1, &vt.ptr.pp_double[i][0], 1, ae_v_len(0,ncols-1), v);
        }
        
        /*
         * update x:  x:=x+dx
         */
        ae_v_add(&x->ptr.p_double[0], 1, &tmp.ptr.p_double[0], 1, ae_v_len(0,ncols-1));
    }
    
    /*
     * fill CX
     */
    if( rep->k>0 )
    {
        ae_matrix_set_length(&rep->cx, ncols, rep->k, _state);
        for(i=0; i<=rep->k-1; i++)
        {
            ae_v_move(&rep->cx.ptr.pp_double[0][i], rep->cx.stride, &vt.ptr.pp_double[kernelidx+i][0], 1, ae_v_len(0,ncols-1));
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal LU solver

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_rmatrixlusolveinternal(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ const ae_matrix* a,
     ae_bool havea,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t rfs;
    ae_int_t nrfs;
    ae_vector xc;
    ae_vector y;
    ae_vector bc;
    ae_vector xa;
    ae_vector xb;
    ae_vector tx;
    double v;
    double verr;
    double mxb;
    ae_bool smallerr;
    ae_bool terminatenexttime;

    ae_frame_make(_state, &_frame_block);
    memset(&xc, 0, sizeof(xc));
    memset(&y, 0, sizeof(y));
    memset(&bc, 0, sizeof(bc));
    memset(&xa, 0, sizeof(xa));
    memset(&xb, 0, sizeof(xb));
    memset(&tx, 0, sizeof(tx));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_vector_init(&xc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&y, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&bc, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xa, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&xb, 0, DT_REAL, _state, ae_true);
    ae_vector_init(&tx, 0, DT_REAL, _state, ae_true);

    
    /*
     * prepare: check inputs, allocate space...
     */
    ae_assert(n>0&&m>0, "RMatrixLUSolveInternal: integrity check 7656 failed", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]<=n-1&&p->ptr.p_int[i]>=i, "RMatrixLUSolveInternal: incorrect pivot, P[i]<i or P[i]>=N", _state);
    }
    ae_matrix_set_length(x, n, m, _state);
    ae_vector_set_length(&y, n, _state);
    ae_vector_set_length(&xc, n, _state);
    ae_vector_set_length(&bc, n, _state);
    ae_vector_set_length(&tx, n+1, _state);
    ae_vector_set_length(&xa, n+1, _state);
    ae_vector_set_length(&xb, n+1, _state);
    
    /*
     * estimate condition number, test for near singularity
     */
    rep->r1 = rmatrixlurcond1(lua, n, _state);
    rep->rinf = rmatrixlurcondinf(lua, n, _state);
    rep->terminationtype = 1;
    if( ae_fp_less(rep->r1,rcondthreshold(_state))||ae_fp_less(rep->rinf,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_double[i][j] = (double)(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * First stage of solution: rough solution with TRSM()
     */
    mxb = 0.0;
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            v = b->ptr.pp_double[i][j];
            mxb = ae_maxreal(mxb, ae_fabs(v, _state), _state);
            x->ptr.pp_double[i][j] = v;
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]!=i )
        {
            for(j=0; j<=m-1; j++)
            {
                v = x->ptr.pp_double[i][j];
                x->ptr.pp_double[i][j] = x->ptr.pp_double[p->ptr.p_int[i]][j];
                x->ptr.pp_double[p->ptr.p_int[i]][j] = v;
            }
        }
    }
    rmatrixlefttrsm(n, m, lua, 0, 0, ae_false, ae_true, 0, x, 0, 0, _state);
    rmatrixlefttrsm(n, m, lua, 0, 0, ae_true, ae_false, 0, x, 0, 0, _state);
    
    /*
     * Second stage: iterative refinement
     */
    if( havea )
    {
        for(k=0; k<=m-1; k++)
        {
            nrfs = directdensesolvers_densesolverrfsmax(n, rep->r1, rep->rinf, _state);
            terminatenexttime = ae_false;
            for(rfs=0; rfs<=nrfs-1; rfs++)
            {
                if( terminatenexttime )
                {
                    break;
                }
                
                /*
                 * generate right part
                 */
                smallerr = ae_true;
                ae_v_move(&xb.ptr.p_double[0], 1, &x->ptr.pp_double[0][k], x->stride, ae_v_len(0,n-1));
                for(i=0; i<=n-1; i++)
                {
                    ae_v_move(&xa.ptr.p_double[0], 1, &a->ptr.pp_double[i][0], 1, ae_v_len(0,n-1));
                    xa.ptr.p_double[n] = (double)(-1);
                    xb.ptr.p_double[n] = b->ptr.pp_double[i][k];
                    xdot(&xa, &xb, n+1, &tx, &v, &verr, _state);
                    y.ptr.p_double[i] = -v;
                    smallerr = smallerr&&ae_fp_less(ae_fabs(v, _state),(double)4*verr);
                }
                if( smallerr )
                {
                    terminatenexttime = ae_true;
                }
                
                /*
                 * solve and update
                 */
                directdensesolvers_rbasiclusolve(lua, p, n, &y, _state);
                ae_v_add(&x->ptr.pp_double[0][k], x->stride, &y.ptr.p_double[0], 1, ae_v_len(0,n-1));
            }
        }
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal Cholesky solver

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_spdmatrixcholeskysolveinternal(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ const ae_matrix* a,
     ae_bool havea,
     /* Real    */ const ae_matrix* b,
     ae_int_t m,
     /* Real    */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;

    ae_matrix_clear(x);
    _densesolverreport_clear(rep);

    
    /*
     * prepare: check inputs, allocate space...
     */
    ae_assert(n>0&&m>0, "SPDMatrixCholeskySolveInternal: integrity check 9858 failed", _state);
    ae_matrix_set_length(x, n, m, _state);
    
    /*
     * estimate condition number, test for near singularity
     */
    rep->r1 = spdmatrixcholeskyrcond(cha, n, isupper, _state);
    rep->rinf = rep->r1;
    rep->terminationtype = 1;
    if( ae_fp_less(rep->r1,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_double[i][j] = (double)(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        return;
    }
    
    /*
     * Solve with TRSM()
     */
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            x->ptr.pp_double[i][j] = b->ptr.pp_double[i][j];
        }
    }
    if( isupper )
    {
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 1, x, 0, 0, _state);
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 0, x, 0, 0, _state);
    }
    else
    {
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 0, x, 0, 0, _state);
        rmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 1, x, 0, 0, _state);
    }
}


/*************************************************************************
Internal LU solver

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_cmatrixlusolveinternal(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ const ae_matrix* a,
     ae_bool havea,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t k;
    ae_int_t rfs;
    ae_int_t nrfs;
    ae_vector xc;
    ae_vector y;
    ae_vector bc;
    ae_vector xa;
    ae_vector xb;
    ae_vector tx;
    ae_vector tmpbuf;
    ae_complex v;
    double verr;
    ae_bool smallerr;
    ae_bool terminatenexttime;

    ae_frame_make(_state, &_frame_block);
    memset(&xc, 0, sizeof(xc));
    memset(&y, 0, sizeof(y));
    memset(&bc, 0, sizeof(bc));
    memset(&xa, 0, sizeof(xa));
    memset(&xb, 0, sizeof(xb));
    memset(&tx, 0, sizeof(tx));
    memset(&tmpbuf, 0, sizeof(tmpbuf));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_vector_init(&xc, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&y, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&bc, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&xa, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&xb, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&tx, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&tmpbuf, 0, DT_REAL, _state, ae_true);

    
    /*
     * prepare: check inputs, allocate space...
     */
    ae_assert(n>0&&m>0, "CMatrixLUSolveInternal: integrity check 9302 failed", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]<=n-1&&p->ptr.p_int[i]>=i, "CMatrixLUSolveInternal: incorrect pivot P[i]<i or P[I]>=N", _state);
    }
    ae_matrix_set_length(x, n, m, _state);
    ae_vector_set_length(&y, n, _state);
    ae_vector_set_length(&xc, n, _state);
    ae_vector_set_length(&bc, n, _state);
    ae_vector_set_length(&tx, n, _state);
    ae_vector_set_length(&xa, n+1, _state);
    ae_vector_set_length(&xb, n+1, _state);
    ae_vector_set_length(&tmpbuf, 2*n+2, _state);
    
    /*
     * estimate condition number, test for near singularity
     */
    rep->r1 = cmatrixlurcond1(lua, n, _state);
    rep->rinf = cmatrixlurcondinf(lua, n, _state);
    rep->terminationtype = 1;
    if( ae_fp_less(rep->r1,rcondthreshold(_state))||ae_fp_less(rep->rinf,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_complex[i][j] = ae_complex_from_i(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * First phase: solve with TRSM()
     */
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            x->ptr.pp_complex[i][j] = b->ptr.pp_complex[i][j];
        }
    }
    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]!=i )
        {
            for(j=0; j<=m-1; j++)
            {
                v = x->ptr.pp_complex[i][j];
                x->ptr.pp_complex[i][j] = x->ptr.pp_complex[p->ptr.p_int[i]][j];
                x->ptr.pp_complex[p->ptr.p_int[i]][j] = v;
            }
        }
    }
    cmatrixlefttrsm(n, m, lua, 0, 0, ae_false, ae_true, 0, x, 0, 0, _state);
    cmatrixlefttrsm(n, m, lua, 0, 0, ae_true, ae_false, 0, x, 0, 0, _state);
    
    /*
     * solve
     */
    for(k=0; k<=m-1; k++)
    {
        ae_v_cmove(&bc.ptr.p_complex[0], 1, &b->ptr.pp_complex[0][k], b->stride, "N", ae_v_len(0,n-1));
        ae_v_cmove(&xc.ptr.p_complex[0], 1, &x->ptr.pp_complex[0][k], x->stride, "N", ae_v_len(0,n-1));
        
        /*
         * Iterative refinement of xc:
         * * calculate r = bc-A*xc using extra-precise dot product
         * * solve A*y = r
         * * update x:=x+r
         *
         * This cycle is executed until one of two things happens:
         * 1. maximum number of iterations reached
         * 2. last iteration decreased error to the lower limit
         */
        if( havea )
        {
            nrfs = directdensesolvers_densesolverrfsmax(n, rep->r1, rep->rinf, _state);
            terminatenexttime = ae_false;
            for(rfs=0; rfs<=nrfs-1; rfs++)
            {
                if( terminatenexttime )
                {
                    break;
                }
                
                /*
                 * generate right part
                 */
                smallerr = ae_true;
                ae_v_cmove(&xb.ptr.p_complex[0], 1, &xc.ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
                for(i=0; i<=n-1; i++)
                {
                    ae_v_cmove(&xa.ptr.p_complex[0], 1, &a->ptr.pp_complex[i][0], 1, "N", ae_v_len(0,n-1));
                    xa.ptr.p_complex[n] = ae_complex_from_i(-1);
                    xb.ptr.p_complex[n] = bc.ptr.p_complex[i];
                    xcdot(&xa, &xb, n+1, &tmpbuf, &v, &verr, _state);
                    y.ptr.p_complex[i] = ae_c_neg(v);
                    smallerr = smallerr&&ae_fp_less(ae_c_abs(v, _state),(double)4*verr);
                }
                if( smallerr )
                {
                    terminatenexttime = ae_true;
                }
                
                /*
                 * solve and update
                 */
                directdensesolvers_cbasiclusolve(lua, p, n, &y, _state);
                ae_v_cadd(&xc.ptr.p_complex[0], 1, &y.ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
            }
        }
        
        /*
         * Store xc.
         * Post-scale result.
         */
        ae_v_cmove(&x->ptr.pp_complex[0][k], x->stride, &xc.ptr.p_complex[0], 1, "N", ae_v_len(0,n-1));
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal Cholesky solver

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_hpdmatrixcholeskysolveinternal(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ const ae_matrix* a,
     ae_bool havea,
     /* Complex */ const ae_matrix* b,
     ae_int_t m,
     /* Complex */ ae_matrix* x,
     densesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_vector xc;
    ae_vector y;
    ae_vector bc;
    ae_vector xa;
    ae_vector xb;
    ae_vector tx;

    ae_frame_make(_state, &_frame_block);
    memset(&xc, 0, sizeof(xc));
    memset(&y, 0, sizeof(y));
    memset(&bc, 0, sizeof(bc));
    memset(&xa, 0, sizeof(xa));
    memset(&xb, 0, sizeof(xb));
    memset(&tx, 0, sizeof(tx));
    ae_matrix_clear(x);
    _densesolverreport_clear(rep);
    ae_vector_init(&xc, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&y, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&bc, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&xa, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&xb, 0, DT_COMPLEX, _state, ae_true);
    ae_vector_init(&tx, 0, DT_COMPLEX, _state, ae_true);

    
    /*
     * prepare: check inputs, allocate space...
     */
    ae_matrix_set_length(x, n, m, _state);
    ae_vector_set_length(&y, n, _state);
    ae_vector_set_length(&xc, n, _state);
    ae_vector_set_length(&bc, n, _state);
    ae_vector_set_length(&tx, n+1, _state);
    ae_vector_set_length(&xa, n+1, _state);
    ae_vector_set_length(&xb, n+1, _state);
    
    /*
     * estimate condition number, test for near singularity
     */
    rep->r1 = hpdmatrixcholeskyrcond(cha, n, isupper, _state);
    rep->rinf = rep->r1;
    rep->terminationtype = 1;
    if( ae_fp_less(rep->r1,rcondthreshold(_state)) )
    {
        for(i=0; i<=n-1; i++)
        {
            for(j=0; j<=m-1; j++)
            {
                x->ptr.pp_complex[i][j] = ae_complex_from_i(0);
            }
        }
        rep->r1 = (double)(0);
        rep->rinf = (double)(0);
        rep->terminationtype = -3;
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * solve
     */
    for(i=0; i<=n-1; i++)
    {
        for(j=0; j<=m-1; j++)
        {
            x->ptr.pp_complex[i][j] = b->ptr.pp_complex[i][j];
        }
    }
    if( isupper )
    {
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 2, x, 0, 0, _state);
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_true, ae_false, 0, x, 0, 0, _state);
    }
    else
    {
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 0, x, 0, 0, _state);
        cmatrixlefttrsm(n, m, cha, 0, 0, ae_false, ae_false, 2, x, 0, 0, _state);
    }
    ae_frame_leave(_state);
}


/*************************************************************************
Internal subroutine.
Returns maximum count of RFS iterations as function of:
1. machine epsilon
2. task size.
3. condition number

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static ae_int_t directdensesolvers_densesolverrfsmax(ae_int_t n,
     double r1,
     double rinf,
     ae_state *_state)
{
    ae_int_t result;


    result = 5;
    return result;
}


/*************************************************************************
Internal subroutine.
Returns maximum count of RFS iterations as function of:
1. machine epsilon
2. task size.
3. norm-2 condition number

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static ae_int_t directdensesolvers_densesolverrfsmaxv2(ae_int_t n,
     double r2,
     ae_state *_state)
{
    ae_int_t result;


    result = directdensesolvers_densesolverrfsmax(n, (double)(0), (double)(0), _state);
    return result;
}


/*************************************************************************
Basic LU solver for PLU*x = y.

This subroutine assumes that:
* A=PLU is well-conditioned, so no zero divisions or overflow may occur

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_rbasiclusolve(/* Real    */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Real    */ ae_vector* xb,
     ae_state *_state)
{
    ae_int_t i;
    double v;


    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]!=i )
        {
            v = xb->ptr.p_double[i];
            xb->ptr.p_double[i] = xb->ptr.p_double[p->ptr.p_int[i]];
            xb->ptr.p_double[p->ptr.p_int[i]] = v;
        }
    }
    for(i=1; i<=n-1; i++)
    {
        v = ae_v_dotproduct(&lua->ptr.pp_double[i][0], 1, &xb->ptr.p_double[0], 1, ae_v_len(0,i-1));
        xb->ptr.p_double[i] = xb->ptr.p_double[i]-v;
    }
    xb->ptr.p_double[n-1] = xb->ptr.p_double[n-1]/lua->ptr.pp_double[n-1][n-1];
    for(i=n-2; i>=0; i--)
    {
        v = ae_v_dotproduct(&lua->ptr.pp_double[i][i+1], 1, &xb->ptr.p_double[i+1], 1, ae_v_len(i+1,n-1));
        xb->ptr.p_double[i] = (xb->ptr.p_double[i]-v)/lua->ptr.pp_double[i][i];
    }
}


/*************************************************************************
Basic Cholesky solver for ScaleA*Cholesky(A)'*x = y.

This subroutine assumes that:
* A*ScaleA is well scaled
* A is well-conditioned, so no zero divisions or overflow may occur

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_spdbasiccholeskysolve(/* Real    */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Real    */ ae_vector* xb,
     ae_state *_state)
{
    ae_int_t i;
    double v;


    
    /*
     * A = L*L' or A=U'*U
     */
    if( isupper )
    {
        
        /*
         * Solve U'*y=b first.
         */
        for(i=0; i<=n-1; i++)
        {
            xb->ptr.p_double[i] = xb->ptr.p_double[i]/cha->ptr.pp_double[i][i];
            if( i<n-1 )
            {
                v = xb->ptr.p_double[i];
                ae_v_subd(&xb->ptr.p_double[i+1], 1, &cha->ptr.pp_double[i][i+1], 1, ae_v_len(i+1,n-1), v);
            }
        }
        
        /*
         * Solve U*x=y then.
         */
        for(i=n-1; i>=0; i--)
        {
            if( i<n-1 )
            {
                v = ae_v_dotproduct(&cha->ptr.pp_double[i][i+1], 1, &xb->ptr.p_double[i+1], 1, ae_v_len(i+1,n-1));
                xb->ptr.p_double[i] = xb->ptr.p_double[i]-v;
            }
            xb->ptr.p_double[i] = xb->ptr.p_double[i]/cha->ptr.pp_double[i][i];
        }
    }
    else
    {
        
        /*
         * Solve L*y=b first
         */
        for(i=0; i<=n-1; i++)
        {
            if( i>0 )
            {
                v = ae_v_dotproduct(&cha->ptr.pp_double[i][0], 1, &xb->ptr.p_double[0], 1, ae_v_len(0,i-1));
                xb->ptr.p_double[i] = xb->ptr.p_double[i]-v;
            }
            xb->ptr.p_double[i] = xb->ptr.p_double[i]/cha->ptr.pp_double[i][i];
        }
        
        /*
         * Solve L'*x=y then.
         */
        for(i=n-1; i>=0; i--)
        {
            xb->ptr.p_double[i] = xb->ptr.p_double[i]/cha->ptr.pp_double[i][i];
            if( i>0 )
            {
                v = xb->ptr.p_double[i];
                ae_v_subd(&xb->ptr.p_double[0], 1, &cha->ptr.pp_double[i][0], 1, ae_v_len(0,i-1), v);
            }
        }
    }
}


/*************************************************************************
Basic LU solver for ScaleA*PLU*x = y.

This subroutine assumes that:
* L is well-scaled, and it is U which needs scaling by ScaleA.
* A=PLU is well-conditioned, so no zero divisions or overflow may occur

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_cbasiclusolve(/* Complex */ const ae_matrix* lua,
     /* Integer */ const ae_vector* p,
     ae_int_t n,
     /* Complex */ ae_vector* xb,
     ae_state *_state)
{
    ae_int_t i;
    ae_complex v;


    for(i=0; i<=n-1; i++)
    {
        if( p->ptr.p_int[i]!=i )
        {
            v = xb->ptr.p_complex[i];
            xb->ptr.p_complex[i] = xb->ptr.p_complex[p->ptr.p_int[i]];
            xb->ptr.p_complex[p->ptr.p_int[i]] = v;
        }
    }
    for(i=1; i<=n-1; i++)
    {
        v = ae_v_cdotproduct(&lua->ptr.pp_complex[i][0], 1, "N", &xb->ptr.p_complex[0], 1, "N", ae_v_len(0,i-1));
        xb->ptr.p_complex[i] = ae_c_sub(xb->ptr.p_complex[i],v);
    }
    xb->ptr.p_complex[n-1] = ae_c_div(xb->ptr.p_complex[n-1],lua->ptr.pp_complex[n-1][n-1]);
    for(i=n-2; i>=0; i--)
    {
        v = ae_v_cdotproduct(&lua->ptr.pp_complex[i][i+1], 1, "N", &xb->ptr.p_complex[i+1], 1, "N", ae_v_len(i+1,n-1));
        xb->ptr.p_complex[i] = ae_c_div(ae_c_sub(xb->ptr.p_complex[i],v),lua->ptr.pp_complex[i][i]);
    }
}


/*************************************************************************
Basic Cholesky solver for ScaleA*Cholesky(A)'*x = y.

This subroutine assumes that:
* A*ScaleA is well scaled
* A is well-conditioned, so no zero divisions or overflow may occur

  -- ALGLIB --
     Copyright 27.01.2010 by Bochkanov Sergey
*************************************************************************/
static void directdensesolvers_hpdbasiccholeskysolve(/* Complex */ const ae_matrix* cha,
     ae_int_t n,
     ae_bool isupper,
     /* Complex */ ae_vector* xb,
     ae_state *_state)
{
    ae_int_t i;
    ae_complex v;


    
    /*
     * A = L*L' or A=U'*U
     */
    if( isupper )
    {
        
        /*
         * Solve U'*y=b first.
         */
        for(i=0; i<=n-1; i++)
        {
            xb->ptr.p_complex[i] = ae_c_div(xb->ptr.p_complex[i],ae_c_conj(cha->ptr.pp_complex[i][i], _state));
            if( i<n-1 )
            {
                v = xb->ptr.p_complex[i];
                ae_v_csubc(&xb->ptr.p_complex[i+1], 1, &cha->ptr.pp_complex[i][i+1], 1, "Conj", ae_v_len(i+1,n-1), v);
            }
        }
        
        /*
         * Solve U*x=y then.
         */
        for(i=n-1; i>=0; i--)
        {
            if( i<n-1 )
            {
                v = ae_v_cdotproduct(&cha->ptr.pp_complex[i][i+1], 1, "N", &xb->ptr.p_complex[i+1], 1, "N", ae_v_len(i+1,n-1));
                xb->ptr.p_complex[i] = ae_c_sub(xb->ptr.p_complex[i],v);
            }
            xb->ptr.p_complex[i] = ae_c_div(xb->ptr.p_complex[i],cha->ptr.pp_complex[i][i]);
        }
    }
    else
    {
        
        /*
         * Solve L*y=b first
         */
        for(i=0; i<=n-1; i++)
        {
            if( i>0 )
            {
                v = ae_v_cdotproduct(&cha->ptr.pp_complex[i][0], 1, "N", &xb->ptr.p_complex[0], 1, "N", ae_v_len(0,i-1));
                xb->ptr.p_complex[i] = ae_c_sub(xb->ptr.p_complex[i],v);
            }
            xb->ptr.p_complex[i] = ae_c_div(xb->ptr.p_complex[i],cha->ptr.pp_complex[i][i]);
        }
        
        /*
         * Solve L'*x=y then.
         */
        for(i=n-1; i>=0; i--)
        {
            xb->ptr.p_complex[i] = ae_c_div(xb->ptr.p_complex[i],ae_c_conj(cha->ptr.pp_complex[i][i], _state));
            if( i>0 )
            {
                v = xb->ptr.p_complex[i];
                ae_v_csubc(&xb->ptr.p_complex[0], 1, &cha->ptr.pp_complex[i][0], 1, "Conj", ae_v_len(0,i-1), v);
            }
        }
    }
}


void _densesolverreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    densesolverreport *p = (densesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _densesolverreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    densesolverreport       *dst = (densesolverreport*)_dst;
    const densesolverreport *src = (const densesolverreport*)_src;
    dst->terminationtype = src->terminationtype;
    dst->r1 = src->r1;
    dst->rinf = src->rinf;
}


void _densesolverreport_clear(void* _p)
{
    densesolverreport *p = (densesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _densesolverreport_destroy(void* _p)
{
    densesolverreport *p = (densesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _densesolverlsreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    densesolverlsreport *p = (densesolverlsreport*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_init(&p->cx, 0, 0, DT_REAL, _state, make_automatic);
}


void _densesolverlsreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    densesolverlsreport       *dst = (densesolverlsreport*)_dst;
    const densesolverlsreport *src = (const densesolverlsreport*)_src;
    dst->terminationtype = src->terminationtype;
    dst->r2 = src->r2;
    ae_matrix_init_copy(&dst->cx, &src->cx, _state, make_automatic);
    dst->n = src->n;
    dst->k = src->k;
}


void _densesolverlsreport_clear(void* _p)
{
    densesolverlsreport *p = (densesolverlsreport*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_clear(&p->cx);
}


void _densesolverlsreport_destroy(void* _p)
{
    densesolverlsreport *p = (densesolverlsreport*)_p;
    ae_touch_ptr((void*)p);
    ae_matrix_destroy(&p->cx);
}


#endif
#if defined(AE_COMPILE_DIRECTSPARSESOLVERS) || !defined(AE_PARTIAL_BUILD)


/*************************************************************************
Sparse linear solver for A*x=b with N*N  sparse  real  symmetric  positive
definite matrix A, N*1 vectors x and b.

This solver  converts  input  matrix  to  SKS  format,  performs  Cholesky
factorization using  SKS  Cholesky  subroutine  (works  well  for  limited
bandwidth matrices) and uses sparse triangular solvers to get solution  of
the original system.

INPUT PARAMETERS
    A       -   sparse matrix, must be NxN exactly
    IsUpper -   which half of A is provided (another half is ignored)
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate or non-SPD system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsespdsolvesks(const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    sparsematrix a2;
    ae_int_t n;

    ae_frame_make(_state, &_frame_block);
    memset(&a2, 0, sizeof(a2));
    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);
    _sparsematrix_init(&a2, _state, ae_true);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseSPDSolveSKS: N<=0", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSPDSolveSKS: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSPDSolveSKS: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSPDSolveSKS: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSPDSolveSKS: B contains infinities or NANs", _state);
    initsparsesolverreport(rep, _state);
    ae_vector_set_length(x, n, _state);
    sparsecopytosks(a, &a2, _state);
    if( !sparsecholeskyskyline(&a2, n, isupper, _state) )
    {
        rep->terminationtype = -3;
        for(i=0; i<=n-1; i++)
        {
            x->ptr.p_double[i] = (double)(0);
        }
        ae_frame_leave(_state);
        return;
    }
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = b->ptr.p_double[i];
    }
    if( isupper )
    {
        sparsetrsv(&a2, isupper, ae_false, 1, x, _state);
        sparsetrsv(&a2, isupper, ae_false, 0, x, _state);
    }
    else
    {
        sparsetrsv(&a2, isupper, ae_false, 0, x, _state);
        sparsetrsv(&a2, isupper, ae_false, 1, x, _state);
    }
    rep->terminationtype = 1;
    ae_frame_leave(_state);
}


/*************************************************************************
Sparse linear solver for A*x=b with N*N  sparse  real  symmetric  positive
definite matrix A, N*1 vectors x and b.

This solver  converts  input  matrix  to  CRS  format,  performs  Cholesky
factorization using supernodal Cholesky  decomposition  with  permutation-
reducing ordering and uses sparse triangular solver to get solution of the
original system.

INPUT PARAMETERS
    A       -   sparse matrix, must be NxN exactly
    IsUpper -   which half of A is provided (another half is ignored)
    B       -   array[N], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate or non-SPD system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsespdsolve(const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    sparsematrix a2;
    ae_int_t n;
    double v;
    ae_vector p;

    ae_frame_make(_state, &_frame_block);
    memset(&a2, 0, sizeof(a2));
    memset(&p, 0, sizeof(p));
    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);
    _sparsematrix_init(&a2, _state, ae_true);
    ae_vector_init(&p, 0, DT_INT, _state, ae_true);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseSPDSolve: N<=0", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSPDSolve: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSPDSolve: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSPDSolve: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSPDSolve: B contains infinities or NANs", _state);
    initsparsesolverreport(rep, _state);
    sparsecopytocrs(a, &a2, _state);
    if( !sparsecholeskyp(&a2, isupper, &p, _state) )
    {
        rep->terminationtype = -3;
        rsetallocv(n, 0.0, x, _state);
        ae_frame_leave(_state);
        return;
    }
    rcopyallocv(n, b, x, _state);
    for(i=0; i<=n-1; i++)
    {
        j = p.ptr.p_int[i];
        v = x->ptr.p_double[i];
        x->ptr.p_double[i] = x->ptr.p_double[j];
        x->ptr.p_double[j] = v;
    }
    if( isupper )
    {
        sparsetrsv(&a2, isupper, ae_false, 1, x, _state);
        sparsetrsv(&a2, isupper, ae_false, 0, x, _state);
    }
    else
    {
        sparsetrsv(&a2, isupper, ae_false, 0, x, _state);
        sparsetrsv(&a2, isupper, ae_false, 1, x, _state);
    }
    for(i=n-1; i>=0; i--)
    {
        j = p.ptr.p_int[i];
        v = x->ptr.p_double[i];
        x->ptr.p_double[i] = x->ptr.p_double[j];
        x->ptr.p_double[j] = v;
    }
    rep->terminationtype = 1;
    ae_frame_leave(_state);
}


/*************************************************************************
Sparse linear solver for A*x=b with N*N real  symmetric  positive definite
matrix A given by its Cholesky decomposition, and N*1 vectors x and b.

IMPORTANT: this solver requires input matrix to be in  the  SKS  (Skyline)
           or CRS (compressed row storage) format. An  exception  will  be
           generated if you pass matrix in some other format.

INPUT PARAMETERS
    A       -   sparse NxN matrix stored in CRs or SKS format, must be NxN
                exactly
    IsUpper -   which half of A is provided (another half is ignored)
    B       -   array[N], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate or non-SPD system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsespdcholeskysolve(const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t n;

    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseSPDCholeskySolve: N<=0", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSPDCholeskySolve: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSPDCholeskySolve: cols(A)!=N", _state);
    ae_assert(sparseissks(a, _state)||sparseiscrs(a, _state), "SparseSPDCholeskySolve: A is not an SKS/CRS matrix", _state);
    ae_assert(b->cnt>=n, "SparseSPDCholeskySolve: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSPDCholeskySolve: B contains infinities or NANs", _state);
    initsparsesolverreport(rep, _state);
    ae_vector_set_length(x, n, _state);
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_eq(sparseget(a, i, i, _state),0.0) )
        {
            rep->terminationtype = -3;
            for(i=0; i<=n-1; i++)
            {
                x->ptr.p_double[i] = (double)(0);
            }
            return;
        }
    }
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = b->ptr.p_double[i];
    }
    if( isupper )
    {
        sparsetrsv(a, isupper, ae_false, 1, x, _state);
        sparsetrsv(a, isupper, ae_false, 0, x, _state);
    }
    else
    {
        sparsetrsv(a, isupper, ae_false, 0, x, _state);
        sparsetrsv(a, isupper, ae_false, 1, x, _state);
    }
    rep->terminationtype = 1;
}


/*************************************************************************
Sparse linear solver for A*x=b with general (nonsymmetric) N*N sparse real
matrix A, N*1 vectors x and b.

This solver converts input matrix to CRS format, performs LU factorization
and uses sparse triangular solvers to get solution of the original system.

INPUT PARAMETERS
    A       -   sparse matrix, must be NxN exactly, any storage format
    N       -   size of A, N>0
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparsesolve(const sparsematrix* a,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t i;
    ae_int_t j;
    ae_int_t n;
    double v;
    sparsematrix a2;
    ae_vector pivp;
    ae_vector pivq;

    ae_frame_make(_state, &_frame_block);
    memset(&a2, 0, sizeof(a2));
    memset(&pivp, 0, sizeof(pivp));
    memset(&pivq, 0, sizeof(pivq));
    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);
    _sparsematrix_init(&a2, _state, ae_true);
    ae_vector_init(&pivp, 0, DT_INT, _state, ae_true);
    ae_vector_init(&pivq, 0, DT_INT, _state, ae_true);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseSolve: N<=0", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSolve: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSolve: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSolve: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSolve: B contains infinities or NANs", _state);
    initsparsesolverreport(rep, _state);
    ae_vector_set_length(x, n, _state);
    sparsecopytocrs(a, &a2, _state);
    if( !sparselu(&a2, 0, &pivp, &pivq, _state) )
    {
        rep->terminationtype = -3;
        for(i=0; i<=n-1; i++)
        {
            x->ptr.p_double[i] = (double)(0);
        }
        ae_frame_leave(_state);
        return;
    }
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = b->ptr.p_double[i];
    }
    for(i=0; i<=n-1; i++)
    {
        j = pivp.ptr.p_int[i];
        v = x->ptr.p_double[i];
        x->ptr.p_double[i] = x->ptr.p_double[j];
        x->ptr.p_double[j] = v;
    }
    sparsetrsv(&a2, ae_false, ae_true, 0, x, _state);
    sparsetrsv(&a2, ae_true, ae_false, 0, x, _state);
    for(i=n-1; i>=0; i--)
    {
        j = pivq.ptr.p_int[i];
        v = x->ptr.p_double[i];
        x->ptr.p_double[i] = x->ptr.p_double[j];
        x->ptr.p_double[j] = v;
    }
    rep->terminationtype = 1;
    ae_frame_leave(_state);
}


/*************************************************************************
Sparse linear solver for A*x=b with general (nonsymmetric) N*N sparse real
matrix A given by its LU factorization, N*1 vectors x and b.

IMPORTANT: this solver requires input matrix  to  be  in  the  CRS  sparse
           storage format. An exception will  be  generated  if  you  pass
           matrix in some other format (HASH or SKS).

INPUT PARAMETERS
    A       -   LU factorization of the sparse matrix, must be NxN exactly
                in CRS storage format
    P, Q    -   pivot indexes from LU factorization
    N       -   size of A, N>0
    B       -   array[0..N-1], right part

OUTPUT PARAMETERS
    X       -   array[N], it contains:
                * rep.terminationtype>0    =>  solution
                * rep.terminationtype=-3   =>  filled by zeros
    Rep     -   solver report, following fields are set:
                * rep.terminationtype - solver status; >0 for success,
                  set to -3 on failure (degenerate system).

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void sparselusolve(const sparsematrix* a,
     /* Integer */ const ae_vector* p,
     /* Integer */ const ae_vector* q,
     /* Real    */ const ae_vector* b,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_int_t i;
    ae_int_t j;
    double v;
    ae_int_t n;

    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);

    n = sparsegetnrows(a, _state);
    ae_assert(n>0, "SparseLUSolve: N<=0", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseLUSolve: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseLUSolve: cols(A)!=N", _state);
    ae_assert(sparseiscrs(a, _state), "SparseLUSolve: A is not an SKS matrix", _state);
    ae_assert(b->cnt>=n, "SparseLUSolve: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseLUSolve: B contains infinities or NANs", _state);
    ae_assert(p->cnt>=n, "SparseLUSolve: length(P)<N", _state);
    ae_assert(q->cnt>=n, "SparseLUSolve: length(Q)<N", _state);
    for(i=0; i<=n-1; i++)
    {
        ae_assert(p->ptr.p_int[i]>=i&&p->ptr.p_int[i]<n, "SparseLUSolve: P is corrupted", _state);
        ae_assert(q->ptr.p_int[i]>=i&&q->ptr.p_int[i]<n, "SparseLUSolve: Q is corrupted", _state);
    }
    initsparsesolverreport(rep, _state);
    ae_vector_set_length(x, n, _state);
    for(i=0; i<=n-1; i++)
    {
        if( a->didx.ptr.p_int[i]==a->uidx.ptr.p_int[i]||a->vals.ptr.p_double[a->didx.ptr.p_int[i]]==0.0 )
        {
            rep->terminationtype = -3;
            for(i=0; i<=n-1; i++)
            {
                x->ptr.p_double[i] = (double)(0);
            }
            return;
        }
    }
    for(i=0; i<=n-1; i++)
    {
        x->ptr.p_double[i] = b->ptr.p_double[i];
    }
    for(i=0; i<=n-1; i++)
    {
        j = p->ptr.p_int[i];
        v = x->ptr.p_double[i];
        x->ptr.p_double[i] = x->ptr.p_double[j];
        x->ptr.p_double[j] = v;
    }
    sparsetrsv(a, ae_false, ae_true, 0, x, _state);
    sparsetrsv(a, ae_true, ae_false, 0, x, _state);
    for(i=n-1; i>=0; i--)
    {
        j = q->ptr.p_int[i];
        v = x->ptr.p_double[i];
        x->ptr.p_double[i] = x->ptr.p_double[j];
        x->ptr.p_double[j] = v;
    }
    rep->terminationtype = 1;
}


/*************************************************************************
Reset report fields

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void initsparsesolverreport(sparsesolverreport* rep, ae_state *_state)
{


    rep->terminationtype = 0;
    rep->nmv = 0;
    rep->iterationscount = 0;
    rep->r2 = (double)(0);
}


void _sparsesolverreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sparsesolverreport *p = (sparsesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _sparsesolverreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sparsesolverreport       *dst = (sparsesolverreport*)_dst;
    const sparsesolverreport *src = (const sparsesolverreport*)_src;
    dst->terminationtype = src->terminationtype;
    dst->nmv = src->nmv;
    dst->iterationscount = src->iterationscount;
    dst->r2 = src->r2;
}


void _sparsesolverreport_clear(void* _p)
{
    sparsesolverreport *p = (sparsesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


void _sparsesolverreport_destroy(void* _p)
{
    sparsesolverreport *p = (sparsesolverreport*)_p;
    ae_touch_ptr((void*)p);
}


#endif
#if defined(AE_COMPILE_ITERATIVESPARSE) || !defined(AE_PARTIAL_BUILD)


/*************************************************************************
Solving sparse symmetric linear system A*x=b using GMRES(k) method. Sparse
symmetric A is given by its lower or upper triangle.

NOTE: use SparseSolveGMRES() to solve system with nonsymmetric A.

This function provides convenience API for an 'expert' interface  provided
by SparseSolverState class. Use SparseSolver  API  if  you  need  advanced
functions like providing initial point, using out-of-core API and so on.

INPUT PARAMETERS:
    A       -   sparse symmetric NxN matrix in any sparse storage  format.
                Using CRS format is recommended because it avoids internal
                conversion.
                An exception will be generated if  A  is  not  NxN  matrix
                (where  N  is  a  size   specified  during  solver  object
                creation).
    IsUpper -   whether upper or lower triangle of A is used:
                * IsUpper=True  => only upper triangle is used and lower
                                   triangle is not referenced at all 
                * IsUpper=False => only lower triangle is used and upper
                                   triangle is not referenced at all
    B       -   right part, array[N]
    K       -   k parameter for  GMRES(k), k>=0.  Zero  value  means  that
                algorithm will choose it automatically.
    EpsF    -   stopping condition, EpsF>=0. The algorithm will stop  when
                residual will decrease below EpsF*|B|. Having EpsF=0 means
                that this stopping condition is ignored.
    MaxIts  -   stopping condition, MaxIts>=0.  The  algorithm  will  stop
                after performing MaxIts iterations. Zero  value  means  no
                limit.

NOTE: having both EpsF=0 and MaxIts=0 means that stopping criteria will be
      chosen automatically.
                
OUTPUT PARAMETERS:
    X       -   array[N], the solution
    Rep     -   solution report:
                * Rep.TerminationType completion code:
                    * -5    CG method was used for a matrix which  is  not
                            positive definite
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                    *  8    the  algorithm  was  terminated   early  with
                            SparseSolverRequestTermination() being called
                            from other thread.
                * Rep.IterationsCount contains iterations count
                * Rep.NMV contains number of matrix-vector calculations
                * Rep.R2 contains squared residual

  -- ALGLIB --
     Copyright 25.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolvesymmetricgmres(const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     ae_int_t k,
     double epsf,
     ae_int_t maxits,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t n;
    sparsematrix convbuf;
    sparsesolverstate solver;

    ae_frame_make(_state, &_frame_block);
    memset(&convbuf, 0, sizeof(convbuf));
    memset(&solver, 0, sizeof(solver));
    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);
    _sparsematrix_init(&convbuf, _state, ae_true);
    _sparsesolverstate_init(&solver, _state, ae_true);

    n = sparsegetnrows(a, _state);
    
    /*
     * Test inputs
     */
    ae_assert(n>=1, "SparseSolveSymmetricGMRES: tried to automatically detect N from sizeof(A), got nonpositive size", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSolveSymmetricGMRES: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSolveSymmetricGMRES: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSolveSymmetricGMRES: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSolveSymmetricGMRES: B contains NAN/INF", _state);
    ae_assert(ae_isfinite(epsf, _state)&&ae_fp_greater_eq(epsf,(double)(0)), "SparseSolveSymmetricGMRES: EpsF<0 or infinite", _state);
    ae_assert(maxits>=0, "SparseSolveSymmetricGMRES: MaxIts<0", _state);
    if( ae_fp_eq(epsf,(double)(0))&&maxits==0 )
    {
        epsf = 1.0E-6;
    }
    
    /*
     * If A is non-CRS, perform conversion
     */
    if( !sparseiscrs(a, _state) )
    {
        sparsecopytocrsbuf(a, &convbuf, _state);
        sparsesolvesymmetricgmres(&convbuf, isupper, b, k, epsf, maxits, x, rep, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Solve using temporary solver object
     */
    sparsesolvercreate(n, &solver, _state);
    sparsesolversetalgogmres(&solver, k, _state);
    sparsesolversetcond(&solver, epsf, maxits, _state);
    sparsesolversolvesymmetric(&solver, a, isupper, b, _state);
    sparsesolverresults(&solver, x, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
Solving sparse linear system A*x=b using GMRES(k) method.

This function provides convenience API for an 'expert' interface  provided
by SparseSolverState class. Use SparseSolver  API  if  you  need  advanced
functions like providing initial point, using out-of-core API and so on.

INPUT PARAMETERS:
    A       -   sparse NxN matrix in any sparse storage format. Using  CRS
                format   is   recommended   because   it  avoids  internal
                conversion.
                An exception will be generated if  A  is  not  NxN  matrix
                (where  N  is  a  size   specified  during  solver  object
                creation).
    B       -   right part, array[N]
    K       -   k parameter for  GMRES(k), k>=0.  Zero  value  means  that
                algorithm will choose it automatically.
    EpsF    -   stopping condition, EpsF>=0. The algorithm will stop  when
                residual will decrease below EpsF*|B|. Having EpsF=0 means
                that this stopping condition is ignored.
    MaxIts  -   stopping condition, MaxIts>=0.  The  algorithm  will  stop
                after performing MaxIts iterations. Zero  value  means  no
                limit.

NOTE: having both EpsF=0 and MaxIts=0 means that stopping criteria will be
      chosen automatically.
                
OUTPUT PARAMETERS:
    X       -   array[N], the solution
    Rep     -   solution report:
                * Rep.TerminationType completion code:
                    * -5    CG method was used for a matrix which  is  not
                            positive definite
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                    *  8    the  algorithm  was  terminated   early  with
                            SparseSolverRequestTermination() being called
                            from other thread.
                * Rep.IterationsCount contains iterations count
                * Rep.NMV contains number of matrix-vector calculations
                * Rep.R2 contains squared residual

  -- ALGLIB --
     Copyright 25.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolvegmres(const sparsematrix* a,
     /* Real    */ const ae_vector* b,
     ae_int_t k,
     double epsf,
     ae_int_t maxits,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{
    ae_frame _frame_block;
    ae_int_t n;
    sparsematrix convbuf;
    sparsesolverstate solver;

    ae_frame_make(_state, &_frame_block);
    memset(&convbuf, 0, sizeof(convbuf));
    memset(&solver, 0, sizeof(solver));
    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);
    _sparsematrix_init(&convbuf, _state, ae_true);
    _sparsesolverstate_init(&solver, _state, ae_true);

    n = sparsegetnrows(a, _state);
    
    /*
     * Test inputs
     */
    ae_assert(n>=1, "SparseSolveGMRES: tried to automatically detect N from sizeof(A), got nonpositive size", _state);
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSolveGMRES: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSolveGMRES: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSolveGMRES: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSolveGMRES: B contains NAN/INF", _state);
    ae_assert(ae_isfinite(epsf, _state)&&ae_fp_greater_eq(epsf,(double)(0)), "SparseSolveGMRES: EpsF<0 or infinite", _state);
    ae_assert(maxits>=0, "SparseSolveGMRES: MaxIts<0", _state);
    if( ae_fp_eq(epsf,(double)(0))&&maxits==0 )
    {
        epsf = 1.0E-6;
    }
    
    /*
     * If A is non-CRS, perform conversion
     */
    if( !sparseiscrs(a, _state) )
    {
        sparsecopytocrsbuf(a, &convbuf, _state);
        sparsesolvegmres(&convbuf, b, k, epsf, maxits, x, rep, _state);
        ae_frame_leave(_state);
        return;
    }
    
    /*
     * Solve using temporary solver object
     */
    sparsesolvercreate(n, &solver, _state);
    sparsesolversetalgogmres(&solver, k, _state);
    sparsesolversetcond(&solver, epsf, maxits, _state);
    sparsesolversolve(&solver, a, b, _state);
    sparsesolverresults(&solver, x, rep, _state);
    ae_frame_leave(_state);
}


/*************************************************************************
This function initializes sparse linear iterative solver object.

This solver can be used  to  solve  nonsymmetric  and  symmetric  positive
definite NxN (square) linear systems.

The solver provides  'expert'  API  which  allows  advanced  control  over
algorithms being used, including ability to get progress report, terminate
long-running solver from other thread, out-of-core solution and so on.

NOTE: there are also convenience  functions  that  allows  quick  one-line
      access to the solvers:
      * SparseSolveCG() to solve SPD linear systems
      * SparseSolveGMRES() to solve unsymmetric linear systems.

NOTE: if you want to solve MxN (rectangular) linear problem  you  may  use
      LinLSQR solver provided by ALGLIB.

USAGE (A is given by the SparseMatrix structure):

    1. User initializes algorithm state with SparseSolverCreate() call
    2. User  selects   algorithm  with one of the SparseSolverSetAlgo???()
       functions. By default, GMRES(k) is used with automatically chosen k
    3. Optionally, user tunes solver parameters, sets starting point, etc.
    4. Depending on whether system is symmetric or not, user calls:
       * SparseSolverSolveSymmetric() for a  symmetric system given by its
         lower or upper triangle
       * SparseSolverSolve() for a nonsymmetric system or a symmetric  one
         given by the full matrix
    5. User calls SparseSolverResults() to get the solution

    It is possible to call SparseSolverSolve???() again to  solve  another
    task with same dimensionality but different matrix and/or  right  part
    without reinitializing SparseSolverState structure.
  
USAGE (out-of-core mode):

    1. User initializes algorithm state with SparseSolverCreate() call
    2. User  selects   algorithm  with one of the SparseSolverSetAlgo???()
       functions. By default, GMRES(k) is used with automatically chosen k
    3. Optionally, user tunes solver parameters, sets starting point, etc.
    4. After that user should work with out-of-core interface  in  a  loop
       like one given below:
       
        > alglib.sparsesolveroocstart(state)
        > while alglib.sparsesolverooccontinue(state) do
        >     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
        >     alglib.sparsesolveroocgetrequestdata(state, out X)
        >     if RequestType=0 then
        >         [calculate  Y=A*X, with X=R^N]
        >     alglib.sparsesolveroocsendresult(state, in Y)
        > alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    N       -   problem dimensionality (fixed at start-up)

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolvercreate(ae_int_t n,
     sparsesolverstate* state,
     ae_state *_state)
{

    _sparsesolverstate_clear(state);

    ae_assert(n>=1, "SparseSolverCreate: N<=0", _state);
    state->n = n;
    state->running = ae_false;
    state->userterminationneeded = ae_false;
    rsetallocv(state->n, 0.0, &state->x0, _state);
    rsetallocv(state->n, 0.0, &state->x, _state);
    rsetallocv(state->n, 0.0, &state->ax, _state);
    rsetallocv(state->n, 0.0, &state->xf, _state);
    rsetallocv(state->n, 0.0, &state->b, _state);
    rsetallocv(state->n, 0.0, &state->wrkb, _state);
    state->reply1 = 0.0;
    sparsesolversetxrep(state, ae_false, _state);
    sparsesolversetcond(state, 0.0, 0, _state);
    sparsesolversetalgogmres(state, 0, _state);
    iterativesparse_clearrequestfields(state, _state);
    iterativesparse_clearreportfields(state, _state);
}


/*************************************************************************
This function sets the solver algorithm to GMRES(k).

NOTE: if you do not need advanced functionality of the  SparseSolver  API,
      you   may   use   convenience   functions   SparseSolveGMRES()   and
      SparseSolveSymmetricGMRES().

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    K       -   GMRES parameter, K>=0:
                * recommended values are in 10..100 range
                * larger values up to N are possible but have little sense
                  - the algorithm will be slower than any dense solver.
                * values above N are truncated down to N
                * zero value means that  default  value  is  chosen.  This
                  value is 50 in the current version, but  it  may  change
                  in future ALGLIB releases.

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversetalgogmres(sparsesolverstate* state,
     ae_int_t k,
     ae_state *_state)
{


    ae_assert(k>=0, "SparseSolverSetAlgoGMRESK: K<0", _state);
    state->algotype = 0;
    if( k==0 )
    {
        k = 50;
    }
    state->gmresk = ae_minint(k, state->n, _state);
}


/*************************************************************************
This function sets starting point.
By default, zero starting point is used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    X       -   starting point, array[N]

OUTPUT PARAMETERS:
    State   -   new starting point was set

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversetstartingpoint(sparsesolverstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{


    ae_assert(state->n<=x->cnt, "SparseSolverSetStartingPoint: Length(X)<N", _state);
    ae_assert(isfinitevector(x, state->n, _state), "SparseSolverSetStartingPoint: X contains infinite or NaN values!", _state);
    rcopyv(state->n, x, &state->x0, _state);
}


/*************************************************************************
This function sets stopping criteria.

INPUT PARAMETERS:
    EpsF    -   algorithm will be stopped if norm of residual is less than 
                EpsF*||b||.
    MaxIts  -   algorithm will be stopped if number of iterations is  more 
                than MaxIts.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
If  both  EpsF  and  MaxIts  are  zero then small EpsF will be set to small 
value.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void sparsesolversetcond(sparsesolverstate* state,
     double epsf,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsf, _state)&&ae_fp_greater_eq(epsf,(double)(0)), "SparseSolverSetCond: EpsF is negative or contains infinite or NaN values", _state);
    ae_assert(maxits>=0, "SparseSolverSetCond: MaxIts is negative", _state);
    if( ae_fp_eq(epsf,(double)(0))&&maxits==0 )
    {
        state->epsf = 1.0E-6;
        state->maxits = 0;
    }
    else
    {
        state->epsf = epsf;
        state->maxits = maxits;
    }
}


/*************************************************************************
Procedure for  the  solution of A*x=b with sparse symmetric A given by its
lower or upper triangle.

This function will work with any solver algorithm  being   used,  SPD  one
(like CG) or not (like GMRES). Using unsymmetric solvers (like  GMRES)  on
SPD problems is suboptimal, but still possible.

NOTE: the  solver  behavior is ill-defined  for  a  situation  when a  SPD
      solver is used on indefinite matrix. It  may solve the problem up to
      desired precision (sometimes, rarely)  or  return  with  error  code
      signalling violation of underlying assumptions.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse symmetric NxN matrix in any sparse storage  format.
                Using CRS format is recommended because it avoids internal
                conversion.
                An exception will be generated if  A  is  not  NxN  matrix
                (where  N  is  a  size   specified  during  solver  object
                creation).
    IsUpper -   whether upper or lower triangle of A is used:
                * IsUpper=True  => only upper triangle is used and lower
                                   triangle is not referenced at all 
                * IsUpper=False => only lower triangle is used and upper
                                   triangle is not referenced at all
    B       -   right part, array[N]

RESULT:
    This function returns no result.
    You can get the solution by calling SparseSolverResults()

  -- ALGLIB --
     Copyright 25.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversolvesymmetric(sparsesolverstate* state,
     const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    
    /*
     * Test inputs
     */
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSolverSolveSymmetric: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSolverSolveSymmetric: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSolverSolveSymmetric: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSolverSolveSymmetric: B contains NAN/INF", _state);
    
    /*
     * If A is non-CRS, perform conversion
     */
    if( !sparseiscrs(a, _state) )
    {
        sparsecopytocrsbuf(a, &state->convbuf, _state);
        sparsesolversolvesymmetric(state, &state->convbuf, isupper, b, _state);
        return;
    }
    
    /*
     * Solve using out-of-core API
     */
    sparsesolveroocstart(state, b, _state);
    while(sparsesolverooccontinue(state, _state))
    {
        if( state->requesttype==-1 )
        {
            
            /*
             * Skip location reports
             */
            continue;
        }
        ae_assert(state->requesttype==0, "SparseSolverSolveSymmetric: integrity check 7372 failed", _state);
        sparsesmv(a, isupper, &state->x, &state->ax, _state);
    }
}


/*************************************************************************
Procedure for the solution of A*x=b with sparse nonsymmetric A

IMPORTANT: this function will work with any solver algorithm  being  used,
           symmetric solver like CG,  or  not.  However,  using  symmetric
           solvers on nonsymmetric problems is  dangerous.  It  may  solve
           the problem up  to  desired  precision  (sometimes,  rarely) or
           terminate with error code signalling  violation  of  underlying
           assumptions.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse NxN matrix in any sparse storage  format.
                Using CRS format is recommended because it avoids internal
                conversion.
                An exception will be generated if  A  is  not  NxN  matrix
                (where  N  is  a  size   specified  during  solver  object
                creation).
    B       -   right part, array[N]

RESULT:
    This function returns no result.
    You can get the solution by calling SparseSolverResults()

  -- ALGLIB --
     Copyright 25.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversolve(sparsesolverstate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* b,
     ae_state *_state)
{
    ae_int_t n;


    n = state->n;
    
    /*
     * Test inputs
     */
    ae_assert(sparsegetnrows(a, _state)==n, "SparseSolverSolve: rows(A)!=N", _state);
    ae_assert(sparsegetncols(a, _state)==n, "SparseSolverSolve: cols(A)!=N", _state);
    ae_assert(b->cnt>=n, "SparseSolverSolve: length(B)<N", _state);
    ae_assert(isfinitevector(b, n, _state), "SparseSolverSolve: B contains NAN/INF", _state);
    
    /*
     * If A is non-CRS, perform conversion
     */
    if( !sparseiscrs(a, _state) )
    {
        sparsecopytocrsbuf(a, &state->convbuf, _state);
        sparsesolversolve(state, &state->convbuf, b, _state);
        return;
    }
    
    /*
     * Solve using out-of-core API
     */
    sparsesolveroocstart(state, b, _state);
    while(sparsesolverooccontinue(state, _state))
    {
        if( state->requesttype==-1 )
        {
            
            /*
             * Skip location reports
             */
            continue;
        }
        ae_assert(state->requesttype==0, "SparseSolverSolve: integrity check 7372 failed", _state);
        sparsemv(a, &state->x, &state->ax, _state);
    }
}


/*************************************************************************
Sparse solver results.

This function must be called after calling one of the SparseSolverSolve()
functions.

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution
    Rep     -   solution report:
                * Rep.TerminationType completion code:
                    * -5    CG method was used for a matrix which  is  not
                            positive definite
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                    *  8    the  algorithm  was  terminated   early  with
                            SparseSolverRequestTermination() being called
                            from other thread.
                * Rep.IterationsCount contains iterations count
                * Rep.NMV contains number of matrix-vector calculations
                * Rep.R2 contains squared residual
s
  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void sparsesolverresults(sparsesolverstate* state,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);

    sparsesolveroocstop(state, x, rep, _state);
}


/*************************************************************************
This function turns on/off reporting during out-of-core processing.

When the solver works in the out-of-core mode, it  can  be  configured  to
report its progress by returning current location. These location  reports
are implemented as a special kind of the out-of-core request:
* SparseSolverOOCGetRequestInfo() returns -1
* SparseSolverOOCGetRequestData() returns current location
* SparseSolverOOCGetRequestData1() returns squared norm of the residual
* SparseSolverOOCSendResult() shall NOT be called

This function has no effect when SparseSolverSolve() is used because  this
function has no method of reporting its progress.

NOTE: when used with GMRES(k), this function reports progress  every  k-th
      iteration.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

  -- ALGLIB --
     Copyright 01.10.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolversetxrep(sparsesolverstate* state,
     ae_bool needxrep,
     ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
This function initiates out-of-core mode of the sparse solver.  It  should
be used in conjunction with other out-of-core-related  functions  of  this
subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State       -   solver object

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocstart(sparsesolverstate* state,
     /* Real    */ const ae_vector* b,
     ae_state *_state)
{


    ae_vector_set_length(&state->rstate.ia, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 2+1, _state);
    state->rstate.stage = -1;
    iterativesparse_clearrequestfields(state, _state);
    iterativesparse_clearreportfields(state, _state);
    state->running = ae_true;
    state->userterminationneeded = ae_false;
    rcopyv(state->n, b, &state->b, _state);
}


/*************************************************************************
This function performs iterative solution of  the  linear  system  in  the
out-of-core mode. It should be used in conjunction with other out-of-core-
related functions of this subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
ae_bool sparsesolverooccontinue(sparsesolverstate* state,
     ae_state *_state)
{
    ae_bool result;


    ae_assert(state->running, "SparseSolverContinue: the solver is not running", _state);
    result = iterativesparse_sparsesolveriteration(state, _state);
    state->running = result;
    return result;
}


/*************************************************************************
This function is used to retrieve information  about  out-of-core  request
sent by the solver:
* RequestType=0  means that matrix-vector products A*x is requested
* RequestType=-1 means that solver reports its progress; this  request  is
  returned only when reports are activated wit SparseSolverSetXRep().

This function returns just request type; in order  to  get contents of the
trial vector, use sparsesolveroocgetrequestdata().

It should be used in conjunction with other out-of-core-related  functions
of this subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode
    
OUTPUT PARAMETERS:
    RequestType     -   type of the request to process:
                        * 0   for matrix-vector product A*x, with A  being
                          NxN system matrix  and X being N-dimensional
                          vector
                        *-1   for location and residual report


  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocgetrequestinfo(sparsesolverstate* state,
     ae_int_t* requesttype,
     ae_state *_state)
{

    *requesttype = 0;

    ae_assert(state->running, "SparseSolverOOCGetRequestInfo: the solver is not running", _state);
    *requesttype = state->requesttype;
}


/*************************************************************************
This function is used  to  retrieve  vector  associated  with  out-of-core
request sent by the solver to user code. Depending  on  the  request  type
(returned by the SparseSolverOOCGetRequestInfo()) this  vector  should  be
multiplied by A or subjected to another processing.

It should be used in conjunction with other out-of-core-related  functions
of this subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode
    X               -   possibly  preallocated   storage;  reallocated  if
                        needed, left unchanged, if large enough  to  store
                        request data.
    
OUTPUT PARAMETERS:
    X               -   array[N] or larger, leading N elements are  filled
                        with vector X.


  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocgetrequestdata(sparsesolverstate* state,
     /* Real    */ ae_vector* x,
     ae_state *_state)
{


    ae_assert(state->running, "SparseSolverOOCGetRequestInfo: the solver is not running", _state);
    rcopyallocv(state->n, &state->x, x, _state);
}


/*************************************************************************
This function is used to retrieve scalar value associated with out-of-core
request sent by the solver to user code. In  the  current  ALGLIB  version
this function is used to retrieve squared residual  norm  during  progress
reports.

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode
    
OUTPUT PARAMETERS:
    V               -   scalar value associated with the current request


  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocgetrequestdata1(sparsesolverstate* state,
     double* v,
     ae_state *_state)
{

    *v = 0.0;

    ae_assert(state->running, "SparseSolverOOCGetRequestInfo: the solver is not running", _state);
    *v = state->reply1;
}


/*************************************************************************
This function is used to send user reply to out-of-core  request  sent  by
the solver. Usually it is product A*x for vector X returned by the solver.

It should be used in conjunction with other out-of-core-related  functions
of this subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State           -   solver running in out-of-core mode
    AX              -   array[N] or larger, leading N elements contain A*x


  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocsendresult(sparsesolverstate* state,
     /* Real    */ const ae_vector* ax,
     ae_state *_state)
{


    ae_assert(state->running, "SparseSolverOOCSendResult: the solver is not running", _state);
    ae_assert(state->requesttype==0, "SparseSolverOOCSendResult: this request type does not accept replies", _state);
    rcopyv(state->n, ax, &state->ax, _state);
}


/*************************************************************************
This  function  finalizes out-of-core mode of the linear solver. It should
be used in conjunction with other out-of-core-related  functions  of  this
subspackage in a loop like one given below:

> alglib.sparsesolveroocstart(state)
> while alglib.sparsesolverooccontinue(state) do
>     alglib.sparsesolveroocgetrequestinfo(state, out RequestType)
>     alglib.sparsesolveroocgetrequestdata(state, out X)
>     if RequestType=0 then
>         [calculate  Y=A*X, with X=R^N]
>     alglib.sparsesolveroocsendresult(state, in Y)
> alglib.sparsesolveroocstop(state, out X, out Report)

INPUT PARAMETERS:
    State       -   solver state
    
OUTPUT PARAMETERS:
    X       -   array[N], the solution.
                Zero-filled on the failure (Rep.TerminationType<0).
    Rep     -   report with additional info:
                * Rep.TerminationType completion code:
                    * -5    CG method was used for a matrix which  is  not
                            positive definite
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                    *  8    the  algorithm  was  terminated   early  with
                            SparseSolverRequestTermination() being called
                            from other thread.
                * Rep.IterationsCount contains iterations count
                * Rep.NMV contains number of matrix-vector calculations
                * Rep.R2 contains squared residual

  -- ALGLIB --
     Copyright 24.09.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolveroocstop(sparsesolverstate* state,
     /* Real    */ ae_vector* x,
     sparsesolverreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _sparsesolverreport_clear(rep);

    ae_assert(!state->running, "SparseSolverOOCStop: the solver is still running", _state);
    ae_vector_set_length(x, state->n, _state);
    rcopyv(state->n, &state->xf, x, _state);
    initsparsesolverreport(rep, _state);
    rep->iterationscount = state->repiterationscount;
    rep->nmv = state->repnmv;
    rep->terminationtype = state->repterminationtype;
    rep->r2 = state->repr2;
}


/*************************************************************************
This subroutine submits request for termination of the running solver.  It
can be called from some other thread which wants the   solver to terminate
or when processing an out-of-core request.

As result, solver  stops  at  point  which  was  "current  accepted"  when
the termination request was submitted and returns error code 8 (successful
termination).  Such   termination   is  a smooth  process  which  properly
deallocates all temporaries.

INPUT PARAMETERS:
    State   -   solver structure

NOTE: calling this function on solver which is NOT running  will  have  no
      effect.
      
NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

NOTE: solver clears termination flag on its start, it means that  if  some
      other thread will request termination too soon, its request will went
      unnoticed.

  -- ALGLIB --
     Copyright 01.10.2021 by Bochkanov Sergey
*************************************************************************/
void sparsesolverrequesttermination(sparsesolverstate* state,
     ae_state *_state)
{


    state->userterminationneeded = ae_true;
}


/*************************************************************************
Reverse communication sparse iteration subroutine

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
static ae_bool iterativesparse_sparsesolveriteration(sparsesolverstate* state,
     ae_state *_state)
{
    ae_int_t outeridx;
    double res;
    double prevres;
    double res0;
    ae_bool result;


    
    /*
     * Reverse communication preparations
     * I know it looks ugly, but it works the same way
     * anywhere from C++ to Python.
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if( state->rstate.stage>=0 )
    {
        outeridx = state->rstate.ia.ptr.p_int[0];
        res = state->rstate.ra.ptr.p_double[0];
        prevres = state->rstate.ra.ptr.p_double[1];
        res0 = state->rstate.ra.ptr.p_double[2];
    }
    else
    {
        outeridx = 359;
        res = -58.0;
        prevres = -919.0;
        res0 = -909.0;
    }
    if( state->rstate.stage==0 )
    {
        goto lbl_0;
    }
    if( state->rstate.stage==1 )
    {
        goto lbl_1;
    }
    if( state->rstate.stage==2 )
    {
        goto lbl_2;
    }
    if( state->rstate.stage==3 )
    {
        goto lbl_3;
    }
    if( state->rstate.stage==4 )
    {
        goto lbl_4;
    }
    
    /*
     * Routine body
     */
    state->running = ae_true;
    iterativesparse_clearrequestfields(state, _state);
    iterativesparse_clearreportfields(state, _state);
    
    /*
     * GMRES?
     */
    if( state->algotype!=0 )
    {
        goto lbl_5;
    }
    if( ae_fp_neq(rdotv2(state->n, &state->x0, _state),(double)(0)) )
    {
        goto lbl_7;
    }
    
    /*
     * Starting point is default one (zero), quick initialization
     */
    rsetv(state->n, 0.0, &state->xf, _state);
    rcopyv(state->n, &state->b, &state->wrkb, _state);
    goto lbl_8;
lbl_7:
    
    /*
     * Non-zero starting point is provided, 
     */
    rcopyv(state->n, &state->x0, &state->xf, _state);
    state->requesttype = 0;
    rcopyv(state->n, &state->x0, &state->x, _state);
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->requesttype = -999;
    state->repnmv = state->repnmv+1;
    rcopyv(state->n, &state->b, &state->wrkb, _state);
    raddv(state->n, -1.0, &state->ax, &state->wrkb, _state);
lbl_8:
    outeridx = 0;
    state->repterminationtype = 5;
    state->repr2 = rdotv2(state->n, &state->wrkb, _state);
    res0 = ae_sqrt(rdotv2(state->n, &state->b, _state), _state);
    res = ae_sqrt(state->repr2, _state);
    if( !state->xrep )
    {
        goto lbl_9;
    }
    
    /*
     * Report initial point
     */
    state->requesttype = -1;
    state->reply1 = res*res;
    rcopyv(state->n, &state->xf, &state->x, _state);
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->requesttype = -999;
lbl_9:
lbl_11:
    if( !(ae_fp_greater(res,(double)(0))&&(state->maxits==0||state->repiterationscount<state->maxits)) )
    {
        goto lbl_12;
    }
    
    /*
     * Solve with GMRES(k) for current residual.
     *
     * We set EpsF-based stopping condition for GMRES(k). It allows us
     * to quickly detect sufficient decrease in the residual. We still
     * have to recompute residual after the GMRES round because residuals
     * computed by GMRES are different from the true one (due to restarts).
     *
     * However, checking residual decrease within GMRES still gives us
     * an opportunity to stop early without waiting for GMRES round to
     * complete.
     */
    fblsgmrescreate(&state->wrkb, state->n, state->gmresk, &state->gmressolver, _state);
    state->gmressolver.epsres = state->epsf*res0/res;
lbl_13:
    if( !fblsgmresiteration(&state->gmressolver, _state) )
    {
        goto lbl_14;
    }
    state->requesttype = 0;
    rcopyv(state->n, &state->gmressolver.x, &state->x, _state);
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->requesttype = -999;
    rcopyv(state->n, &state->ax, &state->gmressolver.ax, _state);
    state->repnmv = state->repnmv+1;
    if( state->userterminationneeded )
    {
        
        /*
         * User requested termination
         */
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    goto lbl_13;
lbl_14:
    state->repiterationscount = state->repiterationscount+state->gmressolver.itsperformed;
    raddv(state->n, 1.0, &state->gmressolver.xs, &state->xf, _state);
    
    /*
     * Update residual, evaluate residual decrease, terminate if needed
     */
    state->requesttype = 0;
    rcopyv(state->n, &state->xf, &state->x, _state);
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->requesttype = -999;
    state->repnmv = state->repnmv+1;
    rcopyv(state->n, &state->b, &state->wrkb, _state);
    raddv(state->n, -1.0, &state->ax, &state->wrkb, _state);
    state->repr2 = rdotv2(state->n, &state->wrkb, _state);
    prevres = res;
    res = ae_sqrt(state->repr2, _state);
    if( !state->xrep )
    {
        goto lbl_15;
    }
    
    /*
     * Report initial point
     */
    state->requesttype = -1;
    state->reply1 = res*res;
    rcopyv(state->n, &state->xf, &state->x, _state);
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->requesttype = -999;
lbl_15:
    if( ae_fp_less_eq(res,state->epsf*res0) )
    {
        
        /*
         * Residual decrease condition met, stopping
         */
        state->repterminationtype = 1;
        goto lbl_12;
    }
    if( ae_fp_greater_eq(res,prevres*((double)1-ae_sqrt(ae_machineepsilon, _state))) )
    {
        
        /*
         * The algorithm stagnated
         */
        state->repterminationtype = 7;
        goto lbl_12;
    }
    if( state->userterminationneeded )
    {
        
        /*
         * User requested termination
         */
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    outeridx = outeridx+1;
    goto lbl_11;
lbl_12:
    result = ae_false;
    return result;
lbl_5:
    ae_assert(ae_false, "SparseSolverIteration: integrity check failed (unexpected algo)", _state);
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = outeridx;
    state->rstate.ra.ptr.p_double[0] = res;
    state->rstate.ra.ptr.p_double[1] = prevres;
    state->rstate.ra.ptr.p_double[2] = res0;
    return result;
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void iterativesparse_clearrequestfields(sparsesolverstate* state,
     ae_state *_state)
{


    state->requesttype = -999;
}


/*************************************************************************
Clears report fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void iterativesparse_clearreportfields(sparsesolverstate* state,
     ae_state *_state)
{


    state->repiterationscount = 0;
    state->repnmv = 0;
    state->repterminationtype = 0;
    state->repr2 = (double)(0);
}


void _sparsesolverstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    sparsesolverstate *p = (sparsesolverstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x0, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->b, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->xf, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ax, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->wrkb, 0, DT_REAL, _state, make_automatic);
    _sparsematrix_init(&p->convbuf, _state, make_automatic);
    _fblsgmresstate_init(&p->gmressolver, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
}


void _sparsesolverstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    sparsesolverstate       *dst = (sparsesolverstate*)_dst;
    const sparsesolverstate *src = (const sparsesolverstate*)_src;
    dst->n = src->n;
    ae_vector_init_copy(&dst->x0, &src->x0, _state, make_automatic);
    dst->epsf = src->epsf;
    dst->maxits = src->maxits;
    dst->algotype = src->algotype;
    dst->gmresk = src->gmresk;
    dst->xrep = src->xrep;
    dst->running = src->running;
    dst->userterminationneeded = src->userterminationneeded;
    ae_vector_init_copy(&dst->b, &src->b, _state, make_automatic);
    ae_vector_init_copy(&dst->xf, &src->xf, _state, make_automatic);
    dst->repiterationscount = src->repiterationscount;
    dst->repnmv = src->repnmv;
    dst->repterminationtype = src->repterminationtype;
    dst->repr2 = src->repr2;
    dst->requesttype = src->requesttype;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->ax, &src->ax, _state, make_automatic);
    dst->reply1 = src->reply1;
    ae_vector_init_copy(&dst->wrkb, &src->wrkb, _state, make_automatic);
    _sparsematrix_init_copy(&dst->convbuf, &src->convbuf, _state, make_automatic);
    _fblsgmresstate_init_copy(&dst->gmressolver, &src->gmressolver, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
}


void _sparsesolverstate_clear(void* _p)
{
    sparsesolverstate *p = (sparsesolverstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x0);
    ae_vector_clear(&p->b);
    ae_vector_clear(&p->xf);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->ax);
    ae_vector_clear(&p->wrkb);
    _sparsematrix_clear(&p->convbuf);
    _fblsgmresstate_clear(&p->gmressolver);
    _rcommstate_clear(&p->rstate);
}


void _sparsesolverstate_destroy(void* _p)
{
    sparsesolverstate *p = (sparsesolverstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x0);
    ae_vector_destroy(&p->b);
    ae_vector_destroy(&p->xf);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->ax);
    ae_vector_destroy(&p->wrkb);
    _sparsematrix_destroy(&p->convbuf);
    _fblsgmresstate_destroy(&p->gmressolver);
    _rcommstate_destroy(&p->rstate);
}


#endif
#if defined(AE_COMPILE_LINCG) || !defined(AE_PARTIAL_BUILD)


/*************************************************************************
This function initializes linear CG Solver. This solver is used  to  solve
symmetric positive definite problems. If you want  to  solve  nonsymmetric
(or non-positive definite) problem you may use LinLSQR solver provided  by
ALGLIB.

USAGE:
1. User initializes algorithm state with LinCGCreate() call
2. User tunes solver parameters with  LinCGSetCond() and other functions
3. Optionally, user sets starting point with LinCGSetStartingPoint()
4. User  calls LinCGSolveSparse() function which takes algorithm state and
   SparseMatrix object.
5. User calls LinCGResults() to get solution
6. Optionally, user may call LinCGSolveSparse()  again  to  solve  another
   problem  with different matrix and/or right part without reinitializing
   LinCGState structure.
  
INPUT PARAMETERS:
    N       -   problem dimension, N>0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgcreate(ae_int_t n, lincgstate* state, ae_state *_state)
{
    ae_int_t i;

    _lincgstate_clear(state);

    ae_assert(n>0, "LinCGCreate: N<=0", _state);
    state->n = n;
    state->prectype = 0;
    state->itsbeforerestart = n;
    state->itsbeforerupdate = 10;
    state->epsf = lincg_defaultprecision;
    state->maxits = 0;
    state->xrep = ae_false;
    state->running = ae_false;
    
    /*
     * * allocate arrays
     * * set RX to NAN (just for the case user calls Results() without 
     *   calling SolveSparse()
     * * set starting point to zero
     * * we do NOT initialize B here because we assume that user should
     *   initializate it using LinCGSetB() function. In case he forgets
     *   to do so, exception will be thrown in the LinCGIteration().
     */
    ae_vector_set_length(&state->rx, state->n, _state);
    ae_vector_set_length(&state->startx, state->n, _state);
    ae_vector_set_length(&state->b, state->n, _state);
    for(i=0; i<=state->n-1; i++)
    {
        state->rx.ptr.p_double[i] = _state->v_nan;
        state->startx.ptr.p_double[i] = 0.0;
        state->b.ptr.p_double[i] = (double)(0);
    }
    ae_vector_set_length(&state->cx, state->n, _state);
    ae_vector_set_length(&state->p, state->n, _state);
    ae_vector_set_length(&state->r, state->n, _state);
    ae_vector_set_length(&state->cr, state->n, _state);
    ae_vector_set_length(&state->z, state->n, _state);
    ae_vector_set_length(&state->cz, state->n, _state);
    ae_vector_set_length(&state->x, state->n, _state);
    ae_vector_set_length(&state->mv, state->n, _state);
    ae_vector_set_length(&state->pv, state->n, _state);
    lincg_updateitersdata(state, _state);
    ae_vector_set_length(&state->rstate.ia, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 2+1, _state);
    state->rstate.stage = -1;
}


/*************************************************************************
This function sets starting point.
By default, zero starting point is used.

INPUT PARAMETERS:
    X       -   starting point, array[N]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetstartingpoint(lincgstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{


    ae_assert(!state->running, "LinCGSetStartingPoint: you can not change starting point because LinCGIteration() function is running", _state);
    ae_assert(state->n<=x->cnt, "LinCGSetStartingPoint: Length(X)<N", _state);
    ae_assert(isfinitevector(x, state->n, _state), "LinCGSetStartingPoint: X contains infinite or NaN values!", _state);
    ae_v_move(&state->startx.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
}


/*************************************************************************
This function sets right part. By default, right part is zero.

INPUT PARAMETERS:
    B       -   right part, array[N].

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetb(lincgstate* state,
     /* Real    */ const ae_vector* b,
     ae_state *_state)
{


    ae_assert(!state->running, "LinCGSetB: you can not set B, because function LinCGIteration is running!", _state);
    ae_assert(b->cnt>=state->n, "LinCGSetB: Length(B)<N", _state);
    ae_assert(isfinitevector(b, state->n, _state), "LinCGSetB: B contains infinite or NaN values!", _state);
    ae_v_move(&state->b.ptr.p_double[0], 1, &b->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
}


/*************************************************************************
This  function  changes  preconditioning  settings  of  LinCGSolveSparse()
function. By default, SolveSparse() uses diagonal preconditioner,  but  if
you want to use solver without preconditioning, you can call this function
which forces solver to use unit matrix for preconditioning.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void lincgsetprecunit(lincgstate* state, ae_state *_state)
{


    ae_assert(!state->running, "LinCGSetPrecUnit: you can not change preconditioner, because function LinCGIteration is running!", _state);
    state->prectype = -1;
}


/*************************************************************************
This  function  changes  preconditioning  settings  of  LinCGSolveSparse()
function.  LinCGSolveSparse() will use diagonal of the  system  matrix  as
preconditioner. This preconditioning mode is active by default.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void lincgsetprecdiag(lincgstate* state, ae_state *_state)
{


    ae_assert(!state->running, "LinCGSetPrecDiag: you can not change preconditioner, because function LinCGIteration is running!", _state);
    state->prectype = 0;
}


/*************************************************************************
This function sets stopping criteria.

INPUT PARAMETERS:
    EpsF    -   algorithm will be stopped if norm of residual is less than 
                EpsF*||b||.
    MaxIts  -   algorithm will be stopped if number of iterations is  more 
                than MaxIts.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
If  both  EpsF  and  MaxIts  are  zero then small EpsF will be set to small 
value.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetcond(lincgstate* state,
     double epsf,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(!state->running, "LinCGSetCond: you can not change stopping criteria when LinCGIteration() is running", _state);
    ae_assert(ae_isfinite(epsf, _state)&&ae_fp_greater_eq(epsf,(double)(0)), "LinCGSetCond: EpsF is negative or contains infinite or NaN values", _state);
    ae_assert(maxits>=0, "LinCGSetCond: MaxIts is negative", _state);
    if( ae_fp_eq(epsf,(double)(0))&&maxits==0 )
    {
        state->epsf = lincg_defaultprecision;
        state->maxits = maxits;
    }
    else
    {
        state->epsf = epsf;
        state->maxits = maxits;
    }
}


/*************************************************************************
Reverse communication version of linear CG.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
ae_bool lincgiteration(lincgstate* state, ae_state *_state)
{
    ae_int_t i;
    double uvar;
    double bnorm;
    double v;
    ae_bool result;


    
    /*
     * Reverse communication preparations
     * I know it looks ugly, but it works the same way
     * anywhere from C++ to Python.
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if( state->rstate.stage>=0 )
    {
        i = state->rstate.ia.ptr.p_int[0];
        uvar = state->rstate.ra.ptr.p_double[0];
        bnorm = state->rstate.ra.ptr.p_double[1];
        v = state->rstate.ra.ptr.p_double[2];
    }
    else
    {
        i = 359;
        uvar = -58.0;
        bnorm = -919.0;
        v = -909.0;
    }
    if( state->rstate.stage==0 )
    {
        goto lbl_0;
    }
    if( state->rstate.stage==1 )
    {
        goto lbl_1;
    }
    if( state->rstate.stage==2 )
    {
        goto lbl_2;
    }
    if( state->rstate.stage==3 )
    {
        goto lbl_3;
    }
    if( state->rstate.stage==4 )
    {
        goto lbl_4;
    }
    if( state->rstate.stage==5 )
    {
        goto lbl_5;
    }
    if( state->rstate.stage==6 )
    {
        goto lbl_6;
    }
    if( state->rstate.stage==7 )
    {
        goto lbl_7;
    }
    
    /*
     * Routine body
     */
    ae_assert(state->b.cnt>0, "LinCGIteration: B is not initialized (you must initialize B by LinCGSetB() call", _state);
    state->running = ae_true;
    state->repnmv = 0;
    lincg_clearrfields(state, _state);
    lincg_updateitersdata(state, _state);
    
    /*
     * Start 0-th iteration
     */
    ae_v_move(&state->rx.ptr.p_double[0], 1, &state->startx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->repnmv = state->repnmv+1;
    lincg_clearrfields(state, _state);
    state->needvmv = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needvmv = ae_false;
    bnorm = (double)(0);
    state->r2 = (double)(0);
    state->meritfunction = (double)(0);
    for(i=0; i<=state->n-1; i++)
    {
        state->r.ptr.p_double[i] = state->b.ptr.p_double[i]-state->mv.ptr.p_double[i];
        state->r2 = state->r2+state->r.ptr.p_double[i]*state->r.ptr.p_double[i];
        state->meritfunction = state->meritfunction+state->mv.ptr.p_double[i]*state->rx.ptr.p_double[i]-(double)2*state->b.ptr.p_double[i]*state->rx.ptr.p_double[i];
        bnorm = bnorm+state->b.ptr.p_double[i]*state->b.ptr.p_double[i];
    }
    bnorm = ae_sqrt(bnorm, _state);
    
    /*
     * Output first report
     */
    if( !state->xrep )
    {
        goto lbl_8;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    lincg_clearrfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->xupdated = ae_false;
lbl_8:
    
    /*
     * Is x0 a solution?
     */
    if( !ae_isfinite(state->r2, _state)||ae_fp_less_eq(ae_sqrt(state->r2, _state),state->epsf*bnorm) )
    {
        state->running = ae_false;
        if( ae_isfinite(state->r2, _state) )
        {
            state->repterminationtype = 1;
        }
        else
        {
            state->repterminationtype = -4;
        }
        result = ae_false;
        return result;
    }
    
    /*
     * Calculate Z and P
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->r.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->repnmv = state->repnmv+1;
    lincg_clearrfields(state, _state);
    state->needprec = ae_true;
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->needprec = ae_false;
    for(i=0; i<=state->n-1; i++)
    {
        state->z.ptr.p_double[i] = state->pv.ptr.p_double[i];
        state->p.ptr.p_double[i] = state->z.ptr.p_double[i];
    }
    
    /*
     * Other iterations(1..N)
     */
    state->repiterationscount = 0;
lbl_10:
    if( ae_false )
    {
        goto lbl_11;
    }
    state->repiterationscount = state->repiterationscount+1;
    
    /*
     * Calculate Alpha
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->p.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->repnmv = state->repnmv+1;
    lincg_clearrfields(state, _state);
    state->needvmv = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needvmv = ae_false;
    if( !ae_isfinite(state->vmv, _state)||ae_fp_less_eq(state->vmv,(double)(0)) )
    {
        
        /*
         * a) Overflow when calculating VMV
         * b) non-positive VMV (non-SPD matrix)
         */
        state->running = ae_false;
        if( ae_isfinite(state->vmv, _state) )
        {
            state->repterminationtype = -5;
        }
        else
        {
            state->repterminationtype = -4;
        }
        result = ae_false;
        return result;
    }
    state->alpha = (double)(0);
    for(i=0; i<=state->n-1; i++)
    {
        state->alpha = state->alpha+state->r.ptr.p_double[i]*state->z.ptr.p_double[i];
    }
    state->alpha = state->alpha/state->vmv;
    if( !ae_isfinite(state->alpha, _state) )
    {
        
        /*
         * Overflow when calculating Alpha
         */
        state->running = ae_false;
        state->repterminationtype = -4;
        result = ae_false;
        return result;
    }
    
    /*
     * Next step toward solution
     */
    for(i=0; i<=state->n-1; i++)
    {
        state->cx.ptr.p_double[i] = state->rx.ptr.p_double[i]+state->alpha*state->p.ptr.p_double[i];
    }
    
    /*
     * Calculate R:
     * * use recurrent relation to update R
     * * at every ItsBeforeRUpdate-th iteration recalculate it from scratch, using matrix-vector product
     *   in case R grows instead of decreasing, algorithm is terminated with positive completion code
     */
    if( !(state->itsbeforerupdate==0||state->repiterationscount%state->itsbeforerupdate!=0) )
    {
        goto lbl_12;
    }
    
    /*
     * Calculate R using recurrent formula
     */
    for(i=0; i<=state->n-1; i++)
    {
        state->cr.ptr.p_double[i] = state->r.ptr.p_double[i]-state->alpha*state->mv.ptr.p_double[i];
        state->x.ptr.p_double[i] = state->cr.ptr.p_double[i];
    }
    goto lbl_13;
lbl_12:
    
    /*
     * Calculate R using matrix-vector multiplication
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->cx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->repnmv = state->repnmv+1;
    lincg_clearrfields(state, _state);
    state->needmv = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->needmv = ae_false;
    for(i=0; i<=state->n-1; i++)
    {
        state->cr.ptr.p_double[i] = state->b.ptr.p_double[i]-state->mv.ptr.p_double[i];
        state->x.ptr.p_double[i] = state->cr.ptr.p_double[i];
    }
    
    /*
     * Calculating merit function
     * Check emergency stopping criterion
     */
    v = (double)(0);
    for(i=0; i<=state->n-1; i++)
    {
        v = v+state->mv.ptr.p_double[i]*state->cx.ptr.p_double[i]-(double)2*state->b.ptr.p_double[i]*state->cx.ptr.p_double[i];
    }
    if( ae_fp_less(v,state->meritfunction) )
    {
        goto lbl_14;
    }
    for(i=0; i<=state->n-1; i++)
    {
        if( !ae_isfinite(state->rx.ptr.p_double[i], _state) )
        {
            state->running = ae_false;
            state->repterminationtype = -4;
            result = ae_false;
            return result;
        }
    }
    
    /*
     *output last report
     */
    if( !state->xrep )
    {
        goto lbl_16;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    lincg_clearrfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->xupdated = ae_false;
lbl_16:
    state->running = ae_false;
    state->repterminationtype = 7;
    result = ae_false;
    return result;
lbl_14:
    state->meritfunction = v;
lbl_13:
    ae_v_move(&state->rx.ptr.p_double[0], 1, &state->cx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    
    /*
     * calculating RNorm
     *
     * NOTE: monotonic decrease of R2 is not guaranteed by algorithm.
     */
    state->r2 = (double)(0);
    for(i=0; i<=state->n-1; i++)
    {
        state->r2 = state->r2+state->cr.ptr.p_double[i]*state->cr.ptr.p_double[i];
    }
    
    /*
     *output report
     */
    if( !state->xrep )
    {
        goto lbl_18;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    lincg_clearrfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state->xupdated = ae_false;
lbl_18:
    
    /*
     *stopping criterion
     *achieved the required precision
     */
    if( !ae_isfinite(state->r2, _state)||ae_fp_less_eq(ae_sqrt(state->r2, _state),state->epsf*bnorm) )
    {
        state->running = ae_false;
        if( ae_isfinite(state->r2, _state) )
        {
            state->repterminationtype = 1;
        }
        else
        {
            state->repterminationtype = -4;
        }
        result = ae_false;
        return result;
    }
    if( state->repiterationscount>=state->maxits&&state->maxits>0 )
    {
        for(i=0; i<=state->n-1; i++)
        {
            if( !ae_isfinite(state->rx.ptr.p_double[i], _state) )
            {
                state->running = ae_false;
                state->repterminationtype = -4;
                result = ae_false;
                return result;
            }
        }
        
        /*
         *if X is finite number
         */
        state->running = ae_false;
        state->repterminationtype = 5;
        result = ae_false;
        return result;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->cr.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    
    /*
     *prepere of parameters for next iteration
     */
    state->repnmv = state->repnmv+1;
    lincg_clearrfields(state, _state);
    state->needprec = ae_true;
    state->rstate.stage = 7;
    goto lbl_rcomm;
lbl_7:
    state->needprec = ae_false;
    ae_v_move(&state->cz.ptr.p_double[0], 1, &state->pv.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    if( state->repiterationscount%state->itsbeforerestart!=0 )
    {
        state->beta = (double)(0);
        uvar = (double)(0);
        for(i=0; i<=state->n-1; i++)
        {
            state->beta = state->beta+state->cz.ptr.p_double[i]*state->cr.ptr.p_double[i];
            uvar = uvar+state->z.ptr.p_double[i]*state->r.ptr.p_double[i];
        }
        
        /*
         *check that UVar is't INF or is't zero
         */
        if( !ae_isfinite(uvar, _state)||ae_fp_eq(uvar,(double)(0)) )
        {
            state->running = ae_false;
            state->repterminationtype = -4;
            result = ae_false;
            return result;
        }
        
        /*
         *calculate .BETA
         */
        state->beta = state->beta/uvar;
        
        /*
         *check that .BETA neither INF nor NaN
         */
        if( !ae_isfinite(state->beta, _state) )
        {
            state->running = ae_false;
            state->repterminationtype = -1;
            result = ae_false;
            return result;
        }
        for(i=0; i<=state->n-1; i++)
        {
            state->p.ptr.p_double[i] = state->cz.ptr.p_double[i]+state->beta*state->p.ptr.p_double[i];
        }
    }
    else
    {
        ae_v_move(&state->p.ptr.p_double[0], 1, &state->cz.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    }
    
    /*
     *prepere data for next iteration
     */
    for(i=0; i<=state->n-1; i++)
    {
        
        /*
         *write (k+1)th iteration to (k )th iteration
         */
        state->r.ptr.p_double[i] = state->cr.ptr.p_double[i];
        state->z.ptr.p_double[i] = state->cz.ptr.p_double[i];
    }
    goto lbl_10;
lbl_11:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = i;
    state->rstate.ra.ptr.p_double[0] = uvar;
    state->rstate.ra.ptr.p_double[1] = bnorm;
    state->rstate.ra.ptr.p_double[2] = v;
    return result;
}


/*************************************************************************
Procedure for solution of A*x=b with sparse A.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse matrix in the CRS format (you MUST contvert  it  to 
                CRS format by calling SparseConvertToCRS() function).
    IsUpper -   whether upper or lower triangle of A is used:
                * IsUpper=True  => only upper triangle is used and lower
                                   triangle is not referenced at all 
                * IsUpper=False => only lower triangle is used and upper
                                   triangle is not referenced at all
    B       -   right part, array[N]

RESULT:
    This function returns no result.
    You can get solution by calling LinCGResults()
    
NOTE: this function uses lightweight preconditioning -  multiplication  by
      inverse of diag(A). If you want, you can turn preconditioning off by
      calling LinCGSetPrecUnit(). However, preconditioning cost is low and
      preconditioner  is  very  important  for  solution  of  badly scaled
      problems.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsolvesparse(lincgstate* state,
     const sparsematrix* a,
     ae_bool isupper,
     /* Real    */ const ae_vector* b,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    double v;
    double vmv;


    n = state->n;
    ae_assert(b->cnt>=state->n, "LinCGSetB: Length(B)<N", _state);
    ae_assert(isfinitevector(b, state->n, _state), "LinCGSetB: B contains infinite or NaN values!", _state);
    
    /*
     * Allocate temporaries
     */
    rvectorsetlengthatleast(&state->tmpd, n, _state);
    
    /*
     * Compute diagonal scaling matrix D
     */
    if( state->prectype==0 )
    {
        
        /*
         * Default preconditioner - inverse of matrix diagonal
         */
        for(i=0; i<=n-1; i++)
        {
            v = sparsegetdiagonal(a, i, _state);
            if( ae_fp_greater(v,(double)(0)) )
            {
                state->tmpd.ptr.p_double[i] = (double)1/ae_sqrt(v, _state);
            }
            else
            {
                state->tmpd.ptr.p_double[i] = (double)(1);
            }
        }
    }
    else
    {
        
        /*
         * No diagonal scaling
         */
        for(i=0; i<=n-1; i++)
        {
            state->tmpd.ptr.p_double[i] = (double)(1);
        }
    }
    
    /*
     * Solve
     */
    lincgrestart(state, _state);
    lincgsetb(state, b, _state);
    while(lincgiteration(state, _state))
    {
        
        /*
         * Process different requests from optimizer
         */
        if( state->needmv )
        {
            sparsesmv(a, isupper, &state->x, &state->mv, _state);
        }
        if( state->needvmv )
        {
            sparsesmv(a, isupper, &state->x, &state->mv, _state);
            vmv = ae_v_dotproduct(&state->x.ptr.p_double[0], 1, &state->mv.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
            state->vmv = vmv;
        }
        if( state->needprec )
        {
            for(i=0; i<=n-1; i++)
            {
                state->pv.ptr.p_double[i] = state->x.ptr.p_double[i]*ae_sqr(state->tmpd.ptr.p_double[i], _state);
            }
        }
    }
}


/*************************************************************************
CG-solver: results.

This function must be called after LinCGSolve

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -5    input matrix is either not positive definite,
                            too large or too small                            
                    * -4    overflow/underflow during solution
                            (ill conditioned problem)
                    *  1    ||residual||<=EpsF*||b||
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            best point found is returned
                * Rep.IterationsCount contains iterations count
                * NMV countains number of matrix-vector calculations

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgresults(const lincgstate* state,
     /* Real    */ ae_vector* x,
     lincgreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _lincgreport_clear(rep);

    ae_assert(!state->running, "LinCGResult: you can not get result, because function LinCGIteration has been launched!", _state);
    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    ae_v_move(&x->ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    rep->iterationscount = state->repiterationscount;
    rep->nmv = state->repnmv;
    rep->terminationtype = state->repterminationtype;
    rep->r2 = state->r2;
}


/*************************************************************************
This function sets restart frequency. By default, algorithm  is  restarted
after N subsequent iterations.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetrestartfreq(lincgstate* state,
     ae_int_t srf,
     ae_state *_state)
{


    ae_assert(!state->running, "LinCGSetRestartFreq: you can not change restart frequency when LinCGIteration() is running", _state);
    ae_assert(srf>0, "LinCGSetRestartFreq: non-positive SRF", _state);
    state->itsbeforerestart = srf;
}


/*************************************************************************
This function sets frequency of residual recalculations.

Algorithm updates residual r_k using iterative formula,  but  recalculates
it from scratch after each 10 iterations. It is done to avoid accumulation
of numerical errors and to stop algorithm when r_k starts to grow.

Such low update frequence (1/10) gives very  little  overhead,  but  makes
algorithm a bit more robust against numerical errors. However, you may
change it 

INPUT PARAMETERS:
    Freq    -   desired update frequency, Freq>=0.
                Zero value means that no updates will be done.

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetrupdatefreq(lincgstate* state,
     ae_int_t freq,
     ae_state *_state)
{


    ae_assert(!state->running, "LinCGSetRUpdateFreq: you can not change update frequency when LinCGIteration() is running", _state);
    ae_assert(freq>=0, "LinCGSetRUpdateFreq: non-positive Freq", _state);
    state->itsbeforerupdate = freq;
}


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinCGOptimize().

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgsetxrep(lincgstate* state, ae_bool needxrep, ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
Procedure for restart function LinCGIteration

  -- ALGLIB --
     Copyright 14.11.2011 by Bochkanov Sergey
*************************************************************************/
void lincgrestart(lincgstate* state, ae_state *_state)
{


    ae_vector_set_length(&state->rstate.ia, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 2+1, _state);
    state->rstate.stage = -1;
    lincg_clearrfields(state, _state);
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void lincg_clearrfields(lincgstate* state, ae_state *_state)
{


    state->xupdated = ae_false;
    state->needmv = ae_false;
    state->needmtv = ae_false;
    state->needmv2 = ae_false;
    state->needvmv = ae_false;
    state->needprec = ae_false;
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void lincg_updateitersdata(lincgstate* state, ae_state *_state)
{


    state->repiterationscount = 0;
    state->repnmv = 0;
    state->repterminationtype = 0;
}


void _lincgstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    lincgstate *p = (lincgstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->rx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->b, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cr, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cz, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->p, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->r, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->z, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->mv, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->pv, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->startx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpd, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
}


void _lincgstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    lincgstate       *dst = (lincgstate*)_dst;
    const lincgstate *src = (const lincgstate*)_src;
    ae_vector_init_copy(&dst->rx, &src->rx, _state, make_automatic);
    ae_vector_init_copy(&dst->b, &src->b, _state, make_automatic);
    dst->n = src->n;
    dst->prectype = src->prectype;
    ae_vector_init_copy(&dst->cx, &src->cx, _state, make_automatic);
    ae_vector_init_copy(&dst->cr, &src->cr, _state, make_automatic);
    ae_vector_init_copy(&dst->cz, &src->cz, _state, make_automatic);
    ae_vector_init_copy(&dst->p, &src->p, _state, make_automatic);
    ae_vector_init_copy(&dst->r, &src->r, _state, make_automatic);
    ae_vector_init_copy(&dst->z, &src->z, _state, make_automatic);
    dst->alpha = src->alpha;
    dst->beta = src->beta;
    dst->r2 = src->r2;
    dst->meritfunction = src->meritfunction;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->mv, &src->mv, _state, make_automatic);
    ae_vector_init_copy(&dst->pv, &src->pv, _state, make_automatic);
    dst->vmv = src->vmv;
    ae_vector_init_copy(&dst->startx, &src->startx, _state, make_automatic);
    dst->epsf = src->epsf;
    dst->maxits = src->maxits;
    dst->itsbeforerestart = src->itsbeforerestart;
    dst->itsbeforerupdate = src->itsbeforerupdate;
    dst->xrep = src->xrep;
    dst->xupdated = src->xupdated;
    dst->needmv = src->needmv;
    dst->needmtv = src->needmtv;
    dst->needmv2 = src->needmv2;
    dst->needvmv = src->needvmv;
    dst->needprec = src->needprec;
    dst->repiterationscount = src->repiterationscount;
    dst->repnmv = src->repnmv;
    dst->repterminationtype = src->repterminationtype;
    dst->running = src->running;
    ae_vector_init_copy(&dst->tmpd, &src->tmpd, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
}


void _lincgstate_clear(void* _p)
{
    lincgstate *p = (lincgstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->rx);
    ae_vector_clear(&p->b);
    ae_vector_clear(&p->cx);
    ae_vector_clear(&p->cr);
    ae_vector_clear(&p->cz);
    ae_vector_clear(&p->p);
    ae_vector_clear(&p->r);
    ae_vector_clear(&p->z);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->mv);
    ae_vector_clear(&p->pv);
    ae_vector_clear(&p->startx);
    ae_vector_clear(&p->tmpd);
    _rcommstate_clear(&p->rstate);
}


void _lincgstate_destroy(void* _p)
{
    lincgstate *p = (lincgstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->rx);
    ae_vector_destroy(&p->b);
    ae_vector_destroy(&p->cx);
    ae_vector_destroy(&p->cr);
    ae_vector_destroy(&p->cz);
    ae_vector_destroy(&p->p);
    ae_vector_destroy(&p->r);
    ae_vector_destroy(&p->z);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->mv);
    ae_vector_destroy(&p->pv);
    ae_vector_destroy(&p->startx);
    ae_vector_destroy(&p->tmpd);
    _rcommstate_destroy(&p->rstate);
}


void _lincgreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    lincgreport *p = (lincgreport*)_p;
    ae_touch_ptr((void*)p);
}


void _lincgreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    lincgreport       *dst = (lincgreport*)_dst;
    const lincgreport *src = (const lincgreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->nmv = src->nmv;
    dst->terminationtype = src->terminationtype;
    dst->r2 = src->r2;
}


void _lincgreport_clear(void* _p)
{
    lincgreport *p = (lincgreport*)_p;
    ae_touch_ptr((void*)p);
}


void _lincgreport_destroy(void* _p)
{
    lincgreport *p = (lincgreport*)_p;
    ae_touch_ptr((void*)p);
}


#endif
#if defined(AE_COMPILE_LINLSQR) || !defined(AE_PARTIAL_BUILD)


/*************************************************************************
This function initializes linear LSQR Solver. This solver is used to solve
non-symmetric (and, possibly, non-square) problems. Least squares solution
is returned for non-compatible systems.

USAGE:
1. User initializes algorithm state with LinLSQRCreate() call
2. User tunes solver parameters with  LinLSQRSetCond() and other functions
3. User  calls  LinLSQRSolveSparse()  function which takes algorithm state 
   and SparseMatrix object.
4. User calls LinLSQRResults() to get solution
5. Optionally, user may call LinLSQRSolveSparse() again to  solve  another  
   problem  with different matrix and/or right part without reinitializing 
   LinLSQRState structure.
  
INPUT PARAMETERS:
    M       -   number of rows in A
    N       -   number of variables, N>0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state
    
NOTE: see also linlsqrcreatebuf()  for  version  which  reuses  previously
      allocated place as much as possible.

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrcreate(ae_int_t m,
     ae_int_t n,
     linlsqrstate* state,
     ae_state *_state)
{

    _linlsqrstate_clear(state);

    ae_assert(m>0, "LinLSQRCreate: M<=0", _state);
    ae_assert(n>0, "LinLSQRCreate: N<=0", _state);
    linlsqrcreatebuf(m, n, state, _state);
}


/*************************************************************************
This function initializes linear LSQR Solver.  It  provides  exactly  same
functionality as linlsqrcreate(), but reuses  previously  allocated  space
as much as possible.
  
INPUT PARAMETERS:
    M       -   number of rows in A
    N       -   number of variables, N>0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 14.11.2018 by Bochkanov Sergey
*************************************************************************/
void linlsqrcreatebuf(ae_int_t m,
     ae_int_t n,
     linlsqrstate* state,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(m>0, "LinLSQRCreateBuf: M<=0", _state);
    ae_assert(n>0, "LinLSQRCreateBuf: N<=0", _state);
    state->m = m;
    state->n = n;
    state->prectype = 0;
    state->epsa = linlsqr_atol;
    state->epsb = linlsqr_btol;
    state->epsc = (double)1/ae_sqrt(ae_machineepsilon, _state);
    state->maxits = 0;
    state->lambdai = (double)(0);
    state->xrep = ae_false;
    state->running = ae_false;
    state->repiterationscount = 0;
    
    /*
     * * allocate arrays
     * * set RX to NAN (just for the case user calls Results() without 
     *   calling SolveSparse()
     * * set B to zero
     */
    normestimatorcreate(m, n, 2, 2, &state->nes, _state);
    ae_vector_set_length(&state->rx, state->n, _state);
    ae_vector_set_length(&state->ui, state->m+state->n, _state);
    ae_vector_set_length(&state->uip1, state->m+state->n, _state);
    ae_vector_set_length(&state->vip1, state->n, _state);
    ae_vector_set_length(&state->vi, state->n, _state);
    ae_vector_set_length(&state->omegai, state->n, _state);
    ae_vector_set_length(&state->omegaip1, state->n, _state);
    ae_vector_set_length(&state->d, state->n, _state);
    ae_vector_set_length(&state->x, state->m+state->n, _state);
    ae_vector_set_length(&state->mv, state->m+state->n, _state);
    ae_vector_set_length(&state->mtv, state->n, _state);
    ae_vector_set_length(&state->b, state->m, _state);
    for(i=0; i<=n-1; i++)
    {
        state->rx.ptr.p_double[i] = _state->v_nan;
    }
    for(i=0; i<=m-1; i++)
    {
        state->b.ptr.p_double[i] = (double)(0);
    }
    ae_vector_set_length(&state->rstate.ia, 1+1, _state);
    ae_vector_set_length(&state->rstate.ra, 0+1, _state);
    state->rstate.stage = -1;
}


/*************************************************************************
This function sets right part. By default, right part is zero.

INPUT PARAMETERS:
    B       -   right part, array[N].

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetb(linlsqrstate* state,
     /* Real    */ const ae_vector* b,
     ae_state *_state)
{
    ae_int_t i;


    ae_assert(!state->running, "LinLSQRSetB: you can not change B when LinLSQRIteration is running", _state);
    ae_assert(state->m<=b->cnt, "LinLSQRSetB: Length(B)<M", _state);
    ae_assert(isfinitevector(b, state->m, _state), "LinLSQRSetB: B contains infinite or NaN values", _state);
    state->bnorm2 = (double)(0);
    for(i=0; i<=state->m-1; i++)
    {
        state->b.ptr.p_double[i] = b->ptr.p_double[i];
        state->bnorm2 = state->bnorm2+b->ptr.p_double[i]*b->ptr.p_double[i];
    }
}


/*************************************************************************
This  function  changes  preconditioning  settings of LinLSQQSolveSparse()
function. By default, SolveSparse() uses diagonal preconditioner,  but  if
you want to use solver without preconditioning, you can call this function
which forces solver to use unit matrix for preconditioning.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetprecunit(linlsqrstate* state, ae_state *_state)
{


    ae_assert(!state->running, "LinLSQRSetPrecUnit: you can not change preconditioner, because function LinLSQRIteration is running!", _state);
    state->prectype = -1;
}


/*************************************************************************
This  function  changes  preconditioning  settings  of  LinCGSolveSparse()
function.  LinCGSolveSparse() will use diagonal of the  system  matrix  as
preconditioner. This preconditioning mode is active by default.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 19.11.2012 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetprecdiag(linlsqrstate* state, ae_state *_state)
{


    ae_assert(!state->running, "LinLSQRSetPrecDiag: you can not change preconditioner, because function LinCGIteration is running!", _state);
    state->prectype = 0;
}


/*************************************************************************
This function sets optional Tikhonov regularization coefficient.
It is zero by default.

INPUT PARAMETERS:
    LambdaI -   regularization factor, LambdaI>=0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state
    
  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetlambdai(linlsqrstate* state,
     double lambdai,
     ae_state *_state)
{


    ae_assert(!state->running, "LinLSQRSetLambdaI: you can not set LambdaI, because function LinLSQRIteration is running", _state);
    ae_assert(ae_isfinite(lambdai, _state)&&ae_fp_greater_eq(lambdai,(double)(0)), "LinLSQRSetLambdaI: LambdaI is infinite or NaN", _state);
    state->lambdai = lambdai;
}


/*************************************************************************

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
ae_bool linlsqriteration(linlsqrstate* state, ae_state *_state)
{
    ae_int_t summn;
    double bnorm;
    ae_int_t i;
    ae_bool result;


    
    /*
     * Reverse communication preparations
     * I know it looks ugly, but it works the same way
     * anywhere from C++ to Python.
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if( state->rstate.stage>=0 )
    {
        summn = state->rstate.ia.ptr.p_int[0];
        i = state->rstate.ia.ptr.p_int[1];
        bnorm = state->rstate.ra.ptr.p_double[0];
    }
    else
    {
        summn = 359;
        i = -58;
        bnorm = -919.0;
    }
    if( state->rstate.stage==0 )
    {
        goto lbl_0;
    }
    if( state->rstate.stage==1 )
    {
        goto lbl_1;
    }
    if( state->rstate.stage==2 )
    {
        goto lbl_2;
    }
    if( state->rstate.stage==3 )
    {
        goto lbl_3;
    }
    if( state->rstate.stage==4 )
    {
        goto lbl_4;
    }
    if( state->rstate.stage==5 )
    {
        goto lbl_5;
    }
    if( state->rstate.stage==6 )
    {
        goto lbl_6;
    }
    
    /*
     * Routine body
     */
    ae_assert(state->b.cnt>0, "LinLSQRIteration: using non-allocated array B", _state);
    summn = state->m+state->n;
    bnorm = ae_sqrt(state->bnorm2, _state);
    state->userterminationneeded = ae_false;
    state->running = ae_true;
    state->repnmv = 0;
    state->repiterationscount = 0;
    state->r2 = state->bnorm2;
    linlsqr_clearrfields(state, _state);
    
    /*
     *estimate for ANorm
     */
    normestimatorrestart(&state->nes, _state);
lbl_7:
    if( !normestimatoriteration(&state->nes, _state) )
    {
        goto lbl_8;
    }
    if( !state->nes.needmv )
    {
        goto lbl_9;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->nes.x.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->repnmv = state->repnmv+1;
    linlsqr_clearrfields(state, _state);
    state->needmv = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needmv = ae_false;
    ae_v_move(&state->nes.mv.ptr.p_double[0], 1, &state->mv.ptr.p_double[0], 1, ae_v_len(0,state->m-1));
    goto lbl_7;
lbl_9:
    if( !state->nes.needmtv )
    {
        goto lbl_11;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->nes.x.ptr.p_double[0], 1, ae_v_len(0,state->m-1));
    
    /*
     *matrix-vector multiplication
     */
    state->repnmv = state->repnmv+1;
    linlsqr_clearrfields(state, _state);
    state->needmtv = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->needmtv = ae_false;
    ae_v_move(&state->nes.mtv.ptr.p_double[0], 1, &state->mtv.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    goto lbl_7;
lbl_11:
    goto lbl_7;
lbl_8:
    normestimatorresults(&state->nes, &state->anorm, _state);
    
    /*
     *initialize .RX by zeros
     */
    for(i=0; i<=state->n-1; i++)
    {
        state->rx.ptr.p_double[i] = (double)(0);
    }
    
    /*
     *output first report
     */
    if( !state->xrep )
    {
        goto lbl_13;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    linlsqr_clearrfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->xupdated = ae_false;
lbl_13:
    
    /*
     * LSQR, Step 0.
     *
     * Algorithm outline corresponds to one which was described at p.50 of
     * "LSQR - an algorithm for sparse linear equations and sparse least 
     * squares" by C.Paige and M.Saunders with one small addition - we
     * explicitly extend system matrix by additional N lines in order 
     * to handle non-zero lambda, i.e. original A is replaced by
     *         [ A        ]
     * A_mod = [          ]
     *         [ lambda*I ].
     *
     * Step 0:
     *     x[0]          = 0
     *     beta[1]*u[1]  = b
     *     alpha[1]*v[1] = A_mod'*u[1]
     *     w[1]          = v[1]
     *     phiBar[1]     = beta[1]
     *     rhoBar[1]     = alpha[1]
     *     d[0]          = 0
     *
     * NOTE:
     * There are three criteria for stopping:
     * (S0) maximum number of iterations
     * (S1) ||Rk||<=EpsB*||B||;
     * (S2) ||A^T*Rk||/(||A||*||Rk||)<=EpsA.
     * It is very important that S2 always checked AFTER S1. It is necessary
     * to avoid division by zero when Rk=0.
     */
    state->betai = bnorm;
    if( ae_fp_eq(state->betai,(double)(0)) )
    {
        
        /*
         * Zero right part
         */
        state->running = ae_false;
        state->repterminationtype = 1;
        result = ae_false;
        return result;
    }
    for(i=0; i<=summn-1; i++)
    {
        if( i<state->m )
        {
            state->ui.ptr.p_double[i] = state->b.ptr.p_double[i]/state->betai;
        }
        else
        {
            state->ui.ptr.p_double[i] = (double)(0);
        }
        state->x.ptr.p_double[i] = state->ui.ptr.p_double[i];
    }
    state->repnmv = state->repnmv+1;
    linlsqr_clearrfields(state, _state);
    state->needmtv = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needmtv = ae_false;
    for(i=0; i<=state->n-1; i++)
    {
        state->mtv.ptr.p_double[i] = state->mtv.ptr.p_double[i]+state->lambdai*state->ui.ptr.p_double[state->m+i];
    }
    state->alphai = (double)(0);
    for(i=0; i<=state->n-1; i++)
    {
        state->alphai = state->alphai+state->mtv.ptr.p_double[i]*state->mtv.ptr.p_double[i];
    }
    state->alphai = ae_sqrt(state->alphai, _state);
    if( ae_fp_eq(state->alphai,(double)(0)) )
    {
        
        /*
         * Orthogonality stopping criterion is met
         */
        state->running = ae_false;
        state->repterminationtype = 4;
        result = ae_false;
        return result;
    }
    for(i=0; i<=state->n-1; i++)
    {
        state->vi.ptr.p_double[i] = state->mtv.ptr.p_double[i]/state->alphai;
        state->omegai.ptr.p_double[i] = state->vi.ptr.p_double[i];
    }
    state->phibari = state->betai;
    state->rhobari = state->alphai;
    for(i=0; i<=state->n-1; i++)
    {
        state->d.ptr.p_double[i] = (double)(0);
    }
    state->dnorm = (double)(0);
    
    /*
     * Steps I=1, 2, ...
     */
lbl_15:
    if( ae_false )
    {
        goto lbl_16;
    }
    
    /*
     * At I-th step State.RepIterationsCount=I.
     */
    state->repiterationscount = state->repiterationscount+1;
    
    /*
     * Bidiagonalization part:
     *     beta[i+1]*u[i+1]  = A_mod*v[i]-alpha[i]*u[i]
     *     alpha[i+1]*v[i+1] = A_mod'*u[i+1] - beta[i+1]*v[i]
     *     
     * NOTE:  beta[i+1]=0 or alpha[i+1]=0 will lead to successful termination
     *        in the end of the current iteration. In this case u/v are zero.
     * NOTE2: algorithm won't fail on zero alpha or beta (there will be no
     *        division by zero because it will be stopped BEFORE division
     *        occurs). However, near-zero alpha and beta won't stop algorithm
     *        and, although no division by zero will happen, orthogonality 
     *        in U and V will be lost.
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->vi.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->repnmv = state->repnmv+1;
    linlsqr_clearrfields(state, _state);
    state->needmv = ae_true;
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->needmv = ae_false;
    for(i=0; i<=state->n-1; i++)
    {
        state->mv.ptr.p_double[state->m+i] = state->lambdai*state->vi.ptr.p_double[i];
    }
    state->betaip1 = (double)(0);
    for(i=0; i<=summn-1; i++)
    {
        state->uip1.ptr.p_double[i] = state->mv.ptr.p_double[i]-state->alphai*state->ui.ptr.p_double[i];
        state->betaip1 = state->betaip1+state->uip1.ptr.p_double[i]*state->uip1.ptr.p_double[i];
    }
    if( ae_fp_neq(state->betaip1,(double)(0)) )
    {
        state->betaip1 = ae_sqrt(state->betaip1, _state);
        for(i=0; i<=summn-1; i++)
        {
            state->uip1.ptr.p_double[i] = state->uip1.ptr.p_double[i]/state->betaip1;
        }
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->uip1.ptr.p_double[0], 1, ae_v_len(0,state->m-1));
    state->repnmv = state->repnmv+1;
    linlsqr_clearrfields(state, _state);
    state->needmtv = ae_true;
    state->rstate.stage = 5;
    goto lbl_rcomm;
lbl_5:
    state->needmtv = ae_false;
    for(i=0; i<=state->n-1; i++)
    {
        state->mtv.ptr.p_double[i] = state->mtv.ptr.p_double[i]+state->lambdai*state->uip1.ptr.p_double[state->m+i];
    }
    state->alphaip1 = (double)(0);
    for(i=0; i<=state->n-1; i++)
    {
        state->vip1.ptr.p_double[i] = state->mtv.ptr.p_double[i]-state->betaip1*state->vi.ptr.p_double[i];
        state->alphaip1 = state->alphaip1+state->vip1.ptr.p_double[i]*state->vip1.ptr.p_double[i];
    }
    if( ae_fp_neq(state->alphaip1,(double)(0)) )
    {
        state->alphaip1 = ae_sqrt(state->alphaip1, _state);
        for(i=0; i<=state->n-1; i++)
        {
            state->vip1.ptr.p_double[i] = state->vip1.ptr.p_double[i]/state->alphaip1;
        }
    }
    
    /*
     * Build next orthogonal transformation
     */
    state->rhoi = safepythag2(state->rhobari, state->betaip1, _state);
    state->ci = state->rhobari/state->rhoi;
    state->si = state->betaip1/state->rhoi;
    state->theta = state->si*state->alphaip1;
    state->rhobarip1 = -state->ci*state->alphaip1;
    state->phii = state->ci*state->phibari;
    state->phibarip1 = state->si*state->phibari;
    
    /*
     * Update .RNorm
     *
     * This tricky  formula  is  necessary  because  simply  writing
     * State.R2:=State.PhiBarIP1*State.PhiBarIP1 does NOT guarantees
     * monotonic decrease of R2. Roundoff error combined with 80-bit
     * precision used internally by Intel chips allows R2 to increase
     * slightly in some rare, but possible cases. This property is
     * undesirable, so we prefer to guard against R increase.
     */
    state->r2 = ae_minreal(state->r2, state->phibarip1*state->phibarip1, _state);
    
    /*
     * Update d and DNorm, check condition-related stopping criteria
     */
    for(i=0; i<=state->n-1; i++)
    {
        state->d.ptr.p_double[i] = (double)1/state->rhoi*(state->vi.ptr.p_double[i]-state->theta*state->d.ptr.p_double[i]);
        state->dnorm = state->dnorm+state->d.ptr.p_double[i]*state->d.ptr.p_double[i];
    }
    if( ae_fp_greater_eq(ae_sqrt(state->dnorm, _state)*state->anorm,state->epsc) )
    {
        state->running = ae_false;
        state->repterminationtype = 7;
        result = ae_false;
        return result;
    }
    
    /*
     * Update x, output report
     */
    for(i=0; i<=state->n-1; i++)
    {
        state->rx.ptr.p_double[i] = state->rx.ptr.p_double[i]+state->phii/state->rhoi*state->omegai.ptr.p_double[i];
    }
    if( !state->xrep )
    {
        goto lbl_17;
    }
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    linlsqr_clearrfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 6;
    goto lbl_rcomm;
lbl_6:
    state->xupdated = ae_false;
lbl_17:
    
    /*
     * Check stopping criteria
     * 1. achieved required number of iterations;
     * 2. ||Rk||<=EpsB*||B||;
     * 3. ||A^T*Rk||/(||A||*||Rk||)<=EpsA;
     */
    if( state->maxits>0&&state->repiterationscount>=state->maxits )
    {
        
        /*
         * Achieved required number of iterations
         */
        state->running = ae_false;
        state->repterminationtype = 5;
        result = ae_false;
        return result;
    }
    if( ae_fp_less_eq(state->phibarip1,state->epsb*bnorm) )
    {
        
        /*
         * ||Rk||<=EpsB*||B||, here ||Rk||=PhiBar
         */
        state->running = ae_false;
        state->repterminationtype = 1;
        result = ae_false;
        return result;
    }
    if( ae_fp_less_eq(state->alphaip1*ae_fabs(state->ci, _state)/state->anorm,state->epsa) )
    {
        
        /*
         * ||A^T*Rk||/(||A||*||Rk||)<=EpsA, here ||A^T*Rk||=PhiBar*Alpha[i+1]*|.C|
         */
        state->running = ae_false;
        state->repterminationtype = 4;
        result = ae_false;
        return result;
    }
    if( state->userterminationneeded )
    {
        
        /*
         * User requested termination
         */
        state->running = ae_false;
        state->repterminationtype = 8;
        result = ae_false;
        return result;
    }
    
    /*
     * Update omega
     */
    for(i=0; i<=state->n-1; i++)
    {
        state->omegaip1.ptr.p_double[i] = state->vip1.ptr.p_double[i]-state->theta/state->rhoi*state->omegai.ptr.p_double[i];
    }
    
    /*
     * Prepare for the next iteration - rename variables:
     * u[i]   := u[i+1]
     * v[i]   := v[i+1]
     * rho[i] := rho[i+1]
     * ...
     */
    ae_v_move(&state->ui.ptr.p_double[0], 1, &state->uip1.ptr.p_double[0], 1, ae_v_len(0,summn-1));
    ae_v_move(&state->vi.ptr.p_double[0], 1, &state->vip1.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    ae_v_move(&state->omegai.ptr.p_double[0], 1, &state->omegaip1.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    state->alphai = state->alphaip1;
    state->betai = state->betaip1;
    state->phibari = state->phibarip1;
    state->rhobari = state->rhobarip1;
    goto lbl_15;
lbl_16:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = summn;
    state->rstate.ia.ptr.p_int[1] = i;
    state->rstate.ra.ptr.p_double[0] = bnorm;
    return result;
}


/*************************************************************************
Procedure for solution of A*x=b with sparse A.

INPUT PARAMETERS:
    State   -   algorithm state
    A       -   sparse M*N matrix in the CRS format (you MUST contvert  it 
                to CRS format  by  calling  SparseConvertToCRS()  function
                BEFORE you pass it to this function).
    B       -   right part, array[M]

RESULT:
    This function returns no result.
    You can get solution by calling LinCGResults()
    
NOTE: this function uses lightweight preconditioning -  multiplication  by
      inverse of diag(A). If you want, you can turn preconditioning off by
      calling LinLSQRSetPrecUnit(). However, preconditioning cost is   low
      and preconditioner is very important for solution  of  badly  scaled
      problems.

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsolvesparse(linlsqrstate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* b,
     ae_state *_state)
{
    ae_int_t n;
    ae_int_t i;
    ae_int_t j;
    ae_int_t t0;
    ae_int_t t1;
    double v;


    n = state->n;
    ae_assert(!state->running, "LinLSQRSolveSparse: you can not call this function when LinLSQRIteration is running", _state);
    ae_assert(b->cnt>=state->m, "LinLSQRSolveSparse: Length(B)<M", _state);
    ae_assert(isfinitevector(b, state->m, _state), "LinLSQRSolveSparse: B contains infinite or NaN values", _state);
    
    /*
     * Allocate temporaries
     */
    rvectorsetlengthatleast(&state->tmpd, n, _state);
    rvectorsetlengthatleast(&state->tmpx, n, _state);
    
    /*
     * Compute diagonal scaling matrix D
     */
    if( state->prectype==0 )
    {
        
        /*
         * Default preconditioner - inverse of column norms
         */
        for(i=0; i<=n-1; i++)
        {
            state->tmpd.ptr.p_double[i] = (double)(0);
        }
        t0 = 0;
        t1 = 0;
        while(sparseenumerate(a, &t0, &t1, &i, &j, &v, _state))
        {
            state->tmpd.ptr.p_double[j] = state->tmpd.ptr.p_double[j]+ae_sqr(v, _state);
        }
        for(i=0; i<=n-1; i++)
        {
            if( ae_fp_greater(state->tmpd.ptr.p_double[i],(double)(0)) )
            {
                state->tmpd.ptr.p_double[i] = (double)1/ae_sqrt(state->tmpd.ptr.p_double[i], _state);
            }
            else
            {
                state->tmpd.ptr.p_double[i] = (double)(1);
            }
        }
    }
    else
    {
        
        /*
         * No diagonal scaling
         */
        for(i=0; i<=n-1; i++)
        {
            state->tmpd.ptr.p_double[i] = (double)(1);
        }
    }
    
    /*
     * Solve.
     *
     * Instead of solving A*x=b we solve preconditioned system (A*D)*(inv(D)*x)=b.
     * Transformed A is not calculated explicitly, we just modify multiplication
     * by A or A'. After solution we modify State.RX so it will store untransformed
     * variables
     */
    linlsqrsetb(state, b, _state);
    linlsqrrestart(state, _state);
    while(linlsqriteration(state, _state))
    {
        if( state->needmv )
        {
            for(i=0; i<=n-1; i++)
            {
                state->tmpx.ptr.p_double[i] = state->tmpd.ptr.p_double[i]*state->x.ptr.p_double[i];
            }
            sparsemv(a, &state->tmpx, &state->mv, _state);
        }
        if( state->needmtv )
        {
            sparsemtv(a, &state->x, &state->mtv, _state);
            for(i=0; i<=n-1; i++)
            {
                state->mtv.ptr.p_double[i] = state->tmpd.ptr.p_double[i]*state->mtv.ptr.p_double[i];
            }
        }
    }
    for(i=0; i<=n-1; i++)
    {
        state->rx.ptr.p_double[i] = state->tmpd.ptr.p_double[i]*state->rx.ptr.p_double[i];
    }
}


/*************************************************************************
This function sets stopping criteria.

INPUT PARAMETERS:
    EpsA    -   algorithm will be stopped if ||A^T*Rk||/(||A||*||Rk||)<=EpsA.
    EpsB    -   algorithm will be stopped if ||Rk||<=EpsB*||B||
    MaxIts  -   algorithm will be stopped if number of iterations
                more than MaxIts.

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE: if EpsA,EpsB,EpsC and MaxIts are zero then these variables will
be setted as default values.
    
  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetcond(linlsqrstate* state,
     double epsa,
     double epsb,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(!state->running, "LinLSQRSetCond: you can not call this function when LinLSQRIteration is running", _state);
    ae_assert(ae_isfinite(epsa, _state)&&ae_fp_greater_eq(epsa,(double)(0)), "LinLSQRSetCond: EpsA is negative, INF or NAN", _state);
    ae_assert(ae_isfinite(epsb, _state)&&ae_fp_greater_eq(epsb,(double)(0)), "LinLSQRSetCond: EpsB is negative, INF or NAN", _state);
    ae_assert(maxits>=0, "LinLSQRSetCond: MaxIts is negative", _state);
    if( (ae_fp_eq(epsa,(double)(0))&&ae_fp_eq(epsb,(double)(0)))&&maxits==0 )
    {
        state->epsa = linlsqr_atol;
        state->epsb = linlsqr_btol;
        state->maxits = state->n;
    }
    else
    {
        state->epsa = epsa;
        state->epsb = epsb;
        state->maxits = maxits;
    }
}


/*************************************************************************
LSQR solver: results.

This function must be called after LinLSQRSolve

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    *  1    ||Rk||<=EpsB*||B||
                    *  4    ||A^T*Rk||/(||A||*||Rk||)<=EpsA
                    *  5    MaxIts steps was taken
                    *  7    rounding errors prevent further progress,
                            X contains best point found so far.
                            (sometimes returned on singular systems)
                    *  8    user requested termination via calling
                            linlsqrrequesttermination()
                * Rep.IterationsCount contains iterations count
                * NMV countains number of matrix-vector calculations
                
  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrresults(const linlsqrstate* state,
     /* Real    */ ae_vector* x,
     linlsqrreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _linlsqrreport_clear(rep);

    ae_assert(!state->running, "LinLSQRResult: you can not call this function when LinLSQRIteration is running", _state);
    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    ae_v_move(&x->ptr.p_double[0], 1, &state->rx.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    rep->iterationscount = state->repiterationscount;
    rep->nmv = state->repnmv;
    rep->terminationtype = state->repterminationtype;
}


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinCGOptimize().

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrsetxrep(linlsqrstate* state,
     ae_bool needxrep,
     ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
This function restarts LinLSQRIteration

  -- ALGLIB --
     Copyright 30.11.2011 by Bochkanov Sergey
*************************************************************************/
void linlsqrrestart(linlsqrstate* state, ae_state *_state)
{


    ae_vector_set_length(&state->rstate.ia, 1+1, _state);
    ae_vector_set_length(&state->rstate.ra, 0+1, _state);
    state->rstate.stage = -1;
    linlsqr_clearrfields(state, _state);
    state->repiterationscount = 0;
}


/*************************************************************************
This function is used to peek into LSQR solver and get  current  iteration
counter. You can safely "peek" into the solver from another thread.

INPUT PARAMETERS:
    S           -   solver object

RESULT:
    iteration counter, in [0,INF)

  -- ALGLIB --
     Copyright 21.05.2018 by Bochkanov Sergey
*************************************************************************/
ae_int_t linlsqrpeekiterationscount(const linlsqrstate* s,
     ae_state *_state)
{
    ae_int_t result;


    result = s->repiterationscount;
    return result;
}


/*************************************************************************
This subroutine submits request for termination of the running solver.  It
can be called from some other thread which wants LSQR solver to  terminate
(obviously, the  thread  running  LSQR  solver can not request termination
because it is already busy working on LSQR).

As result, solver  stops  at  point  which  was  "current  accepted"  when
termination  request  was  submitted  and returns error code 8 (successful
termination).  Such   termination   is  a smooth  process  which  properly
deallocates all temporaries.

INPUT PARAMETERS:
    State   -   solver structure

NOTE: calling this function on solver which is NOT running  will  have  no
      effect.
      
NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

NOTE: solver clears termination flag on its start, it means that  if  some
      other thread will request termination too soon, its request will went
      unnoticed.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void linlsqrrequesttermination(linlsqrstate* state, ae_state *_state)
{


    state->userterminationneeded = ae_true;
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void linlsqr_clearrfields(linlsqrstate* state, ae_state *_state)
{


    state->xupdated = ae_false;
    state->needmv = ae_false;
    state->needmtv = ae_false;
    state->needmv2 = ae_false;
    state->needvmv = ae_false;
    state->needprec = ae_false;
}


void _linlsqrstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    linlsqrstate *p = (linlsqrstate*)_p;
    ae_touch_ptr((void*)p);
    _normestimatorstate_init(&p->nes, _state, make_automatic);
    ae_vector_init(&p->rx, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->b, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->ui, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->uip1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->vi, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->vip1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->omegai, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->omegaip1, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->d, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->mv, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->mtv, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpd, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->tmpx, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
}


void _linlsqrstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    linlsqrstate       *dst = (linlsqrstate*)_dst;
    const linlsqrstate *src = (const linlsqrstate*)_src;
    _normestimatorstate_init_copy(&dst->nes, &src->nes, _state, make_automatic);
    ae_vector_init_copy(&dst->rx, &src->rx, _state, make_automatic);
    ae_vector_init_copy(&dst->b, &src->b, _state, make_automatic);
    dst->n = src->n;
    dst->m = src->m;
    dst->prectype = src->prectype;
    ae_vector_init_copy(&dst->ui, &src->ui, _state, make_automatic);
    ae_vector_init_copy(&dst->uip1, &src->uip1, _state, make_automatic);
    ae_vector_init_copy(&dst->vi, &src->vi, _state, make_automatic);
    ae_vector_init_copy(&dst->vip1, &src->vip1, _state, make_automatic);
    ae_vector_init_copy(&dst->omegai, &src->omegai, _state, make_automatic);
    ae_vector_init_copy(&dst->omegaip1, &src->omegaip1, _state, make_automatic);
    dst->alphai = src->alphai;
    dst->alphaip1 = src->alphaip1;
    dst->betai = src->betai;
    dst->betaip1 = src->betaip1;
    dst->phibari = src->phibari;
    dst->phibarip1 = src->phibarip1;
    dst->phii = src->phii;
    dst->rhobari = src->rhobari;
    dst->rhobarip1 = src->rhobarip1;
    dst->rhoi = src->rhoi;
    dst->ci = src->ci;
    dst->si = src->si;
    dst->theta = src->theta;
    dst->lambdai = src->lambdai;
    ae_vector_init_copy(&dst->d, &src->d, _state, make_automatic);
    dst->anorm = src->anorm;
    dst->bnorm2 = src->bnorm2;
    dst->dnorm = src->dnorm;
    dst->r2 = src->r2;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    ae_vector_init_copy(&dst->mv, &src->mv, _state, make_automatic);
    ae_vector_init_copy(&dst->mtv, &src->mtv, _state, make_automatic);
    dst->epsa = src->epsa;
    dst->epsb = src->epsb;
    dst->epsc = src->epsc;
    dst->maxits = src->maxits;
    dst->xrep = src->xrep;
    dst->xupdated = src->xupdated;
    dst->needmv = src->needmv;
    dst->needmtv = src->needmtv;
    dst->needmv2 = src->needmv2;
    dst->needvmv = src->needvmv;
    dst->needprec = src->needprec;
    dst->repiterationscount = src->repiterationscount;
    dst->repnmv = src->repnmv;
    dst->repterminationtype = src->repterminationtype;
    dst->running = src->running;
    dst->userterminationneeded = src->userterminationneeded;
    ae_vector_init_copy(&dst->tmpd, &src->tmpd, _state, make_automatic);
    ae_vector_init_copy(&dst->tmpx, &src->tmpx, _state, make_automatic);
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
}


void _linlsqrstate_clear(void* _p)
{
    linlsqrstate *p = (linlsqrstate*)_p;
    ae_touch_ptr((void*)p);
    _normestimatorstate_clear(&p->nes);
    ae_vector_clear(&p->rx);
    ae_vector_clear(&p->b);
    ae_vector_clear(&p->ui);
    ae_vector_clear(&p->uip1);
    ae_vector_clear(&p->vi);
    ae_vector_clear(&p->vip1);
    ae_vector_clear(&p->omegai);
    ae_vector_clear(&p->omegaip1);
    ae_vector_clear(&p->d);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->mv);
    ae_vector_clear(&p->mtv);
    ae_vector_clear(&p->tmpd);
    ae_vector_clear(&p->tmpx);
    _rcommstate_clear(&p->rstate);
}


void _linlsqrstate_destroy(void* _p)
{
    linlsqrstate *p = (linlsqrstate*)_p;
    ae_touch_ptr((void*)p);
    _normestimatorstate_destroy(&p->nes);
    ae_vector_destroy(&p->rx);
    ae_vector_destroy(&p->b);
    ae_vector_destroy(&p->ui);
    ae_vector_destroy(&p->uip1);
    ae_vector_destroy(&p->vi);
    ae_vector_destroy(&p->vip1);
    ae_vector_destroy(&p->omegai);
    ae_vector_destroy(&p->omegaip1);
    ae_vector_destroy(&p->d);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->mv);
    ae_vector_destroy(&p->mtv);
    ae_vector_destroy(&p->tmpd);
    ae_vector_destroy(&p->tmpx);
    _rcommstate_destroy(&p->rstate);
}


void _linlsqrreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    linlsqrreport *p = (linlsqrreport*)_p;
    ae_touch_ptr((void*)p);
}


void _linlsqrreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    linlsqrreport       *dst = (linlsqrreport*)_dst;
    const linlsqrreport *src = (const linlsqrreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->nmv = src->nmv;
    dst->terminationtype = src->terminationtype;
}


void _linlsqrreport_clear(void* _p)
{
    linlsqrreport *p = (linlsqrreport*)_p;
    ae_touch_ptr((void*)p);
}


void _linlsqrreport_destroy(void* _p)
{
    linlsqrreport *p = (linlsqrreport*)_p;
    ae_touch_ptr((void*)p);
}


#endif
#if defined(AE_COMPILE_NLEQ) || !defined(AE_PARTIAL_BUILD)


/*************************************************************************
                LEVENBERG-MARQUARDT-LIKE NONLINEAR SOLVER

DESCRIPTION:
This algorithm solves system of nonlinear equations
    F[0](x[0], ..., x[n-1])   = 0
    F[1](x[0], ..., x[n-1])   = 0
    ...
    F[M-1](x[0], ..., x[n-1]) = 0
with M/N do not necessarily coincide.  Algorithm  converges  quadratically
under following conditions:
    * the solution set XS is nonempty
    * for some xs in XS there exist such neighbourhood N(xs) that:
      * vector function F(x) and its Jacobian J(x) are continuously
        differentiable on N
      * ||F(x)|| provides local error bound on N, i.e. there  exists  such
        c1, that ||F(x)||>c1*distance(x,XS)
Note that these conditions are much more weaker than usual non-singularity
conditions. For example, algorithm will converge for any  affine  function
F (whether its Jacobian singular or not).


REQUIREMENTS:
Algorithm will request following information during its operation:
* function vector F[] and Jacobian matrix at given point X
* value of merit function f(x)=F[0]^2(x)+...+F[M-1]^2(x) at given point X


USAGE:
1. User initializes algorithm state with NLEQCreateLM() call
2. User tunes solver parameters with  NLEQSetCond(),  NLEQSetStpMax()  and
   other functions
3. User  calls  NLEQSolve()  function  which  takes  algorithm  state  and
   pointers (delegates, etc.) to callback functions which calculate  merit
   function value and Jacobian.
4. User calls NLEQResults() to get solution
5. Optionally, user may call NLEQRestartFrom() to  solve  another  problem
   with same parameters (N/M) but another starting  point  and/or  another
   function vector. NLEQRestartFrom() allows to reuse already  initialized
   structure.


INPUT PARAMETERS:
    N       -   space dimension, N>1:
                * if provided, only leading N elements of X are used
                * if not provided, determined automatically from size of X
    M       -   system size
    X       -   starting point


OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state


NOTES:
1. you may tune stopping conditions with NLEQSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use NLEQSetStpMax() function to bound algorithm's steps.
3. this  algorithm  is  a  slightly  modified implementation of the method
   described  in  'Levenberg-Marquardt  method  for constrained  nonlinear
   equations with strong local convergence properties' by Christian Kanzow
   Nobuo Yamashita and Masao Fukushima and further  developed  in  'On the
   convergence of a New Levenberg-Marquardt Method'  by  Jin-yan  Fan  and
   Ya-Xiang Yuan.


  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
void nleqcreatelm(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     nleqstate* state,
     ae_state *_state)
{

    _nleqstate_clear(state);

    ae_assert(n>=1, "NLEQCreateLM: N<1!", _state);
    ae_assert(m>=1, "NLEQCreateLM: M<1!", _state);
    ae_assert(x->cnt>=n, "NLEQCreateLM: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, n, _state), "NLEQCreateLM: X contains infinite or NaN values!", _state);
    
    /*
     * Initialize
     */
    state->n = n;
    state->m = m;
    nleqsetcond(state, (double)(0), 0, _state);
    nleqsetxrep(state, ae_false, _state);
    nleqsetstpmax(state, (double)(0), _state);
    ae_vector_set_length(&state->x, n, _state);
    ae_vector_set_length(&state->xbase, n, _state);
    ae_matrix_set_length(&state->j, m, n, _state);
    ae_vector_set_length(&state->fi, m, _state);
    ae_vector_set_length(&state->rightpart, n, _state);
    ae_vector_set_length(&state->candstep, n, _state);
    nleqrestartfrom(state, x, _state);
}


/*************************************************************************
This function sets stopping conditions for the nonlinear solver

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsF    -   >=0
                The subroutine finishes  its work if on k+1-th iteration
                the condition ||F||<=EpsF is satisfied
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsF=0 and MaxIts=0 simultaneously will lead to  automatic
stopping criterion selection (small EpsF).

NOTES:

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void nleqsetcond(nleqstate* state,
     double epsf,
     ae_int_t maxits,
     ae_state *_state)
{


    ae_assert(ae_isfinite(epsf, _state), "NLEQSetCond: EpsF is not finite number!", _state);
    ae_assert(ae_fp_greater_eq(epsf,(double)(0)), "NLEQSetCond: negative EpsF!", _state);
    ae_assert(maxits>=0, "NLEQSetCond: negative MaxIts!", _state);
    if( ae_fp_eq(epsf,(double)(0))&&maxits==0 )
    {
        epsf = 1.0E-6;
    }
    state->epsf = epsf;
    state->maxits = maxits;
}


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to NLEQSolve().

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void nleqsetxrep(nleqstate* state, ae_bool needxrep, ae_state *_state)
{


    state->xrep = needxrep;
}


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when target function  contains  exp()  or  other  fast
growing functions, and algorithm makes  too  large  steps  which  lead  to
overflow. This function allows us to reject steps that are too large  (and
therefore expose us to the possible overflow) without actually calculating
function value at the x+stp*d.

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void nleqsetstpmax(nleqstate* state, double stpmax, ae_state *_state)
{


    ae_assert(ae_isfinite(stpmax, _state), "NLEQSetStpMax: StpMax is not finite!", _state);
    ae_assert(ae_fp_greater_eq(stpmax,(double)(0)), "NLEQSetStpMax: StpMax<0!", _state);
    state->stpmax = stpmax;
}


/*************************************************************************

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
ae_bool nleqiteration(nleqstate* state, ae_state *_state)
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t i;
    double lambdaup;
    double lambdadown;
    double lambdav;
    double rho;
    double mu;
    double stepnorm;
    ae_bool b;
    ae_bool result;


    
    /*
     * Reverse communication preparations
     * I know it looks ugly, but it works the same way
     * anywhere from C++ to Python.
     *
     * This code initializes locals by:
     * * random values determined during code
     *   generation - on first subroutine call
     * * values from previous call - on subsequent calls
     */
    if( state->rstate.stage>=0 )
    {
        n = state->rstate.ia.ptr.p_int[0];
        m = state->rstate.ia.ptr.p_int[1];
        i = state->rstate.ia.ptr.p_int[2];
        b = state->rstate.ba.ptr.p_bool[0];
        lambdaup = state->rstate.ra.ptr.p_double[0];
        lambdadown = state->rstate.ra.ptr.p_double[1];
        lambdav = state->rstate.ra.ptr.p_double[2];
        rho = state->rstate.ra.ptr.p_double[3];
        mu = state->rstate.ra.ptr.p_double[4];
        stepnorm = state->rstate.ra.ptr.p_double[5];
    }
    else
    {
        n = 359;
        m = -58;
        i = -919;
        b = ae_true;
        lambdaup = 81.0;
        lambdadown = 255.0;
        lambdav = 74.0;
        rho = -788.0;
        mu = 809.0;
        stepnorm = 205.0;
    }
    if( state->rstate.stage==0 )
    {
        goto lbl_0;
    }
    if( state->rstate.stage==1 )
    {
        goto lbl_1;
    }
    if( state->rstate.stage==2 )
    {
        goto lbl_2;
    }
    if( state->rstate.stage==3 )
    {
        goto lbl_3;
    }
    if( state->rstate.stage==4 )
    {
        goto lbl_4;
    }
    
    /*
     * Routine body
     */
    
    /*
     * Prepare
     */
    n = state->n;
    m = state->m;
    state->repterminationtype = 0;
    state->repiterationscount = 0;
    state->repnfunc = 0;
    state->repnjac = 0;
    
    /*
     * Calculate F/G, initialize algorithm
     */
    nleq_clearrequestfields(state, _state);
    state->needf = ae_true;
    state->rstate.stage = 0;
    goto lbl_rcomm;
lbl_0:
    state->needf = ae_false;
    state->repnfunc = state->repnfunc+1;
    ae_v_move(&state->xbase.ptr.p_double[0], 1, &state->x.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->fbase = state->f;
    state->fprev = ae_maxrealnumber;
    if( !state->xrep )
    {
        goto lbl_5;
    }
    
    /*
     * progress report
     */
    nleq_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->rstate.stage = 1;
    goto lbl_rcomm;
lbl_1:
    state->xupdated = ae_false;
lbl_5:
    if( ae_fp_less_eq(state->f,ae_sqr(state->epsf, _state)) )
    {
        state->repterminationtype = 1;
        result = ae_false;
        return result;
    }
    
    /*
     * Main cycle
     */
    lambdaup = (double)(10);
    lambdadown = 0.3;
    lambdav = 0.001;
    rho = (double)(1);
lbl_7:
    if( ae_false )
    {
        goto lbl_8;
    }
    
    /*
     * Get Jacobian;
     * before we get to this point we already have State.XBase filled
     * with current point and State.FBase filled with function value
     * at XBase
     */
    nleq_clearrequestfields(state, _state);
    state->needfij = ae_true;
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->rstate.stage = 2;
    goto lbl_rcomm;
lbl_2:
    state->needfij = ae_false;
    state->repnfunc = state->repnfunc+1;
    state->repnjac = state->repnjac+1;
    rmatrixmv(n, m, &state->j, 0, 0, 1, &state->fi, 0, &state->rightpart, 0, _state);
    ae_v_muld(&state->rightpart.ptr.p_double[0], 1, ae_v_len(0,n-1), -1.0);
    
    /*
     * Inner cycle: find good lambda
     */
lbl_9:
    if( ae_false )
    {
        goto lbl_10;
    }
    
    /*
     * Solve (J^T*J + (Lambda+Mu)*I)*y = J^T*F
     * to get step d=-y where:
     * * Mu=||F|| - is damping parameter for nonlinear system
     * * Lambda   - is additional Levenberg-Marquardt parameter
     *              for better convergence when far away from minimum
     */
    for(i=0; i<=n-1; i++)
    {
        state->candstep.ptr.p_double[i] = (double)(0);
    }
    fblssolvecgx(&state->j, m, n, lambdav, &state->rightpart, &state->candstep, &state->cgbuf, _state);
    
    /*
     * Normalize step (it must be no more than StpMax)
     */
    stepnorm = (double)(0);
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_neq(state->candstep.ptr.p_double[i],(double)(0)) )
        {
            stepnorm = (double)(1);
            break;
        }
    }
    linminnormalized(&state->candstep, &stepnorm, n, _state);
    if( ae_fp_neq(state->stpmax,(double)(0)) )
    {
        stepnorm = ae_minreal(stepnorm, state->stpmax, _state);
    }
    
    /*
     * Test new step - is it good enough?
     * * if not, Lambda is increased and we try again.
     * * if step is good, we decrease Lambda and move on.
     *
     * We can break this cycle on two occasions:
     * * step is so small that x+step==x (in floating point arithmetics)
     * * lambda is so large
     */
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    ae_v_addd(&state->x.ptr.p_double[0], 1, &state->candstep.ptr.p_double[0], 1, ae_v_len(0,n-1), stepnorm);
    b = ae_true;
    for(i=0; i<=n-1; i++)
    {
        if( ae_fp_neq(state->x.ptr.p_double[i],state->xbase.ptr.p_double[i]) )
        {
            b = ae_false;
            break;
        }
    }
    if( b )
    {
        
        /*
         * Step is too small, force zero step and break
         */
        stepnorm = (double)(0);
        ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->f = state->fbase;
        goto lbl_10;
    }
    nleq_clearrequestfields(state, _state);
    state->needf = ae_true;
    state->rstate.stage = 3;
    goto lbl_rcomm;
lbl_3:
    state->needf = ae_false;
    state->repnfunc = state->repnfunc+1;
    if( ae_fp_less(state->f,state->fbase) )
    {
        
        /*
         * function value decreased, move on
         */
        nleq_decreaselambda(&lambdav, &rho, lambdadown, _state);
        goto lbl_10;
    }
    if( !nleq_increaselambda(&lambdav, &rho, lambdaup, _state) )
    {
        
        /*
         * Lambda is too large (near overflow), force zero step and break
         */
        stepnorm = (double)(0);
        ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
        state->f = state->fbase;
        goto lbl_10;
    }
    goto lbl_9;
lbl_10:
    
    /*
     * Accept step:
     * * new position
     * * new function value
     */
    state->fbase = state->f;
    ae_v_addd(&state->xbase.ptr.p_double[0], 1, &state->candstep.ptr.p_double[0], 1, ae_v_len(0,n-1), stepnorm);
    state->repiterationscount = state->repiterationscount+1;
    
    /*
     * Report new iteration
     */
    if( !state->xrep )
    {
        goto lbl_11;
    }
    nleq_clearrequestfields(state, _state);
    state->xupdated = ae_true;
    state->f = state->fbase;
    ae_v_move(&state->x.ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,n-1));
    state->rstate.stage = 4;
    goto lbl_rcomm;
lbl_4:
    state->xupdated = ae_false;
lbl_11:
    
    /*
     * Test stopping conditions on F, step (zero/non-zero) and MaxIts;
     * If one of the conditions is met, RepTerminationType is changed.
     */
    if( ae_fp_less_eq(ae_sqrt(state->f, _state),state->epsf) )
    {
        state->repterminationtype = 1;
    }
    if( ae_fp_eq(stepnorm,(double)(0))&&state->repterminationtype==0 )
    {
        state->repterminationtype = -4;
    }
    if( state->repiterationscount>=state->maxits&&state->maxits>0 )
    {
        state->repterminationtype = 5;
    }
    if( state->repterminationtype!=0 )
    {
        goto lbl_8;
    }
    
    /*
     * Now, iteration is finally over
     */
    goto lbl_7;
lbl_8:
    result = ae_false;
    return result;
    
    /*
     * Saving state
     */
lbl_rcomm:
    result = ae_true;
    state->rstate.ia.ptr.p_int[0] = n;
    state->rstate.ia.ptr.p_int[1] = m;
    state->rstate.ia.ptr.p_int[2] = i;
    state->rstate.ba.ptr.p_bool[0] = b;
    state->rstate.ra.ptr.p_double[0] = lambdaup;
    state->rstate.ra.ptr.p_double[1] = lambdadown;
    state->rstate.ra.ptr.p_double[2] = lambdav;
    state->rstate.ra.ptr.p_double[3] = rho;
    state->rstate.ra.ptr.p_double[4] = mu;
    state->rstate.ra.ptr.p_double[5] = stepnorm;
    return result;
}


/*************************************************************************
NLEQ solver results

INPUT PARAMETERS:
    State   -   algorithm state.

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -4    ERROR:  algorithm   has   converged   to   the
                            stationary point Xf which is local minimum  of
                            f=F[0]^2+...+F[m-1]^2, but is not solution  of
                            nonlinear system.
                    *  1    sqrt(f)<=EpsF.
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible
                * Rep.IterationsCount contains iterations count
                * NFEV countains number of function calculations
                * ActiveConstraints contains number of active constraints

  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
void nleqresults(const nleqstate* state,
     /* Real    */ ae_vector* x,
     nleqreport* rep,
     ae_state *_state)
{

    ae_vector_clear(x);
    _nleqreport_clear(rep);

    nleqresultsbuf(state, x, rep, _state);
}


/*************************************************************************
NLEQ solver results

Buffered implementation of NLEQResults(), which uses pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 20.08.2009 by Bochkanov Sergey
*************************************************************************/
void nleqresultsbuf(const nleqstate* state,
     /* Real    */ ae_vector* x,
     nleqreport* rep,
     ae_state *_state)
{


    if( x->cnt<state->n )
    {
        ae_vector_set_length(x, state->n, _state);
    }
    ae_v_move(&x->ptr.p_double[0], 1, &state->xbase.ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    rep->iterationscount = state->repiterationscount;
    rep->nfunc = state->repnfunc;
    rep->njac = state->repnjac;
    rep->terminationtype = state->repterminationtype;
}


/*************************************************************************
This  subroutine  restarts  CG  algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used for reverse communication previously
                allocated with MinCGCreate call.
    X       -   new starting point.
    BndL    -   new lower bounds
    BndU    -   new upper bounds

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void nleqrestartfrom(nleqstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state)
{


    ae_assert(x->cnt>=state->n, "NLEQRestartFrom: Length(X)<N!", _state);
    ae_assert(isfinitevector(x, state->n, _state), "NLEQRestartFrom: X contains infinite or NaN values!", _state);
    ae_v_move(&state->x.ptr.p_double[0], 1, &x->ptr.p_double[0], 1, ae_v_len(0,state->n-1));
    ae_vector_set_length(&state->rstate.ia, 2+1, _state);
    ae_vector_set_length(&state->rstate.ba, 0+1, _state);
    ae_vector_set_length(&state->rstate.ra, 5+1, _state);
    state->rstate.stage = -1;
    nleq_clearrequestfields(state, _state);
}


/*************************************************************************
Clears request fileds (to be sure that we don't forgot to clear something)
*************************************************************************/
static void nleq_clearrequestfields(nleqstate* state, ae_state *_state)
{


    state->needf = ae_false;
    state->needfij = ae_false;
    state->xupdated = ae_false;
}


/*************************************************************************
Increases lambda, returns False when there is a danger of overflow
*************************************************************************/
static ae_bool nleq_increaselambda(double* lambdav,
     double* nu,
     double lambdaup,
     ae_state *_state)
{
    double lnlambda;
    double lnnu;
    double lnlambdaup;
    double lnmax;
    ae_bool result;


    result = ae_false;
    lnlambda = ae_log(*lambdav, _state);
    lnlambdaup = ae_log(lambdaup, _state);
    lnnu = ae_log(*nu, _state);
    lnmax = 0.5*ae_log(ae_maxrealnumber, _state);
    if( ae_fp_greater(lnlambda+lnlambdaup+lnnu,lnmax) )
    {
        return result;
    }
    if( ae_fp_greater(lnnu+ae_log((double)(2), _state),lnmax) )
    {
        return result;
    }
    *lambdav = *lambdav*lambdaup*(*nu);
    *nu = *nu*(double)2;
    result = ae_true;
    return result;
}


/*************************************************************************
Decreases lambda, but leaves it unchanged when there is danger of underflow.
*************************************************************************/
static void nleq_decreaselambda(double* lambdav,
     double* nu,
     double lambdadown,
     ae_state *_state)
{


    *nu = (double)(1);
    if( ae_fp_less(ae_log(*lambdav, _state)+ae_log(lambdadown, _state),ae_log(ae_minrealnumber, _state)) )
    {
        *lambdav = ae_minrealnumber;
    }
    else
    {
        *lambdav = *lambdav*lambdadown;
    }
}


void _nleqstate_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    nleqstate *p = (nleqstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_init(&p->x, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->fi, 0, DT_REAL, _state, make_automatic);
    ae_matrix_init(&p->j, 0, 0, DT_REAL, _state, make_automatic);
    _rcommstate_init(&p->rstate, _state, make_automatic);
    ae_vector_init(&p->xbase, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->candstep, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->rightpart, 0, DT_REAL, _state, make_automatic);
    ae_vector_init(&p->cgbuf, 0, DT_REAL, _state, make_automatic);
}


void _nleqstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    nleqstate       *dst = (nleqstate*)_dst;
    const nleqstate *src = (const nleqstate*)_src;
    dst->n = src->n;
    dst->m = src->m;
    dst->epsf = src->epsf;
    dst->maxits = src->maxits;
    dst->xrep = src->xrep;
    dst->stpmax = src->stpmax;
    ae_vector_init_copy(&dst->x, &src->x, _state, make_automatic);
    dst->f = src->f;
    ae_vector_init_copy(&dst->fi, &src->fi, _state, make_automatic);
    ae_matrix_init_copy(&dst->j, &src->j, _state, make_automatic);
    dst->needf = src->needf;
    dst->needfij = src->needfij;
    dst->xupdated = src->xupdated;
    _rcommstate_init_copy(&dst->rstate, &src->rstate, _state, make_automatic);
    dst->repiterationscount = src->repiterationscount;
    dst->repnfunc = src->repnfunc;
    dst->repnjac = src->repnjac;
    dst->repterminationtype = src->repterminationtype;
    ae_vector_init_copy(&dst->xbase, &src->xbase, _state, make_automatic);
    dst->fbase = src->fbase;
    dst->fprev = src->fprev;
    ae_vector_init_copy(&dst->candstep, &src->candstep, _state, make_automatic);
    ae_vector_init_copy(&dst->rightpart, &src->rightpart, _state, make_automatic);
    ae_vector_init_copy(&dst->cgbuf, &src->cgbuf, _state, make_automatic);
}


void _nleqstate_clear(void* _p)
{
    nleqstate *p = (nleqstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_clear(&p->x);
    ae_vector_clear(&p->fi);
    ae_matrix_clear(&p->j);
    _rcommstate_clear(&p->rstate);
    ae_vector_clear(&p->xbase);
    ae_vector_clear(&p->candstep);
    ae_vector_clear(&p->rightpart);
    ae_vector_clear(&p->cgbuf);
}


void _nleqstate_destroy(void* _p)
{
    nleqstate *p = (nleqstate*)_p;
    ae_touch_ptr((void*)p);
    ae_vector_destroy(&p->x);
    ae_vector_destroy(&p->fi);
    ae_matrix_destroy(&p->j);
    _rcommstate_destroy(&p->rstate);
    ae_vector_destroy(&p->xbase);
    ae_vector_destroy(&p->candstep);
    ae_vector_destroy(&p->rightpart);
    ae_vector_destroy(&p->cgbuf);
}


void _nleqreport_init(void* _p, ae_state *_state, ae_bool make_automatic)
{
    nleqreport *p = (nleqreport*)_p;
    ae_touch_ptr((void*)p);
}


void _nleqreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic)
{
    nleqreport       *dst = (nleqreport*)_dst;
    const nleqreport *src = (const nleqreport*)_src;
    dst->iterationscount = src->iterationscount;
    dst->nfunc = src->nfunc;
    dst->njac = src->njac;
    dst->terminationtype = src->terminationtype;
}


void _nleqreport_clear(void* _p)
{
    nleqreport *p = (nleqreport*)_p;
    ae_touch_ptr((void*)p);
}


void _nleqreport_destroy(void* _p)
{
    nleqreport *p = (nleqreport*)_p;
    ae_touch_ptr((void*)p);
}


#endif

}

