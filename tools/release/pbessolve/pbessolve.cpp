// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbessolve.cpp

#include <iostream>

#include "mcrl2/bes/pbes_input_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/lps/detail/lps_io.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/pbes/detail/pbes_io.h"
#include "mcrl2/pbes/pbesinst_structure_graph.h"
#include "mcrl2/pbes/pbesinst_structure_graph2.h"
#include "mcrl2/pbes/solve_structure_graph.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities::tools;
using mcrl2::bes::tools::pbes_input_tool;
using mcrl2::data::tools::rewriter_tool;
using mcrl2::utilities::tools::input_tool;

// TODO: put this code in the utilities library?
inline
std::string file_extension(const std::string& filename)
{
  auto pos = filename.find_last_of('.');
  if (pos == std::string::npos)
  {
    return "";
  }
  return filename.substr(pos + 1);
}

class pbessolve_tool: public rewriter_tool<pbes_input_tool<input_tool>>
{
  protected:
    typedef rewriter_tool<pbes_input_tool<input_tool>> super;

    search_strategy m_search_strategy;

    // for doing a consistency check on the computed strategy
    bool m_check_strategy = false;

    std::string lpsfile;
    std::string ltsfile;
    std::string evidence_file;

    int m_strategy = 0; // can be 0, 1, 2, 3 or 4

    void add_options(utilities::interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_hidden_option("check-strategy", "do a sanity check on the computed strategy", 'c');
      desc.add_option("search",
                 utilities::make_enum_argument<search_strategy>("SEARCH")
                   .add_value(breadth_first, true)
                   .add_value(depth_first),
                 "use search strategy SEARCH:",
                 'z');
      desc.add_option("file",
                 utilities::make_optional_argument("NAME", "name"),
                 "The file containing the LPS or LTS that was used to generate the PBES using lps2pbes -c. If this "
                 "option is set, a counter example or witness for the encoded property will be generated. The "
                 "extension of the file should be .lps in case of an LPS file, in all other cases it is assumed to "
                 "be an LTS.",
                 'f');
      desc.add_option("evidence-file",
                      utilities::make_optional_argument("NAME", "name"),
                      "The file to which the evidence is written. If not set, a default name will be chosen.");
      desc.add_option("strategy",
                  utilities::make_optional_argument("STRATEGY", "0"),
                      "use strategy STRATEGY:\n"
                      "  '0' Compute all boolean equations which can be"
                      " reached from the initial state, without"
                      " optimization. This is is the most data efficient"
                      " option per generated equation. (default)\n"
                      "  '1' In addition to 0, remove self loops.\n"
                      "  '2' Optimize by immediately substituting the right"
                      " hand sides for already investigated variables that"
                      " are true or false when generating an expression."
                      " This is as memory efficient as 0.\n"
                      "  '3' In addition to 2, also substitute variables that"
                      " are true or false into an already generated right"
                      " hand side. This can mean that certain variables"
                      " become unreachable (e.g. X0 in X0 and X1, when X1"
                      " becomes false, assuming X0 does not occur elsewhere."
                      " It will be maintained which variables have become"
                      " unreachable as these do not have to be investigated."
                      " Depending on the PBES, this can reduce the size of"
                      " the generated BES substantially but requires a"
                      " larger memory footprint.\n"
                      "  '4' In addition to 3, investigate for generated"
                      " variables whether they occur on a loop, such that"
                      " they can be set to true or false, depending on the"
                      " fixed point symbol. This can increase the time"
                      " needed to generate an equation substantially.",
                 's');
    }


    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
      m_check_strategy = parser.options.count("check-strategy") > 0;
      if (parser.options.count("file") > 0)
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
      if (parser.options.count("evidence-file") > 0)
      {
        evidence_file = parser.option_argument("evidence-file");
      }
      m_search_strategy = parser.option_argument_as<mcrl2::pbes_system::search_strategy>("search");
      m_strategy = parser.option_argument_as<int>("strategy");
      if (m_strategy < 0 || m_strategy > 4)
      {
        throw mcrl2::runtime_error("An invalid value " + std::to_string(m_strategy) + " was specified for the strategy option.");
      }
    }

    std::set<utilities::file_format> available_input_formats() const override
    {
      return { pbes_system::pbes_format_internal() };
    }

  public:
    pbessolve_tool()
      : super("pbessolve",
              "Wieger Wesselink",
              "Generate a BES from a PBES and solve it. ",
              "Solves (P)BES from INFILE. "
              "If INFILE is not present, stdin is used. "
              "The PBES is first instantiated into a parity game, "
              "which is then solved using Zielonka's algorithm. "
              "It supports the generation of a witness or counter "
              "example for the property encoded by the PBES."
             ),
      m_search_strategy(breadth_first)
    {}

    template <typename PbesInstAlgorithm>
    void run_algorithm(const pbes_system::pbes& pbesspec)
    {
      structure_graph G;

      PbesInstAlgorithm algorithm(pbesspec, G, rewrite_strategy(), m_search_strategy, m_strategy);
      mCRL2log(log::verbose) << "Generating parity game..." << std::endl;
      timer().start("instantiation");
      algorithm.run();
      timer().finish("instantiation");

      mCRL2log(log::verbose) << "Number of vertices in the structure graph: " << G.all_vertices().size() << std::endl;

      if (!lpsfile.empty())
      {
        lps::specification lpsspec = lps::detail::load_lps(lpsfile);
        lps::detail::instantiate_global_variables(lpsspec); // N.B. This is necessary, because the global variables might not be valid for the evidence.
        bool result;
        lps::specification evidence;
        timer().start("solving");
        std::tie(result, evidence) = solve_structure_graph_with_counter_example(G, lpsspec, pbesspec, algorithm.equation_index());
        timer().finish("solving");
        std::cout << (result ? "true" : "false") << std::endl;
        if (evidence_file.empty())
        {
          evidence_file = input_filename() + ".evidence.lps";
        }
        lps::detail::save_lps(evidence, evidence_file);
        mCRL2log(log::verbose) << "Saved " << (result ? "witness" : "counter example") << " in " << evidence_file << std::endl;
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
        mCRL2log(log::verbose) << "Saved " << (result ? "witness" : "counter example") << " in " << evidence_file << std::endl;
      }
      else
      {
        timer().start("solving");
        bool result = solve_structure_graph(G, m_check_strategy);
        timer().finish("solving");
        std::cout << (result ? "true" : "false") << std::endl;
      }
    }

    bool run() override
    {
      pbes_system::pbes pbesspec = pbes_system::detail::load_pbes(input_filename());
      pbes_system::algorithms::normalize(pbesspec);

      if (m_strategy <= 1)
      {
        run_algorithm<pbesinst_structure_graph_algorithm>(pbesspec);
      }
      else
      {
        run_algorithm<pbesinst_structure_graph_algorithm2>(pbesspec);
      }
      return true;
    }
};

int main(int argc, char* argv[])
{
  return pbessolve_tool().execute(argc, argv);
}
