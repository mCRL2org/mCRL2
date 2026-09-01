// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbesfindabs.h
/// \brief This file provides a tool that enumerates all valid abstraction
///        sets for a PBES. Sets are enumerated level by level, starting with
///        the single-parameter sets, then the two-parameter sets, and so on.
///        Sets that are not data-closed are reported as not valid without
///        solving; since a superset of a not-data-closed set can still be
///        data-closed, such sets are not used for pruning. Only sets that are
///        data-closed yet whose approximation does not prove the answer of the
///        original PBES are blocked; this failure is upward closed, so a set
///        is skipped only when it contains a blocked subset. Every valid set
///        is immediately written to an output file, so partial results survive
///        an interrupted run.

#ifndef MCRL2_PBES_TOOLS_PBESFINDABS_H
#define MCRL2_PBES_TOOLS_PBESFINDABS_H

#include "mcrl2/pbes/tools/pbescegps.h"
#include "mcrl2/utilities/execution_timer.h"
#include "mcrl2/utilities/logger.h"
#include <atomic>
#include <cstddef>
#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace mcrl2::pbes_system
{

/// \brief Options for the all-abstraction-sets tool.
struct pbesfindabs_options
{
  pbescegps_options cepgps; ///< Options used for constructing and solving approximations.
  std::string output_file; ///< The file to which valid abstraction sets are written.
  std::size_t number_of_threads = 1; ///< The number of abstraction sets solved in parallel.
};

/// \brief One entry of the universe of abstractable parameters: the variable
///        itself and its index inside the parameters of its equation.
struct abstractable_parameter
{
  core::identifier_string equation;
  data::variable variable;
  std::size_t index = 0;
};

// Writes valid abstraction sets to a text file. Each set is one block:
//
//   # Abstraction set 3 of size 2
//   X: b d
//   Y: c
//
// The equation lines use the same format as accepted by the --initial-state
// option of pbescegps (equation name, colon, whitespace separated parameter
// names). Sets are written incrementally: every block is flushed as soon as
// the set has been recognized as valid.
class abstraction_set_writer
{
private:
  std::ofstream m_out;
  std::mutex m_mutex;
  std::size_t m_counter = 0;

public:
  explicit abstraction_set_writer(const std::string& filename)
  {
    m_out.open(filename);
    if (!m_out.is_open())
    {
      throw mcrl2::runtime_error("Could not open file '" + filename + "' for writing abstraction sets.");
    }
    m_out << "# Valid abstraction sets, one block per set." << std::endl;
    m_out << "# Each block lists, per equation, the abstracted parameters by name." << std::endl;
  }

  void write(const std::vector<abstractable_parameter>& universe, const std::vector<std::size_t>& set)
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    m_out << "# Abstraction set " << m_counter++ << " of size " << set.size() << std::endl;
    std::map<core::identifier_string, std::vector<std::string>> names_per_equation;
    for (const std::size_t i: set)
    {
      names_per_equation[universe[i].equation].push_back(universe[i].variable.name());
    }
    for (const auto& [equation, names]: names_per_equation)
    {
      m_out << equation << ":";
      for (const std::string& name: names)
      {
        m_out << " " << name;
      }
      m_out << std::endl;
    }
    m_out << std::endl;
    m_out.flush();
  }

  // Human-readable description of a set of universe indices, e.g. "{X: b d}".
  static std::string describe(const std::vector<abstractable_parameter>& universe, const std::vector<std::size_t>& set)
  {
    std::map<core::identifier_string, std::vector<std::string>> names_per_equation;
    for (const std::size_t i: set)
    {
      names_per_equation[universe[i].equation].push_back(std::string(universe[i].variable.name()));
    }
    std::ostringstream stream;
    stream << "{";
    bool first = true;
    for (const auto& [equation, names]: names_per_equation)
    {
      stream << (first ? "" : ", ") << equation << ": ";
      for (const std::string& name: names)
      {
        stream << name << " ";
      }
      first = false;
    }
    stream << "}";
    return stream.str();
  }
};

