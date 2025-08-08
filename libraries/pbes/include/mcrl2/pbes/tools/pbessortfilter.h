// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbessortfilter.h
/// \brief This file provides a tool that can simplify PBESs by
///        substituting PBES equations for variables in the rhs,
///        simplifying the result, and keeping it when it can
///        eliminate PBES variables.

#ifndef MCRL2_PBES_TOOLS_PBESCHAIN_H
#define MCRL2_PBES_TOOLS_PBESCHAIN_H

#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/utilities/file_utility.h"

namespace mcrl2::pbes_system
{

struct pbessortfilter_options
{};

inline pbes filterSort(pbes& p)
{
  data::used_data_equation_selector used_selector(p.data(),
      pbes_system::find_function_symbols(p),
      p.global_variables(),
      false);
  data::data_equation_vector equations;
  for (auto i: p.data().user_defined_equations())
  {
    if (used_selector(i))
    {
      equations.push_back(i);
    }
  }

  data::function_symbol_vector mappings;
  for (auto i: p.data().user_defined_mappings()) {
      if (used_selector(i)) {
          mappings.emplace_back(i);
      }
  }

  data::data_specification data_spec = data::data_specification(p.data().user_defined_sorts(),
      p.data().user_defined_aliases(),
      p.data().user_defined_constructors(),
      mappings,
      equations);

  return pbes(data_spec, p.global_variables(), p.equations(), p.initial_state());
}

inline void pbessortfilter(const std::string& input_filename,
    const std::string& output_filename,
    const utilities::file_format& input_format,
    const utilities::file_format& output_format,
    pbessortfilter_options)
{
  pbes p;
  load_pbes(p, input_filename, input_format);
  p = filterSort(p);
  save_pbes(p, output_filename, output_format);
}

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_TOOLS_PBESSORTFILTER_H
