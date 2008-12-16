// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsconvert.cpp

#define NAME "ltsconvert"
#define AUTHOR "Muck van Weerdenburg"

#include <string>
#include "aterm2.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"

using namespace mcrl2::lts;
using namespace mcrl2::utilities;
using namespace mcrl2::core;

static const std::set<lts_equivalence> &initialise_allowed_eqs()
{
  static std::set<lts_equivalence> s;
  s.insert(lts_eq_bisim);
  s.insert(lts_eq_branching_bisim);
  s.insert(lts_eq_sim);
  s.insert(lts_eq_trace);
  s.insert(lts_eq_weak_trace);
  return s;
}
static const std::set<lts_equivalence> &allowed_eqs()
{
  static const std::set<lts_equivalence> &s = initialise_allowed_eqs();
  return s;
}

static inline std::string get_base(std::string const& s) {
  return s.substr(0, s.find_last_of('.'));
}

static ATermAppl get_lps(std::string const& filename)
{
  if (!filename.empty()) {
    FILE* file = fopen(filename.c_str(),"rb");

    if ( file ) {
      ATerm t = ATreadFromFile(file);
      fclose(file);

      if ( (t == NULL) || (ATgetType(t) != AT_APPL) || !(mcrl2::core::detail::gsIsLinProcSpec((ATermAppl) t) || !strcmp(ATgetName(ATgetAFun((ATermAppl) t)),"spec2gen")) )
      {
        gsErrorMsg("invalid LPS-file '%s'\n",filename.c_str());
      }
      else {
        return (ATermAppl) t;
      }
    }
    else {
      gsErrorMsg("unable to open LPS-file '%s'\n",filename.c_str());
    }
  }

  return 0;
}

class t_tool_options {
  private:
    std::string     infilename;
    std::string     outfilename;

  public:
    std::string     lpsfile;
    lts_type        intype;
    lts_type        outtype;
    lts_equivalence equivalence;
    lts_eq_options  eq_opts;
    bool            print_dot_state;
    bool            determinise;
    bool            check_reach;

    inline t_tool_options() : intype(lts_none), outtype(lts_none), equivalence(lts_eq_none),
                       print_dot_state(true), determinise(false), check_reach(true) {

      eq_opts.reduce.add_class_to_state = false;
    }

    inline std::string source_string() const {
      return (infilename.empty()) ? std::string("standard input") :
                                    std::string("'" + infilename + "'");
    }

    inline std::string target_string() const {
      return (outfilename.empty()) ? std::string("standard output") :
                                     std::string("'" + outfilename + "'");
    }

    inline lts_extra get_extra(lts_type type, std::string const &base_name = "") const {
      if ( type == lts_dot )
      {
        lts_dot_options opts;
        opts.name = new std::string(base_name); // XXX Ugh!
        opts.print_states = print_dot_state;
        return lts_extra(opts);
      } else {
        if ( !lpsfile.empty() )
        {
          ATermAppl spec = get_lps(lpsfile);

          if ( spec != NULL )
          {
            if ( mcrl2::core::detail::gsIsLinProcSpec(spec) )
            {
              return lts_extra(new mcrl2::lps::specification(spec)); // XXX Ugh!
            } else {
              return lts_extra((ATerm) spec);
            }
          }
        }
        return lts_extra();
      }
    }

    void read_lts(lts& l) const {
      gsVerboseMsg("reading LTS from %s...\n", source_string().c_str());

      lts_extra extra = get_extra(intype);

      bool success = false;
      if (infilename.empty()) {
        success = l.read_from(std::cin,intype,extra);
      }
      else {
        success = l.read_from(infilename,intype,extra);
      }
      if (!success) {
        throw mcrl2::runtime_error("cannot read LTS from " + source_string() +
                                               "\nretry with -v/--verbose for more information");
      }

      if ( check_reach ) {
        gsVerboseMsg("checking reachability of input LTS...\n");

        if ( !l.reachability_check(true) ) {
          gsWarningMsg("not all states of the input LTS are reachable from the initial state; removed unreachable states to ensure correct behaviour in LTS tools (including this one)!\n");
        }
      }
    }

