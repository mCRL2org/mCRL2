// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file toolset_revision.cpp

#include "mcrl2/utilities/toolset_revision.h"

namespace mcrl2 {
  namespace utilities {

//Note: compilation of this file requires MCRL2_REVISION to be defined

std::string get_toolset_revision() {
  return MCRL2_REVISION;
}

  }
}

