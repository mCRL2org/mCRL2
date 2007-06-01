///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains sort data structures for the LPS Library.

#ifndef LPS_SORT_H
#define LPS_SORT_H

#include <cassert>
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "lps/pretty_print.h"
#include "lps/detail/soundness_checks.h"
#include "lps/detail/constructors.h"
#include "libstruct.h"

namespace lps {

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
//                | SortArrow(<SortExpr>+, <SortExpr>)                 (- di)
//                | SortArrow(<SortExpr>, <SortExpr>)                      (+ di)
//
//<SortId>       ::= SortId(<String>)
class sort: public aterm_appl
{
  public:
    sort()
      : aterm_appl(detail::constructSortId())
    {}

    /// Constructs a sort with internal representation t.
    ///
    sort(ATermAppl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_SortExpr(m_term));
    }

    sort(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_SortExpr(m_term));
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
    /// <ul>
    /// <li>domain_sorts(A)                    = []      </li>
    /// <li>domain_sorts(A -> (B -> C)       ) = [A,B]   </li>
    /// <li>domain_sorts((A -> B) -> C       ) = [A->B]  </li>
    /// <li>domain_sorts((A -> B) -> (C -> D)) = [A->B,C]</li>
    /// </ul>
    lps::sort_list domain_sorts() const
    {
      return gsGetSortExprDomain(*this);
    }

    /// Returns the range of the sort.
    ///
    /// <ul>
    /// <li>range(A)                    = A</li>
    /// <li>range(A -> (B -> C)       ) = C</li>
    /// <li>range((A -> B) -> C       ) = C</li>
    /// <li>range((A -> B) -> (C -> D)) = D</li>
    /// </ul>
    lps::sort range_sort() const
    {
      return gsGetSortExprResult(*this);
    }
};

inline
bool is_sort(aterm_appl t)
{
  return gsIsSortId(t) || gsIsSortArrow(t);
}

/// Returns the sort 'domain -> range'.
inline
sort arrow(sort_list domain, sort range)
{
  return gsMakeSortArrow(domain, range);
}

namespace sort_expr {

  /// Returns the predefined sort real.
  inline
  sort real()
  {
    return gsMakeSortExprReal();
  }
  
  /// Returns the predefined sort int.
  inline
  sort int_()
  {
    return gsMakeSortExprInt();
  }
  
  /// Returns the predefined sort pos.
  inline
  sort pos()
  {
    return gsMakeSortExprPos();
  }
  
  /// Returns the predefined sort nat.
  inline
  sort nat()
  {
    return gsMakeSortExprNat();
  }
  
  /// Returns the predefined sort int.
  inline
  sort bool_()
  {
    return gsMakeSortExprBool();
  }

  inline bool is_real(aterm_appl t) { return t == real(); }
  inline bool is_int (aterm_appl t) { return t == int_(); }
  inline bool is_pos (aterm_appl t) { return t == pos(); }
  inline bool is_nat (aterm_appl t) { return t == nat(); }                                 
  inline bool is_bool(aterm_appl t) { return t == bool_(); }                                 

} // namespace sort_expr

} // namespace lps

/// INTERNAL ONLY
namespace atermpp
{
using lps::sort;

template<>
struct aterm_traits<sort>
{
  typedef ATermAppl aterm_type;
  static void protect(sort t)   { t.protect(); }
  static void unprotect(sort t) { t.unprotect(); }
  static void mark(sort t)      { t.mark(); }
  static ATerm term(sort t)     { return t.term(); }
  static ATerm* ptr(sort& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // LPS_SORT_H
