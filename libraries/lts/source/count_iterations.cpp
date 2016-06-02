// Author(s): David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands
//
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file count_iterations.cpp
///
/// \brief static variables for time complexity checks during test runs
///
/// \details The macro `MAX_ITERATIONS`, defined in count_iterations.h, uses
/// some static variables, which are defined in this file.
///
/// \author David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands


#include "mcrl2/lts/detail/count_iterations.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{
namespace bisim_gjkw
{

#ifndef NDEBUG

/// These global variables serve to provide the number of states and
/// transitions, respectively. They are used by `MAX_ITERATIONS`.
state_type _count_iterations_n;
trans_type _count_iterations_m;

#endif

} // end namespace bisim_gjkw
} // end namespace detail
} // end namespace lts
} // end namespace mcrl2
