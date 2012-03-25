// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/garbage_collection.h
/// \brief Function to force garbage collection of the ATerm Library.

#ifndef MCRL2_CORE_GARBAGE_COLLECTION_H
#define MCRL2_CORE_GARBAGE_COLLECTION_H

#include "mcrl2/aterm/gc.h"

namespace mcrl2
{

namespace core
{

/// \brief Calls the ATerm Library garbage collector if the flag MCRL2_WITH_GARBAGE_COLLECTION is set.
inline void garbage_collect()
{
#ifdef MCRL2_WITH_GARBAGE_COLLECTION
  aterm::AT_collect(false);
#endif
}

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_GARBAGE_COLLECTION_H
