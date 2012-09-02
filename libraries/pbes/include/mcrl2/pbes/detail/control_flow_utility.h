// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/control_flow_utility.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_CONTROL_FLOW_UTILITY_H
#define MCRL2_PBES_DETAIL_CONTROL_FLOW_UTILITY_H

#include "mcrl2/pbes/detail/pfnf_pbes.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
bool is_constant(const data::data_expression& x)
{
  typedef core::term_traits<data::data_expression> tr;
  return tr::is_constant(x);
}

inline
std::vector<pfnf_equation>::const_iterator find_equation(const pfnf_pbes& p, const core::identifier_string& X)
{
  const std::vector<pfnf_equation>& equations = p.equations();
  for (std::vector<pfnf_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
  {
    if (i->variable().name() == X)
    {
      return i;
    }
  }
  return equations.end();
}

inline
std::string print_pbes_expressions(const atermpp::set<pbes_expression>& v)
{
  std::ostringstream out;
  out << "{";
  for (atermpp::set<pbes_expression>::const_iterator j = v.begin(); j != v.end(); ++j)
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

#endif // MCRL2_PBES_DETAIL_CONTROL_FLOW_UTILITY_H