    void set_source(std::string const& filename) {
      infilename = filename;
    }

    void set_target(std::string const& filename) {
      outfilename = filename;

      if ( outtype == lts_none ) {
        gsVerboseMsg("trying to detect output format by extension...\n");

        outtype = lts::guess_format(outfilename);

        if ( outtype == lts_none ) {
          if ( !lpsfile.empty() ) {
            gsWarningMsg("no output format set; using fsm because --lps was used\n");
            outtype = lts_fsm;
          } else {
            gsWarningMsg("no output format set or detected; using default (mcrl2)\n");
            outtype = lts_mcrl2;
          }
        }
      }
    }

    void write_lts(lts& l) const {
      bool success = false;

      gsVerboseMsg("writing LTS to %s...\n", target_string().c_str());

      if (outfilename.empty()) {
        success = l.write_to(std::cout,outtype,get_extra(outtype, "stdout"));
      }
      else {
        success = l.write_to(outfilename, outtype, get_extra(outtype, get_base(outfilename)));
      }

      if (!success) {
        throw mcrl2::runtime_error("cannot write LTS to " + target_string() +
                                               "\nretry with -v/--verbose for more information");
      }
    }
};

using namespace std;

void process(t_tool_options const& tool_options) {
  lts l;

  tool_options.read_lts(l);

  if ( tool_options.equivalence != lts_eq_none )
  {
    gsVerboseMsg("reducing LTS (modulo %s)...\n", lts::name_of_equivalence(tool_options.equivalence).c_str());
    gsVerboseMsg("before reduction: %lu states and %lu transitions \n",l.num_states(),l.num_transitions());
    l.reduce(tool_options.equivalence, tool_options.eq_opts);
    gsVerboseMsg("after reduction: %lu states and %lu transitions\n",l.num_states(),l.num_transitions());
  }

  if ( tool_options.determinise )
  {
    gsVerboseMsg("determinising LTS...\n");
    gsVerboseMsg("before determinisation: %lu states and %lu transitions\n",l.num_states(),l.num_transitions());
    l.determinise();
    gsVerboseMsg("after determinisation: %lu states and %lu transitions\n",l.num_states(),l.num_transitions());
  }

  tool_options.write_lts(l);
}

