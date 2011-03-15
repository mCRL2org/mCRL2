// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpssumelm.cpp

#include "boost.hpp" // precompiled headers

//LPS Framework
#include "mcrl2/lps/sumelm.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::core;

class sumelm_tool: public input_output_tool
{
  protected:

    typedef input_output_tool super;

  public:

    sumelm_tool()
      : super(
        "lpssumelm",
        "Jeroen Keiren",
        "remove superfluous summations from an LPS",
        "Remove superfluous summations from the linear process specification (LPS) in "
        "INFILE and write the result to OUTFILE. If INFILE is not present, stdin is used. "
        "If OUTFILE is not present, stdout is used.")
    {}

    ///Reads a specification from input_file,
    ///applies sum elimination to it and writes the result to output_file.
    bool run()
    {
      lps::specification lps_specification;

      lps_specification.load(m_input_filename);

      // apply sum elimination to lps_specification and save the output to a binary file
      lps::sumelm_algorithm(lps_specification, core::gsVerbose||core::gsDebug).run();

      gsDebugMsg("Sum elimination completed, saving to %s\n", m_output_filename.c_str());
      lps_specification.save(m_output_filename);

      return true;
    }

};

class sumelm_gui_tool: public mcrl2_gui_tool<sumelm_tool>
{
  public:
    sumelm_gui_tool()
    { }
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return sumelm_gui_tool().execute(argc, argv);
}
