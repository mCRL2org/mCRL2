// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsconstelm.cpp

#include "boost.hpp" // precompiled headers

#include <iostream>
#include <string>
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/new_data/rewriter.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/constelm.h"

using namespace mcrl2;
using utilities::tools::input_output_tool;
using utilities::tools::rewriter_tool;

class lps_constelm_tool: public rewriter_tool<input_output_tool>
{
  protected:
    typedef rewriter_tool<input_output_tool> super;

  public:
    lps_constelm_tool()
      : super(
          "lpsconstelm",
          "Wieger Wesselink",
          "remove constant parameters from an LPS",
          "Reads a file containing an LPS, and applies constant parameter elimination to it. If OUTFILE "
          "is not present, standard output is used. If INFILE is not present, standard input is used."
        )
    {}

    bool run()
    {
      if (mcrl2::core::gsVerbose)
      {
        std::cerr << "lpsconstelm parameters:" << std::endl;
        std::cerr << "  input file:         " << m_input_filename << std::endl;
        std::cerr << "  output file:        " << m_output_filename << std::endl;
      }

      // load the pbes
      lps::specification spec;
      spec.load(m_input_filename);

      // data rewriter
      data::rewriter datar = create_rewriter(spec.data());

      // apply the constelm algorithm
      spec = lps::constelm(spec, datar, true);

      // save the result
      spec.save(m_output_filename);

      return true;
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  lps_constelm_tool tool;
  return tool.execute(argc, argv);
}
