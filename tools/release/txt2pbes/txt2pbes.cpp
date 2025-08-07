// Author(s): Aad Mathijssen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./txt2pbes.cpp
/// \brief Parse a textual description of a PBES.

#define NAME "txt2pbes"
#define AUTHOR "Aad Mathijssen, Wieger Wesselink"

//mCRL2 specific
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/pbes/pbes_output_tool.h"
#include "mcrl2/pbes/txt2pbes.h"


using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using mcrl2::pbes_system::tools::pbes_output_tool;

class txt2pbes_tool: public pbes_output_tool<input_output_tool>
{
  using super = pbes_output_tool<input_output_tool>;

protected:
  bool m_normalize = false;

  void parse_options(const command_line_parser& parser) override
  {
    super::parse_options(parser);
    m_normalize = 0 < parser.options.count("normalize");
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("normalize",
                      "normalization is applied, i.e. negations and implications are eliminated. ", 'n');
    }

  public:
    txt2pbes_tool()
      : super(NAME, AUTHOR,
              "parse a textual description of a PBES",
              "Parse the textual description of a PBES from INFILE and write it to OUTFILE. "
              "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used.\n\n"
              )
    {}

    bool run() override
    {
      pbes p;
      if (input_filename().empty())
      {
        //parse specification from stdin
        mCRL2log(log::verbose) << "reading input from stdin..." << std::endl;
        p = txt2pbes(std::cin, m_normalize);
      }
      else
      {
        //parse specification from input filename
        mCRL2log(log::verbose) << "reading input from file '" <<  input_filename() << "'..." << std::endl;
        std::ifstream instream(input_filename().c_str(), std::ifstream::in|std::ifstream::binary);
        if (!instream)
        {
          throw mcrl2::runtime_error("cannot open input file: " + input_filename());
        }
        p = txt2pbes(instream, m_normalize);
      }
      save_pbes(p, output_filename(), pbes_output_format());
      return true;
    }
};

int main(int argc, char** argv)
{
  return txt2pbes_tool().execute(argc, argv);
}
