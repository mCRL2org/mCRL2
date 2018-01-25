// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/builder.h
/// \brief add your file description here.

#ifndef MCRL2_ATERMPP_BUILDER_H
#define MCRL2_ATERMPP_BUILDER_H

#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/utilities/exception.h"

namespace atermpp {

template <typename Derived>
struct builder
{
  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  // Enter object
  template <typename T>
  void enter(const T&)
  {}

  // Leave object
  template <typename T>
  void leave(const T&)
  {}

  aterm apply(const aterm_int& x)
  {
    derived().enter(x);
    derived().leave(x);
    return x;
  }

  aterm apply(const aterm_list& x)
  {
    derived().enter(x);
    aterm_list result(x.begin(), x.end(), [&](const aterm& v) { return derived().apply(v); } ) ;
    derived().leave(x);
    return result;
  }

  aterm apply(const aterm_appl& x)
  {
    derived().enter(x);
    aterm_appl result(x.function() , x.begin(), x.end(), [&](const aterm& v) { return derived().apply(v); } );
    derived().leave(x);
    return result;
  }

  aterm apply(const aterm& x)
  {
    derived().enter(x);
    aterm result;
    if (x.type_is_appl())
    {
      result = derived().apply(atermpp::down_cast<aterm_appl>(x));
    }
    else if (x.type_is_list())
    {
      result = derived().apply(atermpp::down_cast<aterm_list>(x));
    }
    else if (x.type_is_int())
    {
      result = derived().apply(atermpp::down_cast<aterm_int>(x));
    }
    derived().leave(x);
    return result;
  }
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_BUILDER_H
