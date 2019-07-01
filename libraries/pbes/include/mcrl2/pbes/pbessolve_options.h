// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbessolve_options.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESSOLVE_OPTIONS_H
#define MCRL2_PBES_PBESSOLVE_OPTIONS_H

#include <iomanip>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/pbes/search_strategy.h"

namespace mcrl2 {

namespace pbes_system {

struct pbessolve_options
{
  data::rewrite_strategy rewrite_strategy = data::jitty;
  bool replace_constants_by_variables = false;
  bool prune_todo_list = false;
  search_strategy exploration_strategy = breadth_first;
  int optimization = 0;

  // if true, apply optimization 4 and 5 at every iteration
  bool aggressive = false;

  // for doing a consistency check on the computed strategy
  bool check_strategy = false;

  bool prune_todo_alternative = false;
};

inline
std::ostream& operator<<(std::ostream& out, const pbessolve_options& options)
{
  out << "rewrite-strategy = " << options.rewrite_strategy << std::endl;
  out << "replace-constants-by-variables = " << std::boolalpha << options.replace_constants_by_variables << std::endl;
  out << "reset-todo = " << std::boolalpha << options.prune_todo_list << std::endl;
  out << "search-strategy = " << options.exploration_strategy << std::endl;
  out << "optimization = " << options.optimization << std::endl;
  out << "aggressive = " << std::boolalpha << options.aggressive << std::endl;
  out << "check-strategy = " << std::boolalpha << options.check_strategy << std::endl;
  out << "prune-todo-alternative = " << std::boolalpha << options.prune_todo_alternative << std::endl;
  return out;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESSOLVE_OPTIONS_H
