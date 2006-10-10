#include <assert.h>
#include <stdbool.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdio>
#include <fstream>
#include "aterm2.h"
#include "lin_types.h"
#include "lin_std.h"
#include "lin_alt.h"
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libparse.h"
#include "typecheck.h"
#include "libalpha.h"
#include "dataimpl.h"

#include "mcrl2_revision.h"

#define NAME "mcrl22lpe"
#define VERSION "0.2.1"
#define INFILEEXT ".mcrl2"
#define OUTFILEEXT ".lpe"

//Functions used by the main program
static ATermAppl linearise_file(t_lin_options &lin_options);
static char const* lin_method_to_string(t_lin_method lin_method);
static void AltIllegalOptWarning(char opt);
static void PrintMoreInfo(char *Name);
static void PrintVersion(void);
static void PrintHelp(char *Name);

// Squadt protocol interface and utility pseudo-library
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <squadt_utility.h>

class squadt_interactor : public squadt_tool_interface {

  private:

    enum input_files {
      mcrl2_file_for_input = 0,  ///< file containing an LTS that can be imported using the LTS library
      lpd_file_for_output  = 1,  ///< file used to write the output to
    };

    enum further_options {
      option_linearisation_method = 2,
      option_report_mode,
      option_final_cluster,
      option_no_intermediate_cluster,
      option_no_alpha,
      option_newstate,
      option_binary,
      option_statenames,
      option_no_rewrite,
      option_no_freevars,
      option_no_sumelm,
      option_no_deltaelm,
      option_end_phase
    };

    enum report_options {
      report_normal,
      report_verbose,
      report_debug
    };

  private:

    boost::shared_ptr < sip::datatype::enumeration > linearisation_method_enumeration;

    boost::shared_ptr < sip::datatype::enumeration > linearisation_phase_enumeration;

    boost::shared_ptr < sip::datatype::enumeration > report_mode_enumeration;

  private:

