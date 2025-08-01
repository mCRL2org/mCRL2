// Author(s): Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsbisim2pbes.cpp

#include "mcrl2/lps/io.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/bisimulation_type.h"
#include "mcrl2/pbes/bisimulation.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes_output_tool.h"
#include "mcrl2/utilities/input_input_output_tool.h"


using namespace mcrl2;
using namespace mcrl2::pbes_system;
using utilities::command_line_parser;
using utilities::interface_description;
using utilities::make_enum_argument;
using utilities::tools::input_input_output_tool;
using namespace mcrl2::log;
using pbes_system::tools::pbes_output_tool;

using super = pbes_output_tool<input_input_output_tool>;
class lpsbisim2pbes_tool: public super
{
  protected:
    /// \brief The type of bisimulation
    bisimulation_type m_bisimulation_type;

    /// \brief If true the result is normalized
    bool m_normalize = false;

    /// \brief Parse non-standard options
    /// \param parser A command line parser
    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      m_normalize = parser.options.count("normalize") > 0;
      m_bisimulation_type = parser.option_argument_as<bisimulation_type>("bisimulation");
    }

    void add_options(interface_description& desc) override /*< One can add command line
                     options by overriding the virtual function `add_options`. >*/
    {
      super::add_options(desc);
      desc.add_option("normalize", "normalize the result", 'n');
      desc.add_option("bisimulation", make_enum_argument<bisimulation_type>("NAME")
                      .add_value(strong_bisim, true)  // This is the default
                      .add_value(weak_bisim)
                      .add_value(branching_bisim)
                      .add_value(branching_sim)
                      ,"generate a PBES for the bisimulation type NAME:"
                      ,'b'
                     );
    }

  public:
    lpsbisim2pbes_tool()
      : super(
        "lpsbisim2pbes",
        "Wieger Wesselink; Tim Willemse and Bas Ploeger",
        "computes a bisimulation relation between two LPSs",
        "Reads two files containing an LPS, and computes a PBES that expresses "
        "bisimulation between the two. If OUTFILE is not present, standard "
        "output is used.\n"
      )
    {}

    bool run() override
    {
      mCRL2log(verbose) << "lpsbisim2pbes parameters:" << std::endl;
      mCRL2log(verbose) << "  input file 1 :         " << input_filename1() << std::endl;
      mCRL2log(verbose) << "  input file 2 :         " << input_filename2() << std::endl;
      mCRL2log(verbose) << "  output file  :         " << output_filename() << std::endl;
      mCRL2log(verbose) << "  bisimulation :         " << m_bisimulation_type << std::endl;
      mCRL2log(verbose) << "  normalize    :         " << m_normalize << std::endl;

      lps::specification M;
      lps::specification S;

      load_lps(M, input_filename1());
      load_lps(S, input_filename2());
      pbes result;
      switch (m_bisimulation_type)
      {
        case strong_bisim:
          result = strong_bisimulation(M, S);
          break;
        case weak_bisim:
          result = weak_bisimulation(M, S);
          break;
        case branching_bisim:
          result = branching_bisimulation(M, S);
          break;
        case branching_sim:
          result = branching_simulation_equivalence(M, S);
          break;
      }
      if (m_normalize)
      {
        algorithms::normalize(result);
      }
      save_pbes(result, output_filename(), pbes_output_format());
      return true;
    }
};

int main(int argc, char* argv[])
{
  lpsbisim2pbes_tool tool;
  return tool.execute(argc, argv);
}

