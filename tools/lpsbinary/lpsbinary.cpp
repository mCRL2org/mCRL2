// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsbinary.cpp
/// \brief The binary tool, this runs the binary algorithm.

#include "boost.hpp" // precompiled headers

#include "mcrl2/lps/tools.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class binary_tool: public rewriter_tool<input_output_tool>
{
  protected:

    typedef rewriter_tool<input_output_tool> super;

  public:

    binary_tool()
      : super(
        "lpsbinary",
        "Jeroen Keiren",
        "replace finite sort variables by vectors of boolean variables in an LPS",
        "Replace finite sort variables by vectors of boolean variables in the LPS in "
        "INFILE and write the result to OUTFILE. If INFILE is not present, stdin is used. "
        "If OUTFILE is not present, stdout is used"
      )
    {}

    bool run()
    {
      mcrl2::lps::lpsbinary(m_input_filename, m_output_filename);
      return true;
    }

};

class binary_gui_tool: public mcrl2_gui_tool<binary_tool>
{
  public:
    binary_gui_tool()
    {
      add_rewriter_widget();
    }
};

int main(int argc, char** argv)
{
  return binary_gui_tool().execute(argc, argv);
}
