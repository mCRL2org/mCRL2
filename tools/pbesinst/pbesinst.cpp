// Author(s): Alexander van Dam, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesinst.cpp
/// \brief Add your file description here.

//#define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG

#include <stdexcept>
#include <iostream>
#include <string>
#include <utility>

#include "mcrl2/pbes/tools.h"
#include "mcrl2/pbes/detail/bes_equation_limit.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/pbes_input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using utilities::command_line_parser;
using utilities::interface_description;
using utilities::make_optional_argument;
using utilities::make_enum_argument;
using utilities::tools::input_output_tool;
using utilities::tools::pbes_input_output_tool;
using utilities::tools::rewriter_tool;

/// The pbesinst tool.
class pbesinst_tool: public rewriter_tool<pbes_input_output_tool<input_output_tool> >
{
  protected:
    typedef rewriter_tool<pbes_input_output_tool<input_output_tool> > super;

    pbesinst_strategy m_strategy;
    pbes_file_format m_output_format;
    std::string m_finite_parameter_selection;
    bool m_aterm_ascii;
    bool m_remove_redundant_equations;

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      if (parser.options.count("select") > 0)
      {
        m_finite_parameter_selection = parser.option_argument("select");
        boost::trim(m_finite_parameter_selection);
      }

      if (parser.options.count("equation_limit") > 0)
      {
        int limit = parser.option_argument_as<int>("equation_limit");
        pbes_system::detail::set_bes_equation_limit(limit);
      }

      m_strategy = parser.option_argument_as<pbesinst_strategy>("strategy");
      m_remove_redundant_equations = parser.options.count("remove-equations") > 0;
      m_aterm_ascii = parser.options.count("aterm-ascii") > 0;
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.
      add_option("strategy",
                 make_enum_argument<pbesinst_strategy>("NAME")
                 .add_value(pbesinst_lazy, true)
                 .add_value(pbesinst_finite),
                 "compute the BES using strategy NAME:", 's').
      add_option("select",
                 make_optional_argument("PARAMS", ""),
                 "select finite parameters that need to be expanded\n"
                 "  Examples: X1(b:Bool,c:Bool);X2(b:Bool)\n"
                 "            *(*:Bool)\n",
                 'f');
      desc.add_option("aterm-ascii", "store ATerms in ascii format (default: false)", 'a');
      desc.add_hidden_option("equation_limit",
                             make_optional_argument("NAME", "-1"),
                             "Set a limit to the number of generated BES equations",
                             'l');
      desc.add_option("remove-equations", "remove redundant equations", 'e');
    }

  public:
    /// Constructor.
    pbesinst_tool()
      : super(
        "pbesinst",
        "Wieger Wesselink; Alexander van Dam and Tim Willemse",
        "compute a BES out of a PBES",
        "Transforms the PBES from INFILE into an equivalent BES and writes it to OUTFILE. "
        "If INFILE is not present, standard input is used. If OUTFILE is not present,   "
        "standard output is used."
        "The format of OUTFILE is determined by its extension (unless it is specified "
        "by an option). The supported formats are:\n"
        "  'pbes' for the mCRL2 PBES format,\n"
        "  'bes'  for the mCRL2 BES format,\n"
        "  'cwi'  for the CWI BES format\n"
      ),
      m_strategy(pbesinst_lazy),
      m_aterm_ascii(false)
    {}

    /// Runs the algorithm.
    bool run()
    {
      using namespace mcrl2::pbes_system;

      mCRL2log(verbose) << "parameters of pbesinst:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  strategy:           " << m_strategy << std::endl;
      mCRL2log(verbose) << "  output format:      " << pbes_system::file_format_to_string(pbes_output_format()) << std::endl;
      mCRL2log(verbose) << "  remove redundant equations: " << std::boolalpha << m_remove_redundant_equations << std::endl;

      return pbesinst(input_filename(),
              output_filename(),
              pbes_input_format(),
              pbes_output_format(),
              rewrite_strategy(),
              m_strategy,
              m_finite_parameter_selection,
              m_remove_redundant_equations,
              m_aterm_ascii
             );
    }
};

int main(int argc, char* argv[])
{
  return pbesinst_tool().execute(argc, argv);
}
