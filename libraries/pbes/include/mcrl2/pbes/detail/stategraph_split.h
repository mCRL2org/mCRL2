// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_split.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_SPLIT_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_SPLIT_H

#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/utilities/detail/join.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief The namespace for access functions that operate on both pbes and data expressions
namespace combined_access
{

/// \brief Test for a conjunction
/// \param t A PBES expression
/// \return True if it is a conjunction
inline bool is_and(const pbes_expression& t)
{
  return is_pbes_and(t) || data::sort_bool::is_and_application(t);
}

/// \brief Test for a disjunction
/// \param t A PBES expression
/// \return True if it is a disjunction
inline bool is_or(const pbes_expression& t)
{
  return is_pbes_or(t) || data::sort_bool::is_or_application(t);
}

/// \brief Returns the left hand side of an expression of type and, or or imp.
/// \param t A PBES expression or a data expression
/// \return The left hand side of an expression of type and, or or imp.
inline
pbes_expression left(const pbes_expression& t)
{
  return accessors::data_left(t);
}

/// \brief Returns the left hand side of an expression of type and, or or imp.
/// \param t A PBES expression or a data expression
/// \return The left hand side of an expression of type and, or or imp.
inline
pbes_expression right(const pbes_expression& t)
{
  return accessors::data_right(t);
}

} // namespace combined_access

inline
void stategraph_split_and(const pbes_expression& expr, std::vector<pbes_expression>& result)
{
  namespace a = combined_access;
  utilities::detail::split(expr, std::back_inserter(result), a::is_and, a::left, a::right);
}

inline
void stategraph_split_or(const pbes_expression& expr, std::vector<pbes_expression>& result)
{
  namespace a = combined_access;
  utilities::detail::split(expr, std::back_inserter(result), a::is_or, a::left, a::right);
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_SPLIT_H
