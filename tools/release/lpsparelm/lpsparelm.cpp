// Author(s): J.van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./lpsparelm.cpp

#include "mcrl2/lps/io.h"
#include "mcrl2/lps/parelm.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using mcrl2::utilities::tools::input_output_tool;
using namespace mcrl2::utilities;

class lps_parelm_tool : public input_output_tool
{
  using super = input_output_tool;

public:

  lps_parelm_tool()
      : super("lpsparelm",
            "Wieger Wesselink and Jeroen van der Wulp; Frank Stappers and Tim Willemse",
            "remove unused parameters from an LPS",
            "Remove unused parameters from the linear process specification (LPS) in INFILE "
            "and write the result to OUTFILE. If INFILE is not present, stdin is used. If "
            "OUTFILE is not present, stdout is used.")
  {}

  bool run() override
  {
    lps::stochastic_specification spec;
    load_lps(spec, input_filename());
    lps::parelm(spec, true);
    save_lps(spec, output_filename());
    return true;
    }

};

int main(int argc, char** argv)
{
  return lps_parelm_tool().execute(argc, argv);
}