/// \brief The engine that enumerates all valid abstraction sets of a PBES.
class pbesfindabs_engine
{
public:
  /// \brief Enumerates all valid abstraction sets of the (normalized) PBES p
  ///        and writes them to the output file from the options.
  /// \return The number of valid abstraction sets found.
  std::size_t run(const pbes& p, const pbesfindabs_options& options)
  {
    if (options.number_of_threads == 0)
    {
      throw mcrl2::runtime_error("The number of threads should be at least 1.");
    }

    // One master iterator provides the read-only analyses (initial abstraction
    // set, infinite quantifier guards) and solves the original PBES.
    pbescegps_iterator master;
    master.initialize(p, options.cepgps);

    std::vector<abstractable_parameter> universe = build_universe(p, options, master);
    mCRL2log(log::verbose) << "Universe of abstractable parameters (" << universe.size() << "):" << std::endl;
    for (const abstractable_parameter& param: universe)
    {
      mCRL2log(log::verbose) << "  " << param.equation << " : " << param.variable << std::endl;
    }

    // Determine the direction of the approximations from the answer of the
    // original PBES: if the answer is true, only under-approximations can
    // prove it; dually, if the answer is false, only over-approximations can
    // refute it.
    utilities::execution_timer timer;
    timer.start("solving the original PBES");
    bool original_answer = master.solve(p, options.cepgps).first;
    timer.finish("solving the original PBES");
    const bool is_overapproximation = !original_answer;
    mCRL2log(log::info) << "The original PBES solves to " << (original_answer ? "true" : "false") << "; "
                        << (is_overapproximation ? "only over-approximations will be checked."
                                                 : "only under-approximations will be checked.")
                        << std::endl;
    if (mcrl2::log::mCRL2logEnabled(log::verbose))
    {
      timer.report();
    }

    abstraction_set_writer writer(options.output_file);

    // A family is downward closed by construction: a set is only a candidate
    // when none of its subsets is blocked. "blocked" (data closed yet not
    // proving) is the only failure that is upward closed: extending a blocked
    // set keeps it blocked, and extending a not-data-closed set may yield a
    // data-closed set, so not-data-closed sets are never used to prune.
    //
    // Level 0: the empty set is always valid (it equals the original PBES).
    std::vector<std::vector<std::size_t>> frontier;
    frontier.emplace_back();
    writer.write(universe, frontier.front());
    ++m_valid_count;

    // Levels 1..N: breadth-first enumeration of the downward closed family of
    // non-blocked sets. Level k is generated by joining the non-blocked sets of
    // level k-1, so the search stays within the ideal determined by the minimal
    // blocked sets and never has to consider a full powerset.
    for (std::size_t level = 1; level <= universe.size(); ++level)
    {
      std::vector<std::vector<std::size_t>> candidates = generate_candidates(frontier, universe.size());
      if (candidates.empty())
      {
        break;
      }
      mCRL2log(log::info) << "Level " << level << ": " << candidates.size() << " candidate abstraction set"
                          << (candidates.size() == 1 ? "" : "s") << " to check." << std::endl;

      // Each checked set that is not blocked (i.e. valid or not-data-closed)
      // is kept in the next frontier so that its supersets are still explored.
      frontier.clear();
      check_candidates(p, options, universe, is_overapproximation, candidates, writer, frontier);
      if (frontier.empty())
      {
        break;
      }
    }

    mCRL2log(log::info) << "Found " << m_valid_count << " valid abstraction set" << (m_valid_count == 1 ? "" : "s")
                        << " (written to " << options.output_file << ")." << std::endl;
    return m_valid_count;
  }

private:
  // The outcome of checking a single candidate set.
  enum class outcome
  {
    valid, // data closed and the approximation proves the answer of the original PBES
    not_closed, // not data closed; supersets may still be data closed, so they are kept
    blocked // data closed but the approximation does not prove the answer
  };

