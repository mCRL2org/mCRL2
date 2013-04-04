// Author(s): Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbestransform.cpp

#include "boost.hpp" // precompiled headers

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/pbes/transform.h"
#include "mcrl2/pbes/io.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

//[pbes_transform_tool
class pbes_transform_tool: public input_output_tool
{
  protected:
    typedef input_output_tool super;

    unsigned int m_iterations;
    bool m_mu_value;
    bool m_nu_value;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      m_mu_value = parser.option_argument_as<bool>("mu-value");
      m_nu_value = parser.option_argument_as<bool>("nu-value");
      m_iterations = parser.option_argument_as<int>("iterations");
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("iterations", make_optional_argument("NAME", "1"), "the number of iterations", 'i');
      desc.add_option("mu-value", make_optional_argument("NAME", "0"), "the value for mu", 'm');
      desc.add_option("nu-value", make_optional_argument("NAME", "0"), "the value for nu", 'n');
    }

  public:
    pbes_transform_tool()
      : super(
        "pbestransform",
        "Wieger Wesselink; Sjoerd Cranen",
        "various transformations on a PBES",
        "Reads a file containing a PBES, and applies a transformation to it. If OUTFILE "
        "is not present, standard output is used. If INFILE is not present, standard input is used."
      )
    {}

    bool run()
    {
      pbes_system::pbes p;
      load_pbes(p, input_filename());
      pbes_system::pbes_transform(p, m_iterations, m_mu_value, m_nu_value);
      p.save(output_filename());
      return true;
    }
};

class pbes_transform_gui_tool: public mcrl2_gui_tool<pbes_transform_tool>
{
  public:
    pbes_transform_gui_tool() {}
};

int main(int argc, char** argv)
{
  pbes_transform_gui_tool tool;
  return tool.execute(argc, argv);
}
