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

class sort;

///////////////////////////////////////////////////////////////////////////////
// sort_list
/// \brief singly linked list of sorts
///
typedef term_list<sort> sort_list;

///////////////////////////////////////////////////////////////////////////////
// sort
/// \brief sort expression.
///
/// Models sorts of shape <tt>A -\> B</tt>, where A is the domain and B the range. A constant sort
/// has an empty domain, for example <tt>-\> S</tt>, or simply <tt>S</tt>.
/// 
//<SortExpr>    ::= <SortId>
//                | SortList(<SortExpr>)                                   (- di)
//                | SortSet(<SortExpr>)                                    (- di)
//                | SortBag(<SortExpr>)                                    (- di)
//                | SortStruct(<StructCons>+)                              (- di)
//                | SortArrowProd(<SortExpr>+, <SortExpr>)                 (- di)
//                | SortArrow(<SortExpr>, <SortExpr>)                      (+ di)
//
//<SortId>       ::= SortId(<String>)
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

    sort(aterm_appl t)
      : aterm_appl(t)
    {
      assert(gsIsSortId(t) || gsIsSortArrow(t));
    }

    /// Constructs a sort from a string.
    sort(std::string s)
      : aterm_appl(gsMakeSortId(gsString2ATermAppl(s.c_str())))
    {}
    
    /// Returns true if it is a sort of type A -> B
    bool is_arrow() const
    {
      return gsIsSortArrow(*this);
    }

    /// Returns the domain sorts of the sort.
    ///
    /// domain_sorts(A -> (B -> C)       ) = [A,B]
    /// domain_sorts((A -> B) -> C       ) = [A->B]
    /// domain_sorts((A -> B) -> (C -> D)) = [A->B,C]
    lpe::sort_list domain_sorts() const
    {
      assert(is_arrow());
      return gsGetSortExprDomain(*this);
    }

    /// Returns the range of the sort.
    ///
    /// range(A -> (B -> C)       ) = C
    /// range((A -> B) -> C       ) = C
    /// range((A -> B) -> (C -> D)) = D
    lpe::sort range_sort() const
    {
      assert(is_arrow());
      return gsGetSortExprResult(*this);
    }

    /// Returns a pretty print representation of the term.
    ///                                                   
    std::string pp() const                                
    {                                                     
      return pretty_print(term());                        
    }
};

inline
bool is_sort(aterm_appl t)
{
  return gsIsSortId(t) || gsIsSortArrow(t);
}

/// Returns the sort 'domain -> range'.
inline
sort arrow(sort domain, sort range)
{
  return gsMakeSortArrow(domain, range);
}

} // namespace lpe

#endif // LPE_SORT_H
