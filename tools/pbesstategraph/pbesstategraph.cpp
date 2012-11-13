// Author(s): Wieger Wesselink, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesstategraph.cpp

#include "boost.hpp" // precompiled headers

#include "mcrl2/pbes/tools.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class pbes_stategraph_tool: public input_output_tool
{
  typedef input_output_tool super;

  protected:
    bool m_simplify;
    bool m_apply_to_original;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      m_simplify = parser.option_argument_as<bool>("simplify");
      m_apply_to_original = parser.option_argument_as<bool>("apply-to-original");
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("simplify", make_optional_argument("NAME", "1"), "simplify the PBES during reduction", 's');
      desc.add_option("apply-to-original", make_optional_argument("NAME", "0"), "apply reduction on the original PBES", 'a');
    }

  public:
    pbes_stategraph_tool()
      : super(
        "pbesstategraph",
        "Wieger Wesselink; Jeroen Keiren",
        "reduces a PBES",
        "Reads a file containing a PBES, and reduces it based on an analysis of control flow parameters."
        "If OUTFILE is not present, standard output is used. If INFILE is not present, standard input is used."
      )
    {}

    bool run()
    {
      mCRL2log(verbose) << "pbesstategraph parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;

      pbesstategraph(input_filename(),
                     output_filename(),
                     m_simplify,
                     m_apply_to_original
                    );
      return true;
    }
};

class pbes_stategraph_gui_tool: public mcrl2_gui_tool<pbes_stategraph_tool>
{
  public:
    pbes_stategraph_gui_tool()
    {
      m_gui_options["simplify"] = create_checkbox_widget();
    }
};

int main(int argc, char* argv[])
{
  pbes_stategraph_gui_tool tool;
  return tool.execute(argc, argv);
}

