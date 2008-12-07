// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl22lps.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

#define NAME "mcrl22lps"
#define AUTHOR "Jan Friso Groote"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <fstream>
#include "aterm2.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/lin_types.h"
#include "mcrl2/lps/lin_std.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/data_implementation.h"
#include "mcrl2/core/alpha.h"
#include "mcrl2/utilities/command_line_rewriting.h"
#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/utilities/command_line_interface.h"

#define INFILEEXT ".mcrl2"
#define OUTFILEEXT ".lps"

using namespace mcrl2::utilities;
using namespace mcrl2::core;

//Functions used by the main program
static t_lin_options parse_command_line(int argc, char *argv[]);
static ATermAppl linearise_file(t_lin_options &lin_options);
static char const* lin_method_to_string(t_lin_method lin_method);

// Squadt protocol interface and utility pseudo-library
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include "mcrl2/utilities/mcrl2_squadt_interface.h"

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    /** \brief compiles a t_lin_options instance from a configuration */
    bool extract_task_options(tipi::configuration const& c, t_lin_options&) const;

    static bool initialise_types() {
      tipi::datatype::enumeration< t_lin_method > method_enumeration;

      method_enumeration.
        add(lmRegular, "regular").
        add(lmRegular2, "regular2").
        add(lmStack, "expansion");

      tipi::datatype::enumeration< t_phase > phase_enumeration;

      phase_enumeration.
        add(phNone, "all").
        add(phParse, "parsing").
        add(phTypeCheck, "type-checking").
        add(phAlphaRed, "alphabet_reduction").
        add(phDataImpl, "data_implementation");

      return true;
    }

  public:

    /** \brief constructor */
    squadt_interactor() {
      static bool initialised = initialise_types();

      static_cast< void > (initialised); // harmless, and prevents unused variable warnings
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

const char* mcrl2_file_for_input = "mcrl2_in";
const char* lps_file_for_output  = "lps_out";

const char* option_linearisation_method     = "linearisation_method";
const char* option_final_cluster            = "final_cluster";
const char* option_no_intermediate_cluster  = "no_intermediate_cluster";
const char* option_no_alpha                 = "no_alpha";
const char* option_newstate                 = "newstate";
const char* option_binary                   = "binary";
const char* option_statenames               = "statenames";
const char* option_no_rewrite               = "no_rewrite";
const char* option_no_freevars              = "no_freevars"; 
const char* option_no_sumelm                = "no_sumelm";
const char* option_no_deltaelm              = "no_dataelm";
const char* option_end_phase                = "end_phase";
const char* option_add_delta                = "add_delta";

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(mcrl2_file_for_input, tipi::mime_type("mcrl2", tipi::mime_type::text),
                                                            tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::datatype;
  using namespace tipi::layout::elements;

  std::string infilename = c.get_input(mcrl2_file_for_input).location();

  // Set defaults for options
  if (!c.option_exists(option_final_cluster)) {
    c.add_option(option_final_cluster).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_no_intermediate_cluster)) {
    c.add_option(option_no_intermediate_cluster).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_no_alpha)) {
    c.add_option(option_no_alpha).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_newstate)) {
    c.add_option(option_newstate).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_binary)) {
    c.add_option(option_binary).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_statenames)) {
    c.add_option(option_statenames).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_no_rewrite)) {
    c.add_option(option_no_rewrite).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_no_freevars)) {
    c.add_option(option_no_freevars).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_no_sumelm)) {
    c.add_option(option_no_sumelm).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_no_deltaelm)) {
    c.add_option(option_no_deltaelm).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_add_delta)) {
    c.add_option(option_add_delta).set_argument_value< 0 >(false);
  }

  /* Create display */
  tipi::tool_display d;

  // Helper for linearisation method selection
  mcrl2::utilities::squadt::radio_button_helper < t_lin_method > method_selector(d);

  // Helper for end phase selection
  mcrl2::utilities::squadt::radio_button_helper < t_phase >      phase_selector(d); 

  layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0,5,0,5));

  m.append(d.create< label >().set_text("Method: ")).
    append(d.create< horizontal_box >().
        append(method_selector.associate(lmStack, "Stack")).
        append(method_selector.associate(lmRegular, "Regular", true)).
        append(method_selector.associate(lmRegular2, "Regular2")));

  checkbox& clusterintermediate = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_intermediate_cluster));
  checkbox& clusterfinal        = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_final_cluster));
  checkbox& newstate            = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_newstate));
  checkbox& binary              = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_binary));
  checkbox& statenames          = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_statenames));
  checkbox& add_delta           = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_add_delta));
  checkbox& rewrite             = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_rewrite));
  checkbox& alpha               = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_alpha));
  checkbox& sumelm              = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_sumelm));
  checkbox& deltaelm            = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_deltaelm));
  checkbox& freevars            = d.create< checkbox >().set_status(!c.get_option_argument< bool >(option_no_freevars));

  // two columns to select the linearisation options of the tool
  m.append(d.create< label >().set_text(" ")).
    append(d.create< horizontal_box >().
        append(d.create< vertical_box >().set_default_alignment(layout::right).
            append(clusterintermediate.set_label("Intermediate clustering")).
            append(clusterfinal.set_label("Final clustering")).
            append(newstate.set_label("Use enumerations for state variables")).
            append(binary.set_label("Encode enumerations by booleans")).
            append(statenames.set_label("Derive state names from specification")).
            append(add_delta.set_label("Add delta summands"))).
        append(d.create< vertical_box >().set_default_alignment(layout::left).
            append(rewrite.set_label("Use rewriting")).
            append(alpha.set_label("Apply alphabet axioms")).
            append(sumelm.set_label("Apply sum elimination")).
            append(deltaelm.set_label("Apply delta elimination")).
            append(freevars.set_label("Generate free variables"))));

  // Determine which phases the linearizer will go through. Default is all.
  m.append(d.create< label >().set_text(" ")).
    append(d.create< label >().set_text("Stop after: ")).
    append(d.create< vertical_box >().
        append(phase_selector.associate(phParse, "Parsing")).
        append(phase_selector.associate(phTypeCheck, "Type checking")).
        append(phase_selector.associate(phDataImpl, "Data implementation")).
        append(phase_selector.associate(phAlphaRed, "Alphabet reduction")).
        append(phase_selector.associate(phNone, "Linearisation", true)));

  // Add okay button
  button& okay_button = d.create< button >().set_label("OK");

  m.append(d.create< label >().set_text(" ")).
    append(okay_button, layout::right);

  // Set default values for options if the configuration specifies them
  if (c.option_exists(option_end_phase)) {
    phase_selector.set_selection(c.get_option_argument< t_phase >(option_end_phase, 0));
  }
  if (c.option_exists(option_linearisation_method)) {
    method_selector.set_selection(c.get_option_argument< t_lin_method >(option_linearisation_method, 0));
  }

  send_display_layout(d.manager(m));

  /* Wait for the OK button to be pressed */
  okay_button.await_change();

  // Update configuration
  if (!c.output_exists(lps_file_for_output)) {
    c.add_output(lps_file_for_output, tipi::mime_type("lps", tipi::mime_type::application), c.get_output_name(".lps"));
  }

  c.add_option(option_linearisation_method).
          set_argument_value< 0 >(method_selector.get_selection());

  c.add_option(option_end_phase).
          set_argument_value< 0 >(phase_selector.get_selection());

  if (phase_selector.get_selection() != phNone) { // file will not be produced
    c.remove_output(lps_file_for_output);
  }

  c.get_option(option_final_cluster).set_argument_value< 0 >(clusterfinal.get_status());
  c.get_option(option_no_intermediate_cluster).set_argument_value< 0 >(!clusterintermediate.get_status());
  c.get_option(option_no_alpha).set_argument_value< 0 >(!alpha.get_status());
  c.get_option(option_newstate).set_argument_value< 0 >(newstate.get_status());
  c.get_option(option_binary).set_argument_value< 0 >(binary.get_status());
  c.get_option(option_statenames).set_argument_value< 0 >(statenames.get_status());
  c.get_option(option_no_rewrite).set_argument_value< 0 >(!rewrite.get_status());
  c.get_option(option_no_freevars).set_argument_value< 0 >(!freevars.get_status());
  c.get_option(option_no_sumelm).set_argument_value< 0 >(!sumelm.get_status());
  c.get_option(option_no_deltaelm).set_argument_value< 0 >(!deltaelm.get_status());
  c.get_option(option_add_delta).set_argument_value< 0 >(add_delta.get_status());

  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result |= c.input_exists(mcrl2_file_for_input);
  result |= c.output_exists(lps_file_for_output);
  result |= c.option_exists(option_linearisation_method);

  return (result);
}

