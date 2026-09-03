// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesfindabs.cpp

#include "mcrl2/pbes/tools/pbesfindabs.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/pbes/pbes_rewriter_tool.h"
#include "mcrl2/utilities/basename.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/parallel_tool.h"
#include "mcrl2/utilities/platform.h"
#include "mcrl2/utilities/process_pool.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#if defined(MCRL2_PLATFORM_WINDOWS)
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using data::tools::rewriter_tool;
using pbes_system::tools::pbes_input_tool;
using pbes_system::tools::pbes_rewriter_tool;

class pbesfindabs_tool : public parallel_tool<pbes_input_tool<pbes_rewriter_tool<rewriter_tool<input_tool>>>>
{
protected:
  using super = parallel_tool<pbes_input_tool<pbes_rewriter_tool<rewriter_tool<input_tool>>>>;

  pbesfindabs_options m_options;
  bool m_worker_mode = false;
  bool m_check_over = false;

  // The command line this tool was invoked with (argv[0] resolved to an
  // absolute path). Used to spawn the worker processes.
  std::vector<std::string> m_invocation;

  // The pool of worker processes, created lazily on the first checked level.
  std::unique_ptr<utilities::process_pool> m_pool;

  // Set when standard input had to be materialized to a file so that the
  // workers can load the PBES; the file is removed again on destruction.
  std::string m_temp_input;

  void parse_options(const command_line_parser& parser) override
  {
    super::parse_options(parser);
    m_options.cepgps.init_control_flow = parser.has_option("init-cfp");
    m_options.cepgps.instantiate_infinite_quantifier_guards
      = parser.has_option("instantiate-infinite-quantifier-guards");
    m_options.cepgps.solve_symbolic = parser.has_option("solve-symbolic-args");
    m_options.cepgps.stategraph = parser.has_option("stategraph");
    m_options.cepgps.solve_symbolic_args = parser.option_argument_as<std::string>("solve-symbolic-args");
    m_options.cepgps.optimization = parser.option_argument_as<partial_solve_strategy>("optimization");
    m_options.number_of_threads = number_of_threads();
    m_options.timeout = parser.option_argument_as<double>("timeout");

    m_worker_mode = parser.has_option("worker");
    if (m_worker_mode)
    {
      m_check_over = parser.option_argument("check-direction") == "over";
      return;
    }

    m_options.output_file = parser.option_argument("abstraction-file");
    if (m_options.output_file.empty())
    {
      throw mcrl2::runtime_error("The option -a/--abstraction-file with an output file name is required.");
    }
  }

  void add_options(interface_description& desc) override
  {
    super::add_options(desc);
    desc.add_option("abstraction-file",
      utilities::make_mandatory_argument("FILE"),
      "Write every valid abstraction set to FILE as soon as it is found.",
      'a');
    desc.add_option("init-cfp", "Only consider the non-(global) control flow parameters for abstraction.", 'c');
    desc.add_option("instantiate-infinite-quantifier-guards",
      "Do not consider parameters that occur in the guards of predicate variable instances in the scope of an "
      "infinite quantifier for abstraction, to prevent infinitely many vertices in the structure graph.",
      'g');
    desc.add_option("stategraph", "Use stategraph for each approximation.");
    desc.add_option("optimization",
      utilities::make_enum_argument<partial_solve_strategy>("STRATEGY")
        .add_value_desc(partial_solve_strategy::no_optimisation, "Do not apply any optimizations.", true)
        .add_value_desc(partial_solve_strategy::remove_self_loops, "Remove self loops.")
        .add_value_desc(partial_solve_strategy::propagate_solved_equations_using_substitution,
          "Propagate solved equations using substitution.")
        .add_value_desc(partial_solve_strategy::propagate_solved_equations_using_attractor,
          "Propagate solved equations using an attractor.")
        .add_value_desc(partial_solve_strategy::detect_winning_loops_using_fatal_attractor,
          "Detect winning loops using a fatal attractor.")
        .add_value_desc(partial_solve_strategy::solve_subgames_using_fatal_attractor_local,
          "Solve subgames using a fatal attractor (local version).")
        .add_value_desc(partial_solve_strategy::solve_subgames_using_fatal_attractor_original,
          "Solve subgames using a fatal attractor (original version).")
        .add_value_desc(partial_solve_strategy::solve_subgames_using_solver, "Solve subgames using the solver.")
        .add_value_desc(partial_solve_strategy::detect_winning_loops_original,
          "Detect winning loops (original version)."),
      "Use optimization STRATEGY while solving approximations.",
      's');
    desc.add_option("solve-symbolic-args",
      utilities::make_optional_argument("STR", ""),
      "Solve the PBES and the approximations symbolically using the following arguments.");
    desc.add_option("timeout",
      utilities::make_optional_argument("SECONDS", "0.0"),
      "Skip an abstraction set if checking it takes longer than SECONDS seconds. "
      "Each set is then checked in a child process that is terminated when it is "
      "too slow, so a slow set cannot block the run. 0 means no timeout.",
      't');
    desc.add_hidden_option("worker",
      "Internal: run as a worker process that checks abstraction sets received on standard input.");
    desc.add_hidden_option("check-direction",
      utilities::make_mandatory_argument("DIRECTION"),
      "Internal: the direction of the approximation to check in --worker mode.");
  }

public:
  pbesfindabs_tool()
    : super("pbesfindabs",
        "Jore Booy",
        "Find all valid abstraction sets of a PBES.",
        "Reads a file containing a PBES and enumerates all valid abstraction "
        "sets, writing each of them to the file given by -a. If INFILE is not "
        "present, standard input is used.")
  {}

