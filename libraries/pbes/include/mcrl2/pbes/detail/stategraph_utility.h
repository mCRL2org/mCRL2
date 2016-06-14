// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_utility.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_UTILITY_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_UTILITY_H

#include <sstream>
#include "mcrl2/pbes/pbes_equation.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
std::string print_equation(const pbes_equation& eq)
{
  return (eq.symbol().is_mu() ? "mu " : "nu ")  + pbes_system::pp(eq.variable()) + " = " + pbes_system::pp(eq.formula());
}

inline
std::string print_set(const std::set<std::size_t>& v)
{
  std::ostringstream out;
  out << "{";
  for (std::set<std::size_t>::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    if (i != v.begin())
    {
      out << ", ";
    }
    out << *i;
  }
  out << "}";
  return out.str();
}

inline
const data::data_expression& nth_element(const data::data_expression_list& e, std::size_t n)
{
  assert(n < e.size());
  data::data_expression_list::const_iterator i = e.begin();
  for (std::size_t j = 0; j < n; ++j)
  {
    ++i;
  }
  return *i;
}

inline
std::string print_pbes_expressions(const std::set<pbes_expression>& v)
{
  std::ostringstream out;
  out << "{";
  for (std::set<pbes_expression>::const_iterator j = v.begin(); j != v.end(); ++j)
  {
    if (j != v.begin())
    {
      out << ", ";
    }
    out << pbes_system::pp(*j);
  }
  out << "}";
  return out.str();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_UTILITY_H
