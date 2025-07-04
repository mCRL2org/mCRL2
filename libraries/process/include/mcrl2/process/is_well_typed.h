// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/is_well_typed.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_IS_WELL_TYPED_H
#define MCRL2_PROCESS_IS_WELL_TYPED_H

#include "mcrl2/process/process_specification.h"

namespace mcrl2::process
{

/// \brief Returns true if the process specification is well typed. N.B. The check is very incomplete!
inline
bool is_well_typed(const process_specification& procspec)
{
  std::set<data::variable> free_variables = process::find_free_variables(procspec);
  return free_variables.empty();
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_IS_WELL_TYPED_H
