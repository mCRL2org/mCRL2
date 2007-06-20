// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file atermpp/make_list.h
/// \brief Add your file description here.

#ifndef MCRL2_ATERMPP_MAKE_LIST_H
#define MCRL2_ATERMPP_MAKE_LIST_H

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

#endif // MCRL2_ATERMPP_MAKE_LIST_H
