// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/down_cast.h
/// \brief Function to explicitly down cast expressions in a hierarchy.

#ifndef MCRL2_CORE_DOWN_CAST_H
#define MCRL2_CORE_DOWN_CAST_H

#include "mcrl2/atermpp/aterm.h"

namespace mcrl2
{

namespace core
{

template <class T, class U>
T down_cast(U t,
            typename std::enable_if<std::is_base_of<U, T>::value >::type* = 0,
            typename std::enable_if<std::is_base_of<U, atermpp::aterm>::value >::type* = 0)
{
  static_assert(sizeof(T)==sizeof(atermpp::aterm),
                "Size of types should be equal to the size of an atermpp::aterm");
  return (T)t;
}

template <class T, class U>
T down_cast(U t,
            typename std::enable_if<std::is_base_of<U, T>::value >::type* = 0,
            typename std::enable_if<!std::is_base_of<U, atermpp::aterm>::value >::type* = 0,
            typename std::enable_if<std::is_convertible<U, T>::value>::type* = 0)
{
  return (T)t;
}


template <class T, class U>
T down_cast(U t,
            typename std::enable_if<std::is_base_of<U, T>::value >::type* = 0,
            typename std::enable_if<!std::is_base_of<U, atermpp::aterm>::value >::type* = 0,
            typename std::enable_if<!std::is_convertible<U, T>::value>::type* = 0)
{
  return T(t);
}

} // namespace core

} // namespace mcrl2

#endif // MCRL2_BASIC_IDENTIFIER_STRING_H
