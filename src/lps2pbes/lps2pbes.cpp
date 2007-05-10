#define NAME "lps2pbes"
#define VERSION "0.3"

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <climits>
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <aterm2.h>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint.h"
#include "libprint_c.h"
#include "libparse.h"
#include "typecheck.h"
#include "dataimpl.h"
#include "regfrmtrans.h"
#include "lps/specification.h"
#include "lps/mucalculus.h"
#include "lps/pbes_translate.h"
#include "lps/pbes.h"

using namespace std;
using namespace lps;

//Type definitions
//----------------

//t_phase represents the phases at which the program should be able to stop
typedef enum { PH_NONE, PH_PARSE, PH_TYPE_CHECK, PH_DATA_IMPL, PH_REG_FRM_TRANS } t_phase;

//t_tool_options represents the options of the tool 
typedef struct {
  bool pretty;
  bool untimed;
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
static void print_version(void);
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
#include <utilities/mcrl2_squadt.h>

class squadt_interactor : public mcrl2_squadt::tool_interface {

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
    static const char* option_special_untimed_conversion;

  private:

    boost::shared_ptr < sip::datatype::enumeration > output_format_enumeration;

  public:

    /** \brief constructor */
    squadt_interactor();

    /** \brief configures tool capabilities */
    void set_capabilities(sip::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(sip::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(sip::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(sip::configuration&);
};

const char* squadt_interactor::lps_file_for_input     = "lps_in";
const char* squadt_interactor::formula_file_for_input = "formula_in";
const char* squadt_interactor::pbes_file_for_output   = "pbes_out";

const char* squadt_interactor::option_selected_output_format     = "selected_output_format";
const char* squadt_interactor::option_end_phase                  = "stop_after_phase";
const char* squadt_interactor::option_special_untimed_conversion = "special_untimed_conversion";

squadt_interactor::squadt_interactor() {
  output_format_enumeration.reset(new sip::datatype::enumeration("normal"));

  output_format_enumeration->add_value("readable");
}

void squadt_interactor::set_capabilities(sip::tool::capabilities& c) const {
  c.add_input_combination(lps_file_for_input, sip::mime_type("lps", sip::mime_type::application), sip::tool::category::transformation);
}

void squadt_interactor::user_interactive_configuration(sip::configuration& c) {
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::layout::elements;

  if (!c.option_exists(option_special_untimed_conversion)) {
    c.add_option(option_special_untimed_conversion, false).
        set_argument_value< 0, sip::datatype::boolean >(false, false);
  }

  layout::tool_display::sptr display(new layout::tool_display);

  /* Create and add the top layout manager */
  layout::vertical_box::aptr top(new layout::vertical_box);

  layout::manager* h = new layout::horizontal_box();

  top->add(new label("Phase after which to stop: "), margins(0,5,0,5));

  squadt_utility::radio_button_helper < t_phase > phase_selector(h, PH_NONE, "none");

  phase_selector.associate(h, PH_PARSE, "parsing");
  phase_selector.associate(h, PH_TYPE_CHECK, "type checking");
  phase_selector.associate(h, PH_DATA_IMPL, "data implementation");
  phase_selector.associate(h, PH_REG_FRM_TRANS, "formula translation");

  if (c.option_exists(option_end_phase)) {
    phase_selector.set_selection(static_cast < t_phase > (
        c.get_option_argument< size_t >(option_end_phase, 0)));
  }

  /* Attach row */
  top->add(h, margins(0,5,0,5));

  top->add(new label("Output format : "), margins(0,5,0,5));

  h = new layout::horizontal_box();

  squadt_utility::radio_button_helper < pbes_output_format > format_selector(h, normal, "normal");

  format_selector.associate(h, readable, "readable");

  if (c.option_exists(option_selected_output_format)) {
    format_selector.set_selection(static_cast < pbes_output_format > (
        c.get_option_argument< size_t >(option_selected_output_format, 0)));
  }

  /* Attach row */
  top->add(h, margins(0,5,0,5));

  h = new layout::horizontal_box();

  h->add(new label("Formula file name : "));
  text_field* formula_field = static_cast < text_field* > (h->add(new text_field("")));
  top->add(h);

  if (c.input_exists(formula_file_for_input)) {
    formula_field->set_text(c.get_input(formula_file_for_input).get_location());
  }

  checkbox* special_untimed_conversion = new checkbox("special untimed conversion",
      c.get_option_argument< bool >(option_special_untimed_conversion));

  top->add(special_untimed_conversion, margins(0,5,0,5));
  
  button* okay_button = new button("OK");

  top->add(okay_button, layout::top);

  display->set_manager(top);

  m_communicator.send_display_layout(display);

  /* Wait until the ok button was pressed */
  okay_button->await_change();

  c.add_input(formula_file_for_input, sip::mime_type("mf", sip::mime_type::text), formula_field->get_text());

  /* Add output file to the configuration */
  if (c.output_exists(pbes_file_for_output)) {
    sip::object& output_file = c.get_output(pbes_file_for_output);
 
    output_file.set_location(c.get_output_name(".pbes"));
  }
  else {
    if (format_selector.get_selection() == normal) {
      c.add_output(pbes_file_for_output, sip::mime_type("pbes", sip::mime_type::application), c.get_output_name(".pbes"));
    }
    else {
      c.add_output(pbes_file_for_output, sip::mime_type("pbes", sip::mime_type::text), c.get_output_name(".pbes"));
    }
  }

  c.add_option(option_special_untimed_conversion).set_argument_value< 0, sip::datatype::boolean >(special_untimed_conversion->get_status());
  c.add_option(option_selected_output_format).append_argument(output_format_enumeration,
                                static_cast < pbes_output_format > (format_selector.get_selection()));
  c.add_option(option_end_phase).set_argument_value< 0, sip::datatype::integer >(static_cast < t_phase > (format_selector.get_selection()));

  send_clear_display();
}

bool squadt_interactor::check_configuration(sip::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(lps_file_for_input);
  result &= c.input_exists(formula_file_for_input);
  result &= c.output_exists(pbes_file_for_output);
  result &= c.option_exists(option_end_phase);
  result &= c.option_exists(option_special_untimed_conversion);

  return (result);
}

bool squadt_interactor::perform_task(sip::configuration& c) {
  t_tool_options tool_options;

  tool_options.pretty           = static_cast < pbes_output_format > (c.get_option_argument< size_t >(option_selected_output_format)) != normal;
  tool_options.untimed          = c.get_option_argument< bool >(option_special_untimed_conversion);
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
  ATerm stackbot;

  //initialise ATerm library
  ATinit(argc,argv,&stackbot);
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (!mcrl2_squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
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
  bool opt_untimed = false;
  string formfilename = "";
  #define SHORT_OPTIONS "f:p:ehquvd"
  #define VERSION_OPTION CHAR_MAX + 1
  struct option long_options[] = {
    { "formula",   required_argument,  NULL,  'f' },
    { "end-phase", required_argument,  NULL,  'p' },
    { "untimed",   no_argument,        NULL,  'u' },
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
      case 'u': /* untimed */
        opt_untimed = true;
        break;
      case 'e': /* pretty */
        opt_pretty = true;
        break;
      case 'h': /* help */
        print_help(argv[0]);
        exit(0);
      case VERSION_OPTION: /* version */
        print_version();
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
  tool_options.end_phase    = opt_end_phase;
  tool_options.pretty       = opt_pretty;
  tool_options.untimed      = opt_untimed;
  tool_options.formfilename = formfilename;
  tool_options.infilename   = infilename;
  tool_options.outfilename  = outfilename;
  return tool_options;
}

ATermAppl create_pbes(t_tool_options tool_options)
{
  string infilename = tool_options.infilename;
  string outfilename = tool_options.outfilename;
  string formfilename = tool_options.formfilename;
  t_phase end_phase = tool_options.end_phase;

  //open infilename
  lps::specification lps_spec = lps::specification();
  if (infilename == "") {
    if (!lps_spec.load("-")) {
      gsErrorMsg("cannot open LPS from stdin\n");
      return NULL;
    }
  } else {
    if (!lps_spec.load(infilename)) {
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

  //type check formula
  gsVerboseMsg("type checking...\n");
  result = type_check_state_frm(result, lps_spec);
  if (result == NULL) {
    gsErrorMsg("type checking failed\n");
    return NULL;
  }
  if (end_phase == PH_TYPE_CHECK) {
    return result;
  }

  //implement standard data types and type constructors on the result
  gsVerboseMsg("implementing standard data types and type constructors...\n");
  result = implement_data_state_frm(result, lps_spec);
  if (result == NULL) {
    gsErrorMsg("data implementation failed\n");
    return NULL;
  }
  if (end_phase == PH_DATA_IMPL) {
    return result;
  }

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
  pbes p = lps::pbes_translate(state_formula(result), lps_spec, !tool_options.untimed);
  result = ATermAppl(p);
  if (result == NULL) {
    return NULL;
  }

  return result;
}


static void print_help(char *name)
{
  fprintf(stderr,
    "Usage: %s [OPTION]... -f FILE [INFILE [OUTFILE]]\n"
    "Convert the state formula in FILE and the LPS in INFILE to a parameterised\n"
    "boolean equation system (PBES) and save it to OUTFILE.\n"
    "If OUTFILE is not present, stdout is used. If INFILE is not present, stdin is\n"
    "used.\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -f, --formula=FILE    use the state formula from FILE\n"
    "  -p, --end-phase=PHASE stop conversion after phase PHASE and output the\n"
    "                        result; PHASE can be 'pa' (parse), 'tc' (type check),\n"
    "                        'di' (data implementation) or 'rft' (regular formula\n"
    "                        translation)\n"
    "  -u, --untimed         apply special conversion for untimed LPS's\n"
    "  -e, --external        return the result in the external format\n"
    "  -h, --help            display this help message and terminate\n"
    "      --version         display version information and terminate\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         display concise intermediate messages\n"
    "  -d, --debug           display detailed intermediate messages\n",
    name
  );
}

void print_version(void)
{
  fprintf(stderr,"%s %s (revision %s)\n", NAME, VERSION, REVISION);
}

void print_more_info(char *name)
{
  fprintf(stderr, "Use %s --help for options\n", name);
}
