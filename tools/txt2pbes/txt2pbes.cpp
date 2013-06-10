// Author(s): Aad Mathijssen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./txt2pbes.cpp
/// \brief Parse a textual description of a PBES.

#include "boost.hpp" // precompiled headers

#define NAME "txt2pbes"
#define AUTHOR "Aad Mathijssen, Wieger Wesselink"

//C++
#include <cstdio>
#include <fstream>
#include <string>

//mCRL2 specific
#include "mcrl2/pbes/tools.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class txt2pbes_tool: public input_output_tool
{
  typedef input_output_tool super;

  protected:
    bool m_normalize;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      m_normalize = !(0 < parser.options.count("no-normalize"));
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("no-normalize",
                      "normalization is not applied."
                      "By default txt2pbes attempts to normalize the PBES, i.e. to "
                      "remove negations and implications. ", 'n');
    }

  public:
    txt2pbes_tool()
      : super(NAME, AUTHOR,
              "parse a textual description of a PBES",
              "Parse the textual description of a PBES from INFILE and write it to OUTFILE. "
              "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used.\n\n"
              )
    {}

    bool run()
    {
      txt2pbes(input_filename(),
               output_filename(),
               m_normalize
              );
      return true;
    }
};

int main(int argc, char** argv)
{
  return txt2pbes_tool().execute(argc, argv);
}
