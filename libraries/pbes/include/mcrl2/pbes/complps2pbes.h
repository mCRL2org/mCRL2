// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/complps2pbes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_COMPLPS2PBES_H
#define MCRL2_PBES_COMPLPS2PBES_H

#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/process/is_communicating_lpe.h"
#include "mcrl2/process/is_linear.h"

namespace mcrl2::pbes_system {

inline
pbes_system::pbes complps2pbes(const process::process_specification& procspec, const state_formulas::state_formula& /* formula */)
{
  for (const process::process_equation& equation: procspec.equations())
  {
    if (!process::is_linear(equation))
    {
      throw mcrl2::runtime_error("the equation for " + process::pp(equation.identifier()) + " is not linear!");
    }
  }

  if (!process::is_communicating_lpe(procspec.init()))
  {
    throw mcrl2::runtime_error("the initial state " + process::pp(procspec.init()) + " is not in communicating LPE format!");
  }

  return pbes_system::pbes();
}

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_COMPLPS2PBES_H
