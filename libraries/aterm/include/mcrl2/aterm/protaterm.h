#ifndef ATERM_IPROTECTEDATERM
#define ATERM_IPROTECTEDATERM

#include <set>
#include "mcrl2/aterm/aterm1.h"

namespace aterm
{

class IProtectedATerm
{
    // typedef std::list< IProtectedATerm* > pa_container;
    typedef std::multiset< IProtectedATerm* > pa_container;
    // pa_container::iterator node;

  protected:

    static void AT_protectProtectedATerms()  // This method has a wrong name. It is used for marking terms.
    {
      for (pa_container::iterator i=p_aterms().begin(); i!=p_aterms().end(); i++)
      {
        (*i)->ATmarkTerms();
      }
    }

    static pa_container initialise_p_aterms()
    {
      ATaddProtectFunction(AT_protectProtectedATerms);
      return pa_container();
    }

    static pa_container& p_aterms()
    {
      static pa_container _p_aterms = initialise_p_aterms();
      return _p_aterms;
    }

    void ATprotectProtectedATerm(IProtectedATerm* i)
    {
      p_aterms().insert(i);
      // p_aterms().push_front(i);
      // node = p_aterms().begin();
    }

    void ATunprotectProtectedATerm(IProtectedATerm* i)
    {
      p_aterms().erase(i);
      // p_aterms().erase(node);
    }

  public:

    IProtectedATerm()
    {}

    /* IProtectedATerm &operator=(const IProtectedATerm &old)
    { 
      // Prevent node from being assigned.
      return *this;
    } */

    virtual void ATmarkTerms() = 0; 

    virtual ~IProtectedATerm() = 0;
};

inline IProtectedATerm::~IProtectedATerm()
{
}

} // namespace aterm

#endif // ATERM_IPROTECTEDATERM
