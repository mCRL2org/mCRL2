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
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/core/detail/print_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct pbes_printer
{
  template <typename T>
  std::string operator()(const T& x) const
  {
    return pbes_system::pp(x);
  }
};

/// \brief Creates a string representation of a container.
/// \param v A container
/// \param message A string
/// \param print_index If true, an index is written in front of each term
template <typename Container>
std::string print_list(const Container& v, std::string message = "", bool print_index = false)
{
  return core::detail::print_list(v, pbes_system::detail::pbes_printer(), message, print_index);
}

/// \brief Creates a string representation of a container.
/// \param v A container
/// \param message A string
/// \param print_index If true, an index is written in front of each term
template <typename Container>
std::string print_set(const Container& v, std::string message = "", bool print_index = false)
{
  return core::detail::print_set(v, pbes_system::detail::pbes_printer(), message, print_index);
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PRINT_UTILITY_H
