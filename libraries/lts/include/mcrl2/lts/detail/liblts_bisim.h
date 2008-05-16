// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/liblts_bisim.h

#ifndef _LIBLTS_BISIM_H
#define _LIBLTS_BISIM_H
#include "mcrl2/lts/liblts.h"

namespace mcrl2
{
namespace lts
{

bool bisimulation_reduce(lts &l, bool branching = false, bool add_class_to_state = false, std::vector<std::string> const*tau_actions = NULL);
bool bisimulation_compare(lts &l1, lts &l2, bool branching = false, std::vector<std::string> const*tau_actions = NULL);

}
}

#endif
