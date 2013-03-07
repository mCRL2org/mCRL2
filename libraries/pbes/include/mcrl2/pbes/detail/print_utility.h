// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/print_utility.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PRINT_UTILITY_H
#define MCRL2_PBES_DETAIL_PRINT_UTILITY_H

#include <set>
#include <sstream>
#include <string>
#include "mcrl2/data/variable.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct data_printer
{
  template <typename T>
  std::string operator()(const T& x) const
  {
    return data::pp(x);
  }
};

struct pbes_printer
{
  template <typename T>
  std::string operator()(const T& x) const
  {
    return pbes_system::pp(x);
  }
};

template <typename Set, typename Printer>
std::string print_set(const Set& v, Printer print)
{
  std::ostringstream out;
  out << "{";
  for (typename Set::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    if (i != v.begin())
    {
      out << ", ";
    }
    out << print(*i);
  }
  out << "}";
  return out.str();
}

inline
std::string print_variable_set(const std::set<data::variable>& v)
{
  return print_set(v, data_printer());
/*
  std::ostringstream out;
  out << "{";
  for (std::set<data::variable>::const_iterator j = v.begin(); j != v.end(); ++j)
  {
    if (j != v.begin())
    {
      out << ", ";
    }
    out << data::pp(*j);
  }
  out << "}";
  return out.str();
*/
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PRINT_UTILITY_H
