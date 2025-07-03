// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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

namespace mcrl2::lps::detail {

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

} // namespace mcrl2::lps::detail





#endif // MCRL2_MODAL_FORMULA_DETAIL_TEST_INPUT_H