bool parse_command_line(int ac, char** av, t_tool_options& tool_options) {
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n",
    "Convert the labelled transition system (LTS) from INFILE to OUTFILE in the\n"
    "requested format after applying the selected minimisation method (default is\n"
    "none). If OUTFILE is not supplied, stdout is used. If INFILE is not supplied,\n"
    "stdin is used.\n"
    "\n"
    "The output format is determined by the extension of OUTFILE, whereas the input\n"
    "format is determined by the content of INFILE. Options --in and --out can be\n"
    "used to force the input and output formats. The supported formats are:\n"
    + lts::supported_lts_formats_text(lts_mcrl2)
  );

  clinterface.add_option("no-reach",
      "do not perform a reachability check on the input LTS");
  clinterface.add_option("no-state",
      "leave out state information when saving in dot format", 'n');
  clinterface.add_option("determinise", "determinise LTS", 'D');
  clinterface.add_option("lps", make_mandatory_argument("FILE"),
      "use FILE as the LPS from which the input LTS was generated; this is "
      "might be needed to store the correct parameter names of states when saving "
      "in fsm format and to convert non-mCRL2 LTSs to a mCRL2 LTS", 'l');
  clinterface.add_option("in", make_mandatory_argument("FORMAT"),
      "use FORMAT as the input format", 'i').
    add_option("out", make_mandatory_argument("FORMAT"),
      "use FORMAT as the output format", 'o');
  clinterface.add_option("equivalence", make_mandatory_argument("NAME"),
      "generate an equivalent LTS, preserving equivalence NAME:\n"
      +lts::supported_lts_equivalences_text(allowed_eqs())
      , 'e');
  clinterface.add_option("add",
      "do not minimise but save a copy of the original LTS extended with a "
      "state parameter indicating the bisimulation class a state belongs to "
      "(only for mCRL2)", 'a');
  clinterface.add_option("tau", make_mandatory_argument("ACTNAMES"),
      "consider actions with a name in the comma separated list ACTNAMES to "
      "be internal (tau) actions in addition to those defined as such by "
      "the input");

  command_line_parser parser(clinterface, ac, av);

  if (parser.continue_execution()) {
    if (parser.options.count("lps")) {
      if (1 < parser.options.count("lps")) {
        std::cerr << "warning: multiple LPS files specified; can only use one\n";
      }

      tool_options.lpsfile = parser.option_argument("lps");
    }
    if (parser.options.count("in")) {
      if (1 < parser.options.count("in")) {
        std::cerr << "warning: multiple input formats specified; can only use one\n";
      }

      tool_options.intype = lts::parse_format(parser.option_argument("in"));

      if (tool_options.intype == lts_none) {
        std::cerr << "warning: format '" << parser.option_argument("in") <<
                     "' is not recognised; option ignored" << std::endl;
      }
    }
    if (parser.options.count("out")) {
      if (1 < parser.options.count("out")) {
        std::cerr << "warning: multiple output formats specified; can only use one\n";
      }

      tool_options.outtype = lts::parse_format(parser.option_argument("out"));

      if (tool_options.outtype == lts_none) {
        std::cerr << "warning: format '" << parser.option_argument("out") <<
                     "' is not recognised; option ignored" << std::endl;
      }
    }

    if (parser.options.count("equivalence")) {

      tool_options.equivalence = lts::parse_equivalence(
          parser.option_argument("equivalence"));

      if ( allowed_eqs().count(tool_options.equivalence) == 0 )
      {
        parser.error("option -e/--equivalence has illegal argument '" + 
            parser.option_argument("equivalence") + "'");
      }
    }

    if (parser.options.count("tau")) {
      lts_reduce_add_tau_actions(tool_options.eq_opts, parser.option_argument("tau"));
    }

    tool_options.determinise                       = 0 < parser.options.count("determinise");
    tool_options.check_reach                       = parser.options.count("no-reach") == 0;
    tool_options.print_dot_state                   = parser.options.count("no-state") == 0;
    tool_options.eq_opts.reduce.add_class_to_state = 0 < parser.options.count("add");

    if ( tool_options.determinise && (tool_options.equivalence != lts_eq_none) ) {
      parser.error("cannot use option -D/--determinise together with LTS reduction options\n");
    }

    if (2 < parser.arguments.size()) {
      parser.error("too many file arguments");
    }
    else {
      if (0 < parser.arguments.size()) {
        tool_options.set_source(parser.arguments[0]);
      }
      else {
        if ( tool_options.intype == lts_none ) {
          gsWarningMsg("cannot detect format from stdin and no input format specified; assuming aut format\n");
          tool_options.intype = lts_aut;
        }
      }
      if (1 < parser.arguments.size()) {
        tool_options.set_target(parser.arguments[1]);
      }
      else {
        if ( tool_options.outtype == lts_none ) {
          if ( !tool_options.lpsfile.empty() ) {
            gsWarningMsg("no output format set; using fsm because --lps was used\n");
            tool_options.outtype = lts_fsm;
          } else {
            gsWarningMsg("no output format set or detected; using default (aut)\n");
            tool_options.outtype = lts_aut;
          }
        }
      }
    }
  }

  return parser.continue_execution();
}

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/mcrl2_squadt_interface.h>

static const char* lts_file_for_input  = "lts_in";  ///< file containing an LTS that can be imported using the LTS library
static const char* lts_file_for_output = "lts_out"; ///< file used to write the output to
static const char* lps_file_auxiliary  = "lps_aux"; ///< LPS file needed for some conversion operations

