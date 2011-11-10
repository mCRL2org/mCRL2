// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sort_expression.h
/// \brief The class sort_expression.

#ifndef MCRL2_DATA_SORT_EXPRESSION_H
#define MCRL2_DATA_SORT_EXPRESSION_H

#include <set>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/struct_core.h" // for gsIsSortExpr
#include "mcrl2/atermpp/convert.h"

namespace mcrl2
{

namespace data
{

/// \brief Returns true if the term t is a sort_expression
/// \param t A term
/// \return True if the term is a sort expression.
inline
bool is_sort_expression(const atermpp::aterm_appl& t)
{
  return core::detail::gsIsSortExpr(t);
}

/// \brief Returns true if the term t is a basic sort
inline bool is_basic_sort(const atermpp::aterm_appl& p)
{
  return core::detail::gsIsSortId(p);
}

/// \brief Returns true if the term t is a function sort
inline bool is_function_sort(const atermpp::aterm_appl& p)
{
  return core::detail::gsIsSortArrow(p);
}

/// \brief Returns true if the term t is a container sort
inline bool is_container_sort(const atermpp::aterm_appl& p)
{
  return core::detail::gsIsSortCons(p);
}

/// \brief Returns true if the term t is a structured sort
inline bool is_structured_sort(const atermpp::aterm_appl& p)
{
  return core::detail::gsIsSortStruct(p);
}

/// \brief Returns true if the term t is the unknown sort
inline bool is_unknown_sort(const atermpp::aterm_appl& p)
{
  return core::detail::gsIsSortUnknown(p);
}

/// \brief Returns true if the term t is an expression for multiple possible sorts
inline bool is_multiple_possible_sorts(const atermpp::aterm_appl& p)
{
  return core::detail::gsIsSortsPossible(p);
}

/// \brief sort expression.
///
/// A sort expression can be any of:
/// - basic sort
/// - structured sort
/// - container sort
/// - function sort
/// - alias
/// In the type checker also the following expressions can be used:
/// - unknown sort
/// - multiple possible sorts
class sort_expression: public atermpp::aterm_appl
{
  public:

    /// \brief Constructor.
    ///
    sort_expression()
      : atermpp::aterm_appl(core::detail::constructSortId())
    {}

    /// \brief Constructor.
    /// \param[in] t A term.
    /// \pre t has the internal structure of a sort expression.
    sort_expression(const atermpp::aterm_appl& t)
      : atermpp::aterm_appl(t)
    {
      assert(is_sort_expression(t));
    }

    /// \brief Returns the target sort of this expression.
    /// \return For a function sort D->E, return the target sort of E. Otherwise return this sort.
    inline
    sort_expression target_sort() const
    {
      if (is_function_sort(*this))
      {
        return atermpp::arg2(*this);
      }
      else
      {
        return *this;
      }
    }

}; // class sort_expression

/// \brief list of sorts
typedef atermpp::term_list< sort_expression >  sort_expression_list;
/// \brief vector of sorts
typedef atermpp::vector< sort_expression >     sort_expression_vector;

// template function overloads
std::string pp(const sort_expression& x);
std::string pp(const sort_expression_list& x);
std::string pp(const sort_expression_vector& x);
std::set<data::sort_expression> find_sort_expressions(const data::sort_expression& x);

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SORT_EXPRESSION_H

