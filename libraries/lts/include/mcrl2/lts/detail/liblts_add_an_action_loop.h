// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/liblts_scc.h

#ifndef _LIBLTS_ADD_ACTION_LOOP_H
#define _LIBLTS_ADD_ACTION_LOOP_H
#include <vector>
#include <map>
#include "mcrl2/lts/lts.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{
namespace lts
{

namespace detail
{

template < class LTS_TYPE>
void add_an_action_loop_to_each_state(LTS_TYPE& l, size_t action)
{
  for(size_t i=0; i<l.num_states(); ++i)
  {
    l.add_transition(transition(i,action,i));
  }
}

} // namespace detail;
}
}
#endif // _LIBLTS_ADD_ACTION_LOOP_H
