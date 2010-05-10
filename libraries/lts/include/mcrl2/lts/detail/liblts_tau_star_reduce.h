// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/liblts_tau_star_reduce.h

#ifndef _LIBLTS_TAUSTARREDUCE_H
#define _LIBLTS_TAUSTARREDUCE_H
#include "mcrl2/lts/lts.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

//Replace sequences tau* a tau* by a single action a.
void tau_star_reduce(lts &l);

}
}
}
#endif // _LIBLTS_TAUSTARREDUCE_H
