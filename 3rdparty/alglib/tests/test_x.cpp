#include "stdafx.h"
#include <sstream>
#include <math.h>

#include "alglibmisc.h"
#include "alglibinternal.h"
#include "linalg.h"
#include "statistics.h"
#include "dataanalysis.h"
#include "specialfunctions.h"
#include "solvers.h"
#include "optimization.h"
#include "diffequations.h"
#include "fasttransforms.h"
#include "integration.h"
#include "interpolation.h"

#if AE_OS==AE_WINDOWS
#include <windows.h>
#elif AE_OS==AE_POSIX
#include <pthread.h>
#endif

using namespace alglib;

const char *fmt_str     = "%-29s %s\n";
const char *fmt_speedup = "%-25s %5.1fx\n";

//
// Flag variables
//
bool issue505_passed = true;
bool issue478_passed = true;
bool issue528_passed = true;
bool issue591_passed = true;
bool issue594_passed = true;
bool issue764_passed = true;
bool issue813_passed = true;
bool issue824_passed = true;

//
// Service datatypes
//
typedef struct
{
    alglib_impl::ae_complex cval;
    double rval;
    alglib_impl::ae_int_t ival;
    ae_bool bval;
    alglib_impl::ae_vector i1val;
} innerrec;


typedef struct
{
    ae_bool bval;
    innerrec recval;
    alglib_impl::ae_shared_pool pool;
} seedrec;

void _innerrec_init(void* _p, alglib_impl::ae_state *_state, ae_bool make_automatic)
{
    innerrec *p = (innerrec*)_p;
    alglib_impl::ae_touch_ptr((void*)p);
    alglib_impl::ae_vector_init(&p->i1val, 0, alglib_impl::DT_INT, _state, make_automatic);
}


void _innerrec_init_copy(void* _dst, const void* _src, alglib_impl::ae_state *_state, ae_bool make_automatic)
{
    innerrec *dst = (innerrec*)_dst;
    innerrec *src = (innerrec*)_src;
    dst->cval = src->cval;
    dst->rval = src->rval;
    dst->ival = src->ival;
    dst->bval = src->bval;
    alglib_impl::ae_vector_init_copy(&dst->i1val, &src->i1val, _state, make_automatic);
}


void _innerrec_clear(void* _p)
{
    innerrec *p = (innerrec*)_p;
    alglib_impl::ae_touch_ptr((void*)p);
    alglib_impl::ae_vector_clear(&p->i1val);
}


void _innerrec_destroy(void* _p)
{
    innerrec *p = (innerrec*)_p;
    alglib_impl::ae_touch_ptr((void*)p);
    alglib_impl::ae_vector_destroy(&p->i1val);
}


void _seedrec_init(void* _p, alglib_impl::ae_state *_state, ae_bool make_automatic)
{
    seedrec *p = (seedrec*)_p;
    alglib_impl::ae_touch_ptr((void*)p);
    _innerrec_init(&p->recval, _state, make_automatic);
    alglib_impl::ae_shared_pool_init(&p->pool, _state, make_automatic);
}


void _seedrec_init_copy(void* _dst, const void* _src, alglib_impl::ae_state *_state, ae_bool make_automatic)
{
    seedrec *dst = (seedrec*)_dst;
    seedrec *src = (seedrec*)_src;
    dst->bval = src->bval;
    _innerrec_init_copy(&dst->recval, &src->recval, _state, make_automatic);
    alglib_impl::ae_shared_pool_init_copy(&dst->pool, &src->pool, _state, make_automatic);
}


void _seedrec_clear(void* _p)
{
    seedrec *p = (seedrec*)_p;
    alglib_impl::ae_touch_ptr((void*)p);
    _innerrec_clear(&p->recval);
    alglib_impl::ae_shared_pool_clear(&p->pool);
}


void _seedrec_destroy(void* _p)
{
    seedrec *p = (seedrec*)_p;
    alglib_impl::ae_touch_ptr((void*)p);
    _innerrec_destroy(&p->recval);
    alglib_impl::ae_shared_pool_destroy(&p->pool);
}

void func505_grad(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr)
{
    double x0 = *((double*)ptr);
    
    //
    // This block assigns zero vector to gradient. Because gradient is a proxy vector
    // (vector which uses another object as storage), sizes of gradient and vector being
    // assigned must be equal. In this case data are copied in the memory linked with
    // proxy.
    //
    // Early versions of ALGLIB failed to handle such assignment (it discrupted link
    // between proxy vector and actual gradient stored in the internals of ALGLIB).
    //
    real_1d_array z = "[0]";
    grad = "[0]";
    grad = z;
    
    //
    // This block tries to perform operations which are forbidden for proxy vector:
    // * assign vector of non-matching size
    // * change length of the vector
    // Correct implementation must throw an exception without breaking a link between
    // proxy object and actual vector.
    //
    z = "[0,1]";
    try
    {
        grad = "[0,1]";
        issue505_passed = false;
    }
    catch(...) {}
    try
    {
        grad = z;
        issue505_passed = false;
    }
    catch(...) {}
    try
    {
        grad.setlength(10);
        issue505_passed = false;
    }
    catch(...) {}
    try
    {
        grad.setlength(1);
        issue505_passed = false;
    }
    catch(...) {}
    
    //
    // This block actually calculates function/gradient
    //
    func = pow(x[0]-x0,4);
    grad[0] = 4*pow(x[0]-x0,3);
}

void  func505_vec(const real_1d_array &x, real_1d_array &fi, void *ptr)
{
    double x0 = *((double*)ptr);
    fi[0] = x[0]-x0;
    fi[1] = pow(x[0]-x0,2);
}

void  func505_jac(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr)
{
    double x0 = *((double*)ptr);
    
    //
    // This block assigns zero matrix to Jacobian. Because Jacobian is a proxy matrix
    // (matrix which uses another object as storage), sizes of Jacobian and matrix being
    // assigned must be equal. In this case data are copied in the memory linked with
    // proxy.
    //
    // Early versions of ALGLIB failed to handle such assignment (it discrupted link
    // between proxy and actual matrix stored in the internals of ALGLIB).
    //
    real_2d_array z = "[[0],[0]]";
    jac = "[[0],[0]]";
    jac = z;
    
    //
    // This block tries to perform operations which are forbidden for proxy vector:
    // * assign vector of non-matching size
    // * change length of the vector
    // Correct implementation must throw an exception without breaking a link between
    // proxy object and actual vector.
    //
    try
    {
        jac = "[[0]]";
        issue505_passed = false;
    }
    catch(...) {}
    try
    {
        jac = "[[0,0],[1,1]]";
        issue505_passed = false;
    }
    catch(...) {}
    try
    {
        z = "[[0,1]]";
        jac = z;
        issue505_passed = false;
    }
    catch(...) {}
    try
    {
        jac.setlength(10,6);
        issue505_passed = false;
    }
    catch(...) {}
    try
    {
        jac.setlength(2,1);
        issue505_passed = false;
    }
    catch(...) {}
    
    //
    // This block actually calculates function/gradient
    //
    fi[0] = x[0]-x0;
    fi[1] = pow(x[0]-x0,2);
    jac[0][0] = 1.0;
    jac[1][0] = 2*(x[0]-x0);
}

void issue813_callback(const alglib::real_1d_array&, alglib::real_1d_array&, void*)
{
    throw 0;
}

void issue824_callback_i(const alglib::real_1d_array&, double&, void*)
{
    throw (int*)(NULL);
}

void issue824_callback_d(const alglib::real_1d_array&, double&, void*)
{
    throw (double*)(NULL);
}

void file_put_contents(const char *filename, const char *contents)
{
    FILE *f = fopen(filename, "wb");
    if( f==NULL )
        throw alglib::ap_error("file_put_contents: failed opening file");
    if( fwrite((void*)contents, 1, strlen(contents), f)!=strlen(contents) )
        throw alglib::ap_error("file_put_contents: failed writing to file");
    fclose(f);
}

#if AE_OS==AE_WINDOWS
struct async_rbf_record
{
    alglib::rbfmodel  *p_model;
    alglib::rbfreport *p_report;
    bool thread_finished;
};
DWORD WINAPI async_build_rbf_model(LPVOID T)
{
    async_rbf_record *p = (async_rbf_record*)T;
    alglib::rbfbuildmodel(*(p->p_model), *(p->p_report));
    p->thread_finished = true;
    return 0;
}
#elif AE_OS==AE_POSIX
struct async_rbf_record
{
    alglib::rbfmodel  *p_model;
    alglib::rbfreport *p_report;
    bool thread_finished;
};
void* async_build_rbf_model(void *T)
{
    async_rbf_record *p = (async_rbf_record*)T;
    alglib::rbfbuildmodel(*(p->p_model), *(p->p_report));
    p->thread_finished = true;
    return NULL;
}
#endif

