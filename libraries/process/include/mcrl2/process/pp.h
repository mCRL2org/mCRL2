// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/pp.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_PP_H
#define MCRL2_PROCESS_PP_H

#include "mcrl2/data/print.h"

namespace mcrl2 {

namespace process {

/// \brief Pretty prints a term.
/// \param[in] t A term
template <typename T>
std::string pp(const T& t, typename atermpp::detail::disable_if_container<T>::type* = 0)
{
  return core::pp(t);
}

/// \brief Pretty prints the contents of a container
/// \param[in] t A container
template <typename T>
inline std::string pp(const T& t, typename atermpp::detail::enable_if_container<T>::type* = 0)
{
  return data::pp(t);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PP_H
