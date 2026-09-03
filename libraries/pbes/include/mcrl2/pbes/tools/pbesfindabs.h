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
///        an interrupted run. The engine itself performs no parallel work: it
///        delegates the checking of each level's candidates to a pluggable
///        checker (the tool provides one backed by a pool of worker processes,
///        so a slow set can be killed instead of blocking the run).

#ifndef MCRL2_PBES_TOOLS_PBESFINDABS_H
#define MCRL2_PBES_TOOLS_PBESFINDABS_H

#include "mcrl2/pbes/tools/pbescegps.h"
#include "mcrl2/utilities/execution_timer.h"
#include "mcrl2/utilities/logger.h"
#include <cstddef>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace mcrl2::pbes_system
{

/// \brief The verdict of checking a single candidate abstraction set.
enum class abstraction_set_verdict
{
  valid, // data closed and the approximation proves the answer of the original PBES
  not_closed, // not data closed; supersets may still be data closed, so they are kept
  blocked // data closed but the approximation does not prove the answer
};

/// \brief Marker for a check whose verdict is unknown, e.g. because its time
///        limit expired. Such a set is neither reported nor used for pruning;
///        its supersets are still explored.
struct skipped_t
{
};

/// \brief The outcome of checking one candidate set: a verdict, or skipped.
using check_outcome = std::variant<abstraction_set_verdict, skipped_t>;

/// \brief One entry of the universe of abstractable parameters: the variable
///        itself and its index inside the parameters of its equation.
struct abstractable_parameter
{
  core::identifier_string equation;
  data::variable variable;
  std::size_t index = 0;
};

/// \brief Options for the all-abstraction-sets tool.
struct pbesfindabs_options
{
  /// \brief Checks a batch of candidate sets (each given by its indices into
  ///        universe) and returns one outcome per set, in the same order.
  ///        Implementations may check the sets concurrently and may abandon a
  ///        set (returning skipped) when its time limit expires.
  using batch_checker = std::function<std::vector<check_outcome>(const std::vector<abstractable_parameter>& universe,
    const std::vector<std::vector<std::size_t>>& sets,
    bool is_overapproximation)>;

  pbescegps_options cepgps; ///< Options used for constructing and solving approximations.
  std::string output_file; ///< The file to which valid abstraction sets are written.
  std::size_t number_of_threads = 1; ///< The number of worker processes used to check sets in parallel.
  double timeout = 0.0; ///< Time limit in seconds for checking one abstraction set; 0 means no limit.

  /// \brief Checks a batch of candidate sets. Required: the engine performs no
  ///        checking itself. The tool installs one that runs the checks in a
  ///        pool of worker processes.
  batch_checker checker;
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

// Serializes a set of universe indices to "eq:param,eq:param". The worker
// process decodes this, so the parent and worker need not agree on universe
// indices, only on parameter names.
inline std::string encode_abstracted_set(const std::vector<abstractable_parameter>& universe,
  const std::vector<std::size_t>& set)
{
  std::ostringstream stream;
  bool first = true;
  for (const std::size_t i: set)
  {
    stream << (first ? "" : ",") << universe[i].equation << ":" << universe[i].variable.name();
    first = false;
  }
  return stream.str();
}

// Decodes "eq:param,eq:param" into the universe indices of the named
// parameters, matching them by name against universe.
inline std::vector<std::size_t> decode_abstracted_set(const std::string& text,
  const std::vector<abstractable_parameter>& universe)
{
  std::map<std::pair<core::identifier_string, core::identifier_string>, std::size_t> index_of;
  for (std::size_t i = 0; i < universe.size(); ++i)
  {
    index_of[{universe[i].equation, universe[i].variable.name()}] = i;
  }

  std::vector<std::size_t> set;
  std::stringstream stream(text);
  std::string token;
  while (std::getline(stream, token, ','))
  {
    const std::size_t colon = token.find(':');
    if (colon == std::string::npos || colon == 0 || colon + 1 == token.size())
    {
      throw mcrl2::runtime_error("Malformed abstraction set entry '" + token + "'.");
    }
    const core::identifier_string equation(token.substr(0, colon));
    const core::identifier_string parameter(token.substr(colon + 1));
    const auto it = index_of.find({equation, parameter});
    if (it == index_of.end())
    {
      throw mcrl2::runtime_error("Unknown parameter '" + token + "' in abstraction set.");
    }
    set.push_back(it->second);
  }
  return set;
}

inline std::string to_token(abstraction_set_verdict verdict)
{
  switch (verdict)
  {
  case abstraction_set_verdict::valid:
    return "valid";
  case abstraction_set_verdict::not_closed:
    return "not_closed";
  case abstraction_set_verdict::blocked:
    return "blocked";
  }
  return "error";
}

inline std::optional<abstraction_set_verdict> from_token(const std::string& token)
{
  if (token == "valid")
  {
    return abstraction_set_verdict::valid;
  }
  if (token == "not_closed")
  {
    return abstraction_set_verdict::not_closed;
  }
  if (token == "blocked")
  {
    return abstraction_set_verdict::blocked;
  }
  return std::nullopt;
}

/// \brief The engine that enumerates all valid abstraction sets of a PBES.
class pbesfindabs_engine
{
public:
  /// \brief Enumerates all valid abstraction sets of the (normalized) PBES p
  ///        and writes them to the output file from the options.
  /// \return The number of valid abstraction sets found.
  std::size_t run(const pbes& p, const pbesfindabs_options& options)
  {
    m_valid_count = 0;
    if (options.number_of_threads == 0)
    {
      throw mcrl2::runtime_error("The number of workers should be at least 1.");
    }
    if (!options.checker)
    {
      throw mcrl2::runtime_error("No checker installed: cannot check abstraction sets.");
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

      std::vector<check_outcome> outcomes = options.checker(universe, candidates, is_overapproximation);

      const std::size_t closed_count = static_cast<std::size_t>(std::count_if(outcomes.begin(),
        outcomes.end(),
        [](const check_outcome& outcome) { return counts_as_data_closed(outcome); }));
      mCRL2log(log::info) << "Level " << level << ": " << candidates.size() << " candidate abstraction set"
                          << (candidates.size() == 1 ? "" : "s") << " to check (" << closed_count << " data-closed)."
                          << std::endl;

      // Each checked set that is not blocked (i.e. valid or not-data-closed)
      // is kept in the next frontier so that its supersets are still explored.
      frontier.clear();
      for (std::size_t i = 0; i < candidates.size(); ++i)
      {
        handle_outcome(outcomes[i], universe, candidates[i], options, writer, frontier);
      }
      if (frontier.empty())
      {
        break;
      }
    }

    mCRL2log(log::info) << "Found " << m_valid_count << " valid abstraction set" << (m_valid_count == 1 ? "" : "s")
                        << " (written to " << options.output_file << ")." << std::endl;
    return m_valid_count;
  }

public:
  // Checks the abstraction set given by the indices in set (into the universe
  // of p) and returns its verdict. Used by in-process callers such as the
  // tests; p must already be normalized and options must contain the same
  // settings as the parent run.
  static abstraction_set_verdict check_one_set(const pbes& p,
    const pbesfindabs_options& options,
    const std::vector<std::size_t>& set,
    bool is_overapproximation)
  {
    pbescegps_iterator master;
    master.initialize(p, options.cepgps);
    std::vector<abstractable_parameter> universe = build_universe(p, options, master);
    pbescegps_iterator solver;
    solver.initialize(master.data_rewriter().clone());
    solver.data_rewriter().thread_initialise();
    return check_set(solver, p, options, universe, set, is_overapproximation);
  }

  // Runs the worker loop: reads one set (as "eq:param,eq:param" names) per line
  // from standard input and writes its verdict token to standard output.
  static void run_worker(const pbes& p, const pbesfindabs_options& options, bool is_overapproximation)
  {
    pbescegps_iterator master;
    master.initialize(p, options.cepgps);
    std::vector<abstractable_parameter> universe = build_universe(p, options, master);
    pbescegps_iterator solver;
    solver.initialize(master.data_rewriter().clone());
    solver.data_rewriter().thread_initialise();

    std::string line;
    while (std::getline(std::cin, line))
    {
      if (line.empty())
      {
        continue;
      }
      try
      {
        const std::vector<std::size_t> set = decode_abstracted_set(line, universe);
        std::cout << to_token(check_set(solver, p, options, universe, set, is_overapproximation)) << std::endl;
      }
      catch (const std::exception& e)
      {
        std::cerr << "pbesfindabs worker: " << e.what() << std::endl;
        std::cout << "error" << std::endl;
      }
    }
  }

private:
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
        // c = s + {i}. Dropping the last element of c yields s, which is
        // already non-blocked, so only the subsets obtained by replacing one
        // element of s with i still have to be checked.
        bool all_subsets_non_blocked = true;
        std::vector<std::size_t> subset;
        subset.reserve(s.size());
        for (std::size_t j = 0; j < s.size(); ++j)
        {
          subset.clear();
          subset.insert(subset.end(), s.begin(), s.begin() + j);
          subset.insert(subset.end(), s.begin() + j + 1, s.end());
          subset.push_back(i);
          if (!frontier_set.contains(subset))
          {
            all_subsets_non_blocked = false;
            break;
          }
        }
        if (all_subsets_non_blocked)
        {
          std::vector<std::size_t> c = s;
          c.push_back(i);
          candidates.insert(std::move(c));
        }
      }
    }
    return std::vector<std::vector<std::size_t>>(candidates.begin(), candidates.end());
  }

  // Processes the outcome of one candidate set, updating the writer, the
  // counter of valid sets and the next frontier accordingly.
  void handle_outcome(const check_outcome& outcome,
    const std::vector<abstractable_parameter>& universe,
    const std::vector<std::size_t>& set,
    const pbesfindabs_options& options,
    abstraction_set_writer& writer,
    std::vector<std::vector<std::size_t>>& next_frontier)
  {
    if (std::holds_alternative<skipped_t>(outcome))
    {
      // Verdict unknown: not reported, but also not used for pruning, so its
      // supersets are still explored.
      mCRL2log(log::warning) << "Abstraction set " << writer.describe(universe, set)
                             << " is skipped: checking it exceeded the time limit of " << options.timeout << "s."
                             << std::endl;
      next_frontier.push_back(set);
      return;
    }

    switch (std::get<abstraction_set_verdict>(outcome))
    {
    case abstraction_set_verdict::valid:
      mCRL2log(log::info) << "Found valid abstraction set " << writer.describe(universe, set) << "." << std::endl;
      writer.write(universe, set);
      next_frontier.push_back(set);
      ++m_valid_count;
      break;
    case abstraction_set_verdict::not_closed:
      // Not data closed: reported as not valid, but not used for pruning,
      // since a superset may well be data closed.
      mCRL2log(log::debug) << "Abstraction set " << writer.describe(universe, set)
                           << " is not valid: it is not data closed." << std::endl;
      next_frontier.push_back(set);
      break;
    case abstraction_set_verdict::blocked:
      // The approximation is solvable but does not prove the answer; all
      // supersets behave the same, so this set prunes its own up-set.
      mCRL2log(log::verbose) << "Abstraction set " << writer.describe(universe, set)
                             << " is not valid: the approximation does not prove the answer of the original PBES."
                             << std::endl;
      break;
    }
  }

  // Whether a check outcome counts as "data closed" for the per-level log line:
  // a set is data closed when it was solved (valid or blocked) rather than
  // reported as not data closed or skipped.
  static bool counts_as_data_closed(const check_outcome& outcome)
  {
    const auto* verdict = std::get_if<abstraction_set_verdict>(&outcome);
    return verdict && *verdict != abstraction_set_verdict::not_closed;
  }

  // Returns the data-closure of the abstraction state for a set of universe
  // indices, setting was_closed to whether it was already closed.
  static abstract_param_state closed_state(pbescegps_iterator& solver,
    const pbes& p,
    const std::vector<abstractable_parameter>& universe,
    const std::vector<std::size_t>& set,
    bool& was_closed)
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
    was_closed = (state.W == original_w);
    return state;
  }

  // Checks a single candidate set: first whether it is data-closed (if not,
  // solving is useless and the set is reported as not valid, although its
  // supersets are still worth checking), then by solving the appropriate
  // approximation and comparing it with the answer of the original PBES.
  static abstraction_set_verdict check_set(pbescegps_iterator& solver,
    const pbes& p,
    const pbesfindabs_options& options,
    const std::vector<abstractable_parameter>& universe,
    const std::vector<std::size_t>& set,
    bool is_overapproximation)
  {
    bool was_closed = false;
    abstract_param_state state = closed_state(solver, p, universe, set, was_closed);
    if (!was_closed)
    {
      return abstraction_set_verdict::not_closed;
    }

    structure_graph graph;
    bool result = solver.solve_approximation_cached(p, state, is_overapproximation, options.cepgps, graph);
    return (is_overapproximation ? !result : result) ? abstraction_set_verdict::valid
                                                     : abstraction_set_verdict::blocked;
  }

  // Number of valid sets found so far. Only the (single-threaded) engine
  // touches it.
  std::size_t m_valid_count = 0;
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

/// \brief Runs the worker loop of the tool: reads abstraction sets (by
///        parameter name) from standard input and writes one verdict per line.
///        This is the mode used by the worker processes of the parent run.
inline void pbesfindabs_worker(const std::string& input_filename,
  const utilities::file_format& input_format,
  const pbesfindabs_options& options,
  bool is_overapproximation)
{
  pbes p;
  load_pbes(p, input_filename, input_format);
  algorithms::normalize(p);

  pbesfindabs_engine::run_worker(p, options, is_overapproximation);
}

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_TOOLS_PBESFINDABS_H
