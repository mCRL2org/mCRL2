// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/utility.h
/// \brief Utility functions for the pbes library.

#ifndef MCRL2_PBES_UTILITY_H
#define MCRL2_PBES_UTILITY_H

#include <sstream>
#include "mcrl2/atermpp/algorithm.h"

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/sort_expression.h"

#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/detail/rewriter_wrapper.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/print.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

/// \brief gives the rank of a propositional_variable_instantiation. It is assumed that the variable
/// occurs in the pbes_specification.

inline unsigned long get_rank(const propositional_variable_instantiation& current_variable_instantiation,
                              const pbes& pbes_spec)
{
  unsigned long rank=0;
  const std::vector<pbes_equation>& eqsys = pbes_spec.equations();
  fixpoint_symbol current_fixpoint_symbol=eqsys.begin()->symbol();
  for (auto eqi = eqsys.begin(); eqi != eqsys.end(); eqi++)
  {
    if (eqi->variable().name()==current_variable_instantiation.name())
    {
      return rank;
    }
    if (eqi->symbol()!=current_fixpoint_symbol)
    {
      current_fixpoint_symbol=eqi->symbol();
      rank=rank+1;
    }
  }
  assert(0); // It is assumed that the current_variable_instantiation occurs in the pbes_spec.
  return 0;
}


/// \brief gives the fixed point symbol of a propositional_variable_instantiation. It is assumed that the variable
/// occurs in the pbes_specification. Returns false if the symbol is mu, and true if the symbol in nu.

inline bool get_fixpoint_symbol(const propositional_variable_instantiation current_variable_instantiation,
                                const pbes& pbes_spec)
{
  const std::vector<pbes_equation>& eqsys = pbes_spec.equations();
  for (auto eqi = eqsys.begin(); eqi != eqsys.end(); eqi++)
  {
    if (eqi->variable().name()==current_variable_instantiation.name())
    {
      return (eqi->symbol().is_nu());
    }
  }
  assert(0); // It is assumed that the variable instantiation occurs in the PBES.
  return false;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_UTILITY_H
