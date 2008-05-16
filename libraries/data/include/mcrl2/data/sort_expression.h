// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sort_expression.h
/// \brief The class sort_expression.

#ifndef MCRL2_DATA_SORT_EXPRESSION_H
#define MCRL2_DATA_SORT_EXPRESSION_H

#include <cassert>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/struct.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/detail/constructors.h"

namespace mcrl2 {

namespace data {

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::term_list;

class sort_expression;

/// \brief singly linked list of sort expressions
///
typedef term_list<sort_expression> sort_expression_list;

/// \brief sort expression.
///
/// A sort expression can either be a sort identifier or a sort arrow.
/// 
//<SortExpr>    ::= <SortId>
//                | SortList(<SortExpr>)                                   (- di)
//                | SortSet(<SortExpr>)                                    (- di)
//                | SortBag(<SortExpr>)                                    (- di)
//                | SortStruct(<StructCons>+)                              (- di)
//                | SortArrow(<SortExpr>+, <SortExpr>)                     (- di)
//                | SortArrow(<SortExpr>, <SortExpr>)                      (+ di)
//
//<SortId>       ::= SortId(<String>)
class sort_expression: public aterm_appl
{
  public:
    /// Constructor.
    ///
    sort_expression()
      : aterm_appl(core::detail::constructSortId())
    {}

    /// Constructor.
    ///
    sort_expression(ATermAppl t)
      : aterm_appl(t)
    {
      assert(core::detail::check_rule_SortExpr(m_term));
    }

    /// Constructor.
    ///
    sort_expression(aterm_appl t)
      : aterm_appl(t)
    {
      assert(core::detail::check_rule_SortExpr(m_term));
    }

    /// Constructor.
    ///
    sort_expression(std::string s)
      : aterm_appl(core::detail::gsMakeSortId(core::detail::gsString2ATermAppl(s.c_str())))
    {}
    
    /// Returns true if it is a sort_expression of type A -> B.
    ///
    bool is_arrow() const
    {
      return core::detail::gsIsSortArrow(*this);
    }
};

/// \brief Returns true if the term t is a sort_expression
inline
bool is_sort_expression(aterm_appl t)
{
  return core::detail::gsIsSortId(t) || core::detail::gsIsSortArrow(t);
}

/// Returns the sort_expression 'domain -> range'.
inline
sort_expression arrow(sort_expression_list domain, sort_expression range)
{
  return core::detail::gsMakeSortArrow(domain, range);
}

/// Returns the domain sorts of s.
/// \deprecated
inline
sort_expression_list domain_sorts(sort_expression s)
{
  return core::gsGetSortExprDomain(s);
}

/// Returns the range sort of s.
/// \deprecated
inline
sort_expression result_sort(sort_expression s)
{
  return core::gsGetSortExprResult(s);
}

/// Returns the source of the sort.
/// <ul>
/// <li>source(A) = []</li>
/// <li>source(A->B) = [A]</li>
/// </ul>
/// \deprecated
inline
sort_expression_list source(sort_expression s)
{
  if (s.is_arrow())
    return atermpp::list_arg1(s);
  else
    return atermpp::make_list(s);
}

/// Returns the target of the sort.
/// <ul>
/// <li>target(A) = A</li>
/// <li>target(A->B) = B</li>
/// </ul>
/// \deprecated
inline
sort_expression target(sort_expression s)
{
  if (s.is_arrow())
    return atermpp::arg2(s);
  else
    return s;
}

/// Accessor functions and predicates for sort expressions.
namespace sort_expr {

  /// Returns the predefined sort_expression real.
  inline
  sort_expression real()
  {
    return core::gsMakeSortExprReal();
  }
  
  /// Returns the predefined sort_expression int.
  inline
  sort_expression int_()
  {
    return core::gsMakeSortExprInt();
  }
  
  /// Returns the predefined sort_expression pos.
  inline
  sort_expression pos()
  {
    return core::gsMakeSortExprPos();
  }
  
  /// Returns the predefined sort_expression nat.
  inline
  sort_expression nat()
  {
    return core::gsMakeSortExprNat();
  }
  
  /// Returns the predefined sort_expression bool.
  inline
  sort_expression bool_()
  {
    return core::gsMakeSortExprBool();
  }

  /// \brief Returns true if the term t equals the sort_expression real
  inline bool is_real(aterm_appl t) { return t == real(); }

  /// \brief Returns true if the term t equals the sort_expression int
  inline bool is_int (aterm_appl t) { return t == int_(); }

  /// \brief Returns true if the term t equals the sort_expression pos
  inline bool is_pos (aterm_appl t) { return t == pos(); }

  /// \brief Returns true if the term t equals the sort_expression nat
  inline bool is_nat (aterm_appl t) { return t == nat(); }                                 

  /// \brief Returns true if the term t equals the sort_expression bool
  inline bool is_bool(aterm_appl t) { return t == bool_(); }                                 

} // namespace sort_expr

} // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
using mcrl2::data::sort_expression;

template<>
struct aterm_traits<sort_expression>
{
  typedef ATermAppl aterm_type;
  static void protect(sort_expression t)   { t.protect(); }
  static void unprotect(sort_expression t) { t.unprotect(); }
  static void mark(sort_expression t)      { t.mark(); }
  static ATerm term(sort_expression t)     { return t.term(); }
  static ATerm* ptr(sort_expression& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_DATA_SORT_EXPRESSION_H
