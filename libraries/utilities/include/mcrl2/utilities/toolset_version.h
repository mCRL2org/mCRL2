// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/toolset_version.h
///
/// \brief Get the toolset revision

#ifndef MCRL2_UTILITIES_TOOLSET_VERSION_H
#define MCRL2_UTILITIES_TOOLSET_VERSION_H

#include <string>

namespace mcrl2::utilities
{

/// \brief  Get the toolset revision
/// \return A string representation of the toolset revision
std::string get_toolset_version();

} // namespace mcrl2::utilities


#endif // MCRL2_UTILITIES_TOOLSET_VERSION_H