bool squadt_interactor::extract_task_options(tipi::configuration const& c, t_lin_options& task_options) const {
  bool result = true;

  if (c.input_exists(mcrl2_file_for_input)) {
    task_options.infilename = c.get_input(mcrl2_file_for_input).location();
  }
  else {
    send_error("Configuration does not contain an input object\n");

    result = false;
  }

  if (c.output_exists(lps_file_for_output) ) {
    task_options.outfilename = c.get_output(lps_file_for_output).location();
  }
  else {
    send_error("Configuration does not contain an output object\n");

    result = false;
  }

  if (c.option_exists(option_linearisation_method)) {
    task_options.lin_method = c.get_option_argument< t_lin_method >(option_linearisation_method, 0);
  }
  else {
    send_error("Configuration does not contain a linearisation method\n");

    result = false;
  } 

  task_options.final_cluster           = c.get_option_argument< bool >(option_final_cluster);
  task_options.no_intermediate_cluster = c.get_option_argument< bool >(option_no_intermediate_cluster);
  task_options.noalpha                 = c.get_option_argument< bool >(option_no_alpha);
  task_options.newstate                = c.get_option_argument< bool >(option_newstate);
  task_options.binary                  = c.get_option_argument< bool >(option_binary);
  task_options.statenames              = c.get_option_argument< bool >(option_statenames);
  task_options.norewrite               = c.get_option_argument< bool >(option_no_rewrite);
  task_options.nofreevars              = c.get_option_argument< bool >(option_no_freevars);
  task_options.nosumelm                = c.get_option_argument< bool >(option_no_sumelm);
  task_options.nodeltaelimination      = c.get_option_argument< bool >(option_no_deltaelm);
  task_options.add_delta               = c.get_option_argument< bool >(option_add_delta);
  
  task_options.end_phase = c.get_option_argument< t_phase >(option_end_phase, 0);

  task_options.check_only = (task_options.end_phase != phNone);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  using namespace boost;
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::datatype;
  using namespace tipi::layout::elements;

  bool result = true;

  t_lin_options task_options;

  // Extract configuration
  extract_task_options(c, task_options);

  /* Create display */
  tipi::tool_display d;

  label& message = d.create< label >();

  d.manager(d.create< vertical_box >().
                        append(message.set_text("Linearisation in progress"), layout::left));

  send_display_layout(d);

  // Perform linearisation
  ATermAppl linearisation_result = linearise_file(task_options);

  if (linearisation_result == 0) {
    message.set_text("Linearisation in failed");

    result = false;
  }
  else if (task_options.check_only) {
    message.set_text(str(format("%s contains a well-formed mCRL2 specification.") % task_options.infilename));
  }
  else {
    //store the result
    FILE *outstream = fopen(task_options.outfilename.c_str(), "wb");

    if (outstream != 0) {
      ATwriteToSAFFile((ATerm) linearisation_result, outstream);

      fclose(outstream);
    }
    else {
      send_error(str(format("cannot open output file '%s'\n") % task_options.outfilename));

      result = false;
    }

    if (result) {
      message.set_text("Linearisation finished");
    }
  }

  send_display_layout(d);

  return (result);
}

