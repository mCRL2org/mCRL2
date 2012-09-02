// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/stack.h
/// \brief Protected stack container.

#ifndef MCRL2_ATERMPP_STACK_H
#define MCRL2_ATERMPP_STACK_H

#include <stack>
#include "mcrl2/atermpp/deque.h"

namespace atermpp
{

/// \brief Protected stack container.
template <class T>
class stack: public std::stack<T, atermpp::deque<T> >
{
  public:
    /// \brief Constructor.
    stack()
    {}

    /// \brief Constructor.
    /// \param a An allocator.
    explicit stack(const atermpp::deque<T>& a)
      : std::stack<T, atermpp::deque<T> >(a)
    {}
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_STACK_H
