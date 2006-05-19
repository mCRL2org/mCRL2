#include <malloc.h>
#include "atermpp.h"

IProtectedATerm **p_aterms = NULL;
int p_aterms_size = 0;
int p_aterms_count = 0;

void AT_protectProtectedATerms()
{
  int i;
  for (i=0; i<p_aterms_count; i++)
  {
    p_aterms[i]->ATprotectTerms();
  }
}

void ATprotectProtectedATerm(IProtectedATerm *i)
{
  if ( p_aterms_count == p_aterms_size )
  {
    if ( p_aterms_size == 0 )
    {
      p_aterms_size = 32;
      ATaddProtectFunction(AT_protectProtectedATerms);
    } else {
      p_aterms_size = p_aterms_size * 2;
    }

    p_aterms = (IProtectedATerm **) realloc(p_aterms, p_aterms_size*sizeof(IProtectedATerm *));
  }

  p_aterms[p_aterms_count] = i;
  p_aterms_count++;
}

void ATunprotectProtectedATerm(IProtectedATerm *i)
{
  int j;
  for (j=0; j<p_aterms_count; j++)
  {
    if ( p_aterms[j] == i )
    {
      p_aterms_count--;
      p_aterms[j] = p_aterms[p_aterms_count];
    }
  }
}
