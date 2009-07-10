// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2pbes.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <fstream>
#include "mcrl2/core/text_utility.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/squadt_tool.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;

using mcrl2::core::gsVerboseMsg;

class lps2pbes_tool : public squadt_tool<input_output_tool>
{
  typedef squadt_tool<input_output_tool> super;

  protected:
    bool pretty;
    bool timed;
    std::string formfilename;

    std::string synopsis() const
    {
      return "[OPTION]... --formula=FILE [INFILE [OUTFILE]]\n";
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("formula", make_mandatory_argument("FILE"),
          "use the state formula from FILE", 'f');
      desc.add_option("timed",
          "use the timed version of the algorithm, even for untimed LPS's", 't');
      desc.add_option("pretty",
          "return a pretty printed version of the output", 'P');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      pretty    = 0 < parser.options.count("pretty");
      timed     = 0 < parser.options.count("timed");

      //check for presence of -f
      if (parser.options.count("formula")) {
        formfilename = parser.option_argument("formula");
      }
      else {
        parser.error("option -f is not specified");
      }
    }

  public:
    lps2pbes_tool() : super(
      "lps2pbes",
      "Aad Mathijssen and Wieger Wesselink",
      "generate a PBES from an LPS and a state formula",
      "Convert the state formula in FILE and the LPS in INFILE to a parameterised "
      "boolean equation system (PBES) and save it to OUTFILE. If OUTFILE is not "
      "present, stdout is used. If INFILE is not present, stdin is used.\n"
      "\n"
      "The concrete syntax of state formulas can be found at <http://www.mcrl2.org/mcrl2/wiki/index.php/Language_reference/mu-calculus_syntax>."
      )
    {}

    bool run()
    {
      //load LPS
      if (input_filename().empty()) {
        gsVerboseMsg("reading LPS from stdin...\n");
      } else {
        gsVerboseMsg("reading LPS from file '%s'...\n", input_filename().c_str());
      }
      lps::specification spec;
      spec.load(input_filename());
      //load formula file
      gsVerboseMsg("reading input from file '%s'...\n", formfilename.c_str());
      std::ifstream instream(formfilename.c_str(), std::ifstream::in|std::ifstream::binary);
      if (!instream.is_open()) {
        throw mcrl2::runtime_error("cannot open state formula file: " + formfilename);
      }
      state_formulas::state_formula formula = state_formulas::detail::mcf2statefrm(instream, spec);
      instream.close();
      //convert formula and LPS to a PBES
      gsVerboseMsg("converting state formula and LPS to a PBES...\n");
      pbes_system::pbes<> result = pbes_system::lps2pbes(spec, formula, timed);
      //save the result
      if (output_filename().empty()) {
        gsVerboseMsg("writing PBES to stdout...\n");
      } else {
        gsVerboseMsg("writing PBES to file '%s'...\n", output_filename().c_str());
      }
      result.save(output_filename());
      return true;
    }

// Squadt protocol interface and utility pseudo-library
#ifdef ENABLE_SQUADT_CONNECTIVITY
    enum pbes_output_format {
      normal,
      readable
    };

    static bool initialise_types() {
      tipi::datatype::enumeration< pbes_output_format > output_format_enumeration;

      output_format_enumeration.
        add(readable, "readable").
        add(normal, "normal");

      return true;
    }

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& c) const {
      static bool initialised = initialise_types();

      static_cast< void > (initialised); // harmless, and prevents unused variable warnings

      c.add_input_configuration("main-input", tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration& c) {
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      // Set default values for configuration
      if (!c.option_exists("use_timed_algorithm")) {
        c.add_option("use_timed_algorithm").set_argument_value< 0 >(false);
      }
      if (!c.option_exists("selected_output_format")) {
        c.add_option("selected_output_format").set_argument_value< 0 >(normal);
      }

      /* Create display */
      tipi::tool_display d;

      // Helper for output format selection
      mcrl2::utilities::squadt::radio_button_helper < pbes_output_format > format_selector(d);

      layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0,5,0,5));

      /* Create and add the top layout manager */
      m.append(d.create< label >().set_text("Phase after which to stop: ")).
        append(d.create< label >().set_text("Output format : ")).
        append(d.create< horizontal_box >().
            append(format_selector.associate(normal, "normal", true)).
            append(format_selector.associate(readable, "readable")));

      text_field& formula_field    = d.create< text_field >();
      checkbox&   timed_conversion = d.create< checkbox >().set_status(c.get_option_argument< bool >("use_timed_algorithm"));
      button&     okay_button      = d.create< button >().set_label("OK");

      m.append(d.create< horizontal_box >().
            append(d.create< label >().set_text("Formula file name : ")).
            append(formula_field)).
        append(timed_conversion.set_label("timed conversion")).
        append(d.create< label >().set_text(" ")).
        append(okay_button, layout::right);

      // Set default values if the configuration specifies them
      format_selector.set_selection(
          c.get_option_argument< pbes_output_format >("selected_output_format", 0));

      if (c.input_exists("formula_in")) {
        formula_field.set_text(c.get_input("formula_in").location());
      }

      send_display_layout(d.manager(m));

      /* Wait until the ok button was pressed */
      okay_button.await_change();

      if (c.output_exists("formula_in")) {
        c.get_input("formula_in").location(formula_field.get_text());
      }
      else {
        c.add_input("formula_in", tipi::mime_type("mf", tipi::mime_type::text), formula_field.get_text());
      }

      /* Add output file to the configuration */
      if (c.output_exists("main_output")) {
        tipi::configuration::object& output_file = c.get_output("pbes_out");

        output_file.location(c.get_output_name(".pbes"));
      }
      else {
        if (format_selector.get_selection() == normal) {
          c.add_output("pbes_out", tipi::mime_type("pbes", tipi::mime_type::application), c.get_output_name(".pbes"));
        }
        else {
          c.add_output("pbes_out", tipi::mime_type("pbes", tipi::mime_type::text), c.get_output_name(".pbes"));
        }
      }

      c.get_option("use_timed_algorithm").set_argument_value< 0, tipi::datatype::boolean >(timed_conversion.get_status());
      c.get_option("selected_output_format").set_argument_value< 0 >(format_selector.get_selection());

      send_clear_display();

      // Let squadt_tool update configuration for rewriter and add output file configuration
      update_configuration(c);
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& c) const {
      return c.input_exists("main-input") &&
             c.input_exists("formula_in") &&
             c.output_exists("main-output") &&
             c.option_exists("use_timed_algorithm");
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& c) {

      // Let squadt_tool update configuration for rewriter and add output file configuration
      synchronise_with_configuration(c);

      pretty           = c.get_option_argument< pbes_output_format >("selected_output_format") != normal;
      timed            = c.get_option_argument< bool >("use_timed_algorithm");
      formfilename     = c.get_input("formula_in").location();
      return run();
    }
#endif

};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return lps2pbes_tool().execute(argc, argv);
}