    /** \brief compiles a t_lin_options instance from a configuration */
    bool extract_task_options(sip::configuration const& c, t_lin_options&) const;

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

squadt_interactor::squadt_interactor() {
  linearisation_method_enumeration = sip::datatype::enumeration::create("stack");

  *linearisation_method_enumeration % "regular" % "regular2" % "expansion";

  linearisation_phase_enumeration = sip::datatype::enumeration::create("all");

  *linearisation_phase_enumeration % "parsing" % "type-checking" % "alphabet reduction" % "data implementation";

  report_mode_enumeration = sip::datatype::enumeration::create("normal");

  *report_mode_enumeration % "verbose" % "debug";
}

void squadt_interactor::set_capabilities(sip::tool::capabilities& c) const {
  c.add_input_combination(mcrl2_file_for_input, "Transformation", "mcrl2");
}

void squadt_interactor::user_interactive_configuration(sip::configuration& c) {
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::datatype;
  using namespace sip::layout::elements;

  std::string infilename = c.get_object(mcrl2_file_for_input)->get_location();

  layout::manager::aptr top = layout::vertical_box::create();

  // box to select the output file
//  layout::manager* current_box      = new horizontal_box();
//  text_field*      outfilenamefield = new text_field(suggestedoutfilename);

//  std::string suggestedoutfilename(infilename + ".lpe", 0 , infilename.size()-6);

//  current_box->add(new label("Output file name: "),middle);
//  current_box->add(outfilenamefield,middle);

//  top->add(new label(" "));
//  top->add(current_box,layout::left);

  // Linearisation method selection
  layout::manager* current_box      = new horizontal_box();

  squadt_utility::radio_button_helper < t_lin_method >
                                        method_selector(current_box, lmStack, "Stack");

  method_selector.associate(current_box, lmRegular, "Regular", true);
  method_selector.associate(current_box, lmRegular2, "Regular2");
  method_selector.associate(current_box, lmAlternative, "Expansion");

  top->add(new label(" "));
  top->add(current_box);

  // two columns to select the linearisation options of the tool
  layout::manager* columns = new layout::horizontal_box();

  // left option column
  current_box = new vertical_box();

  checkbox* noclusterintermediate = new checkbox("No intermediate clustering");
  checkbox* clusterfinal          = new checkbox("Final clustering");
  checkbox* newstate              = new checkbox("Use enumerated states");
  checkbox* binary                = new checkbox("Encode enumerated types by booleans");
  checkbox* statenames            = new checkbox("Use informative state names ");

  current_box->add(noclusterintermediate);
  current_box->add(clusterfinal);
  current_box->add(newstate);
  current_box->add(binary);
  current_box->add(statenames);

  columns->add(current_box);

  // right option column
  current_box = new vertical_box();

  checkbox* norewrite  = new checkbox("Do not rewrite");
  checkbox* noalpha    = new checkbox("Do not apply alphabet axioms");
  checkbox* nosumelm   = new checkbox("Do not apply sum elimination");
  checkbox* nodeltaelm = new checkbox("Do not apply delta elimination");
  checkbox* nofreevars = new checkbox("Suppress generating free variables");

  current_box->add(norewrite,layout::left);
  current_box->add(noalpha,layout::left);
  current_box->add(nosumelm,layout::left);
  current_box->add(nodeltaelm,layout::left);
  current_box->add(nofreevars,layout::left);

  columns->add(current_box);

  top->add(new label(" "));
  top->add(columns);

  // Determine which phases the linearizer will go through. Default is all.
  current_box = new horizontal_box();

  squadt_utility::radio_button_helper < t_phase >
                                        phase_selector(current_box, phParse, "Parsing");

  phase_selector.associate(current_box, phTypeCheck, "Type checking");
  phase_selector.associate(current_box, phDataImpl, "Data implementation");
  phase_selector.associate(current_box, phAlphaRed, "Alphabet reduction");
  phase_selector.associate(current_box, phNone, "Linearisation", true);

  top->add(new label(" "));
  top->add(new label("Stop after"),layout::left);
  top->add(current_box);

  // Message report level
  current_box = new horizontal_box();

  squadt_utility::radio_button_helper < report_options >
                                        report_selector(current_box, report_normal, "Normal");

  report_selector.associate(current_box, report_verbose, "Verbose");
  report_selector.associate(current_box, report_debug, "Debug");

  top->add(new label(" "));
  top->add(new label("Report level"));
  top->add(current_box);

  // Add okay button
  button* okay_button = new button("OK");

  top->add(new label(" "));
  top->add(okay_button, layout::right);

  send_display_layout(top);

  /* Wait for the OK button to be pressed */
  okay_button->await_change();

  /* set the squadt configuration to be sent back, such
   * that mcrl22lpe can be restarted later with exactly
   * the same parameters
   */
  if (c.is_fresh()) {
    c.add_output(lpd_file_for_output, "lpe", c.get_output_name(".lpe"));

    c.add_option(option_linearisation_method).
          append_argument(linearisation_method_enumeration, method_selector.get_selection());
      
    c.add_option(option_end_phase).
          append_argument(linearisation_phase_enumeration, phase_selector.get_selection());

    if (clusterfinal->get_status())  {
      c.add_option(option_final_cluster);
    }
    if (noclusterintermediate->get_status()) {
      c.add_option(option_no_intermediate_cluster);
    }
    if (noalpha->get_status()) {
      c.add_option(option_no_alpha);
    }
    if (newstate->get_status()) {
      c.add_option(option_newstate);
    }
    if (binary->get_status()) {
      c.add_option(option_binary);
    }
    if (statenames->get_status()) {
      c.add_option(option_statenames);
    }
    if (norewrite->get_status()) {
      c.add_option(option_no_rewrite);
    }
    if (nofreevars->get_status()) {
      c.add_option(option_no_freevars);
    }
    if (nosumelm->get_status()) {
      c.add_option(option_no_sumelm);
    }
    if (nodeltaelm->get_status()) {
      c.add_option(option_no_deltaelm);
    }
    
    c.add_option(option_report_mode).
                append_argument(report_mode_enumeration, report_selector.get_selection());
  }

  m_communicator.send_clear_display();
}

bool squadt_interactor::check_configuration(sip::configuration const& c) const {
  t_lin_options task_options;

  return (extract_task_options(c, task_options));
}

bool squadt_interactor::extract_task_options(sip::configuration const& c, t_lin_options& task_options) const {
  bool result = true;

  if (c.object_exists(mcrl2_file_for_input)) {
    task_options.infilename = c.get_object(mcrl2_file_for_input)->get_location();
  }
  else {
    send_error("Configuration does not contain an input object\n");

    result = false;
  }

  if (c.object_exists(lpd_file_for_output) ) {
    task_options.outfilename = c.get_object(lpd_file_for_output)->get_location();
  }
  else {
    send_error("Configuration does not contain an output object\n");

    result = false;
  }

  if (c.option_exists(option_linearisation_method)) {
    task_options.lin_method = static_cast < t_lin_method > (boost::any_cast < size_t > (c.get_option_value(option_linearisation_method)));
  }
  else {
    send_error("Configuration does not contain a linearisation method\n");

    result = false;
  } 

  task_options.final_cluster           = c.option_exists(option_final_cluster);
  task_options.no_intermediate_cluster = c.option_exists(option_no_intermediate_cluster);
  task_options.opt_noalpha             = c.option_exists(option_no_alpha);
  task_options.newstate                = c.option_exists(option_newstate);
  task_options.binary                  = c.option_exists(option_binary);
  task_options.statenames              = c.option_exists(option_statenames);
  task_options.norewrite               = c.option_exists(option_no_rewrite);
  task_options.nofreevars              = c.option_exists(option_no_freevars);
  task_options.nosumelm                = c.option_exists(option_no_sumelm);
  task_options.nodeltaelimination      = c.option_exists(option_no_deltaelm);
  
  task_options.opt_end_phase = static_cast < t_phase > (boost::any_cast < size_t > (c.get_option_value(option_end_phase)));

  task_options.opt_check_only = (task_options.opt_end_phase != phNone);
  
  switch (static_cast < report_options > (boost::any_cast < size_t > (c.get_option_value(option_report_mode)))) {
    case report_verbose:
      gsSetVerboseMsg();
      break;
    case report_debug:
      gsSetDebugMsg();
      break;
    case report_normal:
    default:
      break;
  }

  return (result);
}

bool squadt_interactor::perform_task(sip::configuration& c) {
  using namespace boost;
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::datatype;
  using namespace sip::layout::elements;

  bool result = true;

  t_lin_options task_options;
  
  // Extract configuration
  extract_task_options(c, task_options);

  layout::manager::aptr top(layout::vertical_box::create());

  top->add(new label("Linearisation in progress"),layout::left);
  send_display_layout(top);

  // Perform linearisation
  top = layout::vertical_box::create();

  ATermAppl linearisation_result = linearise_file(task_options);
 
  if (linearisation_result == 0) {
    top->add(new label("Linearisation failed"));

    result = false;
  }
  else if (task_options.opt_check_only) {
    top->add(new label(str(format("%s contains a well-formed mCRL2 specification.") % task_options.infilename)));
  }
  else {
    //store the result
    FILE *outstream = fopen(task_options.outfilename.c_str(), "wb");

    if (outstream != 0) {
      ATwriteToBinaryFile((ATerm) linearisation_result, outstream);

      fclose(outstream);
    }
    else {
      send_error(str(format("cannot open output file '%s'\n") % task_options.outfilename));

      result = false;
    }

    if (result) {
      top->add(new label("Linearisation is finished"));
    }
  }

  send_display_layout(top);

  return (result);
}

#endif

using namespace std;

static bool parse_command_line(int argc, char *argv[],t_lin_options &lin_options)
{ 
  //declarations for getopt
  bool lm_chosen = false;
  t_lin_method opt_lin_method = lmRegular;
  bool opt_no_intermediate_cluster = false;
  bool opt_final_cluster = false;
  bool opt_newstate = false;
  bool opt_binary = false;
  bool opt_statenames = false;
  bool opt_noalpha = false;
  bool opt_norewrite = false;
  bool opt_nofreevars = false;
  bool opt_check_only = false;
  bool opt_nosumelm = false;
  bool opt_nodeltaelimination = false;
  t_phase opt_end_phase = phNone;
  #define ShortOptions   "0123cnrwbaofep:hqvdmg"
  #define VersionOption  CHAR_MAX + 1
  struct option LongOptions[] = {
    { "stack",       no_argument,       NULL, '0' },
    { "regular",     no_argument,       NULL, '1' },
    { "regular2",    no_argument,       NULL, '2' },
    { "alternative", no_argument,       NULL, '3' },
    { "cluster",     no_argument,       NULL, 'c' },
    { "no-cluster",  no_argument,       NULL, 'n' },
    { "no-alpha",    no_argument,       NULL, 'r' },
    { "newstate",    no_argument,       NULL, 'w' },
    { "binary",      no_argument,       NULL, 'b' },
    { "statenames",  no_argument,       NULL, 'a' },
    { "no-rewrite",  no_argument,       NULL, 'o' },
    { "no-freevars", no_argument,       NULL, 'f' },
    { "no-sumelm",   no_argument,       NULL, 'm' },
    { "no-deltaelm", no_argument,       NULL, 'g' },
    { "check-only",  no_argument,       NULL, 'e' },
    { "end-phase",   required_argument, NULL, 'p' },
    { "help",        no_argument,       NULL, 'h' },
    { "version",     no_argument,       NULL, VersionOption },
    { "quiet",       no_argument,       NULL, 'q' },
    { "verbose",     no_argument,       NULL, 'v' },
    { "debug",       no_argument,       NULL, 'd' },
    { 0, 0, 0, 0 }
  };
  int Option;
  //parse options
  Option = getopt_long(argc, argv, ShortOptions, LongOptions, NULL);
  while (Option != -1) {
    switch (Option){
      case '0': /* stack */
        if (lm_chosen && opt_lin_method != lmStack) {
          gsErrorMsg("only one method of linearisation is allowed\n");
          return false;
        }
        lm_chosen = true;
        opt_lin_method = lmStack;
        break;
      case '1': /* regular */
        if (lm_chosen && opt_lin_method != lmRegular) {
          gsErrorMsg("only one method of linearisation is allowed\n");
          return false;
        }
        lm_chosen = true;
        opt_lin_method = lmRegular;
        break;
      case '2': /* regular2 */
        if (lm_chosen && opt_lin_method != lmRegular2) {
          gsErrorMsg("only one method of linearisation is allowed\n");
          return false;
        }
        lm_chosen = true;
        opt_lin_method = lmRegular2;
        break;
      case '3': /* alternative */
        if (lm_chosen && opt_lin_method != lmAlternative) {
          gsErrorMsg("only one method of linearisation is allowed\n");
          return false;
        }
        lm_chosen = true;
        opt_lin_method = lmAlternative;
        break;
      case 'c': /* cluster */ 
        opt_final_cluster = true;
        break;
      case 'n': /* no-cluster */
        opt_no_intermediate_cluster = true;
        break;
      case 'r': /* no-alpha */
        opt_noalpha = true;
        break;
      case 'w': /* newstate */ 
        opt_newstate = true;
        break;
      case 'b': /* binary */ 
        opt_binary = true;
        break;
      case 'a': /* statenames */ 
        opt_statenames = true;
        break;
      case 'o': /* no-rewrite */ 
        opt_norewrite = true;
        break;
      case 'f': /* nofreevars */
        opt_nofreevars = true;
        break;
      case 'e': /* check-only */
        opt_check_only = true;
        break;
      case 'm': /* no-sumelm */
        opt_nosumelm = true;
        break;
      case 'g': /* no-deltaelm */
        opt_nodeltaelimination = true;
        break;
      case 'p': /* end-phase */
        if (strcmp(optarg, "pa") == 0) {
          opt_end_phase = phParse;
        } else if (strcmp(optarg, "tc") == 0) {
          opt_end_phase = phTypeCheck;
        } else if (strcmp(optarg, "ar") == 0) {
          opt_end_phase = phAlphaRed;
        } else if (strcmp(optarg, "di") == 0) {
          opt_end_phase = phDataImpl;
        } else {
          gsErrorMsg("option -p has illegal argument '%s'\n", optarg);
          return false;
        }
        break;
      case 'h': /* help */
        PrintHelp(argv[0]);
        return false;
      case VersionOption: /* version */
        PrintVersion();
        return false;
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
        PrintMoreInfo(argv[0]); 
        return false;
    } 
    Option = getopt_long(argc, argv, ShortOptions, LongOptions, NULL);
  }
  //check for dangerous and illegal option combinations
  if (opt_newstate && opt_lin_method == lmStack) {
    gsErrorMsg("option -w can only be used with -1 or -2\n");
    return false;
  }
  if (opt_check_only && (opt_end_phase != phNone)) {
    gsErrorMsg("options -e and -p may not be used in conjunction\n");
    return false;
  }
  if (opt_noalpha && (opt_end_phase == phAlphaRed)) {
    gsErrorMsg("options -r and -p ar may not be used in conjunction\n");
    return false;
  }
  if (opt_lin_method == lmAlternative) {
    if (opt_final_cluster)           AltIllegalOptWarning('c');
    if (opt_no_intermediate_cluster) AltIllegalOptWarning('n');
    if (opt_newstate)                AltIllegalOptWarning('w');
    if (opt_binary)                  AltIllegalOptWarning('b');
    if (opt_statenames)              AltIllegalOptWarning('a');
    if (opt_norewrite)               AltIllegalOptWarning('n');
    if (opt_nofreevars)              AltIllegalOptWarning('f');
  }
  //check for wrong number of arguments
  string infilename;
  string outfilename;
  int noargc; //non-option argument count
  noargc = argc - optind;
  if (noargc > 2) {
    fprintf(stderr, "%s: too many arguments\n", NAME);
    PrintMoreInfo(argv[0]);
    return false;
  } else {
    //noargc >= 0 && noargc <= 2
    if (noargc > 0) {
      infilename = argv[optind];
    }
    if (noargc == 2) {
      outfilename = argv[optind + 1];
    }
  }

  //set linearisation parameters
  lin_options.lin_method = opt_lin_method;
  lin_options.final_cluster = opt_final_cluster;
  lin_options.no_intermediate_cluster = opt_no_intermediate_cluster;
  lin_options.newstate = opt_newstate;
  lin_options.binary = opt_binary;
  lin_options.statenames = opt_statenames;
  lin_options.norewrite = opt_norewrite;
  lin_options.nofreevars = opt_nofreevars;
  lin_options.nosumelm = opt_nosumelm;
  lin_options.nodeltaelimination = opt_nodeltaelimination;
  lin_options.opt_check_only = opt_check_only;
  lin_options.opt_end_phase = opt_end_phase;
  lin_options.opt_noalpha = opt_noalpha;
  lin_options.infilename = infilename;
  lin_options.outfilename = outfilename;

  return true;  // main can continue
}

ATermAppl linearise_file(t_lin_options &lin_options)
{
  ATermAppl result = NULL;
  t_phase end_phase = lin_options.opt_check_only?phTypeCheck:lin_options.opt_end_phase;
  //parse specification
  if (lin_options.infilename == "") {
    //parse specification from stdin
    gsVerboseMsg("parsing input from stdin...\n");
    result = parse_spec(cin);
  } else {
    //parse specification from infilename
    ifstream instream(lin_options.infilename.c_str(), ifstream::in|ifstream::binary);
    if (!instream.is_open()) {
      gsErrorMsg("cannot open input file '%s'\n", lin_options.infilename.c_str());
      return NULL;
    }
    gsVerboseMsg("parsing input file '%s'...\n", lin_options.infilename.c_str());
    result = parse_spec(instream);
    instream.close();
  }
  if (result == NULL) 
  {
    gsErrorMsg("parsing failed\n");
    return NULL;
  }

  if (end_phase == phParse) {
    return result;
  }
  //type check the result
  gsVerboseMsg("type checking...\n");
  result = type_check_spec(result);
  if (result == NULL) 
  {
    gsErrorMsg("type checking failed\n");
    return NULL;
  }
  if (end_phase == phTypeCheck) {
    return result;
  }
  //perform alphabet reductions 
  if (!lin_options.opt_noalpha) {
    gsVerboseMsg("performing alphabet reductions...\n");
    result = gsAlpha(result);
    if (result == NULL)
    {
      gsErrorMsg("alphabet reductions failed\n");
      return NULL;
    }
    if (end_phase == phAlphaRed) {
      return result;
    }
  }
  //implement standard data types and type constructors on the result
  gsVerboseMsg("implementing standard data types and type constructors...\n");
  result = implement_data_spec(result);
  if (result == NULL) 
  {
    gsErrorMsg("data implementation failed\n");
    return NULL;
  }
  if (end_phase == phDataImpl) {
    return result;
  }
  //linearise the result
  gsVerboseMsg("linearising processes using the %s method\n", lin_method_to_string(lin_options.lin_method));

  if (lin_options.lin_method != lmAlternative) {
    result = linearise_std(result, lin_options);
  } else { //lin_options.lin_method == lmAlternative
    result = linearise_alt(result, lin_options);
  }
  if (result == NULL) 
  {
    gsErrorMsg("linearisation failed\n");
    return NULL;
  }
  return result; 
}

inline char const* lin_method_to_string(t_lin_method lin_method)
{
  static const char* method[] = {"stack","regular","regular2","alternative"};

  return (method[lin_method]);
}

void AltIllegalOptWarning(char opt)
{
  gsWarningMsg(
    "option -%c is not supported by linearisation method -3, ignored\n", opt);
}

void PrintMoreInfo(char *Name)
{
  fprintf(stderr, "Use %s --help for options\n", Name);
}

void PrintVersion(void)
{
  fprintf(stderr,"%s %s (revision %d)\n", NAME, VERSION, REVISION);
}

void PrintHelp(char *Name)
{
  fprintf(stderr,
    "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
    "Linearises the mCRL2 specification in INFILE and writes the resulting LPE to\n"
    "OUTFILE. If OUTFILE is not present, stdout is used. If INFILE is not present,\n"
    "stdin is used.\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -0, --stack           the LPE is generated using stack datatypes;\n"
    "                        useful when -1 and -2 do not work\n"
    "  -1, --regular         if the specification is regular, the LPE is generated\n"
    "                        in regular form (default)\n"
    "  -2, --regular2        a variant of regular that uses more data variables;\n"
    "                        sometimes successful when -1 leads to non-termination\n"
    "  -3, --alternative     more general method of linearisation that can handle\n"
    "                        a wider range of specifications; currently it is unable\n"
    "                        to handle time and does not accept the -c to -f options\n"
    "  -c, --cluster         all actions in the final LPE are clustered\n"
    "  -n, --no-cluster      the actions in intermediate LPEs are not clustered\n"
    "                        (default behaviour is that intermediate LPEs are\n"
    "                        clustered and the final LPE is not clustered)\n"
    "  -r, --no-alpha        alphabet reductions are not applied\n"
    "  -w, --newstate        state variables are encoded using enumerated types\n"
    "                        (requires -1 or -2); without -w numbers are used\n"
    "  -b, --binary          when clustering use binary case functions instead of\n"
    "                        n-ary; in the presence of -w, state variables are\n"
    "                        encoded by a vector of boolean variables\n"
    "  -a, --statenames      the names of state variables are derived from the\n"
    "                        specification\n"
    "  -o, --no-rewrite      do not rewrite data terms while linearising;\n"
    "                        useful when the rewrite system does not terminate\n"
    "  -f, --no-freevars     instantiate don't care values with arbitrary constants,\n"
    "                        instead of modelling them by free variables\n"
    "  -m, --no-sumelm       avoid applying sum elimination in parallel composition\n"
    "  -g, --no-deltaelm     avoid removing spurious delta summands\n"
    "  -e, --check-only      check syntax and static semantics; do not linearise\n"
    "  -p, --end-phase=PHASE stop linearisation after phase PHASE and output the\n"
    "                        result; PHASE can be 'pa' (parse), 'tc' (type check),\n"
    "                        'ar' (alphabet reduction) or 'di' (data implementation)\n"
    "  -h, --help            display this help and terminate\n"
    "      --version         display version information and terminate\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         display concise intermediate messages\n"
    "  -d, --debug           display detailed intermediate messages\n",
    Name);
}

// Main 

int main(int argc, char *argv[])
{
  //initialise ATerm library
  ATerm         stack_bottom;
  t_lin_options lin_options;

  ATinit(argc,argv,&stack_bottom);

  //enable constructor functions
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  squadt_interactor c;

  if (!c.try_interaction(argc, argv)) {
#endif
    if (parse_command_line(argc,argv,lin_options)) {
      //linearise infilename with options lin_options
      ATermAppl result = linearise_file(lin_options);

      if (result == NULL) {
        return 1;
      }
      if (lin_options.opt_check_only) {
        if (lin_options.infilename == "") {
          fprintf(stdout, "stdin");
        } else {
          fprintf(stdout, "The file '%s'", lin_options.infilename.c_str());
        }
        fprintf(stdout, " contains a well-formed mCRL2 specification.\n");
        return 0;
      } else {
        //store the result
        if (lin_options.outfilename == "") {
          gsVerboseMsg("saving result to stdout...\n");
          ATwriteToBinaryFile((ATerm) result, stdout);
          fprintf(stdout, "\n");
        } else { //outfilename != NULL
          //open output filename
          FILE *outstream = fopen(lin_options.outfilename.c_str(), "wb");
          if (outstream == NULL) {
            gsErrorMsg("cannot open output file '%s'\n", lin_options.outfilename.c_str());
            return 1;
          }
          gsVerboseMsg("saving result to '%s'...\n", lin_options.outfilename.c_str());
          ATwriteToBinaryFile((ATerm) result, outstream);
          fclose(outstream);
        }
      }
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