  // Collects the parameters that may occur in abstraction sets: all equation
  // parameters, minus control flow parameters (when --init-cfp is given) and
  // minus parameters occurring in guards under an infinite quantifier (when
  // requested), since abstracting the latter may yield infinitely many states.
  static std::vector<abstractable_parameter>
  build_universe(const pbes& p, const pbesfindabs_options& options, pbescegps_iterator& master)
  {
    abstract_param_state initial;
    master.compute_initial_abstraction_set(p, options.cepgps.init_control_flow, initial);
    if (options.cepgps.instantiate_infinite_quantifier_guards)
    {
      master.instantiate_infinite_quantifier_guards(p, initial);
    }

    std::vector<abstractable_parameter> universe;
    for (const pbes_equation& eq: p.equations())
    {
      const core::identifier_string& eq_name = eq.variable().name();
      std::size_t index = 0;
      for (const data::variable& param: eq.variable().parameters())
      {
        if (initial.W[eq_name].contains(param))
        {
          universe.push_back({eq_name, param, index});
        }
        ++index;
      }
    }
    return universe;
  }

  // Generates the size-(k+1) candidates from the non-blocked size-k sets. A
  // candidate is a superset of a non-blocked set that is obtained by appending
  // a larger element, and that keeps the property that all of its size-k
  // subsets are non-blocked. Because the blocked family is upward closed, this
  // is equivalent to considering exactly the sets that contain no blocked
  // subset, so no valid set is ever skipped.
  static std::vector<std::vector<std::size_t>>
  generate_candidates(const std::vector<std::vector<std::size_t>>& frontier, std::size_t universe_size)
  {
    const std::set<std::vector<std::size_t>> frontier_set(frontier.begin(), frontier.end());
    std::set<std::vector<std::size_t>> candidates;
    for (const std::vector<std::size_t>& s: frontier)
    {
      for (std::size_t i = s.empty() ? 0 : s.back() + 1; i < universe_size; ++i)
      {
        std::vector<std::size_t> c = s;
        c.push_back(i);
        bool all_subsets_non_blocked = true;
        for (std::size_t j = 0; j + 1 < c.size(); ++j)
        {
          std::vector<std::size_t> subset;
          subset.reserve(c.size() - 1);
          for (std::size_t k = 0; k < c.size(); ++k)
          {
            if (k != j)
            {
              subset.push_back(c[k]);
            }
          }
          if (!frontier_set.contains(subset))
          {
            all_subsets_non_blocked = false;
            break;
          }
        }
        if (all_subsets_non_blocked)
        {
          candidates.insert(std::move(c));
        }
      }
    }
    return std::vector<std::vector<std::size_t>>(candidates.begin(), candidates.end());
  }

