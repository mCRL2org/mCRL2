///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains sort data structures for the LPE Library.

#ifndef LPE_SORT_H
#define LPE_SORT_H

#include <cassert>
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "lpe/aterm_wrapper.h"
#include "libstruct.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::term_list;

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
    {
      assert(gsIsSortId(t) || gsIsSortArrow(t));
    }

    /// Constructs a sort from a string.
    sort(std::string s)
      : aterm_wrapper(gsMakeSortId(gsString2ATermAppl(s.c_str())))
    {}
};

///////////////////////////////////////////////////////////////////////////////
// sort_list
/// \brief singly linked list of sorts
///
typedef term_list<sort> sort_list;

} // namespace lpe

#endif // LPE_SORT_H
