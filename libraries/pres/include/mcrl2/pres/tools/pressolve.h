// Author(s): Jan Friso Groote. Based on pbessolve.h by Wieger Wesselink; Threads are added by Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pressolve.h
/// \brief This tool transforms an .lps file into a labelled transition system.
///        Optionally, it can be run with multiple treads.

#ifndef MCRL2_PRES_TOOLS_PRESSOLVE_H
#define MCRL2_PRES_TOOLS_PRESSOLVE_H

#include "mcrl2/res/pres_input_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/lps/detail/lps_io.h"
#include "mcrl2/pres/detail/pres_io.h"
#include "mcrl2/pres/pressolve_options.h"
#include "mcrl2/pres/normalize.h"
#include "mcrl2/pres/pres2res.h"
#include "mcrl2/pres/ressolve_gauss_elimination.h"
#include "mcrl2/pres/ressolve_numerical.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::pres_system;
using namespace mcrl2::utilities::tools;
using mcrl2::res::tools::pres_input_tool;
using mcrl2::data::tools::rewriter_tool;
using mcrl2::utilities::tools::input_tool;

// TODO: put this code in the utilities library?
inline std::string file_extension(const std::string& filename)
{
  std::size_t pos = filename.find_last_of('.');
  if (pos == std::string::npos)
  {
    return "";
  }
  return filename.substr(pos + 1);
}

class pressolve_tool
    : public rewriter_tool<pres_input_tool<input_tool>>
{
  protected:
  typedef rewriter_tool<pres_input_tool<input_tool>> super;

  pressolve_options options;
  std::string lpsfile;

  void add_options(utilities::interface_description& desc) override
  {
    super::add_options(desc);
    desc.add_hidden_option("no-remove-unused-rewrite-rules",
                           "do not remove unused rewrite rules. ", 'u');
    desc.add_hidden_option(
        "no-replace-constants-by-variables",
        "Do not move constant expressions to a substitution.");
    desc.add_option("algorithm", utilities::make_enum_argument<pres_system::solution_algorithm>("NAME")
                                 .add_value_short(pres_system::solution_algorithm::gauss_elimination, "g", true)
                                 .add_value_short(pres_system::solution_algorithm::numerical, "n"),
                    "select the algorithm NAME to solve the res after it is generated.",'a');
  }

  void parse_options(const utilities::command_line_parser& parser) override
  {
    super::parse_options(parser);

    options.replace_constants_by_variables =
        !parser.has_option("no-replace-constants-by-variables");
    options.remove_unused_rewrite_rules =
        !parser.has_option("no-remove-unused-rewrite-rules");
    options.rewrite_strategy = rewrite_strategy();
    

    if (parser.has_option("file"))
    {
      std::string filename = parser.option_argument("file");
      if (file_extension(filename) == "lps")
      {
        lpsfile = filename;
      }
    }

    if (parser.has_option("algorithm"))
    {
      options.algorithm = parse_algorithm(parser.option_argument("algorithm"));
    }
  }

  std::set<utilities::file_format> available_input_formats() const override
  {
    return {pres_system::pres_format_internal()};
  }

  public:
  pressolve_tool(const std::string& toolname)
      : super(toolname, 
              "Jan Friso Groote",
              "Generate a BES from a PRES and solve it. ",
              "Solves (P)BES from INFILE. "
              "If INFILE is not present, stdin is used. "
              "The PRES is first instantiated into a parity game, "
              "which is then solved using Zielonka's algorithm. "
              "It supports the generation of a witness or counter "
              "example for the property encoded by the PRES.")
  {
  }

  bool run() override
  {
    pres_system::pres presspec = pres_system::detail::load_pres(input_filename());
    pres_system::normalize(presspec);
    
    mCRL2log(log::verbose) << "Generating RES..." << std::endl;
    timer().start("instantiation");
    pres2res_algorithm pres2res(options,presspec);
    pres resulting_res = pres2res.run();
    timer().finish("instantiation");

    mCRL2log(log::verbose) << "RESULTING RES\n" << resulting_res << "\n";

    timer().start("solving");
    if (options.algorithm==gauss_elimination)
    {
      ressolve_by_gauss_elimination_algorithm solver(options, resulting_res);
      pres_expression result = solver.run();
      std::cout << "Solution: " << result << std::endl;
    }
    else 
    {
      ressolve_by_numerical_iteration solver(options, resulting_res);
      double result = solver.run();
      std::cout << "Solution: " << result << std::endl;
    }  
    timer().finish("solving");
    /* presinst_structure_graph_algorithm algorithm(options, presspec, G);
    run_algorithm<presinst_structure_graph_algorithm>(algorithm, presspec, G,
                                                        sigma); */
    return true;
  }
};

#endif // MCRL2_PRES_TOOLS_PRESSOLVE_H
