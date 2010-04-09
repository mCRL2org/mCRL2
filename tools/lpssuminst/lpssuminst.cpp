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

#include "mcrl2/lps/suminst.h"

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/utilities/squadt_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::core;

class suminst_tool: public squadt_tool< rewriter_tool<input_output_tool> >
{
  protected:

    typedef squadt_tool< rewriter_tool<input_output_tool> > super;

    bool m_tau_summands_only;
    bool m_finite_sorts_only;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("finite", "only instantiate variables whose sorts are finite", 'f');
      desc.add_option("tau", "only instantiate variables in tau summands", 't');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      m_tau_summands_only = 0 < parser.options.count("tau");
      m_finite_sorts_only = 0 < parser.options.count("finite");
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

       mcrl2::data::rewriter r = create_rewriter(lps_specification.data());
       lps::suminst_algorithm<data::rewriter>(lps_specification, r, m_finite_sorts_only, m_tau_summands_only).run();
       lps_specification.save(m_output_filename);

       return true;
    }

//Squadt connectivity
#ifdef ENABLE_SQUADT_CONNECTIVITY
  protected:

# define option_tau_only         "tau_only"
# define option_finite_only      "finite_only"

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& capabilities) const
    {
      // The tool has only one main input combination
      capabilities.add_input_configuration("main-input",
                 tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration& configuration)
    {
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(configuration);

      /* Set defaults where the supplied configuration does not have values */
      if (!configuration.option_exists(option_tau_only)) {
        configuration.add_option(option_tau_only).
           set_argument_value< 0 >(true, false);
      }
      if (!configuration.option_exists(option_finite_only)) {
        configuration.add_option(option_finite_only).
           set_argument_value< 0 >(true, false);
      }
      if (!configuration.output_exists("main-output")) {
        configuration.add_output("main-output",
           tipi::mime_type("lps", tipi::mime_type::application), configuration.get_output_name(".lps"));
      }

      /* Create display */
      tipi::tool_display d;

      layout::vertical_box& m = d.create< vertical_box >();

      add_rewrite_option(d, m);

      /* Prepare user interaction */
      checkbox& tau_only = d.create< checkbox >().set_status(configuration.get_option_argument< bool >(option_tau_only));
      m.append(d.create< label >().set_text(" ")).
        append(tau_only.set_label("Only instantiate tau summands"), layout::left);

      checkbox& finite_only = d.create< checkbox >().set_status(configuration.get_option_argument< bool >(option_finite_only));
      m.append(d.create< label >().set_text(" ")).
        append(finite_only.set_label("Only instantiate variables of finite sorts"), layout::left);

      button& okay_button = d.create< button >().set_label("OK");

      m.append(d.create< label >().set_text(" ")).
        append(okay_button, layout::right);

      send_display_layout(d.manager(m));

      okay_button.await_change();

      // let squadt_tool update configuration for rewriter and input/output files
      update_configuration(configuration);

      /* Update configuration */
      configuration.get_option(option_tau_only).
         set_argument_value< 0 >(tau_only.get_status());
      configuration.get_option(option_finite_only).
         set_argument_value< 0 >(finite_only.get_status());
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& configuration) const
    {
      return configuration.input_exists("main-input") &&
             configuration.output_exists("main-output") &&
             configuration.option_exists("rewrite-strategy");
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& configuration)
    {
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(configuration);
   
      m_tau_summands_only = configuration.option_exists(option_tau_only);
      m_finite_sorts_only = configuration.option_exists(option_finite_only);

      /* Create display */
      tipi::tool_display d;

      send_display_layout(d.manager(d.create< vertical_box >().
                    append(d.create< label >().set_text("Instantiation of summation variables in progress"), layout::left)));

      //Perform instantiation
      bool result = run();

      send_display_layout(d.manager(d.create< vertical_box >().
                    append(d.create< label >().set_text(std::string("Instantiation of summation variables ") + ((result) ? "succeeded" : "failed")), layout::left)));

      return result;
    }
#endif //ENABLE_SQUADT_CONNECTIVITY
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return suminst_tool().execute(argc, argv);
}
