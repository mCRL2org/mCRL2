// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/print_utility.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_PRINT_UTILITY_H
#define MCRL2_PROCESS_DETAIL_PRINT_UTILITY_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/process/process_expression.h"

namespace mcrl2 {

namespace process {

namespace detail {

struct process_printer
{
  template <typename T>
  std::string operator()(const T& x) const
  {
    return process::pp(x);
  }
};

/// \brief Creates a string representation of a container.
/// \param v A container
/// \param message A string
/// \param print_index If true, an index is written in front of each term
template <typename Container>
std::string print_list(const Container& v, std::string message = "", bool print_index = false)
{
  return core::detail::print_list(v, process::detail::process_printer(), message, print_index);
}

/// \brief Creates a string representation of a container.
/// \param v A container
/// \param message A string
/// \param print_index If true, an index is written in front of each term
template <typename Container>
std::string print_set(const Container& v, std::string message = "", bool print_index = false)
{
  return core::detail::print_set(v, process::detail::process_printer(), message, print_index);
}

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_PRINT_UTILITY_H
