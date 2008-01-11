// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/transform.h
/// \brief Transform algorithm.

#ifndef MCRL2_ATERMPP_TRANSFORM_H
#define MCRL2_ATERMPP_TRANSFORM_H

#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

  /// Applies the function f to the list l. If l = [x0, x1, ..., xn] then
  /// the list [f(x0), f(x1), ..., f(xn)] is returned.
  ///
  template <typename Func>
  ATermList transform(ATermList l, const Func& f)
  {
    if (ATgetLength(l) > 0)
      return ATinsert(transform(ATgetNext(l), f), f(ATgetFirst(l)));
    else
      return l;
  }
  
} // namespace atermpp

#endif // MCRL2_ATERMPP_TRANSFORM_H
