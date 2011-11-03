/// Author(s): F.P.M. (Frank) Stappers, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./lpsconstelm.cpp

#include "boost.hpp" // precompiled headers

//mCRL2
#include "mcrl2/lps/tools.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::lps;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class lpsconstelm_tool: public rewriter_tool<input_output_tool >
{
  protected:

    typedef rewriter_tool<input_output_tool> super;

    bool m_instantiate_free_variables;
    bool m_ignore_conditions;
    bool m_remove_trivial_summands;
    bool m_remove_singleton_sorts;

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      m_instantiate_free_variables = parser.options.count("instantiate-free-variables") > 0;
      m_ignore_conditions          = parser.options.count("ignore-conditions") > 0;
      m_remove_trivial_summands    = parser.options.count("remove-trivial-summands") > 0;
      m_remove_singleton_sorts     = parser.options.count("remove-singleton-sorts") > 0;
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc
      // boolean flag (default off)
      .add_option("instantiate-free-variables",
                  "allow free variables to be instantiated as a side effect of the algorithm"
                  "\nNOTE: this functionality is untested!",
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
    bool run()
    {
      lpsconstelm(input_filename(),
                  output_filename(),
                  rewrite_strategy(),
                  m_instantiate_free_variables,
                  m_ignore_conditions,
                  m_remove_trivial_summands,
                  m_remove_singleton_sorts
                );
      return true;
    }

};

class lpsconstelm_gui_tool: public mcrl2_gui_tool<lpsconstelm_tool>
{
  public:
    lpsconstelm_gui_tool()
    {
      m_gui_options["ignore-conditions"] = create_checkbox_widget();
      m_gui_options["instantiate-free-variables"] = create_checkbox_widget();

      add_rewriter_widget();

      m_gui_options["remove-singleton-sorts"] = create_checkbox_widget();
      m_gui_options["remove-trivial-summands"] = create_checkbox_widget();
    }
};


int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return lpsconstelm_gui_tool().execute(argc, argv);
}

