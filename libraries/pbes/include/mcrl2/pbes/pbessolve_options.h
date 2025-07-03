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

namespace mcrl2::pbes_system {

///  \brief Enumeration of partial strategies for solving PBESs.
enum class partial_solve_strategy {
  no_optimisation = 0,
  remove_self_loops = 1,
  propagate_solved_equations_using_substitution = 2,
  propagate_solved_equations_using_attractor = 3,
  detect_winning_loops_using_fatal_attractor = 4,
  solve_subgames_using_fatal_attractor_local = 5,
  solve_subgames_using_fatal_attractor_original = 6,
  solve_subgames_using_solver = 7,
  detect_winning_loops_original = 8
};

inline
std::istream& operator>>(std::istream& is, partial_solve_strategy& strategy)
{
  try
  {
    int i;
    is >> i;
    strategy = static_cast<partial_solve_strategy>(i);
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

inline
std::ostream& operator<<(std::ostream& os, const partial_solve_strategy& strategy)
{
  os << static_cast<int>(strategy);
  return os;
}

struct pbessolve_options
{
  data::rewrite_strategy rewrite_strategy = data::jitty;
  bool replace_constants_by_variables = false;
  bool remove_unused_rewrite_rules = false;
  bool prune_todo_list = false;
  search_strategy exploration_strategy = breadth_first;
  partial_solve_strategy optimization = partial_solve_strategy::no_optimisation;

  // if true, apply optimization 4 and 5 at every iteration
  bool aggressive = false;

  // if true, run the naive algorithm for instantiating pbes with counter example information.
  bool naive_counter_example_instantiation = false;

  // for doing a consistency check on the computed strategy
  bool check_strategy = false;

  bool prune_todo_alternative = false;

  std::size_t number_of_threads = 1;
};

inline
std::ostream& operator<<(std::ostream& out, const pbessolve_options& options)
{
  out << "rewrite-strategy = " << options.rewrite_strategy << std::endl;
  out << "replace-constants-by-variables = " << std::boolalpha << options.replace_constants_by_variables << std::endl;
  out << "remove-unused-rewrite-rules = " << std::boolalpha << options.remove_unused_rewrite_rules << std::endl;
  out << "reset-todo = " << std::boolalpha << options.prune_todo_list << std::endl;
  out << "search-strategy = " << options.exploration_strategy << std::endl;
  out << "optimization = " << static_cast<int>(options.optimization) << std::endl;
  out << "aggressive = " << std::boolalpha << options.aggressive << std::endl;
  out << "check-strategy = " << std::boolalpha << options.check_strategy << std::endl;
  out << "prune-todo-alternative = " << std::boolalpha << options.prune_todo_alternative << std::endl;
  out << "threads = " << options.number_of_threads << std::endl;
  return out;
}

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_PBESSOLVE_OPTIONS_H
