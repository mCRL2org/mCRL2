///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains sort data structures for the mcrl2 library.

#ifndef MCRL2_SORT_H
#define MCRL2_SORT_H

#include <list>
#include "atermpp/aterm.h"
//#include "mcrl2/predefined_symbols.h"
#include "mcrl2/aterm_wrapper.h"
#include "atermpp/aterm_list.h"
#include "gsfunc.h"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::make_term;

///////////////////////////////////////////////////////////////////////////////
// sort
/// \brief sort expression.
///
/// Models sorts of shape <tt>A -\> B</tt>, where A is the domain and B the range. A constant sort
/// has an empty domain, for example <tt>-\> S</tt>, or simply <tt>S</tt>.
/// 
class sort: public aterm_wrapper
{
  public:
    sort()
    {}

    /// Constructs a sort with internal representation t.
    ///
    sort(aterm_appl t)
      : aterm_wrapper(t)
    {}

    /// Constructs a sort with the given domain and range.
    ///
    sort(aterm_appl domain, aterm_appl range)
    {}

    /// Constructs a constant sort from a string. Currently the string s is converted
    /// to a SortId("D").
    /// Probably something more general like sort s("A-\>(B-\>C)"); should be supported.
    ///
    sort(std::string s)
//      : aterm_wrapper(aterm_appl(func_SortId(), atermpp::quoted_string(s)))
      : aterm_wrapper(gsMakeSortId(gsString2ATermAppl(s.c_str())))
    {}

    /// Returns the domain of the sort expression. Note that the domain is a list.
    /// For example the domain of the expression A -\> B -\> C is equal to [A,B].
    ///
    /// The domain of sort expression (A-\>B)-\>C-\>D is [A-\>B, C].
/*
    std::list<sort> domain() const
    {
      return std::list<sort>();
    }
    
    /// Returns the range of the sort expression.
    /// For example the range of the expression <tt>A -\> B -\> C</tt> is equal to C.
    /// The range of sort expression (A-\>B)-\>C-\>D is D.
    ///
    sort range() const
    {
      return sort("empty");
    }

    /// Returns the left hand side of the sort expression.
    ///
    /// The left hand side of sort expression (A-\>B)-\>C-\>D is A-\>B.
    sort lhs() const
    {
      return sort("lhs");
    }   

    /// Returns the right hand side of the sort expression.
    ///
    /// The right hand side of sort expression (A-\>B)-\>C-\>D is C-\>D.
    sort rhs() const
    {
      return sort("rhs");
    }   
*/

    /// Returns true if the sort is constant, i.e. has an empty domain.
    ///
    bool is_constant() const
    {
      return true;
      //return to_appl().function() == func_SortId();
    }   
};

typedef term_list<sort> sort_list;

/// Creates a sort with the given domain and range.
///
inline
sort make_sort(sort domain, sort range)
{
  return sort(aterm_appl(domain), aterm_appl(range));
}

} // namespace mcrl

#endif // MCRL2_SORT_H
