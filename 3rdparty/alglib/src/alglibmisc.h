/*************************************************************************
ALGLIB 3.14.0 (source code generated 2018-06-16)
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
#ifndef _alglibmisc_pkg_h
#define _alglibmisc_pkg_h
#include "ap.h"
#include "alglibinternal.h"

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS COMPUTATIONAL CORE DECLARATIONS (DATATYPES)
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
#if defined(AE_COMPILE_NEARESTNEIGHBOR) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_vector x;
    ae_vector boxmin;
    ae_vector boxmax;
    ae_int_t kneeded;
    double rneeded;
    ae_bool selfmatch;
    double approxf;
    ae_int_t kcur;
    ae_vector idx;
    ae_vector r;
    ae_vector buf;
    ae_vector curboxmin;
    ae_vector curboxmax;
    double curdist;
} kdtreerequestbuffer;
typedef struct
{
    ae_int_t n;
    ae_int_t nx;
    ae_int_t ny;
    ae_int_t normtype;
    ae_matrix xy;
    ae_vector tags;
    ae_vector boxmin;
    ae_vector boxmax;
    ae_vector nodes;
    ae_vector splits;
    kdtreerequestbuffer innerbuf;
    ae_int_t debugcounter;
} kdtree;
#endif
#if defined(AE_COMPILE_HQRND) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t s1;
    ae_int_t s2;
    ae_int_t magicv;
} hqrndstate;
#endif
#if defined(AE_COMPILE_XDEBUG) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t i;
    ae_complex c;
    ae_vector a;
} xdebugrecord1;
#endif

}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS C++ INTERFACE
//
/////////////////////////////////////////////////////////////////////////
namespace alglib
{

#if defined(AE_COMPILE_NEARESTNEIGHBOR) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
Buffer object which is used to perform nearest neighbor  requests  in  the
multithreaded mode (multiple threads working with same KD-tree object).

This object should be created with KDTreeCreateBuffer().
*************************************************************************/
class _kdtreerequestbuffer_owner
{
public:
    _kdtreerequestbuffer_owner();
    _kdtreerequestbuffer_owner(const _kdtreerequestbuffer_owner &rhs);
    _kdtreerequestbuffer_owner& operator=(const _kdtreerequestbuffer_owner &rhs);
    virtual ~_kdtreerequestbuffer_owner();
    alglib_impl::kdtreerequestbuffer* c_ptr();
    alglib_impl::kdtreerequestbuffer* c_ptr() const;
protected:
    alglib_impl::kdtreerequestbuffer *p_struct;
};
class kdtreerequestbuffer : public _kdtreerequestbuffer_owner
{
public:
    kdtreerequestbuffer();
    kdtreerequestbuffer(const kdtreerequestbuffer &rhs);
    kdtreerequestbuffer& operator=(const kdtreerequestbuffer &rhs);
    virtual ~kdtreerequestbuffer();

};


/*************************************************************************
KD-tree object.
*************************************************************************/
class _kdtree_owner
{
public:
    _kdtree_owner();
    _kdtree_owner(const _kdtree_owner &rhs);
    _kdtree_owner& operator=(const _kdtree_owner &rhs);
    virtual ~_kdtree_owner();
    alglib_impl::kdtree* c_ptr();
    alglib_impl::kdtree* c_ptr() const;
protected:
    alglib_impl::kdtree *p_struct;
};
class kdtree : public _kdtree_owner
{
public:
    kdtree();
    kdtree(const kdtree &rhs);
    kdtree& operator=(const kdtree &rhs);
    virtual ~kdtree();

};
#endif

#if defined(AE_COMPILE_HQRND) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
Portable high quality random number generator state.
Initialized with HQRNDRandomize() or HQRNDSeed().

Fields:
    S1, S2      -   seed values
    V           -   precomputed value
    MagicV      -   'magic' value used to determine whether State structure
                    was correctly initialized.
*************************************************************************/
class _hqrndstate_owner
{
public:
    _hqrndstate_owner();
    _hqrndstate_owner(const _hqrndstate_owner &rhs);
    _hqrndstate_owner& operator=(const _hqrndstate_owner &rhs);
    virtual ~_hqrndstate_owner();
    alglib_impl::hqrndstate* c_ptr();
    alglib_impl::hqrndstate* c_ptr() const;
protected:
    alglib_impl::hqrndstate *p_struct;
};
class hqrndstate : public _hqrndstate_owner
{
public:
    hqrndstate();
    hqrndstate(const hqrndstate &rhs);
    hqrndstate& operator=(const hqrndstate &rhs);
    virtual ~hqrndstate();

};
#endif

#if defined(AE_COMPILE_XDEBUG) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************

*************************************************************************/
class _xdebugrecord1_owner
{
public:
    _xdebugrecord1_owner();
    _xdebugrecord1_owner(const _xdebugrecord1_owner &rhs);
    _xdebugrecord1_owner& operator=(const _xdebugrecord1_owner &rhs);
    virtual ~_xdebugrecord1_owner();
    alglib_impl::xdebugrecord1* c_ptr();
    alglib_impl::xdebugrecord1* c_ptr() const;
protected:
    alglib_impl::xdebugrecord1 *p_struct;
};
class xdebugrecord1 : public _xdebugrecord1_owner
{
public:
    xdebugrecord1();
    xdebugrecord1(const xdebugrecord1 &rhs);
    xdebugrecord1& operator=(const xdebugrecord1 &rhs);
    virtual ~xdebugrecord1();
    ae_int_t &i;
    alglib::complex &c;
    real_1d_array a;

};
#endif

#if defined(AE_COMPILE_NEARESTNEIGHBOR) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This function serializes data structure to string.

Important properties of s_out:
* it contains alphanumeric characters, dots, underscores, minus signs
* these symbols are grouped into words, which are separated by spaces
  and Windows-style (CR+LF) newlines
* although  serializer  uses  spaces and CR+LF as separators, you can 
  replace any separator character by arbitrary combination of spaces,
  tabs, Windows or Unix newlines. It allows flexible reformatting  of
  the  string  in  case you want to include it into text or XML file. 
  But you should not insert separators into the middle of the "words"
  nor you should change case of letters.
* s_out can be freely moved between 32-bit and 64-bit systems, little
  and big endian machines, and so on. You can serialize structure  on
  32-bit machine and unserialize it on 64-bit one (or vice versa), or
  serialize  it  on  SPARC  and  unserialize  on  x86.  You  can also 
  serialize  it  in  C++ version of ALGLIB and unserialize in C# one, 
  and vice versa.