static const char* option_selected_transformation            = "selected_transformation";               ///< the selected transformation method
static const char* option_selected_output_format             = "selected_output_format";                ///< the selected output format
static const char* option_no_reachability_check              = "no_reachability_check";                 ///< do not check reachability of input LTS
static const char* option_no_state_information               = "no_state_information";                  ///< dot format output specific option to not save state information
static const char* option_tau_actions                        = "tau_actions";                           ///< the actions that should be recognised as tau
static const char* option_add_bisimulation_equivalence_class = "add_bisimulation_equivalence_class";    ///< adds bisimulation equivalence class to the state information of a state instead of actually reducing modulo bisimulation [mCRL2 specific]

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

  public:

    /** \brief constructor */
    squadt_interactor() {
    }

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration&);
};

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("aut", tipi::mime_type::text), tipi::tool::category::conversion);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("mcrl2-lts", tipi::mime_type::application), tipi::tool::category::conversion);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("svc+mcrl", tipi::mime_type::application), tipi::tool::category::conversion);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("svc", tipi::mime_type::application), tipi::tool::category::conversion);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("fsm", tipi::mime_type::text), tipi::tool::category::conversion);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("dot", tipi::mime_type::text), tipi::tool::category::conversion);
#ifdef USE_BCG
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("bcg", tipi::mime_type::application), tipi::tool::category::conversion);
#endif
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  /* Create display */
  tipi::tool_display d;

  layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0, 5, 0, 5));

  /* Helper for format selection */
  mcrl2::utilities::squadt::radio_button_helper < mcrl2::lts::lts_type > format_selector(d);

  m.append(d.create< horizontal_box >().
                append(d.create< label >().set_text("Output format : ")).
                append(format_selector.associate(mcrl2::lts::lts_aut, "Aldebaran",true)).
                append(format_selector.associate(mcrl2::lts::lts_mcrl, "SVC/mCRL")).
                append(format_selector.associate(mcrl2::lts::lts_mcrl2, "mCRL2")).
                append(format_selector.associate(mcrl2::lts::lts_svc, "SVC")).
#ifdef USE_BCG
                append(format_selector.associate(mcrl2::lts::lts_bcg, "BCG")).
#endif
                append(format_selector.associate(mcrl2::lts::lts_fsm, "FSM")).
                append(format_selector.associate(mcrl2::lts::lts_dot, "dot")),
           margins(0,5,0,5));

  text_field& lps_file_field        = d.create< text_field >();
  checkbox&   check_reachability    = d.create< checkbox >();
  checkbox&   add_state_information = d.create< checkbox >();

  m.append(d.create< label >()).
    append(d.create< horizontal_box >().
                append(d.create< label >().set_text("LPS file name : ")).
                append(lps_file_field)).
    append(check_reachability.set_label("Perform reachability check").set_status(true)).
    append(add_state_information.set_label("Add state information").set_status(true));

//  mcrl2::utilities::squadt::change_visibility_on_toggle(format_selector.get_button(dot), top.get(), add_state_information);
//  format_selector.get_button(dot).on_change(boost::bind(::mcrl2::utilities::squadt::change_visibility_on_toggle, format_selector.get_button(dot), top.get(), add_state_information));

  /* Helper for transformation selection */
  mcrl2::utilities::squadt::radio_button_helper < lts_equivalence > transformation_selector(d);

  m.append(d.create< label >().set_text("LTS transformation:")).
    append(transformation_selector.associate(lts_eq_none, "none", true)).
    append(transformation_selector.associate(lts_eq_bisim, "reduction modulo strong bisimulation equivalence")).
    append(transformation_selector.associate(lts_eq_branching_bisim, "reduction modulo branching bisimulation equivalence")).
    append(transformation_selector.associate(lts_eq_sim, "reduction modulo strong simulation equivalence")).
    append(transformation_selector.associate(lts_eq_trace, "determinisation and reduction modulo trace equivalence")).
    append(transformation_selector.associate(lts_eq_weak_trace, "determinisation and reduction modulo weak trace equivalence")).
    append(transformation_selector.associate(lts_eq_isomorph, "determinisation")); // abusing lts_eq_isomorph for determinisation

  checkbox&   bisimulation_add_eq_classes = d.create< checkbox >();
  text_field& tau_field                   = d.create< text_field >();

  m.append(d.create< label >()).
    append(bisimulation_add_eq_classes.set_label("Add equivalence classes to state instead of reducing LTS")).
    append(d.create< horizontal_box >().
                append(d.create< label >().set_text("Internal (tau) actions : ")).
                append(tau_field.set_text("tau")));

  /* Attach events */
