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
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/text_utility.h"
#include "mcrl2/core/regfrmtrans.h"
#include "mcrl2/data/detail/data_implementation.h"
#include "mcrl2/data/detail/data_reconstruct.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/modal_formula/detail/algorithms.h"
#include "mcrl2/pbes/pbes_translate.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/squadt_tool.h"

using namespace mcrl2;
using namespace mcrl2::lps;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::modal;
using namespace mcrl2::data::detail;
using namespace mcrl2::utilities::tools;
                       
class lps2pbes_tool : public squadt_tool<input_output_tool>
{
  typedef squadt_tool<input_output_tool> super;

  protected:
    //t_phase represents the phases at which the program should be able to stop
    typedef enum { PH_NONE, PH_PARSE, PH_TYPE_CHECK, PH_DATA_IMPL, PH_REG_FRM_TRANS } t_phase;
    
    bool pretty;
    bool timed;
    t_phase end_phase;
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
      desc.add_option("end-phase", make_mandatory_argument("PHASE"),
          "stop conversion and output the state formula after phase PHASE: "
          "'pa' (parsing), "
          "'tc' (type checking), "
          "'di' (data implementation), or "
          "'rft' (regular formula translation)"
        , 'p');
      desc.add_option("pretty",
          "return a pretty printed version of the output", 'P');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      pretty    = 0 < parser.options.count("pretty");
      timed     = 0 < parser.options.count("timed");
      end_phase = PH_NONE;
      
      if (parser.options.count("end-phase"))
      {
        std::string phase = parser.option_argument("end-phase");
        if      (phase == "pa")  { end_phase = PH_PARSE; }
        else if (phase == "tc")  { end_phase = PH_TYPE_CHECK; }
        else if (phase == "di")  { end_phase = PH_DATA_IMPL; }
        else if (phase == "rft") { end_phase = PH_REG_FRM_TRANS; }
        else {
          parser.error("option -p has illegal argument '" + phase + "'");
        }
      }
      