#endif

static t_lin_options parse_command_line(int argc, char *argv[])
{ 
  interface_description clinterface(argv[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n",
    "Linearises the mCRL2 specification in INFILE and writes the resulting LPS to "
    "OUTFILE. If OUTFILE is not present, stdout is used. If INFILE is not present, "
    "stdin is used.");

  clinterface.add_option("lin-method", make_mandatory_argument("NAME"),
      "use linearisation method NAME:\n"
      "  'regular' for generating an LPS in regular form\n"
      "  (specification should be regular, default),\n"
      "  'regular2' for a variant of 'regular' that uses more data variables\n"
      "  (useful when 'regular' does not work), or\n"
      "  'stack' for using stack data types\n"
      "  (useful when 'regular' and 'regular2' do not work)"
      , 'l');
  clinterface.add_option("cluster",
      "all actions in the final LPS are clustered", 'c');
  clinterface.add_option("no-cluster",
      "the actions in intermediate LPSs are not clustered "
      "(default behaviour is that intermediate LPSs are "
      "clustered and the final LPS is not clustered)", 'n');
  clinterface.add_option("no-alpha",
      "alphabet reductions are not applied", 'z');
  clinterface.add_option("newstate",
      "state variables are encoded using enumerated types "
      "(requires linearisation method 'regular' or 'regular2'); without this option numbers are used", 'w');
  clinterface.add_option("binary",
      "when clustering use binary case functions instead of "
      "n-ary; in the presence of -w/--newstate, state variables are "
      "encoded by a vector of boolean variables", 'b');
  clinterface.add_option("statenames",
      "the names of state variables are derived from the specification", 'a');
  clinterface.add_option("no-rewrite",
      "do not rewrite data terms while linearising; useful when the rewrite "
      "system does not terminate", 'o');
  clinterface.add_option("no-freevars",
      "instantiate don't care values with arbitrary constants, "
      "instead of modelling them by free variables", 'f');
  clinterface.add_option("no-sumelm",
      "avoid applying sum elimination in parallel composition", 'm');
  clinterface.add_option("no-deltaelm",
      "avoid removing spurious delta summands", 'g');
  clinterface.add_option("delta",
      "add a true->delta summands to each state in each process; "
      "these delta's subsume all other conditional timed delta's, "
      "effectively reducing the number of delta summands drastically "
      "in the resulting linear process; speeds up linearisation ", 'D');
  clinterface.add_option("check-only",
      "check syntax and static semantics; do not linearise", 'e');
  clinterface.add_option("end-phase", make_mandatory_argument("PHASE"),
      "stop linearisation and output the mCRL2 specification after phase PHASE: "
      "'pa' (parsing), "
      "'tc' (type checking), "
      "'ar' (alphabet reduction), or "
      "'di' (data implementation)"
      , 'p');
  clinterface.add_option("pretty",
      "return a pretty printed version of the output", 'P');
  clinterface.add_rewriting_options();

  command_line_parser parser(clinterface, argc, argv);

  t_lin_options options;

  options.final_cluster           = 0 < parser.options.count("cluster");
  options.no_intermediate_cluster = 0 < parser.options.count("no-cluster");
  options.noalpha                 = 0 < parser.options.count("no-alpha");
  options.newstate                = 0 < parser.options.count("newstate");
  options.binary                  = 0 < parser.options.count("binary");
  options.statenames              = 0 < parser.options.count("statenames");
  options.norewrite               = 0 < parser.options.count("no-rewrite");
  options.nofreevars              = 0 < parser.options.count("no-freevars");
  options.nosumelm                = 0 < parser.options.count("no-sumelm");
  options.nodeltaelimination      = 0 < parser.options.count("no-deltaelm");
  options.add_delta               = 0 < parser.options.count("delta");
  options.pretty                  = 0 < parser.options.count("pretty");
  options.rewrite_strategy        = parser.option_argument_as< RewriteStrategy >("rewriter");
  options.lin_method = lmRegular;

  if (0 < parser.options.count("check-only")) {
    options.check_only = true;
    options.end_phase  = phTypeCheck;
  }

  if (0 < parser.options.count("lin-method")) {
    if (1 < parser.options.count("lin-method")) {
      parser.error("multiple use of option -l/--lin-method; only one occurrence is allowed");
    }
    std::string lin_method_str(parser.option_argument("lin-method"));
    if (lin_method_str == "stack") {
      options.lin_method = lmStack;
    } else if (lin_method_str == "regular") {
      options.lin_method = lmRegular;
    } else if (lin_method_str == "regular2") {
      options.lin_method = lmRegular2;
    } else {
      parser.error("option -l/--lin-method has illegal argument '" + lin_method_str + "'");
    }
  }

  if (parser.options.count("end-phase")) {
    if (1 < parser.options.count("end-phase")) {
      parser.error("multiple use of option -p/--end-phase; only one occurrence is allowed");
    }
    std::string phase(parser.option_argument("end-phase"));
    if (phase == "pa") {
      options.end_phase = phParse;
    } else if (phase == "tc") {
      options.end_phase = phTypeCheck;
    } else if (phase == "ar") {
      options.end_phase = phAlphaRed;
    } else if (phase == "di") {
      options.end_phase = phDataImpl;
    } else {
      parser.error("option -p/--end-phase has illegal argument '" + phase + "'");
    }
  }

  //check for dangerous and illegal option combinations
  if (options.newstate && options.lin_method == lmStack) {
    parser.error("option -w/--newstate cannot be used with -lstack/--lin-method=stack");
  }
  if (options.check_only && options.end_phase != phTypeCheck) {
    parser.error("options -e/--check-only and -p/--end-phase may not be used in conjunction");
  }
  if (options.noalpha && options.end_phase == phAlphaRed) {
    parser.error("options -r/--no-alpha and -par/--end-phase=ar may not be used in conjunction");
  }

  if (2 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }
  else {
    if (0 < parser.arguments.size()) {
      options.infilename = parser.arguments[0];
    }
    if (1 < parser.arguments.size()) {
      options.outfilename = parser.arguments[1];
    }
  }

  return options;
}

ATermAppl linearise_file(t_lin_options &lin_options)
{
  ATermAppl result = NULL;
  //parse specification
  if (lin_options.infilename == "") {
    //parse specification from stdin
    gsVerboseMsg("parsing input from stdin...\n");
    result = parse_proc_spec(std::cin);
  } else {
    //parse specification from infilename
    std::ifstream instream(lin_options.infilename.c_str(), std::ifstream::in|std::ifstream::binary);
    if (!instream.is_open()) {
      gsErrorMsg("cannot open input file '%s'\n", lin_options.infilename.c_str());
      return NULL;
    }
    gsVerboseMsg("parsing input file '%s'...\n", lin_options.infilename.c_str());
    result = parse_proc_spec(instream);
    instream.close();
  }
  if (result == NULL) 
  {
    gsErrorMsg("parsing failed\n");
    return NULL;
  }

  if (lin_options.end_phase == phParse) {
    return result;
  }
  //type check the result
  gsVerboseMsg("type checking...\n");
  result = type_check_proc_spec(result);
  if (result == NULL) 
  {
    gsErrorMsg("type checking failed\n");
    return NULL;
  }
  if (lin_options.end_phase == phTypeCheck) {
    return result;
  }
  //perform alphabet reductions 
  if (!lin_options.noalpha) {
    gsVerboseMsg("performing alphabet reductions...\n");
    result = gsAlpha(result);
    if (result == NULL)
    {
      gsErrorMsg("alphabet reductions failed\n");
      return NULL;
    }
    if (lin_options.end_phase == phAlphaRed) {
      return result;
    }
  }
  //implement standard data types and type constructors on the result
  gsVerboseMsg("implementing standard data types and type constructors...\n");
  result = implement_data_proc_spec(result);
  if (result == NULL) 
  {
    gsErrorMsg("data implementation failed\n");
    return NULL;
  }
  if (lin_options.end_phase == phDataImpl) {
    return result;
  }
  //linearise the result
  gsVerboseMsg("linearising processes using the %s method\n", lin_method_to_string(lin_options.lin_method));

  result = linearise_std(result, lin_options);
  if (result == NULL) 
  {
    gsErrorMsg("linearisation failed\n");
    return NULL;
  }
  return result; 
}

inline char const* lin_method_to_string(t_lin_method lin_method)
{
  static const char* method[] = {"stack","regular","regular2"};

  return (method[lin_method]);
}

// Main 

int main(int argc, char *argv[])
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
      return EXIT_SUCCESS;
    }
