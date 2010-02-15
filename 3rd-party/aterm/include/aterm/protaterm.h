#ifndef ATERM_IPROTECTEDATERM
#define ATERM_IPROTECTEDATERM

#include <set>
#include "aterm1.h"

class IProtectedATerm
{
  typedef std::multiset< IProtectedATerm* > pa_container;

  protected:

    static void AT_protectProtectedATerms()
    {
      for (pa_container::iterator i=p_aterms().begin(); i!=p_aterms().end(); i++)
      {
        (*i)->ATprotectTerms();
      }
    }

    static pa_container initialise_p_aterms()
    {
      ATaddProtectFunction(AT_protectProtectedATerms);
      return pa_container();
    }

    static pa_container &p_aterms()
    {
      static pa_container _p_aterms = initialise_p_aterms();
      return _p_aterms;
    }

    void ATprotectProtectedATerm(IProtectedATerm *i)
    {
      p_aterms().insert(i);
    }

    void ATunprotectProtectedATerm(IProtectedATerm *i)
    {
      p_aterms().erase(p_aterms().find(i));
    }

  public:

    virtual void ATprotectTerms() = 0;

    virtual ~IProtectedATerm() = 0;
};

inline IProtectedATerm::~IProtectedATerm()
{
}

#endif // ATERM_IPROTECTEDATERM
