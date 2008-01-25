// Author(s): Alexander van Dam
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2pbes.cpp
/// \brief Add your file description here.

#define NAME "lps2pbes"
#define AUTHOR "Alexander van Dam, Aad Mathijssen and Wieger Wesselink"

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <aterm2.h>
#include "mcrl2/core/struct.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/parse.h"
#include "mcrl2/core/detail/typecheck.h"
#include "mcrl2/core/detail/data_implementation.h"
#include "mcrl2/core/detail/data_reconstruct.h"
#include "mcrl2/core/detail/regfrmtrans.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/pbes/pbes_translate.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/version_info.h"

using namespace std;
using namespace mcrl2::lps;
using namespace mcrl2::pbes_system;
using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::modal;

//Type definitions
//----------------

//t_phase represents the phases at which the program should be able to stop
typedef enum { PH_NONE, PH_PARSE, PH_TYPE_CHECK, PH_DATA_IMPL, PH_REG_FRM_TRANS } t_phase;

//t_tool_options represents the options of the tool
typedef struct {
  bool pretty;
  bool timed;
  t_phase end_phase;
  string formfilename;
  string infilename;
  string outfilename;
} t_tool_options;

//Functions used by the main program
//----------------------------------

static t_tool_options parse_command_line(int argc, char **argv);
//Post: The command line options are parsed.
//      The program has aborted with a suitable error code, if:
//      - errors were encountered
//      - non-standard behaviour was requested (help or version)
//Ret:  the parsed command line options

static ATermAppl create_pbes(t_tool_options tool_options);
//Pre:  tool_options.formfilename contains a state formula
//      tool_options.infilename contains an LPS ("" indicates stdin)
//      tool_options.end_phase indicates at which phase conversion stops
//Ret:  if end_phase == PH_NONE, the PBES generated from the state formula and
//      the LPS
//      if end_phase != PH_NONE, the state formula after phase end_phase
//      NULL, if something went wrong

static void print_help(char *name);
static void print_more_info(char *name);

bool process(t_tool_options const& tool_options) {
  //process state formula
  ATermAppl result = create_pbes(tool_options);
  if (result == 0) {
    return false;
  }

  //store the result
  string outfilename = tool_options.outfilename;
  bool opt_pretty = tool_options.pretty;
  if (outfilename == "") {
    gsVerboseMsg("saving result to stdout...\n");
    PrintPart_CXX(cout, (ATerm) result, opt_pretty?ppDefault:ppInternal);
    cout << endl;
  } else {
    gsVerboseMsg("saving result to '%s'...\n", outfilename.c_str());
    ofstream outstream(outfilename.c_str(), ofstream::out|ofstream::binary);
    if (!outstream.is_open()) {
      gsErrorMsg("cannot open output file '%s'\n", outfilename.c_str());
      return false;
    }
    PrintPart_CXX(outstream, (ATerm) result, opt_pretty?ppDefault:ppInternal);
    outstream.close();
  }

  return true;
}

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include "mcrl2/utilities/squadt_interface.h"

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    static const char*  lps_file_for_input;      ///< file containing an LPS
    static const char*  formula_file_for_input;  ///< file containing a formula
    static const char*  pbes_file_for_output;    ///< file used to write the output to

    enum pbes_output_format {
      normal,
      readable
    };

    static const char* option_selected_output_format;
    static const char* option_end_phase;
    static const char* option_timed;

  private:

    boost::shared_ptr < tipi::datatype::enumeration > output_format_enumeration;

  public:

    /** \brief constructor */
    squadt_interactor();

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration&);
};

const char* squadt_interactor::lps_file_for_input     = "lps_in";
const char* squadt_interactor::formula_file_for_input = "formula_in";
const char* squadt_interactor::pbes_file_for_output   = "pbes_out";

const char* squadt_interactor::option_selected_output_format     = "selected_output_format";
const char* squadt_interactor::option_end_phase                  = "stop_after_phase";
const char* squadt_interactor::option_timed                      = "use_timed_algorithm";