#endif

    t_lin_options lin_options = parse_command_line(argc, argv);

    //linearise infilename with options lin_options
    ATermAppl result = linearise_file(lin_options);
    if (result == NULL) {
      return EXIT_FAILURE;
    }
    //report on well-formedness (if needed)
    if (lin_options.check_only) {
      if (lin_options.infilename == "") {
        fprintf(stdout, "stdin");
      } else {
        fprintf(stdout, "The file '%s'", lin_options.infilename.c_str());
      }
      fprintf(stdout, " contains a well-formed mCRL2 specification.\n");
      return EXIT_SUCCESS;
    }
    //store the result
    if (lin_options.outfilename.empty()) {
      gsVerboseMsg("saving result to stdout...\n");
    } else {
      gsVerboseMsg("saving result to '%s'...\n", lin_options.outfilename.c_str());
    }
    if ((lin_options.end_phase == phNone) && (!lin_options.pretty)) {
      mcrl2::lps::specification spec(result);
      spec.save(lin_options.outfilename);
    } else {
      if (lin_options.outfilename.empty()) {
        PrintPart_CXX(std::cout, (ATerm) result, (lin_options.pretty)?ppDefault:ppInternal);
        std::cout << std::endl;
      } else {
        std::ofstream outstream(lin_options.outfilename.c_str(), std::ofstream::out|std::ofstream::binary);
        if (!outstream.is_open()) {
          throw mcrl2::runtime_error("could not open output file '" + lin_options.outfilename + "' for writing");
        }
        PrintPart_CXX(outstream, (ATerm) result, lin_options.pretty?ppDefault:ppInternal);
        outstream.close();
      }     
    }
    return EXIT_SUCCESS;
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
