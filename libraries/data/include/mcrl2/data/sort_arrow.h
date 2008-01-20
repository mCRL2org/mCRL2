// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sort_arrow.h
/// \brief The class sort_arrow.

#ifndef MCRL2_DATA_SORT_ARROW_H
#define MCRL2_DATA_SORT_ARROW_H

#include "mcrl2/data/sort_expression.h"

namespace mcrl2 {

namespace data {

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::term_list;
using namespace core::detail;

class sort_arrow;

/// \brief singly linked list of sorts
///
typedef term_list<sort_arrow> sort_arrow_list;

/// \brief sort arrow.
///
/// Models sorts of shape <tt>A -\> B</tt>.
/// 
//<SortExpr>    ::= <SortId>
//                | SortList(<SortExpr>)                                   (- di)
//                | SortSet(<SortExpr>)                                    (- di)
//                | SortBag(<SortExpr>)                                    (- di)
//                | SortStruct(<StructCons>+)                              (- di)
//                | SortArrow(<SortExpr>+, <SortExpr>)                 (- di)
//                | SortArrow(<SortExpr>, <SortExpr>)                      (+ di)
class sort_arrow: public sort_expression
{
  public:
    /// Constructor.
    ///
    sort_arrow()
      : sort_expression(core::detail::constructSortArrow())
    {}

    /// Constructor.
    ///
    sort_arrow(ATermAppl t)
      : sort_expression(t)
    {
      assert(core::detail::check_term_SortArrow(m_term));
    }

    /// Constructor.
    ///
    sort_arrow(aterm_appl t)
      : sort_expression(t)
    {
      assert(core::detail::check_term_SortArrow(m_term));
    }
   
    /// Constructor.
    ///
    sort_arrow(sort_expression_list arguments, sort_expression result)
      : sort_expression(core::detail::gsMakeSortArrow(arguments, result))
    {
      assert(core::detail::check_term_SortArrow(m_term));
    }
   
    /// \overload
    ///
    bool is_arrow() const
    {
      return true;
    }

    /// Returns the argument sorts.
    ///
    sort_expression_list argument_sorts() const
    {
      return atermpp::list_arg1(*this);
    }

    /// Returns the result sort.
    ///
    sort_expression result_sort() const
    {
      return atermpp::arg2(*this);
    }   
};

/// \brief Returns true if the term t is a sort arrow.
inline
bool is_sort_arrow(aterm_appl t)
{
  return core::detail::gsIsSortArrow(t);
}

} // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
using mcrl2::data::sort_arrow;

template<>
struct aterm_traits<sort_arrow>
{
  typedef ATermAppl aterm_type;
  static void protect(sort_arrow t)   { t.protect(); }
  static void unprotect(sort_arrow t) { t.unprotect(); }
  static void mark(sort_arrow t)      { t.mark(); }
  static ATerm term(sort_arrow t)     { return t.term(); }
  static ATerm* ptr(sort_arrow& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_DATA_SORT_ARROW_H