squadt_interactor::squadt_interactor() {
  output_format_enumeration.reset(new tipi::datatype::enumeration("normal"));

  output_format_enumeration->add_value("readable");
}

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(lps_file_for_input, tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  // Set default values for configuration
  if (!c.option_exists(option_timed)) {
    c.add_option(option_timed, false).
        set_argument_value< 0, tipi::datatype::boolean >(false, false);
  }

  /* Create display */
  tipi::layout::tool_display d;

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
  checkbox&   timed_conversion = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_timed));
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
  if (c.option_exists(option_selected_output_format)) {
    format_selector.set_selection(static_cast < pbes_output_format > (
        c.get_option_argument< size_t >(option_selected_output_format, 0)));
  }
  if (c.option_exists(option_end_phase)) {
    phase_selector.set_selection(static_cast < t_phase > (
        c.get_option_argument< long int >(option_end_phase)));
  }
  if (c.input_exists(formula_file_for_input)) {
    formula_field.set_text(c.get_input(formula_file_for_input).get_location());
  }

  send_display_layout(d.set_manager(m));

  /* Wait until the ok button was pressed */
  okay_button.await_change();
  
  if (c.output_exists(formula_file_for_input)) {
    c.get_input(formula_file_for_input).set_location(formula_field.get_text());
  }
  else {
    c.add_input(formula_file_for_input, tipi::mime_type("mf", tipi::mime_type::text), formula_field.get_text());
  }

  /* Add output file to the configuration */
  if (c.output_exists(pbes_file_for_output)) {
    tipi::object& output_file = c.get_output(pbes_file_for_output);

    output_file.set_location(c.get_output_name(".pbes"));
  }
  else {
    if (format_selector.get_selection() == normal) {
      c.add_output(pbes_file_for_output, tipi::mime_type("pbes", tipi::mime_type::application), c.get_output_name(".pbes"));
    }
    else {
      c.add_output(pbes_file_for_output, tipi::mime_type("pbes", tipi::mime_type::text), c.get_output_name(".pbes"));
    }
  }

  c.add_option(option_timed).set_argument_value< 0, tipi::datatype::boolean >(timed_conversion.get_status());
  c.add_option(option_selected_output_format).append_argument(output_format_enumeration,
                                static_cast < pbes_output_format > (format_selector.get_selection()));
  c.add_option(option_end_phase).set_argument_value< 0, tipi::datatype::integer >(static_cast < t_phase > (phase_selector.get_selection()));

  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(lps_file_for_input);
  result &= c.input_exists(formula_file_for_input);
  result &= c.output_exists(pbes_file_for_output);
  result &= c.option_exists(option_end_phase);
  result &= c.option_exists(option_timed);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  t_tool_options tool_options;

  tool_options.pretty           = static_cast < pbes_output_format > (c.get_option_argument< size_t >(option_selected_output_format)) != normal;
  tool_options.timed            = c.get_option_argument< bool >(option_timed);
  tool_options.end_phase        = static_cast < t_phase > (c.get_option_argument< long int >(option_end_phase));
  tool_options.formfilename     = c.get_input(formula_file_for_input).get_location();
  tool_options.infilename       = c.get_input(lps_file_for_input).get_location();
  tool_options.outfilename      = c.get_output(pbes_file_for_output).get_location();

  bool result = process(tool_options);

  if (result) {
    send_clear_display();
  }

  return (result);
}
#endif

//Main program
//------------

