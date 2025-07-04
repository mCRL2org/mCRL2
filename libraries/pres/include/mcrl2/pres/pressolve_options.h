// Author(s): Jan Friso Groote. Based on mcrl2/pbes/pbessolve_options.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/pressolve_options.h
/// \brief This file declares the options for res solvers.

#ifndef MCRL2_PRES_PRESSOLVE_OPTIONS_H
#define MCRL2_PRES_PRESSOLVE_OPTIONS_H

#include <iomanip>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/pres/resalgorithm_type.h"



namespace mcrl2::pres_system {

struct pressolve_options
{
  data::rewrite_strategy rewrite_strategy = data::jitty;
  bool replace_constants_by_variables = false;
  bool remove_unused_rewrite_rules = false;
  solution_algorithm algorithm = gauss_elimination;
  std::size_t precision = 10; // Yield an answer with a precision of at most 10^-precision.
};

inline
std::ostream& operator<<(std::ostream& out, const pressolve_options& options)
{
  out << "rewrite-strategy = " << options.rewrite_strategy << std::endl;
  out << "replace-constants-by-variables = " << std::boolalpha << options.replace_constants_by_variables << std::endl;
  out << "remove-unused-rewrite-rules = " << std::boolalpha << options.remove_unused_rewrite_rules << std::endl;
  out << "solution-algorithm = " << (options.algorithm==gauss_elimination?"Gauss elimination":"numerical") << std::endl;
  out << "the solution has a precision of = 10^-" << options.precision << std::endl;
  return out;
}

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_PRESSOLVE_OPTIONS_H
