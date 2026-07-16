// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbescegar.cpp

#include "mcrl2/pbes/tools/pbescegar.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/pbes/pbes_rewriter_tool.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/input_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using data::tools::rewriter_tool;
using pbes_system::tools::pbes_input_tool;
using pbes_system::tools::pbes_rewriter_tool;

class pbescegar_tool : public pbes_input_tool<pbes_rewriter_tool<rewriter_tool<input_tool>>>
{
protected:
  using super = pbes_input_tool<pbes_rewriter_tool<rewriter_tool<input_tool>>>;

  pbescegar_options m_options;

  void parse_options(const command_line_parser& parser) override { 
      super::parse_options(parser);
      m_options.init_control_flow = parser.has_option("init-cfp");
      m_options.solve_symbolic = parser.has_option("solve-symbolic-args");
      m_options.solve_symbolic_args = parser.option_argument_as<std::string>("solve-symbolic-args");
      // interface_description symbolic_desc;
      // mcrl2::pbes_system::pbessolvesymbolic_tool();
      // command_line_parser symbolic_parser(symbolic_desc, m_options.solve_symbolic_args.c_str());
      
  }

  void add_options(interface_description& desc) override { 
      super::add_options(desc);
      desc.add_option("init-cfp",
                  "Use the (global) control flow parameters as initial parameters.", 'c');
      desc.add_option("solve-symbolic-args",
          utilities::make_optional_argument("STR", ""),
                  "Solve the PBES symbolically using the following arguments.", 's');
  }

public:
  pbescegar_tool()
    : super("pbescegar",
        "Jore Booy",
        "Solve using control flow first, only introducting other parameters iteratively.",
        "Reads a file containing a PBES. If OUTFILE "
        "is not present, standard output is used. If INFILE is not present, standard input is used.")
  {}

  bool run() override
  {
    mCRL2log(verbose) << "pbescegar parameters:" << std::endl;
    mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;

    m_options.rewrite_strategy = rewrite_strategy();
    pbescegar(input_filename(), pbes_input_format(), m_options);

    return true;
  }
};

int main(int argc, char* argv[])
{
  return pbescegar_tool().execute(argc, argv);
}