int main()
{
    //
    // Report system properties
    //
    printf("System:\n");
#ifdef AE_HPC
    printf("* cores count                %3ld\n", (long)alglib_impl::ae_cores_count());
#else
    printf("* cores count                %3ld\n", (long)1);
#endif

    //
    // Check status of allocation counter
    //
#ifdef AE_USE_ALLOC_COUNTER
    printf("Allocation counter activated...\n");
    alglib_impl::_use_alloc_counter = ae_true;
    if( alglib_impl::_alloc_counter!=0 )
    {
        printf("FAILURE: alloc_counter is non-zero on start!\n");
        return 1;
    }
    {
        alglib::real_1d_array x;
        x.setlength(1);
        if( alglib_impl::_alloc_counter==0 )
            printf(":::: WARNING: ALLOC_COUNTER IS INACTIVE!!! ::::: \n");
    }
    if( alglib_impl::_alloc_counter!=0 )
    {
        printf("FAILURE: alloc_counter does not decrease!\n");
        return 1;
    }
#else
    printf("No alloc counter.\nSome tests are skipped.\n");
#endif
    
    //
    // Testing basic functionality
    //
    printf("Basic functions:\n");
    {
        //
        // Testing 1D array functionality
        //
        bool passed = true;
        try
        {
            //
            // 1D boolean
            //
            // Default constructor, string constructor, copy constructor, assignment constructors:
            // * test that array sizes as reported by length match to what was specified
            // * test item-by-item access
            // * test to_string()
            // * test that modification of the copied array does not change original
            // * test that setlength() changes length
            // * test setcontent/getcontent
            // * test getcontent(), operator() and operator[] on constant arrays
            //   (in this case distinct implementation is used which must be tested separately)
            //
            alglib::boolean_1d_array arr_0, arr_1("[]"), arr_2("[true,false,true]"), arr_3(arr_2), arr_4, arr_5;
            arr_4 = arr_2;
            arr_5 = "[true,true,false]";
            passed = passed && (arr_0.length()==0);
            passed = passed && (arr_1.length()==0);
            passed = passed && (arr_2.length()==3);
            passed = passed && (arr_3.length()==3);
            passed = passed && (arr_2[0]==arr_2(0)) && (arr_2[1]==arr_2(1)) && (arr_2[2]==arr_2(2));
            passed = passed &&  arr_2[0] && !arr_2[1] &&  arr_2[2];
            passed = passed &&  arr_3[0] && !arr_3[1] &&  arr_3[2];
            passed = passed &&  arr_4[0] && !arr_4[1] &&  arr_4[2];
            passed = passed &&  arr_5[0] &&  arr_5[1] && !arr_5[2];
            passed = passed && (arr_2.tostring()=="[true,false,true]");
            passed = passed && (arr_3.tostring()=="[true,false,true]");
            passed = passed && (arr_4.tostring()=="[true,false,true]");
            passed = passed && (arr_5.tostring()=="[true,true,false]");
            arr_2[0] = false;
            passed = passed && !arr_2[0] && arr_3[0] && arr_4[0];
            arr_5.setlength(99);
            passed = passed && (arr_5.length()==99);
            
            // setcontent/getcontent
            bool a0[] = {true, false, true, false, false};
            bool a0_mod  = false;
            bool a0_orig = true;
            bool *p6;
            alglib::boolean_1d_array arr_6;
            arr_6.setcontent(5, a0);
            passed = passed && (arr_6[0]==a0[0]) && (arr_6[1]==a0[1]) && (arr_6[2]==a0[2]) && (arr_6[3]==a0[3]) && (arr_6[4]==a0[4]);
            p6 = arr_6.getcontent();
            passed = passed && (p6!=a0);
            passed = passed && (p6[0]==a0[0]) && (p6[1]==a0[1]) && (p6[2]==a0[2]) && (p6[3]==a0[3]) && (p6[4]==a0[4]);
            a0[0] = a0_mod;
            passed = passed && (arr_6[0]!=a0[0]);
            a0[0] = a0_orig;
            
            // operations on constant arrays
            {
                const alglib::boolean_1d_array &ac = arr_6;
                passed = passed && (ac[0]==a0[0]) && (ac[1]==a0[1]) && (ac[2]==a0[2]) && (ac[3]==a0[3]) && (ac[4]==a0[4]);
                passed = passed && (ac(0)==a0[0]) && (ac(1)==a0[1]) && (ac(2)==a0[2]) && (ac(3)==a0[3]) && (ac(4)==a0[4]);
                const bool *p = ac.getcontent();
                passed = passed && (p[0]==a0[0]) && (p[1]==a0[1]) && (p[2]==a0[2]) && (p[3]==a0[3]) && (p[4]==a0[4]);
            }
            
            //
            // Operations with proxy arrays:
            // * changes in target are propagated to proxy and vice versa
            // * assignments where proxy is source create new independent copy
            // * assignments to proxy are checked (their size must match to that of the target)
            // * incorrect assignments or attempts to change length must generate exception
            // * attempts to call setlength() must fail even when new size match original size
            //   of the array
            //
            alglib::boolean_1d_array targt, acopy;
            targt = "[true,false,false,true]";
            alglib::boolean_1d_array proxy(targt.c_ptr());
            acopy = proxy;
            passed = passed &&  targt[0] && !targt[1] && !targt[2] &&  targt[3];
            passed = passed &&  proxy[0] && !proxy[1] && !proxy[2] &&  proxy[3];
            passed = passed &&  acopy[0] && !acopy[1] && !acopy[2] &&  acopy[3];
            
            targt[0] = false;
            passed = passed && !targt[0] && !proxy[0] && acopy[0];
            proxy[0] = true;
            passed = passed &&  targt[0] &&  proxy[0] && acopy[0];
            
            acopy = "[false,true,true,true]";
            proxy = acopy;
            passed = passed && !targt[0] &&  targt[1] &&  targt[2] &&  targt[3];
            passed = passed && !proxy[0] &&  proxy[1] &&  proxy[2] &&  proxy[3];
            proxy = "[true,false,true,true]";
            passed = passed &&  targt[0] && !targt[1] &&  targt[2] &&  targt[3];
            passed = passed &&  proxy[0] && !proxy[1] &&  proxy[2] &&  proxy[3];
            
            try
            {
                acopy = "[false,true,true]";
                proxy = acopy;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy = "[true,true,true]";
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy.setlength(100);
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy.setlength(proxy.length());
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
        }
        catch(...)
        { passed = false; }
        try
        {
            //
            // 1D integer
            //
            // Default constructor, string constructor, copy constructor, assignment constructors:
            // * test that array sizes as reported by length match to what was specified
            // * test item-by-item access
            // * test to_string()
            // * test that modification of the copied array does not change original
            // * test that setlength() changes length
            //
            const char *s1 = "[2,3,-1]";
            const char *s2 = "[5,4,3]";
            const char *s3 = "[6,7,3,-4]";
            const char *s4 = "[9,5,-12,-0]";
            const char *s5 = "[1,7,2,1]";
            const char *s6 = "[7,7,7]";
            int v10 =  2, v11 =  3, v12 = -1, v10_mod = 9;
            int v20 =  5, v21 =  4, v22 =  3;
            int v30 =  6, v31 =  7, v32 =  3, v33 = -4, v30_mod = -6;
            int v40 =  9, v41 =  5, v42 =-12, v43 = 0;
            int v50 =  1, v51 =  7, v52 =  2, v53 = 1;
            
            alglib::integer_1d_array arr_0, arr_1("[]"), arr_2(s1), arr_3(arr_2), arr_4, arr_5;
            arr_4 = arr_2;
            arr_5 = s2;
            passed = passed && (arr_0.length()==0);
            passed = passed && (arr_1.length()==0);
            passed = passed && (arr_2.length()==3);
            passed = passed && (arr_3.length()==3);
            passed = passed && (arr_2[0]==arr_2(0)) && (arr_2[1]==arr_2(1)) && (arr_2[2]==arr_2(2));
            passed = passed && (arr_2[0]==v10) && (arr_2[1]==v11) && (arr_2[2]==v12);
            passed = passed && (arr_3[0]==v10) && (arr_3[1]==v11) && (arr_3[2]==v12);
            passed = passed && (arr_4[0]==v10) && (arr_4[1]==v11) && (arr_4[2]==v12);
            passed = passed && (arr_5[0]==v20) && (arr_5[1]==v21) && (arr_5[2]==v22);
            passed = passed && (arr_2.tostring()==s1);
            passed = passed && (arr_3.tostring()==s1);
            passed = passed && (arr_4.tostring()==s1);
            passed = passed && (arr_5.tostring()==s2);
            arr_2[0] = v10_mod;
            passed = passed && (arr_2[0]==v10_mod) && (arr_3[0]==v10) && (arr_4[0]==v10);
            arr_5.setlength(99);
            passed = passed && (arr_5.length()==99);
            
            // setcontent/getcontent
            alglib::ae_int_t a0[] =   {2, 3, 1, 9, 2};
            alglib::ae_int_t a0_mod  = 7;
            alglib::ae_int_t a0_orig = 2;
            alglib::ae_int_t *p6;
            alglib::integer_1d_array arr_6;
            arr_6.setcontent(5, a0);
            passed = passed && (arr_6[0]==a0[0]) && (arr_6[1]==a0[1]) && (arr_6[2]==a0[2]) && (arr_6[3]==a0[3]) && (arr_6[4]==a0[4]);
            p6 = arr_6.getcontent();
            passed = passed && (p6!=a0);
            passed = passed && (p6[0]==a0[0]) && (p6[1]==a0[1]) && (p6[2]==a0[2]) && (p6[3]==a0[3]) && (p6[4]==a0[4]);
            a0[0] = a0_mod;
            passed = passed && (arr_6[0]!=a0[0]);
            a0[0] = a0_orig;
            
            // operations on constant arrays
            {
                const alglib::integer_1d_array &ac = arr_6;
                passed = passed && (ac[0]==a0[0]) && (ac[1]==a0[1]) && (ac[2]==a0[2]) && (ac[3]==a0[3]) && (ac[4]==a0[4]);
                passed = passed && (ac(0)==a0[0]) && (ac(1)==a0[1]) && (ac(2)==a0[2]) && (ac(3)==a0[3]) && (ac(4)==a0[4]);
                const alglib::ae_int_t *p = ac.getcontent();
                passed = passed && (p[0]==a0[0]) && (p[1]==a0[1]) && (p[2]==a0[2]) && (p[3]==a0[3]) && (p[4]==a0[4]);
            }
            
            //
            // Operations with proxy arrays:
            // * changes in target are propagated to proxy and vice versa
            // * assignments where proxy is source create new independent copy
            // * assignments to proxy are checked (their size must match to that of the target)
            // * incorrect assignments or attempts to change length must generate exception
            // * attempts to call setlength() must fail even when new size match original size
            //   of the array
            //
            alglib::integer_1d_array targt, acopy;
            targt = s3;
            alglib::integer_1d_array proxy(targt.c_ptr());
            acopy = proxy;
            passed = passed && (targt[0]==v30) && (targt[1]==v31) && (targt[2]==v32) && (targt[3]==v33);
            passed = passed && (proxy[0]==v30) && (proxy[1]==v31) && (proxy[2]==v32) && (proxy[3]==v33);
            passed = passed && (acopy[0]==v30) && (acopy[1]==v31) && (acopy[2]==v32) && (acopy[3]==v33);
            
            targt[0] = v30_mod;
            passed = passed && (targt[0]==v30_mod) && (proxy[0]==v30_mod) && (acopy[0]==v30);
            proxy[0] = v30;
            passed = passed && (targt[0]==v30)     && (proxy[0]==v30)     && (acopy[0]==v30);
            
            acopy = s4;
            proxy = acopy;
            passed = passed && (targt[0]==v40) && (targt[1]==v41) && (targt[2]==v42) && (targt[3]==v43);
            passed = passed && (proxy[0]==v40) && (proxy[1]==v41) && (proxy[2]==v42) && (proxy[3]==v43);
            proxy = s5;
            passed = passed && (targt[0]==v50) && (targt[1]==v51) && (targt[2]==v52) && (targt[3]==v53);
            passed = passed && (proxy[0]==v50) && (proxy[1]==v51) && (proxy[2]==v52) && (proxy[3]==v53);
            
            try
            {
                acopy = s6;
                proxy = acopy;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy = s6;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy.setlength(100);
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy.setlength(proxy.length());
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
        }
        catch(...)
        { passed = false; }
        try
        {
            //
            // 1D real
            //
            // Default constructor, string constructor, copy constructor, assignment constructors:
            // * test that array sizes as reported by length match to what was specified
            // * test item-by-item access
            // * test to_string()
            // * test that modification of the copied array does not change original
            // * test that setlength() changes length
            //
            const char *s1     = "[2,3.5,-2.5E-1]";
            const char *s1_fmt = "[2.00,3.50,-0.25]";
            const char *s2     = "[5,4,3.126]";
            const char *s2_fmt = "[5.00,4.00,3.13]";
            const char *s3 = "[6,7,3,-4E2]";
            const char *s4 = "[9,5,-12,-0.01]";
            const char *s5 = "[1,7,2,1]";
            const char *s6 = "[7,7,7]";
            const int dps = 2;
            const double v10 =  2, v11 =  3.5, v12 = -0.25, v10_mod = 9;
            const double v20 =  5, v21 =  4, v22 =  3.126;
            const double v30 =  6, v31 =  7, v32 =  3, v33 = -400, v30_mod = -6;
            const double v40 =  9, v41 =  5, v42 =-12, v43 = -0.01;
            const double v50 =  1, v51 =  7, v52 =  2, v53 = 1;
            
            alglib::real_1d_array arr_0, arr_1("[]"), arr_2(s1), arr_3(arr_2), arr_4, arr_5;
            arr_4 = arr_2;
            arr_5 = s2;
            passed = passed && (arr_0.length()==0);
            passed = passed && (arr_1.length()==0);
            passed = passed && (arr_2.length()==3);
            passed = passed && (arr_3.length()==3);
            passed = passed && (arr_2[0]==arr_2(0)) && (arr_2[1]==arr_2(1)) && (arr_2[2]==arr_2(2));
            passed = passed && (arr_2[0]==v10) && (arr_2[1]==v11) && (arr_2[2]==v12);
            passed = passed && (arr_3[0]==v10) && (arr_3[1]==v11) && (arr_3[2]==v12);
            passed = passed && (arr_4[0]==v10) && (arr_4[1]==v11) && (arr_4[2]==v12);
            passed = passed && (arr_5[0]==v20) && (arr_5[1]==v21) && (arr_5[2]==v22);
            passed = passed && (arr_2.tostring(dps)==s1_fmt);
            passed = passed && (arr_3.tostring(dps)==s1_fmt);
            passed = passed && (arr_4.tostring(dps)==s1_fmt);
            passed = passed && (arr_5.tostring(dps)==s2_fmt);
            arr_2[0] = v10_mod;
            passed = passed && (arr_2[0]==v10_mod) && (arr_3[0]==v10) && (arr_4[0]==v10);
            arr_5.setlength(99);
            passed = passed && (arr_5.length()==99);
            
            // setcontent/getcontent
            double a0[] =   {2, 3.5, 1, 9.125, 2};
            double a0_mod  = 7;
            double a0_orig = 2;
            double *p6;
            alglib::real_1d_array arr_6;
            arr_6.setcontent(5, a0);
            passed = passed && (arr_6[0]==a0[0]) && (arr_6[1]==a0[1]) && (arr_6[2]==a0[2]) && (arr_6[3]==a0[3]) && (arr_6[4]==a0[4]);
            p6 = arr_6.getcontent();
            passed = passed && (p6!=a0);
            passed = passed && (p6[0]==a0[0]) && (p6[1]==a0[1]) && (p6[2]==a0[2]) && (p6[3]==a0[3]) && (p6[4]==a0[4]);
            a0[0] = a0_mod;
            passed = passed && (arr_6[0]!=a0[0]);
            a0[0] = a0_orig;
            
            // operations on constant arrays
            {
                const alglib::real_1d_array &ac = arr_6;
                passed = passed && (ac[0]==a0[0]) && (ac[1]==a0[1]) && (ac[2]==a0[2]) && (ac[3]==a0[3]) && (ac[4]==a0[4]);
                passed = passed && (ac(0)==a0[0]) && (ac(1)==a0[1]) && (ac(2)==a0[2]) && (ac(3)==a0[3]) && (ac(4)==a0[4]);
                const double *p = ac.getcontent();
                passed = passed && (p[0]==a0[0]) && (p[1]==a0[1]) && (p[2]==a0[2]) && (p[3]==a0[3]) && (p[4]==a0[4]);
            }
            
            //
            // Operations with proxy arrays attached via attach_to(ae_vector*):
            // * changes in target are propagated to proxy and vice versa
            // * assignments where proxy is source create new independent copy
            // * assignments to proxy are checked (their size must match to that of the target)
            // * incorrect assignments or attempts to change length must generate exception
            // * attempts to call setlength() must fail even when new size match original size
            //   of the array
            //
            {
                alglib::real_1d_array targt, acopy;
                targt = s3;
                alglib::real_1d_array proxy(targt.c_ptr());
                acopy = proxy;
                passed = passed && (targt[0]==v30) && (targt[1]==v31) && (targt[2]==v32) && (targt[3]==v33);
                passed = passed && (proxy[0]==v30) && (proxy[1]==v31) && (proxy[2]==v32) && (proxy[3]==v33);
                passed = passed && (acopy[0]==v30) && (acopy[1]==v31) && (acopy[2]==v32) && (acopy[3]==v33);
                
                targt[0] = v30_mod;
                passed = passed && (targt[0]==v30_mod) && (proxy[0]==v30_mod) && (acopy[0]==v30);
                proxy[0] = v30;
                passed = passed && (targt[0]==v30)     && (proxy[0]==v30)     && (acopy[0]==v30);
                
                acopy = s4;
                proxy = acopy;
                passed = passed && (targt[0]==v40) && (targt[1]==v41) && (targt[2]==v42) && (targt[3]==v43);
                passed = passed && (proxy[0]==v40) && (proxy[1]==v41) && (proxy[2]==v42) && (proxy[3]==v43);
                proxy = s5;
                passed = passed && (targt[0]==v50) && (targt[1]==v51) && (targt[2]==v52) && (targt[3]==v53);
                passed = passed && (proxy[0]==v50) && (proxy[1]==v51) && (proxy[2]==v52) && (proxy[3]==v53);
                
                try
                {
                    acopy = s6;
                    proxy = acopy;
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                try
                {
                    proxy = s6;
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                try
                {
                    proxy.setlength(100);
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                try
                {
                    proxy.setlength(proxy.length());
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
            }
            
            //
            // >>> Unique for real_1d_array >>>
            //
            // Operations with proxy arrays attached via attach_to(double*):
            // * changes in target are propagated to proxy and vice versa
            // * assignments where proxy is source create new independent copy
            // * assignments to proxy are checked (their size must match to that of the target)
            // * incorrect assignments or attempts to change length must generate exception
            // * attempts to call setlength() must fail even when new size match original size
            //   of the array
            //
            {
                alglib::real_1d_array proxy, acopy;
                double targt[] = {v30, v31, v32, v33};
                proxy.attach_to_ptr(4, targt);
                acopy = proxy;
                passed = passed && (targt[0]==v30) && (targt[1]==v31) && (targt[2]==v32) && (targt[3]==v33);
                passed = passed && (proxy[0]==v30) && (proxy[1]==v31) && (proxy[2]==v32) && (proxy[3]==v33);
                passed = passed && (acopy[0]==v30) && (acopy[1]==v31) && (acopy[2]==v32) && (acopy[3]==v33);
                
                targt[0] = v30_mod;
                passed = passed && (targt[0]==v30_mod) && (proxy[0]==v30_mod) && (acopy[0]==v30);
                proxy[0] = v30;
                passed = passed && (targt[0]==v30)     && (proxy[0]==v30)     && (acopy[0]==v30);
                
                acopy = s4;
                proxy = acopy;
                passed = passed && (targt[0]==v40) && (targt[1]==v41) && (targt[2]==v42) && (targt[3]==v43);
                passed = passed && (proxy[0]==v40) && (proxy[1]==v41) && (proxy[2]==v42) && (proxy[3]==v43);
                proxy = s5;
                passed = passed && (targt[0]==v50) && (targt[1]==v51) && (targt[2]==v52) && (targt[3]==v53);
                passed = passed && (proxy[0]==v50) && (proxy[1]==v51) && (proxy[2]==v52) && (proxy[3]==v53);
                
                try
                {
                    acopy = s6;
                    proxy = acopy;
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                try
                {
                    proxy = s6;
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                try
                {
                    proxy.setlength(100);
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                try
                {
                    proxy.setlength(proxy.length());
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
            }
        }
        catch(...)
        { passed = false; }
        try
        {
            //
            // 1D complex
            //
            // Default constructor, string constructor, copy constructor, assignment constructors:
            // * test that array sizes as reported by length match to what was specified
            // * test item-by-item access
            // * test to_string()
            // * test that modification of the copied array does not change original
            // * test that setlength() changes length
            //
            const char *s1     = "[2,3.5i,1-2.5E-1i]";
            const char *s1_fmt = "[2.00,3.50i,1.00-0.25i]";
            const char *s2     = "[5,-4+1i,3.126]";
            const char *s2_fmt = "[5.00,-4.00+1.00i,3.13]";
            const char *s3 = "[6,7,3,-4E2]";
            const char *s4 = "[9,5,-12,-0.01]";
            const char *s5 = "[1,7,2,1]";
            const char *s6 = "[7,7,7]";
            const int dps = 2;
            alglib::complex v10 =  2, v11 = alglib::complex(0,3.5), v12 = alglib::complex(1,-0.25), v10_mod = 9;
            alglib::complex v20 =  5, v21 = alglib::complex(-4,1),  v22 =  3.126;
            alglib::complex v30 =  6, v31 =  7, v32 =  3, v33 = -400, v30_mod = -6;
            alglib::complex v40 =  9, v41 =  5, v42 =-12, v43 = -0.01;
            alglib::complex v50 =  1, v51 =  7, v52 =  2, v53 = 1;
            
            alglib::complex_1d_array arr_0, arr_1("[]"), arr_2(s1), arr_3(arr_2), arr_4, arr_5;
            arr_4 = arr_2;
            arr_5 = s2;
            passed = passed && (arr_0.length()==0);
            passed = passed && (arr_1.length()==0);
            passed = passed && (arr_2.length()==3);
            passed = passed && (arr_3.length()==3);
            passed = passed && (arr_2[0]==arr_2(0)) && (arr_2[1]==arr_2(1)) && (arr_2[2]==arr_2(2));
            passed = passed && (arr_2[0]==v10) && (arr_2[1]==v11) && (arr_2[2]==v12);
            passed = passed && (arr_3[0]==v10) && (arr_3[1]==v11) && (arr_3[2]==v12);
            passed = passed && (arr_4[0]==v10) && (arr_4[1]==v11) && (arr_4[2]==v12);
            passed = passed && (arr_5[0]==v20) && (arr_5[1]==v21) && (arr_5[2]==v22);
            passed = passed && (arr_2.tostring(dps)==s1_fmt);
            passed = passed && (arr_3.tostring(dps)==s1_fmt);
            passed = passed && (arr_4.tostring(dps)==s1_fmt);
            passed = passed && (arr_5.tostring(dps)==s2_fmt);
            arr_2[0] = v10_mod;
            passed = passed && (arr_2[0]==v10_mod) && (arr_3[0]==v10) && (arr_4[0]==v10);
            arr_5.setlength(99);
            passed = passed && (arr_5.length()==99);
            
            // setcontent/getcontent
            alglib::complex a0[] =   {2, 3.5, 1, 9.125, 2};
            alglib::complex a0_mod  = 7;
            alglib::complex a0_orig = 2;
            alglib::complex *p6;
            alglib::complex_1d_array arr_6;
            arr_6.setcontent(5, a0);
            passed = passed && (arr_6[0]==a0[0]) && (arr_6[1]==a0[1]) && (arr_6[2]==a0[2]) && (arr_6[3]==a0[3]) && (arr_6[4]==a0[4]);
            p6 = arr_6.getcontent();
            passed = passed && (p6!=a0);
            passed = passed && (p6[0]==a0[0]) && (p6[1]==a0[1]) && (p6[2]==a0[2]) && (p6[3]==a0[3]) && (p6[4]==a0[4]);
            a0[0] = a0_mod;
            passed = passed && (arr_6[0]!=a0[0]);
            a0[0] = a0_orig;
            
            // operations on constant arrays
            {
                const alglib::complex_1d_array &ac = arr_6;
                passed = passed && (ac[0]==a0[0]) && (ac[1]==a0[1]) && (ac[2]==a0[2]) && (ac[3]==a0[3]) && (ac[4]==a0[4]);
                passed = passed && (ac(0)==a0[0]) && (ac(1)==a0[1]) && (ac(2)==a0[2]) && (ac(3)==a0[3]) && (ac(4)==a0[4]);
                const alglib::complex *p = ac.getcontent();
                passed = passed && (p[0]==a0[0]) && (p[1]==a0[1]) && (p[2]==a0[2]) && (p[3]==a0[3]) && (p[4]==a0[4]);
            }
            
            //
            // Operations with proxy arrays:
            // * changes in target are propagated to proxy and vice versa
            // * assignments where proxy is source create new independent copy
            // * assignments to proxy are checked (their size must match to that of the target)
            // * incorrect assignments or attempts to change length must generate exception
            // * attempts to call setlength() must fail even when new size match original size
            //   of the array
            //
            alglib::complex_1d_array targt, acopy;
            targt = s3;
            alglib::complex_1d_array proxy(targt.c_ptr());
            acopy = proxy;
            passed = passed && (targt[0]==v30) && (targt[1]==v31) && (targt[2]==v32) && (targt[3]==v33);
            passed = passed && (proxy[0]==v30) && (proxy[1]==v31) && (proxy[2]==v32) && (proxy[3]==v33);
            passed = passed && (acopy[0]==v30) && (acopy[1]==v31) && (acopy[2]==v32) && (acopy[3]==v33);
            
            targt[0] = v30_mod;
            passed = passed && (targt[0]==v30_mod) && (proxy[0]==v30_mod) && (acopy[0]==v30);
            proxy[0] = v30;
            passed = passed && (targt[0]==v30)     && (proxy[0]==v30)     && (acopy[0]==v30);
            
            acopy = s4;
            proxy = acopy;
            passed = passed && (targt[0]==v40) && (targt[1]==v41) && (targt[2]==v42) && (targt[3]==v43);
            passed = passed && (proxy[0]==v40) && (proxy[1]==v41) && (proxy[2]==v42) && (proxy[3]==v43);
            proxy = s5;
            passed = passed && (targt[0]==v50) && (targt[1]==v51) && (targt[2]==v52) && (targt[3]==v53);
            passed = passed && (proxy[0]==v50) && (proxy[1]==v51) && (proxy[2]==v52) && (proxy[3]==v53);
            
            try
            {
                acopy = s6;
                proxy = acopy;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy = s6;
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy.setlength(100);
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
            
            try
            {
                proxy.setlength(proxy.length());
                passed = false;
            }
            catch(alglib::ap_error e)
            { }
            catch(...)
            { passed = false; }
        }
        catch(...)
        { passed = false; }
        
        //
        // Report
        //
        printf(fmt_str, "* 1D arrays", passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !passed )
            return 1;
    }
    
    {
        //
        // Testing 2D array functionality
        //
        bool passed = true;
        try
        {
            //
            // 2D real
            //
            // Default constructor, string constructor, copy constructor, assignment constructors:
            // * test that array sizes as reported by length match to what was specified
            // * test item-by-item access
            // * test to_string()
            // * test that modification of the copied array does not change original
            // * test that setlength() changes length
            //
            const char *s1     = "[[2,3.5,-2.5E-1],[1,2,3]]";
            const char *s1_fmt = "[[2.00,3.50,-0.25],[1.00,2.00,3.00]]";
            const char *s2     = "[[5],[4],[3.126]]";
            const char *s2_fmt = "[[5.00],[4.00],[3.13]]";
            const char *s3 = "[[6,7],[3,-4E2],[-3,-1]]";
            const char *s4 = "[[9,5],[-12,-0.01],[-1,-2]]";
            const char *s5 = "[[1,7],[2,1],[0,4]]";
            const char *s60 = "[[7,7],[7,7]]";
            const char *s61 = "[[7],[7],[7]]";
            const int dps = 2;
            const double v10 =  2, v11 =  3.5, v12 = -0.25, v13=1, v14 = 2, v15 = 3, v10_mod = 9;
            const double v20 =  5, v21 =  4, v22 =  3.126;
            const double v30 =  6, v31 =  7, v32 =  3, v33 = -400, v34=-3, v35=-1, v30_mod = -6;
            /*double v40 =  9, v41 =  5, v42 =-12, v43 = -0.01;
            double v50 =  1, v51 =  7, v52 =  2, v53 = 1;*/
            double r;
            
            alglib::real_2d_array arr_0, arr_1("[[]]"), arr_2(s1), arr_3(arr_2), arr_4, arr_5;
            arr_4 = arr_2;
            arr_5 = s2;
            passed = passed && (arr_0.rows()==0) && (arr_0.cols()==0) && (arr_0.getstride()==0);
            passed = passed && (arr_1.rows()==0) && (arr_1.cols()==0) && (arr_1.getstride()==0);
            passed = passed && (arr_2.rows()==2) && (arr_2.cols()==3) && (arr_2.getstride()>=arr_2.cols());
            passed = passed && (arr_3.rows()==2) && (arr_3.cols()==3) && (arr_3.getstride()>=arr_3.cols());
            passed = passed && (arr_4.rows()==2) && (arr_4.cols()==3) && (arr_4.getstride()>=arr_4.cols());
            passed = passed && (arr_5.rows()==3) && (arr_5.cols()==1) && (arr_5.getstride()>=arr_5.cols());
            passed = passed && (arr_2[0][0]==arr_2(0,0)) && (arr_2[0][1]==arr_2(0,1)) && (arr_2[0][2]==arr_2(0,2));
            passed = passed && (arr_2[1][0]==arr_2(1,0)) && (arr_2[1][1]==arr_2(1,1)) && (arr_2[1][2]==arr_2(1,2));
            passed = passed && (arr_2[0][0]==v10) && (arr_2[0][1]==v11) && (arr_2[0][2]==v12);
            passed = passed && (arr_2[1][0]==v13) && (arr_2[1][1]==v14) && (arr_2[1][2]==v15);
            passed = passed && (arr_3[0][0]==v10) && (arr_3[0][1]==v11) && (arr_3[0][2]==v12);
            passed = passed && (arr_3[1][0]==v13) && (arr_3[1][1]==v14) && (arr_3[1][2]==v15);
            passed = passed && (arr_4[0][0]==v10) && (arr_4[0][1]==v11) && (arr_4[0][2]==v12);
            passed = passed && (arr_4[1][0]==v13) && (arr_4[1][1]==v14) && (arr_4[1][2]==v15);
            passed = passed && (arr_5[0][0]==v20) && (arr_5[1][0]==v21) && (arr_5[2][0]==v22);
            passed = passed && (arr_2.tostring(dps)==s1_fmt);
            passed = passed && (arr_3.tostring(dps)==s1_fmt);
            passed = passed && (arr_4.tostring(dps)==s1_fmt);
            passed = passed && (arr_5.tostring(dps)==s2_fmt);
            arr_2[0][0] = v10_mod;
            passed = passed && (arr_2[0][0]==v10_mod) && (arr_3[0][0]==v10) && (arr_4[0][0]==v10);
            arr_5.setlength(99,97);
            passed = passed && (arr_5.rows()==99) && (arr_5.cols()==97);
            
            //
            // setcontent/elementwise access/constant arrays
            //
            ae_int_t n, m, i, j;
            for(n=1; n<=10; n++)
                for(m=1; m<=10; m++)
                {
                    alglib::real_2d_array arr_6;
                    double a0[100];
                    
                    // fill array by random values, test setcontent(0
                    for(i=0; i<m*n; i++)
                        a0[i] = alglib::randomreal();
                    arr_6.setcontent(m, n, a0);
                    for(i=0; i<m; i++)
                        for(j=0; j<n; j++)
                        {
                            passed = passed && (arr_6[i][j]==a0[i*n+j]);
                            passed = passed && (arr_6(i,j)==a0[i*n+j]);
                        }
                    
                    // test that setcontent() actually copies data instead of creating just reference
                    r = a0[0];
                    a0[0] = a0[0]+1;
                    passed = passed && (arr_6[0][0]!=a0[0]);
                    a0[0] = r;
            
                    // operations on constant arrays
                    {
                        const alglib::real_2d_array &ac = arr_6;
                        for(i=0; i<m; i++)
                            for(j=0; j<n; j++)
                            {
                                passed = passed && (ac[i][j]==a0[i*n+j]);
                                passed = passed && (ac(i,j)==a0[i*n+j]);
                            }
                    }
                }
                
            
            //
            // Operations with proxy arrays:
            // * changes in target are propagated to proxy and vice versa
            // * assignments where proxy is source create new independent copy
            // * assignments to proxy are checked (their size must match to that of the target)
            // * incorrect assignments or attempts to change length must generate exception
            // * attempts to call setlength() must fail even when new size match original size
            //   of the array
            //
            { // test attach_to(ae_matrix*)
                // subtest 0
                alglib::real_2d_array targt, acopy, acopy2;
                targt = s3;
                alglib::real_2d_array proxy(targt.c_ptr());
                acopy = proxy;
                for(i=0; i<targt.rows(); i++)
                    for(j=0; j<targt.cols(); j++)
                    {
                        passed = passed && (proxy[i][j]==targt[i][j]);
                        passed = passed && (acopy[i][j]==targt[i][j]);
                    }
                r = targt[0][0];
                targt[0][0] = r+1;
                passed = passed && (targt[0][0]!=r) && (proxy[0][0]!=r) && (acopy[0][0]==r);
                proxy[0][0] = r;
                passed = passed && (targt[0][0]==r) && (proxy[0][0]==r) && (acopy[0][0]==r);
                
                // subtest 1
                acopy = s4;
                proxy = acopy;
                for(i=0; i<acopy.rows(); i++)
                    for(j=0; j<acopy.cols(); j++)
                    {
                        passed = passed && (proxy[i][j]==acopy[i][j]);
                        passed = passed && (targt[i][j]==acopy[i][j]);
                    }
                r = targt[0][0];
                targt[0][0] = r+1;
                passed = passed && (targt[0][0]!=r) && (proxy[0][0]!=r) && (acopy[0][0]==r);
                proxy[0][0] = r;
                passed = passed && (targt[0][0]==r) && (proxy[0][0]==r) && (acopy[0][0]==r);
                
                // subtest 2
                acopy2 = s5;
                proxy = s5;
                for(i=0; i<acopy.rows(); i++)
                    for(j=0; j<acopy.cols(); j++)
                    {
                        passed = passed && (proxy[i][j]==acopy2[i][j]);
                        passed = passed && (targt[i][j]==acopy2[i][j]);
                    }
                
                // error handling test 0
                try
                {
                    acopy = s60;
                    proxy = acopy;
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                // error handling test 1
                try
                {
                    acopy = s61;
                    proxy = acopy;
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                // error handling test 2
                try
                {
                    proxy = s60;
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                // error handling test 3
                try
                {
                    proxy = s61;
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                // error handling test 4
                try
                {
                    proxy.setlength(100,99);
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                // error handling test 5
                try
                {
                    proxy.setlength(proxy.rows(),proxy.cols());
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
            }
            { // test attach_to(double*)
                // subtest 0
                alglib::real_2d_array proxy, acopy, acopy2;
                double targt[] = {v30, v31, v32, v33, v34, v35};
                const int NCOLS = 2;
                proxy.attach_to_ptr(3, 2, targt);
                acopy = proxy;
                for(i=0; i<proxy.rows(); i++)
                    for(j=0; j<proxy.cols(); j++)
                    {
                        passed = passed && (proxy[i][j]==targt[i*NCOLS+j]);
                        passed = passed && (acopy[i][j]==targt[i*NCOLS+j]);
                    }
                r = targt[0*NCOLS+0];
                targt[0*NCOLS+0] = r+1;
                passed = passed && (targt[0*NCOLS+0]!=r) && (proxy[0][0]!=r) && (acopy[0][0]==r);
                proxy[0][0] = r;
                passed = passed && (targt[0*NCOLS+0]==r) && (proxy[0][0]==r) && (acopy[0][0]==r);
                
                // subtest 1
                acopy = s4;
                proxy = acopy;
                for(i=0; i<acopy.rows(); i++)
                    for(j=0; j<acopy.cols(); j++)
                    {
                        passed = passed && (proxy[i][j]==acopy[i][j]);
                        passed = passed && (targt[i*NCOLS+j]==acopy[i][j]);
                    }
                r = targt[0*NCOLS+0];
                targt[0*NCOLS+0] = r+1;
                passed = passed && (targt[0*NCOLS+0]!=r) && (proxy[0][0]!=r) && (acopy[0][0]==r);
                proxy[0][0] = r;
                passed = passed && (targt[0*NCOLS+0]==r) && (proxy[0][0]==r) && (acopy[0][0]==r);
                
                // subtest 2
                acopy2 = s5;
                proxy = s5;
                for(i=0; i<acopy.rows(); i++)
                    for(j=0; j<acopy.cols(); j++)
                    {
                        passed = passed && (proxy[i][j]==acopy2[i][j]);
                        passed = passed && (targt[i*NCOLS+j]==acopy2[i][j]);
                    }
                
                // error handling test 0
                try
                {
                    acopy = s60;
                    proxy = acopy;
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                // error handling test 1
                try
                {
                    acopy = s61;
                    proxy = acopy;
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                // error handling test 2
                try
                {
                    proxy = s60;
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                // error handling test 3
                try
                {
                    proxy = s61;
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                // error handling test 4
                try
                {
                    proxy.setlength(100,99);
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
                
                // error handling test 5
                try
                {
                    proxy.setlength(proxy.rows(),proxy.cols());
                    passed = false;
                }
                catch(alglib::ap_error e)
                { }
                catch(...)
                { passed = false; }
            }
        }
        catch(...)
        { passed = false; }
        
        //
        // Report
        //
        printf(fmt_str, "* 2D arrays", passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !passed )
            return 1;
    }
    
    {
        //
        // Testing CSV functionality
        //
        const char *csv_name = "alglib-tst-35252-ndg4sf.csv";
        bool passed = true;
        try
        {
            // CSV_DEFAULT must be zero
            passed = passed && alglib::CSV_DEFAULT==0;
            
            // absent file - must fail
            try
            {
                alglib::real_2d_array arr;
                read_csv("nonexistent123foralgtestinglib", '\t', alglib::CSV_DEFAULT, arr);
                passed = false;
            }
            catch(alglib::ap_error)
            { }
            catch(...)
            { passed = false; }
            
            // non-rectangular file - must fail
            try
            {
                alglib::real_2d_array arr;
                file_put_contents(csv_name, "a,b,c\r\n1,2");
                read_csv(csv_name, ',', alglib::CSV_SKIP_HEADERS, arr);
                remove(csv_name);
                passed = false;
            }
            catch(alglib::ap_error)
            { }
            catch(...)
            { passed = false; }
            try
            {
                alglib::real_2d_array arr;
                file_put_contents(csv_name, "a,b,c\r\n1,2,3,4");
                read_csv(csv_name, ',', alglib::CSV_SKIP_HEADERS, arr);
                remove(csv_name);
                passed = false;
            }
            catch(alglib::ap_error)
            { }
            catch(...)
            { passed = false; }
            try
            {
                alglib::real_2d_array arr;
                file_put_contents(csv_name, "1,2,3,4\n1,2,3\n1,2,3");
                read_csv(csv_name, ',', alglib::CSV_DEFAULT, arr);
                remove(csv_name);
                passed = false;
            }
            catch(alglib::ap_error)
            { }
            catch(...)
            { passed = false; }
            
            // empty file
            try
            {
                alglib::real_2d_array arr;
                file_put_contents(csv_name, "");
                read_csv(csv_name, '\t', alglib::CSV_DEFAULT, arr);
                remove(csv_name);
                passed = passed && arr.rows()==0 && arr.cols()==0;
            }
            catch(...)
            { passed = false; }
            
            // one row with header, tab separator
            try
            {
                alglib::real_2d_array arr;
                file_put_contents(csv_name, "a\tb\tc\n");
                read_csv(csv_name, '\t', alglib::CSV_SKIP_HEADERS, arr);
                remove(csv_name);
                passed = passed && arr.rows()==0 && arr.cols()==0;
            }
            catch(...)
            { passed = false; }
            
            // no header, comma-separated, full stop as decimal point
            try
            {
                alglib::real_2d_array arr;
                file_put_contents(csv_name, "1.5,2,3.25\n4,5,6");
                read_csv(csv_name, ',', alglib::CSV_DEFAULT, arr);
                remove(csv_name);
                passed = passed && arr.tostring(2)=="[[1.50,2.00,3.25],[4.00,5.00,6.00]]";
            }
            catch(...)
            { passed = false; }
            
            // header, tab-separated, mixed use of comma and full stop as decimal points
            try
            {
                alglib::real_2d_array arr;
                file_put_contents(csv_name, "a\tb\tc\n1.5\t2\t3,25\n4\t5.25\t6,1\n");
                read_csv(csv_name, '\t', alglib::CSV_SKIP_HEADERS, arr);
                remove(csv_name);
                passed = passed && arr.tostring(2)=="[[1.50,2.00,3.25],[4.00,5.25,6.10]]";
            }
            catch(...)
            { passed = false; }
            
            // header, tab-separated, fixed/exponential, spaces, mixed use of comma and full stop as decimal points
            try
            {
                alglib::real_2d_array arr;
                file_put_contents(csv_name, " a\t b \tc\n1,1\t 2.9\t -3.5  \n  1.1E1  \t 2.0E-1 \t-3E+1 \n+1  \t -2\t 3.    \n.1\t-.2\t+.3\n");
                read_csv(csv_name, '\t', alglib::CSV_SKIP_HEADERS, arr);
                remove(csv_name);
                passed = passed && arr.tostring(2)=="[[1.10,2.90,-3.50],[11.00,0.20,-30.00],[1.00,-2.00,3.00],[0.10,-0.20,0.30]]";
            }
            catch(...)
            { passed = false; }
        }
        catch(...)
        { passed = false; }
        
        //
        // Report
        //
        printf(fmt_str, "* CSV support", passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !passed )
            return 1;
    }
        
    
    //
    // Serialization properties
    //
    {
        //
        // Test kd-tree serialization
        //
        bool passed = true;
        alglib::hqrndstate rs;
        alglib::kdtree tree0;
        alglib::real_2d_array xy, rxy0, rxy1;
        alglib::real_1d_array qx;
        const int npts = 50;
        const int nx = 2;
        const int ny = 1;
        int cnt0, cnt1;
        alglib::hqrndrandomize(rs);
        xy.setlength(npts, nx+ny);
        for(int i=0; i<npts; i++)
            for(int j=0; j<nx+ny; j++)
                xy[i][j] = alglib::hqrndnormal(rs);
        alglib::kdtreebuild(xy, npts, nx, ny, 2, tree0);
        qx.setlength(nx);
        
        try
        {
            // test string serialization/unserialization
            alglib::kdtree tree1;
            std::string s;
            alglib::kdtreeserialize(tree0, s);
            alglib::kdtreeunserialize(s, tree1);
            for(int i=0; i<100; i++)
            {
                for(int j=0; j<nx; j++)
                    qx[j] = alglib::hqrndnormal(rs);
                cnt0 = alglib::kdtreequeryknn(tree0, qx, 1, true);
                cnt1 = alglib::kdtreequeryknn(tree1, qx, 1, true);
                if( (cnt0!=1) || (cnt1!=1) )
                {
                    passed = false;
                    break;
                }
                alglib::kdtreequeryresultsxy(tree0, rxy0);
                alglib::kdtreequeryresultsxy(tree1, rxy1);
                for(int j=0; j<nx+ny; j++)
                    passed = passed && (rxy0[0][j]==rxy1[0][j]);
            }
        }
        catch(...)
        { passed = false; }
        
        try
        {
            // test stream serialization/unserialization
            //
            // NOTE: we add a few symbols at the beginning and after the end of the data
            //       in order to test algorithm ability to work in the middle of the stream
            alglib::kdtree tree1;
            std::stringstream s;
            s.put('b');
            s.put('e');
            s.put('g');
            alglib::kdtreeserialize(tree0, s);
            s.put('e');
            s.put('n');
            s.put('d');
            s.seekg(0);
            passed = passed && (s.get()=='b');
            passed = passed && (s.get()=='e');
            passed = passed && (s.get()=='g');
            alglib::kdtreeunserialize(s, tree1);
            passed = passed && (s.get()=='e');
            passed = passed && (s.get()=='n');
            passed = passed && (s.get()=='d');
            for(int i=0; i<100; i++)
            {
                for(int j=0; j<nx; j++)
                    qx[j] = alglib::hqrndnormal(rs);
                cnt0 = alglib::kdtreequeryknn(tree0, qx, 1, true);
                cnt1 = alglib::kdtreequeryknn(tree1, qx, 1, true);
                if( (cnt0!=1) || (cnt1!=1) )
                {
                    passed = false;
                    break;
                }
                alglib::kdtreequeryresultsxy(tree0, rxy0);
                alglib::kdtreequeryresultsxy(tree1, rxy1);
                for(int j=0; j<nx+ny; j++)
                    passed = passed && (rxy0[0][j]==rxy1[0][j]);
            }
        }
        catch(...)
        { passed = false; }
        
        try
        {
            // test string-to-stream serialization/unserialization
            alglib::kdtree tree1;
            std::string s0;
            alglib::kdtreeserialize(tree0, s0);
            std::stringstream s1(s0);
            alglib::kdtreeunserialize(s1, tree1);
            for(int i=0; i<100; i++)
            {
                for(int j=0; j<nx; j++)
                    qx[j] = alglib::hqrndnormal(rs);
                cnt0 = alglib::kdtreequeryknn(tree0, qx, 1, true);
                cnt1 = alglib::kdtreequeryknn(tree1, qx, 1, true);
                if( (cnt0!=1) || (cnt1!=1) )
                {
                    passed = false;
                    break;
                }
                alglib::kdtreequeryresultsxy(tree0, rxy0);
                alglib::kdtreequeryresultsxy(tree1, rxy1);
                for(int j=0; j<nx+ny; j++)
                    passed = passed && (rxy0[0][j]==rxy1[0][j]);
            }
        }
        catch(...)
        { passed = false; }
        
        try
        {
            // test stream-to-string serialization/unserialization
            alglib::kdtree tree1;
            std::stringstream s0;
            alglib::kdtreeserialize(tree0, s0);
            std::string s1 = s0.str();
            alglib::kdtreeunserialize(s1, tree1);
            for(int i=0; i<100; i++)
            {
                for(int j=0; j<nx; j++)
                    qx[j] = alglib::hqrndnormal(rs);
                cnt0 = alglib::kdtreequeryknn(tree0, qx, 1, true);
                cnt1 = alglib::kdtreequeryknn(tree1, qx, 1, true);
                if( (cnt0!=1) || (cnt1!=1) )
                {
                    passed = false;
                    break;
                }
                alglib::kdtreequeryresultsxy(tree0, rxy0);
                alglib::kdtreequeryresultsxy(tree1, rxy1);
                for(int j=0; j<nx+ny; j++)
                    passed = passed && (rxy0[0][j]==rxy1[0][j]);
            }
        }
        catch(...)
        { passed = false; }
            
        //
        // Report
        //
        printf(fmt_str, "* Serialization (kd-tree)", passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !passed )
            return 1;
    }
    {
        //
        // Test legacy RBF interface
        //
        const char *pc_str = "50000000000 00000000000 20000000000 10000000000 A0000000000 \
30000000000 20000000000 00000000000 A0000000000 30000000000\r\
00000000000 20000000000 A0000000000 60000000000 00000000000\n\
00000000000 00000000000 00000000000 00000000000 00000000000\r\n\
00000000m_3 00000000000 00000000000 00000000m_3 00000000000\n\r\
00000000000 00000000004 00000000000 00000000000\t00000000004 \
00000000000 00000000000 00000000804 00000000000 00000000000 \
00000000804 00000000000 00000000000 00000000G04 00000000000 \
00000000000 00000000G04 00000000000 00000000000 00000000O04 \
00000000000 00000000000 00000000O04 00000000000 00000000000 \
00000000S04 00000000000 00000000000 00000000S04 00000000000 \
00000000000 00000000W04 00000000000 00000000000 00000000W04 \
00000000000 00000000000 00000000Y04 00000000000 00000000000 \
00000000Y04 00000000000 00000000000 00000000K04 00000000000 \
00000000000 00000000K04 00000000000 00000000000 A0000000000 \
00000000000 10000000000 20000000000 30000000000 40000000000 \
60000000000 70000000000 80000000000 90000000000 50000000000 \
30000000000 00000000000 00000000000 00000000000 30000000000 \
00000000Y04 00000000000 00000000000 u1000000000 00000000000 \
00000000000 00000000000 60000000000 80000000000 00000000000 \
50000000000 00000000000 50000000000 50000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 K0000000000 \
00000000I04 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
00000000000 00000000000 00000000000 00000000000 00000000000 \
A0000000000 30000000000 00000000000 00000000000 00000000000 \
00000000m_3 00000000000 00000000000 00000000004 00000000000 \
00000000000 00000000804 00000000000 00000000000 00000000G04 \
00000000000 00000000000 00000000K04 00000000000 00000000000 \
00000000O04 00000000000 00000000000 00000000S04 00000000000 \
00000000000 00000000W04 00000000000 00000000000 00000000Y04 \
00000000000 00000000000 A0000000000 40000000000 00000000q04 \
-pAGQnQBI14 UqUWierJ91C esm8ag6G61C 00000000q04 4wcFMyCtu04 \
oPDvwHqst04 CExQXp8Ct04 00000000q04 litzPFhRb0C oKJvjcct314 \
5-fT-X8w614 00000000q04 3HSOsPVH11C vZWf4dgfv04 GbZg4MTJn04 \
00000000q04 iv7rMhuR71C hRtixp15r_3 EvCEDtLu-0C 00000000q04 \
41CXzA_q71C umRYLK2yp0C 1zzY3Zqd91C 00000000q04 JvxJzDeI21C \
TVbyd7Ygz0C JLywRdR1n0C 00000000q04 KmFarhc4g0C 1ehrn2tUt0C \
AECfwTIX814 00000000q04 Big__6hwt04 nSPzmAQrh_B 2H3o-KftH14 \
00000000q04 n1b9361vI14 mhJhviUE114 54a_qyBrH1C 00000000q04 \
10000000000 40000000000 StLCgor39-3 00000000000 00000000000 \
6qTG7Ae-1_3\n";
        alglib::real_1d_array ref_val("[-0.042560546916643, 0.942523544654062, 0.875197036560778, 0.0656948997826632, -0.743065973803404, -0.8903682039297, -0.26994815318748, 0.602248517290195, 0.980011992233124, 0.436594293214176]");
        bool passed = true;
        
        try
        {
            // test unserialization from string without trailing end-of-stream symbol (dot)
            // this test is necessary for backward compatibility
            double eps = 0.0000000001;
            alglib::rbfmodel model;
            alglib::rbfunserialize(std::string(pc_str), model);
            for(int i=0; i<ref_val.length(); i++)
                passed = passed && (fabs(alglib::rbfcalc2(model,i,0)-ref_val[i])<eps);
        }
        catch(...)
        { passed = false; }
        
        try
        {
            // test unserialization from string with trailing end-of-stream symbol (dot)
            // this test is necessary for forward compatibility
            double eps = 0.0000000001;
            alglib::rbfmodel model;
            alglib::rbfunserialize(std::string(pc_str)+".", model);
            for(int i=0; i<ref_val.length(); i++)
                passed = passed && (fabs(alglib::rbfcalc2(model,i,0)-ref_val[i])<eps);
        }
        catch(...)
        { passed = false; }
        
        try
        {
            // test unserialization from stream WITHOUT trailing end-of-stream symbol (dot)
            // this test MUST fail
            double eps = 0.0000000001;
            std::string _s(pc_str);
            std::istringstream stream(_s);
            alglib::rbfmodel model;
            alglib::rbfunserialize(stream, model);
            passed = false;
        }
        catch(...)
        { /* do nothing, it is expected to fail */ }
        
        try
        {
            // test unserialization from stream WITH trailing end-of-stream symbol (dot)
            // this test must succeed
            double eps = 0.0000000001;
            std::string _s = std::string(pc_str)+".";
            std::istringstream stream(_s);
            alglib::rbfmodel model;
            alglib::rbfunserialize(stream, model);
            for(int i=0; i<ref_val.length(); i++)
                passed = passed && (fabs(alglib::rbfcalc2(model,i,0)-ref_val[i])<eps);
        }
        catch(...)
        { passed = false; }
        
        try
        {
            // test that we can read from the stream after unserialization
            double eps = 0.0000000001;
            std::string _s = std::string(pc_str)+".<az>";
            std::istringstream stream(_s);
            alglib::rbfmodel model;
            alglib::rbfunserialize(stream, model);
            for(int i=0; i<ref_val.length(); i++)
                passed = passed && (fabs(alglib::rbfcalc2(model,i,0)-ref_val[i])<eps);
            passed = passed && (stream.get()=='<');
            passed = passed && (stream.get()=='a');
            passed = passed && (stream.get()=='z');
            passed = passed && (stream.get()=='>');
        }
        catch(...)
        { passed = false; }
            
        //
        // Report
        //
        printf(fmt_str, "* Serialization (RBF)", passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !passed )
            return 1;
    }
    {
        bool passed = true;
#if (AE_OS==AE_WINDOWS) || AE_OS==AE_POSIX
        alglib::hqrndstate rs;
        alglib::rbfmodel  rbf;
        alglib::rbfreport rep;
        alglib::real_2d_array xy;
        int n = 1000, nx = 2, ny = 1;
        double rbase = 1.0;
        async_rbf_record async_rec;
        alglib::hqrndseed(464, 764, rs);
        xy.setlength(n, nx+ny);
        for(int i=0; i<n; i++)
            for(int j=0; j<=nx+ny; j++)
                xy[i][j] = alglib::hqrndnormal(rs);
        alglib::rbfcreate(nx, ny, rbf);
        alglib::rbfsetalgohierarchical(rbf, rbase, 1, 0.0);
        alglib::rbfsetpoints(rbf, xy);
        alglib::rbfsetv2its(rbf, 100000);
        passed = passed && (alglib::rbfpeekprogress(rbf)==0);
        async_rec.p_model = &rbf;
        async_rec.p_report= &rep;
        async_rec.thread_finished = false;
#if AE_OS==AE_WINDOWS
        if( CreateThread(NULL, 0, async_build_rbf_model, &async_rec, 0, NULL)==NULL )
        {
            printf(fmt_str, "* Progress/termination (RBF)", "FAILED");
            printf(">>> unable to create background thread\n");
            fflush(stdout);
            return 1;
        }
#elif AE_OS==AE_POSIX
        pthread_t  thread;
        if( pthread_create(&thread, NULL, async_build_rbf_model, &async_rec)!=0 )
        {
            printf(fmt_str, "* Progress/termination (RBF)", "FAILED");
            printf(">>> unable to create background thread\n");
            fflush(stdout);
            return 1;
        }
#else
#error Unable to determine OS, unexpected here
#endif
        double last_progress = 0;
        for(;;)
        {
            double new_progress = alglib::rbfpeekprogress(rbf);
            passed = passed && (new_progress>=last_progress);
            passed = passed && (new_progress<=0.1); // we expect to terminate well before reaching 10%
            last_progress = new_progress;
            if( new_progress>=0.001 )
            {
                alglib::rbfrequesttermination(rbf);
                break;
            }
        }
        for(;;)
        {
            double new_progress = alglib::rbfpeekprogress(rbf);
            passed = passed && ((new_progress<=0.1) || (new_progress==1.0)); // we expect to terminate well before reaching 10%
            if( async_rec.thread_finished )
                break;
        }
        passed = passed && (alglib::rbfpeekprogress(rbf)==1);
        passed = passed && (rep.terminationtype==8);
        passed = passed && (alglib::rbfcalc2(rbf,alglib::hqrndnormal(rs),alglib::hqrndnormal(rs))==0.0);
        printf(fmt_str, "* Progress/termination (RBF)", passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !passed )
            return 1;
#else
        printf(fmt_str, "* Progress/termination (RBF)", "??");
        fflush(stdout);
        if( !passed )
            return 1;
#endif
    }
    {
        //
        // Test malloc() exceptions in constructors
        //
#ifdef AE_USE_ALLOC_COUNTER
        bool passed = true;
        bool were_exceptions = false;
        for(int eidx=0; ; eidx++) // loop is terminated when we survive through all the tests
        {
            //
            // Select moment when we generate exception in the constructor
            //
            alglib_impl::_malloc_failure_after = alglib_impl::_alloc_counter_total+eidx;
            
            //
            // Perform many activities with ALGLIB, catch exceptions.
            // It is survival test, it checks that we survive exceptions.
            // If it fails, we are likely to end with abort().
            //
            try
            {
                {
                    real_1d_array x0 = "[1,2,3]";
                    real_1d_array x1(x0);
                    real_1d_array x2;
                    x2 = x1;
                    real_1d_array *p = new real_1d_array("[1]");
                    delete p;
                }
                {
                    real_2d_array x0 = "[[1,2,3],[0,0,0]]";
                    real_2d_array x1(x0);
                    real_2d_array x2;
                    x2 = x1;
                    real_2d_array *p = new real_2d_array("[[1],[5]]");
                    delete p;
                }
                {
                    sparsematrix s;
                    sparsecreate(2, 2, s);
                    sparseset(s, 0, 0, 2.0);
                    sparseset(s, 1, 1, 1.0);
                    sparseset(s, 0, 1, 1.0);
                    sparseadd(s, 1, 1, 4.0);
                    sparsematrix s2(s), s3;
                    s3 = s;
                    double v;
                    v = sparseget(s, 0, 0);
                    v = sparseget(s, 0, 1);
                    v = sparseget(s, 1, 0);
                    v = sparseget(s, 1, 1);
                    sparseconverttocrs(s);
                    real_1d_array x = "[1,-1]";
                    real_1d_array y = "[]";
                    sparsemv(s, x, y);
                }
                {
                    real_1d_array x = "[0,0]";
                    double epsg = 0.0000000001;
                    double epsf = 0;
                    double epsx = 0;
                    double stpmax = 0.1;
                    ae_int_t maxits = 0;
                    mincgstate state;
                    mincgreport rep;
                    mincgcreate(x, state);
                    mincgsetcond(state, epsg, epsf, epsx, maxits);
                    mincgsetstpmax(state, stpmax);
                    mincgstate state2;
                    state2 = state;
                    mincgstate state3(state2);
                    mincgrestartfrom(state, x);
                }
                {
                    mlptrainer trn;
                    multilayerperceptron network;
                    mlpreport rep;
                    real_2d_array xy = "[[1,1,1],[1,2,2],[2,1,2],[2,2,4]]";
                    mlpcreatetrainer(2, 1, trn);
                    mlpcreate1(2, 5, 1, network);
                    mlpsetdataset(trn, xy, 4);
                    mlptrainnetwork(trn, network, 5, rep);
                    real_1d_array x = "[2,2]";
                    real_1d_array y = "[0]";
                    mlpprocess(network, x, y);
                }
                {
                    std::string s;
                    double v;
                    rbfmodel model0;
                    rbfmodel model1;
                    real_2d_array xy = "[[-1,0,2],[+1,0,3]]";
                    rbfreport rep;
                    rbfcreate(2, 1, model0);
                    rbfsetpoints(model0, xy);
                    rbfsetalgohierarchical(model0, 1.0, 3, 0.0);
                    rbfbuildmodel(model0, rep);
                    alglib::rbfserialize(model0, s);
                    alglib::rbfunserialize(s, model1);
                    v = rbfcalc2(model0, 0.0, 0.0);
                    v = rbfcalc2(model1, 0.0, 0.0);
                    rbfbuildmodel(model0, rep);
                    v = rbfcalc2(model0, 0.0, 0.0);
                    rbfbuildmodel(model1, rep);
                    v = rbfcalc2(model1, 0.0, 0.0);
                }
                
                //
                // We survived all tests, next iteration will bring no changed, terminate loop!
                //
                break;
            }
            catch(ap_error)
            { were_exceptions = true; }
        }
        alglib_impl::_malloc_failure_after = 0; // turn off artificial malloc failures
        printf(fmt_str, "* Exceptions in constructors", were_exceptions ? (passed ? "OK" : "FAILED") : "..");
        fflush(stdout);
        if( !passed )
            return 1;
#else
        printf(fmt_str, "* Exceptions in constructors", "??");
        fflush(stdout);
#endif
    }
    {
        //
        // Test multithreading-related settings
        //
        // For this test we measure performance of large NxNxN GEMMs
        // with different threading settings.
        //
        printf("SMP settings vs GEMM speedup:\n");
        if( alglib::_ae_cores_count()>1 )
        {
            bool passed = true;
            alglib_impl::ae_uint64_t default_global_threading = alglib::_ae_get_global_threading();
            alglib::ae_int_t default_nworkers = alglib::getnworkers();
            double time_default          = 0,
                   time_glob_ser         = 0,
                   time_glob_smp         = 0,
                   time_glob_ser_loc_ser = 0,
                   time_glob_ser_loc_smp = 0,
                   time_glob_smp_loc_ser = 0,
                   time_glob_smp_loc_smp = 0,
                   time_glob_smp_nw1     = 0;
            alglib::ae_int_t n = 800, mintime = 2000, cnt, t0;
            try
            {
                // allocate temporary matrices
                alglib::real_2d_array a, b, c;
                int i, j;
                a.setlength(n, n);
                b.setlength(n, n);
                c.setlength(n, n);
                for(i=0; i<n; i++)
                    for(j=0; j<n; j++)
                    {
                        a[i][j] = alglib::randomreal()-0.5;
                        b[i][j] = alglib::randomreal()-0.5;
                        c[i][j] = 0.0;
                    }
                
                // measure time; interleave measurements with different settings in order to
                // reduce variance of results
                while(time_default<mintime)
                {
                    // default threading
                    t0 = alglib_impl::ae_tickcount();
                    alglib::rmatrixgemm(
                        n, n, n,
                        1.0,
                        a, 0, 0, 0,
                        b, 0, 0, 0,
                        0.0,
                        c, 0, 0);
                    time_default += alglib_impl::ae_tickcount()-t0;
                    alglib::_ae_set_global_threading(default_global_threading); // restore
                    
                    // global serial
                    t0 = alglib_impl::ae_tickcount();
                    alglib::setglobalthreading(alglib::serial);
                    alglib::rmatrixgemm(
                        n, n, n,
                        1.0,
                        a, 0, 0, 0,
                        b, 0, 0, 0,
                        0.0,
                        c, 0, 0);
                    time_glob_ser += alglib_impl::ae_tickcount()-t0;
                    alglib::_ae_set_global_threading(default_global_threading); // restore
                    
                    // global parallel
                    t0 = alglib_impl::ae_tickcount();
                    alglib::setglobalthreading(alglib::parallel);
                    alglib::rmatrixgemm(
                        n, n, n,
                        1.0,
                        a, 0, 0, 0,
                        b, 0, 0, 0,
                        0.0,
                        c, 0, 0);
                    time_glob_smp += alglib_impl::ae_tickcount()-t0;
                    alglib::_ae_set_global_threading(default_global_threading); // restore
                    
                    // global serial, local serial
                    t0 = alglib_impl::ae_tickcount();
                    alglib::setglobalthreading(alglib::serial);
                    alglib::rmatrixgemm(
                        n, n, n,
                        1.0,
                        a, 0, 0, 0,
                        b, 0, 0, 0,
                        0.0,
                        c, 0, 0,
                        alglib::serial);
                    time_glob_ser_loc_ser += alglib_impl::ae_tickcount()-t0;
                    alglib::_ae_set_global_threading(default_global_threading); // restore
                    
                    // global serial, local parallel
                    t0 = alglib_impl::ae_tickcount();
                    alglib::setglobalthreading(alglib::serial);
                    alglib::rmatrixgemm(
                        n, n, n,
                        1.0,
                        a, 0, 0, 0,
                        b, 0, 0, 0,
                        0.0,
                        c, 0, 0,
                        alglib::parallel);
                    time_glob_ser_loc_smp += alglib_impl::ae_tickcount()-t0;
                    alglib::_ae_set_global_threading(default_global_threading); // restore
                    
                    // global parallel, local serial
                    t0 = alglib_impl::ae_tickcount();
                    alglib::setglobalthreading(alglib::parallel);
                    alglib::rmatrixgemm(
                        n, n, n,
                        1.0,
                        a, 0, 0, 0,
                        b, 0, 0, 0,
                        0.0,
                        c, 0, 0,
                        alglib::serial);
                    time_glob_smp_loc_ser += alglib_impl::ae_tickcount()-t0;
                    alglib::_ae_set_global_threading(default_global_threading); // restore
                    
                    // global parallel, local parallel
                    t0 = alglib_impl::ae_tickcount();
                    alglib::setglobalthreading(alglib::parallel);
                    alglib::rmatrixgemm(
                        n, n, n,
                        1.0,
                        a, 0, 0, 0,
                        b, 0, 0, 0,
                        0.0,
                        c, 0, 0,
                        alglib::parallel);
                    time_glob_smp_loc_smp += alglib_impl::ae_tickcount()-t0;
                    alglib::_ae_set_global_threading(default_global_threading); // restore
                    
                    // global parallel, nworkers=1
                    t0 = alglib_impl::ae_tickcount();
                    alglib::setglobalthreading(alglib::parallel);
                    alglib::setnworkers(1);
                    alglib::rmatrixgemm(
                        n, n, n,
                        1.0,
                        a, 0, 0, 0,
                        b, 0, 0, 0,
                        0.0,
                        c, 0, 0);
                    time_glob_smp_nw1 += alglib_impl::ae_tickcount()-t0;
                    alglib::_ae_set_global_threading(default_global_threading); // restore
                    alglib::setnworkers(default_nworkers);
                }
            }
            catch(ap_error)
            { passed = false; }
            printf(fmt_speedup, "* default speedup",     time_glob_ser/time_glob_ser);
            printf(fmt_speedup, "* serial (global)",     time_glob_ser/time_default);
            printf(fmt_speedup, "* serial (local)",      time_glob_ser/time_glob_ser_loc_ser);
            printf(fmt_speedup, "* serial (nworkers=1)", time_glob_ser/time_glob_smp_nw1);
            printf(fmt_speedup, "* parallel (global)",   time_glob_ser/time_glob_smp);
            printf(fmt_speedup, "* parallel (local) v1", time_glob_ser/time_glob_ser_loc_smp);
            passed = passed && (time_glob_ser/time_default         >0.85) && (time_glob_ser/time_default         <1.15);
            passed = passed && (time_glob_ser/time_glob_ser        >0.85) && (time_glob_ser/time_glob_ser        <1.15);
            passed = passed && (time_glob_ser/time_glob_ser_loc_ser>0.85) && (time_glob_ser/time_glob_ser_loc_ser<1.15);
            passed = passed && (time_glob_ser/time_glob_smp_loc_ser>0.85) && (time_glob_ser/time_glob_smp_loc_ser<1.15);
            passed = passed && (time_glob_ser/time_glob_smp_nw1    >0.85) && (time_glob_ser/time_glob_smp_nw1    <1.15);
            passed = passed && (time_glob_ser/time_glob_smp        >1.30);
            passed = passed && (time_glob_ser/time_glob_ser_loc_smp>1.30);
            passed = passed && (time_glob_ser/time_glob_smp_loc_smp>1.30);
            printf(fmt_str, "* test result", passed ? "OK" : "FAILED (soft failure)");
            fflush(stdout);
            //
            // soft failure:
            // // if( !passed )
            // //   return 1;
            //
        }
        else
        {
            printf(fmt_str, "* test skipped (no SMP)", "??");
            fflush(stdout);
        }
    }

    //
    // Testing issues which must be fixed
    //
    printf("Issues:\n");
    {
        //
        // Testing issue #505 (http://bugs.alglib.net/view.php?id=505) in optimizers.
        // This issue was present in ALL optimizers, but we test it only on two: CG and LM.
        //
        try
        {
            //
            // Test CG
            // Stopping criteria - EpsX
            //
            mincgstate state;
            mincgreport rep;
            real_1d_array x = "[0.0]";
            double x0 = 20*alglib::randomreal()-10;
            double epsx = 1.0E-9;
            mincgcreate(1, x, state);
            mincgsetcond(state, 0.0, 0.0, epsx, 0);
            mincgoptimize(state, func505_grad, NULL, &x0);
            mincgresults(state, x, rep);
            issue505_passed = issue505_passed && (fabs(4*pow(x[0]-x0,3))<1.0E-3);
        }
        catch(...)
        { issue505_passed = false; }
        try
        {
            //
            // Test LM
            // Stopping criteria - after |grad|<epsG
            //
            minlmstate state;
            minlmreport rep;
            real_1d_array x = "[0.0]";
            double x0 = 20*alglib::randomreal()-10;
            double epsx = 1.0E-9;
            minlmcreatevj(1, 2, x, state);
            minlmsetcond(state, epsx, 0);
            minlmoptimize(state, func505_vec, func505_jac, NULL, &x0);
            minlmresults(state, x, rep);
            issue505_passed = issue505_passed && (fabs(x[0]-x0)<1.0E-3);
        }
        catch(...)
        { issue505_passed = false; }
        printf(fmt_str, "* issue 505", issue505_passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !issue505_passed )
            return 1;
            
        //
        // Testing issue #478 (http://bugs.alglib.net/view.php?id=478)
        // in high-quality RNG. It have to correctly handle random numbers
        // larger than 2^31.
        //
        // This test is performed only in 64-bit mode.
        //
        if( sizeof(alglib::ae_int_t)>4 )
        {
            //
            // 64-bit mode, perform test:
            // * use large NMax>2^31
            // * generate 1.000.000 random numbers
            // * use two bins - one for numbers less then NMax/2,
            //   another one for the rest of them
            // * bin sizes are equal to n0, n1
            // * both bins should be approximately equal, we use
            //   ad hoc threshold 0.45 < n0,n1 < 0.55.
            //
            try
            {
                alglib::hqrndstate rs;
                alglib::ae_int_t nmax[3];
                alglib::ae_int_t ncnt = 3, nidx;
                double n0, n1;
                alglib::hqrndrandomize(rs);
                
                //
                // nmax:
                // * first nmax is just large value to test basic uniformity of generator
                //
                nmax[0]  = 1000000;
                nmax[0]  = nmax[0]*nmax[0];
                nmax[1]  = 2147483562;
                nmax[1] *= 1.5;
                nmax[2]  = 2147483562;
                nmax[2] *= 3;
                
                for(nidx=0; nidx<ncnt; nidx++)
                {
                    n0 = 0;
                    n1 = 0;
                    for(int i=0; i<1000000; i++)
                    {
                        alglib::ae_int_t v = alglib::hqrnduniformi(rs, nmax[nidx]);
                        if( v<nmax[nidx]/2 )
                            n0++;
                        else
                            n1++;
                        issue478_passed = issue478_passed && (v>=0) && (v<nmax[nidx]);
                    }
                    issue478_passed = issue478_passed && (n0/(n0+n1)>0.45);
                    issue478_passed = issue478_passed && (n0/(n0+n1)<0.55);
                    issue478_passed = issue478_passed && (n1/(n0+n1)>0.45);
                    issue478_passed = issue478_passed && (n1/(n0+n1)<0.55);
                }
            }
            catch(...)
            { issue478_passed = false; }
            printf(fmt_str, "* issue 478", issue478_passed ? "OK" : "FAILED");
            fflush(stdout);
            if( !issue478_passed )
                return 1;
        }
        else
        {
            //
            // 32-bit mode, skip test
            //
            printf(fmt_str, "* issue 478", "OK (skipped in 32-bit mode)");
            fflush(stdout);
        }
        
        //
        // Testing issue #528 (http://bugs.alglib.net/view.php?id=528)
        // in shared pool and smart pointer which leak memory.
        //
        // In order to test it we create pool, seed it with specially
        // created structure, perform several operations, then clear it.
        // We test allocation counter before and after this operation.
        //
#ifdef AE_USE_ALLOC_COUNTER
        try
        {
            int alloc_cnt;
            alglib_impl::ae_state _alglib_env_state;
            alglib_impl::ae_frame _frame_block;
            alglib_impl::ae_shared_pool pool;
            alglib_impl::ae_smart_ptr ptr0, ptr1;
            void *p0, *p1;
            seedrec seed;
            
            // case #0: just seeding the pool
            alloc_cnt = alglib_impl::_alloc_counter;
            alglib_impl::ae_state_init(&_alglib_env_state);
            alglib_impl::ae_frame_make(&_alglib_env_state, &_frame_block);
            memset(&pool, 0, sizeof(pool));
            memset(&seed, 0, sizeof(seed));
            alglib_impl::ae_shared_pool_init(&pool, &_alglib_env_state, true);
            _seedrec_init(&seed, &_alglib_env_state, ae_true);
            alglib_impl::ae_shared_pool_set_seed(&pool, &seed, sizeof(seed), _seedrec_init, _seedrec_init_copy, _seedrec_destroy, &_alglib_env_state);
            alglib_impl::ae_state_clear(&_alglib_env_state);
            issue528_passed = issue528_passed && (alloc_cnt==alglib_impl::_alloc_counter);
            
            // case #1: seeding and retrieving, not recycling
            alloc_cnt = alglib_impl::_alloc_counter;
            alglib_impl::ae_state_init(&_alglib_env_state);
            alglib_impl::ae_frame_make(&_alglib_env_state, &_frame_block);
            memset(&seed, 0, sizeof(seed));
            memset(&pool, 0, sizeof(pool));
            memset(&ptr0, 0, sizeof(ptr0));
            alglib_impl::ae_smart_ptr_init(&ptr0, (void**)&p0, &_alglib_env_state, true);
            alglib_impl::ae_shared_pool_init(&pool, &_alglib_env_state, true);
            _seedrec_init(&seed, &_alglib_env_state, true);
            alglib_impl::ae_shared_pool_set_seed(&pool, &seed, sizeof(seed), _seedrec_init, _seedrec_init_copy, _seedrec_destroy, &_alglib_env_state);
            alglib_impl::ae_shared_pool_retrieve(&pool, &ptr0, &_alglib_env_state);
            alglib_impl::ae_state_clear(&_alglib_env_state);
            issue528_passed = issue528_passed && (alloc_cnt==alglib_impl::_alloc_counter);
            
            // case #2: seeding and retrieving twice to different pointers, recycling both
            alloc_cnt = alglib_impl::_alloc_counter;
            alglib_impl::ae_state_init(&_alglib_env_state);
            alglib_impl::ae_frame_make(&_alglib_env_state, &_frame_block);
            memset(&ptr0, 0, sizeof(ptr0));
            memset(&ptr1, 0, sizeof(ptr1));
            memset(&pool, 0, sizeof(pool));
            memset(&seed, 0, sizeof(seed));
            alglib_impl::ae_smart_ptr_init(&ptr0, (void**)&p0, &_alglib_env_state, true);
            alglib_impl::ae_smart_ptr_init(&ptr1, (void**)&p1, &_alglib_env_state, true);
            alglib_impl::ae_shared_pool_init(&pool, &_alglib_env_state, true);
            _seedrec_init(&seed, &_alglib_env_state, true);
            alglib_impl::ae_shared_pool_set_seed(&pool, &seed, sizeof(seed), _seedrec_init, _seedrec_init_copy, _seedrec_destroy, &_alglib_env_state);
            alglib_impl::ae_shared_pool_retrieve(&pool, &ptr0, &_alglib_env_state);
            alglib_impl::ae_shared_pool_retrieve(&pool, &ptr1, &_alglib_env_state);
            alglib_impl::ae_shared_pool_recycle(&pool,  &ptr0, &_alglib_env_state);
            alglib_impl::ae_shared_pool_recycle(&pool,  &ptr1, &_alglib_env_state);
            alglib_impl::ae_state_clear(&_alglib_env_state);
            issue528_passed = issue528_passed && (alloc_cnt==alglib_impl::_alloc_counter);
        }
        catch(...)
        { issue528_passed = false; }
        printf(fmt_str, "* issue 528", issue528_passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !issue528_passed )
            return 1;
#else
        printf(fmt_str, "* issue 528", "??");
        fflush(stdout);
#endif
        
        //
        // Testing issue #591 (http://bugs.alglib.net/view.php?id=591)
        // in copying of object containing shared pool as one of its
        // fields.
        //
        // Unfixed ALGLIB crashes because of unneeded assertion in the
        // ae_shared_pool_init_copy() function.
        //
        try
        {
            alglib::multilayerperceptron net0, net1;
            alglib::real_1d_array x("[1,2]"), y0("[0,0]"), y1("[0,0]"), y2("[0,0]");
            alglib::mlpcreate0(2, 2, net0);
            alglib::mlpprocess(net0, x, y0);
            
            //
            // Test assignment constructor
            //
            net1 = net0;
            alglib::mlpprocess(net1, x, y1);
            issue591_passed = issue591_passed && (fabs(y0[0]-y1[0])<1.0E-9) && (fabs(y0[1]-y1[1])<1.0E-9);
            
            //
            // Test copy constructor
            //
            alglib::multilayerperceptron net2(net0);
            alglib::mlpprocess(net2, x, y2);
            issue591_passed = issue591_passed && (fabs(y0[0]-y2[0])<1.0E-9) && (fabs(y0[1]-y2[1])<1.0E-9);
        }
        catch(...)
        { issue591_passed = false; }
        printf(fmt_str, "* issue 591", issue591_passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !issue591_passed )
            return 1;
        
        //
        // Task #594 (http://bugs.alglib.net/view.php?id=594) - additional
        // test for correctness of copying of objects. When we copy ALGLIB
        // object, indenendent new copy is created.
        //
        // This test checks both copying with copy constructor and assignment
        // constructor
        //
        try
        {
            alglib::multilayerperceptron net0, net1;
            alglib::real_1d_array x("[1,2]"), y0("[0,0]"), y1("[0,0]"), y2("[0,0]");
            alglib::mlpcreate0(2, 2, net0);
            alglib::mlpprocess(net0, x, y0);
            
            //
            // Test assignment and copy constructors:
            // * copy object with one of the constructors
            // * process vector with original network
            // * randomize original network
            // * process vector with copied networks and compare
            //
            net1 = net0;
            alglib::multilayerperceptron net2(net0);
            alglib::mlprandomize(net0);
            alglib::mlpprocess(net1, x, y1);
            alglib::mlpprocess(net2, x, y2);
            issue594_passed = issue594_passed && (fabs(y0[0]-y1[0])<1.0E-9) && (fabs(y0[1]-y1[1])<1.0E-9);
            issue594_passed = issue594_passed && (fabs(y0[0]-y2[0])<1.0E-9) && (fabs(y0[1]-y2[1])<1.0E-9);
        }
        catch(...)
        { issue594_passed = false; }
        printf(fmt_str, "* issue 594", issue594_passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !issue594_passed )
            return 1;
        
        //
        // Issue 764#, potential memory leak in the smart pointer
        //
#ifdef AE_USE_ALLOC_COUNTER
        try
        {
            int alloc_cnt;
            alglib_impl::ae_state _alglib_env_state;
            alglib_impl::ae_frame _frame_block;
            alglib_impl::ae_shared_pool pool;
            alglib_impl::ae_smart_ptr ptr0;
            void *p0, *p1;
            seedrec seed;
            
            // seeding shared pool and retrieving twice to same pointer, no recycling
            alloc_cnt = alglib_impl::_alloc_counter;
            alglib_impl::ae_state_init(&_alglib_env_state);
            alglib_impl::ae_frame_make(&_alglib_env_state, &_frame_block);
            memset(&ptr0, 0, sizeof(ptr0));
            memset(&pool, 0, sizeof(pool));
            memset(&seed, 0, sizeof(seed));
            alglib_impl::ae_smart_ptr_init(&ptr0, (void**)&p0, &_alglib_env_state, true);
            alglib_impl::ae_shared_pool_init(&pool, &_alglib_env_state, true);
            _seedrec_init(&seed, &_alglib_env_state, true);
            alglib_impl::ae_shared_pool_set_seed(&pool, &seed, sizeof(seed), _seedrec_init, _seedrec_init_copy, _seedrec_destroy, &_alglib_env_state);
            alglib_impl::ae_shared_pool_retrieve(&pool, &ptr0, &_alglib_env_state);
            alglib_impl::ae_shared_pool_retrieve(&pool, &ptr0, &_alglib_env_state);
            alglib_impl::ae_state_clear(&_alglib_env_state);
            issue764_passed = issue764_passed && (alloc_cnt==alglib_impl::_alloc_counter);
        }
        catch(...)
        { issue764_passed = false; }
        printf(fmt_str, "* issue 764", issue764_passed ? "OK" : "FAILED");
        fflush(stdout);
        if( !issue764_passed )
            return 1;
#else
        printf(fmt_str, "* issue 764", "??");
        fflush(stdout);
#endif

        //
        // Issue 813: MSVC is unable to handle longjmp() from catch() block; the code
        //            cycles forever.
        //
        {
            alglib::minlmstate state;
            alglib::real_1d_array x;
            x.setlength(1);
            x[0] = 0;
            alglib::minlmcreatev(1, x, 1e-5, state);
            issue813_passed = false;
            try
            {
                alglib::minlmoptimize(state, &issue813_callback);
            }
            catch(...)
            {
                issue813_passed = true;
            }
            printf(fmt_str, "* issue 813", issue813_passed ? "OK" : "FAILED");
            fflush(stdout);
            if( !issue813_passed )
                return 1;
        }

        //
        // Issue 824: pre-3.16 versions of ALGLIB hide exceptions generated in user callbacks
        //
        {
            alglib::mincgstate state;
            alglib::real_1d_array x;
            x.setlength(1);
            x[0] = 0;
            alglib::mincgcreatef(1, x, 1e-5, state);
            issue824_passed = true;
            
            // throw int*
            try
            {
                alglib::mincgoptimize(state, &issue824_callback_i);
            }
            catch(int*)
            {
            }
            catch(double *)
            {
                issue824_passed = false;
            }
            catch(...)
            {
                issue824_passed = false;
            }
            
            // throw double*
            try
            {
                alglib::mincgoptimize(state, &issue824_callback_d);
            }
            catch(int*)
            {
                issue824_passed = false;
            }
            catch(double *)
            {
            }
            catch(...)
            {
                issue824_passed = false;
            }
            
            // done
            printf(fmt_str, "* issue 824", issue824_passed ? "OK" : "FAILED");
            fflush(stdout);
            if( !issue824_passed )
                return 1;
        }
    }
    
    //
    // Performance testing
    //
    printf("Performance:\n");
    {
        {
            int _n[]       = { 16, 32, 64, 1024, 0};
            int i, j, k, t, nidx;
            for(nidx=0; _n[nidx]!=0; nidx++)
            {
                //
                // Settings:
                // * n - matrix size
                // * nrepeat - number of repeated multiplications, always divisible by 4
                //
                int n = _n[nidx];
                double desiredflops = n>64 ? 1.0E10 : 1.0E9;
                int nrepeat = (int)(desiredflops/(2*pow((double)n,3.0)));
                nrepeat = 4*(nrepeat/4+1);
                
                //
                // Actual processing
                //
                alglib::real_2d_array a, b, c;
                double perf0, perf1, perf2;
                a.setlength(n, n);
                b.setlength(n, n);
                c.setlength(n, n);
                for(i=0; i<n; i++)
                    for(j=0; j<n; j++)
                    {
                        a[i][j] = alglib::randomreal()-0.5;
                        b[i][j] = alglib::randomreal()-0.5;
                        c[i][j] = 0.0;
                    }
                
                t = alglib_impl::ae_tickcount();
                for(k=0; k<nrepeat; k++)
                    alglib::rmatrixgemm(
                        n, n, n,
                        1.0,
                        a, 0, 0, k%2,
                        b, 0, 0, (k/2)%2,
                        0.0,
                        c, 0, 0);
                t = alglib_impl::ae_tickcount()-t;
                perf0 = 1.0E-6*pow((double)n,3)*2.0*nrepeat/(0.001*t);
                printf("* RGEMM-SEQ-%-4ld (MFLOPS)  %5.0lf\n", (long)n, (double)perf0);
                
                alglib::setnworkers(0);
                t = alglib_impl::ae_tickcount();
                for(k=0; k<nrepeat; k++)
                    alglib::rmatrixgemm(
                        n, n, n,
                        1.0,
                        a, 0, 0, k%2,
                        b, 0, 0, (k/2)%2,
                        0.0,
                        c, 0, 0, alglib::parallel);
                t = alglib_impl::ae_tickcount()-t;
                perf2 = 1.0E-6*pow((double)n,3)*2.0*nrepeat/(0.001*t);
                printf("* RGEMM-MTN-%-4ld           %4.1lfx\n", (long)n, (double)(perf2/perf0));
                alglib::setnworkers(1);
                    
            }
        }
    }
    
    //
    // Check allocation counter on exit
    //
#ifdef AE_USE_ALLOC_COUNTER
    printf("Allocation counter checked... ");
#ifdef _ALGLIB_HAS_WORKSTEALING
    alglib_impl::ae_free_disposed_items();
    alglib_impl::ae_complete_finalization_before_exit();
#endif
    if( alglib_impl::_alloc_counter!=0 )
    {
        printf("FAILURE: alloc_counter is non-zero on end!\n");
        return 1;
    }
    else
        printf("OK\n");
#endif
    
    //
    // Return
    //
    return 0;
}
