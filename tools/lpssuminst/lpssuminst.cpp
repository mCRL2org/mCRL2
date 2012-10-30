// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpssuminst.cpp

#include "boost.hpp" // precompiled headers

#include "mcrl2/lps/tools.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class suminst_tool: public rewriter_tool<input_output_tool>
{
  protected:

    typedef rewriter_tool<input_output_tool> super;

    bool m_tau_summands_only;
    bool m_finite_sorts_only;
    std::string m_sorts_string;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("finite", "only instantiate variables whose sorts are finite", 'f');
      desc.add_option("tau", "only instantiate variables in tau summands", 't');
      desc.add_option("sorts",
                       make_optional_argument("NAME", ""),
                       "select sorts that need to be expanded (comma separated list)\n"
                       "  Examples: Bool; Bool, List(Nat)",
                       's');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      m_tau_summands_only = 0 < parser.options.count("tau");
      m_finite_sorts_only = 0 < parser.options.count("finite");
      if(parser.options.count("sorts"))
      {
        m_sorts_string = parser.option_argument("sorts");
        boost::trim(m_sorts_string);
      }

      if(m_finite_sorts_only && !m_sorts_string.empty())
      {
        throw mcrl2::runtime_error("options `--sorts' and `--finite' are mutually exclusive");
      }
    }

  public:

    suminst_tool()
      : super(
        "lpssuminst",
        "Jeroen Keiren",
        "instantiate summation variables of an LPS",
        "Instantiate the summation variables of the linear process specification (LPS) "
        "in INFILE and write the result to OUTFILE. If INFILE is not present, stdin is "
        "used. If OUTFILE is not present, stdout is used."
      )
    {}

    ///Reads a specification from input_file,
    ///applies instantiation of sums to it and writes the result to output_file.
    bool run()
    {
      mcrl2::lps::lpssuminst(m_input_filename,
                             m_output_filename,
                             m_sorts_string,
                             m_finite_sorts_only,
                             m_tau_summands_only);
      return true;
    }
};

class suminst_gui_tool: public mcrl2_gui_tool<suminst_tool>
{
  public:
    suminst_gui_tool()
    {

      m_gui_options["finite"] = create_checkbox_widget();
      add_rewriter_widget();
      m_gui_options["tau"] = create_checkbox_widget();
    }
};

int main(int argc, char** argv)
{
  return suminst_gui_tool().execute(argc, argv);
}
