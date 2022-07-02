// Author(s): Wieger Wesselink; Threads are added by Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbessolve_parallel.cpp
/// \brief This tool transforms an .lps file into a labelled transition system.
///        Optionally, it can be run with multiple treads.

#ifndef MCRL2_PBES_TOOLS_PBESSOLVE_H
#define MCRL2_PBES_TOOLS_PBESSOLVE_H

#include "mcrl2/bes/pbes_input_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/lps/detail/lps_io.h"
#include "mcrl2/pbes/detail/pbes_io.h"
#include "mcrl2/pbes/pbesinst_structure_graph2.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/parallel_tool.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities::tools;
using mcrl2::bes::tools::pbes_input_tool;
using mcrl2::data::tools::rewriter_tool;
using mcrl2::utilities::tools::input_tool;
using utilities::tools::parallel_tool;

// TODO: put this code in the utilities library?
inline std::string file_extension(const std::string& filename)
{
  auto pos = filename.find_last_of('.');
  if (pos == std::string::npos)
  {
    return "";
  }
  return filename.substr(pos + 1);
}

class pbessolve_tool
    : public parallel_tool<rewriter_tool<pbes_input_tool<input_tool>>>
{
  protected:
  typedef parallel_tool<rewriter_tool<pbes_input_tool<input_tool>>> super;

  pbessolve_options options;
  std::string lpsfile;
  std::string ltsfile;
  std::string evidence_file;

  void add_options(utilities::interface_description& desc) override
  {
    super::add_options(desc);
    desc.add_hidden_option("check-strategy",
                           "do a sanity check on the computed strategy", 'c');
    desc.add_option("search-strategy",
                    utilities::make_enum_argument<search_strategy>("NAME")
                        .add_value_desc(breadth_first,
                                        "Leads to smaller counter examples",
                                        true)
                        .add_value_desc(depth_first, ""),
                    "Use search strategy NAME:", 'z');
    desc.add_option("file", utilities::make_file_argument("NAME"),
                    "The file containing the LPS or LTS that was used to "
                    "generate the PBES using lps2pbes -c. If this "
                    "option is set, a counter example or witness for the "
                    "encoded property will be generated. The "
                    "extension of the file should be .lps in case of an LPS "
                    "file, in all other cases it is assumed to "
                    "be an LTS.",
                    'f');
    desc.add_option("prune-todo-list", "Prune the todo list periodically.");
    desc.add_hidden_option("no-remove-unused-rewrite-rules",
                           "do not remove unused rewrite rules. ", 'u');
    desc.add_option("evidence-file", utilities::make_file_argument("NAME"),
                    "The file to which the evidence is written. If not set, a "
                    "default name will be chosen.");
    desc.add_option(
        "solve-strategy",
        utilities::make_enum_argument<int>("NAME")
            .add_value_desc(0, "No on-the-fly solving is applied", true)
            .add_value_desc(1, "Propagate solved equations using an attractor.")
            .add_value_desc(2, "Detect winning loops.")
            .add_value_desc(3, "Solve subgames using a fatal attractor.")
            .add_value_desc(4, "Solve subgames using the solver."),
        "Use solve strategy NAME. Strategies 1-4 periodically apply on-the-fly "
        "solving, which may lead to early termination.",
        's');
    desc.add_hidden_option(
        "long-strategy",
        utilities::make_enum_argument<int>("STRATEGY")
            .add_value_desc(0, "Do not apply any optimizations.")
            .add_value_desc(1, "Remove self loops.")
            .add_value_desc(2, "Propagate solved equations using substitution.")
            .add_value_desc(3, "Propagate solved equations using an attractor.")
            .add_value_desc(4, "Detect winning loops using a fatal attractor.")
            .add_value_desc(
                5, "Solve subgames using a fatal attractor (local version).")
            .add_value_desc(
                6, "Solve subgames using a fatal attractor (original version).")
            .add_value_desc(7, "Solve subgames using the solver.")
            .add_value_desc(8, "Detect winning loops (original version)."
                               " N.B. This optimization does not work "
                               "correctly in combination with counter examples."
                               " It may also cause stack overflow."),
        "use strategy STRATEGY (N.B. This is a developer option that overrides "
        "--strategy)",
        'l');
    desc.add_hidden_option(
        "no-replace-constants-by-variables",
        "Do not move constant expressions to a substitution.");
    desc.add_hidden_option("aggressive",
                           "Apply optimizations 4 and 5 at every iteration.");
    desc.add_hidden_option("prune-todo-alternative",
                           "Use a variation of todo list pruning.");
  }

  void parse_options(const utilities::command_line_parser& parser) override
  {
    super::parse_options(parser);

    options.check_strategy = parser.has_option("check-strategy");
    options.replace_constants_by_variables =
        !parser.has_option("no-replace-constants-by-variables");
    options.remove_unused_rewrite_rules =
        !parser.has_option("no-remove-unused-rewrite-rules");
    options.aggressive = parser.has_option("aggressive");
    options.prune_todo_list = parser.has_option("prune-todo-list");
    options.prune_todo_alternative =
        parser.has_option("prune-todo-alternative");
    options.exploration_strategy =
        parser.option_argument_as<mcrl2::pbes_system::search_strategy>(
            "search-strategy");
    options.rewrite_strategy = rewrite_strategy();
    options.number_of_threads = number_of_threads();
    

    if (parser.has_option("file"))
    {
      std::string filename = parser.option_argument("file");
      if (file_extension(filename) == "lps")
      {
        lpsfile = filename;
      }
      else
      {
        ltsfile = filename;
      }
    }

    if (parser.has_option("evidence-file"))
    {
      if (!parser.has_option("file"))
      {
        throw mcrl2::runtime_error(
            "Option --evidence-file cannot be used without option --file");
      }
      evidence_file = parser.option_argument("evidence-file");
    }

    if (parser.has_option("long-strategy"))
    {
      options.optimization = parser.option_argument_as<int>("long-strategy");
    }
    else
    {
      options.optimization = parser.option_argument_as<int>("solve-strategy");
      if (options.optimization == 0)
      {
        options.optimization = 2;
      }
      else if (options.optimization == 1)
      {
        options.optimization = 3;
      }
      else if (options.optimization == 2)
      {
        options.optimization = 4;
      }
      else if (options.optimization == 3)
      {
        options.optimization = 6;
      }
      else if (options.optimization == 4)
      {
        options.optimization = 7;
      }
    }

    if (options.optimization < 0 || options.optimization > 8)
    {
      throw mcrl2::runtime_error("Invalid strategy " +
                                 std::to_string(options.optimization));
    }
    if (options.prune_todo_list && options.optimization < 2)
    {
      mCRL2log(log::warning) << "Option --prune-todo-list has no effect for "
                                "strategies less than 2."
                             << std::endl;
    }
  }

  std::set<utilities::file_format> available_input_formats() const override
  {
    return {pbes_system::pbes_format_internal()};
  }

  public:
  pbessolve_tool(const std::string& toolname)
      : super(toolname, "Wieger Wesselink",
              "Generate a BES from a PBES and solve it. ",
              "Solves (P)BES from INFILE. "
              "If INFILE is not present, stdin is used. "
              "The PBES is first instantiated into a parity game, "
              "which is then solved using Zielonka's algorithm. "
              "It supports the generation of a witness or counter "
              "example for the property encoded by the PBES.")
  {
  }

  template <typename PbesInstAlgorithm>
  void run_algorithm(PbesInstAlgorithm& algorithm, pbes_system::pbes& pbesspec,
                     structure_graph& G,
                     const data::mutable_map_substitution<>& sigma)
  {
    mCRL2log(log::verbose) << "Generating parity game..." << std::endl;
    timer().start("instantiation");
    algorithm.run();
    timer().finish("instantiation");

    mCRL2log(log::verbose) << "Number of vertices in the structure graph: "
                           << G.all_vertices().size() << std::endl;

    if ((!lpsfile.empty() || !ltsfile.empty()) &&
        !has_counter_example_information(pbesspec))
    {
      mCRL2log(log::warning)
          << "Warning: the PBES has no counter example information. Did you "
             "use the"
             " --counter-example option when generating the PBES?"
          << std::endl;
    }

    if (!lpsfile.empty())
    {
      lps::specification lpsspec = lps::detail::load_lps(lpsfile);
      lps::detail::replace_global_variables(lpsspec, sigma);

      bool result;
      lps::specification evidence;
      timer().start("solving");
      std::tie(result, evidence) = solve_structure_graph_with_counter_example(
          G, lpsspec, pbesspec, algorithm.equation_index());
      timer().finish("solving");
      std::cout << (result ? "true" : "false") << std::endl;
      if (evidence_file.empty())
      {
        evidence_file = input_filename() + ".evidence.lps";
      }
      lps::detail::save_lps(evidence, evidence_file);
      mCRL2log(log::verbose)
          << "Saved " << (result ? "witness" : "counter example") << " in "
          << evidence_file << std::endl;
    }
    else if (!ltsfile.empty())
    {
      lts::lts_lts_t ltsspec;
      ltsspec.load(ltsfile);
      lts::lts_lts_t evidence;
      timer().start("solving");
      bool result = solve_structure_graph_with_counter_example(G, ltsspec);
      timer().finish("solving");
      std::cout << (result ? "true" : "false") << std::endl;
      if (evidence_file.empty())
      {
        evidence_file = input_filename() + ".evidence.lts";
      }
      ltsspec.save(evidence_file);
      mCRL2log(log::verbose)
          << "Saved " << (result ? "witness" : "counter example") << " in "
          << evidence_file << std::endl;
    }
    else
    {
      timer().start("solving");
      bool result = solve_structure_graph(G, options.check_strategy);
      timer().finish("solving");
      std::cout << (result ? "true" : "false") << std::endl;
    }
  }

  bool run() override
  {
    pbes_system::pbes pbesspec =
        pbes_system::detail::load_pbes(input_filename());
    pbes_system::algorithms::normalize(pbesspec);
    data::mutable_map_substitution<> sigma;

    if (!lpsfile.empty())
    {
      // Make sure that the global variables of the LPS and the PBES get the
      // same values
      sigma = pbes_system::detail::instantiate_global_variables(pbesspec);
      pbes_system::detail::replace_global_variables(pbesspec, sigma);
    }

    structure_graph G;
    if (options.optimization <= 1)
    {
      pbesinst_structure_graph_algorithm algorithm(options, pbesspec, G);
      run_algorithm<pbesinst_structure_graph_algorithm>(algorithm, pbesspec, G,
                                                        sigma);
    }
    else
    {
      pbesinst_structure_graph_algorithm2 algorithm(options, pbesspec, G);
      run_algorithm<pbesinst_structure_graph_algorithm2>(algorithm, pbesspec, G,
                                                         sigma);
    }
    return true;
  }
};

#endif // MCRL2_PBES_TOOLS_PBESSOLVE_H
