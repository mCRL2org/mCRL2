// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_CONFIGURATION_H
#define MCRL2_UTILITIES_CONFIGURATION_H

namespace mcrl2
{
namespace utilities
{
namespace detail
{

/// \brief Enables thread safety for the global term and function symbol pools.
/// Outcomment to compile the toolset without thread safe features. 
/// This is a define, because it is used to determine whether data structures are
/// determined via the keyword thread_local or via static. 
// #define MCRL2_THREAD_SAFE

} // namespace detail

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_CONFIGURATION_H

