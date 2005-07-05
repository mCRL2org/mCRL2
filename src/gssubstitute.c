#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "assert.h"
#include "gssubstitute.h"

static long maximumNumberOfVariables=0;
static ATerm *Substitution=NULL;

void RWsetVariable(ATerm v, ATerm t)
{
  assert(gsIsDataVarId((ATermAppl) v));

  long n=ATgetAFun(ATgetArgument((ATermAppl) v,0));
  
  if (n>=maximumNumberOfVariables)
  { long newsize=(n>=2*maximumNumberOfVariables?
                  (n<1024?1024:(n+1)):2*maximumNumberOfVariables);
    if (maximumNumberOfVariables==0)
    { Substitution=(ATerm *)malloc(newsize*sizeof(ATerm));
    }
    else
    {
      for(long i=0 ; i<maximumNumberOfVariables ; i++)
      {
        ATunprotect(&(Substitution[i]));
      }
    }
    { Substitution=(ATerm *)realloc(Substitution,newsize*sizeof(ATerm));
    }
    if (Substitution==NULL)
    { ATerror("Fail to increase the size of a substitution array to %d\n",newsize); }

    for(long i=0 ; i<maximumNumberOfVariables ; i++)
    {
      ATprotectAppl(&(Substitution[i]));
    }

    for(long i=maximumNumberOfVariables ; i<newsize ; i++)
    { Substitution[i]=NULL;
      ATprotect(&(Substitution[i]));
    }
    maximumNumberOfVariables=newsize;
  }
  Substitution[n]=t;
}

void RWclearVariable(ATerm v)
{ 
  long n=ATgetAFun(ATgetArgument((ATermAppl) v,0));

  Substitution[n]=NULL;
}

ATerm RWapplySubstitution(ATerm v)
{
  long n=ATgetAFun(ATgetArgument((ATermAppl) v,0));

  if (n>=maximumNumberOfVariables)
  return v;

  ATerm r=Substitution[n];

  if (r==NULL)
  return v;

  return r;
}
