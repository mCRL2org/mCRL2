// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file toolset_revision.cpp

//Note: compilation of this file requires MCRL2_REVISION to be defined

#include "mcrl2/utilities/toolset_revision.h"

std::string mcrl2::utilities::get_toolset_revision() { return MCRL2_REVISION; }
