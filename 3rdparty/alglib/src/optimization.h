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
#ifndef _optimization_pkg_h
#define _optimization_pkg_h
#include "ap.h"
#include "alglibinternal.h"
#include "linalg.h"
#include "alglibmisc.h"
#include "solvers.h"

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS COMPUTATIONAL CORE DECLARATIONS (DATATYPES)
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
#if defined(AE_COMPILE_OPTGUARDAPI) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_bool nonc0suspected;
    ae_bool nonc0test0positive;
    ae_int_t nonc0fidx;
    double nonc0lipschitzc;
    ae_bool nonc1suspected;
    ae_bool nonc1test0positive;
    ae_bool nonc1test1positive;
    ae_int_t nonc1fidx;
    double nonc1lipschitzc;
    ae_bool badgradsuspected;
    ae_int_t badgradfidx;
    ae_int_t badgradvidx;
    ae_vector badgradxbase;
    ae_matrix badgraduser;
    ae_matrix badgradnum;
} optguardreport;
typedef struct
{
    ae_bool positive;
    ae_int_t fidx;
    ae_vector x0;
    ae_vector d;
    ae_int_t n;
    ae_vector stp;
    ae_vector f;
    ae_int_t cnt;
    ae_int_t stpidxa;
    ae_int_t stpidxb;
    ae_int_t inneriter;
    ae_int_t outeriter;
} optguardnonc0report;
typedef struct
{
    ae_bool positive;
    ae_int_t fidx;
    ae_vector x0;
    ae_vector d;
    ae_int_t n;
    ae_vector stp;
    ae_vector f;
    ae_int_t cnt;
    ae_int_t stpidxa;
    ae_int_t stpidxb;
    ae_int_t inneriter;
    ae_int_t outeriter;
} optguardnonc1test0report;
typedef struct
{
    ae_bool positive;
    ae_int_t fidx;
    ae_int_t vidx;
    ae_vector x0;
    ae_vector d;
    ae_int_t n;
    ae_vector stp;
    ae_vector g;
    ae_int_t cnt;
    ae_int_t stpidxa;
    ae_int_t stpidxb;
    ae_int_t inneriter;
    ae_int_t outeriter;
} optguardnonc1test1report;
#endif
#if defined(AE_COMPILE_OPTSERV) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_vector norms;
    ae_vector alpha;
    ae_vector rho;
    ae_matrix yk;
    ae_vector idx;
    ae_vector bufa;
    ae_vector bufb;
} precbuflbfgs;
typedef struct
{
    ae_int_t n;
    ae_int_t k;
    ae_vector d;
    ae_matrix v;
    ae_vector bufc;
    ae_matrix bufz;
    ae_matrix bufw;
    ae_vector tmp;
} precbuflowrank;
typedef struct
{
    ae_int_t htype;
    ae_int_t n;
    ae_int_t resetfreq;
    double stpshort;
    double gammasml;
    double reg;
    double smallreg;
    double microreg;
    double wolfeeps;
    double maxhess;
    ae_int_t m;
    ae_matrix hcurrent;
    ae_int_t hage;
    double sumy2;
    double sums2;
    double sumsy;
    ae_int_t memlen;
    double sigma;
    double gamma;
    ae_matrix s;
    ae_matrix y;
    double sigmadecay;
    ae_bool lowrankmodelvalid;
    ae_int_t lowrankk;
    ae_matrix lowrankcp;
    ae_matrix lowrankcm;
    ae_bool lowrankeffdvalid;
    ae_vector lowrankeffd;
    ae_matrix lowranksst;
    ae_matrix lowranksyt;
    ae_int_t updatestatus;
    ae_matrix hincoming;
    ae_vector sk;
    ae_vector yk;
    ae_vector hsk;
    ae_vector buf;
    ae_matrix corr2;
    ae_matrix blk;
    ae_matrix jk;
    ae_matrix invsqrtdlk;
    ae_vector bufvmv;
    ae_vector bufupdhx;
} xbfgshessian;
typedef struct
{
    ae_int_t n;
    ae_int_t k;
    ae_bool checksmoothness;
    ae_vector s;
    ae_vector dcur;
    ae_int_t enqueuedcnt;
    ae_vector enqueuedstp;
    ae_vector enqueuedx;
    ae_vector enqueuedfunc;
    ae_matrix enqueuedjac;
    ae_vector sortedstp;
    ae_vector sortedidx;
    ae_int_t sortedcnt;
    ae_int_t lagprobinneriter;
    ae_int_t lagprobouteriter;
    double lagprobstepmax;
    ae_int_t lagprobnstepsstored;
    ae_vector lagprobxs;
    ae_vector lagprobd;
    double lagprobstp;
    ae_vector lagprobx;
    ae_vector lagprobfi;
    double lagprobrawlag;
    ae_matrix lagprobj;
    ae_matrix lagprobvalues;
    ae_matrix lagprobjacobians;
    ae_vector lagprobsteps;
    ae_vector lagproblagrangians;
    rcommstate lagrangianprobingrcomm;
    ae_bool linesearchspoiled;
    ae_bool linesearchstarted;
    ae_int_t linesearchinneridx;
    ae_int_t linesearchouteridx;
    double nonc0currentrating;
    double nonc1currentrating;
    ae_bool badgradhasxj;
    optguardreport rep;
    double nonc0strrating;
    double nonc0lngrating;
    optguardnonc0report nonc0strrep;
    optguardnonc0report nonc0lngrep;
    double nonc1test0strrating;
    double nonc1test0lngrating;
    optguardnonc1test0report nonc1test0strrep;
    optguardnonc1test0report nonc1test0lngrep;
    double nonc1test1strrating;
    double nonc1test1lngrating;
    optguardnonc1test1report nonc1test1strrep;
    optguardnonc1test1report nonc1test1lngrep;
    ae_bool needfij;
    ae_vector x;
    ae_vector fi;
    ae_matrix j;
    rcommstate rstateg0;
    ae_vector xbase;
    ae_vector fbase;
    ae_vector fm;
    ae_vector fc;
    ae_vector fp;
    ae_vector jm;
    ae_vector jc;
    ae_vector jp;
    ae_matrix jbaseusr;
    ae_matrix jbasenum;
    ae_vector stp;
    ae_vector bufr;
    ae_vector f;
    ae_vector g;
    ae_vector deltax;
    ae_vector tmpidx;
    ae_vector bufi;
    ae_vector xu;
    ae_vector du;
    ae_vector f0;
    ae_matrix j0;
} smoothnessmonitor;
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t nactive;
    ae_matrix xsol;
    ae_vector tgtc;
    ae_matrix tgtb;
    ae_matrix tgta;
    ae_matrix tgtd;
    ae_vector bndl;
    ae_vector bndu;
    ae_matrix densea;
    ae_vector al;
    ae_vector au;
    ae_int_t nlinear;
    ae_vector nlc;
    ae_matrix nlb;
    ae_matrix nla;
    ae_matrix nld;
    ae_vector hl;
    ae_vector hu;
    ae_int_t nnonlinear;
} multiobjectivetestfunction;
#endif
#if defined(AE_COMPILE_MINLBFGS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    double stpmax;
    ae_vector s;
    double diffstep;
    ae_int_t nfev;
    ae_int_t mcstage;
    ae_int_t k;
    ae_int_t q;
    ae_int_t p;
    ae_vector rho;
    ae_matrix yk;
    ae_matrix sk;
    ae_vector xp;
    ae_vector theta;
    ae_vector d;
    double stp;
    double longeststp;
    ae_vector work;
    double fold;
    double trimthreshold;
    ae_vector xbase;
    ae_int_t prectype;
    double gammak;
    ae_matrix denseh;
    ae_vector diagh;
    ae_vector precc;
    ae_vector precd;
    ae_matrix precw;
    ae_int_t preck;
    precbuflbfgs precbuf;
    precbuflowrank lowrankbuf;
    double fbase;
    double fm2;
    double fm1;
    double fp1;
    double fp2;
    double stplimit;
    ae_vector autobuf;
    ae_vector invs;
    ae_vector x;
    double f;
    ae_vector g;
    ae_bool needf;
    ae_bool needfg;
    ae_bool xupdated;
    ae_bool userterminationneeded;
    double teststep;
    rcommstate rstate;
    ae_int_t repiterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    linminstate lstate;
    ae_int_t smoothnessguardlevel;
    smoothnessmonitor smonitor;
    ae_vector lastscaleused;
} minlbfgsstate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t terminationtype;
} minlbfgsreport;
#endif
#if defined(AE_COMPILE_CQMODELS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t k;
    double alpha;
    double tau;
    double theta;
    ae_matrix a;
    ae_matrix q;
    ae_vector b;
    ae_vector r;
    ae_vector xc;
    ae_vector d;
    ae_vector activeset;
    ae_matrix tq2dense;
    ae_matrix tk2;
    ae_vector tq2diag;
    ae_vector tq1;
    ae_vector tk1;
    double tq0;
    double tk0;
    ae_vector txc;
    ae_vector tb;
    ae_int_t nfree;
    ae_int_t ecakind;
    ae_matrix ecadense;
    ae_matrix eq;
    ae_matrix eccm;
    ae_vector ecadiag;
    ae_vector eb;
    double ec;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmpg;
    ae_matrix tmp2;
    ae_bool ismaintermchanged;
    ae_bool issecondarytermchanged;
    ae_bool islineartermchanged;
    ae_bool isactivesetchanged;
} convexquadraticmodel;
#endif
#if defined(AE_COMPILE_LPQPSERV) || !defined(AE_PARTIAL_BUILD)
#endif
#if defined(AE_COMPILE_SNNLS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t ns;
    ae_int_t nd;
    ae_int_t nr;
    ae_matrix densea;
    ae_vector b;
    ae_vector nnc;
    double debugflops;
    ae_int_t debugmaxinnerits;
    ae_vector xn;
    ae_vector xp;
    ae_matrix tmpca;
    ae_matrix tmplq;
    ae_matrix trda;
    ae_vector trdd;
    ae_vector crb;
    ae_vector g;
    ae_vector d;
    ae_vector dx;
    ae_vector diagaa;
    ae_vector cb;
    ae_vector cx;
    ae_vector cborg;
    ae_vector tmpcholesky;
    ae_vector r;
    ae_vector regdiag;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_vector rdtmprowmap;
} snnlssolver;
#endif
#if defined(AE_COMPILE_SACTIVESETS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t algostate;
    ae_vector xc;
    ae_bool hasxc;
    ae_vector s;
    ae_vector h;
    ae_vector cstatus;
    ae_bool basisisready;
    ae_matrix sdensebatch;
    ae_matrix pdensebatch;
    ae_matrix idensebatch;
    ae_int_t densebatchsize;
    ae_vector sparsebatch;
    ae_int_t sparsebatchsize;
    ae_int_t basisage;
    ae_bool feasinitpt;
    ae_bool constraintschanged;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector bndl;
    ae_vector bndu;
    ae_matrix cleic;
    ae_int_t nec;
    ae_int_t nic;
    ae_vector mtnew;
    ae_vector mtx;
    ae_vector mtas;
    ae_vector cdtmp;
    ae_vector corrtmp;
    ae_vector unitdiagonal;
    snnlssolver solver;
    ae_vector scntmp;
    ae_vector tmp0;
    ae_vector tmpfeas;
    ae_matrix tmpm0;
    ae_vector rctmps;
    ae_vector rctmpg;
    ae_vector rctmprightpart;
    ae_matrix rctmpdense0;
    ae_matrix rctmpdense1;
    ae_vector rctmpisequality;
    ae_vector rctmpconstraintidx;
    ae_vector rctmplambdas;
    ae_matrix tmpbasis;
    ae_vector tmpnormestimates;
    ae_vector tmpreciph;
    ae_vector tmpprodp;
    ae_vector tmpprods;
    ae_vector tmpcp;
    ae_vector tmpcs;
    ae_vector tmpci;
} sactiveset;
#endif
#if defined(AE_COMPILE_QQPSOLVER) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxouterits;
    ae_bool cgphase;
    ae_bool cnphase;
    ae_int_t cgminits;
    ae_int_t cgmaxits;
    ae_int_t cnmaxupdates;
    ae_int_t sparsesolver;
} qqpsettings;
typedef struct
{
    ae_int_t n;
    ae_int_t akind;
    ae_matrix densea;
    sparsematrix sparsea;
    ae_bool sparseupper;
    double absamax;
    double absasum;
    double absasum2;
    ae_vector b;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector havebndl;
    ae_vector havebndu;
    ae_vector xs;
    ae_vector xf;
    ae_vector gc;
    ae_vector xp;
    ae_vector dc;
    ae_vector dp;
    ae_vector cgc;
    ae_vector cgp;
    sactiveset sas;
    ae_vector activated;
    ae_int_t nfree;
    ae_int_t cnmodelage;
    ae_matrix densez;
    sparsematrix sparsecca;
    ae_vector yidx;
    ae_vector regdiag;
    ae_vector regx0;
    ae_vector tmpcn;
    ae_vector tmpcni;
    ae_vector tmpcnb;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector stpbuf;
    sparsebuffers sbuf;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repncholesky;
    ae_int_t repncupdates;
} qqpbuffers;
#endif
#if defined(AE_COMPILE_QPDENSEAULSOLVER) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    double epsx;
    ae_int_t outerits;
    double rho;
} qpdenseaulsettings;
typedef struct
{
    ae_vector nulc;
    ae_matrix sclsfta;
    ae_vector sclsftb;
    ae_vector sclsfthasbndl;
    ae_vector sclsfthasbndu;
    ae_vector sclsftbndl;
    ae_vector sclsftbndu;
    ae_vector sclsftxc;
    ae_matrix sclsftcleic;
    ae_vector cidx;
    ae_vector cscales;
    ae_matrix exa;
    ae_vector exb;
    ae_vector exxc;
    ae_vector exbndl;
    ae_vector exbndu;
    ae_vector exscale;
    ae_vector exxorigin;
    qqpsettings qqpsettingsuser;
    qqpbuffers qqpbuf;
    ae_vector nulcest;
    ae_vector tmpg;
    ae_vector tmp0;
    ae_matrix tmp2;
    ae_vector modelg;
    ae_vector d;
    ae_vector deltax;
    convexquadraticmodel dummycqm;
    sparsematrix dummysparse;
    ae_matrix qrkkt;
    ae_vector qrrightpart;
    ae_vector qrtau;
    ae_vector qrsv0;
    ae_vector qrsvx1;
    ae_vector nicerr;
    ae_vector nicnact;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repncholesky;
    ae_int_t repnwrkchanges;
    ae_int_t repnwrk0;
    ae_int_t repnwrk1;
    ae_int_t repnwrkf;
    ae_int_t repnmv;
} qpdenseaulbuffers;
#endif
#if defined(AE_COMPILE_MINBLEIC) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t nmain;
    ae_int_t nslack;
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    ae_bool drep;
    double stpmax;
    double diffstep;
    sactiveset sas;
    ae_vector s;
    ae_int_t prectype;
    ae_vector diagh;
    ae_vector x;
    double f;
    ae_vector g;
    ae_bool needf;
    ae_bool needfg;
    ae_bool xupdated;
    ae_bool lsstart;
    ae_bool steepestdescentstep;
    ae_bool boundedstep;
    ae_bool userterminationneeded;
    rcommstate rstate;
    ae_vector ugc;
    ae_vector cgc;
    ae_vector xn;
    ae_vector ugn;
    ae_vector cgn;
    ae_vector xp;
    double fc;
    double fn;
    double fp;
    ae_vector d;
    ae_matrix cleic;
    ae_int_t nec;
    ae_int_t nic;
    double lastgoodstep;
    double lastscaledgoodstep;
    double maxscaledgrad;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repnfev;
    ae_int_t repvaridx;
    ae_int_t repterminationtype;
    double repdebugeqerr;
    double repdebugfs;
    double repdebugff;
    double repdebugdx;
    ae_int_t repdebugfeasqpits;
    ae_int_t repdebugfeasgpaits;
    ae_vector xstart;
    snnlssolver solver;
    double fbase;
    double fm2;
    double fm1;
    double fp1;
    double fp2;
    double xm1;
    double xp1;
    double gm1;
    double gp1;
    ae_int_t cidx;
    double cval;
    ae_vector tmpprec;
    ae_vector tmp0;
    ae_int_t nfev;
    ae_int_t mcstage;
    double stp;
    double curstpmax;
    double activationstep;
    ae_vector work;
    linminstate lstate;
    double trimthreshold;
    ae_int_t nonmonotoniccnt;
    ae_matrix bufyk;
    ae_matrix bufsk;
    ae_vector bufrho;
    ae_vector buftheta;
    ae_int_t bufsize;
    double teststep;
    ae_int_t smoothnessguardlevel;
    smoothnessmonitor smonitor;
    ae_vector lastscaleused;
    ae_vector invs;
} minbleicstate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t varidx;
    ae_int_t terminationtype;
    double debugeqerr;
    double debugfs;
    double debugff;
    double debugdx;
    ae_int_t debugfeasqpits;
    ae_int_t debugfeasgpaits;
    ae_int_t inneriterationscount;
    ae_int_t outeriterationscount;
} minbleicreport;
#endif
#if defined(AE_COMPILE_QPBLEICSOLVER) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxits;
} qpbleicsettings;
typedef struct
{
    minbleicstate solver;
    minbleicreport solverrep;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmpi;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
} qpbleicbuffers;
#endif
#if defined(AE_COMPILE_VIPMSOLVER) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    ae_vector x;
    ae_vector g;
    ae_vector w;
    ae_vector t;
    ae_vector p;
    ae_vector y;
    ae_vector z;
    ae_vector v;
    ae_vector s;
    ae_vector q;
} vipmvars;
typedef struct
{
    sparsematrix rawsystem;
    ae_vector effectivediag;
    ae_vector isdiagonal;
    ae_vector rowdegrees;
    ae_vector coldegrees;
    ae_int_t ntotal;
    spcholanalysis analysis;
    ae_vector priorities;
    ae_vector diagterm;
    ae_vector dampterm;
    ae_vector tmpb;
    ae_vector tmprhs;
    ae_vector tmpcorr;
} vipmreducedsparsesystem;
typedef struct
{
    ae_vector sigma;
    ae_vector beta;
    ae_vector rho;
    ae_vector nu;
    ae_vector tau;
    ae_vector alpha;
    ae_vector gammaz;
    ae_vector gammas;
    ae_vector gammaw;
    ae_vector gammaq;
} vipmrighthandside;
typedef struct
{
    ae_bool slacksforequalityconstraints;
    ae_int_t n;
    ae_int_t nmain;
    double epsp;
    double epsd;
    double epsgap;
    ae_bool islinear;
    ae_vector scl;
    ae_vector invscl;
    ae_vector xorigin;
    double targetscale;
    ae_vector c;
    ae_matrix denseh;
    sparsematrix sparseh;
    ae_vector diagr;
    ae_int_t hkind;
    ae_bool isdiagonalh;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector rawbndl;
    ae_vector rawbndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_matrix denseafull;
    ae_matrix denseamain;
    sparsematrix sparseafull;
    sparsematrix sparseamain;
    sparsematrix combinedaslack;
    ae_vector ascales;
    ae_vector aflips;
    ae_vector b;
    ae_vector r;
    ae_vector hasr;
    ae_int_t mdense;
    ae_int_t msparse;
    vipmvars current;
    vipmvars best;
    vipmvars trial;
    vipmvars deltaaff;
    vipmvars deltacorr;
    ae_vector isfrozen;
    ae_vector hasgz;
    ae_vector hasts;
    ae_vector haswv;
    ae_vector haspq;
    ae_int_t cntgz;
    ae_int_t cntts;
    ae_int_t cntwv;
    ae_int_t cntpq;
    ae_int_t repiterationscount;
    ae_int_t repncholesky;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    ae_int_t factorizationtype;
    ae_bool factorizationpoweredup;
    ae_bool factorizationpresent;
    ae_vector diagdz;
    ae_vector diagdzi;
    ae_vector diagdziri;
    ae_vector diagds;
    ae_vector diagdsi;
    ae_vector diagdsiri;
    ae_vector diagdw;
    ae_vector diagdwi;
    ae_vector diagdwir;
    ae_vector diagdq;
    ae_vector diagdqi;
    ae_vector diagdqiri;
    ae_vector diagddr;
    ae_vector diagde;
    ae_vector diagder;
    ae_matrix factdensehaug;
    ae_vector factregdhrh;
    ae_vector factinvregdzrz;
    ae_vector factregewave;
    ae_vector facttmpdiag;
    ae_vector facttmpdamp;
    vipmreducedsparsesystem reducedsparsesystem;
    vipmrighthandside rhs;
    ae_vector rhsalphacap;
    ae_vector rhsbetacap;
    ae_vector rhsnucap;
    ae_vector rhstaucap;
    ae_vector deltaxy;
    ae_vector tmphx;
    ae_vector tmpax;
    ae_vector tmpaty;
    vipmvars zerovars;
    ae_vector dummyr;
    ae_vector tmpy;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_matrix tmpr2;
    ae_vector tmplaggrad;
    ae_vector tmpi;
    sparsematrix tmpsparse0;
} vipmstate;
#endif
#if defined(AE_COMPILE_MINQP) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    qqpsettings qqpsettingsuser;
    qpbleicsettings qpbleicsettingsuser;
    qpdenseaulsettings qpdenseaulsettingsuser;
    double veps;
    ae_bool dbgskipconstraintnormalization;
    ae_int_t algokind;
    ae_int_t akind;
    convexquadraticmodel a;
    sparsematrix sparsea;
    ae_bool sparseaupper;
    double absamax;
    double absasum;
    double absasum2;
    ae_vector b;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t stype;
    ae_vector s;
    ae_vector havebndl;
    ae_vector havebndu;
    ae_vector xorigin;
    ae_vector startx;
    ae_bool havex;
    ae_matrix densec;
    sparsematrix sparsec;
    ae_vector cl;
    ae_vector cu;
    ae_int_t mdense;
    ae_int_t msparse;
    ae_vector xs;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repncholesky;
    ae_int_t repnmv;
    ae_int_t repterminationtype;
    ae_vector replagbc;
    ae_vector replaglc;
    ae_vector effectives;
    ae_vector tmp0;
    ae_matrix ecleic;
    ae_vector elaglc;
    ae_vector elagmlt;
    ae_vector elagidx;
    ae_vector dummyr;
    ae_matrix dummyr2;
    sparsematrix dummysparse;
    ae_matrix tmpr2;
    ae_vector wrkbndl;
    ae_vector wrkbndu;
    ae_vector wrkcl;
    ae_vector wrkcu;
    ae_matrix wrkdensec;
    sparsematrix wrksparsec;
    ae_bool qpbleicfirstcall;
    qpbleicbuffers qpbleicbuf;
    qqpbuffers qqpbuf;
    qpdenseaulbuffers qpdenseaulbuf;
    vipmstate vsolver;
} minqpstate;
typedef struct
{
    ae_int_t inneriterationscount;
    ae_int_t outeriterationscount;
    ae_int_t nmv;
    ae_int_t ncholesky;
    ae_int_t terminationtype;
    ae_vector lagbc;
    ae_vector laglc;
} minqpreport;
#endif
#if defined(AE_COMPILE_MINLM) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    double stpmax;
    ae_int_t modelage;
    ae_int_t maxmodelage;
    ae_bool hasfi;
    double epsx;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_bool needf;
    ae_bool needfi;
    double fbase;
    ae_vector modeldiag;
    ae_vector xbase;
    ae_vector fibase;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector havebndl;
    ae_vector havebndu;
    ae_vector s;
    rcommstate rstate;
    ae_vector xdir;
    ae_vector choleskybuf;
    ae_vector tmp0;
    ae_vector tmpct;
    double actualdecrease;
    double predicteddecrease;
    minqpstate qpstate;
    minqpreport qprep;
    sparsematrix tmpsp;
} minlmstepfinder;
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    double diffstep;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    double stpmax;
    ae_int_t maxmodelage;
    ae_bool makeadditers;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_matrix j;
    ae_matrix h;
    ae_vector g;
    ae_bool needf;
    ae_bool needfg;
    ae_bool needfgh;
    ae_bool needfij;
    ae_bool needfi;
    ae_bool xupdated;
    ae_bool userterminationneeded;
    ae_int_t algomode;
    ae_bool hasf;
    ae_bool hasfi;
    ae_bool hasg;
    ae_vector xbase;
    double fbase;
    ae_vector fibase;
    ae_vector gbase;
    ae_matrix quadraticmodel;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector havebndl;
    ae_vector havebndu;
    ae_vector s;
    ae_matrix cleic;
    ae_int_t nec;
    ae_int_t nic;
    double lambdav;
    double nu;
    ae_int_t modelage;
    ae_vector xnew;
    ae_vector xdir;
    ae_vector deltax;
    ae_vector deltaf;
    ae_bool deltaxready;
    ae_bool deltafready;
    smoothnessmonitor smonitor;
    double teststep;
    ae_vector lastscaleused;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    ae_int_t repnfunc;
    ae_int_t repnjac;
    ae_int_t repngrad;
    ae_int_t repnhess;
    ae_int_t repncholesky;
    rcommstate rstate;
    ae_vector choleskybuf;
    ae_vector tmp0;
    double actualdecrease;
    double predicteddecrease;
    double xm1;
    double xp1;
    ae_vector fm1;
    ae_vector fp1;
    ae_vector fc1;
    ae_vector gm1;
    ae_vector gp1;
    ae_vector gc1;
    minlbfgsstate internalstate;
    minlbfgsreport internalrep;
    minqpstate qpstate;
    minqpreport qprep;
    minlmstepfinder finderstate;
} minlmstate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t terminationtype;
    ae_int_t nfunc;
    ae_int_t njac;
    ae_int_t ngrad;
    ae_int_t nhess;
    ae_int_t ncholesky;
} minlmreport;
#endif
#if defined(AE_COMPILE_MINCG) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxits;
    double stpmax;
    double suggestedstep;
    ae_bool xrep;
    ae_bool drep;
    ae_int_t cgtype;
    ae_int_t prectype;
    ae_vector diagh;
    ae_vector diaghl2;
    ae_matrix vcorr;
    ae_int_t vcnt;
    ae_vector s;
    double diffstep;
    ae_int_t nfev;
    ae_int_t mcstage;
    ae_int_t k;
    ae_vector xk;
    ae_vector dk;
    ae_vector xn;
    ae_vector dn;
    ae_vector d;
    double fold;
    double stp;
    double curstpmax;
    ae_vector yk;
    double lastgoodstep;
    double lastscaledstep;
    ae_int_t mcinfo;
    ae_bool innerresetneeded;
    ae_bool terminationneeded;
    double trimthreshold;
    ae_vector xbase;
    ae_int_t rstimer;
    ae_vector x;
    double f;
    ae_vector g;
    ae_bool needf;
    ae_bool needfg;
    ae_bool xupdated;
    ae_bool algpowerup;
    ae_bool lsstart;
    ae_bool lsend;
    ae_bool userterminationneeded;
    rcommstate rstate;
    ae_int_t repiterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    ae_int_t debugrestartscount;
    linminstate lstate;
    double fbase;
    double fm2;
    double fm1;
    double fp1;
    double fp2;
    double betahs;
    double betady;
    ae_vector work0;
    ae_vector work1;
    ae_vector invs;
    double teststep;
    ae_int_t smoothnessguardlevel;
    smoothnessmonitor smonitor;
    ae_vector lastscaleused;
} mincgstate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t terminationtype;
} mincgreport;
#endif
#if defined(AE_COMPILE_NLCSQP) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t algokind;
    vipmstate ipmsolver;
    ae_vector curb;
    ae_vector curbndl;
    ae_vector curbndu;
    ae_vector cural;
    ae_vector curau;
    sparsematrix sparserawlc;
    sparsematrix sparseefflc;
    ae_vector d0;
    ae_matrix denseh;
    ae_vector dummy1;
    ae_matrix densedummy;
    sparsematrix sparsedummy;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    ae_vector retainnegativebclm;
    ae_vector retainpositivebclm;
    ae_vector rescalelag;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector hasal;
    ae_vector hasau;
    ae_matrix activea;
    ae_vector activerhs;
    ae_vector activeidx;
    ae_int_t activesetsize;
} minsqpsubsolver;
typedef struct
{
    ae_vector sclagtmp0;
    ae_vector sclagtmp1;
} minsqptmplagrangian;
typedef struct
{
    ae_vector mftmp0;
} minsqptmpmerit;
typedef struct
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    ae_vector dtrial;
    ae_vector deffective;
    ae_vector d0;
    ae_vector d1;
    ae_vector dmu;
    ae_vector stepkx;
    ae_vector stepkxc;
    ae_vector stepkxn;
    ae_vector stepkfi;
    ae_vector stepkfic;
    ae_vector stepkfin;
    ae_matrix stepkj;
    ae_matrix stepkjc;
    ae_matrix stepkjn;
    ae_vector curlinx;
    ae_vector curlinfi;
    ae_matrix curlinj;
    ae_vector lagbcmult;
    ae_vector lagxcmult;
    ae_vector dummylagbcmult;
    ae_vector dummylagxcmult;
    minsqptmpmerit tmpmerit;
    minsqptmplagrangian tmplagrangianfg;
    ae_vector stepklaggrad;
    ae_vector stepknlaggrad;
    ae_int_t status;
    ae_bool increasebigc;
    ae_bool increasemeritmu;
    ae_bool meritfstagnated;
    ae_vector tmphdiag;
    rcommstate rmeritphasestate;
} minsqpmeritphasestate;
typedef struct
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    ae_vector s;
    ae_matrix scaledcleic;
    ae_vector lcsrcidx;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    double epsx;
    ae_int_t maxits;
    ae_int_t bfgsresetfreq;
    ae_vector x;
    ae_vector fi;
    ae_matrix j;
    double f;
    ae_bool needfij;
    ae_bool xupdated;
    minsqpmeritphasestate meritstate;
    double bigc;
    double meritmu;
    double trustrad;
    double trustradgrowth;
    ae_int_t trustradstagnationcnt;
    ae_int_t fstagnationcnt;
    ae_vector step0x;
    ae_vector stepkx;
    ae_vector backupx;
    ae_vector step0fi;
    ae_vector stepkfi;
    ae_vector backupfi;
    ae_matrix step0j;
    ae_matrix stepkj;
    ae_vector fscales;
    ae_vector tracegamma;
    minsqpsubsolver subsolver;
    xbfgshessian hess;
    minsqptmpmerit tmpmerit;
    ae_int_t repsimplexiterations;
    ae_int_t repsimplexiterations1;
    ae_int_t repsimplexiterations2;
    ae_int_t repsimplexiterations3;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    rcommstate rstate;
} minsqpstate;
#endif
#if defined(AE_COMPILE_LPQPPRESOLVE) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t m;
    ae_int_t n;
    ae_vector rowbegin;
    ae_vector rowend;
    ae_vector idx;
    ae_vector vals;
} dynamiccrs;
typedef struct
{
    niset setn;
    niset setm;
} presolvebuffers;
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    ae_int_t ntrf;
    ae_vector trftype;
    ae_vector idata;
    ae_vector rdata;
    ae_vector idataridx;
    ae_vector rdataridx;
    ae_int_t sourceidx;
    ae_int_t isrc;
    ae_int_t rsrc;
    ae_vector sparseidx0;
    ae_vector sparseval0;
} presolverstack;
typedef struct
{
    ae_int_t newn;
    ae_int_t oldn;
    ae_int_t newm;
    ae_int_t oldm;
    ae_vector rawc;
    ae_vector rawbndl;
    ae_vector rawbndu;
    sparsematrix rawa;
    ae_int_t problemstatus;
    ae_vector lagrangefromresidual;
    ae_vector c;
    ae_vector bndl;
    ae_vector bndu;
    sparsematrix sparsea;
    ae_vector al;
    ae_vector au;
    ae_vector packxperm;
    ae_vector packyperm;
    ae_vector packstatperm;
    ae_vector unpackxperm;
    ae_vector unpackyperm;
    ae_vector unpackstatperm;
    presolverstack trfstack;
    ae_vector s1;
    ae_vector bc1;
    ae_vector x1;
    ae_vector y1;
    ae_vector d;
    presolvebuffers buf;
} presolveinfo;
#endif
#if defined(AE_COMPILE_REVISEDDUALSIMPLEX) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    double pivottol;
    double perturbmag;
    ae_int_t maxtrfage;
    ae_int_t trftype;
    ae_int_t ratiotest;
    ae_int_t pricing;
    ae_int_t shifting;
    double xtolabs;
    double xtolrelabs;
    double dtolabs;
} dualsimplexsettings;
typedef struct
{
    ae_int_t n;
    ae_int_t k;
    ae_vector idx;
    ae_vector vals;
    ae_vector dense;
} dssvector;
typedef struct
{
    ae_int_t ns;
    ae_int_t m;
    ae_vector idx;
    ae_vector nidx;
    ae_vector isbasic;
    ae_int_t trftype;
    ae_bool isvalidtrf;
    ae_int_t trfage;
    ae_matrix denselu;
    sparsematrix sparsel;
    sparsematrix sparseu;
    sparsematrix sparseut;
    ae_vector rowpermbwd;
    ae_vector colpermbwd;
    ae_vector densepfieta;
    ae_vector densemu;
    ae_vector rk;
    ae_vector dk;
    ae_vector dseweights;
    ae_bool dsevalid;
    double eminu;
    ae_int_t statfact;
    ae_int_t statupdt;
    double statoffdiag;
    ae_vector wtmp0;
    ae_vector wtmp1;
    ae_vector wtmp2;
    ae_vector nrs;
    ae_vector tcinvidx;
    ae_matrix denselu2;
    ae_vector densep2;
    ae_vector densep2c;
    sparsematrix sparselu1;
    sparsematrix sparselu2;
    sluv2buffer lubuf2;
    ae_vector tmpi;
    ae_vector utmp0;
    ae_vector utmpi;
    sparsematrix sparseludbg;
} dualsimplexbasis;
typedef struct
{
    ae_int_t ns;
    ae_int_t m;
    ae_vector rawc;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector bndt;
    ae_vector xa;
    ae_vector d;
    ae_int_t state;
    ae_vector xb;
    ae_vector bndlb;
    ae_vector bndub;
    ae_vector bndtb;
    ae_vector bndtollb;
    ae_vector bndtolub;
    ae_vector effc;
} dualsimplexsubproblem;
typedef struct
{
    ae_vector rowscales;
    ae_vector rawbndl;
    ae_vector rawbndu;
    ae_int_t ns;
    ae_int_t m;
    sparsematrix a;
    sparsematrix at;
    dualsimplexbasis basis;
    dualsimplexsubproblem primary;
    dualsimplexsubproblem phase1;
    dualsimplexsubproblem phase3;
    ae_vector repx;
    ae_vector replagbc;
    ae_vector replaglc;
    ae_vector repstats;
    ae_int_t repterminationtype;
    ae_int_t repiterationscount;
    ae_int_t repiterationscount1;
    ae_int_t repiterationscount2;
    ae_int_t repiterationscount3;
    ae_int_t repphase1time;
    ae_int_t repphase2time;
    ae_int_t repphase3time;
    ae_int_t repdualpricingtime;
    ae_int_t repdualbtrantime;
    ae_int_t repdualpivotrowtime;
    ae_int_t repdualratiotesttime;
    ae_int_t repdualftrantime;
    ae_int_t repdualupdatesteptime;
    double repfillpivotrow;
    ae_int_t repfillpivotrowcnt;
    double repfillrhor;
    ae_int_t repfillrhorcnt;
    double repfilldensemu;
    ae_int_t repfilldensemucnt;
    ae_bool dotrace;
    ae_bool dodetailedtrace;
    ae_bool dotimers;
    ae_vector btrantmp0;
    ae_vector btrantmp1;
    ae_vector btrantmp2;
    ae_vector ftrantmp0;
    ae_vector ftrantmp1;
    ae_vector possibleflips;
    ae_int_t possibleflipscnt;
    ae_vector dfctmp0;
    ae_vector dfctmp1;
    ae_vector dfctmp2;
    ae_vector ustmpi;
    apbuffers xydsbuf;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector tmp2;
    dssvector alphar;
    dssvector rhor;
    ae_vector tau;
    ae_vector alphaq;
    ae_vector alphaqim;
    ae_vector eligiblealphar;
    ae_vector harrisset;
} dualsimplexstate;
#endif
#if defined(AE_COMPILE_MINLP) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t algokind;
    double ipmlambda;
    ae_vector s;
    ae_vector c;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t m;
    sparsematrix a;
    ae_vector al;
    ae_vector au;
    ae_vector xs;
    ae_vector lagbc;
    ae_vector laglc;
    ae_vector cs;
    double repf;
    double repprimalerror;
    double repdualerror;
    double repslackerror;
    ae_int_t repiterationscount;
    ae_int_t repterminationtype;
    ae_int_t repn;
    ae_int_t repm;
    double dsseps;
    double ipmeps;
    dualsimplexstate dss;
    vipmstate ipm;
    ae_vector adddtmpi;
    ae_vector adddtmpr;
    ae_vector tmpax;
    ae_vector tmpg;
    presolveinfo presolver;
    ae_vector zeroorigin;
    ae_vector units;
    sparsematrix ipmquadratic;
} minlpstate;
typedef struct
{
    double f;
    ae_vector lagbc;
    ae_vector laglc;
    ae_vector y;
    ae_vector stats;
    double primalerror;
    double dualerror;
    double slackerror;
    ae_int_t iterationscount;
    ae_int_t terminationtype;
} minlpreport;
#endif
#if defined(AE_COMPILE_NLCSLP) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    presolveinfo presolver;
    dualsimplexstate dss;
    dualsimplexsettings dsssettings;
    dualsimplexbasis lastbasis;
    ae_bool basispresent;
    ae_matrix curd;
    ae_int_t curdcnt;
    ae_vector curb;
    ae_vector curbndl;
    ae_vector curbndu;
    ae_vector cural;
    ae_vector curau;
    sparsematrix sparserawlc;
    sparsematrix sparseefflc;
    xbfgshessian hess;
    ae_matrix curhd;
    ae_matrix densedummy;
    sparsematrix sparsedummy;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_vector sk;
    ae_vector yk;
    ae_vector xs;
    ae_vector laglc;
    ae_vector lagbc;
    ae_vector cs;
} minslpsubsolver;
typedef struct
{
    ae_vector sclagtmp0;
    ae_vector sclagtmp1;
} minslptmplagrangian;
typedef struct
{
    ae_vector mftmp0;
} minslptmpmerit;
typedef struct
{
    ae_bool usecorrection;
    ae_vector d;
    ae_vector dx;
    ae_vector stepkxc;
    ae_vector stepkxn;
    ae_vector stepkfic;
    ae_vector stepkfin;
    ae_matrix stepkjc;
    ae_matrix stepkjn;
    ae_vector dummylagmult;
    minslptmpmerit tmpmerit;
    rcommstate rphase13state;
} minslpphase13state;
typedef struct
{
    ae_vector stepkxn;
    ae_vector stepkxc;
    ae_vector stepkfin;
    ae_vector stepkfic;
    ae_matrix stepkjn;
    ae_matrix stepkjc;
    ae_vector stepklaggrad;
    ae_vector stepknlaggrad;
    ae_vector stepknlagmult;
    ae_vector meritlagmult;
    minslptmplagrangian tmplagrangianfg;
    double lastlcerr;
    ae_int_t lastlcidx;
    double lastnlcerr;
    ae_int_t lastnlcidx;
    ae_vector tmp0;
    ae_vector d;
    linminstate mcstate;
    minslptmpmerit tmpmerit;
    rcommstate rphase2state;
} minslpphase2state;
typedef struct
{
    ae_int_t n;
    ae_int_t nec;
    ae_int_t nic;
    ae_int_t nlec;
    ae_int_t nlic;
    ae_vector s;
    ae_matrix scaledcleic;
    ae_vector lcsrcidx;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    double epsx;
    ae_int_t maxits;
    ae_vector x;
    ae_vector fi;
    ae_matrix j;
    double f;
    ae_bool needfij;
    ae_bool xupdated;
    minslpphase13state state13;
    minslpphase2state state2;
    double trustrad;
    double bigc;
    ae_int_t lpfailurecnt;
    ae_int_t fstagnationcnt;
    ae_vector step0x;
    ae_vector stepkx;
    ae_vector backupx;
    ae_vector step0fi;
    ae_vector stepkfi;
    ae_vector backupfi;
    ae_matrix step0j;
    ae_matrix stepkj;
    ae_matrix backupj;
    ae_vector meritlagmult;
    ae_vector dummylagmult;
    ae_vector fscales;
    ae_vector meritfunctionhistory;
    ae_vector maxlaghistory;
    ae_int_t historylen;
    minslpsubsolver subsolver;
    minslptmpmerit tmpmerit;
    ae_int_t repsimplexiterations;
    ae_int_t repsimplexiterations1;
    ae_int_t repsimplexiterations2;
    ae_int_t repsimplexiterations3;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    rcommstate rstate;
} minslpstate;
#endif
#if defined(AE_COMPILE_MINNLC) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    double stabilizingpoint;
    double initialinequalitymultiplier;
    ae_int_t solvertype;
    ae_int_t prectype;
    ae_int_t updatefreq;
    double rho;
    ae_int_t n;
    double epsx;
    ae_int_t maxits;
    ae_int_t aulitscnt;
    ae_bool xrep;
    double stpmax;
    double diffstep;
    double teststep;
    ae_vector s;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_int_t nec;
    ae_int_t nic;
    ae_matrix cleic;
    ae_vector lcsrcidx;
    ae_int_t ng;
    ae_int_t nh;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_matrix j;
    ae_bool needfij;
    ae_bool needfi;
    ae_bool xupdated;
    rcommstate rstate;
    rcommstate rstateaul;
    rcommstate rstateslp;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    ae_matrix scaledcleic;
    ae_vector xc;
    ae_vector xstart;
    ae_vector xbase;
    ae_vector fbase;
    ae_vector dfbase;
    ae_vector fm2;
    ae_vector fm1;
    ae_vector fp1;
    ae_vector fp2;
    ae_vector dfm1;
    ae_vector dfp1;
    ae_vector bufd;
    ae_vector bufc;
    ae_vector tmp0;
    ae_matrix bufw;
    ae_matrix bufz;
    ae_vector xk;
    ae_vector xk1;
    ae_vector gk;
    ae_vector gk1;
    double gammak;
    ae_bool xkpresent;
    minlbfgsstate auloptimizer;
    minlbfgsreport aulreport;
    ae_vector nubc;
    ae_vector nulc;
    ae_vector nunlc;
    ae_bool userterminationneeded;
    minslpstate slpsolverstate;
    minsqpstate sqpsolverstate;
    ae_int_t smoothnessguardlevel;
    smoothnessmonitor smonitor;
    ae_vector lastscaleused;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    ae_int_t repdbgphase0its;
} minnlcstate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t terminationtype;
    double bcerr;
    ae_int_t bcidx;
    double lcerr;
    ae_int_t lcidx;
    double nlcerr;
    ae_int_t nlcidx;
    ae_int_t dbgphase0its;
} minnlcreport;
#endif
#if defined(AE_COMPILE_MONBI) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    ae_vector xstart;
    ae_int_t frontsize;
    ae_bool polishsolutions;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t ksparse;
    ae_int_t kdense;
    ae_matrix densec;
    sparsematrix sparsec;
    ae_vector cl;
    ae_vector cu;
    ae_int_t nnlc;
    ae_vector nl;
    ae_vector nu;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_matrix j;
    ae_bool needfij;
    ae_bool xupdated;
    rcommstate rstate;
    ae_bool userrequestedtermination;
    ae_matrix repparetofront;
    ae_int_t repfrontsize;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    minnlcstate nlcsolver;
    minnlcreport nlcrep;
    ae_vector tmpzero;
    ae_vector tmpone;
    ae_vector tmp0;
    ae_matrix olddensec;
    ae_vector olddensect;
    ae_int_t olddensek;
    ae_vector nlcidx;
    ae_vector nlcmul;
    ae_vector nlcadd;
    ae_int_t nlcnlec;
    ae_int_t nlcnlic;
    ae_vector fideal;
    ae_matrix payoff;
    ae_vector beta;
    ae_vector delta;
    ae_vector subproblemstart;
    hqrndstate rs;
    ae_vector bndlx;
    ae_vector bndux;
    ae_matrix olddensecx;
    ae_vector x1;
    ae_vector x2;
    ae_vector fix1;
} nbistate;
#endif
#if defined(AE_COMPILE_MINMO) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_int_t m;
    double diffstep;
    ae_int_t solvertype;
    double epsx;
    ae_int_t maxits;
    ae_vector s;
    ae_bool xrep;
    ae_vector xstart;
    ae_int_t frontsize;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_int_t msparse;
    ae_int_t mdense;
    ae_matrix densec;
    sparsematrix sparsec;
    ae_vector cl;
    ae_vector cu;
    ae_int_t nnlc;
    ae_vector nl;
    ae_vector nu;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_matrix j;
    ae_bool needfij;
    ae_bool needfi;
    ae_bool xupdated;
    rcommstate rstate;
    nbistate nbi;
    ae_int_t repfrontsize;
    ae_matrix repparetofront;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    double repbcerr;
    ae_int_t repbcidx;
    double replcerr;
    ae_int_t replcidx;
    double repnlcerr;
    ae_int_t repnlcidx;
    ae_vector dummyr1;
    ae_matrix dummyr2;
    sparsematrix dummysparse;
    ae_vector xbase;
    ae_vector fm2;
    ae_vector fm1;
    ae_vector fp1;
    ae_vector fp2;
} minmostate;
typedef struct
{
    ae_int_t inneriterationscount;
    ae_int_t outeriterationscount;
    ae_int_t nfev;
    ae_int_t terminationtype;
    double bcerr;
    ae_int_t bcidx;
    double lcerr;
    ae_int_t lcidx;
    double nlcerr;
    ae_int_t nlcidx;
} minmoreport;
#endif
#if defined(AE_COMPILE_MINNS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    double fc;
    double fn;
    ae_vector xc;
    ae_vector xn;
    ae_vector x0;
    ae_vector gc;
    ae_vector d;
    ae_matrix uh;
    ae_matrix ch;
    ae_matrix rk;
    ae_vector invutc;
    ae_vector tmp0;
    ae_vector tmpidx;
    ae_vector tmpd;
    ae_vector tmpc;
    ae_vector tmplambdas;
    ae_matrix tmpc2;
    ae_vector tmpb;
    snnlssolver nnls;
} minnsqp;
typedef struct
{
    ae_int_t solvertype;
    ae_int_t n;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    double diffstep;
    ae_vector s;
    ae_vector bndl;
    ae_vector bndu;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_int_t nec;
    ae_int_t nic;
    ae_matrix cleic;
    ae_int_t ng;
    ae_int_t nh;
    ae_vector x;
    double f;
    ae_vector fi;
    ae_matrix j;
    ae_bool needfij;
    ae_bool needfi;
    ae_bool xupdated;
    rcommstate rstate;
    rcommstate rstateags;
    hqrndstate agsrs;
    double agsradius;
    ae_int_t agssamplesize;
    double agsraddecay;
    double agsalphadecay;
    double agsdecrease;
    double agsinitstp;
    double agsstattold;
    double agsshortstpabs;
    double agsshortstprel;
    double agsshortf;
    ae_int_t agsshortlimit;
    double agsrhononlinear;
    ae_int_t agsminupdate;
    ae_int_t agsmaxraddecays;
    ae_int_t agsmaxbacktrack;
    ae_int_t agsmaxbacktracknonfull;
    double agspenaltylevel;
    double agspenaltyincrease;
    ae_vector xstart;
    ae_vector xc;
    ae_vector xn;
    ae_vector rawg;
    ae_vector meritg;
    double rawf;
    double meritf;
    ae_vector d;
    ae_vector colmax;
    ae_vector diagh;
    ae_vector signmin;
    ae_vector signmax;
    ae_bool userterminationneeded;
    ae_vector scaledbndl;
    ae_vector scaledbndu;
    ae_matrix scaledcleic;
    double rholinear;
    ae_matrix samplex;
    ae_matrix samplegm;
    ae_matrix samplegmbc;
    ae_vector samplef;
    minnsqp nsqp;
    ae_vector tmp0;
    ae_vector tmp1;
    ae_matrix tmp2;
    ae_vector tmp3;
    ae_vector xbase;
    ae_vector fbase;
    ae_vector fp;
    ae_vector fm;
    ae_vector xscaled;
    ae_int_t repinneriterationscount;
    ae_int_t repouteriterationscount;
    ae_int_t repnfev;
    ae_int_t repvaridx;
    ae_int_t repfuncidx;
    ae_int_t repterminationtype;
    double replcerr;
    double repnlcerr;
    ae_int_t dbgncholesky;
} minnsstate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    double cerr;
    double lcerr;
    double nlcerr;
    ae_int_t terminationtype;
    ae_int_t varidx;
    ae_int_t funcidx;
} minnsreport;
#endif
#if defined(AE_COMPILE_MINCOMP) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    double stpmax;
    ae_int_t cgtype;
    ae_int_t k;
    ae_int_t nfev;
    ae_int_t mcstage;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t curalgo;
    ae_int_t acount;
    double mu;
    double finit;
    double dginit;
    ae_vector ak;
    ae_vector xk;
    ae_vector dk;
    ae_vector an;
    ae_vector xn;
    ae_vector dn;
    ae_vector d;
    double fold;
    double stp;
    ae_vector work;
    ae_vector yk;
    ae_vector gc;
    double laststep;
    ae_vector x;
    double f;
    ae_vector g;
    ae_bool needfg;
    ae_bool xupdated;
    rcommstate rstate;
    ae_int_t repiterationscount;
    ae_int_t repnfev;
    ae_int_t repterminationtype;
    ae_int_t debugrestartscount;
    linminstate lstate;
    double betahs;
    double betady;
} minasastate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t terminationtype;
    ae_int_t activeconstraints;
} minasareport;
#endif
#if defined(AE_COMPILE_MINBC) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t nmain;
    double epsg;
    double epsf;
    double epsx;
    ae_int_t maxits;
    ae_bool xrep;
    double stpmax;
    double diffstep;
    ae_vector s;
    ae_int_t prectype;
    ae_vector diagh;
    ae_vector x;
    double f;
    ae_vector g;
    ae_bool needf;
    ae_bool needfg;
    ae_bool xupdated;
    ae_bool userterminationneeded;
    rcommstate rstate;
    ae_vector xc;
    ae_vector ugc;
    ae_vector cgc;
    ae_vector xn;
    ae_vector ugn;
    ae_vector cgn;
    ae_vector xp;
    double fc;
    double fn;
    double fp;
    ae_vector d;
    double lastscaledgoodstep;
    ae_vector hasbndl;
    ae_vector hasbndu;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t repiterationscount;
    ae_int_t repnfev;
    ae_int_t repvaridx;
    ae_int_t repterminationtype;
    ae_vector xstart;
    double fbase;
    double fm2;
    double fm1;
    double fp1;
    double fp2;
    double xm1;
    double xp1;
    double gm1;
    double gp1;
    ae_vector tmpprec;
    ae_vector tmp0;
    ae_int_t nfev;
    ae_int_t mcstage;
    double stp;
    double curstpmax;
    ae_vector work;
    linminstate lstate;
    double trimthreshold;
    ae_int_t nonmonotoniccnt;
    ae_matrix bufyk;
    ae_matrix bufsk;
    ae_vector bufrho;
    ae_vector buftheta;
    ae_int_t bufsize;
    double teststep;
    ae_int_t smoothnessguardlevel;
    smoothnessmonitor smonitor;
    ae_vector lastscaleused;
    ae_vector invs;
} minbcstate;
typedef struct
{
    ae_int_t iterationscount;
    ae_int_t nfev;
    ae_int_t varidx;
    ae_int_t terminationtype;
} minbcreport;
#endif
#if defined(AE_COMPILE_OPTS) || !defined(AE_PARTIAL_BUILD)
typedef struct
{
    ae_int_t n;
    ae_bool hasknowntarget;
    double targetf;
    ae_vector s;
    ae_vector c;
    ae_vector bndl;
    ae_vector bndu;
    ae_int_t m;
    sparsematrix a;
    ae_vector al;
    ae_vector au;
} lptestproblem;
#endif

}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS C++ INTERFACE
//
/////////////////////////////////////////////////////////////////////////
namespace alglib
{

#if defined(AE_COMPILE_OPTGUARDAPI) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This structure is used to store  OptGuard  report,  i.e.  report  on   the
properties of the nonlinear function being optimized with ALGLIB.

After you tell your optimizer to activate OptGuard  this technology starts
to silently monitor function values and gradients/Jacobians  being  passed
all around during your optimization session. Depending on specific set  of
checks enabled OptGuard may perform additional function evaluations  (say,
about 3*N evaluations if you want to check analytic gradient for errors).

Upon discovering that something strange happens  (function  values  and/or
gradient components change too sharply and/or unexpectedly) OptGuard  sets
one of the "suspicion  flags" (without interrupting optimization session).
After optimization is done, you can examine OptGuard report.

Following report fields can be set:
* nonc0suspected
* nonc1suspected
* badgradsuspected


=== WHAT CAN BE DETECTED WITH OptGuard INTEGRITY CHECKER =================

Following  types  of  errors  in your target function (constraints) can be
caught:
a) discontinuous functions ("non-C0" part of the report)
b) functions with discontinuous derivative ("non-C1" part of the report)
c) errors in the analytic gradient provided by user

These types of errors result in optimizer  stopping  well  before reaching
solution (most often - right after encountering discontinuity).

Type A errors are usually  coding  errors  during  implementation  of  the
target function. Most "normal" problems involve continuous functions,  and
anyway you can't reliably optimize discontinuous function.

Type B errors are either coding errors or (in case code itself is correct)
evidence of the fact  that  your  problem  is  an  "incorrect"  one.  Most
optimizers (except for ones provided by MINNS subpackage) do  not  support
nonsmooth problems.

Type C errors are coding errors which often prevent optimizer from  making
even one step  or result in optimizing stopping  too  early,  as  soon  as
actual descent direction becomes too different from one suggested by user-
supplied gradient.


=== WHAT IS REPORTED =====================================================

Following set of report fields deals with discontinuous  target functions,
ones not belonging to C0 continuity class:

* nonc0suspected - is a flag which is set upon discovering some indication
  of the discontinuity. If this flag is false, the rest of "non-C0" fields
  should be ignored
* nonc0fidx - is an index of the function (0 for  target  function,  1  or
  higher for nonlinear constraints) which is suspected of being "non-C0"
* nonc0lipshitzc - a Lipchitz constant for a function which was  suspected
  of being non-continuous.
* nonc0test0positive -  set  to  indicate  specific  test  which  detected
  continuity violation (test #0)

Following set of report fields deals with discontinuous gradient/Jacobian,
i.e. with functions violating C1 continuity:

* nonc1suspected - is a flag which is set upon discovering some indication
  of the discontinuity. If this flag is false, the rest of "non-C1" fields
  should be ignored
* nonc1fidx - is an index of the function (0 for  target  function,  1  or
  higher for nonlinear constraints) which is suspected of being "non-C1"
* nonc1lipshitzc - a Lipchitz constant for a function gradient  which  was
  suspected of being non-smooth.
* nonc1test0positive -  set  to  indicate  specific  test  which  detected
  continuity violation (test #0)
* nonc1test1positive -  set  to  indicate  specific  test  which  detected
  continuity violation (test #1)

Following set of report fields deals with errors in the gradient:
* badgradsuspected - is a flad which is set upon discovering an  error  in
  the analytic gradient supplied by user
* badgradfidx - index  of   the  function  with bad gradient (0 for target
  function, 1 or higher for nonlinear constraints)
* badgradvidx - index of the variable
* badgradxbase - location where Jacobian is tested
* following  matrices  store  user-supplied  Jacobian  and  its  numerical
  differentiation version (which is assumed to be  free  from  the  coding
  errors), both of them computed near the initial point:
  * badgraduser, an array[K,N], analytic Jacobian supplied by user
  * badgradnum,  an array[K,N], numeric  Jacobian computed by ALGLIB
  Here K is a total number of  nonlinear  functions  (target  +  nonlinear
  constraints), N is a variable number.
  The  element  of  badgraduser[] with index [badgradfidx,badgradvidx]  is
  assumed to be wrong.

More detailed error log can  be  obtained  from  optimizer  by  explicitly
requesting reports for tests C0.0, C1.0, C1.1.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
class _optguardreport_owner
{
public:
    _optguardreport_owner();
    _optguardreport_owner(const _optguardreport_owner &rhs);
    _optguardreport_owner& operator=(const _optguardreport_owner &rhs);
    virtual ~_optguardreport_owner();
    alglib_impl::optguardreport* c_ptr();
    const alglib_impl::optguardreport* c_ptr() const;
protected:
    alglib_impl::optguardreport *p_struct;
};
class optguardreport : public _optguardreport_owner
{
public:
    optguardreport();
    optguardreport(const optguardreport &rhs);
    optguardreport& operator=(const optguardreport &rhs);
    virtual ~optguardreport();
    ae_bool &nonc0suspected;
    ae_bool &nonc0test0positive;
    ae_int_t &nonc0fidx;
    double &nonc0lipschitzc;
    ae_bool &nonc1suspected;
    ae_bool &nonc1test0positive;
    ae_bool &nonc1test1positive;
    ae_int_t &nonc1fidx;
    double &nonc1lipschitzc;
    ae_bool &badgradsuspected;
    ae_int_t &badgradfidx;
    ae_int_t &badgradvidx;
    real_1d_array badgradxbase;
    real_2d_array badgraduser;
    real_2d_array badgradnum;

};


/*************************************************************************
This  structure  is  used  for  detailed   reporting  about  suspected  C0
continuity violation.

=== WHAT IS TESTED =======================================================

C0 test  studies  function  values (not gradient!)  obtained  during  line
searches and monitors estimate of the Lipschitz  constant.  Sudden  spikes
usually indicate that discontinuity was detected.


=== WHAT IS REPORTED =====================================================

Actually, report retrieval function returns TWO report structures:

* one for most suspicious point found so far (one with highest  change  in
  the function value), so called "strongest" report
* another one for most detailed line search (more function  evaluations  =
  easier to understand what's going on) which triggered  test #0 criteria,
  so called "longest" report

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.
* inneriter, outeriter - inner and outer iteration indexes (can be -1 if no
  iteration information was specified)

You can plot function values stored in stp[]  and  f[]  arrays  and  study
behavior of your function by your own eyes, just  to  be  sure  that  test
correctly reported C1 violation.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
class _optguardnonc0report_owner
{
public:
    _optguardnonc0report_owner();
    _optguardnonc0report_owner(const _optguardnonc0report_owner &rhs);
    _optguardnonc0report_owner& operator=(const _optguardnonc0report_owner &rhs);
    virtual ~_optguardnonc0report_owner();
    alglib_impl::optguardnonc0report* c_ptr();
    const alglib_impl::optguardnonc0report* c_ptr() const;
protected:
    alglib_impl::optguardnonc0report *p_struct;
};
class optguardnonc0report : public _optguardnonc0report_owner
{
public:
    optguardnonc0report();
    optguardnonc0report(const optguardnonc0report &rhs);
    optguardnonc0report& operator=(const optguardnonc0report &rhs);
    virtual ~optguardnonc0report();
    ae_bool &positive;
    ae_int_t &fidx;
    real_1d_array x0;
    real_1d_array d;
    ae_int_t &n;
    real_1d_array stp;
    real_1d_array f;
    ae_int_t &cnt;
    ae_int_t &stpidxa;
    ae_int_t &stpidxb;
    ae_int_t &inneriter;
    ae_int_t &outeriter;

};


/*************************************************************************
This  structure  is  used  for  detailed   reporting  about  suspected  C1
continuity violation as flagged by C1 test #0 (OptGuard  has several tests
for C1 continuity, this report is used by #0).

=== WHAT IS TESTED =======================================================

C1 test #0 studies function values (not gradient!)  obtained  during  line
searches and monitors behavior of directional  derivative  estimate.  This
test is less powerful than test #1, but it does  not  depend  on  gradient
values  and  thus  it  is  more  robust  against  artifacts  introduced by
numerical differentiation.


=== WHAT IS REPORTED =====================================================

Actually, report retrieval function returns TWO report structures:

* one for most suspicious point found so far (one with highest  change  in
  the directional derivative), so called "strongest" report
* another one for most detailed line search (more function  evaluations  =
  easier to understand what's going on) which triggered  test #0 criteria,
  so called "longest" report

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.
* inneriter, outeriter - inner and outer iteration indexes (can be -1 if no
  iteration information was specified)

You can plot function values stored in stp[]  and  f[]  arrays  and  study
behavior of your function by your own eyes, just  to  be  sure  that  test
correctly reported C1 violation.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
class _optguardnonc1test0report_owner
{
public:
    _optguardnonc1test0report_owner();
    _optguardnonc1test0report_owner(const _optguardnonc1test0report_owner &rhs);
    _optguardnonc1test0report_owner& operator=(const _optguardnonc1test0report_owner &rhs);
    virtual ~_optguardnonc1test0report_owner();
    alglib_impl::optguardnonc1test0report* c_ptr();
    const alglib_impl::optguardnonc1test0report* c_ptr() const;
protected:
    alglib_impl::optguardnonc1test0report *p_struct;
};
class optguardnonc1test0report : public _optguardnonc1test0report_owner
{
public:
    optguardnonc1test0report();
    optguardnonc1test0report(const optguardnonc1test0report &rhs);
    optguardnonc1test0report& operator=(const optguardnonc1test0report &rhs);
    virtual ~optguardnonc1test0report();
    ae_bool &positive;
    ae_int_t &fidx;
    real_1d_array x0;
    real_1d_array d;
    ae_int_t &n;
    real_1d_array stp;
    real_1d_array f;
    ae_int_t &cnt;
    ae_int_t &stpidxa;
    ae_int_t &stpidxb;
    ae_int_t &inneriter;
    ae_int_t &outeriter;

};


/*************************************************************************
This  structure  is  used  for  detailed   reporting  about  suspected  C1
continuity violation as flagged by C1 test #1 (OptGuard  has several tests
for C1 continuity, this report is used by #1).

=== WHAT IS TESTED =======================================================

C1 test #1 studies individual  components  of  the  gradient  as  recorded
during line searches. Upon discovering discontinuity in the gradient  this
test records specific component which was suspected (or  one  with  highest
indication of discontinuity if multiple components are suspected).

When precise analytic gradient is provided this test is more powerful than
test #0  which  works  with  function  values  and  ignores  user-provided
gradient.  However,  test  #0  becomes  more   powerful   when   numerical
differentiation is employed (in such cases test #1 detects  higher  levels
of numerical noise and becomes too conservative).

This test also tells specific components of the gradient which violate  C1
continuity, which makes it more informative than #0, which just tells that
continuity is violated.


=== WHAT IS REPORTED =====================================================

Actually, report retrieval function returns TWO report structures:

* one for most suspicious point found so far (one with highest  change  in
  the directional derivative), so called "strongest" report
* another one for most detailed line search (more function  evaluations  =
  easier to understand what's going on) which triggered  test #1 criteria,
  so called "longest" report

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
* vidx - is an index of the variable in [0,N) with nonsmooth derivative
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], g[] - arrays of length CNT which store step lengths and  gradient
  values at these points; g[i] is evaluated in  x0+stp[i]*d  and  contains
  vidx-th component of the gradient.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.
* inneriter, outeriter - inner and outer iteration indexes (can be -1 if  no
  iteration information was specified)

You can plot function values stored in stp[]  and  g[]  arrays  and  study
behavior of your function by your own eyes, just  to  be  sure  that  test
correctly reported C1 violation.

  -- ALGLIB --
     Copyright 19.11.2018 by Bochkanov Sergey
*************************************************************************/
class _optguardnonc1test1report_owner
{
public:
    _optguardnonc1test1report_owner();
    _optguardnonc1test1report_owner(const _optguardnonc1test1report_owner &rhs);
    _optguardnonc1test1report_owner& operator=(const _optguardnonc1test1report_owner &rhs);
    virtual ~_optguardnonc1test1report_owner();
    alglib_impl::optguardnonc1test1report* c_ptr();
    const alglib_impl::optguardnonc1test1report* c_ptr() const;
protected:
    alglib_impl::optguardnonc1test1report *p_struct;
};
class optguardnonc1test1report : public _optguardnonc1test1report_owner
{
public:
    optguardnonc1test1report();
    optguardnonc1test1report(const optguardnonc1test1report &rhs);
    optguardnonc1test1report& operator=(const optguardnonc1test1report &rhs);
    virtual ~optguardnonc1test1report();
    ae_bool &positive;
    ae_int_t &fidx;
    ae_int_t &vidx;
    real_1d_array x0;
    real_1d_array d;
    ae_int_t &n;
    real_1d_array stp;
    real_1d_array g;
    ae_int_t &cnt;
    ae_int_t &stpidxa;
    ae_int_t &stpidxb;
    ae_int_t &inneriter;
    ae_int_t &outeriter;

};
#endif

#if defined(AE_COMPILE_OPTSERV) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINLBFGS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************

*************************************************************************/
class _minlbfgsstate_owner
{
public:
    _minlbfgsstate_owner();
    _minlbfgsstate_owner(const _minlbfgsstate_owner &rhs);
    _minlbfgsstate_owner& operator=(const _minlbfgsstate_owner &rhs);
    virtual ~_minlbfgsstate_owner();
    alglib_impl::minlbfgsstate* c_ptr();
    const alglib_impl::minlbfgsstate* c_ptr() const;
protected:
    alglib_impl::minlbfgsstate *p_struct;
};
class minlbfgsstate : public _minlbfgsstate_owner
{
public:
    minlbfgsstate();
    minlbfgsstate(const minlbfgsstate &rhs);
    minlbfgsstate& operator=(const minlbfgsstate &rhs);
    virtual ~minlbfgsstate();
    ae_bool &needf;
    ae_bool &needfg;
    ae_bool &xupdated;
    double &f;
    real_1d_array g;
    real_1d_array x;

};


/*************************************************************************
This structure stores optimization report:
* IterationsCount           total number of inner iterations
* NFEV                      number of gradient evaluations
* TerminationType           termination type (see below)

TERMINATION CODES

TerminationType field contains completion code, which can be:
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signalled.
   1    relative function improvement is no more than EpsF.
   2    relative step is no more than EpsX.
   4    gradient norm is no more than EpsG
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    terminated    by  user  who  called  minlbfgsrequesttermination().
        X contains point which was   "current accepted"  when  termination
        request was submitted.

Other fields of this structure are not documented and should not be used!
*************************************************************************/
class _minlbfgsreport_owner
{
public:
    _minlbfgsreport_owner();
    _minlbfgsreport_owner(const _minlbfgsreport_owner &rhs);
    _minlbfgsreport_owner& operator=(const _minlbfgsreport_owner &rhs);
    virtual ~_minlbfgsreport_owner();
    alglib_impl::minlbfgsreport* c_ptr();
    const alglib_impl::minlbfgsreport* c_ptr() const;
protected:
    alglib_impl::minlbfgsreport *p_struct;
};
class minlbfgsreport : public _minlbfgsreport_owner
{
public:
    minlbfgsreport();
    minlbfgsreport(const minlbfgsreport &rhs);
    minlbfgsreport& operator=(const minlbfgsreport &rhs);
    virtual ~minlbfgsreport();
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    ae_int_t &terminationtype;

};
#endif

#if defined(AE_COMPILE_CQMODELS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_LPQPSERV) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_SNNLS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_SACTIVESETS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_QQPSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_QPDENSEAULSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINBLEIC) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinBLEIC subpackage to work with this
object
*************************************************************************/
class _minbleicstate_owner
{
public:
    _minbleicstate_owner();
    _minbleicstate_owner(const _minbleicstate_owner &rhs);
    _minbleicstate_owner& operator=(const _minbleicstate_owner &rhs);
    virtual ~_minbleicstate_owner();
    alglib_impl::minbleicstate* c_ptr();
    const alglib_impl::minbleicstate* c_ptr() const;
protected:
    alglib_impl::minbleicstate *p_struct;
};
class minbleicstate : public _minbleicstate_owner
{
public:
    minbleicstate();
    minbleicstate(const minbleicstate &rhs);
    minbleicstate& operator=(const minbleicstate &rhs);
    virtual ~minbleicstate();
    ae_bool &needf;
    ae_bool &needfg;
    ae_bool &xupdated;
    double &f;
    real_1d_array g;
    real_1d_array x;

};


/*************************************************************************
This structure stores optimization report:
* IterationsCount           number of iterations
* NFEV                      number of gradient evaluations
* TerminationType           termination type (see below)

TERMINATION CODES

TerminationType field contains completion code, which can be:
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signalled.
  -3    inconsistent constraints. Feasible point is
        either nonexistent or too hard to find. Try to
        restart optimizer with better initial approximation
   1    relative function improvement is no more than EpsF.
   2    relative step is no more than EpsX.
   4    gradient norm is no more than EpsG
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    terminated by user who called minbleicrequesttermination(). X contains
        point which was "current accepted" when  termination  request  was
        submitted.

ADDITIONAL FIELDS

There are additional fields which can be used for debugging:
* DebugEqErr                error in the equality constraints (2-norm)
* DebugFS                   f, calculated at projection of initial point
                            to the feasible set
* DebugFF                   f, calculated at the final point
* DebugDX                   |X_start-X_final|
*************************************************************************/
class _minbleicreport_owner
{
public:
    _minbleicreport_owner();
    _minbleicreport_owner(const _minbleicreport_owner &rhs);
    _minbleicreport_owner& operator=(const _minbleicreport_owner &rhs);
    virtual ~_minbleicreport_owner();
    alglib_impl::minbleicreport* c_ptr();
    const alglib_impl::minbleicreport* c_ptr() const;
protected:
    alglib_impl::minbleicreport *p_struct;
};
class minbleicreport : public _minbleicreport_owner
{
public:
    minbleicreport();
    minbleicreport(const minbleicreport &rhs);
    minbleicreport& operator=(const minbleicreport &rhs);
    virtual ~minbleicreport();
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    ae_int_t &varidx;
    ae_int_t &terminationtype;
    double &debugeqerr;
    double &debugfs;
    double &debugff;
    double &debugdx;
    ae_int_t &debugfeasqpits;
    ae_int_t &debugfeasgpaits;
    ae_int_t &inneriterationscount;
    ae_int_t &outeriterationscount;

};
#endif

#if defined(AE_COMPILE_QPBLEICSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_VIPMSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINQP) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinQP subpackage to work with this
object
*************************************************************************/
class _minqpstate_owner
{
public:
    _minqpstate_owner();
    _minqpstate_owner(const _minqpstate_owner &rhs);
    _minqpstate_owner& operator=(const _minqpstate_owner &rhs);
    virtual ~_minqpstate_owner();
    alglib_impl::minqpstate* c_ptr();
    const alglib_impl::minqpstate* c_ptr() const;
protected:
    alglib_impl::minqpstate *p_struct;
};
class minqpstate : public _minqpstate_owner
{
public:
    minqpstate();
    minqpstate(const minqpstate &rhs);
    minqpstate& operator=(const minqpstate &rhs);
    virtual ~minqpstate();

};


/*************************************************************************
This structure stores optimization report:
* InnerIterationsCount      number of inner iterations
* OuterIterationsCount      number of outer iterations
* NCholesky                 number of Cholesky decomposition
* NMV                       number of matrix-vector products
                            (only products calculated as part of iterative
                            process are counted)
* TerminationType           completion code (see below)
* LagBC                     Lagrange multipliers for box constraints,
                            array[N], not filled by QP-BLEIC solver
* LagLC                     Lagrange multipliers for linear constraints,
                            array[MSparse+MDense], ignored by QP-BLEIC solver

=== COMPLETION CODES =====================================================

Completion codes:
* -9    failure of the automatic scale evaluation:  one  of  the  diagonal
        elements of the quadratic term is non-positive.  Specify  variable
        scales manually!
* -5    inappropriate solver was used:
        * QuickQP solver for problem with general linear constraints (dense/sparse)
* -4    BLEIC-QP or QuickQP solver found unconstrained direction
        of negative curvature (function is unbounded from
        below  even  under  constraints),  no  meaningful
        minimum can be found.
* -3    inconsistent constraints (or, maybe, feasible point is
        too hard to find). If you are sure that constraints are feasible,
        try to restart optimizer with better initial approximation.
* -2    IPM solver has difficulty finding primal/dual feasible point.
        It is likely that the problem is either infeasible or unbounded,
        but it is difficult to determine exact reason for termination.
        X contains best point found so far.
*  1..4 successful completion
*  5    MaxIts steps was taken
*  7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.

=== LAGRANGE MULTIPLIERS =================================================

Some  optimizers  report  values of  Lagrange  multipliers  on  successful
completion (positive completion code):
* DENSE-IPM-QP and SPARSE-IPM-QP return very precise Lagrange  multipliers
  as determined during solution process.
* DENSE-AUL-QP returns approximate Lagrange multipliers  (which  are  very
  close to "true"  Lagrange  multipliers  except  for  overconstrained  or
  degenerate problems)

Two arrays of multipliers are returned:
* LagBC is array[N] which is loaded with multipliers from box constraints;
  LagBC[i]>0 means that I-th constraint is at the  upper bound, LagBC[I]<0
  means that I-th constraint is at the lower bound, LagBC[I]=0 means  that
  I-th box constraint is inactive.
* LagLC is array[MSparse+MDense] which is  loaded  with  multipliers  from
  general  linear  constraints  (former  MSparse  elements  corresponds to
  sparse part of the constraint matrix, latter MDense are  for  the  dense
  constraints, as was specified by user).
  LagLC[i]>0 means that I-th constraint at  the  upper  bound,  LagLC[i]<0
  means that I-th constraint is at the lower bound, LagLC[i]=0 means  that
  I-th linear constraint is inactive.

On failure (or when optimizer does not support Lagrange multipliers) these
arrays are zero-filled.

It is expected that at solution the dual feasibility condition holds:

    C+H*(Xs-X0) + SUM(Ei*LagBC[i],i=0..n-1) + SUM(Ai*LagLC[i],i=0..m-1) ~ 0

where
* C is a linear term
* H is a quadratic term
* Xs is a solution, and X0 is an origin term (zero by default)
* Ei is a vector with 1.0 at position I and 0 in other positions
* Ai is an I-th row of linear constraint matrix

NOTE: methods  from  IPM  family  may  also  return  meaningful   Lagrange
      multipliers  on  completion   with   code   -2   (infeasibility   or
      unboundedness  detected).
*************************************************************************/
class _minqpreport_owner
{
public:
    _minqpreport_owner();
    _minqpreport_owner(const _minqpreport_owner &rhs);
    _minqpreport_owner& operator=(const _minqpreport_owner &rhs);
    virtual ~_minqpreport_owner();
    alglib_impl::minqpreport* c_ptr();
    const alglib_impl::minqpreport* c_ptr() const;
protected:
    alglib_impl::minqpreport *p_struct;
};
class minqpreport : public _minqpreport_owner
{
public:
    minqpreport();
    minqpreport(const minqpreport &rhs);
    minqpreport& operator=(const minqpreport &rhs);
    virtual ~minqpreport();
    ae_int_t &inneriterationscount;
    ae_int_t &outeriterationscount;
    ae_int_t &nmv;
    ae_int_t &ncholesky;
    ae_int_t &terminationtype;
    real_1d_array lagbc;
    real_1d_array laglc;

};
#endif

#if defined(AE_COMPILE_MINLM) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
Levenberg-Marquardt optimizer.

This structure should be created using one of the MinLMCreate???()
functions. You should not access its fields directly; use ALGLIB functions
to work with it.
*************************************************************************/
class _minlmstate_owner
{
public:
    _minlmstate_owner();
    _minlmstate_owner(const _minlmstate_owner &rhs);
    _minlmstate_owner& operator=(const _minlmstate_owner &rhs);
    virtual ~_minlmstate_owner();
    alglib_impl::minlmstate* c_ptr();
    const alglib_impl::minlmstate* c_ptr() const;
protected:
    alglib_impl::minlmstate *p_struct;
};
class minlmstate : public _minlmstate_owner
{
public:
    minlmstate();
    minlmstate(const minlmstate &rhs);
    minlmstate& operator=(const minlmstate &rhs);
    virtual ~minlmstate();
    ae_bool &needf;
    ae_bool &needfg;
    ae_bool &needfgh;
    ae_bool &needfi;
    ae_bool &needfij;
    ae_bool &xupdated;
    double &f;
    real_1d_array fi;
    real_1d_array g;
    real_2d_array h;
    real_2d_array j;
    real_1d_array x;

};


/*************************************************************************
Optimization report, filled by MinLMResults() function

FIELDS:
* TerminationType, completetion code:
    * -8    optimizer detected NAN/INF values either in the function itself,
            or in its Jacobian
    * -5    inappropriate solver was used:
            * solver created with minlmcreatefgh() used  on  problem  with
              general linear constraints (set with minlmsetlc() call).
    * -3    constraints are inconsistent
    *  2    relative step is no more than EpsX.
    *  5    MaxIts steps was taken
    *  7    stopping conditions are too stringent,
            further improvement is impossible
    *  8    terminated   by  user  who  called  MinLMRequestTermination().
            X contains point which was "current accepted" when termination
            request was submitted.
* IterationsCount, contains iterations count
* NFunc, number of function calculations
* NJac, number of Jacobi matrix calculations
* NGrad, number of gradient calculations
* NHess, number of Hessian calculations
* NCholesky, number of Cholesky decomposition calculations
*************************************************************************/
class _minlmreport_owner
{
public:
    _minlmreport_owner();
    _minlmreport_owner(const _minlmreport_owner &rhs);
    _minlmreport_owner& operator=(const _minlmreport_owner &rhs);
    virtual ~_minlmreport_owner();
    alglib_impl::minlmreport* c_ptr();
    const alglib_impl::minlmreport* c_ptr() const;
protected:
    alglib_impl::minlmreport *p_struct;
};
class minlmreport : public _minlmreport_owner
{
public:
    minlmreport();
    minlmreport(const minlmreport &rhs);
    minlmreport& operator=(const minlmreport &rhs);
    virtual ~minlmreport();
    ae_int_t &iterationscount;
    ae_int_t &terminationtype;
    ae_int_t &nfunc;
    ae_int_t &njac;
    ae_int_t &ngrad;
    ae_int_t &nhess;
    ae_int_t &ncholesky;

};
#endif

#if defined(AE_COMPILE_MINCG) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This object stores state of the nonlinear CG optimizer.

You should use ALGLIB functions to work with this object.
*************************************************************************/
class _mincgstate_owner
{
public:
    _mincgstate_owner();
    _mincgstate_owner(const _mincgstate_owner &rhs);
    _mincgstate_owner& operator=(const _mincgstate_owner &rhs);
    virtual ~_mincgstate_owner();
    alglib_impl::mincgstate* c_ptr();
    const alglib_impl::mincgstate* c_ptr() const;
protected:
    alglib_impl::mincgstate *p_struct;
};
class mincgstate : public _mincgstate_owner
{
public:
    mincgstate();
    mincgstate(const mincgstate &rhs);
    mincgstate& operator=(const mincgstate &rhs);
    virtual ~mincgstate();
    ae_bool &needf;
    ae_bool &needfg;
    ae_bool &xupdated;
    double &f;
    real_1d_array g;
    real_1d_array x;

};


/*************************************************************************
This structure stores optimization report:
* IterationsCount           total number of inner iterations
* NFEV                      number of gradient evaluations
* TerminationType           termination type (see below)

TERMINATION CODES

TerminationType field contains completion code, which can be:
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signalled.
   1    relative function improvement is no more than EpsF.
   2    relative step is no more than EpsX.
   4    gradient norm is no more than EpsG
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    terminated by user who called mincgrequesttermination(). X contains
        point which was "current accepted" when  termination  request  was
        submitted.

Other fields of this structure are not documented and should not be used!
*************************************************************************/
class _mincgreport_owner
{
public:
    _mincgreport_owner();
    _mincgreport_owner(const _mincgreport_owner &rhs);
    _mincgreport_owner& operator=(const _mincgreport_owner &rhs);
    virtual ~_mincgreport_owner();
    alglib_impl::mincgreport* c_ptr();
    const alglib_impl::mincgreport* c_ptr() const;
protected:
    alglib_impl::mincgreport *p_struct;
};
class mincgreport : public _mincgreport_owner
{
public:
    mincgreport();
    mincgreport(const mincgreport &rhs);
    mincgreport& operator=(const mincgreport &rhs);
    virtual ~mincgreport();
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    ae_int_t &terminationtype;

};
#endif

#if defined(AE_COMPILE_NLCSQP) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_LPQPPRESOLVE) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_REVISEDDUALSIMPLEX) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINLP) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This object stores linear solver state.
You should use functions provided by MinLP subpackage to work with this
object
*************************************************************************/
class _minlpstate_owner
{
public:
    _minlpstate_owner();
    _minlpstate_owner(const _minlpstate_owner &rhs);
    _minlpstate_owner& operator=(const _minlpstate_owner &rhs);
    virtual ~_minlpstate_owner();
    alglib_impl::minlpstate* c_ptr();
    const alglib_impl::minlpstate* c_ptr() const;
protected:
    alglib_impl::minlpstate *p_struct;
};
class minlpstate : public _minlpstate_owner
{
public:
    minlpstate();
    minlpstate(const minlpstate &rhs);
    minlpstate& operator=(const minlpstate &rhs);
    virtual ~minlpstate();

};


/*************************************************************************
This structure stores optimization report:
* f                         target function value
* lagbc                     Lagrange coefficients for box constraints
* laglc                     Lagrange coefficients for linear constraints
* y                         dual variables
* stats                     array[N+M], statuses of box (N) and linear (M)
                            constraints. This array is filled only by  DSS
                            algorithm because IPM always stops at INTERIOR
                            point:
                            * stats[i]>0  =>  constraint at upper bound
                                              (also used for free non-basic
                                              variables set to zero)
                            * stats[i]<0  =>  constraint at lower bound
                            * stats[i]=0  =>  constraint is inactive, basic
                                              variable
* primalerror               primal feasibility error
* dualerror                 dual feasibility error
* slackerror                complementary slackness error
* iterationscount           iteration count
* terminationtype           completion code (see below)

COMPLETION CODES

Completion codes:
* -4    LP problem is primal unbounded (dual infeasible)
* -3    LP problem is primal infeasible (dual unbounded)
*  1..4 successful completion
*  5    MaxIts steps was taken
*  7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.

LAGRANGE COEFFICIENTS

Positive Lagrange coefficient means that constraint is at its upper bound.
Negative coefficient means that constraint is at its lower  bound.  It  is
expected that at solution the dual feasibility condition holds:

    C + SUM(Ei*LagBC[i],i=0..n-1) + SUM(Ai*LagLC[i],i=0..m-1) ~ 0

where
* C is a cost vector (linear term)
* Ei is a vector with 1.0 at position I and 0 in other positions
* Ai is an I-th row of linear constraint matrix
*************************************************************************/
class _minlpreport_owner
{
public:
    _minlpreport_owner();
    _minlpreport_owner(const _minlpreport_owner &rhs);
    _minlpreport_owner& operator=(const _minlpreport_owner &rhs);
    virtual ~_minlpreport_owner();
    alglib_impl::minlpreport* c_ptr();
    const alglib_impl::minlpreport* c_ptr() const;
protected:
    alglib_impl::minlpreport *p_struct;
};
class minlpreport : public _minlpreport_owner
{
public:
    minlpreport();
    minlpreport(const minlpreport &rhs);
    minlpreport& operator=(const minlpreport &rhs);
    virtual ~minlpreport();
    double &f;
    real_1d_array lagbc;
    real_1d_array laglc;
    real_1d_array y;
    integer_1d_array stats;
    double &primalerror;
    double &dualerror;
    double &slackerror;
    ae_int_t &iterationscount;
    ae_int_t &terminationtype;

};
#endif

#if defined(AE_COMPILE_NLCSLP) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINNLC) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinNLC subpackage to work  with  this
object
*************************************************************************/
class _minnlcstate_owner
{
public:
    _minnlcstate_owner();
    _minnlcstate_owner(const _minnlcstate_owner &rhs);
    _minnlcstate_owner& operator=(const _minnlcstate_owner &rhs);
    virtual ~_minnlcstate_owner();
    alglib_impl::minnlcstate* c_ptr();
    const alglib_impl::minnlcstate* c_ptr() const;
protected:
    alglib_impl::minnlcstate *p_struct;
};
class minnlcstate : public _minnlcstate_owner
{
public:
    minnlcstate();
    minnlcstate(const minnlcstate &rhs);
    minnlcstate& operator=(const minnlcstate &rhs);
    virtual ~minnlcstate();
    ae_bool &needfi;
    ae_bool &needfij;
    ae_bool &xupdated;
    double &f;
    real_1d_array fi;
    real_2d_array j;
    real_1d_array x;

};


/*************************************************************************
These fields store optimization report:
* iterationscount           total number of inner iterations
* nfev                      number of gradient evaluations
* terminationtype           termination type (see below)

Scaled constraint violations are reported:
* bcerr                     maximum violation of the box constraints
* bcidx                     index of the most violated box  constraint (or
                            -1, if all box constraints  are  satisfied  or
                            there is no box constraint)
* lcerr                     maximum violation of the  linear  constraints,
                            computed as maximum  scaled  distance  between
                            final point and constraint boundary.
* lcidx                     index of the most violated  linear  constraint
                            (or -1, if all constraints  are  satisfied  or
                            there is no general linear constraints)
* nlcerr                    maximum violation of the nonlinear constraints
* nlcidx                    index of the most violated nonlinear constraint
                            (or -1, if all constraints  are  satisfied  or
                            there is no nonlinear constraints)

Violations of box constraints are scaled on per-component basis  according
to  the  scale  vector s[] as specified by minnlcsetscale(). Violations of
the general linear  constraints  are  also  computed  using  user-supplied
variable scaling. Violations of nonlinear constraints are computed "as is"

TERMINATION CODES

TerminationType field contains completion code, which can be either:

=== FAILURE CODE ===
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signaled.
  -3    box  constraints  are  infeasible.  Note: infeasibility of non-box
        constraints does NOT trigger emergency  completion;  you  have  to
        examine  bcerr/lcerr/nlcerr   to  detect   possibly   inconsistent
        constraints.

=== SUCCESS CODE ===
   2    relative step is no more than EpsX.
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    user requested algorithm termination via minnlcrequesttermination(),
        last accepted point is returned

Other fields of this structure are not documented and should not be used!
*************************************************************************/
class _minnlcreport_owner
{
public:
    _minnlcreport_owner();
    _minnlcreport_owner(const _minnlcreport_owner &rhs);
    _minnlcreport_owner& operator=(const _minnlcreport_owner &rhs);
    virtual ~_minnlcreport_owner();
    alglib_impl::minnlcreport* c_ptr();
    const alglib_impl::minnlcreport* c_ptr() const;
protected:
    alglib_impl::minnlcreport *p_struct;
};
class minnlcreport : public _minnlcreport_owner
{
public:
    minnlcreport();
    minnlcreport(const minnlcreport &rhs);
    minnlcreport& operator=(const minnlcreport &rhs);
    virtual ~minnlcreport();
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    ae_int_t &terminationtype;
    double &bcerr;
    ae_int_t &bcidx;
    double &lcerr;
    ae_int_t &lcidx;
    double &nlcerr;
    ae_int_t &nlcidx;
    ae_int_t &dbgphase0its;

};
#endif

#if defined(AE_COMPILE_MONBI) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINMO) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinMO subpackage to work  with  this
object
*************************************************************************/
class _minmostate_owner
{
public:
    _minmostate_owner();
    _minmostate_owner(const _minmostate_owner &rhs);
    _minmostate_owner& operator=(const _minmostate_owner &rhs);
    virtual ~_minmostate_owner();
    alglib_impl::minmostate* c_ptr();
    const alglib_impl::minmostate* c_ptr() const;
protected:
    alglib_impl::minmostate *p_struct;
};
class minmostate : public _minmostate_owner
{
public:
    minmostate();
    minmostate(const minmostate &rhs);
    minmostate& operator=(const minmostate &rhs);
    virtual ~minmostate();
    ae_bool &needfi;
    ae_bool &needfij;
    ae_bool &xupdated;
    double &f;
    real_1d_array fi;
    real_2d_array j;
    real_1d_array x;

};


/*************************************************************************
These fields store optimization report:
* inneriterationscount      total number of inner iterations
* outeriterationscount      number of internal optimization sessions performed
* nfev                      number of gradient evaluations
* terminationtype           termination type (see below)

Scaled constraint violations (maximum over all Pareto points) are reported:
* bcerr                     maximum violation of the box constraints
* bcidx                     index of the most violated box  constraint (or
                            -1, if all box constraints  are  satisfied  or
                            there are no box constraint)
* lcerr                     maximum violation of the  linear  constraints,
                            computed as maximum  scaled  distance  between
                            final point and constraint boundary.
* lcidx                     index of the most violated  linear  constraint
                            (or -1, if all constraints  are  satisfied  or
                            there are no general linear constraints)
* nlcerr                    maximum violation of the nonlinear constraints
* nlcidx                    index of the most violated nonlinear constraint
                            (or -1, if all constraints  are  satisfied  or
                            there are no nonlinear constraints)

Violations  of  the  box  constraints  are  scaled  on per-component basis
according to  the  scale  vector s[]  as specified by the minmosetscale().
Violations of the general linear  constraints  are  also   computed  using
user-supplied variable scaling. Violations of  the  nonlinear  constraints
are computed "as is"

TERMINATION CODES

TerminationType field contains completion code, which can be either:

=== FAILURE CODE ===
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signaled.
  -3    box  constraints  are  infeasible.  Note: infeasibility of non-box
        constraints does NOT trigger emergency  completion;  you  have  to
        examine  bcerr/lcerr/nlcerr   to  detect   possibly   inconsistent
        constraints.

=== SUCCESS CODE ===
   2    relative step is no more than EpsX.
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.

NOTE: The solver internally performs many optimization sessions:  one  for
      each Pareto point, and some  amount  of  preparatory  optimizations.
      Different optimization  sessions  may  return  different  completion
      codes. If at least one of internal optimizations failed, its failure
      code is returned. If none of them failed, the most frequent code  is
      returned.

Other fields of this structure are not documented and should not be used!
*************************************************************************/
class _minmoreport_owner
{
public:
    _minmoreport_owner();
    _minmoreport_owner(const _minmoreport_owner &rhs);
    _minmoreport_owner& operator=(const _minmoreport_owner &rhs);
    virtual ~_minmoreport_owner();
    alglib_impl::minmoreport* c_ptr();
    const alglib_impl::minmoreport* c_ptr() const;
protected:
    alglib_impl::minmoreport *p_struct;
};
class minmoreport : public _minmoreport_owner
{
public:
    minmoreport();
    minmoreport(const minmoreport &rhs);
    minmoreport& operator=(const minmoreport &rhs);
    virtual ~minmoreport();
    ae_int_t &inneriterationscount;
    ae_int_t &outeriterationscount;
    ae_int_t &nfev;
    ae_int_t &terminationtype;
    double &bcerr;
    ae_int_t &bcidx;
    double &lcerr;
    ae_int_t &lcidx;
    double &nlcerr;
    ae_int_t &nlcidx;

};
#endif

#if defined(AE_COMPILE_MINNS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinNS subpackage to work  with  this
object
*************************************************************************/
class _minnsstate_owner
{
public:
    _minnsstate_owner();
    _minnsstate_owner(const _minnsstate_owner &rhs);
    _minnsstate_owner& operator=(const _minnsstate_owner &rhs);
    virtual ~_minnsstate_owner();
    alglib_impl::minnsstate* c_ptr();
    const alglib_impl::minnsstate* c_ptr() const;
protected:
    alglib_impl::minnsstate *p_struct;
};
class minnsstate : public _minnsstate_owner
{
public:
    minnsstate();
    minnsstate(const minnsstate &rhs);
    minnsstate& operator=(const minnsstate &rhs);
    virtual ~minnsstate();
    ae_bool &needfi;
    ae_bool &needfij;
    ae_bool &xupdated;
    double &f;
    real_1d_array fi;
    real_2d_array j;
    real_1d_array x;

};


/*************************************************************************
This structure stores optimization report:
* IterationsCount           total number of inner iterations
* NFEV                      number of gradient evaluations
* TerminationType           termination type (see below)
* CErr                      maximum violation of all types of constraints
* LCErr                     maximum violation of linear constraints
* NLCErr                    maximum violation of nonlinear constraints

TERMINATION CODES

TerminationType field contains completion code, which can be:
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signalled.
  -3    box constraints are inconsistent
  -1    inconsistent parameters were passed:
        * penalty parameter for minnssetalgoags() is zero,
          but we have nonlinear constraints set by minnssetnlc()
   2    sampling radius decreased below epsx
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    User requested termination via MinNSRequestTermination()

Other fields of this structure are not documented and should not be used!
*************************************************************************/
class _minnsreport_owner
{
public:
    _minnsreport_owner();
    _minnsreport_owner(const _minnsreport_owner &rhs);
    _minnsreport_owner& operator=(const _minnsreport_owner &rhs);
    virtual ~_minnsreport_owner();
    alglib_impl::minnsreport* c_ptr();
    const alglib_impl::minnsreport* c_ptr() const;
protected:
    alglib_impl::minnsreport *p_struct;
};
class minnsreport : public _minnsreport_owner
{
public:
    minnsreport();
    minnsreport(const minnsreport &rhs);
    minnsreport& operator=(const minnsreport &rhs);
    virtual ~minnsreport();
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    double &cerr;
    double &lcerr;
    double &nlcerr;
    ae_int_t &terminationtype;
    ae_int_t &varidx;
    ae_int_t &funcidx;

};
#endif

#if defined(AE_COMPILE_MINCOMP) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************

*************************************************************************/
class _minasastate_owner
{
public:
    _minasastate_owner();
    _minasastate_owner(const _minasastate_owner &rhs);
    _minasastate_owner& operator=(const _minasastate_owner &rhs);
    virtual ~_minasastate_owner();
    alglib_impl::minasastate* c_ptr();
    const alglib_impl::minasastate* c_ptr() const;
protected:
    alglib_impl::minasastate *p_struct;
};
class minasastate : public _minasastate_owner
{
public:
    minasastate();
    minasastate(const minasastate &rhs);
    minasastate& operator=(const minasastate &rhs);
    virtual ~minasastate();
    ae_bool &needfg;
    ae_bool &xupdated;
    double &f;
    real_1d_array g;
    real_1d_array x;

};


/*************************************************************************

*************************************************************************/
class _minasareport_owner
{
public:
    _minasareport_owner();
    _minasareport_owner(const _minasareport_owner &rhs);
    _minasareport_owner& operator=(const _minasareport_owner &rhs);
    virtual ~_minasareport_owner();
    alglib_impl::minasareport* c_ptr();
    const alglib_impl::minasareport* c_ptr() const;
protected:
    alglib_impl::minasareport *p_struct;
};
class minasareport : public _minasareport_owner
{
public:
    minasareport();
    minasareport(const minasareport &rhs);
    minasareport& operator=(const minasareport &rhs);
    virtual ~minasareport();
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    ae_int_t &terminationtype;
    ae_int_t &activeconstraints;

};
#endif

#if defined(AE_COMPILE_MINBC) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This object stores nonlinear optimizer state.
You should use functions provided by MinBC subpackage to work with this
object
*************************************************************************/
class _minbcstate_owner
{
public:
    _minbcstate_owner();
    _minbcstate_owner(const _minbcstate_owner &rhs);
    _minbcstate_owner& operator=(const _minbcstate_owner &rhs);
    virtual ~_minbcstate_owner();
    alglib_impl::minbcstate* c_ptr();
    const alglib_impl::minbcstate* c_ptr() const;
protected:
    alglib_impl::minbcstate *p_struct;
};
class minbcstate : public _minbcstate_owner
{
public:
    minbcstate();
    minbcstate(const minbcstate &rhs);
    minbcstate& operator=(const minbcstate &rhs);
    virtual ~minbcstate();
    ae_bool &needf;
    ae_bool &needfg;
    ae_bool &xupdated;
    double &f;
    real_1d_array g;
    real_1d_array x;

};


/*************************************************************************
This structure stores optimization report:
* iterationscount           number of iterations
* nfev                      number of gradient evaluations
* terminationtype           termination type (see below)

TERMINATION CODES

terminationtype field contains completion code, which can be:
  -8    internal integrity control detected  infinite  or  NAN  values  in
        function/gradient. Abnormal termination signalled.
  -3    inconsistent constraints.
   1    relative function improvement is no more than EpsF.
   2    relative step is no more than EpsX.
   4    gradient norm is no more than EpsG
   5    MaxIts steps was taken
   7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.
   8    terminated by user who called minbcrequesttermination(). X contains
        point which was "current accepted" when  termination  request  was
        submitted.
*************************************************************************/
class _minbcreport_owner
{
public:
    _minbcreport_owner();
    _minbcreport_owner(const _minbcreport_owner &rhs);
    _minbcreport_owner& operator=(const _minbcreport_owner &rhs);
    virtual ~_minbcreport_owner();
    alglib_impl::minbcreport* c_ptr();
    const alglib_impl::minbcreport* c_ptr() const;
protected:
    alglib_impl::minbcreport *p_struct;
};
class minbcreport : public _minbcreport_owner
{
public:
    minbcreport();
    minbcreport(const minbcreport &rhs);
    minbcreport& operator=(const minbcreport &rhs);
    virtual ~minbcreport();
    ae_int_t &iterationscount;
    ae_int_t &nfev;
    ae_int_t &varidx;
    ae_int_t &terminationtype;

};
#endif

#if defined(AE_COMPILE_OPTS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This is a test problem class  intended  for  internal  performance  tests.
Never use it directly in your projects.
*************************************************************************/
class _lptestproblem_owner
{
public:
    _lptestproblem_owner();
    _lptestproblem_owner(const _lptestproblem_owner &rhs);
    _lptestproblem_owner& operator=(const _lptestproblem_owner &rhs);
    virtual ~_lptestproblem_owner();
    alglib_impl::lptestproblem* c_ptr();
    const alglib_impl::lptestproblem* c_ptr() const;
protected:
    alglib_impl::lptestproblem *p_struct;
};
class lptestproblem : public _lptestproblem_owner
{
public:
    lptestproblem();
    lptestproblem(const lptestproblem &rhs);
    lptestproblem& operator=(const lptestproblem &rhs);
    virtual ~lptestproblem();

};
#endif

#if defined(AE_COMPILE_OPTGUARDAPI) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_OPTSERV) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINLBFGS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
        LIMITED MEMORY BFGS METHOD FOR LARGE SCALE OPTIMIZATION

DESCRIPTION:
The subroutine minimizes function F(x) of N arguments by  using  a  quasi-
Newton method (LBFGS scheme) which is optimized to use  a  minimum  amount
of memory.
The subroutine generates the approximation of an inverse Hessian matrix by
using information about the last M steps of the algorithm  (instead of N).
It lessens a required amount of memory from a value  of  order  N^2  to  a
value of order 2*N*M.


REQUIREMENTS:
Algorithm will request following information during its operation:
* function value F and its gradient G (simultaneously) at given point X


USAGE:
1. User initializes algorithm state with MinLBFGSCreate() call
2. User tunes solver parameters with MinLBFGSSetCond() MinLBFGSSetStpMax()
   and other functions
3. User calls MinLBFGSOptimize() function which takes algorithm  state and
   pointer (delegate, etc.) to callback function which calculates F/G.
4. User calls MinLBFGSResults() to get solution
5. Optionally user may call MinLBFGSRestartFrom() to solve another problem
   with same N/M but another starting point and/or another function.
   MinLBFGSRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension. N>0
    M       -   number of corrections in the BFGS scheme of Hessian
                approximation update. Recommended value:  3<=M<=7. The smaller
                value causes worse convergence, the bigger will  not  cause  a
                considerably better convergence, but will cause a fall in  the
                performance. M<=N.
    X       -   initial solution approximation, array[0..N-1].


OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state


NOTES:
1. you may tune stopping conditions with MinLBFGSSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLBFGSSetStpMax() function to bound algorithm's  steps.  However,
   L-BFGS rarely needs such a tuning.


  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgscreate(const ae_int_t n, const ae_int_t m, const real_1d_array &x, minlbfgsstate &state, const xparams _xparams = alglib::xdefault);
void minlbfgscreate(const ae_int_t m, const real_1d_array &x, minlbfgsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
The subroutine is finite difference variant of MinLBFGSCreate().  It  uses
finite differences in order to differentiate target function.

Description below contains information which is specific to  this function
only. We recommend to read comments on MinLBFGSCreate() in  order  to  get
more information about creation of LBFGS optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of corrections in the BFGS scheme of Hessian
                approximation update. Recommended value:  3<=M<=7. The smaller
                value causes worse convergence, the bigger will  not  cause  a
                considerably better convergence, but will cause a fall in  the
                performance. M<=N.
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinLBFGSSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is   less  robust  and  precise.  LBFGS  needs  exact  gradient values.
   Imprecise gradient may slow  down  convergence,  especially  on  highly
   nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void minlbfgscreatef(const ae_int_t n, const ae_int_t m, const real_1d_array &x, const double diffstep, minlbfgsstate &state, const xparams _xparams = alglib::xdefault);
void minlbfgscreatef(const ae_int_t m, const real_1d_array &x, const double diffstep, minlbfgsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for L-BFGS optimization algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinLBFGSSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinLBFGSSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to
automatic stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetcond(minlbfgsstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinLBFGSOptimize().


  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetxrep(minlbfgsstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0 (default),  if
                you don't want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetstpmax(minlbfgsstate &state, const double stpmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for LBFGS optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

In  most  optimizers  (and  in  the  LBFGS  too)  scaling is NOT a form of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner  by  separate  call  to  one  of  the  MinLBFGSSetPrec...()
functions.

There  is  special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetscale(minlbfgsstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification  of  the  preconditioner:  default  preconditioner    (simple
scaling, same for all elements of X) is used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetprecdefault(minlbfgsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: Cholesky factorization of  approximate
Hessian is used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    P       -   triangular preconditioner, Cholesky factorization of
                the approximate Hessian. array[0..N-1,0..N-1],
                (if larger, only leading N elements are used).
    IsUpper -   whether upper or lower triangle of P is given
                (other triangle is not referenced)

After call to this function preconditioner is changed to P  (P  is  copied
into the internal buffer).

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

NOTE 2:  P  should  be nonsingular. Exception will be thrown otherwise.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetpreccholesky(minlbfgsstate &state, const real_2d_array &p, const bool isupper, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

NOTE 2: D[i] should be positive. Exception will be thrown otherwise.

NOTE 3: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetprecdiag(minlbfgsstate &state, const real_1d_array &d, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables  with  MinLBFGSSetScale()
call  (before  or after MinLBFGSSetPrecScale() call). Without knowledge of
the scale of your variables scale-based preconditioner will be  just  unit
matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetprecscale(minlbfgsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minlbfgsiteration(minlbfgsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to launch iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    func    -   callback which calculates function (or merit function)
                value func at given point x
    grad    -   callback which calculates function (or merit function)
                value func and gradient grad at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL

NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied gradient,  and one which uses function value
   only  and  numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   (either MinLBFGSCreate() for analytical gradient  or  MinLBFGSCreateF()
   for numerical differentiation) you should choose appropriate variant of
   MinLBFGSOptimize() - one  which  accepts  function  AND gradient or one
   which accepts function ONLY.

   Be careful to choose variant of MinLBFGSOptimize() which corresponds to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinLBFGSOptimize()  and specific
   function used to create optimizer.


                     |         USER PASSED TO MinLBFGSOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinLBFGSCreateF() |     work                FAIL
   MinLBFGSCreate()  |     FAIL                work

   Here "FAIL" denotes inappropriate combinations  of  optimizer  creation
   function  and  MinLBFGSOptimize()  version.   Attemps   to   use   such
   combination (for example, to create optimizer with MinLBFGSCreateF() and
   to pass gradient information to MinCGOptimize()) will lead to exception
   being thrown. Either  you  did  not pass gradient when it WAS needed or
   you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey


*************************************************************************/
void minlbfgsoptimize(minlbfgsstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minlbfgsoptimize(minlbfgsstate &state,
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic gradient.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target function  at  the  initial  point
(note: future versions may also perform check  at  the  final  point)  and
compares numerical gradient with analytic one provided by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which  stores  both  gradients  and  specific  components  highlighted  as
suspicious by the OptGuard.

The primary OptGuard report can be retrieved with minlbfgsoptguardresults().

IMPORTANT: gradient check is a high-overhead option which  will  cost  you
           about 3*N additional function evaluations. In many cases it may
           cost as much as the rest of the optimization session.

           YOU SHOULD NOT USE IT IN THE PRODUCTION CODE UNLESS YOU WANT TO
           CHECK DERIVATIVES PROVIDED BY SOME THIRD PARTY.

NOTE: unlike previous incarnation of the gradient checking code,  OptGuard
      does NOT interrupt optimization even if it discovers bad gradient.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step used for numerical differentiation:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification
                    You should carefully choose TestStep. Value  which  is
                    too large (so large that  function  behavior  is  non-
                    cubic at this scale) will lead  to  false  alarms. Too
                    short step will result in rounding  errors  dominating
                    numerical derivative.

                    You may use different step for different parameters by
                    means of setting scale with minlbfgssetscale().

=== EXPLANATION ==========================================================

In order to verify gradient algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point

  -- ALGLIB --
     Copyright 15.06.2014 by Bochkanov Sergey
*************************************************************************/
void minlbfgsoptguardgradient(minlbfgsstate &state, const double teststep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates nonsmoothness monitoring  option  of
the  OptGuard  integrity  checker. Smoothness  monitor  silently  observes
solution process and tries to detect ill-posed problems, i.e. ones with:
a) discontinuous target function (non-C0)
b) nonsmooth     target function (non-C1)

Smoothness monitoring does NOT interrupt optimization  even if it suspects
that your problem is nonsmooth. It just sets corresponding  flags  in  the
OptGuard report which can be retrieved after optimization is over.

Smoothness monitoring is a moderate overhead option which often adds  less
than 1% to the optimizer running time. Thus, you can use it even for large
scale problems.

NOTE: OptGuard does  NOT  guarantee  that  it  will  always  detect  C0/C1
      continuity violations.

      First, minor errors are hard to  catch - say, a 0.0001 difference in
      the model values at two sides of the gap may be due to discontinuity
      of the model - or simply because the model has changed.

      Second, C1-violations  are  especially  difficult  to  detect  in  a
      noninvasive way. The optimizer usually  performs  very  short  steps
      near the nonsmoothness, and differentiation  usually   introduces  a
      lot of numerical noise.  It  is  hard  to  tell  whether  some  tiny
      discontinuity in the slope is due to real nonsmoothness or just  due
      to numerical noise alone.

      Our top priority was to avoid false positives, so in some rare cases
      minor errors may went unnoticed (however, in most cases they can  be
      spotted with restart from different initial point).

INPUT PARAMETERS:
    state   -   algorithm state
    level   -   monitoring level:
                * 0 - monitoring is disabled
                * 1 - noninvasive low-overhead monitoring; function values
                      and/or gradients are recorded, but OptGuard does not
                      try to perform additional evaluations  in  order  to
                      get more information about suspicious locations.

=== EXPLANATION ==========================================================

One major source of headache during optimization  is  the  possibility  of
the coding errors in the target function/constraints (or their gradients).
Such  errors   most   often   manifest   themselves  as  discontinuity  or
nonsmoothness of the target/constraints.

Another frequent situation is when you try to optimize something involving
lots of min() and max() operations, i.e. nonsmooth target. Although not  a
coding error, it is nonsmoothness anyway - and smooth  optimizers  usually
stop right after encountering nonsmoothness, well before reaching solution.

OptGuard integrity checker helps you to catch such situations: it monitors
function values/gradients being passed  to  the  optimizer  and  tries  to
errors. Upon discovering suspicious pair of points it  raises  appropriate
flag (and allows you to continue optimization). When optimization is done,
you can study OptGuard result.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minlbfgsoptguardsmoothness(minlbfgsstate &state, const ae_int_t level, const xparams _xparams = alglib::xdefault);
void minlbfgsoptguardsmoothness(minlbfgsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Results of OptGuard integrity check, should be called  after  optimization
session is over.

=== PRIMARY REPORT =======================================================

OptGuard performs several checks which are intended to catch common errors
in the implementation of nonlinear function/gradient:
* incorrect analytic gradient
* discontinuous (non-C0) target functions (constraints)
* nonsmooth     (non-C1) target functions (constraints)

Each of these checks is activated with appropriate function:
* minlbfgsoptguardgradient() for gradient verification
* minlbfgsoptguardsmoothness() for C0/C1 checks

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradvidx for specific variable (gradient element) suspected
  * rep.badgradxbase, a point where gradient is tested
  * rep.badgraduser, user-provided gradient  (stored  as  2D  matrix  with
    single row in order to make  report  structure  compatible  with  more
    complex optimizers like MinNLC or MinLM)
  * rep.badgradnum,   reference    gradient    obtained    via   numerical
    differentiation (stored as  2D matrix with single row in order to make
    report structure compatible with more complex optimizers  like  MinNLC
    or MinLM)
* rep.nonc0suspected
* rep.nonc1suspected

=== ADDITIONAL REPORTS/LOGS ==============================================

Several different tests are performed to catch C0/C1 errors, you can  find
out specific test signaled error by looking to:
* rep.nonc0test0positive, for non-C0 test #0
* rep.nonc1test0positive, for non-C1 test #0
* rep.nonc1test1positive, for non-C1 test #1

Additional information (including line search logs)  can  be  obtained  by
means of:
* minlbfgsoptguardnonc1test0results()
* minlbfgsoptguardnonc1test1results()
which return detailed error reports, specific points where discontinuities
were found, and so on.

==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    rep     -   generic OptGuard report;  more  detailed  reports  can  be
                retrieved with other functions.

NOTE: false negatives (nonsmooth problems are not identified as  nonsmooth
      ones) are possible although unlikely.

      The reason  is  that  you  need  to  make several evaluations around
      nonsmoothness  in  order  to  accumulate  enough  information  about
      function curvature. Say, if you start right from the nonsmooth point,
      optimizer simply won't get enough data to understand what  is  going
      wrong before it terminates due to abrupt changes in the  derivative.
      It is also  possible  that  "unlucky"  step  will  move  us  to  the
      termination too quickly.

      Our current approach is to have less than 0.1%  false  negatives  in
      our test examples  (measured  with  multiple  restarts  from  random
      points), and to have exactly 0% false positives.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minlbfgsoptguardresults(minlbfgsstate &state, optguardreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #0

Nonsmoothness (non-C1) test #0 studies  function  values  (not  gradient!)
obtained during line searches and monitors  behavior  of  the  directional
derivative estimate.

This test is less powerful than test #1, but it does  not  depend  on  the
gradient values and thus it is more robust against artifacts introduced by
numerical differentiation.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #0 "strong" report
    lngrep  -   C1 test #0 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minlbfgsoptguardnonc1test0results(const minlbfgsstate &state, optguardnonc1test0report &strrep, optguardnonc1test0report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #1

Nonsmoothness (non-C1)  test  #1  studies  individual  components  of  the
gradient computed during line search.

When precise analytic gradient is provided this test is more powerful than
test #0  which  works  with  function  values  and  ignores  user-provided
gradient.  However,  test  #0  becomes  more   powerful   when   numerical
differentiation is employed (in such cases test #1 detects  higher  levels
of numerical noise and becomes too conservative).

This test also tells specific components of the gradient which violate  C1
continuity, which makes it more informative than #0, which just tells that
continuity is violated.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* vidx - is an index of the variable in [0,N) with nonsmooth derivative
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], g[] - arrays of length CNT which store step lengths and  gradient
  values at these points; g[i] is evaluated in  x0+stp[i]*d  and  contains
  vidx-th component of the gradient.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #1 "strong" report
    lngrep  -   C1 test #1 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minlbfgsoptguardnonc1test1results(minlbfgsstate &state, optguardnonc1test1report &strrep, optguardnonc1test1report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
L-BFGS algorithm results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -8    internal integrity control  detected  infinite
                            or NAN values in  function/gradient.  Abnormal
                            termination signalled.
                    * -2    rounding errors prevent further improvement.
                            X contains best point found.
                    * -1    incorrect parameters were specified
                    *  1    relative function improvement is no more than
                            EpsF.
                    *  2    relative step is no more than EpsX.
                    *  4    gradient norm is no more than EpsG
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible
                    *  8    terminated by user who called minlbfgsrequesttermination().
                            X contains point which was "current accepted" when
                            termination request was submitted.
                * Rep.IterationsCount contains iterations count
                * NFEV countains number of function calculations

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgsresults(const minlbfgsstate &state, real_1d_array &x, minlbfgsreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
L-BFGS algorithm results

Buffered implementation of MinLBFGSResults which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 20.08.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgsresultsbuf(const minlbfgsstate &state, real_1d_array &x, minlbfgsreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  subroutine restarts LBFGS algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used to store algorithm state
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgsrestartfrom(minlbfgsstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void minlbfgsrequesttermination(minlbfgsstate &state, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_CQMODELS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_LPQPSERV) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_SNNLS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_SACTIVESETS) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_QQPSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_QPDENSEAULSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINBLEIC) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                     BOUND CONSTRAINED OPTIMIZATION
       WITH ADDITIONAL LINEAR EQUALITY AND INEQUALITY CONSTRAINTS

DESCRIPTION:
The  subroutine  minimizes  function   F(x)  of N arguments subject to any
combination of:
* bound constraints
* linear inequality constraints
* linear equality constraints

REQUIREMENTS:
* user must provide function value and gradient
* starting point X0 must be feasible or
  not too far away from the feasible set
* grad(f) must be Lipschitz continuous on a level set:
  L = { x : f(x)<=f(x0) }
* function must be defined everywhere on the feasible set F

USAGE:

Constrained optimization if far more complex than the unconstrained one.
Here we give very brief outline of the BLEIC optimizer. We strongly recommend
you to read examples in the ALGLIB Reference Manual and to read ALGLIB User Guide
on optimization, which is available at http://www.alglib.net/optimization/

1. User initializes algorithm state with MinBLEICCreate() call

2. USer adds boundary and/or linear constraints by calling
   MinBLEICSetBC() and MinBLEICSetLC() functions.

3. User sets stopping conditions with MinBLEICSetCond().

4. User calls MinBLEICOptimize() function which takes algorithm  state and
   pointer (delegate, etc.) to callback function which calculates F/G.

5. User calls MinBLEICResults() to get solution

6. Optionally user may call MinBLEICRestartFrom() to solve another problem
   with same N but another starting point.
   MinBLEICRestartFrom() allows to reuse already initialized structure.

NOTE: if you have box-only constraints (no  general  linear  constraints),
      then MinBC optimizer can be better option. It uses  special,  faster
      constraint activation method, which performs better on problems with
      multiple constraints active at the solution.

      On small-scale problems performance of MinBC is similar to  that  of
      MinBLEIC, but on large-scale ones (hundreds and thousands of  active
      constraints) it can be several times faster than MinBLEIC.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size ofX
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleiccreate(const ae_int_t n, const real_1d_array &x, minbleicstate &state, const xparams _xparams = alglib::xdefault);
void minbleiccreate(const real_1d_array &x, minbleicstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
The subroutine is finite difference variant of MinBLEICCreate().  It  uses
finite differences in order to differentiate target function.

Description below contains information which is specific to  this function
only. We recommend to read comments on MinBLEICCreate() in  order  to  get
more information about creation of BLEIC optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinBLEICSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less  robust and precise. CG needs exact gradient values. Imprecise
   gradient may slow  down  convergence, especially  on  highly  nonlinear
   problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void minbleiccreatef(const ae_int_t n, const real_1d_array &x, const double diffstep, minbleicstate &state, const xparams _xparams = alglib::xdefault);
void minbleiccreatef(const real_1d_array &x, const double diffstep, minbleicstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets boundary constraints for BLEIC optimizer.

Boundary constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with MinBLEICRestartFrom().

NOTE: if you have box-only constraints (no  general  linear  constraints),
      then MinBC optimizer can be better option. It uses  special,  faster
      constraint activation method, which performs better on problems with
      multiple constraints active at the solution.

      On small-scale problems performance of MinBC is similar to  that  of
      MinBLEIC, but on large-scale ones (hundreds and thousands of  active
      constraints) it can be several times faster than MinBLEIC.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: this solver has following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by  bound  constraints,
  even  when  numerical  differentiation is used (algorithm adjusts  nodes
  according to boundary constraints)

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetbc(minbleicstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets linear constraints for BLEIC optimizer.

Linear constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with MinBLEICRestartFrom().

INPUT PARAMETERS:
    State   -   structure previously allocated with MinBLEICCreate call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE 1: linear (non-bound) constraints are satisfied only approximately:
* there always exists some minor violation (about Epsilon in magnitude)
  due to rounding errors
* numerical differentiation, if used, may  lead  to  function  evaluations
  outside  of the feasible  area,   because   algorithm  does  NOT  change
  numerical differentiation formula according to linear constraints.
If you want constraints to be  satisfied  exactly, try to reformulate your
problem  in  such  manner  that  all constraints will become boundary ones
(this kind of constraints is always satisfied exactly, both in  the  final
solution and in all intermediate points).

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetlc(minbleicstate &state, const real_2d_array &c, const integer_1d_array &ct, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minbleicsetlc(minbleicstate &state, const real_2d_array &c, const integer_1d_array &ct, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for the optimizer.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinBLEICSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinBLEICSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection.

NOTE: when SetCond() called with non-zero MaxIts, BLEIC solver may perform
      slightly more than MaxIts iterations. I.e., MaxIts  sets  non-strict
      limit on iterations count.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetcond(minbleicstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for BLEIC optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

In  most  optimizers  (and  in  the  BLEIC  too)  scaling is NOT a form of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner  by  separate  call  to  one  of  the  MinBLEICSetPrec...()
functions.

There is a special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minbleicsetscale(minbleicstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: preconditioning is turned off.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetprecdefault(minbleicstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE 1: D[i] should be positive. Exception will be thrown otherwise.

NOTE 2: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetprecdiag(minbleicstate &state, const real_1d_array &d, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables  with  MinBLEICSetScale()
call  (before  or after MinBLEICSetPrecScale() call). Without knowledge of
the scale of your variables scale-based preconditioner will be  just  unit
matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetprecscale(minbleicstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinBLEICOptimize().

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetxrep(minbleicstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets maximum step length

IMPORTANT: this feature is hard to combine with preconditioning. You can't
set upper limit on step length, when you solve optimization  problem  with
linear (non-boundary) constraints AND preconditioner turned on.

When  non-boundary  constraints  are  present,  you  have to either a) use
preconditioner, or b) use upper limit on step length.  YOU CAN'T USE BOTH!
In this case algorithm will terminate with appropriate error code.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  lead   to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetstpmax(minbleicstate &state, const double stpmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minbleiciteration(minbleicstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to launch iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    func    -   callback which calculates function (or merit function)
                value func at given point x
    grad    -   callback which calculates function (or merit function)
                value func and gradient grad at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL

NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied gradient,  and one which uses function value
   only  and  numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   (either  MinBLEICCreate() for analytical gradient or  MinBLEICCreateF()
   for numerical differentiation) you should choose appropriate variant of
   MinBLEICOptimize() - one  which  accepts  function  AND gradient or one
   which accepts function ONLY.

   Be careful to choose variant of MinBLEICOptimize() which corresponds to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinBLEICOptimize()  and specific
   function used to create optimizer.


                     |         USER PASSED TO MinBLEICOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinBLEICCreateF() |     work                FAIL
   MinBLEICCreate()  |     FAIL                work

   Here "FAIL" denotes inappropriate combinations  of  optimizer  creation
   function  and  MinBLEICOptimize()  version.   Attemps   to   use   such
   combination (for  example,  to  create optimizer with MinBLEICCreateF()
   and  to  pass  gradient information to MinBLEICOptimize()) will lead to
   exception being thrown. Either  you  did  not pass gradient when it WAS
   needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey


*************************************************************************/
void minbleicoptimize(minbleicstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minbleicoptimize(minbleicstate &state,
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic gradient.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target function  at  the  initial  point
(note: future versions may also perform check  at  the  final  point)  and
compares numerical gradient with analytic one provided by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which  stores  both  gradients  and  specific  components  highlighted  as
suspicious by the OptGuard.

The primary OptGuard report can be retrieved with minbleicoptguardresults().

IMPORTANT: gradient check is a high-overhead option which  will  cost  you
           about 3*N additional function evaluations. In many cases it may
           cost as much as the rest of the optimization session.

           YOU SHOULD NOT USE IT IN THE PRODUCTION CODE UNLESS YOU WANT TO
           CHECK DERIVATIVES PROVIDED BY SOME THIRD PARTY.

NOTE: unlike previous incarnation of the gradient checking code,  OptGuard
      does NOT interrupt optimization even if it discovers bad gradient.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step used for numerical differentiation:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification
                    You should carefully choose TestStep. Value  which  is
                    too large (so large that  function  behavior  is  non-
                    cubic at this scale) will lead  to  false  alarms. Too
                    short step will result in rounding  errors  dominating
                    numerical derivative.

                    You may use different step for different parameters by
                    means of setting scale with minbleicsetscale().

=== EXPLANATION ==========================================================

In order to verify gradient algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point

  -- ALGLIB --
     Copyright 15.06.2014 by Bochkanov Sergey
*************************************************************************/
void minbleicoptguardgradient(minbleicstate &state, const double teststep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates nonsmoothness monitoring  option  of
the  OptGuard  integrity  checker. Smoothness  monitor  silently  observes
solution process and tries to detect ill-posed problems, i.e. ones with:
a) discontinuous target function (non-C0)
b) nonsmooth     target function (non-C1)

Smoothness monitoring does NOT interrupt optimization  even if it suspects
that your problem is nonsmooth. It just sets corresponding  flags  in  the
OptGuard report which can be retrieved after optimization is over.

Smoothness monitoring is a moderate overhead option which often adds  less
than 1% to the optimizer running time. Thus, you can use it even for large
scale problems.

NOTE: OptGuard does  NOT  guarantee  that  it  will  always  detect  C0/C1
      continuity violations.

      First, minor errors are hard to  catch - say, a 0.0001 difference in
      the model values at two sides of the gap may be due to discontinuity
      of the model - or simply because the model has changed.

      Second, C1-violations  are  especially  difficult  to  detect  in  a
      noninvasive way. The optimizer usually  performs  very  short  steps
      near the nonsmoothness, and differentiation  usually   introduces  a
      lot of numerical noise.  It  is  hard  to  tell  whether  some  tiny
      discontinuity in the slope is due to real nonsmoothness or just  due
      to numerical noise alone.

      Our top priority was to avoid false positives, so in some rare cases
      minor errors may went unnoticed (however, in most cases they can  be
      spotted with restart from different initial point).

INPUT PARAMETERS:
    state   -   algorithm state
    level   -   monitoring level:
                * 0 - monitoring is disabled
                * 1 - noninvasive low-overhead monitoring; function values
                      and/or gradients are recorded, but OptGuard does not
                      try to perform additional evaluations  in  order  to
                      get more information about suspicious locations.

=== EXPLANATION ==========================================================

One major source of headache during optimization  is  the  possibility  of
the coding errors in the target function/constraints (or their gradients).
Such  errors   most   often   manifest   themselves  as  discontinuity  or
nonsmoothness of the target/constraints.

Another frequent situation is when you try to optimize something involving
lots of min() and max() operations, i.e. nonsmooth target. Although not  a
coding error, it is nonsmoothness anyway - and smooth  optimizers  usually
stop right after encountering nonsmoothness, well before reaching solution.

OptGuard integrity checker helps you to catch such situations: it monitors
function values/gradients being passed  to  the  optimizer  and  tries  to
errors. Upon discovering suspicious pair of points it  raises  appropriate
flag (and allows you to continue optimization). When optimization is done,
you can study OptGuard result.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbleicoptguardsmoothness(minbleicstate &state, const ae_int_t level, const xparams _xparams = alglib::xdefault);
void minbleicoptguardsmoothness(minbleicstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Results of OptGuard integrity check, should be called  after  optimization
session is over.

=== PRIMARY REPORT =======================================================

OptGuard performs several checks which are intended to catch common errors
in the implementation of nonlinear function/gradient:
* incorrect analytic gradient
* discontinuous (non-C0) target functions (constraints)
* nonsmooth     (non-C1) target functions (constraints)

Each of these checks is activated with appropriate function:
* minbleicoptguardgradient() for gradient verification
* minbleicoptguardsmoothness() for C0/C1 checks

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradvidx for specific variable (gradient element) suspected
  * rep.badgradxbase, a point where gradient is tested
  * rep.badgraduser, user-provided gradient  (stored  as  2D  matrix  with
    single row in order to make  report  structure  compatible  with  more
    complex optimizers like MinNLC or MinLM)
  * rep.badgradnum,   reference    gradient    obtained    via   numerical
    differentiation (stored as  2D matrix with single row in order to make
    report structure compatible with more complex optimizers  like  MinNLC
    or MinLM)
* rep.nonc0suspected
* rep.nonc1suspected

=== ADDITIONAL REPORTS/LOGS ==============================================

Several different tests are performed to catch C0/C1 errors, you can  find
out specific test signaled error by looking to:
* rep.nonc0test0positive, for non-C0 test #0
* rep.nonc1test0positive, for non-C1 test #0
* rep.nonc1test1positive, for non-C1 test #1

Additional information (including line search logs)  can  be  obtained  by
means of:
* minbleicoptguardnonc1test0results()
* minbleicoptguardnonc1test1results()
which return detailed error reports, specific points where discontinuities
were found, and so on.

==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    rep     -   generic OptGuard report;  more  detailed  reports  can  be
                retrieved with other functions.

NOTE: false negatives (nonsmooth problems are not identified as  nonsmooth
      ones) are possible although unlikely.

      The reason  is  that  you  need  to  make several evaluations around
      nonsmoothness  in  order  to  accumulate  enough  information  about
      function curvature. Say, if you start right from the nonsmooth point,
      optimizer simply won't get enough data to understand what  is  going
      wrong before it terminates due to abrupt changes in the  derivative.
      It is also  possible  that  "unlucky"  step  will  move  us  to  the
      termination too quickly.

      Our current approach is to have less than 0.1%  false  negatives  in
      our test examples  (measured  with  multiple  restarts  from  random
      points), and to have exactly 0% false positives.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbleicoptguardresults(minbleicstate &state, optguardreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #0

Nonsmoothness (non-C1) test #0 studies  function  values  (not  gradient!)
obtained during line searches and monitors  behavior  of  the  directional
derivative estimate.

This test is less powerful than test #1, but it does  not  depend  on  the
gradient values and thus it is more robust against artifacts introduced by
numerical differentiation.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #0 "strong" report
    lngrep  -   C1 test #0 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbleicoptguardnonc1test0results(const minbleicstate &state, optguardnonc1test0report &strrep, optguardnonc1test0report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #1

Nonsmoothness (non-C1)  test  #1  studies  individual  components  of  the
gradient computed during line search.

When precise analytic gradient is provided this test is more powerful than
test #0  which  works  with  function  values  and  ignores  user-provided
gradient.  However,  test  #0  becomes  more   powerful   when   numerical
differentiation is employed (in such cases test #1 detects  higher  levels
of numerical noise and becomes too conservative).

This test also tells specific components of the gradient which violate  C1
continuity, which makes it more informative than #0, which just tells that
continuity is violated.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* vidx - is an index of the variable in [0,N) with nonsmooth derivative
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], g[] - arrays of length CNT which store step lengths and  gradient
  values at these points; g[i] is evaluated in  x0+stp[i]*d  and  contains
  vidx-th component of the gradient.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #1 "strong" report
    lngrep  -   C1 test #1 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbleicoptguardnonc1test1results(minbleicstate &state, optguardnonc1test1report &strrep, optguardnonc1test1report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
BLEIC results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report. You should check Rep.TerminationType
                in  order  to  distinguish  successful  termination  from
                unsuccessful one:
                * -8    internal integrity control  detected  infinite or
                        NAN   values   in   function/gradient.   Abnormal
                        termination signalled.
                * -3   inconsistent constraints. Feasible point is
                       either nonexistent or too hard to find. Try to
                       restart optimizer with better initial approximation
                *  1   relative function improvement is no more than EpsF.
                *  2   scaled step is no more than EpsX.
                *  4   scaled gradient norm is no more than EpsG.
                *  5   MaxIts steps was taken
                *  8   terminated by user who called minbleicrequesttermination().
                       X contains point which was "current accepted"  when
                       termination request was submitted.
                More information about fields of this  structure  can  be
                found in the comments on MinBLEICReport datatype.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicresults(const minbleicstate &state, real_1d_array &x, minbleicreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
BLEIC results

Buffered implementation of MinBLEICResults() which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicresultsbuf(const minbleicstate &state, real_1d_array &x, minbleicreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine restarts algorithm from new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinBLEICCreate call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicrestartfrom(minbleicstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void minbleicrequesttermination(minbleicstate &state, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_QPBLEICSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_VIPMSOLVER) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINQP) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                    CONSTRAINED QUADRATIC PROGRAMMING

The subroutine creates QP optimizer. After initial creation,  it  contains
default optimization problem with zero quadratic and linear terms  and  no
constraints.

In order to actually solve something you should:
* set cost vector with minqpsetlinearterm()
* set variable bounds with minqpsetbc() or minqpsetbcall()
* specify constraint matrix with one of the following functions:
  * modern API:
    * minqpsetlc2()       for sparse two-sided constraints AL <= A*x <= AU
    * minqpsetlc2dense()  for dense  two-sided constraints AL <= A*x <= AU
    * minqpsetlc2mixed()  for mixed  two-sided constraints AL <= A*x <= AU
    * minqpaddlc2dense()  to add one dense row to the dense constraint submatrix
    * minqpaddlc2()       to add one sparse row to the sparse constraint submatrix
    * minqpaddlc2sparsefromdense() to add one sparse row (passed as a dense array) to the sparse constraint submatrix
  * legacy API:
    * minqpsetlc()        for dense one-sided equality/inequality constraints
    * minqpsetlcsparse()  for sparse one-sided equality/inequality constraints
    * minqpsetlcmixed()   for mixed dense/sparse one-sided equality/inequality constraints
* choose appropriate QP solver and set it  and  its stopping  criteria  by
  means of minqpsetalgo??????() function
* call minqpoptimize() to run the solver and  minqpresults()  to  get  the
  solution vector and additional information.

Following solvers are recommended for convex and semidefinite problems:
* QuickQP for dense problems with box-only constraints (or no constraints
  at all)
* DENSE-IPM-QP for  convex  or  semidefinite  problems  with   medium  (up
  to several thousands) variable count, dense/sparse  quadratic  term  and
  any number  (up  to  many  thousands)  of  dense/sparse  general  linear
  constraints
* SPARSE-IPM-QP for convex  or  semidefinite  problems  with   large (many
  thousands) variable count, sparse quadratic term AND linear constraints.

If your problem happens to be nonconvex,  but  either  (a) is  effectively
convexified under constraints,  or  (b)  has  unique  solution  even  with
nonconvex target, then you can use:
* QuickQP for dense nonconvex problems with box-only constraints
* DENSE-AUL-QP  for   dense   nonconvex   problems  which  are effectively
  convexified under constraints with up to several thousands of  variables
  and any (small or large) number of general linear constraints
* QP-BLEIC for dense/sparse problems with small (up to  several  hundreds)
  number of general linear  constraints  and  arbitrarily  large  variable
  count.

INPUT PARAMETERS:
    N       -   problem size

OUTPUT PARAMETERS:
    State   -   optimizer with zero quadratic/linear terms
                and no constraints

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpcreate(const ae_int_t n, minqpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets linear term for QP solver.

By default, linear term is zero.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    B       -   linear term, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetlinearterm(minqpstate &state, const real_1d_array &b, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  dense  quadratic  term  for  QP solver. By  default,
quadratic term is zero.

IMPORTANT:

This solver minimizes following  function:
    f(x) = 0.5*x'*A*x + b'*x.
Note that quadratic term has 0.5 before it. So if  you  want  to  minimize
    f(x) = x^2 + x
you should rewrite your problem as follows:
    f(x) = 0.5*(2*x^2) + x
and your matrix A will be equal to [[2.0]], not to [[1.0]]

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    A       -   matrix, array[N,N]
    IsUpper -   storage type:
                * if True, symmetric matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn't used
                * if False, symmetric matrix  A  is  given  by  its lower
                  triangle, and the upper triangle isn't used

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetquadraticterm(minqpstate &state, const real_2d_array &a, const bool isupper, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  sparse  quadratic  term  for  QP solver. By default,
quadratic  term  is  zero.  This  function  overrides  previous  calls  to
minqpsetquadraticterm() or minqpsetquadratictermsparse().

NOTE: dense solvers like DENSE-AUL-QP or DENSE-IPM-QP  will  convert  this
      matrix to dense storage anyway.

IMPORTANT:

This solver minimizes following  function:
    f(x) = 0.5*x'*A*x + b'*x.
Note that quadratic term has 0.5 before it. So if  you  want  to  minimize
    f(x) = x^2 + x
you should rewrite your problem as follows:
    f(x) = 0.5*(2*x^2) + x
and your matrix A will be equal to [[2.0]], not to [[1.0]]

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    A       -   matrix, array[N,N]
    IsUpper -   (optional) storage type:
                * if True, symmetric matrix  A  is  given  by  its  upper
                  triangle, and the lower triangle isn't used
                * if False, symmetric matrix  A  is  given  by  its lower
                  triangle, and the upper triangle isn't used
                * if not given, both lower and upper  triangles  must  be
                  filled.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetquadratictermsparse(minqpstate &state, const sparsematrix &a, const bool isupper, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets starting point for QP solver. It is useful to have good
initial approximation to the solution, because it will increase  speed  of
convergence and identification of active constraints.

NOTE: interior point solvers ignore initial point provided by user.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    X       -   starting point, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetstartingpoint(minqpstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function sets origin for QP solver. By default, following QP program
is solved:

    min(0.5*x'*A*x+b'*x)

This function allows to solve different problem:

    min(0.5*(x-x_origin)'*A*(x-x_origin)+b'*(x-x_origin))

Specification of non-zero origin affects function being minimized, but not
constraints. Box and  linear  constraints  are  still  calculated  without
origin.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    XOrigin -   origin, array[N].

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetorigin(minqpstate &state, const real_1d_array &xorigin, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison  with  tolerances)  and  as
preconditioner.

Scale of the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the
   function

If you do not know how to choose scales of your variables, you can:
* read www.alglib.net/optimization/scaling.php article
* use minqpsetscaleautodiag(), which calculates scale  using  diagonal  of
  the  quadratic  term:  S  is  set to 1/sqrt(diag(A)), which works well
  sometimes.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetscale(minqpstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets automatic evaluation of variable scaling.

IMPORTANT: this function works only for  matrices  with positive  diagonal
           elements! Zero or negative elements will  result  in  -9  error
           code  being  returned.  Specify  scale  vector  manually   with
           minqpsetscale() in such cases.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison  with  tolerances)  and  as
preconditioner.

The  best  way  to  set  scaling  is  to manually specify variable scales.
However, sometimes you just need quick-and-dirty solution  -  either  when
you perform fast prototyping, or when you know your problem well  and  you
are 100% sure that this quick solution is robust enough in your case.

One such solution is to evaluate scale of I-th variable as 1/Sqrt(A[i,i]),
where A[i,i] is an I-th diagonal element of the quadratic term.

Such approach works well sometimes, but you have to be careful here.

INPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 26.12.2017 by Bochkanov Sergey
*************************************************************************/
void minqpsetscaleautodiag(minqpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells solver to use BLEIC-based algorithm and sets  stopping
criteria for the algorithm.

This algorithm is intended for large-scale  problems,  possibly nonconvex,
with small number of general linear constraints. Feasible initial point is
essential for good performance.

IMPORTANT: when DENSE-IPM (or DENSE-AUL for  nonconvex  problems)  solvers
           are applicable, their performance is much better than  that  of
           BLEIC-QP.
           We recommend  you to use BLEIC only when other solvers can  not
           be used.

ALGORITHM FEATURES:

* supports dense and sparse QP problems
* supports box and general linear equality/inequality constraints
* can solve all types of problems  (convex,  semidefinite,  nonconvex)  as
  long as they are bounded from below under constraints.
  Say, it is possible to solve "min{-x^2} subject to -1<=x<=+1".
  Of course, global  minimum  is found only  for  positive  definite   and
  semidefinite  problems.  As  for indefinite ones - only local minimum is
  found.

ALGORITHM OUTLINE:

* BLEIC-QP solver is just a driver function for MinBLEIC solver; it solves
  quadratic  programming   problem   as   general   linearly   constrained
  optimization problem, which is solved by means of BLEIC solver  (part of
  ALGLIB, active set method).

ALGORITHM LIMITATIONS:
* This algorithm is inefficient on  problems with hundreds  and  thousands
  of general inequality constraints and infeasible initial point.  Initial
  feasibility detection stage may take too long on such constraint sets.
  Consider using DENSE-IPM or DENSE-AUL instead.
* unlike QuickQP solver, this algorithm does not perform Newton steps  and
  does not use Level 3 BLAS. Being general-purpose active set  method,  it
  can activate constraints only one-by-one. Thus, its performance is lower
  than that of QuickQP.
* its precision is also a bit  inferior  to  that  of   QuickQP.  BLEIC-QP
  performs only LBFGS steps (no Newton steps), which are good at detecting
  neighborhood of the solution, buy needs many iterations to find solution
  with more than 6 digits of precision.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled constrained gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinQPSetScale()
    EpsF    -   >=0
                The  subroutine  finishes its work if exploratory steepest
                descent  step  on  k+1-th iteration  satisfies   following
                condition:  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
    EpsX    -   >=0
                The  subroutine  finishes its work if exploratory steepest
                descent  step  on  k+1-th iteration  satisfies   following
                condition:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinQPSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited. NOTE: this  algorithm uses  LBFGS
                iterations,  which  are  relatively  cheap,  but   improve
                function value only a bit. So you will need many iterations
                to converge - from 0.1*N to 10*N, depending  on  problem's
                condition number.

IT IS VERY IMPORTANT TO CALL MinQPSetScale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS STOPPING CRITERIA ARE SCALE-DEPENDENT!

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection (presently it is  small    step
length, but it may change in the future versions of ALGLIB).

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgobleic(minqpstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells QP solver to use DENSE-AUL algorithm and sets stopping
criteria for the algorithm.

This  algorithm  is  intended  for  non-convex problems with moderate  (up
to several thousands) variable count and arbitrary number  of  constraints
which are either (a) effectively convexified under constraints or (b) have
unique solution even with nonconvex target.

IMPORTANT: when DENSE-IPM solver is applicable, its performance is usually
           much better than that of DENSE-AUL.
           We recommend  you to use DENSE-AUL only when other solvers  can
           not be used.

ALGORITHM FEATURES:

* supports  box  and  dense/sparse  general   linear   equality/inequality
  constraints
* convergence is theoretically proved for positive-definite  (convex)   QP
  problems. Semidefinite and non-convex problems can be solved as long  as
  they  are   bounded  from  below  under  constraints,  although  without
  theoretical guarantees.

ALGORITHM OUTLINE:

* this  algorithm   is   an   augmented   Lagrangian   method  with  dense
  preconditioner (hence  its  name).
* it performs several outer iterations in order to refine  values  of  the
  Lagrange multipliers. Single outer  iteration  is  a  solution  of  some
  unconstrained optimization problem: first  it  performs  dense  Cholesky
  factorization of the Hessian in order to build preconditioner  (adaptive
  regularization is applied to enforce positive  definiteness),  and  then
  it uses L-BFGS optimizer to solve optimization problem.
* typically you need about 5-10 outer iterations to converge to solution

ALGORITHM LIMITATIONS:

* because dense Cholesky driver is used, this algorithm has O(N^2)  memory
  requirements and O(OuterIterations*N^3) minimum running time.  From  the
  practical  point  of  view,  it  limits  its  applicability  by  several
  thousands of variables.
  From  the  other  side,  variables  count  is  the most limiting factor,
  and dependence on constraint count is  much  more  lower. Assuming  that
  constraint matrix is sparse, it may handle tens of thousands  of general
  linear constraints.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   >=0, stopping criteria for inner optimizer.
                Inner  iterations  are  stopped  when  step  length  (with
                variable scaling being applied) is less than EpsX.
                See  minqpsetscale()  for  more  information  on  variable
                scaling.
    Rho     -   penalty coefficient, Rho>0:
                * large enough  that  algorithm  converges  with   desired
                  precision.
                * not TOO large to prevent ill-conditioning
                * recommended values are 100, 1000 or 10000
    ItsCnt  -   number of outer iterations:
                * recommended values: 10-15 (although  in  most  cases  it
                  converges within 5 iterations, you may need a  few  more
                  to be sure).
                * ItsCnt=0 means that small number of outer iterations  is
                  automatically chosen (10 iterations in current version).
                * ItsCnt=1 means that AUL algorithm performs just as usual
                  penalty method.
                * ItsCnt>1 means that  AUL  algorithm  performs  specified
                  number of outer iterations

IT IS VERY IMPORTANT TO CALL minqpsetscale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS CONVERGENCE PROPERTIES AND STOPPING CRITERIA ARE SCALE-DEPENDENT!

NOTE: Passing  EpsX=0  will  lead  to  automatic  step  length  selection
      (specific step length chosen may change in the future  versions  of
      ALGLIB, so it is better to specify step length explicitly).

  -- ALGLIB --
     Copyright 20.08.2016 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgodenseaul(minqpstate &state, const double epsx, const double rho, const ae_int_t itscnt, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells QP solver to  use  DENSE-IPM  QP  algorithm  and  sets
stopping criteria for the algorithm.

This  algorithm  is  intended  for convex and semidefinite  problems  with
moderate (up to several thousands) variable count and arbitrary number  of
constraints.

IMPORTANT: this algorithm won't work for nonconvex problems, use DENSE-AUL
           or BLEIC-QP instead. If you try to  run  DENSE-IPM  on  problem
           with  indefinite  matrix  (matrix having  at least one negative
           eigenvalue) then depending on circumstances it may  either  (a)
           stall at some  arbitrary  point,  or  (b)  throw  exception  on
           failure of Cholesky decomposition.

ALGORITHM FEATURES:

* supports  box  and  dense/sparse  general   linear   equality/inequality
  constraints

ALGORITHM OUTLINE:

* this  algorithm  is  our implementation  of  interior  point  method  as
  formulated by  R.J.Vanderbei, with minor modifications to the  algorithm
  (damped Newton directions are extensively used)
* like all interior point methods, this algorithm  tends  to  converge  in
  roughly same number of iterations (between 15 and 50) independently from
  the problem dimensionality

ALGORITHM LIMITATIONS:

* because dense Cholesky driver is used, for  N-dimensional  problem  with
  M dense constaints this algorithm has O(N^2+N*M) memory requirements and
  O(N^3+N*M^2) running time.
  Having sparse constraints with Z nonzeros per row  relaxes  storage  and
  running time down to O(N^2+M*Z) and O(N^3+N*Z^2)
  From the practical  point  of  view,  it  limits  its  applicability  by
  several thousands of variables.
  From  the  other  side,  variables  count  is  the most limiting factor,
  and dependence on constraint count is  much  more  lower. Assuming  that
  constraint matrix is sparse, it may handle tens of thousands  of general
  linear constraints.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    Eps     -   >=0, stopping criteria. The algorithm stops  when   primal
                and dual infeasiblities as well as complementarity gap are
                less than Eps.

IT IS VERY IMPORTANT TO CALL minqpsetscale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS CONVERGENCE PROPERTIES AND STOPPING CRITERIA ARE SCALE-DEPENDENT!

NOTE: Passing EpsX=0 will lead to automatic selection of small epsilon.

===== TRACING IPM SOLVER =================================================

IPM solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'IPM'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'IPM.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'IPM'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("IPM,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgodenseipm(minqpstate &state, const double eps, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells QP solver to  use  SPARSE-IPM  QP algorithm  and  sets
stopping criteria for the algorithm.

This  algorithm  is  intended  for convex and semidefinite  problems  with
large  variable  and  constraint  count  and  sparse  quadratic  term  and
constraints. It is possible to have  some  limited  set  of  dense  linear
constraints - they will be handled separately by dense BLAS - but the more
dense constraints you have, the more time solver needs.

IMPORTANT: internally this solver performs large  and  sparse  (N+M)x(N+M)
           triangular factorization. So it expects both quadratic term and
           constraints to be highly sparse. However, its  running  time is
           influenced by BOTH fill factor and sparsity pattern.

           Generally we expect that no more than few nonzero  elements per
           row are present. However different sparsity patterns may result
           in completely different running  times  even  given  same  fill
           factor.

           In many cases this algorithm outperforms DENSE-IPM by order  of
           magnitude. However, in some cases you may  get  better  results
           with DENSE-IPM even when solving sparse task.

IMPORTANT: this algorithm won't work for nonconvex problems, use DENSE-AUL
           or BLEIC-QP instead. If you try to  run  DENSE-IPM  on  problem
           with  indefinite  matrix  (matrix having  at least one negative
           eigenvalue) then depending on circumstances it may  either  (a)
           stall at some  arbitrary  point,  or  (b)  throw  exception  on
           failure of Cholesky decomposition.

ALGORITHM FEATURES:

* supports  box  and  dense/sparse  general   linear   equality/inequality
  constraints
* specializes on large-scale sparse problems

ALGORITHM OUTLINE:

* this  algorithm  is  our implementation  of  interior  point  method  as
  formulated by  R.J.Vanderbei, with minor modifications to the  algorithm
  (damped Newton directions are extensively used)
* like all interior point methods, this algorithm  tends  to  converge  in
  roughly same number of iterations (between 15 and 50) independently from
  the problem dimensionality

ALGORITHM LIMITATIONS:

* this algorithm may handle moderate number  of dense constraints, usually
  no more than a thousand of dense ones without losing its efficiency.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    Eps     -   >=0, stopping criteria. The algorithm stops  when   primal
                and dual infeasiblities as well as complementarity gap are
                less than Eps.

IT IS VERY IMPORTANT TO CALL minqpsetscale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS CONVERGENCE PROPERTIES AND STOPPING CRITERIA ARE SCALE-DEPENDENT!

NOTE: Passing EpsX=0 will lead to automatic selection of small epsilon.

===== TRACING IPM SOLVER =================================================

IPM solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'IPM'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'IPM.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'IPM'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("IPM,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgosparseipm(minqpstate &state, const double eps, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells solver to use QuickQP  algorithm:  special  extra-fast
algorithm for problems with box-only constrants. It may  solve  non-convex
problems as long as they are bounded from below under constraints.

ALGORITHM FEATURES:
* several times faster than DENSE-IPM when running on box-only problem
* utilizes accelerated methods for activation of constraints.
* supports dense and sparse QP problems
* supports ONLY box constraints; general linear constraints are NOT
  supported by this solver
* can solve all types of problems  (convex,  semidefinite,  nonconvex)  as
  long as they are bounded from below under constraints.
  Say, it is possible to solve "min{-x^2} subject to -1<=x<=+1".
  In convex/semidefinite case global minimum  is  returned,  in  nonconvex
  case - algorithm returns one of the local minimums.

ALGORITHM OUTLINE:

* algorithm  performs  two kinds of iterations: constrained CG  iterations
  and constrained Newton iterations
* initially it performs small number of constrained CG  iterations,  which
  can efficiently activate/deactivate multiple constraints
* after CG phase algorithm tries to calculate Cholesky  decomposition  and
  to perform several constrained Newton steps. If  Cholesky  decomposition
  failed (matrix is indefinite even under constraints),  we  perform  more
  CG iterations until we converge to such set of constraints  that  system
  matrix becomes  positive  definite.  Constrained  Newton  steps  greatly
  increase convergence speed and precision.
* algorithm interleaves CG and Newton iterations which  allows  to  handle
  indefinite matrices (CG phase) and quickly converge after final  set  of
  constraints is found (Newton phase). Combination of CG and Newton phases
  is called "outer iteration".
* it is possible to turn off Newton  phase  (beneficial  for  semidefinite
  problems - Cholesky decomposition will fail too often)

ALGORITHM LIMITATIONS:

* algorithm does not support general  linear  constraints;  only  box ones
  are supported
* Cholesky decomposition for sparse problems  is  performed  with  Skyline
  Cholesky solver, which is intended for low-profile matrices. No profile-
  reducing reordering of variables is performed in this version of ALGLIB.
* problems with near-zero negative eigenvalues (or exacty zero  ones)  may
  experience about 2-3x performance penalty. The reason is  that  Cholesky
  decomposition can not be performed until we identify directions of  zero
  and negative curvature and activate corresponding boundary constraints -
  but we need a lot of trial and errors because these directions  are hard
  to notice in the matrix spectrum.
  In this case you may turn off Newton phase of algorithm.
  Large negative eigenvalues  are  not  an  issue,  so  highly  non-convex
  problems can be solved very efficiently.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled constrained gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinQPSetScale()
    EpsF    -   >=0
                The  subroutine  finishes its work if exploratory steepest
                descent  step  on  k+1-th iteration  satisfies   following
                condition:  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
    EpsX    -   >=0
                The  subroutine  finishes its work if exploratory steepest
                descent  step  on  k+1-th iteration  satisfies   following
                condition:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinQPSetScale()
    MaxOuterIts-maximum number of OUTER iterations.  One  outer  iteration
                includes some amount of CG iterations (from 5 to  ~N)  and
                one or several (usually small amount) Newton steps.  Thus,
                one outer iteration has high cost, but can greatly  reduce
                funcation value.
                Use 0 if you do not want to limit number of outer iterations.
    UseNewton-  use Newton phase or not:
                * Newton phase improves performance of  positive  definite
                  dense problems (about 2 times improvement can be observed)
                * can result in some performance penalty  on  semidefinite
                  or slightly negative definite  problems  -  each  Newton
                  phase will bring no improvement (Cholesky failure),  but
                  still will require computational time.
                * if you doubt, you can turn off this  phase  -  optimizer
                  will retain its most of its high speed.

IT IS VERY IMPORTANT TO CALL MinQPSetScale() WHEN YOU USE THIS  ALGORITHM
BECAUSE ITS STOPPING CRITERIA ARE SCALE-DEPENDENT!

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection (presently it is  small    step
length, but it may change in the future versions of ALGLIB).

  -- ALGLIB --
     Copyright 22.05.2014 by Bochkanov Sergey
*************************************************************************/
void minqpsetalgoquickqp(minqpstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxouterits, const bool usenewton, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets box constraints for QP solver

Box constraints are inactive by default (after  initial  creation).  After
being  set,  they are  preserved until explicitly overwritten with another
minqpsetbc()  or  minqpsetbcall()  call,  or  partially  overwritten  with
minqpsetbci() call.

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd[i]             BndL[i]=BndU[i]
    lower bound         BndL[i]<=x[i]           BndU[i]=+INF
    upper bound         x[i]<=BndU[i]           BndL[i]=-INF
    range               BndL[i]<=x[i]<=BndU[i]  ...
    free variable       -                       BndL[I]=-INF, BndU[I]+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF (latter is recommended because
                it will allow solver to use better algorithm).
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF (latter is recommended because
                it will allow solver to use better algorithm).

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).

NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.

NOTE: if constraints for all variables are same you may use minqpsetbcall()
      which allows to specify constraints without using arrays.

NOTE: BndL>BndU will result in QP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetbc(minqpstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets box constraints for QP solver (all variables  at  once,
same constraints for all variables)

Box constraints are inactive by default (after  initial  creation).  After
being  set,  they are  preserved until explicitly overwritten with another
minqpsetbc()  or  minqpsetbcall()  call,  or  partially  overwritten  with
minqpsetbci() call.

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd                BndL=BndU
    lower bound         BndL<=x[i]              BndU=+INF
    upper bound         x[i]<=BndU              BndL=-INF
    range               BndL<=x[i]<=BndU        ...
    free variable       -                       BndL=-INF, BndU+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bound, same for all variables
    BndU    -   upper bound, same for all variables

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).

NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.

NOTE: BndL>BndU will result in QP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetbcall(minqpstate &state, const double bndl, const double bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets box constraints for I-th variable (other variables are
not modified).

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd                BndL=BndU
    lower bound         BndL<=x[i]              BndU=+INF
    upper bound         x[i]<=BndU              BndL=-INF
    range               BndL<=x[i]<=BndU        ...
    free variable       -                       BndL=-INF, BndU+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bound
    BndU    -   upper bound

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).

NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.

NOTE: BndL>BndU will result in QP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpsetbci(minqpstate &state, const ae_int_t i, const double bndl, const double bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets dense linear constraints for QP optimizer.

This  function  overrides  results  of  previous  calls  to  minqpsetlc(),
minqpsetlcsparse() and minqpsetlcmixed().  After  call  to  this  function
all non-box constraints are dropped, and you have only  those  constraints
which were specified in the present call.

If you want  to  specify  mixed  (with  dense  and  sparse  terms)  linear
constraints, you should call minqpsetlcmixed().

INPUT PARAMETERS:
    State   -   structure previously allocated with MinQPCreate call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE 1: linear (non-bound) constraints are satisfied only approximately  -
        there always exists some violation due  to  numerical  errors  and
        algorithmic limitations (BLEIC-QP solver is most  precise,  AUL-QP
        solver is less precise).

  -- ALGLIB --
     Copyright 19.06.2012 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc(minqpstate &state, const real_2d_array &c, const integer_1d_array &ct, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minqpsetlc(minqpstate &state, const real_2d_array &c, const integer_1d_array &ct, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets sparse linear constraints for QP optimizer.

This  function  overrides  results  of  previous  calls  to  minqpsetlc(),
minqpsetlcsparse() and minqpsetlcmixed().  After  call  to  this  function
all non-box constraints are dropped, and you have only  those  constraints
which were specified in the present call.

If you want  to  specify  mixed  (with  dense  and  sparse  terms)  linear
constraints, you should call minqpsetlcmixed().

INPUT PARAMETERS:
    State   -   structure previously allocated with MinQPCreate call.
    C       -   linear  constraints,  sparse  matrix  with  dimensions  at
                least [K,N+1]. If matrix has  larger  size,  only  leading
                Kx(N+1) rectangle is used.
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0

NOTE 1: linear (non-bound) constraints are satisfied only approximately  -
        there always exists some violation due  to  numerical  errors  and
        algorithmic limitations (BLEIC-QP solver is most  precise,  AUL-QP
        solver is less precise).

  -- ALGLIB --
     Copyright 22.08.2016 by Bochkanov Sergey
*************************************************************************/
void minqpsetlcsparse(minqpstate &state, const sparsematrix &c, const integer_1d_array &ct, const ae_int_t k, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets mixed linear constraints, which include a set of  dense
rows, and a set of sparse rows.

This  function  overrides  results  of  previous  calls  to  minqpsetlc(),
minqpsetlcsparse() and minqpsetlcmixed().

This function may be useful if constraint matrix includes large number  of
both types of rows - dense and sparse. If you have just a few sparse rows,
you  may  represent  them  in  dense  format  without losing  performance.
Similarly, if you have just a few dense rows, you may store them in sparse
format with almost same performance.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinQPCreate call.
    SparseC -   linear constraints, sparse  matrix with dimensions EXACTLY
                EQUAL TO [SparseK,N+1].  Each  row  of  C  represents  one
                constraint, either equality or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    SparseCT-   type of sparse constraints, array[K]:
                * if SparseCT[i]>0, then I-th constraint is SparseC[i,*]*x >= SparseC[i,n+1]
                * if SparseCT[i]=0, then I-th constraint is SparseC[i,*]*x  = SparseC[i,n+1]
                * if SparseCT[i]<0, then I-th constraint is SparseC[i,*]*x <= SparseC[i,n+1]
    SparseK -   number of sparse equality/inequality constraints, K>=0
    DenseC  -   dense linear constraints, array[K,N+1].
                Each row of DenseC represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of DenseC (including right part) must be finite.
    DenseCT -   type of constraints, array[K]:
                * if DenseCT[i]>0, then I-th constraint is DenseC[i,*]*x >= DenseC[i,n+1]
                * if DenseCT[i]=0, then I-th constraint is DenseC[i,*]*x  = DenseC[i,n+1]
                * if DenseCT[i]<0, then I-th constraint is DenseC[i,*]*x <= DenseC[i,n+1]
    DenseK  -   number of equality/inequality constraints, DenseK>=0

NOTE 1: linear (non-box) constraints  are  satisfied only approximately  -
        there always exists some violation due  to  numerical  errors  and
        algorithmic limitations (BLEIC-QP solver is most  precise,  AUL-QP
        solver is less precise).

NOTE 2: due to backward compatibility reasons SparseC can be  larger  than
        [SparseK,N+1]. In this case only leading  [SparseK,N+1]  submatrix
        will be  used.  However,  the  rest  of  ALGLIB  has  more  strict
        requirements on the input size, so we recommend you to pass sparse
        term whose size exactly matches algorithm expectations.

  -- ALGLIB --
     Copyright 22.08.2016 by Bochkanov Sergey
*************************************************************************/
void minqpsetlcmixed(minqpstate &state, const sparsematrix &sparsec, const integer_1d_array &sparsect, const ae_int_t sparsek, const real_2d_array &densec, const integer_1d_array &densect, const ae_int_t densek, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides legacy API for specification of mixed  dense/sparse
linear constraints.

New conventions used by ALGLIB since release  3.16.0  state  that  set  of
sparse constraints comes first,  followed  by  set  of  dense  ones.  This
convention is essential when you talk about things like order of  Lagrange
multipliers.

However, legacy API accepted mixed  constraints  in  reverse  order.  This
function is here to simplify situation with code relying on legacy API. It
simply accepts constraints in one order (old) and passes them to new  API,
now in correct order.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetlcmixedlegacy(minqpstate &state, const real_2d_array &densec, const integer_1d_array &densect, const ae_int_t densek, const sparsematrix &sparsec, const integer_1d_array &sparsect, const ae_int_t sparsek, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets two-sided linear constraints AL <= A*x <= AU with dense
constraint matrix A.

NOTE: knowing  that  constraint  matrix  is  dense  helps  some QP solvers
      (especially modern IPM method) to utilize efficient  dense  Level  3
      BLAS for dense parts of the problem. If your problem has both  dense
      and sparse constraints, you  can  use  minqpsetlc2mixed()  function,
      which will result in dense algebra being applied to dense terms, and
      sparse sparse linear algebra applied to sparse terms.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    A       -   linear constraints, array[K,N]. Each row of  A  represents
                one  constraint. One-sided  inequality   constraints, two-
                sided inequality  constraints,  equality  constraints  are
                supported (see below)
    AL, AU  -   lower and upper bounds, array[K];
                * AL[i]=AU[i] => equality constraint Ai*x
                * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                * AL[i]=-INF, AU[i]=+INF => constraint is ignored
    K       -   number of equality/inequality constraints,  K>=0;  if  not
                given, inferred from sizes of A, AL, AU.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc2dense(minqpstate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minqpsetlc2dense(minqpstate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
sparse constraining matrix A. Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    A       -   sparse matrix with size [K,N] (exactly!).
                Each row of A represents one general linear constraint.
                A can be stored in any sparse storage format.
    AL, AU  -   lower and upper bounds, array[K];
                * AL[i]=AU[i] => equality constraint Ai*x
                * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                * AL[i]=-INF, AU[i]=+INF => constraint is ignored
    K       -   number  of equality/inequality constraints, K>=0.  If  K=0
                is specified, A, AL, AU are ignored.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc2(minqpstate &state, const sparsematrix &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
mixed constraining matrix A including sparse part (first SparseK rows) and
dense part (last DenseK rows). Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

This function may be useful if constraint matrix includes large number  of
both types of rows - dense and sparse. If you have just a few sparse rows,
you  may  represent  them  in  dense  format  without losing  performance.
Similarly, if you have just a few dense rows, you may store them in sparse
format with almost same performance.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    SparseA -   sparse matrix with size [K,N] (exactly!).
                Each row of A represents one general linear constraint.
                A can be stored in any sparse storage format.
    SparseK -   number of sparse constraints, SparseK>=0
    DenseA  -   linear constraints, array[K,N], set of dense constraints.
                Each row of A represents one general linear constraint.
    DenseK  -   number of dense constraints, DenseK>=0
    AL, AU  -   lower and upper bounds, array[SparseK+DenseK], with former
                SparseK elements corresponding to sparse constraints,  and
                latter DenseK elements corresponding to dense constraints;
                * AL[i]=AU[i] => equality constraint Ai*x
                * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                * AL[i]=-INF, AU[i]=+INF => constraint is ignored
    K       -   number  of equality/inequality constraints, K>=0.  If  K=0
                is specified, A, AL, AU are ignored.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minqpsetlc2mixed(minqpstate &state, const sparsematrix &sparsea, const ae_int_t ksparse, const real_2d_array &densea, const ae_int_t kdense, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
matrix of currently present dense constraints.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    A       -   linear constraint coefficient, array[N], right side is NOT
                included.
    AL, AU  -   lower and upper bounds;
                * AL=AU    => equality constraint Ai*x
                * AL<AU    => two-sided constraint AL<=A*x<=AU
                * AL=-INF  => one-sided constraint Ai*x<=AU
                * AU=+INF  => one-sided constraint AL<=Ai*x
                * AL=-INF, AU=+INF => constraint is ignored

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minqpaddlc2dense(minqpstate &state, const real_1d_array &a, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present sparse constraints.

Constraint is passed in compressed format: as list of non-zero entries  of
coefficient vector A. Such approach is more efficient than  dense  storage
for highly sparse constraint vectors.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    IdxA    -   array[NNZ], indexes of non-zero elements of A:
                * can be unsorted
                * can include duplicate indexes (corresponding entries  of
                  ValA[] will be summed)
    ValA    -   array[NNZ], values of non-zero elements of A
    NNZ     -   number of non-zero coefficients in A
    AL, AU  -   lower and upper bounds;
                * AL=AU    => equality constraint A*x
                * AL<AU    => two-sided constraint AL<=A*x<=AU
                * AL=-INF  => one-sided constraint A*x<=AU
                * AU=+INF  => one-sided constraint AL<=A*x
                * AL=-INF, AU=+INF => constraint is ignored

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minqpaddlc2(minqpstate &state, const integer_1d_array &idxa, const real_1d_array &vala, const ae_int_t nnz, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present sparse constraints.

Constraint vector A is  passed  as  a  dense  array  which  is  internally
sparsified by this function.

INPUT PARAMETERS:
    State   -   structure previously allocated with minqpcreate() call.
    DA      -   array[N], constraint vector
    AL, AU  -   lower and upper bounds;
                * AL=AU    => equality constraint A*x
                * AL<AU    => two-sided constraint AL<=A*x<=AU
                * AL=-INF  => one-sided constraint A*x<=AU
                * AU=+INF  => one-sided constraint AL<=A*x
                * AL=-INF, AU=+INF => constraint is ignored

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minqpaddlc2sparsefromdense(minqpstate &state, const real_1d_array &da, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function solves quadratic programming problem.

Prior to calling this function you should choose solver by means of one of
the following functions:

* minqpsetalgoquickqp()     - for QuickQP solver
* minqpsetalgobleic()       - for BLEIC-QP solver
* minqpsetalgodenseaul()    - for Dense-AUL-QP solver
* minqpsetalgodenseipm()    - for Dense-IPM-QP solver

These functions also allow you to control stopping criteria of the solver.
If you did not set solver,  MinQP  subpackage  will  automatically  select
solver for your problem and will run it with default stopping criteria.

However, it is better to set explicitly solver and its stopping criteria.

INPUT PARAMETERS:
    State   -   algorithm state

You should use MinQPResults() function to access results after calls
to this function.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey.
     Special thanks to Elvira Illarionova  for  important  suggestions  on
     the linearly constrained QP algorithm.
*************************************************************************/
void minqpoptimize(minqpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
QP solver results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution (on failure - the best point found
                so far).
    Rep     -   optimization report, contains:
                * completion code in Rep.TerminationType (positive  values
                  denote some kind of success, negative - failures)
                * Lagrange multipliers - for QP solvers which support them
                * other statistics
                See comments on minqpreport structure for more information

Following completion codes are returned in Rep.TerminationType:
* -9    failure of the automatic scale evaluation:  one  of  the  diagonal
        elements of the quadratic term is non-positive.  Specify  variable
        scales manually!
* -5    inappropriate solver was used:
        * QuickQP solver for problem with general linear constraints
* -4    the function is unbounded from below even under constraints,
        no meaningful minimum can be found.
* -3    inconsistent constraints (or, maybe, feasible point is too hard to
        find).
* -2    IPM solver has difficulty finding primal/dual feasible point.
        It is likely that the problem is either infeasible or unbounded,
        but it is difficult to determine exact reason for termination.
        X contains best point found so far.
*  >0   success
*  7    stopping conditions are too stringent,
        further improvement is impossible,
        X contains best point found so far.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpresults(const minqpstate &state, real_1d_array &x, minqpreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
QP results

Buffered implementation of MinQPResults() which uses pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minqpresultsbuf(const minqpstate &state, real_1d_array &x, minqpreport &rep, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_MINLM) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                IMPROVED LEVENBERG-MARQUARDT METHOD FOR
                 NON-LINEAR LEAST SQUARES OPTIMIZATION

DESCRIPTION:
This function is used to find minimum of function which is represented  as
sum of squares:
    F(x) = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])
using value of function vector f[] and Jacobian of f[].


REQUIREMENTS:
This algorithm will request following information during its operation:

* function vector f[] at given point X
* function vector f[] and Jacobian of f[] (simultaneously) at given point

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts fvec()  and jac() callbacks.
First  one  is used to calculate f[] at given point, second one calculates
f[] and Jacobian df[i]/dx[j].

You can try to initialize MinLMState structure with VJ  function and  then
use incorrect version  of  MinLMOptimize()  (for  example,  version  which
works  with  general  form function and does not provide Jacobian), but it
will  lead  to  exception  being  thrown  after first attempt to calculate
Jacobian.


USAGE:
1. User initializes algorithm state with MinLMCreateVJ() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N/M but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of functions f[i]
    X       -   initial solution, array[0..N-1]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatevj(const ae_int_t n, const ae_int_t m, const real_1d_array &x, minlmstate &state, const xparams _xparams = alglib::xdefault);
void minlmcreatevj(const ae_int_t m, const real_1d_array &x, minlmstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
                IMPROVED LEVENBERG-MARQUARDT METHOD FOR
                 NON-LINEAR LEAST SQUARES OPTIMIZATION

DESCRIPTION:
This function is used to find minimum of function which is represented  as
sum of squares:
    F(x) = f[0]^2(x[0],...,x[n-1]) + ... + f[m-1]^2(x[0],...,x[n-1])
using value of function vector f[] only. Finite differences  are  used  to
calculate Jacobian.


REQUIREMENTS:
This algorithm will request following information during its operation:
* function vector f[] at given point X

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts fvec() callback.

You can try to initialize MinLMState structure with VJ  function and  then
use incorrect version  of  MinLMOptimize()  (for  example,  version  which
works with general form function and does not accept function vector), but
it will  lead  to  exception being thrown after first attempt to calculate
Jacobian.


USAGE:
1. User initializes algorithm state with MinLMCreateV() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N/M but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    M       -   number of functions f[i]
    X       -   initial solution, array[0..N-1]
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

See also MinLMIteration, MinLMResults.

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatev(const ae_int_t n, const ae_int_t m, const real_1d_array &x, const double diffstep, minlmstate &state, const xparams _xparams = alglib::xdefault);
void minlmcreatev(const ae_int_t m, const real_1d_array &x, const double diffstep, minlmstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
    LEVENBERG-MARQUARDT-LIKE METHOD FOR NON-LINEAR OPTIMIZATION

DESCRIPTION:
This  function  is  used  to  find  minimum  of general form (not "sum-of-
-squares") function
    F = F(x[0], ..., x[n-1])
using  its  gradient  and  Hessian.  Levenberg-Marquardt modification with
L-BFGS pre-optimization and internal pre-conditioned  L-BFGS  optimization
after each Levenberg-Marquardt step is used.


REQUIREMENTS:
This algorithm will request following information during its operation:

* function value F at given point X
* F and gradient G (simultaneously) at given point X
* F, G and Hessian H (simultaneously) at given point X

There are several overloaded versions of  MinLMOptimize()  function  which
correspond  to  different LM-like optimization algorithms provided by this
unit. You should choose version which accepts func(),  grad()  and  hess()
function pointers. First pointer is used to calculate F  at  given  point,
second  one  calculates  F(x)  and  grad F(x),  third one calculates F(x),
grad F(x), hess F(x).

You can try to initialize MinLMState structure with FGH-function and  then
use incorrect version of MinLMOptimize() (for example, version which  does
not provide Hessian matrix), but it will lead to  exception  being  thrown
after first attempt to calculate Hessian.


USAGE:
1. User initializes algorithm state with MinLMCreateFGH() call
2. User tunes solver parameters with MinLMSetCond(),  MinLMSetStpMax() and
   other functions
3. User calls MinLMOptimize() function which  takes algorithm  state   and
   pointers (delegates, etc.) to callback functions.
4. User calls MinLMResults() to get solution
5. Optionally, user may call MinLMRestartFrom() to solve  another  problem
   with same N but another starting point and/or another function.
   MinLMRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   dimension, N>1
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   initial solution, array[0..N-1]

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. you may tune stopping conditions with MinLMSetCond() function
2. if target function contains exp() or other fast growing functions,  and
   optimization algorithm makes too large steps which leads  to  overflow,
   use MinLMSetStpMax() function to bound algorithm's steps.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatefgh(const ae_int_t n, const real_1d_array &x, minlmstate &state, const xparams _xparams = alglib::xdefault);
void minlmcreatefgh(const real_1d_array &x, minlmstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for Levenberg-Marquardt optimization
algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinLMSetScale()
                Recommended values: 1E-9 ... 1E-12.
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations   is    unlimited.   Only   Levenberg-Marquardt
                iterations  are  counted  (L-BFGS/CG  iterations  are  NOT
                counted because their cost is very low compared to that of
                LM).

Passing  EpsX=0  and  MaxIts=0  (simultaneously)  will  lead  to automatic
stopping criterion selection (small EpsX).

NOTE: it is not recommended to set large EpsX (say, 0.001). Because LM  is
      a second-order method, it performs very precise steps anyway.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetcond(minlmstate &state, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinLMOptimize(). Both Levenberg-Marquardt and internal  L-BFGS
iterations are reported.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetxrep(minlmstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

NOTE: non-zero StpMax leads to moderate  performance  degradation  because
intermediate  step  of  preconditioned L-BFGS optimization is incompatible
with limits on step size.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetstpmax(minlmstate &state, const double stpmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for LM optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Generally, scale is NOT considered to be a form of preconditioner.  But LM
optimizer is unique in that it uses scaling matrix both  in  the  stopping
condition tests and as Marquardt damping factor.

Proper scaling is very important for the algorithm performance. It is less
important for the quality of results, but still has some influence (it  is
easier  to  converge  when  variables  are  properly  scaled, so premature
stopping is possible when very badly scalled variables are  combined  with
relaxed stopping conditions).

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlmsetscale(minlmstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets boundary constraints for LM optimizer

Boundary constraints are inactive by default (after initial creation).
They are preserved until explicitly turned off with another SetBC() call.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF (latter is recommended because
                it will allow solver to use better algorithm).
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF (latter is recommended because
                it will allow solver to use better algorithm).

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: this solver has following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by bound constraints
  or at its boundary

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlmsetbc(minlmstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets general linear constraints for LM optimizer

Linear constraints are inactive by default (after initial creation).  They
are preserved until explicitly turned off with another minlmsetlc() call.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

IMPORTANT: if you have linear constraints, it is strongly  recommended  to
           set scale of variables with minlmsetscale(). QP solver which is
           used to calculate linearly constrained steps heavily relies  on
           good scaling of input problems.

IMPORTANT: solvers created with minlmcreatefgh()  do  not  support  linear
           constraints.

NOTE: linear  (non-bound)  constraints are satisfied only approximately  -
      there  always  exists some violation due  to  numerical  errors  and
      algorithmic limitations.

NOTE: general linear constraints  add  significant  overhead  to  solution
      process. Although solver performs roughly same amount of  iterations
      (when compared  with  similar  box-only  constrained  problem), each
      iteration   now    involves  solution  of  linearly  constrained  QP
      subproblem, which requires ~3-5 times more Cholesky  decompositions.
      Thus, if you can reformulate your problem in such way  this  it  has
      only box constraints, it may be beneficial to do so.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlmsetlc(minlmstate &state, const real_2d_array &c, const integer_1d_array &ct, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minlmsetlc(minlmstate &state, const real_2d_array &c, const integer_1d_array &ct, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function is used to change acceleration settings

You can choose between three acceleration strategies:
* AccType=0, no acceleration.
* AccType=1, secant updates are used to update quadratic model after  each
  iteration. After fixed number of iterations (or after  model  breakdown)
  we  recalculate  quadratic  model  using  analytic  Jacobian  or  finite
  differences. Number of secant-based iterations depends  on  optimization
  settings: about 3 iterations - when we have analytic Jacobian, up to 2*N
  iterations - when we use finite differences to calculate Jacobian.

AccType=1 is recommended when Jacobian  calculation  cost is prohibitively
high (several Mx1 function vector calculations  followed  by  several  NxN
Cholesky factorizations are faster than calculation of one M*N  Jacobian).
It should also be used when we have no Jacobian, because finite difference
approximation takes too much time to compute.

Table below list  optimization  protocols  (XYZ  protocol  corresponds  to
MinLMCreateXYZ) and acceleration types they support (and use by  default).

ACCELERATION TYPES SUPPORTED BY OPTIMIZATION PROTOCOLS:

protocol    0   1   comment
V           +   +
VJ          +   +
FGH         +

DEFAULT VALUES:

protocol    0   1   comment
V               x   without acceleration it is so slooooooooow
VJ          x
FGH         x

NOTE: this  function should be called before optimization. Attempt to call
it during algorithm iterations may result in unexpected behavior.

NOTE: attempt to call this function with unsupported protocol/acceleration
combination will result in exception being thrown.

  -- ALGLIB --
     Copyright 14.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlmsetacctype(minlmstate &state, const ae_int_t acctype, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minlmiteration(minlmstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to launch iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    func    -   callback which calculates function (or merit function)
                value func at given point x
    grad    -   callback which calculates function (or merit function)
                value func and gradient grad at given point x
    hess    -   callback which calculates function (or merit function)
                value func, gradient grad and Hessian hess at given point x
    fvec    -   callback which calculates function vector fi[]
                at given point x
    jac     -   callback which calculates function vector fi[]
                and Jacobian jac at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL

NOTES:

1. Depending on function used to create state  structure,  this  algorithm
   may accept Jacobian and/or Hessian and/or gradient.  According  to  the
   said above, there ase several versions of this function,  which  accept
   different sets of callbacks.

   This flexibility opens way to subtle errors - you may create state with
   MinLMCreateFGH() (optimization using Hessian), but call function  which
   does not accept Hessian. So when algorithm will request Hessian,  there
   will be no callback to call. In this case exception will be thrown.

   Be careful to avoid such errors because there is no way to find them at
   compile time - you can see them at runtime only.

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey


*************************************************************************/
void minlmoptimize(minlmstate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minlmoptimize(minlmstate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minlmoptimize(minlmstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void (*hess)(const real_1d_array &x, double &func, real_1d_array &grad, real_2d_array &hess, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minlmoptimize(minlmstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minlmoptimize(minlmstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic Jacobian.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target function vector  at  the  initial
point (note: future versions may also perform check  at  the final  point)
and compares numerical Jacobian with analytic one provided by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which stores  both  Jacobians,  and  specific  components  highlighted  as
suspicious by the OptGuard.

The OptGuard report can be retrieved with minlmoptguardresults().

IMPORTANT: gradient check is a high-overhead option which  will  cost  you
           about 3*N additional function evaluations. In many cases it may
           cost as much as the rest of the optimization session.

           YOU SHOULD NOT USE IT IN THE PRODUCTION CODE UNLESS YOU WANT TO
           CHECK DERIVATIVES PROVIDED BY SOME THIRD PARTY.

NOTE: unlike previous incarnation of the gradient checking code,  OptGuard
      does NOT interrupt optimization even if it discovers bad gradient.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step used for numerical differentiation:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification
                    You should carefully choose TestStep. Value  which  is
                    too large (so large that  function  behavior  is  non-
                    cubic at this scale) will lead  to  false  alarms. Too
                    short step will result in rounding  errors  dominating
                    numerical derivative.

                    You may use different step for different parameters by
                    means of setting scale with minlmsetscale().

=== EXPLANATION ==========================================================

In order to verify gradient algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point

  -- ALGLIB --
     Copyright 15.06.2014 by Bochkanov Sergey
*************************************************************************/
void minlmoptguardgradient(minlmstate &state, const double teststep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Results of OptGuard integrity check, should be called  after  optimization
session is over.

OptGuard checks analytic Jacobian  against  reference  value  obtained  by
numerical differentiation with user-specified step.

NOTE: other optimizers perform additional OptGuard checks for things  like
      C0/C1-continuity violations. However, LM optimizer  can  check  only
      for incorrect Jacobian.

      The reason is that unlike line search methods LM optimizer does  not
      perform extensive evaluations along the line. Thus, we simply do not
      have enough data to catch C0/C1-violations.

This check is activated with  minlmoptguardgradient() function.

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradfidx for specific function (Jacobian row) suspected
  * rep.badgradvidx for specific variable (Jacobian column) suspected
  * rep.badgradxbase, a point where gradient/Jacobian is tested
  * rep.badgraduser, user-provided gradient/Jacobian
  * rep.badgradnum, reference gradient/Jacobian obtained via numerical
    differentiation

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    rep     -   OptGuard report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minlmoptguardresults(minlmstate &state, optguardreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Levenberg-Marquardt algorithm results

NOTE: if you activated OptGuard integrity checking functionality and  want
      to get OptGuard report,  it  can  be  retrieved  with  the  help  of
      minlmoptguardresults() function.

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization  report;  includes  termination   codes   and
                additional information. Termination codes are listed below,
                see comments for this structure for more info.
                Termination code is stored in rep.terminationtype field:
                * -8    optimizer detected NAN/INF values either in the
                        function itself, or in its Jacobian
                * -3    constraints are inconsistent
                *  2    relative step is no more than EpsX.
                *  5    MaxIts steps was taken
                *  7    stopping conditions are too stringent,
                        further improvement is impossible
                *  8    terminated by user who called minlmrequesttermination().
                        X contains point which was "current accepted" when
                        termination request was submitted.

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmresults(const minlmstate &state, real_1d_array &x, minlmreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Levenberg-Marquardt algorithm results

Buffered implementation of MinLMResults(), which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 10.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmresultsbuf(const minlmstate &state, real_1d_array &x, minlmreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  subroutine  restarts  LM  algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used for reverse communication previously
                allocated with MinLMCreateXXX call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void minlmrestartfrom(minlmstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void minlmrequesttermination(minlmstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is obsolete function.

Since ALGLIB 3.3 it is equivalent to MinLMCreateVJ().

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatevgj(const ae_int_t n, const ae_int_t m, const real_1d_array &x, minlmstate &state, const xparams _xparams = alglib::xdefault);
void minlmcreatevgj(const ae_int_t m, const real_1d_array &x, minlmstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is obsolete function.

Since ALGLIB 3.3 it is equivalent to MinLMCreateFJ().

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatefgj(const ae_int_t n, const ae_int_t m, const real_1d_array &x, minlmstate &state, const xparams _xparams = alglib::xdefault);
void minlmcreatefgj(const ae_int_t m, const real_1d_array &x, minlmstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function is considered obsolete since ALGLIB 3.1.0 and is present for
backward  compatibility  only.  We  recommend  to use MinLMCreateVJ, which
provides similar, but more consistent and feature-rich interface.

  -- ALGLIB --
     Copyright 30.03.2009 by Bochkanov Sergey
*************************************************************************/
void minlmcreatefj(const ae_int_t n, const ae_int_t m, const real_1d_array &x, minlmstate &state, const xparams _xparams = alglib::xdefault);
void minlmcreatefj(const ae_int_t m, const real_1d_array &x, minlmstate &state, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_MINCG) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
        NONLINEAR CONJUGATE GRADIENT METHOD

DESCRIPTION:
The subroutine minimizes function F(x) of N arguments by using one of  the
nonlinear conjugate gradient methods.

These CG methods are globally convergent (even on non-convex functions) as
long as grad(f) is Lipschitz continuous in  a  some  neighborhood  of  the
L = { x : f(x)<=f(x0) }.


REQUIREMENTS:
Algorithm will request following information during its operation:
* function value F and its gradient G (simultaneously) at given point X


USAGE:
1. User initializes algorithm state with MinCGCreate() call
2. User tunes solver parameters with MinCGSetCond(), MinCGSetStpMax() and
   other functions
3. User calls MinCGOptimize() function which takes algorithm  state   and
   pointer (delegate, etc.) to callback function which calculates F/G.
4. User calls MinCGResults() to get solution
5. Optionally, user may call MinCGRestartFrom() to solve another  problem
   with same N but another starting point and/or another function.
   MinCGRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 25.03.2010 by Bochkanov Sergey
*************************************************************************/
void mincgcreate(const ae_int_t n, const real_1d_array &x, mincgstate &state, const xparams _xparams = alglib::xdefault);
void mincgcreate(const real_1d_array &x, mincgstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
The subroutine is finite difference variant of MinCGCreate(). It uses
finite differences in order to differentiate target function.

Description below contains information which is specific to this function
only. We recommend to read comments on MinCGCreate() in order to get more
information about creation of CG optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinCGSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less  robust  and  precise.  L-BFGS  needs  exact  gradient values.
   Imprecise  gradient may slow down  convergence,  especially  on  highly
   nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void mincgcreatef(const ae_int_t n, const real_1d_array &x, const double diffstep, mincgstate &state, const xparams _xparams = alglib::xdefault);
void mincgcreatef(const real_1d_array &x, const double diffstep, mincgstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for CG optimization algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinCGSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - ste pvector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinCGSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0, EpsX=0 and MaxIts=0 (simultaneously) will lead to
automatic stopping criterion selection (small EpsX).

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetcond(mincgstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for CG optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of CG optimizer  -  step
along I-th axis is equal to DiffStep*S[I].

In   most   optimizers  (and  in  the  CG  too)  scaling is NOT a form  of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner by separate call to one of the MinCGSetPrec...() functions.

There  is  special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void mincgsetscale(mincgstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinCGOptimize().

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetxrep(mincgstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets CG algorithm.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    CGType  -   algorithm type:
                * -1    automatic selection of the best algorithm
                * 0     DY (Dai and Yuan) algorithm
                * 1     Hybrid DY-HS algorithm

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetcgtype(mincgstate &state, const ae_int_t cgtype, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetstpmax(mincgstate &state, const double stpmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function allows to suggest initial step length to the CG algorithm.

Suggested  step  length  is used as starting point for the line search. It
can be useful when you have  badly  scaled  problem,  i.e.  when  ||grad||
(which is used as initial estimate for the first step) is many  orders  of
magnitude different from the desired step.

Line search  may  fail  on  such problems without good estimate of initial
step length. Imagine, for example, problem with ||grad||=10^50 and desired
step equal to 0.1 Line  search function will use 10^50  as  initial  step,
then  it  will  decrease step length by 2 (up to 20 attempts) and will get
10^44, which is still too large.

This function allows us to tell than line search should  be  started  from
some moderate step length, like 1.0, so algorithm will be able  to  detect
desired step length in a several searches.

Default behavior (when no step is suggested) is to use preconditioner,  if
it is available, to generate initial estimate of step length.

This function influences only first iteration of algorithm. It  should  be
called between MinCGCreate/MinCGRestartFrom() call and MinCGOptimize call.
Suggested step is ignored if you have preconditioner.

INPUT PARAMETERS:
    State   -   structure used to store algorithm state.
    Stp     -   initial estimate of the step length.
                Can be zero (no estimate).

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsuggeststep(mincgstate &state, const double stp, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: preconditioning is turned off.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecdefault(mincgstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

NOTE 2: D[i] should be positive. Exception will be thrown otherwise.

NOTE 3: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecdiag(mincgstate &state, const real_1d_array &d, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables with MinCGSetScale() call
(before or after MinCGSetPrecScale() call). Without knowledge of the scale
of your variables scale-based preconditioner will be just unit matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTE:  you  can  change  preconditioner  "on  the  fly",  during algorithm
iterations.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void mincgsetprecscale(mincgstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool mincgiteration(mincgstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to launch iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    func    -   callback which calculates function (or merit function)
                value func at given point x
    grad    -   callback which calculates function (or merit function)
                value func and gradient grad at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL

NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied  gradient, and one which uses function value
   only  and  numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   (either MinCGCreate()  for analytical gradient  or  MinCGCreateF()  for
   numerical differentiation) you should  choose  appropriate  variant  of
   MinCGOptimize() - one which accepts function AND gradient or one  which
   accepts function ONLY.

   Be careful to choose variant of MinCGOptimize()  which  corresponds  to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed  to  MinCGOptimize()  and  specific
   function used to create optimizer.


                  |         USER PASSED TO MinCGOptimize()
   CREATED WITH   |  function only   |  function and gradient
   ------------------------------------------------------------
   MinCGCreateF() |     work                FAIL
   MinCGCreate()  |     FAIL                work

   Here "FAIL" denotes inappropriate combinations  of  optimizer  creation
   function and MinCGOptimize() version. Attemps to use  such  combination
   (for  example,  to create optimizer with  MinCGCreateF()  and  to  pass
   gradient information to MinCGOptimize()) will lead to  exception  being
   thrown. Either  you  did  not  pass  gradient when it WAS needed or you
   passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey


*************************************************************************/
void mincgoptimize(mincgstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void mincgoptimize(mincgstate &state,
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic gradient.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target function  at  the  initial  point
(note: future versions may also perform check  at  the  final  point)  and
compares numerical gradient with analytic one provided by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which  stores  both  gradients  and  specific  components  highlighted  as
suspicious by the OptGuard.

The primary OptGuard report can be retrieved with mincgoptguardresults().

IMPORTANT: gradient check is a high-overhead option which  will  cost  you
           about 3*N additional function evaluations. In many cases it may
           cost as much as the rest of the optimization session.

           YOU SHOULD NOT USE IT IN THE PRODUCTION CODE UNLESS YOU WANT TO
           CHECK DERIVATIVES PROVIDED BY SOME THIRD PARTY.

NOTE: unlike previous incarnation of the gradient checking code,  OptGuard
      does NOT interrupt optimization even if it discovers bad gradient.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step used for numerical differentiation:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification
                    You should carefully choose TestStep. Value  which  is
                    too large (so large that  function  behavior  is  non-
                    cubic at this scale) will lead  to  false  alarms. Too
                    short step will result in rounding  errors  dominating
                    numerical derivative.

                    You may use different step for different parameters by
                    means of setting scale with mincgsetscale().

=== EXPLANATION ==========================================================

In order to verify gradient algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point

  -- ALGLIB --
     Copyright 15.06.2014 by Bochkanov Sergey
*************************************************************************/
void mincgoptguardgradient(mincgstate &state, const double teststep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates nonsmoothness monitoring  option  of
the  OptGuard  integrity  checker. Smoothness  monitor  silently  observes
solution process and tries to detect ill-posed problems, i.e. ones with:
a) discontinuous target function (non-C0)
b) nonsmooth     target function (non-C1)

Smoothness monitoring does NOT interrupt optimization  even if it suspects
that your problem is nonsmooth. It just sets corresponding  flags  in  the
OptGuard report which can be retrieved after optimization is over.

Smoothness monitoring is a moderate overhead option which often adds  less
than 1% to the optimizer running time. Thus, you can use it even for large
scale problems.

NOTE: OptGuard does  NOT  guarantee  that  it  will  always  detect  C0/C1
      continuity violations.

      First, minor errors are hard to  catch - say, a 0.0001 difference in
      the model values at two sides of the gap may be due to discontinuity
      of the model - or simply because the model has changed.

      Second, C1-violations  are  especially  difficult  to  detect  in  a
      noninvasive way. The optimizer usually  performs  very  short  steps
      near the nonsmoothness, and differentiation  usually   introduces  a
      lot of numerical noise.  It  is  hard  to  tell  whether  some  tiny
      discontinuity in the slope is due to real nonsmoothness or just  due
      to numerical noise alone.

      Our top priority was to avoid false positives, so in some rare cases
      minor errors may went unnoticed (however, in most cases they can  be
      spotted with restart from different initial point).

INPUT PARAMETERS:
    state   -   algorithm state
    level   -   monitoring level:
                * 0 - monitoring is disabled
                * 1 - noninvasive low-overhead monitoring; function values
                      and/or gradients are recorded, but OptGuard does not
                      try to perform additional evaluations  in  order  to
                      get more information about suspicious locations.

=== EXPLANATION ==========================================================

One major source of headache during optimization  is  the  possibility  of
the coding errors in the target function/constraints (or their gradients).
Such  errors   most   often   manifest   themselves  as  discontinuity  or
nonsmoothness of the target/constraints.

Another frequent situation is when you try to optimize something involving
lots of min() and max() operations, i.e. nonsmooth target. Although not  a
coding error, it is nonsmoothness anyway - and smooth  optimizers  usually
stop right after encountering nonsmoothness, well before reaching solution.

OptGuard integrity checker helps you to catch such situations: it monitors
function values/gradients being passed  to  the  optimizer  and  tries  to
errors. Upon discovering suspicious pair of points it  raises  appropriate
flag (and allows you to continue optimization). When optimization is done,
you can study OptGuard result.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void mincgoptguardsmoothness(mincgstate &state, const ae_int_t level, const xparams _xparams = alglib::xdefault);
void mincgoptguardsmoothness(mincgstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Results of OptGuard integrity check, should be called  after  optimization
session is over.

=== PRIMARY REPORT =======================================================

OptGuard performs several checks which are intended to catch common errors
in the implementation of nonlinear function/gradient:
* incorrect analytic gradient
* discontinuous (non-C0) target functions (constraints)
* nonsmooth     (non-C1) target functions (constraints)

Each of these checks is activated with appropriate function:
* mincgoptguardgradient() for gradient verification
* mincgoptguardsmoothness() for C0/C1 checks

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradvidx for specific variable (gradient element) suspected
  * rep.badgradxbase, a point where gradient is tested
  * rep.badgraduser, user-provided gradient  (stored  as  2D  matrix  with
    single row in order to make  report  structure  compatible  with  more
    complex optimizers like MinNLC or MinLM)
  * rep.badgradnum,   reference    gradient    obtained    via   numerical
    differentiation (stored as  2D matrix with single row in order to make
    report structure compatible with more complex optimizers  like  MinNLC
    or MinLM)
* rep.nonc0suspected
* rep.nonc1suspected

=== ADDITIONAL REPORTS/LOGS ==============================================

Several different tests are performed to catch C0/C1 errors, you can  find
out specific test signaled error by looking to:
* rep.nonc0test0positive, for non-C0 test #0
* rep.nonc1test0positive, for non-C1 test #0
* rep.nonc1test1positive, for non-C1 test #1

Additional information (including line search logs)  can  be  obtained  by
means of:
* mincgoptguardnonc1test0results()
* mincgoptguardnonc1test1results()
which return detailed error reports, specific points where discontinuities
were found, and so on.

==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    rep     -   generic OptGuard report;  more  detailed  reports  can  be
                retrieved with other functions.

NOTE: false negatives (nonsmooth problems are not identified as  nonsmooth
      ones) are possible although unlikely.

      The reason  is  that  you  need  to  make several evaluations around
      nonsmoothness  in  order  to  accumulate  enough  information  about
      function curvature. Say, if you start right from the nonsmooth point,
      optimizer simply won't get enough data to understand what  is  going
      wrong before it terminates due to abrupt changes in the  derivative.
      It is also  possible  that  "unlucky"  step  will  move  us  to  the
      termination too quickly.

      Our current approach is to have less than 0.1%  false  negatives  in
      our test examples  (measured  with  multiple  restarts  from  random
      points), and to have exactly 0% false positives.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void mincgoptguardresults(mincgstate &state, optguardreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #0

Nonsmoothness (non-C1) test #0 studies  function  values  (not  gradient!)
obtained during line searches and monitors  behavior  of  the  directional
derivative estimate.

This test is less powerful than test #1, but it does  not  depend  on  the
gradient values and thus it is more robust against artifacts introduced by
numerical differentiation.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #0 "strong" report
    lngrep  -   C1 test #0 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void mincgoptguardnonc1test0results(const mincgstate &state, optguardnonc1test0report &strrep, optguardnonc1test0report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #1

Nonsmoothness (non-C1)  test  #1  studies  individual  components  of  the
gradient computed during line search.

When precise analytic gradient is provided this test is more powerful than
test #0  which  works  with  function  values  and  ignores  user-provided
gradient.  However,  test  #0  becomes  more   powerful   when   numerical
differentiation is employed (in such cases test #1 detects  higher  levels
of numerical noise and becomes too conservative).

This test also tells specific components of the gradient which violate  C1
continuity, which makes it more informative than #0, which just tells that
continuity is violated.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* vidx - is an index of the variable in [0,N) with nonsmooth derivative
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], g[] - arrays of length CNT which store step lengths and  gradient
  values at these points; g[i] is evaluated in  x0+stp[i]*d  and  contains
  vidx-th component of the gradient.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #1 "strong" report
    lngrep  -   C1 test #1 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void mincgoptguardnonc1test1results(mincgstate &state, optguardnonc1test1report &strrep, optguardnonc1test1report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Conjugate gradient results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report:
                * Rep.TerminationType completetion code:
                    * -8    internal integrity control  detected  infinite
                            or NAN values in  function/gradient.  Abnormal
                            termination signalled.
                    * -7    gradient verification failed.
                            See MinCGSetGradientCheck() for more information.
                    *  1    relative function improvement is no more than
                            EpsF.
                    *  2    relative step is no more than EpsX.
                    *  4    gradient norm is no more than EpsG
                    *  5    MaxIts steps was taken
                    *  7    stopping conditions are too stringent,
                            further improvement is impossible,
                            we return best X found so far
                    *  8    terminated by user
                * Rep.IterationsCount contains iterations count
                * NFEV countains number of function calculations

  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey
*************************************************************************/
void mincgresults(const mincgstate &state, real_1d_array &x, mincgreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Conjugate gradient results

Buffered implementation of MinCGResults(), which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 20.04.2009 by Bochkanov Sergey
*************************************************************************/
void mincgresultsbuf(const mincgstate &state, real_1d_array &x, mincgreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  subroutine  restarts  CG  algorithm from new point. All optimization
parameters are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure used to store algorithm state.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void mincgrestartfrom(mincgstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void mincgrequesttermination(mincgstate &state, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_NLCSQP) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_LPQPPRESOLVE) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_REVISEDDUALSIMPLEX) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINLP) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                            LINEAR PROGRAMMING

The subroutine creates LP  solver.  After  initial  creation  it  contains
default optimization problem with zero cost vector and all variables being
fixed to zero values and no constraints.

In order to actually solve something you should:
* set cost vector with minlpsetcost()
* set variable bounds with minlpsetbc() or minlpsetbcall()
* specify constraint matrix with one of the following functions:
  [*] minlpsetlc()        for dense one-sided constraints
  [*] minlpsetlc2dense()  for dense two-sided constraints
  [*] minlpsetlc2()       for sparse two-sided constraints
  [*] minlpaddlc2dense()  to add one dense row to constraint matrix
  [*] minlpaddlc2()       to add one row to constraint matrix (compressed format)
* call minlpoptimize() to run the solver and  minlpresults()  to  get  the
  solution vector and additional information.

By  default,  LP  solver uses best algorithm available. As of ALGLIB 3.17,
sparse interior point (barrier) solver is used. Future releases of  ALGLIB
may introduce other solvers.

User may choose specific LP algorithm by calling:
* minlpsetalgodss() for revised dual simplex method with DSE  pricing  and
  bounds flipping ratio test (aka long dual step).  Large-scale  sparse LU
  solverwith  Forest-Tomlin update is used internally  as  linear  algebra
  driver.
* minlpsetalgoipm() for sparse interior point method

INPUT PARAMETERS:
    N       -   problem size

OUTPUT PARAMETERS:
    State   -   optimizer in the default state

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpcreate(const ae_int_t n, minlpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets LP algorithm to revised dual simplex method.

ALGLIB implementation of dual simplex method supports advanced performance
and stability improvements like DSE pricing , bounds flipping  ratio  test
(aka long dual step), Forest-Tomlin update, shifting.

INPUT PARAMETERS:
    State   -   optimizer
    Eps     -   stopping condition, Eps>=0:
                * should be small number about 1E-6 or 1E-7.
                * zero value means that solver automatically selects good
                  value (can be different in different ALGLIB versions)
                * default value is zero
                Algorithm stops when relative error is less than Eps.

===== TRACING DSS SOLVER =================================================

DSS solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'DSS'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'DSS.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'DSS'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("DSS,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 08.11.2020 by Bochkanov Sergey
*************************************************************************/
void minlpsetalgodss(minlpstate &state, const double eps, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets LP algorithm to sparse interior point method.

ALGORITHM INFORMATION:

* this  algorithm  is  our implementation  of  interior  point  method  as
  formulated by  R.J.Vanderbei, with minor modifications to the  algorithm
  (damped Newton directions are extensively used)
* like all interior point methods, this algorithm  tends  to  converge  in
  roughly same number of iterations (between 15 and 50) independently from
  the problem dimensionality

INPUT PARAMETERS:
    State   -   optimizer
    Eps     -   stopping condition, Eps>=0:
                * should be small number about 1E-6 or 1E-8.
                * zero value means that solver automatically selects good
                  value (can be different in different ALGLIB versions)
                * default value is zero
                Algorithm  stops  when  primal  error  AND  dual error AND
                duality gap are less than Eps.

===== TRACING IPM SOLVER =================================================

IPM solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'IPM'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'IPM.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'IPM'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("IPM,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 08.11.2020 by Bochkanov Sergey
*************************************************************************/
void minlpsetalgoipm(minlpstate &state, const double eps, const xparams _xparams = alglib::xdefault);
void minlpsetalgoipm(minlpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets cost term for LP solver.

By default, cost term is zero.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    C       -   cost term, array[N].

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetcost(minlpstate &state, const real_1d_array &c, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients.

ALGLIB optimizers use scaling matrices to test stopping  conditions and as
preconditioner.

Scale of the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the
   function

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetscale(minlpstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets box constraints for LP solver (all variables  at  once,
different constraints for different variables).

The default state of constraints is to have all variables fixed  at  zero.
You have to overwrite it by your own constraint vector. Constraint  status
is preserved until constraints are  explicitly  overwritten  with  another
minlpsetbc()  call,   overwritten   with  minlpsetbcall(),  or   partially
overwritten with minlmsetbci() call.

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd[i]             BndL[i]=BndU[i]
    lower bound         BndL[i]<=x[i]           BndU[i]=+INF
    upper bound         x[i]<=BndU[i]           BndL[i]=-INF
    range               BndL[i]<=x[i]<=BndU[i]  ...
    free variable       -                       BndL[I]=-INF, BndU[I]+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
    BndU    -   upper bounds, array[N].

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).

NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.

NOTE: if constraints for all variables are same you may use minlpsetbcall()
      which allows to specify constraints without using arrays.

NOTE: BndL>BndU will result in LP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetbc(minlpstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets box constraints for LP solver (all variables  at  once,
same constraints for all variables)

The default state of constraints is to have all variables fixed  at  zero.
You have to overwrite it by your own constraint vector. Constraint  status
is preserved until constraints are  explicitly  overwritten  with  another
minlpsetbc() call or partially overwritten with minlpsetbcall().

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd[i]             BndL[i]=BndU[i]
    lower bound         BndL[i]<=x[i]           BndU[i]=+INF
    upper bound         x[i]<=BndU[i]           BndL[i]=-INF
    range               BndL[i]<=x[i]<=BndU[i]  ...
    free variable       -                       BndL[I]=-INF, BndU[I]+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bound, same for all variables
    BndU    -   upper bound, same for all variables

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).

NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.

NOTE: minlpsetbc() can  be  used  to  specify  different  constraints  for
      different variables.

NOTE: BndL>BndU will result in LP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetbcall(minlpstate &state, const double bndl, const double bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets box constraints for I-th variable (other variables are
not modified).

The default state of constraints is to have all variables fixed  at  zero.
You have to overwrite it by your own constraint vector.

Following types of constraints are supported:

    DESCRIPTION         CONSTRAINT              HOW TO SPECIFY
    fixed variable      x[i]=Bnd[i]             BndL[i]=BndU[i]
    lower bound         BndL[i]<=x[i]           BndU[i]=+INF
    upper bound         x[i]<=BndU[i]           BndL[i]=-INF
    range               BndL[i]<=x[i]<=BndU[i]  ...
    free variable       -                       BndL[I]=-INF, BndU[I]+INF

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    I       -   variable index, in [0,N)
    BndL    -   lower bound for I-th variable
    BndU    -   upper bound for I-th variable

NOTE: infinite values can be specified by means of Double.PositiveInfinity
      and  Double.NegativeInfinity  (in  C#)  and  alglib::fp_posinf   and
      alglib::fp_neginf (in C++).

NOTE: you may replace infinities by very small/very large values,  but  it
      is not recommended because large numbers may introduce large numerical
      errors in the algorithm.

NOTE: minlpsetbc() can  be  used  to  specify  different  constraints  for
      different variables.

NOTE: BndL>BndU will result in LP problem being recognized as infeasible.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetbci(minlpstate &state, const ae_int_t i, const double bndl, const double bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets one-sided linear constraints A*x ~ AU, where "~" can be
a mix of "<=", "=" and ">=".

IMPORTANT: this function is provided here for compatibility with the  rest
           of ALGLIB optimizers which accept constraints  in  format  like
           this one. Many real-life problems feature two-sided constraints
           like a0 <= a*x <= a1. It is really inefficient to add them as a
           pair of one-sided constraints.

           Use minlpsetlc2dense(), minlpsetlc2(), minlpaddlc2()  (or   its
           sparse version) wherever possible.

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
    A       -   linear constraints, array[K,N+1]. Each row of A represents
                one constraint, with first N elements being linear coefficients,
                and last element being right side.
    CT      -   constraint types, array[K]:
                * if CT[i]>0, then I-th constraint is A[i,*]*x >= A[i,n]
                * if CT[i]=0, then I-th constraint is A[i,*]*x  = A[i,n]
                * if CT[i]<0, then I-th constraint is A[i,*]*x <= A[i,n]
    K       -   number of equality/inequality constraints,  K>=0;  if  not
                given, inferred from sizes of A and CT.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetlc(minlpstate &state, const real_2d_array &a, const integer_1d_array &ct, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minlpsetlc(minlpstate &state, const real_2d_array &a, const integer_1d_array &ct, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets two-sided linear constraints AL <= A*x <= AU.

This version accepts dense matrix as  input;  internally  LP  solver  uses
sparse storage  anyway  (most  LP  problems  are  sparse),  but  for  your
convenience it may accept dense inputs. This  function  overwrites  linear
constraints set by previous calls (if such calls were made).

We recommend you to use sparse version of this function unless  you  solve
small-scale LP problem (less than few hundreds of variables).

NOTE: there also exist several versions of this function:
      * one-sided dense version which  accepts  constraints  in  the  same
        format as one used by QP and  NLP solvers
      * two-sided sparse version which accepts sparse matrix
      * two-sided dense  version which allows you to add constraints row by row
      * two-sided sparse version which allows you to add constraints row by row

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
    A       -   linear constraints, array[K,N]. Each row of  A  represents
                one  constraint. One-sided  inequality   constraints, two-
                sided inequality  constraints,  equality  constraints  are
                supported (see below)
    AL, AU  -   lower and upper bounds, array[K];
                * AL[i]=AU[i] => equality constraint Ai*x
                * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                * AL[i]=-INF, AU[i]=+INF => constraint is ignored
    K       -   number of equality/inequality constraints,  K>=0;  if  not
                given, inferred from sizes of A, AL, AU.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetlc2dense(minlpstate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minlpsetlc2dense(minlpstate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
sparse constraining matrix A. Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
    A       -   sparse matrix with size [K,N] (exactly!).
                Each row of A represents one general linear constraint.
                A can be stored in any sparse storage format.
    AL, AU  -   lower and upper bounds, array[K];
                * AL[i]=AU[i] => equality constraint Ai*x
                * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                * AL[i]=-INF, AU[i]=+INF => constraint is ignored
    K       -   number  of equality/inequality constraints, K>=0.  If  K=0
                is specified, A, AL, AU are ignored.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpsetlc2(minlpstate &state, const sparsematrix &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present constraints.

This version accepts dense constraint vector as input, but  sparsifies  it
for internal storage and processing. Thus, time to add one  constraint  in
is O(N) - we have to scan entire array of length N. Sparse version of this
function is order of magnitude faster for  constraints  with  just  a  few
nonzeros per row.

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
    A       -   linear constraint coefficient, array[N], right side is NOT
                included.
    AL, AU  -   lower and upper bounds;
                * AL=AU    => equality constraint Ai*x
                * AL<AU    => two-sided constraint AL<=A*x<=AU
                * AL=-INF  => one-sided constraint Ai*x<=AU
                * AU=+INF  => one-sided constraint AL<=Ai*x
                * AL=-INF, AU=+INF => constraint is ignored

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpaddlc2dense(minlpstate &state, const real_1d_array &a, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present constraints.

Constraint is passed in compressed format: as list of non-zero entries  of
coefficient vector A. Such approach is more efficient than  dense  storage
for highly sparse constraint vectors.

INPUT PARAMETERS:
    State   -   structure previously allocated with minlpcreate() call.
    IdxA    -   array[NNZ], indexes of non-zero elements of A:
                * can be unsorted
                * can include duplicate indexes (corresponding entries  of
                  ValA[] will be summed)
    ValA    -   array[NNZ], values of non-zero elements of A
    NNZ     -   number of non-zero coefficients in A
    AL, AU  -   lower and upper bounds;
                * AL=AU    => equality constraint A*x
                * AL<AU    => two-sided constraint AL<=A*x<=AU
                * AL=-INF  => one-sided constraint A*x<=AU
                * AU=+INF  => one-sided constraint AL<=A*x
                * AL=-INF, AU=+INF => constraint is ignored

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minlpaddlc2(minlpstate &state, const integer_1d_array &idxa, const real_1d_array &vala, const ae_int_t nnz, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function solves LP problem.

INPUT PARAMETERS:
    State   -   algorithm state

You should use minlpresults() function to access results  after  calls  to
this function.

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey.
*************************************************************************/
void minlpoptimize(minlpstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
LP solver results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[N], solution (on failure: last trial point)
    Rep     -   optimization report. You should check Rep.TerminationType,
                which contains completion code, and you may check  another
                fields which contain another information  about  algorithm
                functioning.

                Failure codes returned by algorithm are:
                * -4    LP problem is primal unbounded (dual infeasible)
                * -3    LP problem is primal infeasible (dual unbounded)
                * -2    IPM solver detected that problem is either
                        infeasible or unbounded

                Success codes:
                *  1..4 successful completion
                *  5    MaxIts steps was taken

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlpresults(const minlpstate &state, real_1d_array &x, minlpreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
LP results

Buffered implementation of MinLPResults() which uses pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void minlpresultsbuf(const minlpstate &state, real_1d_array &x, minlpreport &rep, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_NLCSLP) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINNLC) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                  NONLINEARLY  CONSTRAINED  OPTIMIZATION

DESCRIPTION:
The  subroutine  minimizes  function   F(x)  of N arguments subject to any
combination of:
* bound constraints
* linear inequality constraints
* linear equality constraints
* nonlinear equality constraints Gi(x)=0
* nonlinear inequality constraints Hi(x)<=0

REQUIREMENTS:
* user must provide function value and gradient for F(), H(), G()
* starting point X0 must be feasible or not too far away from the feasible
  set
* F(), G(), H() are continuously differentiable on the  feasible  set  and
  its neighborhood
* nonlinear constraints G() and H() must have non-zero gradient at  G(x)=0
  and at H(x)=0. Say, constraint like x^2>=1 is supported, but x^2>=0   is
  NOT supported.

USAGE:

Constrained optimization if far more complex than the  unconstrained  one.
Nonlinearly constrained optimization is one of the most esoteric numerical
procedures.

Here we give very brief outline  of  the  MinNLC  optimizer.  We  strongly
recommend you to study examples in the ALGLIB Reference Manual and to read
ALGLIB User Guide on optimization, which is available at
http://www.alglib.net/optimization/

1. User initializes algorithm state with MinNLCCreate() call  and  chooses
   what NLC solver to use. There is some solver which is used by  default,
   with default settings, but you should NOT rely on  default  choice.  It
   may change in future releases of ALGLIB without notice, and no one  can
   guarantee that new solver will be  able  to  solve  your  problem  with
   default settings.

   From the other side, if you choose solver explicitly, you can be pretty
   sure that it will work with new ALGLIB releases.

   In the current release following solvers can be used:
   * SQP solver, recommended for medium-scale problems (less than thousand
     of variables) with hard-to-evaluate target functions.  Requires  less
     function  evaluations  than  other  solvers  but  each  step involves
     solution of QP subproblem, so running time may be higher than that of
     AUL (another recommended option). Activated  with  minnlcsetalgosqp()
     function.
   * AUL solver with dense  preconditioner,  recommended  for  large-scale
     problems or for problems  with  cheap  target  function.  Needs  more
     function evaluations that SQP (about  5x-10x  times  more),  but  its
     iterations  are  much  cheaper  that  that  of  SQP.  Activated  with
     minnlcsetalgoaul() function.
   * SLP solver, successive linear programming. The slowest one,  requires
     more target function evaluations that SQP and  AUL.  However,  it  is
     somewhat more robust in tricky cases, so it can be used  as  a backup
     plan. Activated with minnlcsetalgoslp() function.

2. [optional] user activates OptGuard  integrity checker  which  tries  to
   detect possible errors in the user-supplied callbacks:
   * discontinuity/nonsmoothness of the target/nonlinear constraints
   * errors in the analytic gradient provided by user
   This feature is essential for early prototyping stages because it helps
   to catch common coding and problem statement errors.
   OptGuard can be activated with following functions (one per each  check
   performed):
   * minnlcoptguardsmoothness()
   * minnlcoptguardgradient()

3. User adds boundary and/or linear and/or nonlinear constraints by  means
   of calling one of the following functions:
   a) minnlcsetbc() for boundary constraints
   b) minnlcsetlc() for linear constraints
   c) minnlcsetnlc() for nonlinear constraints
   You may combine (a), (b) and (c) in one optimization problem.

4. User sets scale of the variables with minnlcsetscale() function. It  is
   VERY important to set  scale  of  the  variables,  because  nonlinearly
   constrained problems are hard to solve when variables are badly scaled.

5. User sets  stopping  conditions  with  minnlcsetcond(). If  NLC  solver
   uses  inner/outer  iteration  layout,  this  function   sets   stopping
   conditions for INNER iterations.

6. Finally, user calls minnlcoptimize()  function  which  takes  algorithm
   state and pointer (delegate, etc.) to callback function which calculates
   F/G/H.

7. User calls  minnlcresults()  to  get  solution;  additionally  you  can
   retrieve OptGuard report with minnlcoptguardresults(), and get detailed
   report about purported errors in the target function with:
   * minnlcoptguardnonc1test0results()
   * minnlcoptguardnonc1test1results()

8. Optionally user may call minnlcrestartfrom() to solve  another  problem
   with same N but another starting point. minnlcrestartfrom()  allows  to
   reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size ofX
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlccreate(const ae_int_t n, const real_1d_array &x, minnlcstate &state, const xparams _xparams = alglib::xdefault);
void minnlccreate(const real_1d_array &x, minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine is a finite  difference variant of MinNLCCreate(). It uses
finite differences in order to differentiate target function.

Description below contains information which is specific to this  function
only. We recommend to read comments on MinNLCCreate() in order to get more
information about creation of NLC optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size ofX
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinNLCSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large TRUNCATION  errors, while too small
   step will result in too large NUMERICAL  errors.  1.0E-4  can  be  good
   value to start from.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less   robust   and  precise.  Imprecise  gradient  may  slow  down
   convergence, especially on highly nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlccreatef(const ae_int_t n, const real_1d_array &x, const double diffstep, minnlcstate &state, const xparams _xparams = alglib::xdefault);
void minnlccreatef(const real_1d_array &x, const double diffstep, minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets boundary constraints for NLC optimizer.

Boundary constraints are inactive by  default  (after  initial  creation).
They are preserved after algorithm restart with  MinNLCRestartFrom().

You may combine boundary constraints with  general  linear ones - and with
nonlinear ones! Boundary constraints are  handled  more  efficiently  than
other types.  Thus,  if  your  problem  has  mixed  constraints,  you  may
explicitly specify some of them as boundary and save some time/space.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

NOTE 1:  it is possible to specify  BndL[i]=BndU[i].  In  this  case  I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2:  when you solve your problem  with  augmented  Lagrangian  solver,
         boundary constraints are  satisfied  only  approximately!  It  is
         possible   that  algorithm  will  evaluate  function  outside  of
         feasible area!

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetbc(minnlcstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets linear constraints for MinNLC optimizer.

Linear constraints are inactive by default (after initial creation).  They
are preserved after algorithm restart with MinNLCRestartFrom().

You may combine linear constraints with boundary ones - and with nonlinear
ones! If your problem has mixed constraints, you  may  explicitly  specify
some of them as linear. It  may  help  optimizer   to   handle  them  more
efficiently.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinNLCCreate call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE 1: when you solve your problem  with  augmented  Lagrangian   solver,
        linear constraints are  satisfied  only   approximately!   It   is
        possible   that  algorithm  will  evaluate  function  outside   of
        feasible area!

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetlc(minnlcstate &state, const real_2d_array &c, const integer_1d_array &ct, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minnlcsetlc(minnlcstate &state, const real_2d_array &c, const integer_1d_array &ct, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets nonlinear constraints for MinNLC optimizer.

In fact, this function sets NUMBER of nonlinear  constraints.  Constraints
itself (constraint functions) are passed to MinNLCOptimize() method.  This
method requires user-defined vector function F[]  and  its  Jacobian  J[],
where:
* first component of F[] and first row  of  Jacobian  J[]  corresponds  to
  function being minimized
* next NLEC components of F[] (and rows  of  J)  correspond  to  nonlinear
  equality constraints G_i(x)=0
* next NLIC components of F[] (and rows  of  J)  correspond  to  nonlinear
  inequality constraints H_i(x)<=0

NOTE: you may combine nonlinear constraints with linear/boundary ones.  If
      your problem has mixed constraints, you  may explicitly specify some
      of them as linear ones. It may help optimizer to  handle  them  more
      efficiently.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinNLCCreate call.
    NLEC    -   number of Non-Linear Equality Constraints (NLEC), >=0
    NLIC    -   number of Non-Linear Inquality Constraints (NLIC), >=0

NOTE 1: when you solve your problem  with  augmented  Lagrangian   solver,
        nonlinear constraints are satisfied only  approximately!   It   is
        possible   that  algorithm  will  evaluate  function  outside   of
        feasible area!

NOTE 2: algorithm scales variables  according  to   scale   specified   by
        MinNLCSetScale()  function,  so  it can handle problems with badly
        scaled variables (as long as we KNOW their scales).

        However,  there  is  no  way  to  automatically  scale   nonlinear
        constraints Gi(x) and Hi(x). Inappropriate scaling  of  Gi/Hi  may
        ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
        is NOT same as solving it with constraint "0.001*G0(x)=0".

        It  means  that  YOU  are  the  one who is responsible for correct
        scaling of nonlinear constraints Gi(x) and Hi(x). We recommend you
        to scale nonlinear constraints in such way that I-th component  of
        dG/dX (or dH/dx) has approximately unit  magnitude  (for  problems
        with unit scale)  or  has  magnitude approximately equal to 1/S[i]
        (where S is a scale set by MinNLCSetScale() function).


  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetnlc(minnlcstate &state, const ae_int_t nlec, const ae_int_t nlic, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for inner iterations of  optimizer.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinNLCSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsX=0 and MaxIts=0 (simultaneously) will lead to automatic
selection of the stopping condition.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetcond(minnlcstate &state, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for NLC optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetscale(minnlcstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets preconditioner to "inexact LBFGS-based" mode.

Preconditioning is very important for convergence of  Augmented Lagrangian
algorithm because presence of penalty term makes problem  ill-conditioned.
Difference between  performance  of  preconditioned  and  unpreconditioned
methods can be as large as 100x!

MinNLC optimizer may use following preconditioners,  each  with   its  own
benefits and drawbacks:
    a) inexact LBFGS-based, with O(N*K) evaluation time
    b) exact low rank one,  with O(N*K^2) evaluation time
    c) exact robust one,    with O(N^3+K*N^2) evaluation time
where K is a total number of general linear and nonlinear constraints (box
ones are not counted).

Inexact  LBFGS-based  preconditioner  uses L-BFGS  formula  combined  with
orthogonality assumption to perform very fast updates. For a N-dimensional
problem with K general linear or nonlinear constraints (boundary ones  are
not counted) it has O(N*K) cost per iteration.  This   preconditioner  has
best  quality  (less  iterations)  when   general   linear  and  nonlinear
constraints are orthogonal to each other (orthogonality  with  respect  to
boundary constraints is not required). Number of iterations increases when
constraints  are  non-orthogonal, because algorithm assumes orthogonality,
but still it is better than no preconditioner at all.

INPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 26.09.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetprecinexact(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets preconditioner to "exact low rank" mode.

Preconditioning is very important for convergence of  Augmented Lagrangian
algorithm because presence of penalty term makes problem  ill-conditioned.
Difference between  performance  of  preconditioned  and  unpreconditioned
methods can be as large as 100x!

MinNLC optimizer may use following preconditioners,  each  with   its  own
benefits and drawbacks:
    a) inexact LBFGS-based, with O(N*K) evaluation time
    b) exact low rank one,  with O(N*K^2) evaluation time
    c) exact robust one,    with O(N^3+K*N^2) evaluation time
where K is a total number of general linear and nonlinear constraints (box
ones are not counted).

It also provides special unpreconditioned mode of operation which  can  be
used for test purposes. Comments below discuss low rank preconditioner.

Exact low-rank preconditioner  uses  Woodbury  matrix  identity  to  build
quadratic model of the penalized function. It has following features:
* no special assumptions about orthogonality of constraints
* preconditioner evaluation is optimized for K<<N. Its cost  is  O(N*K^2),
  so it may become prohibitively slow for K>=N.
* finally, stability of the process is guaranteed only for K<<N.  Woodbury
  update often fail for K>=N due to degeneracy of  intermediate  matrices.
  That's why we recommend to use "exact robust"  preconditioner  for  such
  cases.

RECOMMENDATIONS

We  recommend  to  choose  between  "exact  low  rank"  and "exact robust"
preconditioners, with "low rank" version being chosen  when  you  know  in
advance that total count of non-box constraints won't exceed N, and "robust"
version being chosen when you need bulletproof solution.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    UpdateFreq- update frequency. Preconditioner is  rebuilt  after  every
                UpdateFreq iterations. Recommended value: 10 or higher.
                Zero value means that good default value will be used.

  -- ALGLIB --
     Copyright 26.09.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetprecexactlowrank(minnlcstate &state, const ae_int_t updatefreq, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets preconditioner to "exact robust" mode.

Preconditioning is very important for convergence of  Augmented Lagrangian
algorithm because presence of penalty term makes problem  ill-conditioned.
Difference between  performance  of  preconditioned  and  unpreconditioned
methods can be as large as 100x!

MinNLC optimizer may use following preconditioners,  each  with   its  own
benefits and drawbacks:
    a) inexact LBFGS-based, with O(N*K) evaluation time
    b) exact low rank one,  with O(N*K^2) evaluation time
    c) exact robust one,    with O(N^3+K*N^2) evaluation time
where K is a total number of general linear and nonlinear constraints (box
ones are not counted).

It also provides special unpreconditioned mode of operation which  can  be
used for test purposes. Comments below discuss robust preconditioner.

Exact  robust  preconditioner   uses   Cholesky  decomposition  to  invert
approximate Hessian matrix H=D+W'*C*W (where D stands for  diagonal  terms
of Hessian, combined result of initial scaling matrix and penalty from box
constraints; W stands for general linear constraints and linearization  of
nonlinear ones; C stands for diagonal matrix of penalty coefficients).

This preconditioner has following features:
* no special assumptions about constraint structure
* preconditioner is optimized  for  stability;  unlike  "exact  low  rank"
  version which fails for K>=N, this one works well for any value of K.
* the only drawback is that is takes O(N^3+K*N^2) time  to  build  it.  No
  economical  Woodbury update is applied even when it  makes  sense,  thus
  there  are  exist situations (K<<N) when "exact low rank" preconditioner
  outperforms this one.

RECOMMENDATIONS

We  recommend  to  choose  between  "exact  low  rank"  and "exact robust"
preconditioners, with "low rank" version being chosen  when  you  know  in
advance that total count of non-box constraints won't exceed N, and "robust"
version being chosen when you need bulletproof solution.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    UpdateFreq- update frequency. Preconditioner is  rebuilt  after  every
                UpdateFreq iterations. Recommended value: 10 or higher.
                Zero value means that good default value will be used.

  -- ALGLIB --
     Copyright 26.09.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetprecexactrobust(minnlcstate &state, const ae_int_t updatefreq, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets preconditioner to "turned off" mode.

Preconditioning is very important for convergence of  Augmented Lagrangian
algorithm because presence of penalty term makes problem  ill-conditioned.
Difference between  performance  of  preconditioned  and  unpreconditioned
methods can be as large as 100x!

MinNLC optimizer may  utilize  two  preconditioners,  each  with  its  own
benefits and drawbacks: a) inexact LBFGS-based, and b) exact low rank one.
It also provides special unpreconditioned mode of operation which  can  be
used for test purposes.

This function activates this test mode. Do not use it in  production  code
to solve real-life problems.

INPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 26.09.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetprecnone(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets maximum step length (after scaling of step vector  with
respect to variable scales specified by minnlcsetscale() call).

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0 (default),  if
                you don't want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  leads  to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

NOTE: different solvers employed by MinNLC optimizer use  different  norms
      for step; AUL solver uses 2-norm, whilst SLP solver uses INF-norm.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minnlcsetstpmax(minnlcstate &state, const double stpmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  tells MinNLC unit to use  Augmented  Lagrangian  algorithm
for nonlinearly constrained  optimization.  This  algorithm  is  a  slight
modification of one described in "A Modified Barrier-Augmented  Lagrangian
Method for  Constrained  Minimization  (1999)"  by  D.GOLDFARB,  R.POLYAK,
K. SCHEINBERG, I.YUZEFOVICH.

AUL solver can be significantly faster than SQP on easy  problems  due  to
cheaper iterations, although it needs more function evaluations.

Augmented Lagrangian algorithm works by converting problem  of  minimizing
F(x) subject to equality/inequality constraints   to unconstrained problem
of the form

    min[ f(x) +
        + Rho*PENALTY_EQ(x)   + SHIFT_EQ(x,Nu1) +
        + Rho*PENALTY_INEQ(x) + SHIFT_INEQ(x,Nu2) ]

where:
* Rho is a fixed penalization coefficient
* PENALTY_EQ(x) is a penalty term, which is used to APPROXIMATELY  enforce
  equality constraints
* SHIFT_EQ(x) is a special "shift"  term  which  is  used  to  "fine-tune"
  equality constraints, greatly increasing precision
* PENALTY_INEQ(x) is a penalty term which is used to approximately enforce
  inequality constraints
* SHIFT_INEQ(x) is a special "shift"  term  which  is  used to "fine-tune"
  inequality constraints, greatly increasing precision
* Nu1/Nu2 are vectors of Lagrange coefficients which are fine-tuned during
  outer iterations of algorithm

This  version  of  AUL  algorithm  uses   preconditioner,  which   greatly
accelerates convergence. Because this  algorithm  is  similar  to  penalty
methods,  it  may  perform  steps  into  infeasible  area.  All  kinds  of
constraints (boundary, linear and nonlinear ones) may   be   violated   in
intermediate points - and in the solution.  However,  properly  configured
AUL method is significantly better at handling  constraints  than  barrier
and/or penalty methods.

The very basic outline of algorithm is given below:
1) first outer iteration is performed with "default"  values  of  Lagrange
   multipliers Nu1/Nu2. Solution quality is low (candidate  point  can  be
   too  far  away  from  true  solution; large violation of constraints is
   possible) and is comparable with that of penalty methods.
2) subsequent outer iterations  refine  Lagrange  multipliers  and improve
   quality of the solution.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    Rho     -   penalty coefficient, Rho>0:
                * large enough  that  algorithm  converges  with   desired
                  precision. Minimum value is 10*max(S'*diag(H)*S),  where
                  S is a scale matrix (set by MinNLCSetScale) and H  is  a
                  Hessian of the function being minimized. If you can  not
                  easily estimate Hessian norm,  see  our  recommendations
                  below.
                * not TOO large to prevent ill-conditioning
                * for unit-scale problems (variables and Hessian have unit
                  magnitude), Rho=100 or Rho=1000 can be used.
                * it is important to note that Rho is internally multiplied
                  by scaling matrix, i.e. optimum value of Rho depends  on
                  scale of variables specified  by  MinNLCSetScale().
    ItsCnt  -   number of outer iterations:
                * ItsCnt=0 means that small number of outer iterations  is
                  automatically chosen (10 iterations in current version).
                * ItsCnt=1 means that AUL algorithm performs just as usual
                  barrier method.
                * ItsCnt>1 means that  AUL  algorithm  performs  specified
                  number of outer iterations

HOW TO CHOOSE PARAMETERS

Nonlinear optimization is a tricky area and Augmented Lagrangian algorithm
is sometimes hard to tune. Good values of  Rho  and  ItsCnt  are  problem-
specific.  In  order  to  help  you   we   prepared   following   set   of
recommendations:

* for  unit-scale  problems  (variables  and Hessian have unit magnitude),
  Rho=100 or Rho=1000 can be used.

* start from  some  small  value of Rho and solve problem  with  just  one
  outer iteration (ItcCnt=1). In this case algorithm behaves like  penalty
  method. Increase Rho in 2x or 10x steps until you  see  that  one  outer
  iteration returns point which is "rough approximation to solution".

  It is very important to have Rho so  large  that  penalty  term  becomes
  constraining i.e. modified function becomes highly convex in constrained
  directions.

  From the other side, too large Rho may prevent you  from  converging  to
  the solution. You can diagnose it by studying number of inner iterations
  performed by algorithm: too few (5-10 on  1000-dimensional  problem)  or
  too many (orders of magnitude more than  dimensionality)  usually  means
  that Rho is too large.

* with just one outer iteration you  usually  have  low-quality  solution.
  Some constraints can be violated with very  large  margin,  while  other
  ones (which are NOT violated in the true solution) can push final  point
  too far in the inner area of the feasible set.

  For example, if you have constraint x0>=0 and true solution  x0=1,  then
  merely a presence of "x0>=0" will introduce a bias towards larger values
  of x0. Say, algorithm may stop at x0=1.5 instead of 1.0.

* after you found good Rho, you may increase number of  outer  iterations.
  ItsCnt=10 is a good value. Subsequent outer iteration will refine values
  of  Lagrange  multipliers.  Constraints  which  were  violated  will  be
  enforced, inactive constraints will be dropped (corresponding multipliers
  will be decreased). Ideally, you  should  see  10-1000x  improvement  in
  constraint handling (constraint violation is reduced).

* if  you  see  that  algorithm  converges  to  vicinity  of solution, but
  additional outer iterations do not refine solution,  it  may  mean  that
  algorithm is unstable - it wanders around true  solution,  but  can  not
  approach it. Sometimes algorithm may be stabilized by increasing Rho one
  more time, making it 5x or 10x larger.

SCALING OF CONSTRAINTS [IMPORTANT]

AUL optimizer scales   variables   according   to   scale   specified   by
MinNLCSetScale() function, so it can handle  problems  with  badly  scaled
variables (as long as we KNOW their scales).   However,  because  function
being optimized is a mix  of  original  function and  constraint-dependent
penalty  functions, it  is   important  to   rescale  both  variables  AND
constraints.

Say,  if  you  minimize f(x)=x^2 subject to 1000000*x>=0,  then  you  have
constraint whose scale is different from that of target  function (another
example is 0.000001*x>=0). It is also possible to have constraints   whose
scales  are   misaligned:   1000000*x0>=0, 0.000001*x1<=0.   Inappropriate
scaling may ruin convergence because minimizing x^2 subject to x>=0 is NOT
same as minimizing it subject to 1000000*x>=0.

Because we  know  coefficients  of  boundary/linear  constraints,  we  can
automatically rescale and normalize them. However,  there  is  no  way  to
automatically rescale nonlinear constraints Gi(x) and  Hi(x)  -  they  are
black boxes.

It means that YOU are the one who is  responsible  for  correct scaling of
nonlinear constraints  Gi(x)  and  Hi(x).  We  recommend  you  to  rescale
nonlinear constraints in such way that I-th component of dG/dX (or  dH/dx)
has magnitude approximately equal to 1/S[i] (where S  is  a  scale  set by
MinNLCSetScale() function).

WHAT IF IT DOES NOT CONVERGE?

It is possible that AUL algorithm fails to converge to precise  values  of
Lagrange multipliers. It stops somewhere around true solution, but candidate
point is still too far from solution, and some constraints  are  violated.
Such kind of failure is specific for Lagrangian algorithms -  technically,
they stop at some point, but this point is not constrained solution.

There are exist several reasons why algorithm may fail to converge:
a) too loose stopping criteria for inner iteration
b) degenerate, redundant constraints
c) target function has unconstrained extremum exactly at the  boundary  of
   some constraint
d) numerical noise in the target function

In all these cases algorithm is unstable - each outer iteration results in
large and almost random step which improves handling of some  constraints,
but violates other ones (ideally  outer iterations should form a  sequence
of progressively decreasing steps towards solution).

First reason possible is  that  too  loose  stopping  criteria  for  inner
iteration were specified. Augmented Lagrangian algorithm solves a sequence
of intermediate problems, and requries each of them to be solved with high
precision. Insufficient precision results in incorrect update of  Lagrange
multipliers.

Another reason is that you may have specified degenerate constraints: say,
some constraint was repeated twice. In most cases AUL algorithm gracefully
handles such situations, but sometimes it may spend too much time figuring
out subtle degeneracies in constraint matrix.

Third reason is tricky and hard to diagnose. Consider situation  when  you
minimize  f=x^2  subject to constraint x>=0.  Unconstrained   extremum  is
located  exactly  at  the  boundary  of  constrained  area.  In  this case
algorithm will tend to oscillate between negative  and  positive  x.  Each
time it stops at x<0 it "reinforces" constraint x>=0, and each time it  is
bounced to x>0 it "relaxes" constraint (and is  attracted  to  x<0).

Such situation  sometimes  happens  in  problems  with  hidden  symetries.
Algorithm  is  got  caught  in  a  loop with  Lagrange  multipliers  being
continuously increased/decreased. Luckily, such loop forms after at  least
three iterations, so this problem can be solved by  DECREASING  number  of
outer iterations down to 1-2 and increasing  penalty  coefficient  Rho  as
much as possible.

Final reason is numerical noise. AUL algorithm is robust against  moderate
noise (more robust than, say, active set methods),  but  large  noise  may
destabilize algorithm.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgoaul(minnlcstate &state, const double rho, const ae_int_t itscnt, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This   function  tells  MinNLC  optimizer  to  use  SLP (Successive Linear
Programming) algorithm for  nonlinearly  constrained   optimization.  This
algorithm  is  a  slight  modification  of  one  described  in  "A  Linear
programming-based optimization algorithm for solving nonlinear programming
problems" (2010) by Claus Still and Tapio Westerlund.

This solver is the slowest one in ALGLIB, it requires more target function
evaluations that SQP and AUL. However it is somewhat more robust in tricky
cases, so it can be used as a backup plan. We recommend to use  this  algo
when SQP/AUL do not work (does not return  the  solution  you  expect). If
trying different approach gives same  results,  then  MAYBE  something  is
wrong with your optimization problem.

Despite its name ("linear" = "first order method") this algorithm performs
steps similar to that of conjugate gradients method;  internally  it  uses
orthogonality/conjugacy requirement for subsequent steps  which  makes  it
closer to second order methods in terms of convergence speed.

Convergence is proved for the following case:
* function and constraints are continuously differentiable (C1 class)
* extended Mangasarian–Fromovitz constraint qualification  (EMFCQ)  holds;
  in the context of this algorithm EMFCQ  means  that  one  can,  for  any
  infeasible  point,  find  a  search  direction  such that the constraint
  infeasibilities are reduced.

This algorithm has following nice properties:
* no parameters to tune
* no convexity requirements for target function or constraints
* initial point can be infeasible
* algorithm respects box constraints in all intermediate points  (it  does
  not even evaluate function outside of box constrained area)
* once linear constraints are enforced, algorithm will not violate them
* no such guarantees can be provided for nonlinear constraints,  but  once
  nonlinear constraints are enforced, algorithm will try  to  respect them
  as much as possible
* numerical differentiation does not  violate  box  constraints  (although
  general linear and nonlinear ones can be violated during differentiation)
* from our experience, this algorithm is somewhat more  robust  in  really
  difficult cases

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

===== TRACING SLP SOLVER =================================================

SLP solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'SLP'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
                  It also prints OptGuard  integrity  checker  report when
                  nonsmoothness of target/constraints is suspected.
* 'SLP.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'SLP'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format
* 'SLP.PROBING' - to let algorithm insert additional function  evaluations
                  before line search  in  order  to  build  human-readable
                  chart of the raw  Lagrangian  (~40  additional  function
                  evaluations is performed for  each  line  search).  This
                  symbol also implicitly defines 'SLP'. Definition of this
                  symbol also automatically activates OptGuard  smoothness
                  monitor.
* 'OPTGUARD'    - for report of smoothness/continuity violations in target
                  and/or constraints. This kind of reporting is   included
                  in 'SLP', but it comes with lots of additional info.  If
                  you  need  just  smoothness  monitoring,   specify  this
                  setting.

                  NOTE: this tag merely directs  OptGuard  output  to  log
                        file. Even if you specify it, you  still  have  to
                        configure OptGuard  by calling minnlcoptguard...()
                        family of functions.

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related   overhead.  Specifying  'SLP.PROBING'  adds   even  larger
overhead due to additional function evaluations being performed.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("SLP,SLP.PROBING,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 02.04.2018 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgoslp(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This   function  tells  MinNLC  optimizer to use SQP (Successive Quadratic
Programming) algorithm for nonlinearly constrained optimization.

This algorithm needs order of magnitude (5x-10x) less function evaluations
than AUL solver, but has higher overhead because each  iteration  involves
solution of quadratic programming problem.

Convergence is proved for the following case:
* function and constraints are continuously differentiable (C1 class)

This algorithm has following nice properties:
* no parameters to tune
* no convexity requirements for target function or constraints
* initial point can be infeasible
* algorithm respects box constraints in all intermediate points  (it  does
  not even evaluate function outside of box constrained area)
* once linear constraints are enforced, algorithm will not violate them
* no such guarantees can be provided for nonlinear constraints,  but  once
  nonlinear constraints are enforced, algorithm will try  to  respect them
  as much as possible
* numerical differentiation does not  violate  box  constraints  (although
  general linear and nonlinear ones can be violated during differentiation)

We recommend this algorithm as a default option for medium-scale  problems
(less than thousand of variables) or problems with target  function  being
hard to evaluate.

For   large-scale  problems  or  ones  with very  cheap  target   function
AUL solver can be better option.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

===== INTERACTION WITH OPTGUARD ==========================================

OptGuard integrity  checker  allows us to catch problems  like  errors  in
gradients   and  discontinuity/nonsmoothness  of  the  target/constraints.
The latter kind of problems can be detected  by looking upon line searches
performed during optimization and searching for signs of nonsmoothness.

The problem with SQP is that it is too good for OptGuard to work - it does
not perform line searches. It typically  needs  1-2  function  evaluations
per step, and it is not enough for OptGuard to detect nonsmoothness.

So, if you suspect that your problem is  nonsmooth  and  if  you  want  to
confirm or deny it, we recommend you to either:
* use AUL or SLP solvers, which can detect nonsmoothness of the problem
* or, alternatively, activate 'SQP.PROBING' trace  tag  that  will  insert
  additional  function  evaluations (~40  per  line  step) that will  help
  OptGuard integrity checker to study properties of your problem

===== TRACING SQP SOLVER =================================================

SQP solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'SQP'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
                  It also prints OptGuard  integrity  checker  report when
                  nonsmoothness of target/constraints is suspected.
* 'SQP.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'SQP'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format
* 'SQP.PROBING' - to let algorithm insert additional function  evaluations
                  before line search  in  order  to  build  human-readable
                  chart of the raw  Lagrangian  (~40  additional  function
                  evaluations is performed for  each  line  search).  This
                  symbol  also  implicitly  defines  'SQP'  and  activates
                  OptGuard integrity checker which detects continuity  and
                  smoothness violations. An OptGuard log is printed at the
                  end of the file.

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related   overhead.  Specifying  'SQP.PROBING'  adds   even  larger
overhead due to additional function evaluations being performed.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("SQP,SQP.PROBING,PREC.F6", "path/to/trace.log")
>

  -- ALGLIB --
     Copyright 02.12.2019 by Bochkanov Sergey
*************************************************************************/
void minnlcsetalgosqp(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinNLCOptimize().

NOTE: algorithm passes two parameters to rep() callback  -  current  point
      and penalized function value at current point. Important -  function
      value which is returned is NOT function being minimized. It  is  sum
      of the value of the function being minimized - and penalty term.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minnlcsetxrep(minnlcstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minnlciteration(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to launch iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    fvec    -   callback which calculates function vector fi[]
                at given point x
    jac     -   callback which calculates function vector fi[]
                and Jacobian jac at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL


NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied Jacobian, and one which uses  only  function
   vector and numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   you should choose appropriate variant of MinNLCOptimize() -  one  which
   accepts function AND Jacobian or one which accepts ONLY function.

   Be careful to choose variant of MinNLCOptimize()  which  corresponds to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinNLCOptimize()   and  specific
   function used to create optimizer.


                     |         USER PASSED TO MinNLCOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinNLCCreateF()   |     works               FAILS
   MinNLCCreate()    |     FAILS               works

   Here "FAILS" denotes inappropriate combinations  of  optimizer creation
   function  and  MinNLCOptimize()  version.   Attemps   to    use    such
   combination will lead to exception. Either  you  did  not pass gradient
   when it WAS needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey


*************************************************************************/
void minnlcoptimize(minnlcstate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minnlcoptimize(minnlcstate &state,
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic gradient/Jacobian.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target  function  (constraints)  at  the
initial point (note: future versions may also perform check  at  the final
point) and compares numerical gradient/Jacobian with analytic one provided
by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which stores both gradients/Jacobians, and specific components highlighted
as suspicious by the OptGuard.

The primary OptGuard report can be retrieved with minnlcoptguardresults().

IMPORTANT: gradient check is a high-overhead option which  will  cost  you
           about 3*N additional function evaluations. In many cases it may
           cost as much as the rest of the optimization session.

           YOU SHOULD NOT USE IT IN THE PRODUCTION CODE UNLESS YOU WANT TO
           CHECK DERIVATIVES PROVIDED BY SOME THIRD PARTY.

NOTE: unlike previous incarnation of the gradient checking code,  OptGuard
      does NOT interrupt optimization even if it discovers bad gradient.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step used for numerical differentiation:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification
                    You should carefully choose TestStep. Value  which  is
                    too large (so large that  function  behavior  is  non-
                    cubic at this scale) will lead  to  false  alarms. Too
                    short step will result in rounding  errors  dominating
                    numerical derivative.

                    You may use different step for different parameters by
                    means of setting scale with minnlcsetscale().

=== EXPLANATION ==========================================================

In order to verify gradient algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point

  -- ALGLIB --
     Copyright 15.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcoptguardgradient(minnlcstate &state, const double teststep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates nonsmoothness monitoring  option  of
the  OptGuard  integrity  checker. Smoothness  monitor  silently  observes
solution process and tries to detect ill-posed problems, i.e. ones with:
a) discontinuous target function (non-C0) and/or constraints
b) nonsmooth     target function (non-C1) and/or constraints

Smoothness monitoring does NOT interrupt optimization  even if it suspects
that your problem is nonsmooth. It just sets corresponding  flags  in  the
OptGuard report which can be retrieved after optimization is over.

Smoothness monitoring is a moderate overhead option which often adds  less
than 1% to the optimizer running time. Thus, you can use it even for large
scale problems.

NOTE: OptGuard does  NOT  guarantee  that  it  will  always  detect  C0/C1
      continuity violations.

      First, minor errors are hard to  catch - say, a 0.0001 difference in
      the model values at two sides of the gap may be due to discontinuity
      of the model - or simply because the model has changed.

      Second, C1-violations  are  especially  difficult  to  detect  in  a
      noninvasive way. The optimizer usually  performs  very  short  steps
      near the nonsmoothness, and differentiation  usually   introduces  a
      lot of numerical noise.  It  is  hard  to  tell  whether  some  tiny
      discontinuity in the slope is due to real nonsmoothness or just  due
      to numerical noise alone.

      Our top priority was to avoid false positives, so in some rare cases
      minor errors may went unnoticed (however, in most cases they can  be
      spotted with restart from different initial point).

INPUT PARAMETERS:
    state   -   algorithm state
    level   -   monitoring level:
                * 0 - monitoring is disabled
                * 1 - noninvasive low-overhead monitoring; function values
                      and/or gradients are recorded, but OptGuard does not
                      try to perform additional evaluations  in  order  to
                      get more information about suspicious locations.
                      This kind of monitoring does not work well with  SQP
                      because SQP solver needs just 1-2 function evaluations
                      per step, which is not enough for OptGuard  to  make
                      any conclusions.

=== EXPLANATION ==========================================================

One major source of headache during optimization  is  the  possibility  of
the coding errors in the target function/constraints (or their gradients).
Such  errors   most   often   manifest   themselves  as  discontinuity  or
nonsmoothness of the target/constraints.

Another frequent situation is when you try to optimize something involving
lots of min() and max() operations, i.e. nonsmooth target. Although not  a
coding error, it is nonsmoothness anyway - and smooth  optimizers  usually
stop right after encountering nonsmoothness, well before reaching solution.

OptGuard integrity checker helps you to catch such situations: it monitors
function values/gradients being passed  to  the  optimizer  and  tries  to
errors. Upon discovering suspicious pair of points it  raises  appropriate
flag (and allows you to continue optimization). When optimization is done,
you can study OptGuard result.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minnlcoptguardsmoothness(minnlcstate &state, const ae_int_t level, const xparams _xparams = alglib::xdefault);
void minnlcoptguardsmoothness(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Results of OptGuard integrity check, should be called  after  optimization
session is over.

=== PRIMARY REPORT =======================================================

OptGuard performs several checks which are intended to catch common errors
in the implementation of nonlinear function/gradient:
* incorrect analytic gradient
* discontinuous (non-C0) target functions (constraints)
* nonsmooth     (non-C1) target functions (constraints)

Each of these checks is activated with appropriate function:
* minnlcoptguardgradient() for gradient verification
* minnlcoptguardsmoothness() for C0/C1 checks

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradfidx for specific function (Jacobian row) suspected
  * rep.badgradvidx for specific variable (Jacobian column) suspected
  * rep.badgradxbase, a point where gradient/Jacobian is tested
  * rep.badgraduser, user-provided gradient/Jacobian
  * rep.badgradnum, reference gradient/Jacobian obtained via numerical
    differentiation
* rep.nonc0suspected, and additionally:
  * rep.nonc0fidx - an index of specific function violating C0 continuity
* rep.nonc1suspected, and additionally
  * rep.nonc1fidx - an index of specific function violating C1 continuity
Here function index 0 means  target function, index 1  or  higher  denotes
nonlinear constraints.

=== ADDITIONAL REPORTS/LOGS ==============================================

Several different tests are performed to catch C0/C1 errors, you can  find
out specific test signaled error by looking to:
* rep.nonc0test0positive, for non-C0 test #0
* rep.nonc1test0positive, for non-C1 test #0
* rep.nonc1test1positive, for non-C1 test #1

Additional information (including line search logs)  can  be  obtained  by
means of:
* minnlcoptguardnonc1test0results()
* minnlcoptguardnonc1test1results()
which return detailed error reports, specific points where discontinuities
were found, and so on.

==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    rep     -   generic OptGuard report;  more  detailed  reports  can  be
                retrieved with other functions.

NOTE: false negatives (nonsmooth problems are not identified as  nonsmooth
      ones) are possible although unlikely.

      The reason  is  that  you  need  to  make several evaluations around
      nonsmoothness  in  order  to  accumulate  enough  information  about
      function curvature. Say, if you start right from the nonsmooth point,
      optimizer simply won't get enough data to understand what  is  going
      wrong before it terminates due to abrupt changes in the  derivative.
      It is also  possible  that  "unlucky"  step  will  move  us  to  the
      termination too quickly.

      Our current approach is to have less than 0.1%  false  negatives  in
      our test examples  (measured  with  multiple  restarts  from  random
      points), and to have exactly 0% false positives.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minnlcoptguardresults(minnlcstate &state, optguardreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #0

Nonsmoothness (non-C1) test #0 studies  function  values  (not  gradient!)
obtained during line searches and monitors  behavior  of  the  directional
derivative estimate.

This test is less powerful than test #1, but it does  not  depend  on  the
gradient values and thus it is more robust against artifacts introduced by
numerical differentiation.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #0 "strong" report
    lngrep  -   C1 test #0 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minnlcoptguardnonc1test0results(const minnlcstate &state, optguardnonc1test0report &strrep, optguardnonc1test0report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #1

Nonsmoothness (non-C1)  test  #1  studies  individual  components  of  the
gradient computed during line search.

When precise analytic gradient is provided this test is more powerful than
test #0  which  works  with  function  values  and  ignores  user-provided
gradient.  However,  test  #0  becomes  more   powerful   when   numerical
differentiation is employed (in such cases test #1 detects  higher  levels
of numerical noise and becomes too conservative).

This test also tells specific components of the gradient which violate  C1
continuity, which makes it more informative than #0, which just tells that
continuity is violated.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* fidx - is an index of the function (0 for  target  function, 1 or higher
  for nonlinear constraints) which is suspected of being "non-C1"
* vidx - is an index of the variable in [0,N) with nonsmooth derivative
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], g[] - arrays of length CNT which store step lengths and  gradient
  values at these points; g[i] is evaluated in  x0+stp[i]*d  and  contains
  vidx-th component of the gradient.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #1 "strong" report
    lngrep  -   C1 test #1 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minnlcoptguardnonc1test1results(minnlcstate &state, optguardnonc1test1report &strrep, optguardnonc1test1report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
MinNLC results:  the  solution  found,  completion  codes  and  additional
information.

If you activated OptGuard integrity checking functionality and want to get
OptGuard report, it can be retrieved with:
* minnlcoptguardresults() - for a primary report about (a) suspected C0/C1
  continuity violations and (b) errors in the analytic gradient.
* minnlcoptguardnonc1test0results() - for C1 continuity violation test #0,
  detailed line search log
* minnlcoptguardnonc1test1results() - for C1 continuity violation test #1,
  detailed line search log

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report, contains information about completion
                code, constraint violation at the solution and so on.

                You   should   check   rep.terminationtype  in  order   to
                distinguish successful termination from unsuccessful one:

                === FAILURE CODES ===
                * -8    internal  integrity control  detected  infinite or
                        NAN   values    in   function/gradient.   Abnormal
                        termination signalled.
                * -3    box  constraints are infeasible.
                        Note: infeasibility of  non-box  constraints  does
                              NOT trigger emergency completion;  you  have
                              to examine rep.bcerr/rep.lcerr/rep.nlcerr to
                              detect possibly inconsistent constraints.

                === SUCCESS CODES ===
                *  2   scaled step is no more than EpsX.
                *  5   MaxIts steps were taken.
                *  8   user   requested    algorithm    termination    via
                       minnlcrequesttermination(), last accepted point  is
                       returned.

                More information about fields of this  structure  can  be
                found in the comments on minnlcreport datatype.

  -- ALGLIB --
     Copyright 06.06.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcresults(const minnlcstate &state, real_1d_array &x, minnlcreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
NLC results

Buffered implementation of MinNLCResults() which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minnlcresultsbuf(const minnlcstate &state, real_1d_array &x, minnlcreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void minnlcrequesttermination(minnlcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine restarts algorithm from new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinNLCCreate call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minnlcrestartfrom(minnlcstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_MONBI) || !defined(AE_PARTIAL_BUILD)

#endif

#if defined(AE_COMPILE_MINMO) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                    MULTI-OBJECTIVE  OPTIMIZATION

DESCRIPTION:

The  solver  minimizes an M-dimensional vector function F(x) of N arguments
subject to any combination of:
* box constraints
* two-sided linear equality/inequality constraints AL<=A*x<=AU, where some
  of AL/AU can be infinite (i.e. missing)
* two-sided nonlinear equality/inequality constraints NL<=C(x)<=NU,  where
  some of NL/NU can be infinite (i.e. missing)

REQUIREMENTS:
* F(), C() are continuously differentiable on the feasible set and on  its
  neighborhood

USAGE:

1. User initializes algorithm state using either:
   * minmocreate()  to perform optimization with user-supplied Jacobian
   * minmocreatef() to perform optimization with numerical differentiation

2. User chooses which multi-objective solver to use. At the present moment
   only NBI (Normal Boundary Intersection) solver is implemented, which is
   activated by calling minmosetalgonbi().

3. User adds boundary and/or linear and/or nonlinear constraints by  means
   of calling one of the following functions:
   a) minmosetbc() for boundary constraints
   b) minmosetlc2()      for two-sided sparse linear constraints;
      minmosetlc2dense() for two-sided dense  linear constraints;
      minmosetlc2mixed() for two-sided mixed sparse/dense constraints
   c) minmosetnlc2()     for two-sided nonlinear constraints
   You may combine (a), (b) and (c) in one optimization problem.

4. User sets scale of the variables with minmosetscale() function.  It  is
   VERY important to set  scale  of  the  variables,  because  nonlinearly
   constrained problems are hard to solve when variables are badly scaled.

5. User sets  stopping  conditions  with  minmosetcond().

6. Finally, user calls minmooptimize()   function  which  takes  algorithm
   state  and  pointers  (delegate, etc.) to the callback functions  which
   calculate F/C

7. User calls  minmoresults()  to  get the solution

8. Optionally user may call minmorestartfrom() to solve  another   problem
   with same M,N but another starting point. minmorestartfrom() allows  to
   reuse an already initialized optimizer structure.


INPUT PARAMETERS:
    N       -   variables count, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from the size of X
    M       -   objectives count, M>0.
                M=1 is possible, although makes little sense - it is better
                to use MinNLC directly.
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to reinforce feasibility during  initial  stages  of  the
                  optimization

OUTPUT PARAMETERS:
    State   -   structure that stores algorithm state

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmocreate(const ae_int_t n, const ae_int_t m, const real_1d_array &x, minmostate &state, const xparams _xparams = alglib::xdefault);
void minmocreate(const ae_int_t m, const real_1d_array &x, minmostate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine is a finite  difference variant of minmocreate().  It uses
finite differences in order to differentiate target function.

Description below contains information which is specific to this  function
only. We recommend to read comments on minmocreate() too.

INPUT PARAMETERS:
    N       -   variables count, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from the size of X
    M       -   objectives count, M>0.
                M=1 is possible, although makes little sense - it is better
                to use MinNLC directly.
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to reinforce feasibility during  initial  stages  of  the
                  optimization
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure that stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is a scaling vector which can be set by minmosetscale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step means too large TRUNCATION errors,  whilst  too  small  step
   means too large NUMERICAL errors.
   1.0E-4 can be good value to start from for a unit-scaled problem.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less   robust   and  precise.  Imprecise  gradient  may  slow  down
   convergence, especially on highly nonlinear problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmocreatef(const ae_int_t n, const ae_int_t m, const real_1d_array &x, const double diffstep, minmostate &state, const xparams _xparams = alglib::xdefault);
void minmocreatef(const ae_int_t m, const real_1d_array &x, const double diffstep, minmostate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Use the NBI (Normal Boundary Intersection)  algorithm  for  multiobjective
optimization.

NBI is a simple yet powerful multiobjective  optimization  algorithm  that
has the following attractive properties:
* it generates nearly uniformly distributed Pareto points
* it is applicable to problems with more than 2 objectives
* it naturally supports a mix of box, linear and nonlinear constraints
* it is less sensitive to the bad scaling of the targets

The only drawback of the algorithm is that for more than 2  objectives  it
can miss some small parts of the Pareto front that are  located  near  its
boundaries.

INPUT PARAMETERS:
    State       -   structure which stores algorithm state
    FrontSize   -   desired Pareto front size, FrontSize>=M,
                    where M is an objectives count
    PolishSolutions-whether additional solution improving phase is needed
                    or not:
                    * if False, the original NBI as formulated  by Das and
                      Dennis is used. It quickly produces  good solutions,
                      but these solutions can be suboptimal (usually within
                      0.1% of the optimal values).
                      The reason is that the original NBI formulation does
                      not account for  degeneracies that allow significant
                      progress for one objective with no deterioration for
                      other objectives.
                    * if True,  the  original  NBI  is  followed  by   the
                      additional solution  polishing  phase.  This  solver
                      mode is several times slower than the original  NBI,
                      but produces better solutions.

  -- ALGLIB --
     Copyright 20.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetalgonbi(minmostate &state, const ae_int_t frontsize, const bool polishsolutions, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets boundary constraints for the MO optimizer.

Boundary constraints are inactive by  default  (after  initial  creation).
They are preserved after algorithm restart with MinMORestartFrom().

You may combine boundary constraints with  general  linear ones - and with
nonlinear ones! Boundary constraints are  handled  more  efficiently  than
other types.  Thus,  if  your  problem  has  mixed  constraints,  you  may
explicitly specify some of them as boundary and save some time/space.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                a very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                a very large number or +INF.

NOTE 1:  it is possible to specify  BndL[i]=BndU[i].  In  this  case  I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetbc(minmostate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets two-sided linear constraints AL <= A*x <= AU with dense
constraint matrix A.

NOTE: knowing  that  constraint  matrix  is dense may help some MO solvers
      to utilize efficient dense Level 3  BLAS  for  dense  parts  of  the
      problem. If your problem has both dense and sparse constraints,  you
      can use minmosetlc2mixed() function.

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
    A       -   linear constraints, array[K,N]. Each row of  A  represents
                one  constraint. One-sided  inequality   constraints, two-
                sided inequality  constraints,  equality  constraints  are
                supported (see below)
    AL, AU  -   lower and upper bounds, array[K];
                * AL[i]=AU[i] => equality constraint Ai*x
                * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                * AL[i]=-INF, AU[i]=+INF => constraint is ignored
    K       -   number of equality/inequality constraints,  K>=0;  if  not
                given, inferred from sizes of A, AL, AU.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetlc2dense(minmostate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minmosetlc2dense(minmostate &state, const real_2d_array &a, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
sparse constraining matrix A. Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
    A       -   sparse matrix with size [K,N] (exactly!).
                Each row of A represents one general linear constraint.
                A can be stored in any sparse storage format.
    AL, AU  -   lower and upper bounds, array[K];
                * AL[i]=AU[i] => equality constraint Ai*x
                * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                * AL[i]=-INF, AU[i]=+INF => constraint is ignored
    K       -   number  of equality/inequality constraints, K>=0.  If  K=0
                is specified, A, AL, AU are ignored.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minmosetlc2(minmostate &state, const sparsematrix &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t k, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  sets  two-sided linear  constraints  AL <= A*x <= AU  with
mixed constraining matrix A including sparse part (first SparseK rows) and
dense part (last DenseK rows). Recommended for large-scale problems.

This  function  overwrites  linear  (non-box)  constraints set by previous
calls (if such calls were made).

This function may be useful if constraint matrix includes large number  of
both types of rows - dense and sparse. If you have just a few sparse rows,
you  may  represent  them  in  dense  format  without losing  performance.
Similarly, if you have just a few dense rows,  you  can  store them in the
sparse format with almost same performance.

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
    SparseA -   sparse matrix with size [K,N] (exactly!).
                Each row of A represents one general linear constraint.
                A can be stored in any sparse storage format.
    SparseK -   number of sparse constraints, SparseK>=0
    DenseA  -   linear constraints, array[K,N], set of dense constraints.
                Each row of A represents one general linear constraint.
    DenseK  -   number of dense constraints, DenseK>=0
    AL, AU  -   lower and upper bounds, array[SparseK+DenseK], with former
                SparseK elements corresponding to sparse constraints,  and
                latter DenseK elements corresponding to dense constraints;
                * AL[i]=AU[i] => equality constraint Ai*x
                * AL[i]<AU[i] => two-sided constraint AL[i]<=Ai*x<=AU[i]
                * AL[i]=-INF  => one-sided constraint Ai*x<=AU[i]
                * AU[i]=+INF  => one-sided constraint AL[i]<=Ai*x
                * AL[i]=-INF, AU[i]=+INF => constraint is ignored
    K       -   number  of equality/inequality constraints, K>=0.  If  K=0
                is specified, A, AL, AU are ignored.

  -- ALGLIB --
     Copyright 01.11.2019 by Bochkanov Sergey
*************************************************************************/
void minmosetlc2mixed(minmostate &state, const sparsematrix &sparsea, const ae_int_t ksparse, const real_2d_array &densea, const ae_int_t kdense, const real_1d_array &al, const real_1d_array &au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear  constraint  AL<=A*x<=AU  to  dense
constraints list.

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
    A       -   linear constraint coefficient, array[N], right side is NOT
                included.
    AL, AU  -   lower and upper bounds;
                * AL=AU    => equality constraint Ai*x
                * AL<AU    => two-sided constraint AL<=A*x<=AU
                * AL=-INF  => one-sided constraint Ai*x<=AU
                * AU=+INF  => one-sided constraint AL<=Ai*x
                * AL=-INF, AU=+INF => constraint is ignored

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minmoaddlc2dense(minmostate &state, const real_1d_array &a, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of sparse constraints.

Constraint is passed in the compressed  format:  as  a  list  of  non-zero
entries of the coefficient vector A. Such approach is more efficient  than
the dense storage for highly sparse constraint vectors.

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
    IdxA    -   array[NNZ], indexes of non-zero elements of A:
                * can be unsorted
                * can include duplicate indexes (corresponding entries  of
                  ValA[] will be summed)
    ValA    -   array[NNZ], values of non-zero elements of A
    NNZ     -   number of non-zero coefficients in A
    AL, AU  -   lower and upper bounds;
                * AL=AU    => equality constraint A*x
                * AL<AU    => two-sided constraint AL<=A*x<=AU
                * AL=-INF  => one-sided constraint A*x<=AU
                * AU=+INF  => one-sided constraint AL<=A*x
                * AL=-INF, AU=+INF => constraint is ignored

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minmoaddlc2(minmostate &state, const integer_1d_array &idxa, const real_1d_array &vala, const ae_int_t nnz, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function appends two-sided linear constraint  AL <= A*x <= AU  to the
list of currently present sparse constraints.

Constraint vector A is  passed  as  a  dense  array  which  is  internally
sparsified by this function.

INPUT PARAMETERS:
    State   -   structure previously allocated with minmocreate() call.
    DA      -   array[N], constraint vector
    AL, AU  -   lower and upper bounds;
                * AL=AU    => equality constraint A*x
                * AL<AU    => two-sided constraint AL<=A*x<=AU
                * AL=-INF  => one-sided constraint A*x<=AU
                * AU=+INF  => one-sided constraint AL<=A*x
                * AL=-INF, AU=+INF => constraint is ignored

  -- ALGLIB --
     Copyright 19.07.2018 by Bochkanov Sergey
*************************************************************************/
void minmoaddlc2sparsefromdense(minmostate &state, const real_1d_array &da, const double al, const double au, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets two-sided nonlinear constraints for MinMO optimizer.

In fact, this function sets only  NUMBER  of  the  nonlinear  constraints.
Constraints  themselves  (constraint  functions)   are   passed   to   the
MinMOOptimize() method.

This method accepts user-defined vector function F[] and its Jacobian J[],
where:
* first M components of F[] and first M rows  of  J[]  correspond  to  the
  multiple objectives
* subsequent NNLC components of F[] (and rows of J[])  correspond  to  the
  two-sided nonlinear constraints NL<=C(x)<=NU, where
  * NL[i]=NU[i] => I-th row is an equality constraint Ci(x)=NL
  * NL[i]<NU[i] => I-th tow is a  two-sided constraint NL[i]<=Ci(x)<=NU[i]
  * NL[i]=-INF  => I-th row is an one-sided constraint Ci(x)<=NU[i]
  * NU[i]=+INF  => I-th row is an one-sided constraint NL[i]<=Ci(x)
  * NL[i]=-INF, NU[i]=+INF => constraint is ignored

NOTE: you may combine nonlinear constraints with linear/boundary ones.  If
      your problem has mixed constraints, you  may explicitly specify some
      of them as linear or box ones.
      It helps optimizer to handle them more efficiently.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinMOCreate call.
    NL      -   array[NNLC], lower bounds, can contain -INF
    NU      -   array[NNLC], lower bounds, can contain +INF
    NNLC    -   constraints count, NNLC>=0

NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
        possible   that  algorithm  will  evaluate  function  outside   of
        feasible area!

NOTE 2: algorithm scales variables  according  to the scale  specified by
        MinMOSetScale()  function,  so  it can handle problems with badly
        scaled variables (as long as we KNOW their scales).

        However,  there  is  no  way  to  automatically  scale   nonlinear
        constraints. Inappropriate scaling  of nonlinear  constraints  may
        ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
        is NOT the same as solving it with constraint "0.001*G0(x)=0".

        It means that YOU are  the  one who is responsible for the correct
        scaling of the nonlinear constraints Gi(x) and Hi(x). We recommend
        you to scale nonlinear constraints in such a way that the Jacobian
        rows have approximately unit magnitude  (for  problems  with  unit
        scale) or have magnitude approximately equal to 1/S[i] (where S is
        a scale set by MinMOSetScale() function).

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetnlc2(minmostate &state, const real_1d_array &nl, const real_1d_array &nu, const ae_int_t nnlc, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for inner iterations of the optimizer.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinMOSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsX=0 and MaxIts=0 (simultaneously) will lead to an automatic
selection of the stopping condition.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetcond(minmostate &state, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for the MO optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetscale(minmostate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting of the Pareto front points.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback  function  (if  it
was provided to MinMOOptimize) every time we find a Pareto front point.

NOTE: according to the communication protocol used by ALGLIB,  the  solver
      passes two parameters to the rep() callback - a current point and  a
      target value at the current point.
      However, because  we solve a  multi-objective  problem,  the  target
      parameter is not used and set to zero.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmosetxrep(minmostate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minmoiteration(minmostate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to launch iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    fvec    -   callback which calculates function vector fi[]
                at given point x
    jac     -   callback which calculates function vector fi[]
                and Jacobian jac at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL


NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied Jacobian, and one which uses  only  function
   vector and numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   you should choose appropriate variant of MinMOOptimize() -  one   which
   needs function vector AND Jacobian or one which needs ONLY function.

   Be careful to choose variant of MinMOOptimize()  which  corresponds  to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinMOOptimize()   and   specific
   function used to create optimizer.


                     |         USER PASSED TO MinMOOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinMOCreateF()    |     works               FAILS
   MinMOCreate()     |     FAILS               works

   Here "FAILS" denotes inappropriate combinations  of  optimizer creation
   function  and  MinMOOptimize()  version.   Attemps   to    use     such
   combination will lead to exception. Either  you  did  not pass gradient
   when it WAS needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey


*************************************************************************/
void minmooptimize(minmostate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minmooptimize(minmostate &state,
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
MinMO results:  the  solution  found,  completion  codes  and   additional
information.

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    ParetoFront-array[FrontSize,N+M], approximate Pareto front.
                Its columns have the following structure:
                * first N columns are variable values
                * next  M columns are objectives at these points
                Its rows have the following structure:
                * first M rows contain solutions to single-objective tasks
                  with I-th row storing result for  I-th  objective  being
                  minimized ignoring other ones.
                  Thus, ParetoFront[I,N+I] for  0<=I<M  stores  so  called
                  'ideal objective vector'.
                * subsequent FrontSize-M rows  store  variables/objectives
                  at  various  randomly  and  nearly   uniformly   sampled
                  locations of the Pareto front.

    FrontSize-  front size, >=0.
                * no larger than the number passed to setalgo()
                * for  a  single-objective  task,  FrontSize=1  is  ALWAYS
                  returned, no matter what was specified during setalgo()
                  call.
                * if  the   solver   was   prematurely   terminated   with
                  minnorequesttermination(), an  incomplete  Pareto  front
                  will be returned (it may even have less than M rows)
                * if a  failure (negative completion code) was   signaled,
                  FrontSize=0 will be returned

    Rep     -   optimization report, contains information about completion
                code, constraint violation at the solution and so on.

                You   should   check   rep.terminationtype  in  order   to
                distinguish successful termination from unsuccessful one:

                === FAILURE CODES ===
                * -8    internal  integrity control  detected  infinite or
                        NAN   values    in   function/gradient.   Abnormal
                        termination signalled.
                * -3    constraint bounds are  infeasible,  i.e.  we  have
                        box/linear/nonlinear constraint  with  two  bounds
                        present, and a lower one being  greater  than  the
                        upper one.
                        Note: less obvious infeasibilities of  constraints
                              do NOT  trigger  emergency  completion;  you
                              have to examine rep.bcerr/rep.lcerr/rep.nlcerr
                              to detect possibly inconsistent constraints.

                === SUCCESS CODES ===
                *  2   scaled step is no more than EpsX.
                *  5   MaxIts steps were taken.
                *  8   user   requested    algorithm    termination    via
                       minmorequesttermination(), last accepted point   is
                       returned.

                More information about fields of this  structure  can  be
                found in the comments on minmoreport datatype.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmoresults(const minmostate &state, real_2d_array &paretofront, ae_int_t &frontsize, minmoreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine  submits  request  for  the  termination  of  the  running
optimizer.

It should be called from the user-supplied callback when user decides that
it is time to "smoothly" terminate optimization process, or from some other
thread. As a result, optimizer stops  at  the  state  which  was  "current
accepted" when termination request was submitted and returns error code  8
(successful termination).

Usually it results in an incomplete Pareto front being returned.

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmorequesttermination(minmostate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine restarts algorithm from the new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinMOCreate call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 01.03.2023 by Bochkanov Sergey
*************************************************************************/
void minmorestartfrom(minmostate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_MINNS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                  NONSMOOTH NONCONVEX OPTIMIZATION
            SUBJECT TO BOX/LINEAR/NONLINEAR-NONSMOOTH CONSTRAINTS

DESCRIPTION:

The  subroutine  minimizes  function   F(x)  of N arguments subject to any
combination of:
* bound constraints
* linear inequality constraints
* linear equality constraints
* nonlinear equality constraints Gi(x)=0
* nonlinear inequality constraints Hi(x)<=0

IMPORTANT: see MinNSSetAlgoAGS for important  information  on  performance
           restrictions of AGS solver.

REQUIREMENTS:
* starting point X0 must be feasible or not too far away from the feasible
  set
* F(), G(), H() are continuous, locally Lipschitz  and  continuously  (but
  not necessarily twice) differentiable in an open dense  subset  of  R^N.
  Functions F(), G() and H() may be nonsmooth and non-convex.
  Informally speaking, it means  that  functions  are  composed  of  large
  differentiable "patches" with nonsmoothness having  place  only  at  the
  boundaries between these "patches".
  Most real-life nonsmooth  functions  satisfy  these  requirements.  Say,
  anything which involves finite number of abs(), min() and max() is  very
  likely to pass the test.
  Say, it is possible to optimize anything of the following:
  * f=abs(x0)+2*abs(x1)
  * f=max(x0,x1)
  * f=sin(max(x0,x1)+abs(x2))
* for nonlinearly constrained problems: F()  must  be  bounded from  below
  without nonlinear constraints (this requirement is due to the fact that,
  contrary to box and linear constraints, nonlinear ones  require  special
  handling).
* user must provide function value and gradient for F(), H(), G()  at  all
  points where function/gradient can be calculated. If optimizer  requires
  value exactly at the boundary between "patches" (say, at x=0 for f=abs(x)),
  where gradient is not defined, user may resolve tie arbitrarily (in  our
  case - return +1 or -1 at its discretion).
* NS solver supports numerical differentiation, i.e. it may  differentiate
  your function for you,  but  it  results  in  2N  increase  of  function
  evaluations. Not recommended unless you solve really small problems. See
  minnscreatef() for more information on this functionality.

USAGE:

1. User initializes algorithm state with MinNSCreate() call  and   chooses
   what NLC solver to use. There is some solver which is used by  default,
   with default settings, but you should NOT rely on  default  choice.  It
   may change in future releases of ALGLIB without notice, and no one  can
   guarantee that new solver will be  able  to  solve  your  problem  with
   default settings.

   From the other side, if you choose solver explicitly, you can be pretty
   sure that it will work with new ALGLIB releases.

   In the current release following solvers can be used:
   * AGS solver (activated with MinNSSetAlgoAGS() function)

2. User adds boundary and/or linear and/or nonlinear constraints by  means
   of calling one of the following functions:
   a) MinNSSetBC() for boundary constraints
   b) MinNSSetLC() for linear constraints
   c) MinNSSetNLC() for nonlinear constraints
   You may combine (a), (b) and (c) in one optimization problem.

3. User sets scale of the variables with MinNSSetScale() function. It   is
   VERY important to set  scale  of  the  variables,  because  nonlinearly
   constrained problems are hard to solve when variables are badly scaled.

4. User sets stopping conditions with MinNSSetCond().

5. Finally, user calls MinNSOptimize()  function  which  takes   algorithm
   state and pointer (delegate, etc) to callback function which calculates
   F/G/H.

7. User calls MinNSResults() to get solution

8. Optionally user may call MinNSRestartFrom() to solve   another  problem
   with same N but another starting point. MinNSRestartFrom()  allows   to
   reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

NOTE: minnscreatef() function may be used if  you  do  not  have  analytic
      gradient.   This   function  creates  solver  which  uses  numerical
      differentiation with user-specified step.

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnscreate(const ae_int_t n, const real_1d_array &x, minnsstate &state, const xparams _xparams = alglib::xdefault);
void minnscreate(const real_1d_array &x, minnsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Version of minnscreatef() which uses numerical differentiation. I.e.,  you
do not have to calculate derivatives yourself. However, this version needs
2N times more function evaluations.

2-point differentiation formula is  used,  because  more  precise  4-point
formula is unstable when used on non-smooth functions.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.
    DiffStep-   differentiation  step,  DiffStep>0.   Algorithm   performs
                numerical differentiation  with  step  for  I-th  variable
                being equal to DiffStep*S[I] (here S[] is a  scale vector,
                set by minnssetscale() function).
                Do not use  too  small  steps,  because  it  may  lead  to
                catastrophic cancellation during intermediate calculations.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnscreatef(const ae_int_t n, const real_1d_array &x, const double diffstep, minnsstate &state, const xparams _xparams = alglib::xdefault);
void minnscreatef(const real_1d_array &x, const double diffstep, minnsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets boundary constraints.

Boundary constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with minnsrestartfrom().

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: AGS solver has following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by  bound  constraints,
  even  when  numerical  differentiation is used (algorithm adjusts  nodes
  according to boundary constraints)

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnssetbc(minnsstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets linear constraints.

Linear constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with minnsrestartfrom().

INPUT PARAMETERS:
    State   -   structure previously allocated with minnscreate() call.
    C       -   linear constraints, array[K,N+1].
                Each row of C represents one constraint, either equality
                or inequality (see below):
                * first N elements correspond to coefficients,
                * last element corresponds to the right part.
                All elements of C (including right part) must be finite.
    CT      -   type of constraints, array[K]:
                * if CT[i]>0, then I-th constraint is C[i,*]*x >= C[i,n+1]
                * if CT[i]=0, then I-th constraint is C[i,*]*x  = C[i,n+1]
                * if CT[i]<0, then I-th constraint is C[i,*]*x <= C[i,n+1]
    K       -   number of equality/inequality constraints, K>=0:
                * if given, only leading K elements of C/CT are used
                * if not given, automatically determined from sizes of C/CT

NOTE: linear (non-bound) constraints are satisfied only approximately:

* there always exists some minor violation (about current sampling  radius
  in magnitude during optimization, about EpsX in the solution) due to use
  of penalty method to handle constraints.
* numerical differentiation, if used, may  lead  to  function  evaluations
  outside  of the feasible  area,   because   algorithm  does  NOT  change
  numerical differentiation formula according to linear constraints.

If you want constraints to be  satisfied  exactly, try to reformulate your
problem  in  such  manner  that  all constraints will become boundary ones
(this kind of constraints is always satisfied exactly, both in  the  final
solution and in all intermediate points).

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnssetlc(minnsstate &state, const real_2d_array &c, const integer_1d_array &ct, const ae_int_t k, const xparams _xparams = alglib::xdefault);
void minnssetlc(minnsstate &state, const real_2d_array &c, const integer_1d_array &ct, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets nonlinear constraints.

In fact, this function sets NUMBER of nonlinear  constraints.  Constraints
itself (constraint functions) are passed to minnsoptimize() method.   This
method requires user-defined vector function F[]  and  its  Jacobian  J[],
where:
* first component of F[] and first row  of  Jacobian  J[]  correspond   to
  function being minimized
* next NLEC components of F[] (and rows  of  J)  correspond  to  nonlinear
  equality constraints G_i(x)=0
* next NLIC components of F[] (and rows  of  J)  correspond  to  nonlinear
  inequality constraints H_i(x)<=0

NOTE: you may combine nonlinear constraints with linear/boundary ones.  If
      your problem has mixed constraints, you  may explicitly specify some
      of them as linear ones. It may help optimizer to  handle  them  more
      efficiently.

INPUT PARAMETERS:
    State   -   structure previously allocated with minnscreate() call.
    NLEC    -   number of Non-Linear Equality Constraints (NLEC), >=0
    NLIC    -   number of Non-Linear Inquality Constraints (NLIC), >=0

NOTE 1: nonlinear constraints are satisfied only  approximately!   It   is
        possible   that  algorithm  will  evaluate  function  outside   of
        the feasible area!

NOTE 2: algorithm scales variables  according  to   scale   specified   by
        minnssetscale()  function,  so  it can handle problems with  badly
        scaled variables (as long as we KNOW their scales).

        However,  there  is  no  way  to  automatically  scale   nonlinear
        constraints Gi(x) and Hi(x). Inappropriate scaling  of  Gi/Hi  may
        ruin convergence. Solving problem with  constraint  "1000*G0(x)=0"
        is NOT same as solving it with constraint "0.001*G0(x)=0".

        It  means  that  YOU  are  the  one who is responsible for correct
        scaling of nonlinear constraints Gi(x) and Hi(x). We recommend you
        to scale nonlinear constraints in such way that I-th component  of
        dG/dX (or dH/dx) has approximately unit  magnitude  (for  problems
        with unit scale)  or  has  magnitude approximately equal to 1/S[i]
        (where S is a scale set by minnssetscale() function).

NOTE 3: nonlinear constraints are always hard to handle,  no  matter  what
        algorithm you try to use. Even basic box/linear constraints modify
        function  curvature   by  adding   valleys  and  ridges.  However,
        nonlinear constraints add valleys which are very  hard  to  follow
        due to their "curved" nature.

        It means that optimization with single nonlinear constraint may be
        significantly slower than optimization with multiple linear  ones.
        It is normal situation, and we recommend you to  carefully  choose
        Rho parameter of minnssetalgoags(), because too  large  value  may
        slow down convergence.


  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnssetnlc(minnsstate &state, const ae_int_t nlec, const ae_int_t nlic, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for iterations of optimizer.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsX    -   >=0
                The AGS solver finishes its work if  on  k+1-th  iteration
                sampling radius decreases below EpsX.
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsX=0  and  MaxIts=0  (simultaneously)  will  lead  to  automatic
stopping criterion selection. We do not recommend you to rely  on  default
choice in production code.

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnssetcond(minnsstate &state, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for NLC optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnssetscale(minnsstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function tells MinNS unit to use  AGS  (adaptive  gradient  sampling)
algorithm for nonsmooth constrained  optimization.  This  algorithm  is  a
slight modification of one described in  "An  Adaptive  Gradient  Sampling
Algorithm for Nonsmooth Optimization" by Frank E. Curtisy and Xiaocun Quez.

This optimizer has following benefits and drawbacks:
+ robustness; it can be used with nonsmooth and nonconvex functions.
+ relatively easy tuning; most of the metaparameters are easy to select.
- it has convergence of steepest descent, slower than CG/LBFGS.
- each iteration involves evaluation of ~2N gradient values  and  solution
  of 2Nx2N quadratic programming problem, which  limits  applicability  of
  algorithm by small-scale problems (up to 50-100).

IMPORTANT: this  algorithm  has  convergence  guarantees,   i.e.  it  will
           steadily move towards some stationary point of the function.

           However, "stationary point" does not  always  mean  "solution".
           Nonsmooth problems often have "flat spots",  i.e.  areas  where
           function do not change at all. Such "flat spots" are stationary
           points by definition, and algorithm may be caught here.

           Nonsmooth CONVEX tasks are not prone to  this  problem. Say, if
           your function has form f()=MAX(f0,f1,...), and f_i are  convex,
           then f() is convex too and you have guaranteed  convergence  to
           solution.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    Radius  -   initial sampling radius, >=0.

                Internally multiplied  by  vector of  per-variable  scales
                specified by minnssetscale()).

                You should select relatively large sampling radius, roughly
                proportional to scaled length of the first  steps  of  the
                algorithm. Something close to 0.1 in magnitude  should  be
                good for most problems.

                AGS solver can automatically decrease radius, so too large
                radius is  not a problem (assuming that you  won't  choose
                so large radius that algorithm  will  sample  function  in
                too far away points, where gradient value is irrelevant).

                Too small radius won't cause algorithm to fail, but it may
                slow down algorithm (it may  have  to  perform  too  short
                steps).
    Penalty -   penalty coefficient for nonlinear constraints:
                * for problem with nonlinear constraints  should  be  some
                  problem-specific  positive   value,  large  enough  that
                  penalty term changes shape of the function.
                  Starting  from  some  problem-specific   value   penalty
                  coefficient becomes  large  enough  to  exactly  enforce
                  nonlinear constraints;  larger  values  do  not  improve
                  precision.
                  Increasing it too much may slow down convergence, so you
                  should choose it carefully.
                * can be zero for problems WITHOUT  nonlinear  constraints
                  (i.e. for unconstrained ones or ones with  just  box  or
                  linear constraints)
                * if you specify zero value for problem with at least  one
                  nonlinear  constraint,  algorithm  will  terminate  with
                  error code -1.

ALGORITHM OUTLINE

The very basic outline of unconstrained AGS algorithm is given below:

0. If sampling radius is below EpsX  or  we  performed  more  then  MaxIts
   iterations - STOP.
1. sample O(N) gradient values at random locations  around  current point;
   informally speaking, this sample is an implicit piecewise  linear model
   of the function, although algorithm formulation does  not  mention that
   explicitly
2. solve quadratic programming problem in order to find descent direction
3. if QP solver tells us that we  are  near  solution,  decrease  sampling
   radius and move to (0)
4. perform backtracking line search
5. after moving to new point, goto (0)

Constraint handling details:
* box constraints are handled exactly by algorithm
* linear/nonlinear constraints are handled by adding L1  penalty.  Because
  our solver can handle nonsmoothness, we can  use  L1  penalty  function,
  which is an exact one  (i.e.  exact  solution  is  returned  under  such
  penalty).
* penalty coefficient for  linear  constraints  is  chosen  automatically;
  however, penalty coefficient for nonlinear constraints must be specified
  by user.

===== TRACING AGS SOLVER =================================================

AGS solver supports advanced tracing capabilities. You can trace algorithm
output by specifying following trace symbols (case-insensitive)  by  means
of trace_file() call:
* 'AGS'         - for basic trace of algorithm  steps and decisions.  Only
                  short scalars (function values and deltas) are  printed.
                  N-dimensional quantities like search directions are  NOT
                  printed.
* 'AGS.DETAILED'- for output of points being visited and search directions
                  This  symbol  also  implicitly  defines  'AGS'. You  can
                  control output format by additionally specifying:
                  * nothing     to output in  6-digit exponential format
                  * 'PREC.E15'  to output in 15-digit exponential format
                  * 'PREC.F6'   to output in  6-digit fixed-point format
* 'AGS.DETAILED.SAMPLE'-
                  for output of points being visited ,  search  directions
                  and gradient sample. May take a LOT of  space ,  do  not
                  use it on problems with more that several tens of vars.
                  This  symbol   also    implicitly   defines   'AGS'  and
                  'AGS.DETAILED'.

By default trace is disabled and adds  no  overhead  to  the  optimization
process. However, specifying any of the symbols adds some  formatting  and
output-related overhead.

You may specify multiple symbols by separating them with commas:
>
> alglib::trace_file("AGS,PREC.F6", "path/to/trace.log")
>


  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnssetalgoags(minnsstate &state, const double radius, const double penalty, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to minnsoptimize().

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minnssetxrep(minnsstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnsrequesttermination(minnsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minnsiteration(minnsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to launch iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    fvec    -   callback which calculates function vector fi[]
                at given point x
    jac     -   callback which calculates function vector fi[]
                and Jacobian jac at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL


NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied Jacobian, and one which uses  only  function
   vector and numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   you should choose appropriate variant of  minnsoptimize() -  one  which
   accepts function AND Jacobian or one which accepts ONLY function.

   Be careful to choose variant of minnsoptimize()  which  corresponds  to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to minnsoptimize()    and  specific
   function used to create optimizer.


                     |         USER PASSED TO minnsoptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   minnscreatef()    |     works               FAILS
   minnscreate()     |     FAILS               works

   Here "FAILS" denotes inappropriate combinations  of  optimizer creation
   function  and  minnsoptimize()  version.   Attemps   to    use     such
   combination will lead to exception. Either  you  did  not pass gradient
   when it WAS needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey


*************************************************************************/
void minnsoptimize(minnsstate &state,
    void (*fvec)(const real_1d_array &x, real_1d_array &fi, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minnsoptimize(minnsstate &state,
    void  (*jac)(const real_1d_array &x, real_1d_array &fi, real_2d_array &jac, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
MinNS results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report. You should check Rep.TerminationType
                in  order  to  distinguish  successful  termination  from
                unsuccessful one:
                * -8   internal integrity control  detected  infinite  or
                       NAN   values   in   function/gradient.    Abnormal
                       termination signalled.
                * -3   box constraints are inconsistent
                * -1   inconsistent parameters were passed:
                       * penalty parameter for minnssetalgoags() is zero,
                         but we have nonlinear constraints set by minnssetnlc()
                *  2   sampling radius decreased below epsx
                *  7    stopping conditions are too stringent,
                        further improvement is impossible,
                        X contains best point found so far.
                *  8    User requested termination via minnsrequesttermination()

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnsresults(const minnsstate &state, real_1d_array &x, minnsreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************

Buffered implementation of minnsresults() which uses pre-allocated  buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnsresultsbuf(const minnsstate &state, real_1d_array &x, minnsreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine restarts algorithm from new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with minnscreate() call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 18.05.2015 by Bochkanov Sergey
*************************************************************************/
void minnsrestartfrom(minnsstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_MINCOMP) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
Obsolete function, use MinLBFGSSetPrecDefault() instead.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetdefaultpreconditioner(minlbfgsstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete function, use MinLBFGSSetCholeskyPreconditioner() instead.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minlbfgssetcholeskypreconditioner(minlbfgsstate &state, const real_2d_array &p, const bool isupper, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is obsolete function which was used by previous version of the  BLEIC
optimizer. It does nothing in the current version of BLEIC.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetbarrierwidth(minbleicstate &state, const double mu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is obsolete function which was used by previous version of the  BLEIC
optimizer. It does nothing in the current version of BLEIC.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbleicsetbarrierdecay(minbleicstate &state, const double mudecay, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 25.03.2010 by Bochkanov Sergey
*************************************************************************/
void minasacreate(const ae_int_t n, const real_1d_array &x, const real_1d_array &bndl, const real_1d_array &bndu, minasastate &state, const xparams _xparams = alglib::xdefault);
void minasacreate(const real_1d_array &x, const real_1d_array &bndl, const real_1d_array &bndu, minasastate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetcond(minasastate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetxrep(minasastate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetalgorithm(minasastate &state, const ae_int_t algotype, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minasasetstpmax(minasastate &state, const double stpmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minasaiteration(minasastate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to launch iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    grad    -   callback which calculates function (or merit function)
                value func and gradient grad at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL


  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey


*************************************************************************/
void minasaoptimize(minasastate &state,
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void minasaresults(const minasastate &state, real_1d_array &x, minasareport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 20.03.2009 by Bochkanov Sergey
*************************************************************************/
void minasaresultsbuf(const minasastate &state, real_1d_array &x, minasareport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Obsolete optimization algorithm.
Was replaced by MinBLEIC subpackage.

  -- ALGLIB --
     Copyright 30.07.2010 by Bochkanov Sergey
*************************************************************************/
void minasarestartfrom(minasastate &state, const real_1d_array &x, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_MINBC) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
                     BOX CONSTRAINED OPTIMIZATION
          WITH FAST ACTIVATION OF MULTIPLE BOX CONSTRAINTS

DESCRIPTION:
The  subroutine  minimizes  function   F(x) of N arguments subject  to box
constraints (with some of box constraints actually being equality ones).

This optimizer uses algorithm similar to that of MinBLEIC (optimizer  with
general linear constraints), but presence of box-only  constraints  allows
us to use faster constraint activation strategies. On large-scale problems,
with multiple constraints active at the solution, this  optimizer  can  be
several times faster than BLEIC.

REQUIREMENTS:
* user must provide function value and gradient
* starting point X0 must be feasible or
  not too far away from the feasible set
* grad(f) must be Lipschitz continuous on a level set:
  L = { x : f(x)<=f(x0) }
* function must be defined everywhere on the feasible set F

USAGE:

Constrained optimization if far more complex than the unconstrained one.
Here we give very brief outline of the BC optimizer. We strongly recommend
you to read examples in the ALGLIB Reference Manual and to read ALGLIB User Guide
on optimization, which is available at http://www.alglib.net/optimization/

1. User initializes algorithm state with MinBCCreate() call

2. USer adds box constraints by calling MinBCSetBC() function.

3. User sets stopping conditions with MinBCSetCond().

4. User calls MinBCOptimize() function which takes algorithm  state and
   pointer (delegate, etc.) to callback function which calculates F/G.

5. User calls MinBCResults() to get solution

6. Optionally user may call MinBCRestartFrom() to solve another problem
   with same N but another starting point.
   MinBCRestartFrom() allows to reuse already initialized structure.


INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size ofX
    X       -   starting point, array[N]:
                * it is better to set X to a feasible point
                * but X can be infeasible, in which case algorithm will try
                  to find feasible point first, using X as initial
                  approximation.

OUTPUT PARAMETERS:
    State   -   structure stores algorithm state

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbccreate(const ae_int_t n, const real_1d_array &x, minbcstate &state, const xparams _xparams = alglib::xdefault);
void minbccreate(const real_1d_array &x, minbcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
The subroutine is finite difference variant of MinBCCreate().  It  uses
finite differences in order to differentiate target function.

Description below contains information which is specific to  this function
only. We recommend to read comments on MinBCCreate() in  order  to  get
more information about creation of BC optimizer.

INPUT PARAMETERS:
    N       -   problem dimension, N>0:
                * if given, only leading N elements of X are used
                * if not given, automatically determined from size of X
    X       -   starting point, array[0..N-1].
    DiffStep-   differentiation step, >0

OUTPUT PARAMETERS:
    State   -   structure which stores algorithm state

NOTES:
1. algorithm uses 4-point central formula for differentiation.
2. differentiation step along I-th axis is equal to DiffStep*S[I] where
   S[] is scaling vector which can be set by MinBCSetScale() call.
3. we recommend you to use moderate values of  differentiation  step.  Too
   large step will result in too large truncation  errors, while too small
   step will result in too large numerical  errors.  1.0E-6  can  be  good
   value to start with.
4. Numerical  differentiation  is   very   inefficient  -   one   gradient
   calculation needs 4*N function evaluations. This function will work for
   any N - either small (1...10), moderate (10...100) or  large  (100...).
   However, performance penalty will be too severe for any N's except  for
   small ones.
   We should also say that code which relies on numerical  differentiation
   is  less  robust and precise. CG needs exact gradient values. Imprecise
   gradient may slow  down  convergence, especially  on  highly  nonlinear
   problems.
   Thus  we  recommend to use this function for fast prototyping on small-
   dimensional problems only, and to implement analytical gradient as soon
   as possible.

  -- ALGLIB --
     Copyright 16.05.2011 by Bochkanov Sergey
*************************************************************************/
void minbccreatef(const ae_int_t n, const real_1d_array &x, const double diffstep, minbcstate &state, const xparams _xparams = alglib::xdefault);
void minbccreatef(const real_1d_array &x, const double diffstep, minbcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets boundary constraints for BC optimizer.

Boundary constraints are inactive by default (after initial creation).
They are preserved after algorithm restart with MinBCRestartFrom().

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    BndL    -   lower bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very small number or -INF.
    BndU    -   upper bounds, array[N].
                If some (all) variables are unbounded, you may specify
                very large number or +INF.

NOTE 1: it is possible to specify BndL[i]=BndU[i]. In this case I-th
variable will be "frozen" at X[i]=BndL[i]=BndU[i].

NOTE 2: this solver has following useful properties:
* bound constraints are always satisfied exactly
* function is evaluated only INSIDE area specified by  bound  constraints,
  even  when  numerical  differentiation is used (algorithm adjusts  nodes
  according to boundary constraints)

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetbc(minbcstate &state, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets stopping conditions for the optimizer.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    EpsG    -   >=0
                The  subroutine  finishes  its  work   if   the  condition
                |v|<EpsG is satisfied, where:
                * |.| means Euclidian norm
                * v - scaled gradient vector, v[i]=g[i]*s[i]
                * g - gradient
                * s - scaling coefficients set by MinBCSetScale()
    EpsF    -   >=0
                The  subroutine  finishes  its work if on k+1-th iteration
                the  condition  |F(k+1)-F(k)|<=EpsF*max{|F(k)|,|F(k+1)|,1}
                is satisfied.
    EpsX    -   >=0
                The subroutine finishes its work if  on  k+1-th  iteration
                the condition |v|<=EpsX is fulfilled, where:
                * |.| means Euclidian norm
                * v - scaled step vector, v[i]=dx[i]/s[i]
                * dx - step vector, dx=X(k+1)-X(k)
                * s - scaling coefficients set by MinBCSetScale()
    MaxIts  -   maximum number of iterations. If MaxIts=0, the  number  of
                iterations is unlimited.

Passing EpsG=0, EpsF=0 and EpsX=0 and MaxIts=0 (simultaneously) will lead
to automatic stopping criterion selection.

NOTE: when SetCond() called with non-zero MaxIts, BC solver may perform
      slightly more than MaxIts iterations. I.e., MaxIts  sets  non-strict
      limit on iterations count.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetcond(minbcstate &state, const double epsg, const double epsf, const double epsx, const ae_int_t maxits, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets scaling coefficients for BC optimizer.

ALGLIB optimizers use scaling matrices to test stopping  conditions  (step
size and gradient are scaled before comparison with tolerances).  Scale of
the I-th variable is a translation invariant measure of:
a) "how large" the variable is
b) how large the step should be to make significant changes in the function

Scaling is also used by finite difference variant of the optimizer  - step
along I-th axis is equal to DiffStep*S[I].

In  most  optimizers  (and  in  the  BC  too)  scaling is NOT a form of
preconditioning. It just  affects  stopping  conditions.  You  should  set
preconditioner  by  separate  call  to  one  of  the  MinBCSetPrec...()
functions.

There is a special  preconditioning  mode, however,  which  uses   scaling
coefficients to form diagonal preconditioning matrix. You  can  turn  this
mode on, if you want.   But  you should understand that scaling is not the
same thing as preconditioning - these are two different, although  related
forms of tuning solver.

INPUT PARAMETERS:
    State   -   structure stores algorithm state
    S       -   array[N], non-zero scaling coefficients
                S[i] may be negative, sign doesn't matter.

  -- ALGLIB --
     Copyright 14.01.2011 by Bochkanov Sergey
*************************************************************************/
void minbcsetscale(minbcstate &state, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: preconditioning is turned off.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetprecdefault(minbcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification  of  the  preconditioner:  diagonal of approximate Hessian is
used.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    D       -   diagonal of the approximate Hessian, array[0..N-1],
                (if larger, only leading N elements are used).

NOTE 1: D[i] should be positive. Exception will be thrown otherwise.

NOTE 2: you should pass diagonal of approximate Hessian - NOT ITS INVERSE.

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetprecdiag(minbcstate &state, const real_1d_array &d, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Modification of the preconditioner: scale-based diagonal preconditioning.

This preconditioning mode can be useful when you  don't  have  approximate
diagonal of Hessian, but you know that your  variables  are  badly  scaled
(for  example,  one  variable is in [1,10], and another in [1000,100000]),
and most part of the ill-conditioning comes from different scales of vars.

In this case simple  scale-based  preconditioner,  with H[i] = 1/(s[i]^2),
can greatly improve convergence.

IMPRTANT: you should set scale of your variables  with  MinBCSetScale()
call  (before  or after MinBCSetPrecScale() call). Without knowledge of
the scale of your variables scale-based preconditioner will be  just  unit
matrix.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state

  -- ALGLIB --
     Copyright 13.10.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetprecscale(minbcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function turns on/off reporting.

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    NeedXRep-   whether iteration reports are needed or not

If NeedXRep is True, algorithm will call rep() callback function if  it is
provided to MinBCOptimize().

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetxrep(minbcstate &state, const bool needxrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function sets maximum step length

INPUT PARAMETERS:
    State   -   structure which stores algorithm state
    StpMax  -   maximum step length, >=0. Set StpMax to 0.0,  if you don't
                want to limit step length.

Use this subroutine when you optimize target function which contains exp()
or  other  fast  growing  functions,  and optimization algorithm makes too
large  steps  which  lead   to overflow. This function allows us to reject
steps  that  are  too  large  (and  therefore  expose  us  to the possible
overflow) without actually calculating function value at the x+stp*d.

  -- ALGLIB --
     Copyright 02.04.2010 by Bochkanov Sergey
*************************************************************************/
void minbcsetstpmax(minbcstate &state, const double stpmax, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This function provides reverse communication interface
Reverse communication interface is not documented or recommended to use.
See below for functions which provide better documented API
*************************************************************************/
bool minbciteration(minbcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This family of functions is used to launch iterations of nonlinear optimizer

These functions accept following parameters:
    state   -   algorithm state
    func    -   callback which calculates function (or merit function)
                value func at given point x
    grad    -   callback which calculates function (or merit function)
                value func and gradient grad at given point x
    rep     -   optional callback which is called after each iteration
                can be NULL
    ptr     -   optional pointer which is passed to func/grad/hess/jac/rep
                can be NULL

NOTES:

1. This function has two different implementations: one which  uses  exact
   (analytical) user-supplied gradient,  and one which uses function value
   only  and  numerically  differentiates  function  in  order  to  obtain
   gradient.

   Depending  on  the  specific  function  used to create optimizer object
   (either  MinBCCreate() for analytical gradient or  MinBCCreateF()
   for numerical differentiation) you should choose appropriate variant of
   MinBCOptimize() - one  which  accepts  function  AND gradient or one
   which accepts function ONLY.

   Be careful to choose variant of MinBCOptimize() which corresponds to
   your optimization scheme! Table below lists different  combinations  of
   callback (function/gradient) passed to MinBCOptimize()  and specific
   function used to create optimizer.


                     |         USER PASSED TO MinBCOptimize()
   CREATED WITH      |  function only   |  function and gradient
   ------------------------------------------------------------
   MinBCCreateF()    |     works               FAILS
   MinBCCreate()     |     FAILS               works

   Here "FAIL" denotes inappropriate combinations  of  optimizer  creation
   function  and  MinBCOptimize()  version.   Attemps   to   use   such
   combination (for  example,  to  create optimizer with MinBCCreateF()
   and  to  pass  gradient  information  to  MinCGOptimize()) will lead to
   exception being thrown. Either  you  did  not pass gradient when it WAS
   needed or you passed gradient when it was NOT needed.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey


*************************************************************************/
void minbcoptimize(minbcstate &state,
    void (*func)(const real_1d_array &x, double &func, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);
void minbcoptimize(minbcstate &state,
    void (*grad)(const real_1d_array &x, double &func, real_1d_array &grad, void *ptr),
    void  (*rep)(const real_1d_array &x, double func, void *ptr) = NULL,
    void *ptr = NULL,
    const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates verification  of  the  user-supplied
analytic gradient.

Upon  activation  of  this  option  OptGuard  integrity  checker  performs
numerical differentiation of your target function  at  the  initial  point
(note: future versions may also perform check  at  the  final  point)  and
compares numerical gradient with analytic one provided by you.

If difference is too large, an error flag is set and optimization  session
continues. After optimization session is over, you can retrieve the report
which  stores  both  gradients  and  specific  components  highlighted  as
suspicious by the OptGuard.

The primary OptGuard report can be retrieved with minbcoptguardresults().

IMPORTANT: gradient check is a high-overhead option which  will  cost  you
           about 3*N additional function evaluations. In many cases it may
           cost as much as the rest of the optimization session.

           YOU SHOULD NOT USE IT IN THE PRODUCTION CODE UNLESS YOU WANT TO
           CHECK DERIVATIVES PROVIDED BY SOME THIRD PARTY.

NOTE: unlike previous incarnation of the gradient checking code,  OptGuard
      does NOT interrupt optimization even if it discovers bad gradient.

INPUT PARAMETERS:
    State       -   structure used to store algorithm state
    TestStep    -   verification step used for numerical differentiation:
                    * TestStep=0 turns verification off
                    * TestStep>0 activates verification
                    You should carefully choose TestStep. Value  which  is
                    too large (so large that  function  behavior  is  non-
                    cubic at this scale) will lead  to  false  alarms. Too
                    short step will result in rounding  errors  dominating
                    numerical derivative.

                    You may use different step for different parameters by
                    means of setting scale with minbcsetscale().

=== EXPLANATION ==========================================================

In order to verify gradient algorithm performs following steps:
  * two trial steps are made to X[i]-TestStep*S[i] and X[i]+TestStep*S[i],
    where X[i] is i-th component of the initial point and S[i] is a  scale
    of i-th parameter
  * F(X) is evaluated at these trial points
  * we perform one more evaluation in the middle point of the interval
  * we  build  cubic  model using function values and derivatives at trial
    points and we compare its prediction with actual value in  the  middle
    point

  -- ALGLIB --
     Copyright 15.06.2014 by Bochkanov Sergey
*************************************************************************/
void minbcoptguardgradient(minbcstate &state, const double teststep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This  function  activates/deactivates nonsmoothness monitoring  option  of
the  OptGuard  integrity  checker. Smoothness  monitor  silently  observes
solution process and tries to detect ill-posed problems, i.e. ones with:
a) discontinuous target function (non-C0)
b) nonsmooth     target function (non-C1)

Smoothness monitoring does NOT interrupt optimization  even if it suspects
that your problem is nonsmooth. It just sets corresponding  flags  in  the
OptGuard report which can be retrieved after optimization is over.

Smoothness monitoring is a moderate overhead option which often adds  less
than 1% to the optimizer running time. Thus, you can use it even for large
scale problems.

NOTE: OptGuard does  NOT  guarantee  that  it  will  always  detect  C0/C1
      continuity violations.

      First, minor errors are hard to  catch - say, a 0.0001 difference in
      the model values at two sides of the gap may be due to discontinuity
      of the model - or simply because the model has changed.

      Second, C1-violations  are  especially  difficult  to  detect  in  a
      noninvasive way. The optimizer usually  performs  very  short  steps
      near the nonsmoothness, and differentiation  usually   introduces  a
      lot of numerical noise.  It  is  hard  to  tell  whether  some  tiny
      discontinuity in the slope is due to real nonsmoothness or just  due
      to numerical noise alone.

      Our top priority was to avoid false positives, so in some rare cases
      minor errors may went unnoticed (however, in most cases they can  be
      spotted with restart from different initial point).

INPUT PARAMETERS:
    state   -   algorithm state
    level   -   monitoring level:
                * 0 - monitoring is disabled
                * 1 - noninvasive low-overhead monitoring; function values
                      and/or gradients are recorded, but OptGuard does not
                      try to perform additional evaluations  in  order  to
                      get more information about suspicious locations.

=== EXPLANATION ==========================================================

One major source of headache during optimization  is  the  possibility  of
the coding errors in the target function/constraints (or their gradients).
Such  errors   most   often   manifest   themselves  as  discontinuity  or
nonsmoothness of the target/constraints.

Another frequent situation is when you try to optimize something involving
lots of min() and max() operations, i.e. nonsmooth target. Although not  a
coding error, it is nonsmoothness anyway - and smooth  optimizers  usually
stop right after encountering nonsmoothness, well before reaching solution.

OptGuard integrity checker helps you to catch such situations: it monitors
function values/gradients being passed  to  the  optimizer  and  tries  to
errors. Upon discovering suspicious pair of points it  raises  appropriate
flag (and allows you to continue optimization). When optimization is done,
you can study OptGuard result.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbcoptguardsmoothness(minbcstate &state, const ae_int_t level, const xparams _xparams = alglib::xdefault);
void minbcoptguardsmoothness(minbcstate &state, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Results of OptGuard integrity check, should be called  after  optimization
session is over.

=== PRIMARY REPORT =======================================================

OptGuard performs several checks which are intended to catch common errors
in the implementation of nonlinear function/gradient:
* incorrect analytic gradient
* discontinuous (non-C0) target functions (constraints)
* nonsmooth     (non-C1) target functions (constraints)

Each of these checks is activated with appropriate function:
* minbcoptguardgradient() for gradient verification
* minbcoptguardsmoothness() for C0/C1 checks

Following flags are set when these errors are suspected:
* rep.badgradsuspected, and additionally:
  * rep.badgradvidx for specific variable (gradient element) suspected
  * rep.badgradxbase, a point where gradient is tested
  * rep.badgraduser, user-provided gradient  (stored  as  2D  matrix  with
    single row in order to make  report  structure  compatible  with  more
    complex optimizers like MinNLC or MinLM)
  * rep.badgradnum,   reference    gradient    obtained    via   numerical
    differentiation (stored as  2D matrix with single row in order to make
    report structure compatible with more complex optimizers  like  MinNLC
    or MinLM)
* rep.nonc0suspected
* rep.nonc1suspected

=== ADDITIONAL REPORTS/LOGS ==============================================

Several different tests are performed to catch C0/C1 errors, you can  find
out specific test signaled error by looking to:
* rep.nonc0test0positive, for non-C0 test #0
* rep.nonc1test0positive, for non-C1 test #0
* rep.nonc1test1positive, for non-C1 test #1

Additional information (including line search logs)  can  be  obtained  by
means of:
* minbcoptguardnonc1test0results()
* minbcoptguardnonc1test1results()
which return detailed error reports, specific points where discontinuities
were found, and so on.

==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    rep     -   generic OptGuard report;  more  detailed  reports  can  be
                retrieved with other functions.

NOTE: false negatives (nonsmooth problems are not identified as  nonsmooth
      ones) are possible although unlikely.

      The reason  is  that  you  need  to  make several evaluations around
      nonsmoothness  in  order  to  accumulate  enough  information  about
      function curvature. Say, if you start right from the nonsmooth point,
      optimizer simply won't get enough data to understand what  is  going
      wrong before it terminates due to abrupt changes in the  derivative.
      It is also  possible  that  "unlucky"  step  will  move  us  to  the
      termination too quickly.

      Our current approach is to have less than 0.1%  false  negatives  in
      our test examples  (measured  with  multiple  restarts  from  random
      points), and to have exactly 0% false positives.

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbcoptguardresults(minbcstate &state, optguardreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #0

Nonsmoothness (non-C1) test #0 studies  function  values  (not  gradient!)
obtained during line searches and monitors  behavior  of  the  directional
derivative estimate.

This test is less powerful than test #1, but it does  not  depend  on  the
gradient values and thus it is more robust against artifacts introduced by
numerical differentiation.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], f[] - arrays of length CNT which store step lengths and  function
  values at these points; f[i] is evaluated in x0+stp[i]*d.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #0 "strong" report
    lngrep  -   C1 test #0 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbcoptguardnonc1test0results(const minbcstate &state, optguardnonc1test0report &strrep, optguardnonc1test0report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Detailed results of the OptGuard integrity check for nonsmoothness test #1

Nonsmoothness (non-C1)  test  #1  studies  individual  components  of  the
gradient computed during line search.

When precise analytic gradient is provided this test is more powerful than
test #0  which  works  with  function  values  and  ignores  user-provided
gradient.  However,  test  #0  becomes  more   powerful   when   numerical
differentiation is employed (in such cases test #1 detects  higher  levels
of numerical noise and becomes too conservative).

This test also tells specific components of the gradient which violate  C1
continuity, which makes it more informative than #0, which just tells that
continuity is violated.

Two reports are returned:
* a "strongest" one, corresponding  to  line   search  which  had  highest
  value of the nonsmoothness indicator
* a "longest" one, corresponding to line search which  had  more  function
  evaluations, and thus is more detailed

In both cases following fields are returned:

* positive - is TRUE  when test flagged suspicious point;  FALSE  if  test
  did not notice anything (in the latter cases fields below are empty).
* vidx - is an index of the variable in [0,N) with nonsmooth derivative
* x0[], d[] - arrays of length N which store initial point  and  direction
  for line search (d[] can be normalized, but does not have to)
* stp[], g[] - arrays of length CNT which store step lengths and  gradient
  values at these points; g[i] is evaluated in  x0+stp[i]*d  and  contains
  vidx-th component of the gradient.
* stpidxa, stpidxb - we  suspect  that  function  violates  C1  continuity
  between steps #stpidxa and #stpidxb (usually we have  stpidxb=stpidxa+3,
  with  most  likely  position  of  the  violation  between  stpidxa+1 and
  stpidxa+2.

==========================================================================
= SHORTLY SPEAKING: build a 2D plot of (stp,f) and look at it -  you  will
=                   see where C1 continuity is violated.
==========================================================================

INPUT PARAMETERS:
    state   -   algorithm state

OUTPUT PARAMETERS:
    strrep  -   C1 test #1 "strong" report
    lngrep  -   C1 test #1 "long" report

  -- ALGLIB --
     Copyright 21.11.2018 by Bochkanov Sergey
*************************************************************************/
void minbcoptguardnonc1test1results(minbcstate &state, optguardnonc1test1report &strrep, optguardnonc1test1report &lngrep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
BC results

INPUT PARAMETERS:
    State   -   algorithm state

OUTPUT PARAMETERS:
    X       -   array[0..N-1], solution
    Rep     -   optimization report. You should check Rep.TerminationType
                in  order  to  distinguish  successful  termination  from
                unsuccessful one:
                * -8    internal integrity control  detected  infinite or
                        NAN   values   in   function/gradient.   Abnormal
                        termination signalled.
                * -3   inconsistent constraints.
                *  1   relative function improvement is no more than EpsF.
                *  2   scaled step is no more than EpsX.
                *  4   scaled gradient norm is no more than EpsG.
                *  5   MaxIts steps was taken
                *  8   terminated by user who called minbcrequesttermination().
                       X contains point which was "current accepted"  when
                       termination request was submitted.
                More information about fields of this  structure  can  be
                found in the comments on MinBCReport datatype.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcresults(const minbcstate &state, real_1d_array &x, minbcreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
BC results

Buffered implementation of MinBCResults() which uses pre-allocated buffer
to store X[]. If buffer size is  too  small,  it  resizes  buffer.  It  is
intended to be used in the inner cycles of performance critical algorithms
where array reallocation penalty is too large to be ignored.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcresultsbuf(const minbcstate &state, real_1d_array &x, minbcreport &rep, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine restarts algorithm from new point.
All optimization parameters (including constraints) are left unchanged.

This  function  allows  to  solve multiple  optimization  problems  (which
must have  same number of dimensions) without object reallocation penalty.

INPUT PARAMETERS:
    State   -   structure previously allocated with MinBCCreate call.
    X       -   new starting point.

  -- ALGLIB --
     Copyright 28.11.2010 by Bochkanov Sergey
*************************************************************************/
void minbcrestartfrom(minbcstate &state, const real_1d_array &x, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This subroutine submits request for termination of running  optimizer.  It
should be called from user-supplied callback when user decides that it  is
time to "smoothly" terminate optimization process.  As  result,  optimizer
stops at point which was "current accepted" when termination  request  was
submitted and returns error code 8 (successful termination).

INPUT PARAMETERS:
    State   -   optimizer structure

NOTE: after  request  for  termination  optimizer  may   perform   several
      additional calls to user-supplied callbacks. It does  NOT  guarantee
      to stop immediately - it just guarantees that these additional calls
      will be discarded later.

NOTE: calling this function on optimizer which is NOT running will have no
      effect.

NOTE: multiple calls to this function are possible. First call is counted,
      subsequent calls are silently ignored.

  -- ALGLIB --
     Copyright 08.10.2014 by Bochkanov Sergey
*************************************************************************/
void minbcrequesttermination(minbcstate &state, const xparams _xparams = alglib::xdefault);
#endif

#if defined(AE_COMPILE_OPTS) || !defined(AE_PARTIAL_BUILD)
/*************************************************************************
This function serializes data structure to string.

Important properties of s_out:
* it contains alphanumeric characters, dots, underscores, minus signs
* these symbols are grouped into words, which are separated by spaces
  and Windows-style (CR+LF) newlines
* although  serializer  uses  spaces and CR+LF as separators, you can 
  replace any separator character by arbitrary combination of spaces,
  tabs, Windows or Unix newlines. It allows flexible reformatting  of
  the  string in case you want to include it into a text or XML file. 
  But you should not insert separators into the middle of the "words"
  nor should you change the case of letters.
* s_out can be freely moved between 32-bit and 64-bit systems, little
  and big endian machines, and so on. You can serialize structure  on
  32-bit machine and unserialize it on 64-bit one (or vice versa), or
  serialize  it  on  SPARC  and  unserialize  on  x86.  You  can also 
  serialize it in C++ version of ALGLIB and unserialize it in C# one, 
  and vice versa.
*************************************************************************/
void lptestproblemserialize(const lptestproblem &obj, std::string &s_out);


/*************************************************************************
This function serializes data structure to C++ stream.

Data stream generated by this function is same as  string  representation
generated  by  string  version  of  serializer - alphanumeric characters,
dots, underscores, minus signs, which are grouped into words separated by
spaces and CR+LF.

We recommend you to read comments on string version of serializer to find
out more about serialization of AlGLIB objects.
*************************************************************************/
void lptestproblemserialize(const lptestproblem &obj, std::ostream &s_out);


/*************************************************************************
This function unserializes data structure from string.
*************************************************************************/
void lptestproblemunserialize(const std::string &s_in, lptestproblem &obj);


/*************************************************************************
This function unserializes data structure from stream.
*************************************************************************/
void lptestproblemunserialize(const std::istream &s_in, lptestproblem &obj);


/*************************************************************************
Initialize test LP problem.

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemcreate(const ae_int_t n, const bool hasknowntarget, const double targetf, lptestproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
bool lptestproblemhasknowntarget(lptestproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
double lptestproblemgettargetf(lptestproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
ae_int_t lptestproblemgetn(lptestproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Query test problem info

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
ae_int_t lptestproblemgetm(lptestproblem &p, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Set scale for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetscale(lptestproblem &p, const real_1d_array &s, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Set cost for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetcost(lptestproblem &p, const real_1d_array &c, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Set box constraints for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetbc(lptestproblem &p, const real_1d_array &bndl, const real_1d_array &bndu, const xparams _xparams = alglib::xdefault);


/*************************************************************************
Set box constraints for test LP problem

This function is intended for internal use by ALGLIB.

  -- ALGLIB --
     Copyright 20.07.2021 by Bochkanov Sergey
*************************************************************************/
void lptestproblemsetlc2(lptestproblem &p, const sparsematrix &a, const real_1d_array &al, const real_1d_array &au, const ae_int_t m, const xparams _xparams = alglib::xdefault);


/*************************************************************************
This is internal function intended to  be  used  only  by  ALGLIB  itself.
Although for technical reasons it is made publicly available (and has  its
own manual entry), you should never call it.

  -- ALGLIB --
     Copyright 11.01.2011 by Bochkanov Sergey
*************************************************************************/
void xdbgminlpcreatefromtestproblem(const lptestproblem &p, minlpstate &state, const xparams _xparams = alglib::xdefault);
#endif
}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS COMPUTATIONAL CORE DECLARATIONS (FUNCTIONS)
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
#if defined(AE_COMPILE_OPTGUARDAPI) || !defined(AE_PARTIAL_BUILD)
void optguardinitinternal(optguardreport* rep,
     ae_int_t n,
     ae_int_t k,
     ae_state *_state);
void optguardexportreport(const optguardreport* srcrep,
     ae_int_t n,
     ae_int_t k,
     ae_bool badgradhasxj,
     optguardreport* dstrep,
     ae_state *_state);
void smoothnessmonitorexportc1test0report(const optguardnonc1test0report* srcrep,
     /* Real    */ const ae_vector* s,
     optguardnonc1test0report* dstrep,
     ae_state *_state);
void smoothnessmonitorexportc1test1report(const optguardnonc1test1report* srcrep,
     /* Real    */ const ae_vector* s,
     optguardnonc1test1report* dstrep,
     ae_state *_state);
ae_bool optguardallclear(const optguardreport* rep, ae_state *_state);
void _optguardreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _optguardreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _optguardreport_clear(void* _p);
void _optguardreport_destroy(void* _p);
void _optguardnonc0report_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _optguardnonc0report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _optguardnonc0report_clear(void* _p);
void _optguardnonc0report_destroy(void* _p);
void _optguardnonc1test0report_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _optguardnonc1test0report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _optguardnonc1test0report_clear(void* _p);
void _optguardnonc1test0report_destroy(void* _p);
void _optguardnonc1test1report_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _optguardnonc1test1report_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _optguardnonc1test1report_clear(void* _p);
void _optguardnonc1test1report_destroy(void* _p);
#endif
#if defined(AE_COMPILE_OPTSERV) || !defined(AE_PARTIAL_BUILD)
void checkbcviolation(/* Boolean */ const ae_vector* hasbndl,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* hasbndu,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* x,
     ae_int_t n,
     /* Real    */ const ae_vector* s,
     ae_bool nonunits,
     double* bcerr,
     ae_int_t* bcidx,
     ae_state *_state);
void checklcviolation(/* Real    */ const ae_matrix* cleic,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t nec,
     ae_int_t nic,
     /* Real    */ const ae_vector* x,
     ae_int_t n,
     double* lcerr,
     ae_int_t* lcidx,
     ae_state *_state);
void checknlcviolation(/* Real    */ const ae_vector* fi,
     ae_int_t ng,
     ae_int_t nh,
     double* nlcerr,
     ae_int_t* nlcidx,
     ae_state *_state);
void unscaleandchecknlcviolation(/* Real    */ const ae_vector* fi,
     /* Real    */ const ae_vector* fscales,
     ae_int_t ng,
     ae_int_t nh,
     double* nlcerr,
     ae_int_t* nlcidx,
     ae_state *_state);
void trimprepare(double f, double* threshold, ae_state *_state);
void trimfunction(double* f,
     /* Real    */ ae_vector* g,
     ae_int_t n,
     double threshold,
     ae_state *_state);
ae_bool enforceboundaryconstraints(/* Real    */ ae_vector* x,
     /* Real    */ const ae_vector* bl,
     /* Boolean */ const ae_vector* havebl,
     /* Real    */ const ae_vector* bu,
     /* Boolean */ const ae_vector* havebu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_state *_state);
void projectgradientintobc(/* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* g,
     /* Real    */ const ae_vector* bl,
     /* Boolean */ const ae_vector* havebl,
     /* Real    */ const ae_vector* bu,
     /* Boolean */ const ae_vector* havebu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_state *_state);
void calculatestepbound(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* d,
     double alpha,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_int_t* variabletofreeze,
     double* valuetofreeze,
     double* maxsteplen,
     ae_state *_state);
ae_int_t postprocessboundedstep(/* Real    */ ae_vector* x,
     /* Real    */ const ae_vector* xprev,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_int_t variabletofreeze,
     double valuetofreeze,
     double steptaken,
     double maxsteplen,
     ae_state *_state);
void filterdirection(/* Real    */ ae_vector* d,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     /* Real    */ const ae_vector* s,
     ae_int_t nmain,
     ae_int_t nslack,
     double droptol,
     ae_state *_state);
ae_int_t numberofchangedconstraints(/* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* xprev,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     ae_state *_state);
ae_bool findfeasiblepoint(/* Real    */ ae_vector* x,
     /* Real    */ const ae_vector* bndl,
     /* Boolean */ const ae_vector* havebndl,
     /* Real    */ const ae_vector* bndu,
     /* Boolean */ const ae_vector* havebndu,
     ae_int_t nmain,
     ae_int_t nslack,
     /* Real    */ const ae_matrix* _ce,
     ae_int_t k,
     double epsi,
     ae_int_t* qpits,
     ae_int_t* gpaits,
     ae_state *_state);
ae_bool derivativecheck(double f0,
     double df0,
     double f1,
     double df1,
     double f,
     double df,
     double width,
     ae_state *_state);
void estimateparabolicmodel(double absasum,
     double absasum2,
     double mx,
     double mb,
     double md,
     double d1,
     double d2,
     ae_int_t* d1est,
     ae_int_t* d2est,
     ae_state *_state);
void inexactlbfgspreconditioner(/* Real    */ ae_vector* s,
     ae_int_t n,
     /* Real    */ const ae_vector* d,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_matrix* w,
     ae_int_t k,
     precbuflbfgs* buf,
     ae_state *_state);
void preparelowrankpreconditioner(/* Real    */ const ae_vector* d,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_matrix* w,
     ae_int_t n,
     ae_int_t k,
     precbuflowrank* buf,
     ae_state *_state);
void applylowrankpreconditioner(/* Real    */ ae_vector* s,
     precbuflowrank* buf,
     ae_state *_state);
void smoothnessmonitorinit(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* s,
     ae_int_t n,
     ae_int_t k,
     ae_bool checksmoothness,
     ae_state *_state);
void smoothnessmonitorstartlinesearch(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     ae_int_t inneriter,
     ae_int_t outeriter,
     ae_state *_state);
void smoothnessmonitorstartlinesearch1u(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* invs,
     /* Real    */ const ae_vector* x,
     double f0,
     /* Real    */ const ae_vector* j0,
     ae_int_t inneriter,
     ae_int_t outeriter,
     ae_state *_state);
void smoothnessmonitorenqueuepoint(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* d,
     double stp,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* fi,
     /* Real    */ const ae_matrix* jac,
     ae_state *_state);
void smoothnessmonitorenqueuepoint1u(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* invs,
     /* Real    */ const ae_vector* d,
     double stp,
     /* Real    */ const ae_vector* x,
     double f0,
     /* Real    */ const ae_vector* j0,
     ae_state *_state);
void smoothnessmonitorfinalizelinesearch(smoothnessmonitor* monitor,
     ae_state *_state);
void smoothnessmonitorstartlagrangianprobing(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* d,
     double stpmax,
     ae_int_t inneriter,
     ae_int_t outeriter,
     ae_state *_state);
ae_bool smoothnessmonitorprobelagrangian(smoothnessmonitor* monitor,
     ae_state *_state);
void smoothnessmonitortracelagrangianprobingresults(smoothnessmonitor* monitor,
     ae_state *_state);
void smoothnessmonitortracestatus(const smoothnessmonitor* monitor,
     ae_bool callersuggeststrace,
     ae_state *_state);
void smoothnessmonitorexportreport(smoothnessmonitor* monitor,
     optguardreport* rep,
     ae_state *_state);
ae_bool smoothnessmonitorcheckgradientatx0(smoothnessmonitor* monitor,
     /* Real    */ const ae_vector* unscaledx0,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_bool hasboxconstraints,
     double teststep,
     ae_state *_state);
void hessianinitbfgs(xbfgshessian* hess,
     ae_int_t n,
     ae_int_t resetfreq,
     double stpshort,
     ae_state *_state);
void hessianinitlowrank(xbfgshessian* hess,
     ae_int_t n,
     ae_int_t m,
     double stpshort,
     double maxhess,
     ae_state *_state);
void hessianupdate(xbfgshessian* hess,
     /* Real    */ const ae_vector* x0,
     /* Real    */ const ae_vector* g0,
     /* Real    */ const ae_vector* x1,
     /* Real    */ const ae_vector* g1,
     ae_bool dotrace,
     ae_state *_state);
void hessiangetdiagonal(xbfgshessian* hess,
     /* Real    */ ae_vector* d,
     ae_state *_state);
void hessiangetmatrix(xbfgshessian* hess,
     ae_bool isupper,
     /* Real    */ ae_matrix* h,
     ae_state *_state);
void hessianmv(xbfgshessian* hess,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* hx,
     ae_state *_state);
double hessianvmv(xbfgshessian* hess,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void hessianxmv(xbfgshessian* hess,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* hx,
     double* xhx,
     ae_state *_state);
void motfcreaterandomunknown(ae_int_t n,
     ae_int_t m,
     ae_int_t nequality,
     ae_int_t ninequality,
     ae_int_t taskkind,
     double nlquadratic,
     double nlquartic,
     hqrndstate* rs,
     multiobjectivetestfunction* problem,
     ae_state *_state);
void motfeval(const multiobjectivetestfunction* problem,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* fi,
     ae_bool needfi,
     /* Real    */ ae_matrix* jac,
     ae_bool needjac,
     ae_state *_state);
void converttwosidedlctoonesidedold(const sparsematrix* sparsec,
     ae_int_t ksparse,
     /* Real    */ const ae_matrix* densec,
     ae_int_t kdense,
     ae_int_t n,
     /* Real    */ const ae_vector* cl,
     /* Real    */ const ae_vector* cu,
     /* Real    */ ae_matrix* olddensec,
     /* Integer */ ae_vector* olddensect,
     ae_int_t* olddensek,
     ae_state *_state);
void converttwosidednlctoonesidedold(/* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     /* Integer */ ae_vector* nlcidx,
     /* Real    */ ae_vector* nlcmul,
     /* Real    */ ae_vector* nlcadd,
     ae_int_t* cntnlec,
     ae_int_t* cntnlic,
     ae_state *_state);
void trustradincreasemomentum(double* growthfactor,
     double growthincrease,
     double maxgrowthfactor,
     ae_state *_state);
void trustradresetmomentum(double* growthfactor,
     double mingrowthfactor,
     ae_state *_state);
void _precbuflbfgs_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _precbuflbfgs_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _precbuflbfgs_clear(void* _p);
void _precbuflbfgs_destroy(void* _p);
void _precbuflowrank_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _precbuflowrank_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _precbuflowrank_clear(void* _p);
void _precbuflowrank_destroy(void* _p);
void _xbfgshessian_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _xbfgshessian_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _xbfgshessian_clear(void* _p);
void _xbfgshessian_destroy(void* _p);
void _smoothnessmonitor_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _smoothnessmonitor_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _smoothnessmonitor_clear(void* _p);
void _smoothnessmonitor_destroy(void* _p);
void _multiobjectivetestfunction_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _multiobjectivetestfunction_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _multiobjectivetestfunction_clear(void* _p);
void _multiobjectivetestfunction_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINLBFGS) || !defined(AE_PARTIAL_BUILD)
void minlbfgscreate(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     minlbfgsstate* state,
     ae_state *_state);
void minlbfgscreatef(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minlbfgsstate* state,
     ae_state *_state);
void minlbfgssetcond(minlbfgsstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minlbfgssetxrep(minlbfgsstate* state,
     ae_bool needxrep,
     ae_state *_state);
void minlbfgssetstpmax(minlbfgsstate* state,
     double stpmax,
     ae_state *_state);
void minlbfgssetscale(minlbfgsstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minlbfgscreatex(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     ae_int_t flags,
     double diffstep,
     minlbfgsstate* state,
     ae_state *_state);
void minlbfgssetprecdefault(minlbfgsstate* state, ae_state *_state);
void minlbfgssetpreccholesky(minlbfgsstate* state,
     /* Real    */ const ae_matrix* p,
     ae_bool isupper,
     ae_state *_state);
void minlbfgssetprecdiag(minlbfgsstate* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
void minlbfgssetprecscale(minlbfgsstate* state, ae_state *_state);
void minlbfgssetprecrankklbfgsfast(minlbfgsstate* state,
     /* Real    */ const ae_vector* d,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_matrix* w,
     ae_int_t cnt,
     ae_state *_state);
void minlbfgssetpreclowrankexact(minlbfgsstate* state,
     /* Real    */ const ae_vector* d,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_matrix* w,
     ae_int_t cnt,
     ae_state *_state);
ae_bool minlbfgsiteration(minlbfgsstate* state, ae_state *_state);
void minlbfgsoptguardgradient(minlbfgsstate* state,
     double teststep,
     ae_state *_state);
void minlbfgsoptguardsmoothness(minlbfgsstate* state,
     ae_int_t level,
     ae_state *_state);
void minlbfgsoptguardresults(minlbfgsstate* state,
     optguardreport* rep,
     ae_state *_state);
void minlbfgsoptguardnonc1test0results(const minlbfgsstate* state,
     optguardnonc1test0report* strrep,
     optguardnonc1test0report* lngrep,
     ae_state *_state);
void minlbfgsoptguardnonc1test1results(minlbfgsstate* state,
     optguardnonc1test1report* strrep,
     optguardnonc1test1report* lngrep,
     ae_state *_state);
void minlbfgsresults(const minlbfgsstate* state,
     /* Real    */ ae_vector* x,
     minlbfgsreport* rep,
     ae_state *_state);
void minlbfgsresultsbuf(const minlbfgsstate* state,
     /* Real    */ ae_vector* x,
     minlbfgsreport* rep,
     ae_state *_state);
void minlbfgsrestartfrom(minlbfgsstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minlbfgsrequesttermination(minlbfgsstate* state, ae_state *_state);
void _minlbfgsstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlbfgsstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlbfgsstate_clear(void* _p);
void _minlbfgsstate_destroy(void* _p);
void _minlbfgsreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlbfgsreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlbfgsreport_clear(void* _p);
void _minlbfgsreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_CQMODELS) || !defined(AE_PARTIAL_BUILD)
void cqminit(ae_int_t n, convexquadraticmodel* s, ae_state *_state);
void cqmseta(convexquadraticmodel* s,
     /* Real    */ const ae_matrix* a,
     ae_bool isupper,
     double alpha,
     ae_state *_state);
void cqmgeta(const convexquadraticmodel* s,
     /* Real    */ ae_matrix* a,
     ae_state *_state);
void cqmrewritedensediagonal(convexquadraticmodel* s,
     /* Real    */ const ae_vector* z,
     ae_state *_state);
void cqmsetd(convexquadraticmodel* s,
     /* Real    */ const ae_vector* d,
     double tau,
     ae_state *_state);
void cqmdropa(convexquadraticmodel* s, ae_state *_state);
void cqmsetb(convexquadraticmodel* s,
     /* Real    */ const ae_vector* b,
     ae_state *_state);
void cqmsetq(convexquadraticmodel* s,
     /* Real    */ const ae_matrix* q,
     /* Real    */ const ae_vector* r,
     ae_int_t k,
     double theta,
     ae_state *_state);
void cqmsetactiveset(convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     /* Boolean */ const ae_vector* activeset,
     ae_state *_state);
double cqmeval(const convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void cqmevalx(const convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     double* r,
     double* noise,
     ae_state *_state);
void cqmgradunconstrained(const convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* g,
     ae_state *_state);
double cqmxtadx2(const convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* tmp,
     ae_state *_state);
void cqmadx(const convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     /* Real    */ ae_vector* y,
     ae_state *_state);
ae_bool cqmconstrainedoptimum(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void cqmscalevector(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void cqmgetdiaga(convexquadraticmodel* s,
     /* Real    */ ae_vector* x,
     ae_state *_state);
double cqmdebugconstrainedevalt(convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
double cqmdebugconstrainedevale(convexquadraticmodel* s,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void _convexquadraticmodel_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _convexquadraticmodel_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _convexquadraticmodel_clear(void* _p);
void _convexquadraticmodel_destroy(void* _p);
#endif
#if defined(AE_COMPILE_LPQPSERV) || !defined(AE_PARTIAL_BUILD)
void scaleshiftmixedlcinplace(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     sparsematrix* sparsea,
     ae_int_t msparse,
     /* Real    */ ae_matrix* densea,
     ae_int_t mdense,
     /* Real    */ ae_vector* al,
     /* Real    */ ae_vector* au,
     ae_state *_state);
void scaleshiftbcinplace(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     /* Real    */ ae_vector* bndl,
     /* Real    */ ae_vector* bndu,
     ae_int_t n,
     ae_state *_state);
void scaleshiftdensebrlcinplace(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     /* Real    */ ae_matrix* densea,
     /* Real    */ ae_vector* ab,
     /* Real    */ ae_vector* ar,
     ae_int_t m,
     ae_state *_state);
void scaleshiftmixedbrlcinplace(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     sparsematrix* sparsea,
     ae_int_t msparse,
     /* Real    */ ae_matrix* densea,
     ae_int_t mdense,
     /* Real    */ ae_vector* ab,
     /* Real    */ ae_vector* ar,
     ae_state *_state);
void scaledenseqpinplace(/* Real    */ ae_matrix* densea,
     ae_bool isupper,
     ae_int_t nmain,
     /* Real    */ ae_vector* denseb,
     ae_int_t ntotal,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void scalesparseqpinplace(/* Real    */ const ae_vector* s,
     ae_int_t n,
     sparsematrix* sparsea,
     /* Real    */ ae_matrix* densecorrc,
     /* Real    */ ae_vector* densecorrd,
     ae_int_t corrrank,
     /* Real    */ ae_vector* denseb,
     ae_state *_state);
void normalizedensebrlcinplace(/* Real    */ ae_matrix* densea,
     /* Real    */ ae_vector* ab,
     /* Real    */ ae_vector* ar,
     ae_int_t n,
     ae_int_t m,
     /* Real    */ ae_vector* rownorms,
     ae_bool neednorms,
     ae_state *_state);
void normalizemixedbrlcinplace(sparsematrix* sparsea,
     ae_int_t msparse,
     /* Real    */ ae_matrix* densea,
     ae_int_t mdense,
     /* Real    */ ae_vector* ab,
     /* Real    */ ae_vector* ar,
     ae_int_t n,
     ae_bool limitedamplification,
     /* Real    */ ae_vector* rownorms,
     ae_bool neednorms,
     ae_state *_state);
double normalizedenseqpinplace(/* Real    */ ae_matrix* densea,
     ae_bool isupper,
     ae_int_t nmain,
     /* Real    */ ae_vector* denseb,
     ae_int_t ntotal,
     ae_state *_state);
double normalizesparseqpinplace(sparsematrix* sparsea,
     ae_bool isupper,
     /* Real    */ ae_matrix* densecorrc,
     /* Real    */ ae_vector* densecorrd,
     ae_int_t corrrank,
     /* Real    */ ae_vector* denseb,
     ae_int_t n,
     ae_state *_state);
void unscaleunshiftpointbc(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     /* Real    */ const ae_vector* rawbndl,
     /* Real    */ const ae_vector* rawbndu,
     /* Real    */ const ae_vector* sclsftbndl,
     /* Real    */ const ae_vector* sclsftbndu,
     /* Boolean */ const ae_vector* hasbndl,
     /* Boolean */ const ae_vector* hasbndu,
     /* Real    */ ae_vector* x,
     ae_int_t n,
     ae_state *_state);
#endif
#if defined(AE_COMPILE_SNNLS) || !defined(AE_PARTIAL_BUILD)
void snnlsinit(ae_int_t nsmax,
     ae_int_t ndmax,
     ae_int_t nrmax,
     snnlssolver* s,
     ae_state *_state);
void snnlssetproblem(snnlssolver* s,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* b,
     ae_int_t ns,
     ae_int_t nd,
     ae_int_t nr,
     ae_state *_state);
void snnlsdropnnc(snnlssolver* s, ae_int_t idx, ae_state *_state);
void snnlssolve(snnlssolver* s,
     /* Real    */ ae_vector* x,
     ae_state *_state);
void _snnlssolver_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _snnlssolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _snnlssolver_clear(void* _p);
void _snnlssolver_destroy(void* _p);
#endif
#if defined(AE_COMPILE_SACTIVESETS) || !defined(AE_PARTIAL_BUILD)
void sasinit(ae_int_t n, sactiveset* s, ae_state *_state);
void sassetscale(sactiveset* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void sassetprecdiag(sactiveset* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
void sassetbc(sactiveset* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void sassetlc(sactiveset* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void sassetlcx(sactiveset* state,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t nec,
     ae_int_t nic,
     ae_state *_state);
ae_bool sasstartoptimization(sactiveset* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void sasexploredirection(const sactiveset* state,
     /* Real    */ const ae_vector* d,
     double* stpmax,
     ae_int_t* cidx,
     double* vval,
     ae_state *_state);
ae_int_t sasmoveto(sactiveset* state,
     /* Real    */ const ae_vector* xn,
     ae_bool needact,
     ae_int_t cidx,
     double cval,
     ae_state *_state);
void sasimmediateactivation(sactiveset* state,
     ae_int_t cidx,
     double cval,
     ae_state *_state);
void sasconstraineddescent(sactiveset* state,
     /* Real    */ const ae_vector* g,
     /* Real    */ ae_vector* d,
     ae_state *_state);
void sasconstraineddescentprec(sactiveset* state,
     /* Real    */ const ae_vector* g,
     /* Real    */ ae_vector* d,
     ae_state *_state);
void sasconstraineddirection(sactiveset* state,
     /* Real    */ ae_vector* d,
     ae_state *_state);
void sasconstraineddirectionprec(sactiveset* state,
     /* Real    */ ae_vector* d,
     ae_state *_state);
void sascorrection(sactiveset* state,
     /* Real    */ ae_vector* x,
     double* penalty,
     ae_state *_state);
double sasactivelcpenalty1(sactiveset* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
double sasscaledconstrainednorm(sactiveset* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
void sasstopoptimization(sactiveset* state, ae_state *_state);
void sasreactivateconstraints(sactiveset* state,
     /* Real    */ const ae_vector* gc,
     ae_state *_state);
void sasreactivateconstraintsprec(sactiveset* state,
     /* Real    */ const ae_vector* gc,
     ae_state *_state);
void sasrebuildbasis(sactiveset* state, ae_state *_state);
void sasappendtobasis(sactiveset* state,
     /* Boolean */ const ae_vector* newentries,
     ae_state *_state);
void _sactiveset_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _sactiveset_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _sactiveset_clear(void* _p);
void _sactiveset_destroy(void* _p);
#endif
#if defined(AE_COMPILE_QQPSOLVER) || !defined(AE_PARTIAL_BUILD)
void qqploaddefaults(ae_int_t n, qqpsettings* s, ae_state *_state);
void qqpcopysettings(const qqpsettings* src,
     qqpsettings* dst,
     ae_state *_state);
void qqppreallocategrowdense(qqpbuffers* sstate,
     ae_int_t nexpected,
     ae_int_t ngrowto,
     ae_state *_state);
void qqpoptimize(const convexquadraticmodel* cqmac,
     const sparsematrix* sparseac,
     /* Real    */ const ae_matrix* denseac,
     ae_int_t akind,
     ae_bool isupper,
     /* Real    */ const ae_vector* bc,
     /* Real    */ const ae_vector* bndlc,
     /* Real    */ const ae_vector* bnduc,
     /* Real    */ const ae_vector* sc,
     /* Real    */ const ae_vector* xoriginc,
     ae_int_t nc,
     const qqpsettings* settings,
     qqpbuffers* sstate,
     /* Real    */ ae_vector* xs,
     ae_int_t* terminationtype,
     ae_state *_state);
void _qqpsettings_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _qqpsettings_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _qqpsettings_clear(void* _p);
void _qqpsettings_destroy(void* _p);
void _qqpbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _qqpbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _qqpbuffers_clear(void* _p);
void _qqpbuffers_destroy(void* _p);
#endif
#if defined(AE_COMPILE_QPDENSEAULSOLVER) || !defined(AE_PARTIAL_BUILD)
void qpdenseaulloaddefaults(ae_int_t nmain,
     qpdenseaulsettings* s,
     ae_state *_state);
void qpdenseauloptimize(const convexquadraticmodel* a,
     const sparsematrix* sparsea,
     ae_int_t akind,
     ae_bool sparseaupper,
     /* Real    */ const ae_vector* b,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t nn,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t dnec,
     ae_int_t dnic,
     const sparsematrix* scleic,
     ae_int_t snec,
     ae_int_t snic,
     ae_bool renormlc,
     const qpdenseaulsettings* settings,
     qpdenseaulbuffers* state,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     ae_int_t* terminationtype,
     ae_state *_state);
void _qpdenseaulsettings_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _qpdenseaulsettings_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _qpdenseaulsettings_clear(void* _p);
void _qpdenseaulsettings_destroy(void* _p);
void _qpdenseaulbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _qpdenseaulbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _qpdenseaulbuffers_clear(void* _p);
void _qpdenseaulbuffers_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINBLEIC) || !defined(AE_PARTIAL_BUILD)
void minbleiccreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     minbleicstate* state,
     ae_state *_state);
void minbleiccreatef(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minbleicstate* state,
     ae_state *_state);
void minbleicsetbc(minbleicstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minbleicsetlc(minbleicstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void minbleicsetcond(minbleicstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minbleicsetscale(minbleicstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minbleicsetprecdefault(minbleicstate* state, ae_state *_state);
void minbleicsetprecdiag(minbleicstate* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
void minbleicsetprecscale(minbleicstate* state, ae_state *_state);
void minbleicsetxrep(minbleicstate* state,
     ae_bool needxrep,
     ae_state *_state);
void minbleicsetdrep(minbleicstate* state,
     ae_bool needdrep,
     ae_state *_state);
void minbleicsetstpmax(minbleicstate* state,
     double stpmax,
     ae_state *_state);
ae_bool minbleiciteration(minbleicstate* state, ae_state *_state);
void minbleicoptguardgradient(minbleicstate* state,
     double teststep,
     ae_state *_state);
void minbleicoptguardsmoothness(minbleicstate* state,
     ae_int_t level,
     ae_state *_state);
void minbleicoptguardresults(minbleicstate* state,
     optguardreport* rep,
     ae_state *_state);
void minbleicoptguardnonc1test0results(const minbleicstate* state,
     optguardnonc1test0report* strrep,
     optguardnonc1test0report* lngrep,
     ae_state *_state);
void minbleicoptguardnonc1test1results(minbleicstate* state,
     optguardnonc1test1report* strrep,
     optguardnonc1test1report* lngrep,
     ae_state *_state);
void minbleicresults(const minbleicstate* state,
     /* Real    */ ae_vector* x,
     minbleicreport* rep,
     ae_state *_state);
void minbleicresultsbuf(const minbleicstate* state,
     /* Real    */ ae_vector* x,
     minbleicreport* rep,
     ae_state *_state);
void minbleicrestartfrom(minbleicstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minbleicrequesttermination(minbleicstate* state, ae_state *_state);
void minbleicemergencytermination(minbleicstate* state, ae_state *_state);
void _minbleicstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minbleicstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minbleicstate_clear(void* _p);
void _minbleicstate_destroy(void* _p);
void _minbleicreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minbleicreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minbleicreport_clear(void* _p);
void _minbleicreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_QPBLEICSOLVER) || !defined(AE_PARTIAL_BUILD)
void qpbleicloaddefaults(ae_int_t nmain,
     qpbleicsettings* s,
     ae_state *_state);
void qpbleiccopysettings(const qpbleicsettings* src,
     qpbleicsettings* dst,
     ae_state *_state);
void qpbleicoptimize(const convexquadraticmodel* a,
     const sparsematrix* sparsea,
     ae_int_t akind,
     ae_bool sparseaupper,
     double absasum,
     double absasum2,
     /* Real    */ const ae_vector* b,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     /* Real    */ const ae_matrix* cleic,
     ae_int_t nec,
     ae_int_t nic,
     const qpbleicsettings* settings,
     qpbleicbuffers* sstate,
     ae_bool* firstcall,
     /* Real    */ ae_vector* xs,
     ae_int_t* terminationtype,
     ae_state *_state);
void _qpbleicsettings_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _qpbleicsettings_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _qpbleicsettings_clear(void* _p);
void _qpbleicsettings_destroy(void* _p);
void _qpbleicbuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _qpbleicbuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _qpbleicbuffers_clear(void* _p);
void _qpbleicbuffers_destroy(void* _p);
#endif
#if defined(AE_COMPILE_VIPMSOLVER) || !defined(AE_PARTIAL_BUILD)
void vipminitdense(vipmstate* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     ae_state *_state);
void vipminitdensewithslacks(vipmstate* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t nmain,
     ae_int_t n,
     ae_state *_state);
void vipminitsparse(vipmstate* state,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* xorigin,
     ae_int_t n,
     ae_state *_state);
void vipmsetquadraticlinear(vipmstate* state,
     /* Real    */ const ae_matrix* denseh,
     const sparsematrix* sparseh,
     ae_int_t hkind,
     ae_bool isupper,
     /* Real    */ const ae_vector* c,
     ae_state *_state);
void vipmsetconstraints(vipmstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     const sparsematrix* sparsea,
     ae_int_t msparse,
     /* Real    */ const ae_matrix* densea,
     ae_int_t mdense,
     /* Real    */ const ae_vector* cl,
     /* Real    */ const ae_vector* cu,
     ae_state *_state);
void vipmsetcond(vipmstate* state,
     double epsp,
     double epsd,
     double epsgap,
     ae_state *_state);
void vipmoptimize(vipmstate* state,
     ae_bool dropbigbounds,
     /* Real    */ ae_vector* xs,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     ae_int_t* terminationtype,
     ae_state *_state);
void _vipmvars_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _vipmvars_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _vipmvars_clear(void* _p);
void _vipmvars_destroy(void* _p);
void _vipmreducedsparsesystem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _vipmreducedsparsesystem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _vipmreducedsparsesystem_clear(void* _p);
void _vipmreducedsparsesystem_destroy(void* _p);
void _vipmrighthandside_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _vipmrighthandside_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _vipmrighthandside_clear(void* _p);
void _vipmrighthandside_destroy(void* _p);
void _vipmstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _vipmstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _vipmstate_clear(void* _p);
void _vipmstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINQP) || !defined(AE_PARTIAL_BUILD)
void minqpcreate(ae_int_t n, minqpstate* state, ae_state *_state);
void minqpsetlinearterm(minqpstate* state,
     /* Real    */ const ae_vector* b,
     ae_state *_state);
void minqpsetquadraticterm(minqpstate* state,
     /* Real    */ const ae_matrix* a,
     ae_bool isupper,
     ae_state *_state);
void minqpsetquadratictermsparse(minqpstate* state,
     const sparsematrix* a,
     ae_bool isupper,
     ae_state *_state);
void minqpsetstartingpoint(minqpstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minqpsetorigin(minqpstate* state,
     /* Real    */ const ae_vector* xorigin,
     ae_state *_state);
void minqpsetscale(minqpstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minqpsetscaleautodiag(minqpstate* state, ae_state *_state);
void minqpsetalgobleic(minqpstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minqpsetalgodenseaul(minqpstate* state,
     double epsx,
     double rho,
     ae_int_t itscnt,
     ae_state *_state);
void minqpsetalgodenseipm(minqpstate* state, double eps, ae_state *_state);
void minqpsetalgosparseipm(minqpstate* state,
     double eps,
     ae_state *_state);
void minqpsetalgoquickqp(minqpstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxouterits,
     ae_bool usenewton,
     ae_state *_state);
void minqpsetbc(minqpstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minqpsetbcall(minqpstate* state,
     double bndl,
     double bndu,
     ae_state *_state);
void minqpsetbci(minqpstate* state,
     ae_int_t i,
     double bndl,
     double bndu,
     ae_state *_state);
void minqpsetlc(minqpstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void minqpsetlcsparse(minqpstate* state,
     const sparsematrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void minqpsetlcmixed(minqpstate* state,
     const sparsematrix* sparsec,
     /* Integer */ const ae_vector* sparsect,
     ae_int_t sparsek,
     /* Real    */ const ae_matrix* densec,
     /* Integer */ const ae_vector* densect,
     ae_int_t densek,
     ae_state *_state);
void minqpsetlcmixedlegacy(minqpstate* state,
     /* Real    */ const ae_matrix* densec,
     /* Integer */ const ae_vector* densect,
     ae_int_t densek,
     const sparsematrix* sparsec,
     /* Integer */ const ae_vector* sparsect,
     ae_int_t sparsek,
     ae_state *_state);
void minqpsetlc2dense(minqpstate* state,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minqpsetlc2(minqpstate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minqpsetlc2mixed(minqpstate* state,
     const sparsematrix* sparsea,
     ae_int_t ksparse,
     /* Real    */ const ae_matrix* densea,
     ae_int_t kdense,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_state *_state);
void minqpaddlc2dense(minqpstate* state,
     /* Real    */ const ae_vector* a,
     double al,
     double au,
     ae_state *_state);
void minqpaddlc2(minqpstate* state,
     /* Integer */ const ae_vector* idxa,
     /* Real    */ const ae_vector* vala,
     ae_int_t nnz,
     double al,
     double au,
     ae_state *_state);
void minqpaddlc2sparsefromdense(minqpstate* state,
     /* Real    */ const ae_vector* da,
     double al,
     double au,
     ae_state *_state);
void minqpoptimize(minqpstate* state, ae_state *_state);
void minqpresults(const minqpstate* state,
     /* Real    */ ae_vector* x,
     minqpreport* rep,
     ae_state *_state);
void minqpresultsbuf(const minqpstate* state,
     /* Real    */ ae_vector* x,
     minqpreport* rep,
     ae_state *_state);
void minqpsetlineartermfast(minqpstate* state,
     /* Real    */ const ae_vector* b,
     ae_state *_state);
void minqpsetquadratictermfast(minqpstate* state,
     /* Real    */ const ae_matrix* a,
     ae_bool isupper,
     double s,
     ae_state *_state);
void minqprewritediagonal(minqpstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minqpsetstartingpointfast(minqpstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minqpsetoriginfast(minqpstate* state,
     /* Real    */ const ae_vector* xorigin,
     ae_state *_state);
void _minqpstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minqpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minqpstate_clear(void* _p);
void _minqpstate_destroy(void* _p);
void _minqpreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minqpreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minqpreport_clear(void* _p);
void _minqpreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINLM) || !defined(AE_PARTIAL_BUILD)
void minlmcreatevj(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     minlmstate* state,
     ae_state *_state);
void minlmcreatev(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minlmstate* state,
     ae_state *_state);
void minlmcreatefgh(ae_int_t n,
     /* Real    */ const ae_vector* x,
     minlmstate* state,
     ae_state *_state);
void minlmsetcond(minlmstate* state,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minlmsetxrep(minlmstate* state, ae_bool needxrep, ae_state *_state);
void minlmsetstpmax(minlmstate* state, double stpmax, ae_state *_state);
void minlmsetscale(minlmstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minlmsetbc(minlmstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minlmsetlc(minlmstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void minlmsetacctype(minlmstate* state,
     ae_int_t acctype,
     ae_state *_state);
ae_bool minlmiteration(minlmstate* state, ae_state *_state);
void minlmoptguardgradient(minlmstate* state,
     double teststep,
     ae_state *_state);
void minlmoptguardresults(minlmstate* state,
     optguardreport* rep,
     ae_state *_state);
void minlmresults(const minlmstate* state,
     /* Real    */ ae_vector* x,
     minlmreport* rep,
     ae_state *_state);
void minlmresultsbuf(const minlmstate* state,
     /* Real    */ ae_vector* x,
     minlmreport* rep,
     ae_state *_state);
void minlmrestartfrom(minlmstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minlmrequesttermination(minlmstate* state, ae_state *_state);
void minlmcreatevgj(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     minlmstate* state,
     ae_state *_state);
void minlmcreatefgj(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     minlmstate* state,
     ae_state *_state);
void minlmcreatefj(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     minlmstate* state,
     ae_state *_state);
void _minlmstepfinder_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlmstepfinder_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlmstepfinder_clear(void* _p);
void _minlmstepfinder_destroy(void* _p);
void _minlmstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlmstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlmstate_clear(void* _p);
void _minlmstate_destroy(void* _p);
void _minlmreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlmreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlmreport_clear(void* _p);
void _minlmreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINCG) || !defined(AE_PARTIAL_BUILD)
void mincgcreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     mincgstate* state,
     ae_state *_state);
void mincgcreatef(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     mincgstate* state,
     ae_state *_state);
void mincgsetcond(mincgstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void mincgsetscale(mincgstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void mincgsetxrep(mincgstate* state, ae_bool needxrep, ae_state *_state);
void mincgsetdrep(mincgstate* state, ae_bool needdrep, ae_state *_state);
void mincgsetcgtype(mincgstate* state, ae_int_t cgtype, ae_state *_state);
void mincgsetstpmax(mincgstate* state, double stpmax, ae_state *_state);
void mincgsuggeststep(mincgstate* state, double stp, ae_state *_state);
double mincglastgoodstep(mincgstate* state, ae_state *_state);
void mincgsetprecdefault(mincgstate* state, ae_state *_state);
void mincgsetprecdiag(mincgstate* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
void mincgsetprecscale(mincgstate* state, ae_state *_state);
ae_bool mincgiteration(mincgstate* state, ae_state *_state);
void mincgoptguardgradient(mincgstate* state,
     double teststep,
     ae_state *_state);
void mincgoptguardsmoothness(mincgstate* state,
     ae_int_t level,
     ae_state *_state);
void mincgoptguardresults(mincgstate* state,
     optguardreport* rep,
     ae_state *_state);
void mincgoptguardnonc1test0results(const mincgstate* state,
     optguardnonc1test0report* strrep,
     optguardnonc1test0report* lngrep,
     ae_state *_state);
void mincgoptguardnonc1test1results(mincgstate* state,
     optguardnonc1test1report* strrep,
     optguardnonc1test1report* lngrep,
     ae_state *_state);
void mincgresults(const mincgstate* state,
     /* Real    */ ae_vector* x,
     mincgreport* rep,
     ae_state *_state);
void mincgresultsbuf(const mincgstate* state,
     /* Real    */ ae_vector* x,
     mincgreport* rep,
     ae_state *_state);
void mincgrestartfrom(mincgstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void mincgrequesttermination(mincgstate* state, ae_state *_state);
void mincgsetprecdiagfast(mincgstate* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
void mincgsetpreclowrankfast(mincgstate* state,
     /* Real    */ const ae_vector* d1,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_matrix* v,
     ae_int_t vcnt,
     ae_state *_state);
void mincgsetprecvarpart(mincgstate* state,
     /* Real    */ const ae_vector* d2,
     ae_state *_state);
void _mincgstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mincgstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mincgstate_clear(void* _p);
void _mincgstate_destroy(void* _p);
void _mincgreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _mincgreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _mincgreport_clear(void* _p);
void _mincgreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_NLCSQP) || !defined(AE_PARTIAL_BUILD)
void minsqpinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     /* Real    */ const ae_matrix* cleic,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t nec,
     ae_int_t nic,
     ae_int_t nlec,
     ae_int_t nlic,
     double epsx,
     ae_int_t maxits,
     minsqpstate* state,
     ae_state *_state);
ae_bool minsqpiteration(minsqpstate* state,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
     ae_state *_state);
void _minsqpsubsolver_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minsqpsubsolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minsqpsubsolver_clear(void* _p);
void _minsqpsubsolver_destroy(void* _p);
void _minsqptmplagrangian_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minsqptmplagrangian_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minsqptmplagrangian_clear(void* _p);
void _minsqptmplagrangian_destroy(void* _p);
void _minsqptmpmerit_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minsqptmpmerit_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minsqptmpmerit_clear(void* _p);
void _minsqptmpmerit_destroy(void* _p);
void _minsqpmeritphasestate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minsqpmeritphasestate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minsqpmeritphasestate_clear(void* _p);
void _minsqpmeritphasestate_destroy(void* _p);
void _minsqpstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minsqpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minsqpstate_clear(void* _p);
void _minsqpstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_LPQPPRESOLVE) || !defined(AE_PARTIAL_BUILD)
void presolvenonescaleuser(/* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_int_t n,
     const sparsematrix* sparsea,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_bool dotrace,
     presolveinfo* info,
     ae_state *_state);
void presolvelp(/* Real    */ const ae_vector* raws,
     /* Real    */ const ae_vector* rawc,
     /* Real    */ const ae_vector* rawbndl,
     /* Real    */ const ae_vector* rawbndu,
     ae_int_t n,
     const sparsematrix* rawsparsea,
     /* Real    */ const ae_vector* rawal,
     /* Real    */ const ae_vector* rawau,
     ae_int_t m,
     ae_bool dotrace,
     presolveinfo* presolved,
     ae_state *_state);
void presolvebwd(presolveinfo* info,
     /* Real    */ ae_vector* x,
     /* Integer */ ae_vector* stats,
     /* Real    */ ae_vector* lagbc,
     /* Real    */ ae_vector* laglc,
     ae_state *_state);
void _dynamiccrs_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dynamiccrs_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dynamiccrs_clear(void* _p);
void _dynamiccrs_destroy(void* _p);
void _presolvebuffers_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _presolvebuffers_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _presolvebuffers_clear(void* _p);
void _presolvebuffers_destroy(void* _p);
void _presolverstack_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _presolverstack_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _presolverstack_clear(void* _p);
void _presolverstack_destroy(void* _p);
void _presolveinfo_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _presolveinfo_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _presolveinfo_clear(void* _p);
void _presolveinfo_destroy(void* _p);
#endif
#if defined(AE_COMPILE_REVISEDDUALSIMPLEX) || !defined(AE_PARTIAL_BUILD)
void dsssettingsinit(dualsimplexsettings* settings, ae_state *_state);
void dssinit(ae_int_t n, dualsimplexstate* s, ae_state *_state);
void dsssetproblem(dualsimplexstate* state,
     /* Real    */ const ae_vector* c,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_matrix* densea,
     const sparsematrix* sparsea,
     ae_int_t akind,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     const dualsimplexbasis* proposedbasis,
     ae_int_t basisinittype,
     const dualsimplexsettings* settings,
     ae_state *_state);
void dssexportbasis(const dualsimplexstate* state,
     dualsimplexbasis* basis,
     ae_state *_state);
void dssoptimize(dualsimplexstate* state,
     const dualsimplexsettings* settings,
     ae_state *_state);
void _dualsimplexsettings_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dualsimplexsettings_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dualsimplexsettings_clear(void* _p);
void _dualsimplexsettings_destroy(void* _p);
void _dssvector_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dssvector_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dssvector_clear(void* _p);
void _dssvector_destroy(void* _p);
void _dualsimplexbasis_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dualsimplexbasis_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dualsimplexbasis_clear(void* _p);
void _dualsimplexbasis_destroy(void* _p);
void _dualsimplexsubproblem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dualsimplexsubproblem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dualsimplexsubproblem_clear(void* _p);
void _dualsimplexsubproblem_destroy(void* _p);
void _dualsimplexstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _dualsimplexstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _dualsimplexstate_clear(void* _p);
void _dualsimplexstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINLP) || !defined(AE_PARTIAL_BUILD)
void minlpcreate(ae_int_t n, minlpstate* state, ae_state *_state);
void minlpsetalgodss(minlpstate* state, double eps, ae_state *_state);
void minlpsetalgoipm(minlpstate* state, double eps, ae_state *_state);
void minlpsetcost(minlpstate* state,
     /* Real    */ const ae_vector* c,
     ae_state *_state);
void minlpsetscale(minlpstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minlpsetbc(minlpstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minlpsetbcall(minlpstate* state,
     double bndl,
     double bndu,
     ae_state *_state);
void minlpsetbci(minlpstate* state,
     ae_int_t i,
     double bndl,
     double bndu,
     ae_state *_state);
void minlpsetlc(minlpstate* state,
     /* Real    */ const ae_matrix* a,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void minlpsetlc2dense(minlpstate* state,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minlpsetlc2(minlpstate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minlpaddlc2dense(minlpstate* state,
     /* Real    */ const ae_vector* a,
     double al,
     double au,
     ae_state *_state);
void minlpaddlc2(minlpstate* state,
     /* Integer */ const ae_vector* idxa,
     /* Real    */ const ae_vector* vala,
     ae_int_t nnz,
     double al,
     double au,
     ae_state *_state);
void minlpoptimize(minlpstate* state, ae_state *_state);
void minlpresults(const minlpstate* state,
     /* Real    */ ae_vector* x,
     minlpreport* rep,
     ae_state *_state);
void minlpresultsbuf(const minlpstate* state,
     /* Real    */ ae_vector* x,
     minlpreport* rep,
     ae_state *_state);
void _minlpstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlpstate_clear(void* _p);
void _minlpstate_destroy(void* _p);
void _minlpreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minlpreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minlpreport_clear(void* _p);
void _minlpreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_NLCSLP) || !defined(AE_PARTIAL_BUILD)
void minslpinitbuf(/* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     /* Real    */ const ae_vector* s,
     /* Real    */ const ae_vector* x0,
     ae_int_t n,
     /* Real    */ const ae_matrix* cleic,
     /* Integer */ const ae_vector* lcsrcidx,
     ae_int_t nec,
     ae_int_t nic,
     ae_int_t nlec,
     ae_int_t nlic,
     double epsx,
     ae_int_t maxits,
     minslpstate* state,
     ae_state *_state);
ae_bool minslpiteration(minslpstate* state,
     smoothnessmonitor* smonitor,
     ae_bool userterminationneeded,
     ae_state *_state);
void _minslpsubsolver_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minslpsubsolver_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minslpsubsolver_clear(void* _p);
void _minslpsubsolver_destroy(void* _p);
void _minslptmplagrangian_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minslptmplagrangian_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minslptmplagrangian_clear(void* _p);
void _minslptmplagrangian_destroy(void* _p);
void _minslptmpmerit_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minslptmpmerit_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minslptmpmerit_clear(void* _p);
void _minslptmpmerit_destroy(void* _p);
void _minslpphase13state_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minslpphase13state_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minslpphase13state_clear(void* _p);
void _minslpphase13state_destroy(void* _p);
void _minslpphase2state_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minslpphase2state_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minslpphase2state_clear(void* _p);
void _minslpphase2state_destroy(void* _p);
void _minslpstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minslpstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minslpstate_clear(void* _p);
void _minslpstate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINNLC) || !defined(AE_PARTIAL_BUILD)
void minnlccreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     minnlcstate* state,
     ae_state *_state);
void minnlccreatef(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minnlcstate* state,
     ae_state *_state);
void minnlcsetbc(minnlcstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minnlcsetlc(minnlcstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void minnlcsetnlc(minnlcstate* state,
     ae_int_t nlec,
     ae_int_t nlic,
     ae_state *_state);
void minnlcsetcond(minnlcstate* state,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minnlcsetscale(minnlcstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minnlcsetprecinexact(minnlcstate* state, ae_state *_state);
void minnlcsetprecexactlowrank(minnlcstate* state,
     ae_int_t updatefreq,
     ae_state *_state);
void minnlcsetprecexactrobust(minnlcstate* state,
     ae_int_t updatefreq,
     ae_state *_state);
void minnlcsetprecnone(minnlcstate* state, ae_state *_state);
void minnlcsetstpmax(minnlcstate* state, double stpmax, ae_state *_state);
void minnlcsetalgoaul(minnlcstate* state,
     double rho,
     ae_int_t itscnt,
     ae_state *_state);
void minnlcsetalgoslp(minnlcstate* state, ae_state *_state);
void minnlcsetalgosqp(minnlcstate* state, ae_state *_state);
void minnlcsetxrep(minnlcstate* state, ae_bool needxrep, ae_state *_state);
ae_bool minnlciteration(minnlcstate* state, ae_state *_state);
void minnlcoptguardgradient(minnlcstate* state,
     double teststep,
     ae_state *_state);
void minnlcoptguardsmoothness(minnlcstate* state,
     ae_int_t level,
     ae_state *_state);
void minnlcoptguardresults(minnlcstate* state,
     optguardreport* rep,
     ae_state *_state);
void minnlcoptguardnonc1test0results(const minnlcstate* state,
     optguardnonc1test0report* strrep,
     optguardnonc1test0report* lngrep,
     ae_state *_state);
void minnlcoptguardnonc1test1results(minnlcstate* state,
     optguardnonc1test1report* strrep,
     optguardnonc1test1report* lngrep,
     ae_state *_state);
void minnlcresults(const minnlcstate* state,
     /* Real    */ ae_vector* x,
     minnlcreport* rep,
     ae_state *_state);
void minnlcresultsbuf(const minnlcstate* state,
     /* Real    */ ae_vector* x,
     minnlcreport* rep,
     ae_state *_state);
void minnlcrequesttermination(minnlcstate* state, ae_state *_state);
void minnlcrestartfrom(minnlcstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minnlcequalitypenaltyfunction(double alpha,
     double* f,
     double* df,
     double* d2f,
     ae_state *_state);
void minnlcinequalitypenaltyfunction(double alpha,
     double stabilizingpoint,
     double* f,
     double* df,
     double* d2f,
     ae_state *_state);
void minnlcinequalityshiftfunction(double alpha,
     double* f,
     double* df,
     double* d2f,
     ae_state *_state);
void _minnlcstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minnlcstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minnlcstate_clear(void* _p);
void _minnlcstate_destroy(void* _p);
void _minnlcreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minnlcreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minnlcreport_clear(void* _p);
void _minnlcreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MONBI) || !defined(AE_PARTIAL_BUILD)
void nbiscaleandinitbuf(/* Real    */ const ae_vector* x0,
     /* Real    */ const ae_vector* s,
     ae_int_t n,
     ae_int_t m,
     ae_int_t frontsize,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     const sparsematrix* sparsea,
     /* Real    */ const ae_matrix* densea,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t ksparse,
     ae_int_t kdense,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     double epsx,
     ae_int_t maxits,
     ae_bool polishsolutions,
     nbistate* state,
     ae_state *_state);
ae_bool nbiiteration(nbistate* state, ae_state *_state);
void _nbistate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _nbistate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _nbistate_clear(void* _p);
void _nbistate_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINMO) || !defined(AE_PARTIAL_BUILD)
void minmocreate(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     minmostate* state,
     ae_state *_state);
void minmocreatef(ae_int_t n,
     ae_int_t m,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minmostate* state,
     ae_state *_state);
void minmosetalgonbi(minmostate* state,
     ae_int_t frontsize,
     ae_bool polishsolutions,
     ae_state *_state);
void minmosetbc(minmostate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minmosetlc2dense(minmostate* state,
     /* Real    */ const ae_matrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minmosetlc2(minmostate* state,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t k,
     ae_state *_state);
void minmosetlc2mixed(minmostate* state,
     const sparsematrix* sparsea,
     ae_int_t ksparse,
     /* Real    */ const ae_matrix* densea,
     ae_int_t kdense,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_state *_state);
void minmoaddlc2dense(minmostate* state,
     /* Real    */ const ae_vector* a,
     double al,
     double au,
     ae_state *_state);
void minmoaddlc2(minmostate* state,
     /* Integer */ const ae_vector* idxa,
     /* Real    */ const ae_vector* vala,
     ae_int_t nnz,
     double al,
     double au,
     ae_state *_state);
void minmoaddlc2sparsefromdense(minmostate* state,
     /* Real    */ const ae_vector* da,
     double al,
     double au,
     ae_state *_state);
void minmosetnlc2(minmostate* state,
     /* Real    */ const ae_vector* nl,
     /* Real    */ const ae_vector* nu,
     ae_int_t nnlc,
     ae_state *_state);
void minmosetcond(minmostate* state,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minmosetscale(minmostate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minmosetxrep(minmostate* state, ae_bool needxrep, ae_state *_state);
ae_bool minmoiteration(minmostate* state, ae_state *_state);
void minmoresults(const minmostate* state,
     /* Real    */ ae_matrix* paretofront,
     ae_int_t* frontsize,
     minmoreport* rep,
     ae_state *_state);
void minmorequesttermination(minmostate* state, ae_state *_state);
void minmorestartfrom(minmostate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void _minmostate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minmostate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minmostate_clear(void* _p);
void _minmostate_destroy(void* _p);
void _minmoreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minmoreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minmoreport_clear(void* _p);
void _minmoreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINNS) || !defined(AE_PARTIAL_BUILD)
void minnscreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     minnsstate* state,
     ae_state *_state);
void minnscreatef(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minnsstate* state,
     ae_state *_state);
void minnssetbc(minnsstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minnssetlc(minnsstate* state,
     /* Real    */ const ae_matrix* c,
     /* Integer */ const ae_vector* ct,
     ae_int_t k,
     ae_state *_state);
void minnssetnlc(minnsstate* state,
     ae_int_t nlec,
     ae_int_t nlic,
     ae_state *_state);
void minnssetcond(minnsstate* state,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minnssetscale(minnsstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minnssetalgoags(minnsstate* state,
     double radius,
     double penalty,
     ae_state *_state);
void minnssetxrep(minnsstate* state, ae_bool needxrep, ae_state *_state);
void minnsrequesttermination(minnsstate* state, ae_state *_state);
ae_bool minnsiteration(minnsstate* state, ae_state *_state);
void minnsresults(const minnsstate* state,
     /* Real    */ ae_vector* x,
     minnsreport* rep,
     ae_state *_state);
void minnsresultsbuf(const minnsstate* state,
     /* Real    */ ae_vector* x,
     minnsreport* rep,
     ae_state *_state);
void minnsrestartfrom(minnsstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void _minnsqp_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minnsqp_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minnsqp_clear(void* _p);
void _minnsqp_destroy(void* _p);
void _minnsstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minnsstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minnsstate_clear(void* _p);
void _minnsstate_destroy(void* _p);
void _minnsreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minnsreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minnsreport_clear(void* _p);
void _minnsreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINCOMP) || !defined(AE_PARTIAL_BUILD)
void minlbfgssetdefaultpreconditioner(minlbfgsstate* state,
     ae_state *_state);
void minlbfgssetcholeskypreconditioner(minlbfgsstate* state,
     /* Real    */ const ae_matrix* p,
     ae_bool isupper,
     ae_state *_state);
void minbleicsetbarrierwidth(minbleicstate* state,
     double mu,
     ae_state *_state);
void minbleicsetbarrierdecay(minbleicstate* state,
     double mudecay,
     ae_state *_state);
void minasacreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     minasastate* state,
     ae_state *_state);
void minasasetcond(minasastate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minasasetxrep(minasastate* state, ae_bool needxrep, ae_state *_state);
void minasasetalgorithm(minasastate* state,
     ae_int_t algotype,
     ae_state *_state);
void minasasetstpmax(minasastate* state, double stpmax, ae_state *_state);
ae_bool minasaiteration(minasastate* state, ae_state *_state);
void minasaresults(const minasastate* state,
     /* Real    */ ae_vector* x,
     minasareport* rep,
     ae_state *_state);
void minasaresultsbuf(const minasastate* state,
     /* Real    */ ae_vector* x,
     minasareport* rep,
     ae_state *_state);
void minasarestartfrom(minasastate* state,
     /* Real    */ const ae_vector* x,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void _minasastate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minasastate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minasastate_clear(void* _p);
void _minasastate_destroy(void* _p);
void _minasareport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minasareport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minasareport_clear(void* _p);
void _minasareport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_MINBC) || !defined(AE_PARTIAL_BUILD)
void minbccreate(ae_int_t n,
     /* Real    */ const ae_vector* x,
     minbcstate* state,
     ae_state *_state);
void minbccreatef(ae_int_t n,
     /* Real    */ const ae_vector* x,
     double diffstep,
     minbcstate* state,
     ae_state *_state);
void minbcsetbc(minbcstate* state,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void minbcsetcond(minbcstate* state,
     double epsg,
     double epsf,
     double epsx,
     ae_int_t maxits,
     ae_state *_state);
void minbcsetscale(minbcstate* state,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void minbcsetprecdefault(minbcstate* state, ae_state *_state);
void minbcsetprecdiag(minbcstate* state,
     /* Real    */ const ae_vector* d,
     ae_state *_state);
void minbcsetprecscale(minbcstate* state, ae_state *_state);
void minbcsetxrep(minbcstate* state, ae_bool needxrep, ae_state *_state);
void minbcsetstpmax(minbcstate* state, double stpmax, ae_state *_state);
ae_bool minbciteration(minbcstate* state, ae_state *_state);
void minbcoptguardgradient(minbcstate* state,
     double teststep,
     ae_state *_state);
void minbcoptguardsmoothness(minbcstate* state,
     ae_int_t level,
     ae_state *_state);
void minbcoptguardresults(minbcstate* state,
     optguardreport* rep,
     ae_state *_state);
void minbcoptguardnonc1test0results(const minbcstate* state,
     optguardnonc1test0report* strrep,
     optguardnonc1test0report* lngrep,
     ae_state *_state);
void minbcoptguardnonc1test1results(minbcstate* state,
     optguardnonc1test1report* strrep,
     optguardnonc1test1report* lngrep,
     ae_state *_state);
void minbcresults(const minbcstate* state,
     /* Real    */ ae_vector* x,
     minbcreport* rep,
     ae_state *_state);
void minbcresultsbuf(const minbcstate* state,
     /* Real    */ ae_vector* x,
     minbcreport* rep,
     ae_state *_state);
void minbcrestartfrom(minbcstate* state,
     /* Real    */ const ae_vector* x,
     ae_state *_state);
void minbcrequesttermination(minbcstate* state, ae_state *_state);
void _minbcstate_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minbcstate_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minbcstate_clear(void* _p);
void _minbcstate_destroy(void* _p);
void _minbcreport_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _minbcreport_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _minbcreport_clear(void* _p);
void _minbcreport_destroy(void* _p);
#endif
#if defined(AE_COMPILE_OPTS) || !defined(AE_PARTIAL_BUILD)
void lptestproblemcreate(ae_int_t n,
     ae_bool hasknowntarget,
     double targetf,
     lptestproblem* p,
     ae_state *_state);
ae_bool lptestproblemhasknowntarget(lptestproblem* p, ae_state *_state);
double lptestproblemgettargetf(lptestproblem* p, ae_state *_state);
ae_int_t lptestproblemgetn(lptestproblem* p, ae_state *_state);
ae_int_t lptestproblemgetm(lptestproblem* p, ae_state *_state);
void lptestproblemsetscale(lptestproblem* p,
     /* Real    */ const ae_vector* s,
     ae_state *_state);
void lptestproblemsetcost(lptestproblem* p,
     /* Real    */ const ae_vector* c,
     ae_state *_state);
void lptestproblemsetbc(lptestproblem* p,
     /* Real    */ const ae_vector* bndl,
     /* Real    */ const ae_vector* bndu,
     ae_state *_state);
void lptestproblemsetlc2(lptestproblem* p,
     const sparsematrix* a,
     /* Real    */ const ae_vector* al,
     /* Real    */ const ae_vector* au,
     ae_int_t m,
     ae_state *_state);
void lptestproblemalloc(ae_serializer* s,
     const lptestproblem* p,
     ae_state *_state);
void lptestproblemserialize(ae_serializer* s,
     const lptestproblem* p,
     ae_state *_state);
void lptestproblemunserialize(ae_serializer* s,
     lptestproblem* p,
     ae_state *_state);
void xdbgminlpcreatefromtestproblem(const lptestproblem* p,
     minlpstate* state,
     ae_state *_state);
void _lptestproblem_init(void* _p, ae_state *_state, ae_bool make_automatic);
void _lptestproblem_init_copy(void* _dst, const void* _src, ae_state *_state, ae_bool make_automatic);
void _lptestproblem_clear(void* _p);
void _lptestproblem_destroy(void* _p);
#endif

}
#endif

