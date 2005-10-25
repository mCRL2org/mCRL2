// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/replace.h
// date          : 25-10-2005
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#ifndef ATERM_REPLACE_H
#define ATERM_REPLACE_H

/** @file
  */

#include <iostream>
#include <vector>
#include "atermpp/aterm.h"
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_list.h"

namespace atermpp
{

  /// Returns a copy of aterm t in which all occurrences of src
  /// have been replaced with dest.
  ///
  inline
  aterm replace(aterm src, aterm dest, aterm t)
  {
    if (t == src)
      return dest;
  
    if (t.type() == AT_APPL)
    {
      if (aterm_appl(t).function().arity() == 0)
        return t;
      aterm_list args = aterm_appl(t).argument_list();
      std::vector<aterm> v;
      for (aterm_list::iterator i = args.begin(); i != args.end(); ++i)
      {
        v.push_back(replace(src, dest, *i));
      }
      return aterm_appl(aterm_appl(t).function(), aterm_list(v.begin(), v.end()));
    }
    else if (t.type() == AT_LIST)
    {
      if (aterm_list(t).size() == 0)
        return t;
      aterm_list args(t);
      std::vector<aterm_appl> v;
      for (aterm_list::iterator i = args.begin(); i != args.end(); ++i)
      {
        v.push_back(replace(src, dest, *i));
      }
      return aterm_list(v.begin(), v.end());
    }
    return t;
  }
  
} // namespace atermpp

#endif // ATERM_REPLACE_H