int main(int argc, char **argv)
{
  MCRL2_ATERM_INIT(argc, argv)

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (!mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
#endif
    //parse command line
    t_tool_options tool_options = parse_command_line(argc, argv);

    if (!process(tool_options)) {
      return 1;
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}

static t_tool_options parse_command_line(int argc, char **argv)
{
  t_tool_options tool_options;
  //declarations for getopt
  t_phase opt_end_phase = PH_NONE;
  bool opt_pretty = false;
  bool opt_timed = false;
  string formfilename = "";
  #define SHORT_OPTIONS "f:p:ehqtvd"
  #define VERSION_OPTION 0x1
  struct option long_options[] = {
    { "formula",   required_argument,  NULL,  'f' },
    { "timed",     no_argument,        NULL,  't' },
    { "end-phase", required_argument,  NULL,  'p' },
    { "external",  no_argument,        NULL,  'e' },
    { "help",      no_argument,        NULL,  'h' },
    { "version",   no_argument,        NULL,  VERSION_OPTION },
    { "quiet",     no_argument,        NULL,  'q' },
    { "verbose",   no_argument,        NULL,  'v' },
    { "debug",     no_argument,        NULL,  'd' },
    { 0, 0, 0, 0 }
  };
  int option;
  //parse options
  while ((option = getopt_long(argc, argv, SHORT_OPTIONS, long_options, NULL)) != -1) {
    switch (option) {
      case 'f': /* formula */
        formfilename = optarg;
        break;
      case 't': /* timed */
        opt_timed = true;
        break;
      case 'p': /* end-phase */
        if (strcmp(optarg, "pa") == 0) {
          opt_end_phase = PH_PARSE;
        } else if (strcmp(optarg, "tc") == 0) {
          opt_end_phase = PH_TYPE_CHECK;
        } else if (strcmp(optarg, "di") == 0) {
          opt_end_phase = PH_DATA_IMPL;
        } else if (strcmp(optarg, "rft") == 0) {
          opt_end_phase = PH_REG_FRM_TRANS;
        } else {
          gsErrorMsg("option -p has illegal argument '%s'\n", optarg);
          exit(1);
        }
        break;
      case 'e': /* pretty */
        opt_pretty = true;
        break;
      case 'h': /* help */
        print_help(argv[0]);
        exit(0);
      case VERSION_OPTION: /* version */
        print_version_information(NAME, AUTHOR);
        exit(0);
      case 'q': /* quiet */
        gsSetQuietMsg();
        break;
      case 'v': /* verbose */
        gsSetVerboseMsg();
        break;
      case 'd': /* debug */
        gsSetDebugMsg();
        break;
      case '?':
      default:
        print_more_info(argv[0]);
        exit(1);
    }
  }
  //check for presence of -f
  if (formfilename == "") {
    gsErrorMsg("option -f is not specified\n");
    exit(1);
  }
  //check for wrong number of arguments
  string infilename;
  string outfilename;
  int noargc; //non-option argument count
  noargc = argc - optind;
  if (noargc > 2) {
    fprintf(stderr, "%s: too many arguments\n", NAME);
    print_more_info(argv[0]);
    exit(1);
  } else {
    //noargc >= 0 && noargc <= 2
    if (noargc > 0) {
      infilename = argv[optind];
    }
    if (noargc == 2) {
      outfilename = argv[optind + 1];
      //check if input and output files are the same; disabled since it is not
      //problematic
      /*
      if (strcmp(infilename,outfilename) == 0) {
        gsErrorMsg("input and output files are the same\n");
        exit(1);
      }
      */
    }
  }
  tool_options.formfilename = formfilename;
  tool_options.infilename   = infilename;
  tool_options.outfilename  = outfilename;
  tool_options.timed        = opt_timed;
  tool_options.end_phase    = opt_end_phase;
  tool_options.pretty       = opt_pretty;
  return tool_options;
}

ATermAppl create_pbes(t_tool_options tool_options)
{
  string infilename = tool_options.infilename;
  string outfilename = tool_options.outfilename;
  string formfilename = tool_options.formfilename;
  t_phase end_phase = tool_options.end_phase;

  //open infilename
  specification lps_spec;
  if (infilename == "") {
    try
    {
      lps_spec.load("-");
    }
    catch (std::runtime_error e)
    {
      gsErrorMsg("cannot open LPS from stdin\n");
      return NULL;
    }
  } else {
    try
    {
      lps_spec.load(infilename);
    }
    catch (std::runtime_error e)
    {
      gsErrorMsg("cannot open LPS from '%s'\n", infilename.c_str());
      return NULL;
    }
  }

  //check if lps_spec is an LPS
  //XXX need soundness check

  //parse formula from formfilename
  gsVerboseMsg("parsing formula from '%s'...\n", formfilename.c_str());
  ifstream formstream(formfilename.c_str(), ifstream::in|ifstream::binary);
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

  ATermAppl reconstructed_spec = reconstruct_spec(lps_spec);

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
  pbes<> p = pbes_translate(state_formula(result), lps_spec, tool_options.timed);
  result = ATermAppl(p);
  if (result == NULL) {
    return NULL;
  }

  return result;
}


static void print_help(char *name)
{
  fprintf(stdout,
    "Usage: %s [OPTION]... -f FILE [INFILE [OUTFILE]]\n"
    "Convert the state formula in FILE and the LPS in INFILE to a parameterised\n"
    "boolean equation system (PBES) and save it to OUTFILE. If OUTFILE is not\n"
    "present, stdout is used. If INFILE is not present, stdin is used.\n"
    "\n"
    "Options:\n"
    "  -fFILE, --formula=FILE      use the state formula from FILE\n"
    "  -t, --timed                 use the timed version of the algorithm, even for\n"
    "                              untimed LPS's\n"
    "  -pPHASE, --end-phase=PHASE  stop conversion and output the result after PHASE\n"
    "                              'pa' (parsing), 'tc' (type checking), 'di'\n"
    "                              (data implementation) or 'rft' (regular formula\n"
    "                              translation)\n"
    "  -e, --external              return the result in the external format\n"
    "  -h, --help                  display this help message and terminate\n"
    "      --version               display version information and terminate\n"
    "  -q, --quiet                 do not display warning messages\n"
    "  -v, --verbose               display concise intermediate messages\n"
    "  -d, --debug                 display detailed intermediate messages\n"
    "\n"
    "Report bugs at <http://www.mcrl2.org/issuetracker>.\n"
    , name
  );
}

void print_more_info(char *name)
{
  fprintf(stderr, "Use %s --help for options\n", name);
}
