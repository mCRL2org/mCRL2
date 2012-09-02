// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/complps2pbes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_COMPLPS2PBES_H
#define MCRL2_PBES_COMPLPS2PBES_H

#include <string>
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/process/is_linear.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/detail/is_communicating_lpe.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace pbes_system {

void complps2pbes(const std::string& input_filename,
                  const std::string& output_filename,
                  const std::string& formfilename
                 );

inline
pbes_system::pbes<> complps2pbes(const process::process_specification& procspec, const state_formulas::state_formula& /* formula */)
{
  const atermpp::vector<process::process_equation>& equations = procspec.equations();
  for (atermpp::vector<process::process_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
  {
    if (!process::is_linear(*i))
    {
      throw mcrl2::runtime_error("the equation for " + process::pp(i->identifier()) + " is not linear!");
    }
  }

  if (!process::detail::is_communicating_lpe(procspec.init()))
  {
    throw mcrl2::runtime_error("the initial state " + process::pp(procspec.init()) + " is not in communicating LPE format!");
  }

  return pbes_system::pbes<>();
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_COMPLPS2PBES_H
