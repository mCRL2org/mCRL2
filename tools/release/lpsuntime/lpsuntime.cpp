// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsuntime.cpp

#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/untime.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::lps;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::data::tools;

class untime_tool: public rewriter_tool< input_output_tool >
{
  protected:

    using super = rewriter_tool<input_output_tool>;

    bool add_invariants = false;
    bool apply_fourier_motzkin = false;

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);

      if (parser.options.count("invariant")>0)
      {
        add_invariants = true;
      }
      if (parser.options.count("eliminate-sumvars")>0)
      {
        apply_fourier_motzkin = true;
      }
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("invariant",
                 "add invariants for parameters of type Real to the output",
                 'i');
      desc.add_option("eliminate-sumvars",
                 "when possible, eliminate summation variables of type Real "
                 "using the Fourier Motzkin algorithm. This can significantly "
                 "change the structure of the conditions.",
                 'e');
    }

  public:

    untime_tool()
      : super(
        "lpsuntime",
        "Jeroen Keiren",
        "remove time from an LPS",
        "Remove time from the linear process specification (LPS) in INFILE and write the "
        "result to OUTFILE. If INFILE is not present, stdin is used. If OUTFILE is not "
        "present, stdout is used.")
    {}

    bool run() override
    {      
      stochastic_specification spec;
      load_lps(spec, m_input_filename);
      data::rewriter rewr(spec.data(),m_rewrite_strategy);
      untime_algorithm<stochastic_specification>(spec, add_invariants, apply_fourier_motzkin, rewr).run();
      save_lps(spec, m_output_filename);
      return true;
    }

};

int main(int argc, char** argv)
{
  return untime_tool().execute(argc, argv);
}
