#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "assert.h"

static long maximumNumberOfVariables=0;
static ATermAppl *Substitution=NULL;

void RWsetVariable(ATermAppl v, ATermAppl t)
{
  assert(gsIsDataVarId(v));

  long n=ATgetAFun(ATgetArgument(v,0));
  
  if (n>=maximumNumberOfVariables)
  { long newsize=(n>=2*maximumNumberOfVariables?
                  (n<1024?1024:(n+1)):2*maximumNumberOfVariables);
    if (maximumNumberOfVariables==0)
    { Substitution=(ATermAppl *)malloc(newsize*sizeof(ATermAppl *));
    }
    else
    {
      for(long i=0 ; i<maximumNumberOfVariables ; i++)
      {
        ATunprotectAppl((ATermAppl *)&(Substitution[i]));
      }
    }
    { Substitution=(ATermAppl *)realloc(Substitution,newsize*sizeof(ATermAppl *));
    }
    if (Substitution==NULL)
    { ATerror("Fail to increase the size of a substitution array to %d\n",newsize); }

    for(long i=0 ; i<maximumNumberOfVariables ; i++)
    {
      ATprotectAppl((ATermAppl *)&(Substitution[i]));
    }

    for(long i=maximumNumberOfVariables ; i<newsize ; i++)
    { Substitution[i]=NULL;
      ATprotect((ATerm *)&(Substitution[i]));
    }
    maximumNumberOfVariables=newsize;
  }
  Substitution[n]=t;
}

void RWclearVariable(ATermAppl v)
{ 
  long n=ATgetAFun(ATgetArgument(v,0));

  Substitution[n]=NULL;
}

ATermAppl RWapplySubstitution(ATermAppl v)
{
  long n=ATgetAFun(ATgetArgument(v,0));

  if (n>=maximumNumberOfVariables)
  return v;

  ATermAppl r=Substitution[n];

  if (r==NULL)
  return v;

  return r;
}
