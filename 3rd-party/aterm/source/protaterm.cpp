#include <set>
#include "protaterm.h"

using namespace std;

IProtectedATerm::~IProtectedATerm()
{
}

typedef multiset<IProtectedATerm *> pa_container;

static void AT_protectProtectedATerms();

static pa_container initialise_p_aterms()
{
  ATaddProtectFunction(AT_protectProtectedATerms);
  return pa_container();
}
static pa_container p_aterms = initialise_p_aterms();

static void AT_protectProtectedATerms()
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