  ~pbesfindabs_tool() override
  {
    if (!m_temp_input.empty())
    {
      std::remove(m_temp_input.c_str());
    }
  }

  bool pre_run(int& argc, char** argv) override
  {
    m_invocation.assign(argv, argv + argc);
    if (!m_invocation.empty())
    {
      const std::string path = utilities::get_executable_path();
      if (!path.empty())
      {
        m_invocation[0] = path;
      }
    }
    return super::pre_run(argc, argv);
  }

  bool run() override
  {
    m_options.cepgps.rewrite_strategy = rewrite_strategy();

    if (m_worker_mode)
    {
      // Worker process: check sets received on standard input, one per line.
      mcrl2::log::logger::set_reporting_level(mcrl2::log::quiet);
      pbesfindabs_worker(input_filename(), pbes_input_format(), m_options, m_check_over);
      return true;
    }

    mCRL2log(verbose) << "pbesfindabs parameters:" << std::endl;
    mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
    mCRL2log(verbose) << "  abstraction file:   " << m_options.output_file << std::endl;
    mCRL2log(verbose) << "  number of workers:  " << m_options.number_of_threads << std::endl;
    mCRL2log(verbose) << "  timeout per set:    " << m_options.timeout << " seconds" << std::endl;

    // The workers load the PBES from a file, so standard input has to be
    // materialized to a temporary file first.
    if (input_filename().empty())
    {
      input_filename() = materialize_stdin();
    }

    m_options.checker
      = [this](const std::vector<abstractable_parameter>& universe,
          const std::vector<std::vector<std::size_t>>& batch,
          bool is_overapproximation) { return check_batch(universe, batch, is_overapproximation); };
    pbesfindabs(input_filename(), pbes_input_format(), m_options);

    return true;
  }

private:
  // Writes standard input to a temporary file and returns its path.
  std::string materialize_stdin()
  {
    std::string pid;
#if defined(MCRL2_PLATFORM_WINDOWS)
    pid = std::to_string(GetCurrentProcessId());
#else
    pid = std::to_string(getpid());
#endif
    const std::string path
      = (std::filesystem::temp_directory_path() / ("pbesfindabs_stdin_" + pid + ".pbes")).string();
    std::ofstream out(path, std::ios::binary);
    if (!out)
    {
      throw mcrl2::runtime_error("Could not create a temporary file for standard input.");
    }
    out << std::cin.rdbuf();
    out.close();
    m_temp_input = path;
    return path;
  }

  // The command line of a worker that checks in the given direction: this
  // tool's own command line plus the hidden worker options.
  std::vector<std::string> worker_command(bool is_overapproximation) const
  {
    std::vector<std::string> command = m_invocation;
    // When the input came from standard input it is not part of m_invocation,
    // so the temporary file must be added as a positional argument.
    if (!m_temp_input.empty())
    {
      command.push_back(input_filename());
    }
    command.push_back("--worker");
    command.push_back("--check-direction=" + std::string(is_overapproximation ? "over" : "under"));
    return command;
  }

  // Checks a batch of candidate sets in the pool of worker processes, in
  // parallel, and maps their responses back to outcomes.
  std::vector<check_outcome> check_batch(const std::vector<abstractable_parameter>& universe,
    const std::vector<std::vector<std::size_t>>& batch,
    bool is_overapproximation)
  {
    if (!m_pool)
    {
      m_pool = std::make_unique<utilities::process_pool>(worker_command(is_overapproximation),
        m_options.number_of_threads,
        m_options.timeout);
    }

    std::vector<std::string> requests;
    requests.reserve(batch.size());
    for (const std::vector<std::size_t>& set: batch)
    {
      requests.push_back(encode_abstracted_set(universe, set));
    }

    std::vector<std::string> responses = m_pool->run(requests);

    std::vector<check_outcome> outcomes;
    outcomes.reserve(responses.size());
    for (const std::string& response: responses)
    {
      if (response.empty())
      {
        outcomes.push_back(skipped_t{});
        continue;
      }
      if (const auto verdict = from_token(response))
      {
        outcomes.push_back(*verdict);
      }
      else
      {
        mCRL2log(warning) << "A worker failed to check an abstraction set ('" << response << "'); it is skipped."
                          << std::endl;
        outcomes.push_back(skipped_t{});
      }
    }
    return outcomes;
  }
};

int main(int argc, char* argv[])
{
  return pbesfindabs_tool().execute(argc, argv);
}
