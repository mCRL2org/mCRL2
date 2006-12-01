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
  template <typename T0>
  aterm_list make_list(T0 t0)
  {
    return aterm_list(ATmakeList1(aterm_traits<T0>::term(t0)));
  }
  
  template <typename T0, typename T1>
  aterm_list make_list(T0 t0, T1 t1)
  {
    return aterm_list(ATmakeList2(aterm_traits<T0>::term(t0), aterm_traits<T1>::term(t1)));
  }
  
  template <typename T0, typename T1, typename T2>
  aterm_list make_list(T0 t0, T1 t1, T2 t2)
  {
    return aterm_list(ATmakeList3(aterm_traits<T0>::term(t0), aterm_traits<T1>::term(t1), aterm_traits<T2>::term(t2)));
  }

  template <typename T0, typename T1, typename T2, typename T3>
  aterm_list make_list(T0 t0, T1 t1, T2 t2, T3 t3)
  {
    return aterm_list(ATmakeList4(aterm_traits<T0>::term(t0), aterm_traits<T1>::term(t1), aterm_traits<T2>::term(t2), aterm_traits<T3>::term(t3)));
  }

} // namespace atermpp

#endif // ATERM_MAKE_LIST_H
