// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsbinary.cpp
/// \brief The binary tool, this runs the binary algorithm.

#include "mcrl2/lps/tools.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"

using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using mcrl2::data::tools::rewriter_tool;

class binary_tool: public rewriter_tool<input_output_tool>
{
  protected:

    typedef rewriter_tool<input_output_tool> super;

    std::string m_parameter_selection;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("select", make_mandatory_argument("[PARAMS]"),
                      "select parameters that need to be replaced. "
                      "All selected parameters must have a finite sort other than Bool. "
                      "Examples: --select=a:*,b:Data or --select=*:Enum4.",
                      'f');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      if (0 < parser.options.count("select"))
      {
        m_parameter_selection = parser.option_argument("select");
        trim(m_parameter_selection);
        if (m_parameter_selection.empty())
        {
          mCRL2log(mcrl2::log::info) << "Ignoring option --select since its argument is empty." << std::endl;
        }
      }
    }
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
      mcrl2::lps::lpsbinary(m_input_filename, m_output_filename, m_parameter_selection);
      return true;
    }

};

int main(int argc, char** argv)
{
  return binary_tool().execute(argc, argv);
}
