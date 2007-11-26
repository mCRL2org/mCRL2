// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sort_expression_expression.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_SORT_EXPRESSION_H
#define MCRL2_DATA_SORT_EXPRESSION_H

#include <cassert>
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "atermpp/aterm_access.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/struct.h"

/// The namespace of the mCRL2 tool set (will be renamed to mcrl2).
namespace lps {

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::term_list;

class sort_expression;

///////////////////////////////////////////////////////////////////////////////
// sort_expression_list
/// \brief singly linked list of sort expressions
///
typedef term_list<sort_expression> sort_expression_list;

///////////////////////////////////////////////////////////////////////////////
// sort_expression
/// \brief sort expression.
///
/// Either a sort id or a sort arrow.
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
    sort_expression()
      : aterm_appl(detail::constructSortId())
    {}

    /// Constructs a sort_expression with internal representation t.
    ///
    sort_expression(ATermAppl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_SortExpr(m_term));
    }

    sort_expression(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_SortExpr(m_term));
    }

    /// Constructs a sort_expression from a string.
    ///
    sort_expression(std::string s)
      : aterm_appl(gsMakeSortId(gsString2ATermAppl(s.c_str())))
    {}
    
    /// Returns true if it is a sort_expression of type A -> B.
    ///
    bool is_arrow() const
    {
      return gsIsSortArrow(*this);
    }
};

/// \brief Returns true if the term t is a sort_expression
inline
bool is_sort_expression(aterm_appl t)
{
  return gsIsSortId(t) || gsIsSortArrow(t);
}

/// Returns the sort_expression 'domain -> range'.
inline
sort_expression arrow(sort_expression_list domain, sort_expression range)
{
  return gsMakeSortArrow(domain, range);
}

/// DEPRECATED Returns the domain sorts of s.
///
inline
sort_expression_list domain_sorts(sort_expression s)
{
  return gsGetSortExprDomain(s);
}

/// DEPRECATED Returns the range sort of s.
///
inline
sort_expression result_sort(sort_expression s)
{
  return gsGetSortExprResult(s);
}

/// DEPRECATED Returns the source of the sort.
/// <ul>
/// <li>source(A) = []</li>
/// <li>source(A->B) = [A]</li>
/// </ul>
///
inline
sort_expression_list source(sort_expression s)
{
  if (s.is_arrow())
    return atermpp::list_arg1(s);
  else
    return atermpp::make_list(s);
}

/// DEPRECATED Returns the target of the sort.
/// <ul>
/// <li>target(A) = A</li>
/// <li>target(A->B) = B</li>
/// </ul>
///
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
    return gsMakeSortExprReal();
  }
  
  /// Returns the predefined sort_expression int.
  inline
  sort_expression int_()
  {
    return gsMakeSortExprInt();
  }
  
  /// Returns the predefined sort_expression pos.
  inline
  sort_expression pos()
  {
    return gsMakeSortExprPos();
  }
  
  /// Returns the predefined sort_expression nat.
  inline
  sort_expression nat()
  {
    return gsMakeSortExprNat();
  }
  
  /// Returns the predefined sort_expression int.
  inline
  sort_expression bool_()
  {
    return gsMakeSortExprBool();
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

} // namespace lps

/// \internal
namespace atermpp
{
using lps::sort_expression;

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

#endif // MCRL2_DATA_SORT_EXPRESSION_H
