///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains sort data structures for the LPE Library.

#ifndef LPE_SORT_H
#define LPE_SORT_H

#include <cassert>
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "lpe/pretty_print.h"
#include "libstruct.h"

namespace lpe {

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::term_list;

///////////////////////////////////////////////////////////////////////////////
// sort
/// \brief sort expression.
///
/// Models sorts of shape <tt>A -\> B</tt>, where A is the domain and B the range. A constant sort
/// has an empty domain, for example <tt>-\> S</tt>, or simply <tt>S</tt>.
/// 
class sort: public aterm_appl
{
  public:
    sort()
    {}

    /// Constructs a sort with internal representation t.
    ///
    sort(ATermAppl t)
      : aterm_appl(t)
    {
      assert(gsIsSortId(t) || gsIsSortArrow(t));
    }

    sort(aterm x)
      : aterm_appl(x)
    {
      aterm_appl t(x);
      assert(gsIsSortId(t) || gsIsSortArrow(t));
    }

    /// Constructs a sort from a string.
    sort(std::string s)
      : aterm_appl(gsMakeSortId(gsString2ATermAppl(s.c_str())))
    {}
    
    /// Returns the predefined sort real.
    static sort real()
    {
      return sort(gsMakeSortExprReal());
    }

    /// Returns true if it is a sort of type A -> B
    bool is_arrow() const
    {
      return gsIsSortArrow(appl());
    }

    /// Returns a pretty print representation of the term.
    ///                                                   
    std::string pp() const                                
    {                                                     
      return pretty_print(term());                        
    }
};

///////////////////////////////////////////////////////////////////////////////
// sort_list
/// \brief singly linked list of sorts
///
typedef term_list<sort> sort_list;

} // namespace lpe

#endif // LPE_SORT_H