  // Solves all candidates of one level using a pool of worker threads; each
  // worker takes the next unclaimed candidate. Valid sets are written to the
  // output file from the worker that found them, as soon as they are found.
  // Sets that are not blocked (valid or not-data-closed) are collected in
  // next_frontier, so that supersets of not-data-closed sets are still checked.
  void check_candidates(const pbes& p,
    const pbesfindabs_options& options,
    const std::vector<abstractable_parameter>& universe,
    bool is_overapproximation,
    const std::vector<std::vector<std::size_t>>& candidates,
    abstraction_set_writer& writer,
    std::vector<std::vector<std::size_t>>& next_frontier)
  {
    std::atomic<std::size_t> next_candidate{0};
    std::mutex frontier_mutex;

    const std::size_t num_workers
      = std::min<std::size_t>(options.number_of_threads, candidates.empty() ? 1 : candidates.size());

    auto worker_procedure = [&]()
    {
      // Every worker uses its own iterator with its own data rewriter and its
      // own cache of approximation results.
      pbescegps_iterator solver;
      solver.initialize(p, options.cepgps);

      for (;;)
      {
        const std::size_t i = next_candidate.fetch_add(1);
        if (i >= candidates.size())
        {
          return;
        }
        const std::vector<std::size_t>& set = candidates[i];

        outcome result;
        {
          // The log level is overridden for this thread only: all solving and
          // abstraction output that check_set produces (which goes no higher
          // than verbose) is suppressed, so the workers do not interleave
          // solver chatter; this thread's own findings remain visible.
          const mcrl2::log::scoped_reporting_level solver_logging(mcrl2::log::info);
          result = check_set(solver, p, options, universe, set, is_overapproximation);
        }

        switch (result)
        {
        case outcome::valid:
        {
          mCRL2log(log::info) << "Found valid abstraction set " << writer.describe(universe, set) << std::endl;
          writer.write(universe, set);
          std::lock_guard<std::mutex> guard(frontier_mutex);
          next_frontier.push_back(set);
          ++m_valid_count;
          break;
        }
        case outcome::not_closed:
        {
          // Not data closed: reported as not valid, but not used for pruning,
          // since a superset may well be data closed.
          mCRL2log(log::verbose) << "Abstraction set " << writer.describe(universe, set)
                                 << " is not valid: it is not data closed." << std::endl;
          std::lock_guard<std::mutex> guard(frontier_mutex);
          next_frontier.push_back(set);
          break;
        }
        case outcome::blocked:
        {
          // The approximation is solvable but does not prove the answer; all
          // supersets behave the same, so this set prunes its own up-set.
          mCRL2log(log::verbose) << "Abstraction set " << writer.describe(universe, set)
                                 << " is not valid: the approximation does not prove the answer of the original PBES."
                                 << std::endl;
          break;
        }
        }
      }
    };

    std::vector<std::thread> threads;
    threads.reserve(num_workers);
    try
    {
      for (std::size_t t = 0; t < num_workers; ++t)
      {
        threads.emplace_back(worker_procedure);
      }
    }
    catch (const std::system_error& e)
    {
      mCRL2log(log::warning) << "Could not start all " << num_workers << " worker threads (" << e.what()
                             << "); continuing with the running ones." << std::endl;
    }
    for (std::thread& t: threads)
    {
      if (t.joinable())
      {
        t.join();
      }
    }
  }

  // Checks a single candidate set: first whether it is data-closed (if not,
  // solving is useless and the set is reported as not valid, although its
  // supersets are still worth checking), then by solving the appropriate
  // approximation and comparing it with the answer of the original PBES.
  static outcome check_set(pbescegps_iterator& solver,
    const pbes& p,
    const pbesfindabs_options& options,
    const std::vector<abstractable_parameter>& universe,
    const std::vector<std::size_t>& set,
    bool is_overapproximation)
  {
    abstract_param_state state;
    for (const pbes_equation& eq: p.equations())
    {
      state.I[eq.variable().name()] = std::set<std::size_t>();
      state.W[eq.variable().name()] = std::set<data::variable>();
    }
    for (const std::size_t i: set)
    {
      state.add_abstracted_variable(p, universe[i].equation, universe[i].variable);
    }

    std::map<core::identifier_string, std::set<data::variable>> original_w = state.W;
    solver.make_data_closed(p, state);
    if (state.W != original_w)
    {
      return outcome::not_closed;
    }

    structure_graph graph;
    bool result = solver.solve_approximation_cached(p, state, is_overapproximation, options.cepgps, graph);
    return (is_overapproximation ? !result : result) ? outcome::valid : outcome::blocked;
  }

  // Number of valid sets found so far. Incremented from the worker threads, so
  // it is atomic; it is only read by the main thread between levels.
  std::atomic<std::size_t> m_valid_count{0};
};

/// \brief Enumerates all valid abstraction sets of the PBES in the input file.
/// \param input_filename The file containing the PBES.
/// \param input_format The format of the input file.
/// \param options The options of the tool.
/// \return The number of valid abstraction sets found.
inline std::size_t pbesfindabs(const std::string& input_filename,
  const utilities::file_format& input_format,
  const pbesfindabs_options& options)
{
  pbes p;
  load_pbes(p, input_filename, input_format);
  algorithms::normalize(p);

  pbesfindabs_engine engine;
  return engine.run(p, options);
}

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_TOOLS_PBESFINDABS_H