*************************************************************************/
void kdtreeserialize(kdtree &obj, std::string &s_out);


/*************************************************************************
This function unserializes data structure from string.
*************************************************************************/
void kdtreeunserialize(const std::string &s_in, kdtree &obj);




/*************************************************************************
This function serializes data structure to C++ stream.

Data stream generated by this function is same as  string  representation
generated  by  string  version  of  serializer - alphanumeric characters,
dots, underscores, minus signs, which are grouped into words separated by
spaces and CR+LF.

We recommend you to read comments on string version of serializer to find
out more about serialization of AlGLIB objects.
*************************************************************************/
void kdtreeserialize(kdtree &obj, std::ostream &s_out);


/*************************************************************************
This function unserializes data structure from stream.
*************************************************************************/
void kdtreeunserialize(const std::istream &s_in, kdtree &obj);


/*************************************************************************
KD-tree creation

This subroutine creates KD-tree from set of X-values and optional Y-values

INPUT PARAMETERS
    XY      -   dataset, array[0..N-1,0..NX+NY-1].
                one row corresponds to one point.
                first NX columns contain X-values, next NY (NY may be zero)
                columns may contain associated Y-values
    N       -   number of points, N>=0.
    NX      -   space dimension, NX>=1.
    NY      -   number of optional Y-values, NY>=0.
    NormType-   norm type:
                * 0 denotes infinity-norm
                * 1 denotes 1-norm
                * 2 denotes 2-norm (Euclidean norm)

OUTPUT PARAMETERS
    KDT     -   KD-tree


NOTES

1. KD-tree  creation  have O(N*logN) complexity and O(N*(2*NX+NY))  memory
   requirements.
2. Although KD-trees may be used with any combination of N  and  NX,  they
   are more efficient than brute-force search only when N >> 4^NX. So they
   are most useful in low-dimensional tasks (NX=2, NX=3). NX=1  is another
   inefficient case, because  simple  binary  search  (without  additional
   structures) is much more efficient in such tasks than KD-trees.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreebuild(const real_2d_array &xy, const ae_int_t n, const ae_int_t nx, const ae_int_t ny, const ae_int_t normtype, kdtree &kdt, const xparams _xparams = alglib::xdefault);
void kdtreebuild(const real_2d_array &xy, const ae_int_t nx, const ae_int_t ny, const ae_int_t normtype, kdtree &kdt, const xparams _xparams = alglib::xdefault);


/*************************************************************************
KD-tree creation

This  subroutine  creates  KD-tree  from set of X-values, integer tags and
optional Y-values

INPUT PARAMETERS
    XY      -   dataset, array[0..N-1,0..NX+NY-1].
                one row corresponds to one point.
                first NX columns contain X-values, next NY (NY may be zero)
                columns may contain associated Y-values
    Tags    -   tags, array[0..N-1], contains integer tags associated
                with points.
    N       -   number of points, N>=0
    NX      -   space dimension, NX>=1.
    NY      -   number of optional Y-values, NY>=0.
    NormType-   norm type:
                * 0 denotes infinity-norm
                * 1 denotes 1-norm
                * 2 denotes 2-norm (Euclidean norm)

OUTPUT PARAMETERS
    KDT     -   KD-tree

NOTES

1. KD-tree  creation  have O(N*logN) complexity and O(N*(2*NX+NY))  memory
   requirements.
2. Although KD-trees may be used with any combination of N  and  NX,  they
   are more efficient than brute-force search only when N >> 4^NX. So they
   are most useful in low-dimensional tasks (NX=2, NX=3). NX=1  is another
   inefficient case, because  simple  binary  search  (without  additional
   structures) is much more efficient in such tasks than KD-trees.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreebuildtagged(const real_2d_array &xy, const integer_1d_array &tags, const ae_int_t n, const ae_int_t nx, const ae_int_t ny, const ae_int_t normtype, kdtree &kdt, const xparams _xparams = alglib::xdefault);
void kdtreebuildtagged(const real_2d_array &xy, const integer_1d_array &tags, const ae_int_t nx, const ae_int_t ny, const ae_int_t normtype, kdtree &kdt, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function creates buffer  structure  which  can  be  used  to  perform
parallel KD-tree requests.

KD-tree subpackage provides two sets of request functions - ones which use
internal buffer of KD-tree object  (these  functions  are  single-threaded
because they use same buffer, which can not shared between  threads),  and
ones which use external buffer.

This function is used to initialize external buffer.

INPUT PARAMETERS
    KDT         -   KD-tree which is associated with newly created buffer

OUTPUT PARAMETERS
    Buf         -   external buffer.


IMPORTANT: KD-tree buffer should be used only with  KD-tree  object  which
           was used to initialize buffer. Any attempt to use biffer   with
           different object is dangerous - you  may  get  integrity  check
           failure (exception) because sizes of internal arrays do not fit
           to dimensions of KD-tree structure.

  -- ALGLIB --
     Copyright 18.03.2016 by Bochkanov Sergey
*************************************************************************/
void kdtreecreaterequestbuffer(const kdtree &kdt, kdtreerequestbuffer &buf, const xparams _xparams = alglib::xdefault);


