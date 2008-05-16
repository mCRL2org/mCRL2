// Author(s): Yaroslav S. Usenko
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/alpha.h
/// \brief Library for applying alphabet reductions to specifications 

#ifndef PARSER_LIBALPHA_H
#define PARSER_LIBALPHA_H

#include <aterm2.h>

namespace mcrl2 {
  namespace core {

/// Applies an alphabet reduction to a specification
ATermAppl gsAlpha(ATermAppl Spec);

  }
}

#endif // PARSER_LIBALPHA_H
