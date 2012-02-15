// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//


#ifndef MCRL2_LTS_DETAIL_TRANSITION_H
#define MCRL2_LTS_DETAIL_TRANSITION_H

#include "mcrl2/lts/transition.h"

namespace mcrl2
{
namespace lts
{

namespace detail
{

inline bool compare_transitions_slt(const transition t1, const transition t2)
{
  if (t1.from() != t2.from())
  {
    return t1.from() < t2.from();
  }
  else if (t1.label() != t2.label())
  {
    return t1.label() < t2.label();
  }
  else
  {
    return t1.to() < t2.to();
  }
}

inline bool compare_transitions_lts(const transition t1, const transition t2)
{
  if (t1.label() != t2.label())
  {
    return t1.label() < t2.label();
  }
  else if (t1.to() != t2.to())
  {
    return t1.to() < t2.to();
  }
  else
  {
    return t1.from() < t2.from();
  }
}

} // detail
} // lts
} // mcrl2

#endif
