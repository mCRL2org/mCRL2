// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/unused.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_UNUSED_H
#define MCRL2_UTILITIES_UNUSED_H

namespace mcrl2 {

namespace utilities {

/// \brief Function that can be used to silence unused parameter warnings.
template<class... T> void mcrl2_unused(T&&...)
{}

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_UNUSED_H