      //check for presence of -f
      if (parser.options.count("formula")) {
        formfilename = parser.option_argument("formula");
      }
      else {
        parser.error("option -f is not specified");
      }
    }

    //Pre:  formfilename contains a state formula
    //      input_filename() contains an LPS ("" indicates stdin)
    //      end_phase indicates at which phase conversion stops
    //Ret:  if end_phase == PH_NONE, the PBES generated from the state formula and
    //      the LPS
    //      if end_phase != PH_NONE, the state formula after phase end_phase
    //      NULL, if something went wrong
    ATermAppl create_pbes()
    {
      //open input_filename()
      specification lps_spec;
      lps_spec.load(input_filename());
    
      //parse formula from formfilename
      gsVerboseMsg("parsing formula from '%s'...\n", formfilename.c_str());
      std::ifstream formstream(formfilename.c_str(), std::ifstream::in|std::ifstream::binary);
      if (!formstream.is_open()) {
        gsErrorMsg("cannot open formula file '%s'\n", formfilename.c_str());
        return NULL;
      }
      ATermAppl result = parse_state_frm(formstream);
      formstream.close();
      if (result == NULL) {
        gsErrorMsg("parsing failed\n");
        return NULL;
      }
      if (end_phase == PH_PARSE) {
        return result;
      }
    
      lps_spec.data() = mcrl2::data::remove_all_system_defined(lps_spec.data());
      ATermAppl reconstructed_spec = specification_to_aterm(lps_spec);

      //type check formula
      gsVerboseMsg("type checking...\n");
      result = type_check_state_frm(result, reconstructed_spec);
      if (result == NULL) {
        gsErrorMsg("type checking failed\n");
        return NULL;
      }
      if (end_phase == PH_TYPE_CHECK) {
        return result;
      }
    
      //implement standard data types and type constructors on the result
      gsVerboseMsg("implementing standard data types and type constructors...\n");
      result = implement_data_state_frm(result, reconstructed_spec);
      if (result == NULL) {
        gsErrorMsg("data implementation failed\n");
        return NULL;
      }
      if (end_phase == PH_DATA_IMPL) {
        return result;
      }
    
      //update lps_spec with the newly implemented specification
      lps_spec = specification(reconstructed_spec);
    
      //translate regular formulas in terms of state and action formulas
      gsVerboseMsg("translating regular formulas in terms of state and action formulas...\n");
      result = translate_reg_frms(result);
      if (result == NULL) {
        gsErrorMsg("regular formula translation failed\n");
        return NULL;
      }
      if (end_phase == PH_REG_FRM_TRANS) {
        return result;
      }
    
      //generate PBES from state formula and LPS
      gsVerboseMsg("generating PBES from state formula and LPS...\n");
      pbes<> p = pbes_translate(state_formula(result), lps_spec, timed);
      result = ATermAppl(p);
      if (result == NULL) {
        return NULL;
      }
    
      return result;
    }

    pbes<> create_pbes_new()
    {
      specification spec;
      spec.load(input_filename());
      state_formula formula = modal::detail::mcf2statefrm(core::read_text(formfilename), spec);
      return lps2pbes(spec, formula, timed);
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
      "The concrete syntax of state formulas can be found at <http://www.mcrl2.org/wiki/index.php/mu-calculus_syntax>."
      )
    {}

    bool run()
    {
      //process state formula
      ATermAppl result = create_pbes();

      if (result == 0) {
        return false;
      }

      //store the result
      if (output_filename().empty()) {
        gsVerboseMsg("saving result to standard output...\n");
      } else {
        gsVerboseMsg("saving result to '%s'...\n", output_filename().c_str());
      }
      if ((end_phase == PH_NONE) && (!pretty)) {
        mcrl2::pbes_system::pbes<> pbes_spec(result);
        pbes_spec.save(output_filename());
      } else {
        if (output_filename().empty()) {
          PrintPart_CXX(std::cout, (ATerm) result, (pretty)?ppDefault:ppInternal);
          std::cout << std::endl;
        } else {
          std::ofstream outstream(output_filename().c_str(), std::ofstream::out|std::ofstream::binary);
          if (!outstream.is_open()) {
            throw mcrl2::runtime_error("could not open output file '" + output_filename() + "' for writing");
          }
          PrintPart_CXX(outstream, (ATerm) result, pretty?ppDefault:ppInternal);
          outstream.close();
        }
      }
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

      tipi::datatype::enumeration< t_phase > end_phase_enumeration;

      end_phase_enumeration.
        add(PH_NONE, "none").
        add(PH_PARSE, "parse").
        add(PH_TYPE_CHECK, "type_check").
        add(PH_DATA_IMPL, "data_implementation").
        add(PH_REG_FRM_TRANS, "formula_translation");

      return true;
    }

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities& c) const {
      static bool initialised = initialise_types();

      static_cast< void > (initialised); // harmless, and prevents unused variable warnings

      c.add_input_configuration("lps_in", tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
    }

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration& c) {
      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      // Set default values for configuration
      if (!c.option_exists("use_timed_algorithm")) {
        c.add_option("use_timed_algorithm").set_argument_value< 0 >(false);
      }
      if (!c.option_exists("selected_output_format")) {
        c.add_option("selected_output_format").set_argument_value< 0 >(normal);
      }
      if (!c.option_exists("stop_after_phase")) {
        c.add_option("stop_after_phase").set_argument_value< 0 >(PH_NONE);
      }

      /* Create display */
      tipi::tool_display d;

      // Helper for end phase selection
      mcrl2::utilities::squadt::radio_button_helper < t_phase > phase_selector(d);

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
        append(d.create< vertical_box >().
            append(phase_selector.associate(PH_NONE, "none", true)).
            append(phase_selector.associate(PH_PARSE, "parsing")).
            append(phase_selector.associate(PH_TYPE_CHECK, "type checking")).
            append(phase_selector.associate(PH_DATA_IMPL, "data implementation")).
            append(phase_selector.associate(PH_REG_FRM_TRANS, "formula translation"))).
        append(d.create< label >().set_text(" ")).
        append(okay_button, layout::right);

      // Set default values if the configuration specifies them
      format_selector.set_selection(
          c.get_option_argument< pbes_output_format >("selected_output_format", 0));
      phase_selector.set_selection(c.get_option_argument< t_phase >("stop_after_phase"));

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
      if (c.output_exists("pbes_out")) {
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
      c.get_option("stop_after_phase").set_argument_value< 0 >(phase_selector.get_selection());

      send_clear_display();
    }

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const& c) const {
      bool result = true;

      result &= c.input_exists("lps_in");
      result &= c.input_exists("formula_in");
      result &= c.output_exists("pbes_out");
      result &= c.option_exists("stop_after_phase");
      result &= c.option_exists("use_timed_algorithm");

      return (result);
    }

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration& c) {
      pretty           = c.get_option_argument< pbes_output_format >("selected_output_format") != normal;
      timed            = c.get_option_argument< bool >("use_timed_algorithm");
      end_phase        = c.get_option_argument< t_phase >("stop_after_phase");
      formfilename     = c.get_input("formula_in").location();
      input_filename()       = c.get_input("lps_in").location();
      output_filename()      = c.get_output("pbes_out").location();
      return run();
    }
#endif

};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return lps2pbes_tool().execute(argc, argv);
}
