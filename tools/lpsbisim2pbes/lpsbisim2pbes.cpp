// Author(s): Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsbisim2pbes.cpp

#include "boost.hpp" // precompiled headers

#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/bisimulation.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/utilities/tool.h"
#include "mcrl2/utilities/input_input_output_tool.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/exception.h"

using namespace mcrl2;
using namespace mcrl2::lps;
using namespace mcrl2::pbes_system;
using utilities::command_line_parser;
using utilities::interface_description;
using utilities::make_mandatory_argument;
using utilities::tools::tool;
using utilities::tools::input_input_output_tool;
using namespace mcrl2::log;

enum bisimulation_type
{
  strong_bisim,
  weak_bisim,
  branching_bisim,
  branching_sim
};

static
bisimulation_type parse_bisimulation_type(const std::string& type)
{
  if (type == "strong-bisim"        ) return strong_bisim;
  else if (type == "weak-bisim"     ) return weak_bisim;  
  else if (type == "branching-bisim") return branching_bisim;
  else if (type == "branching-sim"  ) return branching_sim;
  throw mcrl2::runtime_error(std::string("unknown bisimulation type ") + type + "!");
  return strong_bisim;
}

static
std::string print_bisimulation_type(int type)
{
  switch (type)
  {
    case strong_bisim:
      return "strong bisimulation";
    case weak_bisim:
      return "weak bisimulation";
    case branching_bisim:
      return "branching bisimulation";
    case branching_sim:
      return "branching simulation equivalence";
  }
  return "unknown type";
}

typedef input_input_output_tool super;
class lpsbisim2pbes_tool: public super
{
  protected:
    /// \brief The type of bisimulation
    bisimulation_type m_bisimulation_type;

    /// \brief If true the result is normalized
    bool normalize;

    /// \brief Parse non-standard options
    /// \param parser A command line parser
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      if (2 < parser.arguments.size())
      {
        m_output_filename = parser.arguments[2];
      }
      normalize = parser.options.count("normalize") > 0;

      m_bisimulation_type = parse_bisimulation_type(parser.option_argument("bisimulation"));
    }

    void add_options(interface_description& desc) /*< One can add command line
                     options by overriding the virtual function `add_options`. >*/
    {
      super::add_options(desc);
      desc.add_option("normalize", "normalize the result", 'n');
      desc.add_option("bisimulation", make_mandatory_argument("NAME"),
                      "generate a PBES for the bisimulation type NAME:\n"
                      "'strong-bisim' for strong bisimilarity,\n"
                      "'weak-bisim' for weak bisimilarity,'\n"
                      "'branching-bisim' for branching bisimilarity,\n"
                      "'branching-sim' for branching simulation equivalence"
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

    bool run()
    {
      specification M;
      specification S;

      mCRL2log(verbose) << "lpsbisim2pbes parameters:" << std::endl;
      mCRL2log(verbose) << "  input file 1 :         " << input_filename1() << std::endl;
      mCRL2log(verbose) << "  input file 2 :         " << input_filename2() << std::endl;
      mCRL2log(verbose) << "  output file  :         " << output_filename() << std::endl;
      mCRL2log(verbose) << "  bisimulation :         " << print_bisimulation_type(m_bisimulation_type) << std::endl;
      mCRL2log(verbose) << "  normalize    :         " << normalize << std::endl;

      M.load(input_filename1());
      S.load(input_filename2());
      pbes<> result;
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
      if (normalize)
      {
        pbes_system::normalize(result);
      }
      result.save(output_filename());

      return true;
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  lpsbisim2pbes_tool tool;
  return tool.execute(argc, argv);
}

