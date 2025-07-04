/// Author(s): F.P.M. (Frank) Stappers, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./lpsconstelm.cpp

#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/constelm.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::lps;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

using mcrl2::data::tools::rewriter_tool;

class lpsconstelm_tool: public rewriter_tool<input_output_tool >
{
  protected:

    using super = rewriter_tool<input_output_tool>;

    bool m_instantiate_free_variables = false;
    bool m_ignore_conditions = false;
    bool m_remove_trivial_summands = false;
    bool m_remove_singleton_sorts = false;

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);

      m_instantiate_free_variables = parser.has_option("instantiate-free-variables");
      m_ignore_conditions          = parser.has_option("ignore-conditions");
      m_remove_trivial_summands    = parser.has_option("remove-trivial-summands");
      m_remove_singleton_sorts     = parser.has_option("remove-singleton-sorts");
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc
      // boolean flag (default off)
      .add_option("instantiate-free-variables",
                  "allow free variables to be instantiated as a side effect of the algorithm. This functionality is untested!",
                  'f')
      .add_option("ignore-conditions",
                  "ignore conditions by assuming they evaluate to true",
                  'c')
      .add_option("remove-trivial-summands",
                  "remove summands with condition false",
                  't')
      .add_option("remove-singleton-sorts",
                  "remove parameters with single element sorts",
                  's')
      ;
    }

  public:

    lpsconstelm_tool()
      : super(
        "lpsconstelm",
        "Wieger Wesselink; Frank Stappers",
        "removes constant parameters from an LPS",
        make_tool_description(
          "Remove constant process parameters from the LPS in INFILE and write the result to OUTFILE."
        )
      )
    {}

    ///Reads a specification from input_file,
    ///applies instantiation of sums to it and writes the result to output_file.
    bool run() override
    {
      lps::stochastic_specification spec;
      load_lps(spec, input_filename());
      mcrl2::data::rewriter R(spec.data(), rewrite_strategy());
      lps::constelm_algorithm<data::rewriter, stochastic_specification> algorithm(spec, R);

      // preprocess: remove single element sorts
      if (m_remove_singleton_sorts)
      {
        algorithm.remove_singleton_sorts();
      }

      // apply constelm
      algorithm.run(m_instantiate_free_variables, m_ignore_conditions);

      // postprocess: remove trivial summands
      if (m_remove_trivial_summands)
      {
        algorithm.remove_trivial_summands();
      }

      save_lps(spec, output_filename());
      return true;
    }

};

int main(int argc, char** argv)
{
  return lpsconstelm_tool().execute(argc, argv);
}

