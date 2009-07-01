// Author(s): Yaroslav S. Usenko
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/alpha.h
/// \brief Alphabet reductions of mCRL2 process specifications.

#ifndef PARSER_LIBALPHA_H
#define PARSER_LIBALPHA_H

#include <aterm2.h>

namespace mcrl2 {
  namespace core {

/** \brief     Apply alphabet reduction to an mCRL2 process specification.
 *  \param[in] Spec An ATerm representation of an mCRL2 process
 *             specification in the internal format after type checking.
 *  \return    Spec after applying alphabet reductions.
**/
ATermAppl gsAlpha(ATermAppl Spec);

  }
}

#endif // PARSER_LIBALPHA_H
