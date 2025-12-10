// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsexploredomains.cpp
/// \brief The explore_domains tool which enumerates the possible parameter values of a linear process

#include "mcrl2/lps/io.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/data/rewriter.h"
#include "lpsparvalues.h"

using namespace mcrl2;
using namespace mcrl2::lps;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using mcrl2::data::tools::rewriter_tool;


class lps_explore_domains_tool: public rewriter_tool<input_tool>
{
  protected:

    using super = rewriter_tool<input_tool>;

    std::size_t m_maximal_number_of_rounds = std::numeric_limits<std::size_t>::max();

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("max", make_mandatory_argument("NUM"),
                      "limit the number of iterations. This may cause certain parameters in some domains to not be found. "
                      "The estimated state space size can also be too low. ",
                      'l');
    }

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);

      if (0 < parser.options.count("max"))
      {
        m_maximal_number_of_rounds = parser.option_argument_as<std::size_t>("max");
      }
    }


  public:

    lps_explore_domains_tool()
      : super(
        "lpsexploredomains",
        "Jan Friso Groote",
        "enumerate the values that the parameters of an LPS can take during state space exploration",
        "Find the values that the parameters of an LPS n INFILE can take during state space exploration. "
        "If enumeration fails, try to use --qlimit=N with N an increasing value. "
        "It can also help to apply lpssuminst --finite to a linear process first. "
        "If INFILE is not present, stdin is used. "
      )
    {}

    bool run() override
    {
      stochastic_specification spec;
      load_lps(spec, m_input_filename);
      data::rewriter r(spec.data(), rewrite_strategy());

      lps::lps_explore_domains_algorithm<data::rewriter, stochastic_specification>(spec, r, m_qlimit, m_maximal_number_of_rounds).run();
      return true;
    }

};

int main(int argc, char** argv)
{
  return lps_explore_domains_tool().execute(argc, argv);
}
