// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/toolset_revision.h
///
/// \brief Get the toolset revision

#ifndef MCRL2_TOOLSET_REVISION_H
#define MCRL2_TOOLSET_REVISION_H

#include <string>

namespace mcrl2 {
  namespace utilities {

/// \brief  Get the toolset revision
/// \return A string representation of the toolset revision
std::string get_toolset_revision();

  }
}

#endif // MCRL2_TOOLSET_REVISION_H
