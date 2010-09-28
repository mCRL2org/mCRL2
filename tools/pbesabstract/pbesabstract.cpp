// Author(s): Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesabstract.cpp

#include "boost.hpp" // precompiled headers

#include <iostream>
#include <string>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/abstract.h"
#include "mcrl2/pbes/detail/pbes2bes_variable_map_parser.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using utilities::tools::input_output_tool;

class pbes_abstract_tool: public input_output_tool
{
  protected:
    typedef input_output_tool super;

    std::string m_parameter_selection;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      if (parser.options.count("select") > 0)
      {
      	m_parameter_selection = parser.option_argument("select");
      	boost::trim(m_parameter_selection);
      }
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("select",
          make_optional_argument("NAME", ""),
          "select parameters that need to be expanded\n"
          "  Examples: X1(b:Bool,c:Bool);X2(b:Bool)\n"
          "            *(*:Bool)\n",
          'f');
    }

  public:
    pbes_abstract_tool()
      : super(
          "pbesabstract",
          "Wieger Wesselink; Tom Haenen and Tim Willemse",
          "Abstract parameters of a PBES",
          "Reads a file containing a PBES, and applies the abstract algorithm. "
          "If OUTFILE is not present, standard output is used."
          "If INFILE is not present, standard input is used."
        )
    {}

    bool run()
    {
      if (mcrl2::core::gsVerbose)
      {
        std::cerr << "pbesabstract parameters:" << std::endl;
        std::cerr << "  input file:         " << m_input_filename << std::endl;
        std::cerr << "  output file:        " << m_output_filename << std::endl;
        std::cerr << "  parameters:         " << m_parameter_selection << std::endl;
      }

      unsigned int log_level = 0;
      if (mcrl2::core::gsVerbose)
      {
        log_level = 1;
      }
      if (mcrl2::core::gsDebug)
      {
        log_level = 2;
      }     	

      // load the pbes
      pbes<> p;
      p.load(m_input_filename);

      // run the algorithm
      pbes_abstract_algorithm algorithm(log_level);
      pbes2bes_variable_map variable_map = pbes_system::detail::parse_variable_map(p, m_parameter_selection);
      algorithm.run(p, variable_map);

      // save the result
      p.save(m_output_filename);

      return true;
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  pbes_abstract_tool tool;
  return tool.execute(argc, argv);
}
