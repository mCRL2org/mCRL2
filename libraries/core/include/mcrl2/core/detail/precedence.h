// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/precedence.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_DETAIL_PRECEDENCE_H
#define MCRL2_CORE_DETAIL_PRECEDENCE_H

namespace mcrl2 {

namespace core {

namespace detail {

namespace precedences {

const int max_precedence = 10000;

template <typename T>
int precedence(const T&)
{
  return max_precedence;
}

} // namespace precedences

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_PRECEDENCE_H
