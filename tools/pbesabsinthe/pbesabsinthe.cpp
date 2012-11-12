// Author(s): Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesabsint.cpp

#include "boost.hpp" // precompiled headers

#include <iostream>
#include <string>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/pbes/absinthe_strategy.h"
#include "mcrl2/pbes/tools.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class pbes_absinthe_tool: public input_output_tool
{
  protected:
    typedef input_output_tool super;

    std::string m_abstraction_file;
    bool m_print_used_function_symbols;
    bool m_enable_logging;
    absinthe_strategy m_strategy;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      m_strategy = parser.option_argument_as<absinthe_strategy>("strategy");
      m_abstraction_file = parser.option_argument("abstraction-file");
      m_print_used_function_symbols = parser.options.count("used-function-symbols") > 0;
      m_enable_logging = parser.options.count("enable-logging") > 0;
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);

      desc.add_option("abstraction-file",
                       make_file_argument("FILE"),
                       "use the abstraction specification in FILE. ",
                       'a');

      desc.add_option("strategy",
                       make_enum_argument<absinthe_strategy>("NAME")
                      .add_value(absinthe_over)
                      .add_value(absinthe_under),
                       "use the approximation strategy NAME:",
                       's');
      desc.add_option("used-function-symbols", "print used function symbols", 'u');
      desc.add_option("enable-logging", "print absinthe specific log messages", 'l');
    }

  public:
    pbes_absinthe_tool()
      : super(
        "pbesabsinthe",
        "Wieger Wesselink; Maciek Gazda and Tim Willemse",
        "apply data domain abstracion to a PBES",
        "Reads a file containing a PBES, and applies abstraction to it's data domain, based on a\n"
        "user defined mappings. If OUTFILE is not present, standard output is used. If INFILE is not\n"
        "present, standard input is used."
      )
    {}

    bool run()
    {
      mCRL2log(verbose) << "pbesabsinthe parameters:  " << std::endl;
      mCRL2log(verbose) << "  input file:             " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:            " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  abstraction file:       " << m_abstraction_file << std::endl;
      mCRL2log(verbose) << "  approximation strategy: " << print_absinthe_strategy(m_strategy) << std::endl;

      pbesabsinthe(input_filename(),
                   output_filename(),
                   m_abstraction_file,
                   m_strategy,
                   m_print_used_function_symbols,
                   m_enable_logging
                 );
      return true;
    }

};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return pbes_absinthe_tool().execute(argc, argv);
}
