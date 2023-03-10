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

namespace mcrl2::utilities::detail
{

/// Enables thread safety for the whole toolset.
#ifdef MCRL2_THREAD_SAFE 
  constexpr static bool GlobalThreadSafe = true;
#else
  constexpr static bool GlobalThreadSafe = false;
#endif 

}

#endif // MCRL2_UTILITIES_CONFIGURATION_H