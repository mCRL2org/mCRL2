// Author(s): Wieger Wesselink, Simon Janssen, Tim Willemse
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesconstelm.cpp

#include "boost.hpp" // precompiled headers

// #define MCRL2_PBES_CONSTELM_DEBUG
// #define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include <iostream>
#include <string>
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/pbes_rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/constelm.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace utilities::tools;

class pbes_constelm_tool: public squadt_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool> > >
{
  protected:
    typedef squadt_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool> > > super;

    bool m_compute_conditions;
    bool m_remove_redundant_equations;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      m_compute_conditions = parser.options.count("compute-conditions") > 0;
      m_remove_redundant_equations = parser.options.count("remove-equations") > 0;
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("compute-conditions", "compute propagation conditions", 'c');
      desc.add_option("remove-equations", "remove redundant equations", 'e');
    }

  public:
    pbes_constelm_tool()
      : super(
          "pbesconstelm",
          "Wieger Wesselink; Simon Janssen and Tim Willemse",
          "remove constant parameters from a PBES",
          "Reads a file containing a PBES, and applies constant parameter elimination to it. If OUTFILE "
          "is not present, standard output is used. If INFILE is not present, standard input is used."
        )
    {}

    bool run()
    {
      if (mcrl2::core::gsVerbose)
      {
        std::cerr << "pbesconstelm parameters:" << std::endl;
        std::cerr << "  input file:         " << m_input_filename << std::endl;
        std::cerr << "  output file:        " << m_output_filename << std::endl;
        std::cerr << "  compute conditions: " << std::boolalpha << m_compute_conditions << std::endl;
        std::cerr << "  remove redundant equations: " << std::boolalpha << m_remove_redundant_equations << std::endl;
      }

      // load the pbes
      pbes<> p;
      p.load(m_input_filename);

      // data rewriter
      data::rewriter datar = create_rewriter(p.data());

      // pbes rewriter
      switch (rewriter_type())
      {
        case simplify:
        {
          typedef simplifying_rewriter<pbes_system::pbes_expression, data::rewriter> my_pbes_rewriter;
          my_pbes_rewriter pbesr(datar);
          pbes_constelm_algorithm<pbes_system::pbes_expression, data::rewriter, my_pbes_rewriter> algorithm(datar, pbesr);
          data::number_postfix_generator name_generator("UNIQUE_PREFIX");
          algorithm.run(p, m_compute_conditions, m_remove_redundant_equations);
          break;
        }
        case quantifier_all:
        case quantifier_finite:
        {
          typedef pbes_system::enumerate_quantifiers_rewriter<pbes_system::pbes_expression, data::rewriter_with_variables, data::data_enumerator<> > my_pbes_rewriter;
          bool enumerate_infinite_sorts = (rewriter_type() == quantifier_all);
          data::number_postfix_generator name_generator("UNIQUE_PREFIX");
          data::data_enumerator<> datae(p.data(), datar, name_generator);
          data::rewriter_with_variables datarv(datar);
          my_pbes_rewriter pbesr(datarv, datae, enumerate_infinite_sorts);
          pbes_constelm_algorithm<pbes_system::pbes_expression, data::rewriter, my_pbes_rewriter> algorithm(datar, pbesr);
          algorithm.run(p, m_compute_conditions, m_remove_redundant_equations);
          break;
        }
        default:
        { }
      }

      // save the result
      p.save(m_output_filename);

      return true;
    }

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY

#define option_compute_conditions "compute_conditions"
#define option_remove_redundant_equations "remove_redundant_equations"

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& c) const {
      c.add_input_configuration("main-input",
                 tipi::mime_type("pbes", tipi::mime_type::application),
                                         tipi::tool::category::transformation);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration& c) {

      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      if (!c.output_exists("main-output")) {
        c.add_output("main-output",
                 tipi::mime_type("pbes", tipi::mime_type::application),
                                                 c.get_output_name(".pbes"));
      }

      // Set defaults where the supplied configuration does not have values */
      if (!c.option_exists(option_compute_conditions))
      {
        c.add_option(option_compute_conditions).
          set_argument_value< 0 >(true, false);
      }
      if (!c.option_exists(option_remove_redundant_equations))
      {
        c.add_option(option_remove_redundant_equations).
          set_argument_value< 0 >(true, false);
      }

      // Create display
      tipi::tool_display d;

      layout::vertical_box& m = d.create< vertical_box >();

      add_rewrite_option(d, m);

      // Prepare user interaction
      checkbox& compute_conditions = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_compute_conditions));
      m.append(d.create< label >().set_text(" ")).
        append(compute_conditions.set_label("Compute conditions"), layout::left);
      checkbox& remove_redundant_equations = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_remove_redundant_equations));
      m.append(d.create< label >().set_text(" ")).
        append(remove_redundant_equations.set_label("Remove redundant equation"), layout::left);

      button& okay_button = d.create< button >().set_label("OK");

      m.append(d.create< label >().set_text(" ")).
        append(okay_button, layout::right);

      send_display_layout(d.manager(m));

      okay_button.await_change();

      // Let squadt_tool update configuration for rewriter and input/output files
      update_configuration(c);

      // Update configuration
      c.get_option(option_compute_conditions).
        set_argument_value< 0 >(compute_conditions.get_status());
      c.get_option(option_remove_redundant_equations).
        set_argument_value< 0 >(remove_redundant_equations.get_status());
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& c) const {
      return c.input_exists("main-input") && c.output_exists("main-output");
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& c) {
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      m_compute_conditions = c.option_exists(option_compute_conditions);
      m_remove_redundant_equations = c.option_exists(option_remove_redundant_equations);

      // Create display
      tipi::tool_display d;

      send_display_layout(d.manager(d.create< vertical_box >().
        append(d.create< label >().set_text("Constant elimination in progress"), layout::left)));

      // Run
      bool result = run() == 0;

      send_display_layout(d.manager(d.create<vertical_box>().
                                    append(d.create< label >().set_text(std::string("Constant elimination ") + ((result) ? "succeeded" : "failed")), layout::left)));

      return result;

      return run();
    }
#endif


};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  pbes_constelm_tool tool;
  return tool.execute(argc, argv);
}
