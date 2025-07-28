// Author(s): Jan Friso Groote. Based on txt2bes by Wieger Wesselink.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file txt2pres.cpp
/// \brief Parse a textual description of a BES.

//mCRL2 specific
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/pres/pres_output_tool.h"
#include "mcrl2/pres/txt2pres.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using pres_system::tools::pres_output_tool;

class txt2res_tool: public pres_output_tool<input_output_tool>
{
  using super = pres_output_tool<input_output_tool>;

public:
  txt2res_tool()
      : super("lps2res",
            "Jan Friso Groote",
            "parse a textual description of a BES",
            "Parse the textual description of a BES from INFILE and write it to OUTFILE. "
            "If INFILE is not present, stdin is used. If OUTFILE is not present, stdout is used.")
  {}

  bool run() override
  {
    pres_system::pres p;
    if (input_filename().empty())
    {
      p = pres_system::txt2pres(std::cin);
    }
    else
    {
      std::ifstream from(input_filename().c_str());
      p = pres_system::txt2pres(from);
    }
    pres_system::save_pres(p, output_filename(), pres_output_format());
    return true;
    }
};

int main(int argc, char** argv)
{
  return txt2res_tool().execute(argc, argv);
}