/*************************************************************************
K-NN query: K nearest neighbors

IMPORTANT: this function can not be used in multithreaded code because  it
           uses internal temporary buffer of kd-tree object, which can not
           be shared between multiple threads.  If  you  want  to  perform
           parallel requests, use function  which  uses  external  request
           buffer: KDTreeTsQueryKNN() ("Ts" stands for "thread-safe").

INPUT PARAMETERS
    KDT         -   KD-tree
    X           -   point, array[0..NX-1].
    K           -   number of neighbors to return, K>=1
    SelfMatch   -   whether self-matches are allowed:
                    * if True, nearest neighbor may be the point itself
                      (if it exists in original dataset)
                    * if False, then only points with non-zero distance
                      are returned
                    * if not given, considered True

RESULT
    number of actual neighbors found (either K or N, if K>N).

This  subroutine  performs  query  and  stores  its result in the internal
structures of the KD-tree. You can use  following  subroutines  to  obtain
these results:
* KDTreeQueryResultsX() to get X-values
* KDTreeQueryResultsXY() to get X- and Y-values
* KDTreeQueryResultsTags() to get tag values
* KDTreeQueryResultsDistances() to get distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreequeryknn(const kdtree &kdt, const real_1d_array &x, const ae_int_t k, const bool selfmatch, const xparams _xparams = alglib::xdefault);
ae_int_t kdtreequeryknn(const kdtree &kdt, const real_1d_array &x, const ae_int_t k, const xparams _xparams = alglib::xdefault);


/*************************************************************************
K-NN query: K nearest neighbors, using external thread-local buffer.

You can call this function from multiple threads for same kd-tree instance,
assuming that different instances of buffer object are passed to different
threads.

INPUT PARAMETERS
    KDT         -   kd-tree
    Buf         -   request buffer  object  created  for  this  particular
                    instance of kd-tree structure with kdtreecreaterequestbuffer()
                    function.
    X           -   point, array[0..NX-1].
    K           -   number of neighbors to return, K>=1
    SelfMatch   -   whether self-matches are allowed:
                    * if True, nearest neighbor may be the point itself
                      (if it exists in original dataset)
                    * if False, then only points with non-zero distance
                      are returned
                    * if not given, considered True

RESULT
    number of actual neighbors found (either K or N, if K>N).

This  subroutine  performs  query  and  stores  its result in the internal
structures  of  the  buffer object. You can use following  subroutines  to
obtain these results (pay attention to "buf" in their names):
* KDTreeTsQueryResultsX() to get X-values
* KDTreeTsQueryResultsXY() to get X- and Y-values
* KDTreeTsQueryResultsTags() to get tag values
* KDTreeTsQueryResultsDistances() to get distances

IMPORTANT: kd-tree buffer should be used only with  KD-tree  object  which
           was used to initialize buffer. Any attempt to use biffer   with
           different object is dangerous - you  may  get  integrity  check
           failure (exception) because sizes of internal arrays do not fit
           to dimensions of KD-tree structure.

  -- ALGLIB --
     Copyright 18.03.2016 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreetsqueryknn(const kdtree &kdt, const kdtreerequestbuffer &buf, const real_1d_array &x, const ae_int_t k, const bool selfmatch, const xparams _xparams = alglib::xdefault);
ae_int_t kdtreetsqueryknn(const kdtree &kdt, const kdtreerequestbuffer &buf, const real_1d_array &x, const ae_int_t k, const xparams _xparams = alglib::xdefault);


/*************************************************************************
R-NN query: all points within R-sphere centered at X

IMPORTANT: this function can not be used in multithreaded code because  it
           uses internal temporary buffer of kd-tree object, which can not
           be shared between multiple threads.  If  you  want  to  perform
           parallel requests, use function  which  uses  external  request
           buffer: KDTreeTsQueryRNN() ("Ts" stands for "thread-safe").

INPUT PARAMETERS
    KDT         -   KD-tree
    X           -   point, array[0..NX-1].
    R           -   radius of sphere (in corresponding norm), R>0
    SelfMatch   -   whether self-matches are allowed:
                    * if True, nearest neighbor may be the point itself
                      (if it exists in original dataset)
                    * if False, then only points with non-zero distance
                      are returned
                    * if not given, considered True

RESULT
    number of neighbors found, >=0

This  subroutine  performs  query  and  stores  its result in the internal
structures of the KD-tree. You can use  following  subroutines  to  obtain
actual results:
* KDTreeQueryResultsX() to get X-values
* KDTreeQueryResultsXY() to get X- and Y-values
* KDTreeQueryResultsTags() to get tag values
* KDTreeQueryResultsDistances() to get distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreequeryrnn(const kdtree &kdt, const real_1d_array &x, const double r, const bool selfmatch, const xparams _xparams = alglib::xdefault);
ae_int_t kdtreequeryrnn(const kdtree &kdt, const real_1d_array &x, const double r, const xparams _xparams = alglib::xdefault);


/*************************************************************************
R-NN query: all points within  R-sphere  centered  at  X,  using  external
thread-local buffer.

You can call this function from multiple threads for same kd-tree instance,
assuming that different instances of buffer object are passed to different
threads.

INPUT PARAMETERS
    KDT         -   KD-tree
    Buf         -   request buffer  object  created  for  this  particular
                    instance of kd-tree structure with kdtreecreaterequestbuffer()
                    function.
    X           -   point, array[0..NX-1].
    R           -   radius of sphere (in corresponding norm), R>0
    SelfMatch   -   whether self-matches are allowed:
                    * if True, nearest neighbor may be the point itself
                      (if it exists in original dataset)
                    * if False, then only points with non-zero distance
                      are returned
                    * if not given, considered True

RESULT
    number of neighbors found, >=0

This  subroutine  performs  query  and  stores  its result in the internal
structures  of  the  buffer object. You can use following  subroutines  to
obtain these results (pay attention to "buf" in their names):
* KDTreeTsQueryResultsX() to get X-values
* KDTreeTsQueryResultsXY() to get X- and Y-values
* KDTreeTsQueryResultsTags() to get tag values
* KDTreeTsQueryResultsDistances() to get distances

IMPORTANT: kd-tree buffer should be used only with  KD-tree  object  which
           was used to initialize buffer. Any attempt to use biffer   with
           different object is dangerous - you  may  get  integrity  check
           failure (exception) because sizes of internal arrays do not fit
           to dimensions of KD-tree structure.

  -- ALGLIB --
     Copyright 18.03.2016 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreetsqueryrnn(const kdtree &kdt, const kdtreerequestbuffer &buf, const real_1d_array &x, const double r, const bool selfmatch, const xparams _xparams = alglib::xdefault);
ae_int_t kdtreetsqueryrnn(const kdtree &kdt, const kdtreerequestbuffer &buf, const real_1d_array &x, const double r, const xparams _xparams = alglib::xdefault);


/*************************************************************************
K-NN query: approximate K nearest neighbors

IMPORTANT: this function can not be used in multithreaded code because  it
           uses internal temporary buffer of kd-tree object, which can not
           be shared between multiple threads.  If  you  want  to  perform
           parallel requests, use function  which  uses  external  request
           buffer: KDTreeTsQueryAKNN() ("Ts" stands for "thread-safe").

INPUT PARAMETERS
    KDT         -   KD-tree
    X           -   point, array[0..NX-1].
    K           -   number of neighbors to return, K>=1
    SelfMatch   -   whether self-matches are allowed:
                    * if True, nearest neighbor may be the point itself
                      (if it exists in original dataset)
                    * if False, then only points with non-zero distance
                      are returned
                    * if not given, considered True
    Eps         -   approximation factor, Eps>=0. eps-approximate  nearest
                    neighbor  is  a  neighbor  whose distance from X is at
                    most (1+eps) times distance of true nearest neighbor.

RESULT
    number of actual neighbors found (either K or N, if K>N).

NOTES
    significant performance gain may be achieved only when Eps  is  is  on
    the order of magnitude of 1 or larger.

This  subroutine  performs  query  and  stores  its result in the internal
structures of the KD-tree. You can use  following  subroutines  to  obtain
these results:
* KDTreeQueryResultsX() to get X-values
* KDTreeQueryResultsXY() to get X- and Y-values
* KDTreeQueryResultsTags() to get tag values
* KDTreeQueryResultsDistances() to get distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreequeryaknn(const kdtree &kdt, const real_1d_array &x, const ae_int_t k, const bool selfmatch, const double eps, const xparams _xparams = alglib::xdefault);
ae_int_t kdtreequeryaknn(const kdtree &kdt, const real_1d_array &x, const ae_int_t k, const double eps, const xparams _xparams = alglib::xdefault);


/*************************************************************************
K-NN query: approximate K nearest neighbors, using thread-local buffer.

You can call this function from multiple threads for same kd-tree instance,
assuming that different instances of buffer object are passed to different
threads.

INPUT PARAMETERS
    KDT         -   KD-tree
    Buf         -   request buffer  object  created  for  this  particular
                    instance of kd-tree structure with kdtreecreaterequestbuffer()
                    function.
    X           -   point, array[0..NX-1].
    K           -   number of neighbors to return, K>=1
    SelfMatch   -   whether self-matches are allowed:
                    * if True, nearest neighbor may be the point itself
                      (if it exists in original dataset)
                    * if False, then only points with non-zero distance
                      are returned
                    * if not given, considered True
    Eps         -   approximation factor, Eps>=0. eps-approximate  nearest
                    neighbor  is  a  neighbor  whose distance from X is at
                    most (1+eps) times distance of true nearest neighbor.

RESULT
    number of actual neighbors found (either K or N, if K>N).

NOTES
    significant performance gain may be achieved only when Eps  is  is  on
    the order of magnitude of 1 or larger.

This  subroutine  performs  query  and  stores  its result in the internal
structures  of  the  buffer object. You can use following  subroutines  to
obtain these results (pay attention to "buf" in their names):
* KDTreeTsQueryResultsX() to get X-values
* KDTreeTsQueryResultsXY() to get X- and Y-values
* KDTreeTsQueryResultsTags() to get tag values
* KDTreeTsQueryResultsDistances() to get distances

IMPORTANT: kd-tree buffer should be used only with  KD-tree  object  which
           was used to initialize buffer. Any attempt to use biffer   with
           different object is dangerous - you  may  get  integrity  check
           failure (exception) because sizes of internal arrays do not fit
           to dimensions of KD-tree structure.

  -- ALGLIB --
     Copyright 18.03.2016 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreetsqueryaknn(const kdtree &kdt, const kdtreerequestbuffer &buf, const real_1d_array &x, const ae_int_t k, const bool selfmatch, const double eps, const xparams _xparams = alglib::xdefault);
ae_int_t kdtreetsqueryaknn(const kdtree &kdt, const kdtreerequestbuffer &buf, const real_1d_array &x, const ae_int_t k, const double eps, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Box query: all points within user-specified box.

IMPORTANT: this function can not be used in multithreaded code because  it
           uses internal temporary buffer of kd-tree object, which can not
           be shared between multiple threads.  If  you  want  to  perform
           parallel requests, use function  which  uses  external  request
           buffer: KDTreeTsQueryBox() ("Ts" stands for "thread-safe").

INPUT PARAMETERS
    KDT         -   KD-tree
    BoxMin      -   lower bounds, array[0..NX-1].
    BoxMax      -   upper bounds, array[0..NX-1].


RESULT
    number of actual neighbors found (in [0,N]).

This  subroutine  performs  query  and  stores  its result in the internal
structures of the KD-tree. You can use  following  subroutines  to  obtain
these results:
* KDTreeQueryResultsX() to get X-values
* KDTreeQueryResultsXY() to get X- and Y-values
* KDTreeQueryResultsTags() to get tag values
* KDTreeQueryResultsDistances() returns zeros for this request

NOTE: this particular query returns unordered results, because there is no
      meaningful way of  ordering  points.  Furthermore,  no 'distance' is
      associated with points - it is either INSIDE  or OUTSIDE (so request
      for distances will return zeros).

  -- ALGLIB --
     Copyright 14.05.2016 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreequerybox(const kdtree &kdt, const real_1d_array &boxmin, const real_1d_array &boxmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Box query: all points within user-specified box, using thread-local buffer.

You can call this function from multiple threads for same kd-tree instance,
assuming that different instances of buffer object are passed to different
threads.

INPUT PARAMETERS
    KDT         -   KD-tree
    Buf         -   request buffer  object  created  for  this  particular
                    instance of kd-tree structure with kdtreecreaterequestbuffer()
                    function.
    BoxMin      -   lower bounds, array[0..NX-1].
    BoxMax      -   upper bounds, array[0..NX-1].

RESULT
    number of actual neighbors found (in [0,N]).

This  subroutine  performs  query  and  stores  its result in the internal
structures  of  the  buffer object. You can use following  subroutines  to
obtain these results (pay attention to "ts" in their names):
* KDTreeTsQueryResultsX() to get X-values
* KDTreeTsQueryResultsXY() to get X- and Y-values
* KDTreeTsQueryResultsTags() to get tag values
* KDTreeTsQueryResultsDistances() returns zeros for this query

NOTE: this particular query returns unordered results, because there is no
      meaningful way of  ordering  points.  Furthermore,  no 'distance' is
      associated with points - it is either INSIDE  or OUTSIDE (so request
      for distances will return zeros).

IMPORTANT: kd-tree buffer should be used only with  KD-tree  object  which
           was used to initialize buffer. Any attempt to use biffer   with
           different object is dangerous - you  may  get  integrity  check
           failure (exception) because sizes of internal arrays do not fit
           to dimensions of KD-tree structure.

  -- ALGLIB --
     Copyright 14.05.2016 by Bochkanov Sergey
*************************************************************************/
ae_int_t kdtreetsquerybox(const kdtree &kdt, const kdtreerequestbuffer &buf, const real_1d_array &boxmin, const real_1d_array &boxmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
X-values from last query.

This function retuns results stored in  the  internal  buffer  of  kd-tree
object. If you performed buffered requests (ones which  use  instances  of
kdtreerequestbuffer class), you  should  call  buffered  version  of  this
function - kdtreetsqueryresultsx().

INPUT PARAMETERS
    KDT     -   KD-tree
    X       -   possibly pre-allocated buffer. If X is too small to store
                result, it is resized. If size(X) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    X       -   rows are filled with X-values

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsXY()            X- and Y-values
* KDTreeQueryResultsTags()          tag values
* KDTreeQueryResultsDistances()     distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultsx(const kdtree &kdt, real_2d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
X- and Y-values from last query

This function retuns results stored in  the  internal  buffer  of  kd-tree
object. If you performed buffered requests (ones which  use  instances  of
kdtreerequestbuffer class), you  should  call  buffered  version  of  this
function - kdtreetsqueryresultsxy().

INPUT PARAMETERS
    KDT     -   KD-tree
    XY      -   possibly pre-allocated buffer. If XY is too small to store
                result, it is resized. If size(XY) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    XY      -   rows are filled with points: first NX columns with
                X-values, next NY columns - with Y-values.

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsX()             X-values
* KDTreeQueryResultsTags()          tag values
* KDTreeQueryResultsDistances()     distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultsxy(const kdtree &kdt, real_2d_array &xy, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Tags from last query

This function retuns results stored in  the  internal  buffer  of  kd-tree
object. If you performed buffered requests (ones which  use  instances  of
kdtreerequestbuffer class), you  should  call  buffered  version  of  this
function - kdtreetsqueryresultstags().

INPUT PARAMETERS
    KDT     -   KD-tree
    Tags    -   possibly pre-allocated buffer. If X is too small to store
                result, it is resized. If size(X) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    Tags    -   filled with tags associated with points,
                or, when no tags were supplied, with zeros

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsX()             X-values
* KDTreeQueryResultsXY()            X- and Y-values
* KDTreeQueryResultsDistances()     distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultstags(const kdtree &kdt, integer_1d_array &tags, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Distances from last query

This function retuns results stored in  the  internal  buffer  of  kd-tree
object. If you performed buffered requests (ones which  use  instances  of
kdtreerequestbuffer class), you  should  call  buffered  version  of  this
function - kdtreetsqueryresultsdistances().

INPUT PARAMETERS
    KDT     -   KD-tree
    R       -   possibly pre-allocated buffer. If X is too small to store
                result, it is resized. If size(X) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    R       -   filled with distances (in corresponding norm)

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsX()             X-values
* KDTreeQueryResultsXY()            X- and Y-values
* KDTreeQueryResultsTags()          tag values

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultsdistances(const kdtree &kdt, real_1d_array &r, const xparams _xparams = alglib::xdefault);


/*************************************************************************
X-values from last query associated with kdtreerequestbuffer object.

INPUT PARAMETERS
    KDT     -   KD-tree
    Buf     -   request  buffer  object  created   for   this   particular
                instance of kd-tree structure.
    X       -   possibly pre-allocated buffer. If X is too small to store
                result, it is resized. If size(X) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    X       -   rows are filled with X-values

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsXY()            X- and Y-values
* KDTreeQueryResultsTags()          tag values
* KDTreeQueryResultsDistances()     distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreetsqueryresultsx(const kdtree &kdt, const kdtreerequestbuffer &buf, real_2d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
X- and Y-values from last query associated with kdtreerequestbuffer object.

INPUT PARAMETERS
    KDT     -   KD-tree
    Buf     -   request  buffer  object  created   for   this   particular
                instance of kd-tree structure.
    XY      -   possibly pre-allocated buffer. If XY is too small to store
                result, it is resized. If size(XY) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    XY      -   rows are filled with points: first NX columns with
                X-values, next NY columns - with Y-values.

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsX()             X-values
* KDTreeQueryResultsTags()          tag values
* KDTreeQueryResultsDistances()     distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreetsqueryresultsxy(const kdtree &kdt, const kdtreerequestbuffer &buf, real_2d_array &xy, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Tags from last query associated with kdtreerequestbuffer object.

This function retuns results stored in  the  internal  buffer  of  kd-tree
object. If you performed buffered requests (ones which  use  instances  of
kdtreerequestbuffer class), you  should  call  buffered  version  of  this
function - KDTreeTsqueryresultstags().

INPUT PARAMETERS
    KDT     -   KD-tree
    Buf     -   request  buffer  object  created   for   this   particular
                instance of kd-tree structure.
    Tags    -   possibly pre-allocated buffer. If X is too small to store
                result, it is resized. If size(X) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    Tags    -   filled with tags associated with points,
                or, when no tags were supplied, with zeros

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsX()             X-values
* KDTreeQueryResultsXY()            X- and Y-values
* KDTreeQueryResultsDistances()     distances

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreetsqueryresultstags(const kdtree &kdt, const kdtreerequestbuffer &buf, integer_1d_array &tags, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Distances from last query associated with kdtreerequestbuffer object.

This function retuns results stored in  the  internal  buffer  of  kd-tree
object. If you performed buffered requests (ones which  use  instances  of
kdtreerequestbuffer class), you  should  call  buffered  version  of  this
function - KDTreeTsqueryresultsdistances().

INPUT PARAMETERS
    KDT     -   KD-tree
    Buf     -   request  buffer  object  created   for   this   particular
                instance of kd-tree structure.
    R       -   possibly pre-allocated buffer. If X is too small to store
                result, it is resized. If size(X) is enough to store
                result, it is left unchanged.

OUTPUT PARAMETERS
    R       -   filled with distances (in corresponding norm)

NOTES
1. points are ordered by distance from the query point (first = closest)
2. if  XY is larger than required to store result, only leading part  will
   be overwritten; trailing part will be left unchanged. So  if  on  input
   XY = [[A,B],[C,D]], and result is [1,2],  then  on  exit  we  will  get
   XY = [[1,2],[C,D]]. This is done purposely to increase performance;  if
   you want function  to  resize  array  according  to  result  size,  use
   function with same name and suffix 'I'.

SEE ALSO
* KDTreeQueryResultsX()             X-values
* KDTreeQueryResultsXY()            X- and Y-values
* KDTreeQueryResultsTags()          tag values

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreetsqueryresultsdistances(const kdtree &kdt, const kdtreerequestbuffer &buf, real_1d_array &r, const xparams _xparams = alglib::xdefault);


/*************************************************************************
X-values from last query; 'interactive' variant for languages like  Python
which   support    constructs   like  "X = KDTreeQueryResultsXI(KDT)"  and
interactive mode of interpreter.

This function allocates new array on each call,  so  it  is  significantly
slower than its 'non-interactive' counterpart, but it is  more  convenient
when you call it from command line.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultsxi(const kdtree &kdt, real_2d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
XY-values from last query; 'interactive' variant for languages like Python
which   support    constructs   like "XY = KDTreeQueryResultsXYI(KDT)" and
interactive mode of interpreter.

This function allocates new array on each call,  so  it  is  significantly
slower than its 'non-interactive' counterpart, but it is  more  convenient
when you call it from command line.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultsxyi(const kdtree &kdt, real_2d_array &xy, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Tags  from  last  query;  'interactive' variant for languages like  Python
which  support  constructs  like "Tags = KDTreeQueryResultsTagsI(KDT)" and
interactive mode of interpreter.

This function allocates new array on each call,  so  it  is  significantly
slower than its 'non-interactive' counterpart, but it is  more  convenient
when you call it from command line.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultstagsi(const kdtree &kdt, integer_1d_array &tags, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Distances from last query; 'interactive' variant for languages like Python
which  support  constructs   like  "R = KDTreeQueryResultsDistancesI(KDT)"
and interactive mode of interpreter.

This function allocates new array on each call,  so  it  is  significantly
slower than its 'non-interactive' counterpart, but it is  more  convenient
when you call it from command line.

  -- ALGLIB --
     Copyright 28.02.2010 by Bochkanov Sergey
*************************************************************************/
void kdtreequeryresultsdistancesi(const kdtree &kdt, real_1d_array &r, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_HQRND) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
HQRNDState  initialization  with  random  values  which come from standard
RNG.

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
void hqrndrandomize(hqrndstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
HQRNDState initialization with seed values

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
void hqrndseed(const ae_int_t s1, const ae_int_t s2, hqrndstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function generates random real number in (0,1),
not including interval boundaries

State structure must be initialized with HQRNDRandomize() or HQRNDSeed().

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
double hqrnduniformr(const hqrndstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function generates random integer number in [0, N)

1. State structure must be initialized with HQRNDRandomize() or HQRNDSeed()
2. N can be any positive number except for very large numbers:
   * close to 2^31 on 32-bit systems
   * close to 2^62 on 64-bit systems
   An exception will be generated if N is too large.

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
ae_int_t hqrnduniformi(const hqrndstate &state, const ae_int_t n, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Random number generator: normal numbers

This function generates one random number from normal distribution.
Its performance is equal to that of HQRNDNormal2()

State structure must be initialized with HQRNDRandomize() or HQRNDSeed().

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
double hqrndnormal(const hqrndstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Random number generator: random X and Y such that X^2+Y^2=1

State structure must be initialized with HQRNDRandomize() or HQRNDSeed().

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
void hqrndunit2(const hqrndstate &state, double &x, double &y, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Random number generator: normal numbers

This function generates two independent random numbers from normal
distribution. Its performance is equal to that of HQRNDNormal()

State structure must be initialized with HQRNDRandomize() or HQRNDSeed().

  -- ALGLIB --
     Copyright 02.12.2009 by Bochkanov Sergey
*************************************************************************/
void hqrndnormal2(const hqrndstate &state, double &x1, double &x2, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Random number generator: exponential distribution

State structure must be initialized with HQRNDRandomize() or HQRNDSeed().

  -- ALGLIB --
     Copyright 11.08.2007 by Bochkanov Sergey
*************************************************************************/
double hqrndexponential(const hqrndstate &state, const double lambdav, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function generates  random number from discrete distribution given by
finite sample X.

INPUT PARAMETERS
    State   -   high quality random number generator, must be
                initialized with HQRNDRandomize() or HQRNDSeed().
        X   -   finite sample
        N   -   number of elements to use, N>=1

RESULT
    this function returns one of the X[i] for random i=0..N-1

  -- ALGLIB --
     Copyright 08.11.2011 by Bochkanov Sergey
*************************************************************************/
double hqrnddiscrete(const hqrndstate &state, const real_1d_array &x, const ae_int_t n, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function generates random number from continuous  distribution  given
by finite sample X.

INPUT PARAMETERS
    State   -   high quality random number generator, must be
                initialized with HQRNDRandomize() or HQRNDSeed().
        X   -   finite sample, array[N] (can be larger, in this  case only
                leading N elements are used). THIS ARRAY MUST BE SORTED BY
                ASCENDING.
        N   -   number of elements to use, N>=1

RESULT
    this function returns random number from continuous distribution which
    tries to approximate X as mush as possible. min(X)<=Result<=max(X).

  -- ALGLIB --
     Copyright 08.11.2011 by Bochkanov Sergey
*************************************************************************/
double hqrndcontinuous(const hqrndstate &state, const real_1d_array &x, const ae_int_t n, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_XDEBUG) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Creates and returns XDebugRecord1 structure:
* integer and complex fields of Rec1 are set to 1 and 1+i correspondingly
* array field of Rec1 is set to [2,3]

  -- ALGLIB --
     Copyright 27.05.2014 by Bochkanov Sergey
*************************************************************************/
void xdebuginitrecord1(xdebugrecord1 &rec1, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Counts number of True values in the boolean 1D array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
ae_int_t xdebugb1count(const boolean_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by NOT(a[i]).
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugb1not(const boolean_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Appends copy of array to itself.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugb1appendcopy(boolean_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate N-element array with even-numbered elements set to True.
Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugb1outeven(const ae_int_t n, boolean_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of elements in the array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
ae_int_t xdebugi1sum(const integer_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by -A[I]
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugi1neg(const integer_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Appends copy of array to itself.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugi1appendcopy(integer_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate N-element array with even-numbered A[I] set to I, and odd-numbered
ones set to 0.

Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugi1outeven(const ae_int_t n, integer_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of elements in the array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
double xdebugr1sum(const real_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by -A[I]
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugr1neg(const real_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Appends copy of array to itself.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugr1appendcopy(real_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate N-element array with even-numbered A[I] set to I*0.25,
and odd-numbered ones are set to 0.

Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugr1outeven(const ae_int_t n, real_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of elements in the array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
alglib::complex xdebugc1sum(const complex_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by -A[I]
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugc1neg(const complex_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Appends copy of array to itself.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugc1appendcopy(complex_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate N-element array with even-numbered A[K] set to (x,y) = (K*0.25, K*0.125)
and odd-numbered ones are set to 0.

Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugc1outeven(const ae_int_t n, complex_1d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Counts number of True values in the boolean 2D array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
ae_int_t xdebugb2count(const boolean_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by NOT(a[i]).
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugb2not(const boolean_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Transposes array.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugb2transpose(boolean_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate MxN matrix with elements set to "Sin(3*I+5*J)>0"
Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugb2outsin(const ae_int_t m, const ae_int_t n, boolean_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of elements in the array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
ae_int_t xdebugi2sum(const integer_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by -a[i,j]
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugi2neg(const integer_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Transposes array.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugi2transpose(integer_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate MxN matrix with elements set to "Sign(Sin(3*I+5*J))"
Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugi2outsin(const ae_int_t m, const ae_int_t n, integer_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of elements in the array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
double xdebugr2sum(const real_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by -a[i,j]
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugr2neg(const real_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Transposes array.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugr2transpose(real_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate MxN matrix with elements set to "Sin(3*I+5*J)"
Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugr2outsin(const ae_int_t m, const ae_int_t n, real_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of elements in the array.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
alglib::complex xdebugc2sum(const complex_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Replace all values in array by -a[i,j]
Array is passed using "shared" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugc2neg(const complex_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Transposes array.
Array is passed using "var" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugc2transpose(complex_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Generate MxN matrix with elements set to "Sin(3*I+5*J),Cos(3*I+5*J)"
Array is passed using "out" convention.

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
void xdebugc2outsincos(const ae_int_t m, const ae_int_t n, complex_2d_array &a, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is debug function intended for testing ALGLIB interface generator.
Never use it in any real life project.

Returns sum of a[i,j]*(1+b[i,j]) such that c[i,j] is True

  -- ALGLIB --
     Copyright 11.10.2013 by Bochkanov Sergey
*************************************************************************/
double xdebugmaskedbiasedproductsum(const ae_int_t m, const ae_int_t n, const real_2d_array &a, const real_2d_array &b, const boolean_2d_array &c, const xparams _xparams = alglib::xdefault);
#endif
}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS COMPUTATIONAL CORE DECLARATIONS (FUNCTIONS)
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
#if defined(AE_COMPILE_NEARESTNEIGHBOR) || !defined(AE_PARTIAL_BUILD)
void kdtreebuild(/* Real    */ ae_matrix* xy,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t normtype,
     kdtree* kdt,
     ae_state *_state);
void kdtreebuildtagged(/* Real    */ ae_matrix* xy,
     /* Integer */ ae_vector* tags,
     ae_int_t n,
     ae_int_t nx,
     ae_int_t ny,
     ae_int_t normtype,
     kdtree* kdt,
     ae_state *_state);
void kdtreecreaterequestbuffer(kdtree* kdt,
     kdtreerequestbuffer* buf,
     ae_state *_state);
ae_int_t kdtreequeryknn(kdtree* kdt,
     /* Real    */ ae_vector* x,
     ae_int_t k,
     ae_bool selfmatch,
     ae_state *_state);
ae_int_t kdtreetsqueryknn(kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Real    */ ae_vector* x,
     ae_int_t k,
     ae_bool selfmatch,
     ae_state *_state);
ae_int_t kdtreequeryrnn(kdtree* kdt,
     /* Real    */ ae_vector* x,
     double r,
     ae_bool selfmatch,
     ae_state *_state);
ae_int_t kdtreetsqueryrnn(kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Real    */ ae_vector* x,
     double r,
     ae_bool selfmatch,
     ae_state *_state);
ae_int_t kdtreequeryaknn(kdtree* kdt,
     /* Real    */ ae_vector* x,
     ae_int_t k,
     ae_bool selfmatch,
     double eps,
     ae_state *_state);
ae_int_t kdtreetsqueryaknn(kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Real    */ ae_vector* x,
     ae_int_t k,
     ae_bool selfmatch,
     double eps,
     ae_state *_state);
ae_int_t kdtreequerybox(kdtree* kdt,
     /* Real    */ ae_vector* boxmin,
     /* Real    */ ae_vector* boxmax,
     ae_state *_state);
ae_int_t kdtreetsquerybox(kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Real    */ ae_vector* boxmin,
     /* Real    */ ae_vector* boxmax,
     ae_state *_state);
void kdtreequeryresultsx(kdtree* kdt,
     /* Real    */ ae_matrix* x,
     ae_state *_state);
void kdtreequeryresultsxy(kdtree* kdt,
     /* Real    */ ae_matrix* xy,
     ae_state *_state);
void kdtreequeryresultstags(kdtree* kdt,
     /* Integer */ ae_vector* tags,
     ae_state *_state);
void kdtreequeryresultsdistances(kdtree* kdt,
     /* Real    */ ae_vector* r,
     ae_state *_state);
void kdtreetsqueryresultsx(kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Real    */ ae_matrix* x,
     ae_state *_state);
void kdtreetsqueryresultsxy(kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Real    */ ae_matrix* xy,
     ae_state *_state);
void kdtreetsqueryresultstags(kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Integer */ ae_vector* tags,
     ae_state *_state);
void kdtreetsqueryresultsdistances(kdtree* kdt,
     kdtreerequestbuffer* buf,
     /* Real    */ ae_vector* r,
     ae_state *_state);
void kdtreequeryresultsxi(kdtree* kdt,
     /* Real    */ ae_matrix* x,
     ae_state *_state);
void kdtreequeryresultsxyi(kdtree* kdt,
     /* Real    */ ae_matrix* xy,
     ae_state *_state);
void kdtreequeryresultstagsi(kdtree* kdt,
     /* Integer */ ae_vector* tags,
     ae_state *_state);
void kdtreequeryresultsdistancesi(kdtree* kdt,
     /* Real    */ ae_vector* r,
     ae_state *_state);
void kdtreeexplorebox(kdtree* kdt,
     /* Real    */ ae_vector* boxmin,
     /* Real    */ ae_vector* boxmax,
     ae_state *_state);
void kdtreeexplorenodetype(kdtree* kdt,
     ae_int_t node,
     ae_int_t* nodetype,
     ae_state *_state);
void kdtreeexploreleaf(kdtree* kdt,
     ae_int_t node,
     /* Real    */ ae_matrix* xy,
     ae_int_t* k,
     ae_state *_state);
void kdtreeexploresplit(kdtree* kdt,
     ae_int_t node,
     ae_int_t* d,
     double* s,
     ae_int_t* nodele,
     ae_int_t* nodege,
     ae_state *_state);
void kdtreealloc(ae_serializer* s, kdtree* tree, ae_state *_state);
void kdtreeserialize(ae_serializer* s, kdtree* tree, ae_state *_state);
void kdtreeunserialize(ae_serializer* s, kdtree* tree, ae_state *_state);
void _kdtreerequestbuffer_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _kdtreerequestbuffer_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic);
void _kdtreerequestbuffer_clear(void* _p);
void _kdtreerequestbuffer_destroy(void* _p);
void _kdtree_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _kdtree_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic);
void _kdtree_clear(void* _p);
void _kdtree_destroy(void* _p);
#endif
#if defined(AE_COMPILE_HQRND) || !defined(AE_PARTIAL_BUILD)
void hqrndrandomize(hqrndstate* state, ae_state *_state);
void hqrndseed(ae_int_t s1,
     ae_int_t s2,
     hqrndstate* state,
     ae_state *_state);
double hqrnduniformr(hqrndstate* state, ae_state *_state);
ae_int_t hqrnduniformi(hqrndstate* state, ae_int_t n, ae_state *_state);
double hqrndnormal(hqrndstate* state, ae_state *_state);
void hqrndunit2(hqrndstate* state, double* x, double* y, ae_state *_state);
void hqrndnormal2(hqrndstate* state,
     double* x1,
     double* x2,
     ae_state *_state);
double hqrndexponential(hqrndstate* state,
     double lambdav,
     ae_state *_state);
double hqrnddiscrete(hqrndstate* state,
     /* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);
double hqrndcontinuous(hqrndstate* state,
     /* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);
void _hqrndstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _hqrndstate_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic);
void _hqrndstate_clear(void* _p);
void _hqrndstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_XDEBUG) || !defined(AE_PARTIAL_BUILD)
void xdebuginitrecord1(xdebugrecord1* rec1, ae_state *_state);
ae_int_t xdebugb1count(/* Boolean */ ae_vector* a, ae_state *_state);
void xdebugb1not(/* Boolean */ ae_vector* a, ae_state *_state);
void xdebugb1appendcopy(/* Boolean */ ae_vector* a, ae_state *_state);
void xdebugb1outeven(ae_int_t n,
     /* Boolean */ ae_vector* a,
     ae_state *_state);
ae_int_t xdebugi1sum(/* Integer */ ae_vector* a, ae_state *_state);
void xdebugi1neg(/* Integer */ ae_vector* a, ae_state *_state);
void xdebugi1appendcopy(/* Integer */ ae_vector* a, ae_state *_state);
void xdebugi1outeven(ae_int_t n,
     /* Integer */ ae_vector* a,
     ae_state *_state);
double xdebugr1sum(/* Real    */ ae_vector* a, ae_state *_state);
void xdebugr1neg(/* Real    */ ae_vector* a, ae_state *_state);
void xdebugr1appendcopy(/* Real    */ ae_vector* a, ae_state *_state);
void xdebugr1outeven(ae_int_t n,
     /* Real    */ ae_vector* a,
     ae_state *_state);
ae_complex xdebugc1sum(/* Complex */ ae_vector* a, ae_state *_state);
void xdebugc1neg(/* Complex */ ae_vector* a, ae_state *_state);
void xdebugc1appendcopy(/* Complex */ ae_vector* a, ae_state *_state);
void xdebugc1outeven(ae_int_t n,
     /* Complex */ ae_vector* a,
     ae_state *_state);
ae_int_t xdebugb2count(/* Boolean */ ae_matrix* a, ae_state *_state);
void xdebugb2not(/* Boolean */ ae_matrix* a, ae_state *_state);
void xdebugb2transpose(/* Boolean */ ae_matrix* a, ae_state *_state);
void xdebugb2outsin(ae_int_t m,
     ae_int_t n,
     /* Boolean */ ae_matrix* a,
     ae_state *_state);
ae_int_t xdebugi2sum(/* Integer */ ae_matrix* a, ae_state *_state);
void xdebugi2neg(/* Integer */ ae_matrix* a, ae_state *_state);
void xdebugi2transpose(/* Integer */ ae_matrix* a, ae_state *_state);
void xdebugi2outsin(ae_int_t m,
     ae_int_t n,
     /* Integer */ ae_matrix* a,
     ae_state *_state);
double xdebugr2sum(/* Real    */ ae_matrix* a, ae_state *_state);
void xdebugr2neg(/* Real    */ ae_matrix* a, ae_state *_state);
void xdebugr2transpose(/* Real    */ ae_matrix* a, ae_state *_state);
void xdebugr2outsin(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     ae_state *_state);
ae_complex xdebugc2sum(/* Complex */ ae_matrix* a, ae_state *_state);
void xdebugc2neg(/* Complex */ ae_matrix* a, ae_state *_state);
void xdebugc2transpose(/* Complex */ ae_matrix* a, ae_state *_state);
void xdebugc2outsincos(ae_int_t m,
     ae_int_t n,
     /* Complex */ ae_matrix* a,
     ae_state *_state);
double xdebugmaskedbiasedproductsum(ae_int_t m,
     ae_int_t n,
     /* Real    */ ae_matrix* a,
     /* Real    */ ae_matrix* b,
     /* Boolean */ ae_matrix* c,
     ae_state *_state);
void _xdebugrecord1_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _xdebugrecord1_init_copy(void* _dst, void* _src, ae_state *_state, ae_bool make_automatic);
void _xdebugrecord1_clear(void* _p);
void _xdebugrecord1_destroy(void* _p);
#endif

}
#endif

