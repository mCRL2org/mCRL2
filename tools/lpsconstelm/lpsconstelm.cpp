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

#define NAME "lpsconstelm"
#define AUTHOR "Frank Stappers"

//mCRL2
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/constelm.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/atermpp/aterm_init.h"


using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

class lpsconstelm_tool: public squadt_tool< rewriter_tool<input_output_tool> >
{
  protected:

    typedef squadt_tool< rewriter_tool<input_output_tool> > super;
    
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
          "NOTE: this functionality is untested!",
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
          "Wieger Wesselink",
          "removes constant parameters from an LPS",
          "Removes constant process parameters from the LPS in INFILE and write the result "
          "to OUTFILE. If OUTFILE is not present, standard output is used. If INFILE is not"
          "present, standard input is used."
        )
    {}

    ///Reads a specification from input_file,
    ///applies instantiation of sums to it and writes the result to output_file.
    bool run()
    {
      lps::specification spec;
      spec.load(m_input_filename);
      mcrl2::data::rewriter R = create_rewriter(spec.data() );

      lps::constelm_algorithm<data::rewriter> algorithm(spec, R, mcrl2::core::gsVerbose);

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
      
      spec.save(m_output_filename);
      return true;
    }

// Squadt protocol interface and utility pseudo-library
#ifdef ENABLE_SQUADT_CONNECTIVITY
    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& c) const {
      c.add_input_configuration("main-input",
                 tipi::mime_type("lps", tipi::mime_type::application),
                                         tipi::tool::category::transformation);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration& c) {
      if (!c.output_exists("main-output")) {
        c.add_output("main-output",
                 tipi::mime_type("lps", tipi::mime_type::application),
                                                 c.get_output_name(".lps"));
      }
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& c) const {
      return c.input_exists("main-input") && c.output_exists("main-output");
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& c) {
      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      return run();
    }
#endif
};



int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return lpsconstelm_tool().execute(argc, argv);
}

