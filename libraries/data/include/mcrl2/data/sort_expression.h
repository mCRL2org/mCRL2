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
#include "mcrl2/core/down_cast.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/struct_core.h" // for gsIsSortExpr
#include "mcrl2/core/detail/soundness_checks.h"

namespace mcrl2
{

namespace data
{

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
inline bool is_untyped_sort(const atermpp::aterm_appl& p)
{
  return core::detail::gsIsUntypedSortUnknown(p);
}

/// \brief Returns true if the term t is an expression for multiple possible sorts
inline bool is_untyped_possible_sorts(const atermpp::aterm_appl& p)
{
  return core::detail::gsIsUntypedSortsPossible(p);
}

//--- start generated class sort_expression ---//
/// \brief A sort expression
class sort_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    sort_expression()
      : atermpp::aterm_appl(core::detail::constructSortExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit sort_expression(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_SortExpr(*this));
    }
//--- start user section sort_expression ---//
    /// \brief Returns the target sort of this expression.
    /// \return For a function sort D->E, return the target sort of E. Otherwise return this sort.
    inline
    const sort_expression& target_sort() const
    {
      if (is_function_sort(*this))
      {
        return atermpp::aterm_cast<const sort_expression>(atermpp::arg2(*this));
      }
      else
      {
        return atermpp::aterm_cast<const sort_expression>(*this);
      }
    }
//--- end user section sort_expression ---//
};

/// \brief list of sort_expressions
typedef atermpp::term_list<sort_expression> sort_expression_list;

/// \brief vector of sort_expressions
typedef std::vector<sort_expression>    sort_expression_vector;

/// \brief swap overload
inline void swap(sort_expression& t1, sort_expression& t2)
{
  t1.swap(t2);
}

//--- end generated class sort_expression ---//

/// \brief Test for a sort_expression expression
/// \param x A term
/// \return True if it is a sort_expression expression
inline
bool is_sort_expression(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsSortId(x) ||
         core::detail::gsIsSortCons(x) ||
         core::detail::gsIsSortStruct(x) ||
         core::detail::gsIsSortArrow(x) ||
         core::detail::gsIsUntypedSortUnknown(x) ||
         core::detail::gsIsUntypedSortsPossible(x);
}

// template function overloads
std::string pp(const sort_expression& x);
std::string pp(const sort_expression_list& x);
std::string pp(const sort_expression_vector& x);
std::set<data::sort_expression> find_sort_expressions(const data::sort_expression& x);

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SORT_EXPRESSION_H

