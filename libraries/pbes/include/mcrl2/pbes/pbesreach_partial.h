// Author(s): Wieger Wesselink, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_PBES_PBESREACH_PARTIAL_H
#define MCRL2_PBES_PBESREACH_PARTIAL_H

#ifdef MCRL2_ENABLE_SYLVAN

#include <sylvan_ldd.hpp>

#include "mcrl2/pbes/pbesreach.h"
#include "mcrl2/pbes/symbolic_pbessolve.h"

namespace mcrl2::pbes_system {

/// Variant of pbesreach_algorithm that performs partial solving during
/// the computation of the symbolic parity game
class pbesreach_algorithm_partial : public pbesreach_algorithm
{
public:

  pbesreach_algorithm_partial(const srf_pbes& pbesspec, const symbolic_reachability_options& options_)
    : pbesreach_algorithm(pbesspec, options_),
    m_partial_solution(symbolic_solution_t())
  {}

  void on_end_while_loop() override
  {
    time_exploring += explore_timer.seconds();
    ++iteration_count;

    // if (iteration_count % 10 == 0 || m_options.aggressive)
    if (time_solving * 10 < (time_solving + time_exploring) || m_options.aggressive)
    {
      mCRL2log(log::verbose) << "start partial solving\n";
      stopwatch timer;

      ldd V = union_(m_visited, m_todo);
      symbolic_parity_game G(pbes(),
        summand_groups(),
        data_index(),
        V,
        m_options.no_relprod,
        m_options.chaining,
        m_options.check_strategy);
      G.print_information();
      symbolic_pbessolve_algorithm solver(G);

      if (m_options.solve_strategy == 1)
      {
        m_partial_solution = solver.detect_solitair_cycles(m_initial_vertex,
          V,
          m_todo,
          false,
          m_deadlocks,
          m_partial_solution);
      }
      else if (m_options.solve_strategy == 2)
      {
        m_partial_solution = solver.detect_solitair_cycles(m_initial_vertex,
          V,
          m_todo,
          true,
          m_deadlocks,
          m_partial_solution);
      }
      else if (m_options.solve_strategy == 3)
      {
        m_partial_solution = solver.detect_forced_cycles(m_initial_vertex,
          V,
          m_todo,
          false,
          m_deadlocks,
          m_partial_solution);
      }
      else if (m_options.solve_strategy == 4)
      {
        m_partial_solution = solver.detect_forced_cycles(m_initial_vertex,
          V,
          m_todo,
          true,
          m_deadlocks,
          m_partial_solution);
      }
      else if (m_options.solve_strategy == 5)
      {
        std::tie(m_partial_solution.winning[0], m_partial_solution.winning[1])
          = solver.detect_fatal_attractors(m_initial_vertex,
            V,
            m_todo,
            false,
            m_deadlocks,
            m_partial_solution.winning[0],
            m_partial_solution.winning[1]);
      }
      else if (m_options.solve_strategy == 6)
      {
        std::tie(m_partial_solution.winning[0], m_partial_solution.winning[1])
          = solver.detect_fatal_attractors(m_initial_vertex,
            V,
            m_todo,
            true,
            m_deadlocks,
            m_partial_solution.winning[0],
            m_partial_solution.winning[1]);
      }
      else if (m_options.solve_strategy == 7)
      {
        m_partial_solution
          = solver.partial_solve(m_initial_vertex, V, m_todo, m_deadlocks, m_partial_solution);
      }

      mCRL2log(log::verbose) << "found solution for" << std::setw(12) << satcount(m_partial_solution.winning[0]) + satcount(m_partial_solution.winning[1])
                             << " BES equations" << std::endl;
      mCRL2log(log::verbose) << "finished partial solving (time = " << std::setprecision(2) << std::fixed
                             << timer.seconds() << "s)\n";

      time_solving += timer.seconds();
    }

    explore_timer.reset();
  }

  bool solution_found() const override
  {
    if (includes(m_partial_solution.winning[0], m_initial_vertex))
    {
      return true;
    }
    else if (includes(m_partial_solution.winning[1], m_initial_vertex))
    {
      return true;
    }

    return false;
  }

  symbolic_solution_t partial_solution() const override { return m_partial_solution; }

private:
  /// Partial solution that has already been computed.
  symbolic_solution_t m_partial_solution;
  std::size_t iteration_count = 0;

  double time_solving = 0.0;
  double time_exploring = 0.0;
  stopwatch explore_timer;
};

} // namespace mcrl2::pbes_system

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_PBES_PBESREACH_PARTIAL_H
