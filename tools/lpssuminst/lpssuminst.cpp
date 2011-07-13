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

#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/lps/suminst.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::core;

class suminst_tool: public rewriter_tool<input_output_tool>
{
  protected:

    typedef rewriter_tool<input_output_tool> super;

    bool m_tau_summands_only;
    bool m_finite_sorts_only;
    std::string m_sorts_string;
    atermpp::set<data::sort_expression> m_sorts;

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

    atermpp::set<data::sort_expression> parse_sort_expressions(const std::string& s, const data::data_specification& spec)
    {
      std::vector<std::string> parts = utilities::split(utilities::remove_whitespace(s), ",");
      atermpp::set<data::sort_expression> result;

      for(std::vector<std::string>::const_iterator i = parts.begin(); i != parts.end(); ++i)
      {
        result.insert(data::parse_sort_expression(*i, spec));
      }

      return result;
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
      lps::specification lps_specification;
      lps_specification.load(m_input_filename);

      // Determine set of sorts to be expanded
      if(!m_sorts_string.empty())
      {
        m_sorts = parse_sort_expressions(m_sorts_string, lps_specification.data());
      }
      else if (m_finite_sorts_only)
      {
        m_sorts = lps::finite_sorts(lps_specification.data());
      }
      else
      {
        m_sorts = atermpp::convert<atermpp::set<data::sort_expression> >(lps_specification.data().sorts());
      }

      mCRL2log(verbose, "lpssuminst") << "expanding summation variables of sorts: " << data::pp(m_sorts) << std::endl;

      mcrl2::data::rewriter r = create_rewriter(lps_specification.data());
      lps::suminst_algorithm<data::rewriter>(lps_specification, r, m_sorts, m_tau_summands_only).run();
      lps_specification.save(m_output_filename);

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
  MCRL2_ATERMPP_INIT(argc, argv)

  return suminst_gui_tool().execute(argc, argv);
}
