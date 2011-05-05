#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/aterm_ext.h"

using namespace aterm;

#ifdef _MSC_VER
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif // _MSC_VER

extern "C" {
  DLLEXPORT ATermAppl rewrite(ATermAppl);
  DLLEXPORT void set_subst(ATermAppl Var, ATerm Expr);
  DLLEXPORT void clear_subst(ATermAppl Var);
  DLLEXPORT void clear_substs();
  DLLEXPORT void rewrite_init();
  DLLEXPORT void rewrite_cleanup();
  DLLEXPORT ATermAppl rewrite(const ATermAppl t);
}

static inline ATermAppl rewrite(const ATerm t) { return rewrite((ATermAppl)t); }

static inline ATermAppl makeAppl0(AFun a, ATerm h) { return ATmakeAppl1(a,h); }
static inline ATermAppl makeAppl1(AFun a, ATerm h, ATermAppl t) { return ATmakeAppl2(a,h,(ATerm) t); }
static inline ATermAppl makeAppl2(AFun a, ATerm h, ATermAppl t1, ATermAppl t2) { return ATmakeAppl3(a,h,(ATerm) t1, (ATerm) t2); }
static inline ATermAppl makeAppl3(AFun a, ATerm h, ATermAppl t1, ATermAppl t2, ATermAppl t3) { return ATmakeAppl4(a,h,(ATerm) t1, (ATerm) t2, (ATerm) t3); }
static inline ATermAppl makeAppl4(AFun a, ATerm h, ATermAppl t1, ATermAppl t2, ATermAppl t3, ATermAppl t4) { return ATmakeAppl5(a,h,(ATerm) t1, (ATerm) t2, (ATerm) t3, (ATerm) t4); }
static inline ATermAppl makeAppl5(AFun a, ATerm h, ATermAppl t1, ATermAppl t2, ATermAppl t3, ATermAppl t4, ATermAppl t5) { return ATmakeAppl6(a,h,(ATerm) t1, (ATerm) t2, (ATerm) t3, (ATerm) t4, (ATerm) t5); }

static ATerm *substs = NULL;
static long substs_size = 0;

ATerm inline get_subst(ATermAppl Var)
{
  long n = ATgetAFun(ATgetArgument(Var,0));
  if ( n >= substs_size )
  {
    return (ATerm) Var;
  }
  ATerm r = substs[n];
  if ( r == NULL )
  {
    return (ATerm) Var;
  }
  return r;
}

void set_subst(ATermAppl Var, ATerm Expr)
{
  long n = ATgetAFun(ATgetArgument(Var,0));

  if ( n >= substs_size )
  {
    long newsize;

    if ( n >= 2*substs_size )
    {
      if ( n < 1024 )
      {
        newsize = 1024;
      } else {
        newsize = n+1;
      }
    } else {
      newsize = 2*substs_size;
    }

    if ( substs_size > 0 )
    {
      ATunprotectArray(substs);
    }
    substs = (ATerm *) realloc(substs,newsize*sizeof(ATerm));
    
    if ( substs == NULL )
    {
      fprintf(stderr, "Failed to increase the size of a substitution array");
      exit(1);
    }

    for (long i=substs_size; i<newsize; i++)
    {
      substs[i]=NULL;
    }

    ATprotectArray(substs,newsize);
    substs_size = newsize;
  }

  substs[n] = Expr;
}

void clear_subst(ATermAppl Var)
{
  long n = ATgetAFun(ATgetArgument(Var,0));

  if ( n < substs_size )
  {
    substs[n] = NULL;
  }
}

void clear_substs()
{
  for (long i=0; i<substs_size; i++)
  {
    substs[i] = NULL;
  }
}

static AFun *apples;
static int num_apples = 0;
#define GET_APPL_INC 5
static AFun getAppl(int arity)
{
  int old_num = num_apples;

  if ( apples == NULL )
  {
    apples = (AFun *) malloc(0*sizeof(AFun));
  }

  while ( arity >= num_apples )
  {
    num_apples += GET_APPL_INC;
  }
  if ( old_num < num_apples )
  {
    apples = (AFun *) realloc(apples,num_apples*sizeof(AFun));
    for (int i=old_num; i<num_apples; i++)
    {
      char c[10];
      sprintf(c, "appl#%%i", i+1);
      apples[i] = ATmakeAFun(c,i+1,false);
      ATprotectAFun(apples[i]);
    }
  }

  return apples[arity];
}
