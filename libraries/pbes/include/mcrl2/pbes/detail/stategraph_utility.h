// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_utility.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_UTILITY_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_UTILITY_H

#include "mcrl2/pbes/pbes_equation.h"





namespace mcrl2::pbes_system::detail {

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
  for (auto i = v.begin(); i != v.end(); ++i)
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
  for (auto j = v.begin(); j != v.end(); ++j)
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

} // namespace mcrl2::pbes_system::detail





#endif // MCRL2_PBES_DETAIL_STATEGRAPH_UTILITY_H
