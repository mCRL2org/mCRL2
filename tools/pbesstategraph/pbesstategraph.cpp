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
#include "mcrl2/utilities/rewriter_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class pbes_stategraph_tool: public rewriter_tool<input_output_tool>
{
  typedef rewriter_tool<input_output_tool> super;

  protected:
    bool m_simplify;
    bool m_apply_to_original;
    bool m_use_pfnf_variant;
    bool m_use_local_variant;
    bool m_print_influence_graph;
    bool m_use_marking_optimization;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      m_simplify = parser.option_argument_as<bool>("simplify");
      m_apply_to_original = parser.option_argument_as<bool>("apply-to-original");
      m_use_pfnf_variant = parser.option_argument_as<bool>("use-pfnf-variant");
      m_use_local_variant = parser.option_argument_as<bool>("use-local-variant");
      m_print_influence_graph = parser.option_argument_as<bool>("print-influence-graph");
      m_use_marking_optimization = parser.option_argument_as<bool>("use-marking-optimization");
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("simplify", make_optional_argument("NAME", "1"), "simplify the PBES during reduction", 's');
      desc.add_option("apply-to-original", make_optional_argument("NAME", "0"), "apply reduction on the original PBES", 'a');
      desc.add_option("use-pfnf-variant", make_optional_argument("NAME", "0"), "first convert the PBES into PFNF format", 'p');
      desc.add_option("use-local-variant", make_optional_argument("NAME", "0"), "use the local variant of the algorithm", 'l');
      desc.add_option("print-influence-graph", make_optional_argument("NAME", "0"), "print the influence graph", 'i');
      desc.add_option("use-marking-optimization", make_optional_argument("NAME", "0"), "apply an optimization during marking", 'm');
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
      mCRL2log(verbose) << "  input file:               " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:              " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  simplify:                 " << std::boolalpha << m_simplify << std::endl;
      mCRL2log(verbose) << "  apply to original:        " << std::boolalpha << m_apply_to_original << std::endl;
      mCRL2log(verbose) << "  use pfnf variant:         " << std::boolalpha << m_use_pfnf_variant << std::endl;
      mCRL2log(verbose) << "  use local variant:        " << std::boolalpha << m_use_local_variant << std::endl;
      mCRL2log(verbose) << "  print influence graph:    " << std::boolalpha << m_print_influence_graph << std::endl;
      mCRL2log(verbose) << "  use marking optimization: " << std::boolalpha << m_use_marking_optimization << std::endl;

      pbesstategraph(input_filename(),
                     output_filename(),
                     rewrite_strategy(),
                     m_simplify,
                     m_apply_to_original,
                     m_use_pfnf_variant,
                     m_use_local_variant,
                     m_print_influence_graph,
                     m_use_marking_optimization
                    );
      return true;
    }
};

int main(int argc, char* argv[])
{
  return pbes_stategraph_tool().execute(argc, argv);
}

