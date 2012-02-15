// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file toolset_version.cpp

//Note: compilation of this file requires MCRL2_VERSION to be defined

#include "mcrl2/utilities/toolset_version.h"

std::string mcrl2::utilities::get_toolset_version()
{
  return MCRL2_VERSION;
}
