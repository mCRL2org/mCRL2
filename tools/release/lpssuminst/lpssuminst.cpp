// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpssuminst.cpp

#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/suminst.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::lps;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

using mcrl2::data::tools::rewriter_tool;

class suminst_tool: public rewriter_tool<input_output_tool>
{
  protected:

    using super = rewriter_tool<input_output_tool>;

    bool m_tau_summands_only = false;
    bool m_finite_sorts_only = false;
    std::string m_sorts_string;

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("finite", "only instantiate variables whose sorts are finite", 'f');
      desc.add_option("tau", "only instantiate variables in tau summands", 't');
      desc.add_option("sorts",
                       make_optional_argument("NAME", ""),
                       "select sorts that need to be expanded (comma separated list). Examples: Bool; Bool, List(Nat)",
                       's');
    }

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      m_tau_summands_only = 0 < parser.options.count("tau");
      m_finite_sorts_only = 0 < parser.options.count("finite");
      if(parser.options.count("sorts"))
      {
        m_sorts_string = parser.option_argument("sorts");
        remove_whitespace(m_sorts_string);
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
    bool run() override
    {
      stochastic_specification spec;
      load_lps(spec, input_filename());
      std::set<data::sort_expression> sorts;

      // Determine set of sorts to be expanded
      if(!m_sorts_string.empty())
      {
        std::vector<std::string> parts = utilities::split(utilities::remove_whitespace(m_sorts_string), ",");
        for (const std::string& part : parts)
        {
          sorts.insert(data::parse_sort_expression(part, spec.data()));
        }
      }
      else if (m_finite_sorts_only)
      {
        sorts = lps::finite_sorts(spec.data());
      }
      else
      {
        const std::set<data::sort_expression>& sort_set=spec.data().sorts();
        sorts = std::set<data::sort_expression>(sort_set.begin(),sort_set.end());
      }

      mCRL2log(log::verbose) << "expanding summation variables of sorts: " << data::pp(sorts) << std::endl;

      mcrl2::data::rewriter r(spec.data(), m_rewrite_strategy);
      lps::suminst_algorithm<data::rewriter, stochastic_specification>(spec, r, sorts, m_tau_summands_only).run();
      save_lps(spec, output_filename());
      return true;
    }
};

int main(int argc, char** argv)
{
  return suminst_tool().execute(argc, argv);
}
