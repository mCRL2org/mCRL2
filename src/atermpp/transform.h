// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/transform.h
// date          : 15-11-2005
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file aterm_transform.h
/// Contains the definition of the transform algorithm.

#ifndef ATERM_TRANSFORM_H
#define ATERM_TRANSFORM_H

#include "atermpp/aterm.h"

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

#endif // ATERM_TRANSFORM_H