//  transformation_selector.get_button(no_transformation).
//        on_change(boost::bind(mcrl2::utilities::squadt::change_visibility_on_toggle,
//              transformation_selector.get_button(no_transformation), top.get(), tau_field));
//  transformation_selector.get_button(minimisation_modulo_strong_bisimulation).
//        on_change(boost::bind(mcrl2::utilities::squadt::change_visibility_on_toggle,
//              transformation_selector.get_button(minimisation_modulo_strong_bisimulation), top.get(), bisimulation_add_eq_classes));
//  transformation_selector.get_button(minimisation_modulo_branching_bisimulation).
//        on_change(boost::bind(mcrl2::utilities::squadt::change_visibility_on_toggle,
//              transformation_selector.get_button(minimisation_modulo_branching_bisimulation), top.get(), bisimulation_add_eq_classes));

  button& okay_button = d.create< button >().set_label("OK");

  // Add some default values for existing options in the current configuration
  if (c.option_exists(option_selected_output_format)) {
    format_selector.set_selection(c.get_option_argument< mcrl2::lts::lts_type >(option_selected_output_format, 0));
  }
  if (c.option_exists(option_selected_transformation)) {
    transformation_selector.set_selection(c.get_option_argument< lts_equivalence >(option_selected_transformation, 0));
  }
  if (c.input_exists(lps_file_auxiliary)) {
      lps_file_field.set_text(c.get_input(lps_file_auxiliary).location());
  }
  if (c.option_exists(option_no_reachability_check)) {
    check_reachability.set_status(c.get_option_argument< bool >(option_no_reachability_check));
  }
  if (c.option_exists(option_no_state_information)) {
    add_state_information.set_status(c.get_option_argument< bool >(option_no_state_information));
  }
  if (c.option_exists(option_add_bisimulation_equivalence_class)) {
    bisimulation_add_eq_classes.set_status(c.get_option_argument< bool >(option_add_bisimulation_equivalence_class));
  }
  if (c.option_exists(option_tau_actions)) {
    tau_field.set_text(c.get_option_argument< std::string >(option_tau_actions));
  }

  send_display_layout(d.manager(m.append(okay_button, layout::top)));

  /* Wait until the ok button was pressed */
  okay_button.await_change();

  /* Add output file to the configuration */
  std::string     output_name(c.get_output_name("." +
                    lts::extension_for_type(format_selector.get_selection())));
  tipi::mime_type output_type(mcrl2::utilities::squadt::lts_type_to_mime_type(format_selector.get_selection()));

  if (c.output_exists(lts_file_for_output)) {
    tipi::configuration::object& output_file = c.get_output(lts_file_for_output);

    output_file.type(output_type);
    output_file.location(output_name);
  }
  else {
    c.add_output(lts_file_for_output, output_type, output_name);
  }

  /* Add lps file when output is FSM format or when the output is mCRL2 and the input is Aldebaran or mCRL */
  if ((format_selector.get_selection() == lts_fsm && (
         c.get_input(lts_file_for_input).type().sub_type() == "svc" ||
         c.get_input(lts_file_for_input).type().sub_type() == "svc+mcrl" ||
         c.get_input(lts_file_for_input).type().sub_type() == "mcrl2-lts"))
   || (format_selector.get_selection() == lts_mcrl2 && (
         c.get_input(lts_file_for_input).type().sub_type() == "aut" ||
         c.get_input(lts_file_for_input).type().sub_type() == "svc" ||
         c.get_input(lts_file_for_input).type().sub_type() == "svc+mcrl"))) {

    if (c.input_exists(lps_file_auxiliary)) {
      c.get_input(lps_file_auxiliary).location(lps_file_field.get_text());
    }
    else {
      c.add_input(lps_file_auxiliary, tipi::mime_type("lps", tipi::mime_type::application), lps_file_field.get_text());
    }
  }
  else {
    c.remove_input(lps_file_auxiliary);
  }

  lts_equivalence selected_transformation = transformation_selector.get_selection();

  c.add_option(option_selected_transformation).set_argument_value< 0 >(selected_transformation);
  c.add_option(option_selected_output_format).set_argument_value< 0 >(format_selector.get_selection());

  if ((selected_transformation == lts_eq_bisim || selected_transformation == lts_eq_branching_bisim)) {
    c.add_option(option_add_bisimulation_equivalence_class).
        set_argument_value< 0 >(bisimulation_add_eq_classes.get_status());
  }
  else {
    c.remove_option(option_add_bisimulation_equivalence_class);
  }

  c.add_option(option_no_reachability_check).set_argument_value< 0 >(check_reachability.get_status());

  if (format_selector.get_selection() == lts_dot) {
    c.add_option(option_no_state_information).
       set_argument_value< 0 >(add_state_information.get_status());
  }
  else {
    c.remove_option(option_no_state_information);
  }

  if (!tau_field.get_text().empty()) {
    c.add_option(option_tau_actions).set_argument_value< 0 >(tau_field.get_text());
  }
  else {
    c.remove_option(option_tau_actions);
  }

  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(lts_file_for_input);
  result &= c.output_exists(lts_file_for_output);
  result &= c.option_exists(option_selected_transformation);

  if (c.option_exists(option_tau_actions)) {
    lts_eq_options eq_opts;

    /* Need to detect whether the next operation completes successfully or not, exceptions anyone? */
    lts_reduce_add_tau_actions(eq_opts,(c.get_option_argument< std::string >(option_tau_actions)));
  }

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  t_tool_options tool_options;

  if (c.input_exists(lps_file_auxiliary)) {
    tool_options.lpsfile = c.get_input(lps_file_auxiliary).location();
  }
  if (c.option_exists(option_no_state_information)) {
    tool_options.print_dot_state = !(c.get_option_argument< bool >(option_no_state_information));
  }
  if (c.option_exists(option_no_reachability_check)) {
    tool_options.check_reach = !(c.get_option_argument< bool >(option_no_reachability_check));
  }

  tool_options.intype  = lts::parse_format(c.get_output(lts_file_for_input).type().sub_type());
  tool_options.outtype = lts::parse_format(c.get_output(lts_file_for_output).type().sub_type());
  tool_options.set_source(c.get_input(lts_file_for_input).location());
  tool_options.set_target(c.get_output(lts_file_for_output).location());

  lts_equivalence method = c.get_option_argument< lts_equivalence >(option_selected_transformation);

  if (method != lts_eq_none) {
    tool_options.equivalence = method;

    if (method == mcrl2::lts::lts_eq_isomorph) {
      tool_options.determinise = true;
    }

    if (c.option_exists(option_add_bisimulation_equivalence_class)) {
      tool_options.eq_opts.reduce.add_class_to_state = c.get_option_argument< bool >(option_add_bisimulation_equivalence_class);
    }
    if (c.option_exists(option_tau_actions)) {
      lts_reduce_add_tau_actions(tool_options.eq_opts, c.get_option_argument< std::string >(option_tau_actions));
    }
  }

  process(tool_options);

  send_clear_display();

  return true;
}
#endif


int main(int argc, char **argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
      return EXIT_SUCCESS;
    }
#endif

    t_tool_options options;

    if (parse_command_line(argc, argv, options)) {
      process(options);
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
