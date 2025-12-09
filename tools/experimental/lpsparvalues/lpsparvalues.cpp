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

  public:

    lps_explore_domains_tool()
      : super(
        "lpsexploredomains",
        "Jan Friso Groote",
        "enumerate the values that the parameters of an LPS can take during state space exploration",
        "Find the values that the parameters of an LPS n INFILE can take during state space exploration. "
        "If enumeration fails, try to use --qlimit=N with N an increasing value. "
        "If INFILE is not present, stdin is used. "
      )
    {}

    bool run() override
    {
      stochastic_specification spec;
      load_lps(spec, m_input_filename);
      data::rewriter r(spec.data(), rewrite_strategy());

      lps::lps_explore_domains_algorithm<data::rewriter, stochastic_specification>(spec, r, m_qlimit).run();
      return true;
    }

};

int main(int argc, char** argv)
{
  return lps_explore_domains_tool().execute(argc, argv);
}
