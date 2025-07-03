// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/skip.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_SKIP_H
#define MCRL2_UTILITIES_SKIP_H

namespace mcrl2::utilities
{

/// \brief The skip operation with a variable number of arguments
struct skip
{
  template<typename... Args>
  void operator()(const Args&...) const {}
};

} // namespace mcrl2::utilities

#endif // MCRL2_UTILITIES_SKIP_H
