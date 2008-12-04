// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/lps2pbes.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_LPS2PBES_H
#define MCRL2_PBES_LPS2PBES_H

#include <string>
#include "mcrl2/modal_formula/detail/algorithms.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_translate.h"
#include "mcrl2/lps/mcrl22lps.h"

namespace mcrl2 {

namespace pbes_system {

  inline
  pbes<> lps2pbes(const lps::specification& spec, const modal::state_formula& formula, bool timed)
  {
    return pbes_translate(formula, spec, timed);
  }

  inline
  pbes<> lps2pbes(const std::string& spec_text, const std::string& formula_text, bool timed)
  {
    pbes<> result;
    lps::specification spec = lps::mcrl22lps(spec_text);
    modal::state_formula f = modal::detail::mcf2statefrm(formula_text, spec);
    return lps2pbes(spec, f, timed);
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_LPS2PBES_H
