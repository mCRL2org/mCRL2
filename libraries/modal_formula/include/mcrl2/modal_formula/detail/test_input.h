// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/test_input.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_DETAIL_TEST_INPUT_H
#define MCRL2_MODAL_FORMULA_DETAIL_TEST_INPUT_H

#include <string>

namespace mcrl2 {

namespace lps {

namespace detail {

inline
std::string NO_DEADLOCK()
{
  return "[true*]<true>true";
}

inline
std::string NO_LIVELOCK()
{
  return "[true*]<true>true";
}

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_DETAIL_TEST_INPUT_H
