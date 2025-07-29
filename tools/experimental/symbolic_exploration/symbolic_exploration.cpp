// Author(s): Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file symbolic_exploration.cpp

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/pbes/pbes_output_tool.h"
#include "mcrl2/pbes/detail/symbolic_exploration.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using pbes_system::tools::pbes_output_tool;
using pbes_system::tools::pbes_input_tool;

class symbolic_exploration_tool: public pbes_input_tool<pbes_output_tool<input_output_tool> >
{
  using super = pbes_input_tool<pbes_output_tool<input_output_tool>>;

protected:
  bool m_optimized;
  bool m_clustered;
  bool m_instantiate;

  void parse_options(const command_line_parser& parser)
  {
    super::parse_options(parser);
    m_optimized = parser.option_argument_as<bool>("optimize");
    m_clustered = parser.options.count("clustered") > 0;
    m_instantiate = parser.options.count("instantiate") > 0;
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("optimize", make_optional_argument("NAME", "1"), "simplify the PBES during the normalization (default true)", 'O');
      desc.add_option("clustered", "generate a clustered PBES", 'c');
      desc.add_option("instantiate", "instantiate the PBES", 'I');
    }

  public:
    symbolic_exploration_tool()
      : super(
        "symbolic_exploration",
        "Wieger Wesselink; Tom Boshoven and Tim Willemse",
        "normalize a PBES to GBQNF format",
        "Reads a file containing a PBES, and applies ??? normalization to it. If OUTFILE "
        "is not present, standard output is used. If INFILE is not present, standard input is used."
      )
    {}

    bool run()
    {
      mCRL2log(verbose) << "symbolic_exploration parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  optimized:          " << std::boolalpha << m_optimized << std::endl;
      mCRL2log(verbose) << "  clustered:          " << std::boolalpha << m_clustered << std::endl;
      mCRL2log(verbose) << "  instantiate:        " << std::boolalpha << m_instantiate << std::endl;
      pbes_system::detail::symbolic_exploration(input_filename(),
                                                output_filename(),
                                                pbes_input_format(),
                                                pbes_output_format(),
                                                m_optimized,
                                                m_clustered,
                                                m_instantiate);
      return true;
    }
};

int main(int argc, char* argv[])
{
  return symbolic_exploration_tool().execute(argc, argv);
}
