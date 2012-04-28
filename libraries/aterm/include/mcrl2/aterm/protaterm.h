#ifndef ATERM_IPROTECTEDATERM
#define ATERM_IPROTECTEDATERM

#include <list>
#include "mcrl2/aterm/aterm1.h"

namespace aterm
{

/* class IProtectedATerm
{
    typedef std::list< IProtectedATerm* > pa_container;
    pa_container::iterator node;

  protected:

    // Intentionally declare a static global list of derived objects from pa_container
    // that all have a virtual function ATmarkTerms() which is called when marking
    // ATerms which are then not thrown away.
    static void AT_markProtectedATerms()  
    {
      for (pa_container::iterator i=p_aterms().begin(); i!=p_aterms().end(); i++)
      {
        (*i)->ATmarkTerms();
      }
    }

    static pa_container initialise_p_aterms()
    {
      ATaddProtectFunction(AT_markProtectedATerms);
      return pa_container();
    }

    static pa_container& p_aterms()
    {
      static pa_container _p_aterms = initialise_p_aterms();
      return _p_aterms;
    }

    void protect_aterms(IProtectedATerm* i)
    {
      p_aterms().push_front(i);
      node = p_aterms().begin();
    }

  public:

    IProtectedATerm()
    {}

    /// Assignment operator.
    IProtectedATerm &operator=(const IProtectedATerm &)
    { 
      // Prevent node from being assigned.
      return *this;
    } 

    virtual void ATmarkTerms() = 0; 

    virtual ~IProtectedATerm()
    {
      p_aterms().erase(node);
    }
}; */

} // namespace aterm

#endif // ATERM_IPROTECTEDATERM
