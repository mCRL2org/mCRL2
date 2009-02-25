#include <set>
#include "protaterm.h"

using namespace std;

IProtectedATerm::~IProtectedATerm()
{
}

typedef multiset<IProtectedATerm *> pa_container;
static pa_container p_aterms;

void AT_protectProtectedATerms()
{
  for (pa_container::iterator i=p_aterms.begin(); i!=p_aterms.end(); i++)
  {
    (*i)->ATprotectTerms();
  }
}

void ATprotectProtectedATerm(IProtectedATerm *i)
{
  p_aterms.insert(i);
}

void ATunprotectProtectedATerm(IProtectedATerm *i)
{
  p_aterms.erase(p_aterms.find(i));
}
