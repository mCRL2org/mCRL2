// ======================================================================
//
// Copyright (c) 2004, 2005, 2006 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/make_list.h
// date          : 22-9-2006
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file make_list.h
/// Contains functions for initializing a list.

#ifndef ATERM_MAKE_LIST_H
#define ATERM_MAKE_LIST_H

#include "atermpp/aterm.h"

namespace atermpp
{
  inline
  aterm_list make_list(aterm t0)
  {
    return aterm_list(ATmakeList1(t0));
  }
  
  inline
  aterm_list make_list(aterm t0, aterm t1)
  {
    return aterm_list(ATmakeList2(t0, t1));
  }
  
  inline
  aterm_list make_list(aterm t0, aterm t1, aterm t2)
  {
    return aterm_list(ATmakeList3(t0, t1, t2));
  }

  inline
  aterm_list make_list(aterm t0, aterm t1, aterm t2, aterm t3)
  {
    return aterm_list(ATmakeList4(t0, t1, t2, t3));
  }

} // namespace atermpp

#endif // ATERM_MAKE_LIST_H
