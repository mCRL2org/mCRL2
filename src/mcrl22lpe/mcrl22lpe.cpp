// #define ENABLE_SQUADT_CONNECTIVITY


#define NAME "mcrl22lpe"
#define VERSION "0.2.1"
#define INFILEEXT ".mcrl2"
#define OUTFILEEXT ".lpe"

#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <sip/tool.h>
#endif

#include <assert.h>
#include <stdbool.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <aterm2.h>
#include <string.h>
#include <cstdio>
#include <fstream>
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

using namespace std;

//Functions used by the main program
static ATermAppl linearise_file(t_lin_options &lin_options);
static void AltIllegalOptWarning(char opt);
static void PrintMoreInfo(char *Name);
static void PrintVersion(void);
static void PrintHelp(char *Name);
static void PrintLinMethod(FILE *stream, t_lin_method lin_method);

static int parse_command_line(int argc, char *argv[],t_lin_options &lin_options)
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
          return 1;
        }
        lm_chosen = true;
        opt_lin_method = lmStack;
        break;
      case '1': /* regular */
        if (lm_chosen && opt_lin_method != lmRegular) {
          gsErrorMsg("only one method of linearisation is allowed\n");
          return 1;
        }
        lm_chosen = true;
        opt_lin_method = lmRegular;
        break;
      case '2': /* regular2 */
        if (lm_chosen && opt_lin_method != lmRegular2) {
          gsErrorMsg("only one method of linearisation is allowed\n");
          return 1;
        }
        lm_chosen = true;
        opt_lin_method = lmRegular2;
        break;
      case '3': /* alternative */
        if (lm_chosen && opt_lin_method != lmAlternative) {
          gsErrorMsg("only one method of linearisation is allowed\n");
          return 1;
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
          return 1;
        }
        break;
      case 'h': /* help */
        PrintHelp(argv[0]);
        return 1;
      case VersionOption: /* version */
        PrintVersion();
        return 1;
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
        return 1;
    } 
    Option = getopt_long(argc, argv, ShortOptions, LongOptions, NULL);
  }
  //check for dangerous and illegal option combinations
  if (opt_newstate && opt_lin_method == lmStack) {
    gsErrorMsg("option -w can only be used with -1 or -2\n");
    return 1;
  }
  if (opt_check_only && (opt_end_phase != phNone)) {
    gsErrorMsg("options -e and -p may not be used in conjunction\n");
    return 1;
  }
  if (opt_noalpha && (opt_end_phase == phAlphaRed)) {
    gsErrorMsg("options -r and -p ar may not be used in conjunction\n");
    return 1;
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
    return 1;
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
  return 0;  // main can continue
}

#ifdef ENABLE_SQUADT_CONNECTIVITY

static bool get_squadt_parameters(int argc, 
                                  char *argv[],
                                  t_lin_options &lin_options,
                                  sip::tool::communicator &tc)
{
  std::string infilename;
  const unsigned int lpd_file_for_input=0;

  sip::tool::capabilities& cp = tc.get_tool_capabilities();
  cp.add_input_combination(lpd_file_for_input, "Transformation", "mcrl2");
  if (tc.activate(argc,argv)) 
  { bool valid = false;
    /* Static configuration cycle */
    while (!valid) 
    {
      /* Wait for configuration data to be sent 
       * (either a previous configuration, or only an input combination) */
      sip::configuration::sptr configuration = tc.await_configuration();
      /* Validate configuration specification, 
       * should contain a file name of an LPD that is to be read as input */
      valid  = configuration.get() != 0;
      valid &= configuration->object_exists(lpd_file_for_input);
      if (valid) 
      { /* An object with the correct id exists, assume the URI is relative 
           (i.e. a file name in the local file system) */
        infilename = configuration->get_object(lpd_file_for_input)->get_location();
        tc.set_configuration(configuration);
      }
      else 
      { sip::report report;
        report.set_error("Invalid input combination!");
        tc.send_report(report);
        exit(1);
      }
    }

    using namespace sip;
    using namespace sip::layout;
    using namespace sip::layout::elements;

    tool_display::sptr display(new layout::tool_display);
    layout::manager::aptr layout_manager = layout::vertical_box::create();

    layout_manager->add(new label(" "),layout::left);
    // box to select the outputfile
    layout::horizontal_box* outputfilename_box = new layout::horizontal_box();
    layout_manager->add(outputfilename_box,layout::left);
    outputfilename_box->add(new label("Output file name: "),middle);
    assert(infilename.size()>6);  // infilename has the shape "... .mcrl2"
    std::string suggestedoutfilename(infilename,0,infilename.size()-6);
    suggestedoutfilename=suggestedoutfilename + ".lpe";
    text_field* outfilenamefield=new text_field(suggestedoutfilename);
    outputfilename_box->add(outfilenamefield,middle);
    // outputfilename_box->add(new label(suggestedoutfilename),middle);

    layout_manager->add(new label(" "),layout::left);

    // box to select the linearisation method
    horizontal_box* linearisation_method_box = new horizontal_box();
    layout_manager->add(linearisation_method_box);
    linearisation_method_box->add(new label("Linearisation method: "),middle);
    radio_button *select_regular=new radio_button("Regular");
    linearisation_method_box->add(select_regular,middle);
    radio_button *select_regular2=new radio_button("Regular2",select_regular);
    linearisation_method_box->add(select_regular2,middle);
    radio_button *select_stack=new radio_button("Stack",select_regular2);
    linearisation_method_box->add(select_stack,middle);
    radio_button *select_expansion=new radio_button("Expansion",select_stack);
    linearisation_method_box->add(select_expansion,middle);

    layout_manager->add(new label(" "),layout::left);
    // two columns to select the linearisation options of the tool
    layout::horizontal_box* option_columns = new layout::horizontal_box();
    layout_manager->add(option_columns);
    // left option column
    vertical_box* left_option_column = new vertical_box();
    option_columns->add(left_option_column,top);
    checkbox* clusterintermediate = new checkbox("Intermediate clustering",true);
    left_option_column->add(clusterintermediate,layout::left);
    checkbox* clusterfinal = new checkbox("Final clustering",true);
    left_option_column->add(clusterfinal,layout::left);
    checkbox* newstate = new checkbox("Use enumerated states",false);
    left_option_column->add(newstate,layout::left);
    checkbox* binary = new checkbox("Encode enumerated types by booleans ",false);
    left_option_column->add(binary,layout::left);
    checkbox* statenames = new checkbox("Use informative statenames ",false);
    left_option_column->add(statenames,layout::left);
    // right option column
    layout::vertical_box* right_option_column = new layout::vertical_box();
    option_columns->add(right_option_column,top);
    checkbox* norewrite = new checkbox("Do not rewrite",true);
    right_option_column->add(norewrite,layout::left);
    checkbox* noalpha = new checkbox("Do not apply alphabet axioms",false);
    right_option_column->add(noalpha,layout::left);
    checkbox* nosumelm = new checkbox("Do not apply sum elimination",false);
    right_option_column->add(nosumelm,layout::left);
    checkbox* nodeltaelm = new checkbox("Do not apply delta elimination",false);
    right_option_column->add(nodeltaelm,layout::left);
    checkbox* nofreevars = new checkbox("Suppress generating free variables",false);
    right_option_column->add(nofreevars,layout::left);

    layout_manager->add(new label(" "),layout::left);
    layout_manager->add(new label("Determine output phase"),layout::left);
    // Determine which phases the linearizer will go through. Default is all.
    horizontal_box *phases_box = new horizontal_box();
    layout_manager->add(phases_box);
    radio_button *all_phases=new radio_button("None");
    phases_box->add(all_phases,middle);
    radio_button *parse_phase=new radio_button("Parsing",all_phases);
    phases_box->add(parse_phase,middle);
    radio_button *typecheck_phase=new radio_button("Typechecking",parse_phase);
    phases_box->add(typecheck_phase,middle);
    radio_button *alpha_phase=new radio_button("Alphabet reductions",typecheck_phase);
    phases_box->add(alpha_phase,middle);
    radio_button *data_phase=new radio_button("Data implementation",alpha_phase);
    phases_box->add(data_phase,middle);


    // The ok button must be put at the rightmost lowermost place

    horizontal_box* okay_box = new horizontal_box();
    layout_manager->add(okay_box,layout::right);
    button* okay_button = new button("OK");
    okay_box->add(okay_button,layout::bottom);
    
    layout_manager->add(new label(" "),layout::left);

    cerr << "Configuration sent\n";
    display->set_top_manager(layout_manager);

    tc.send_display_layout(display);
    cerr << "Configuration sent1\n";

    okay_button->await_change();

    lin_options.infilename=infilename;

    if (select_regular->is_selected())
    { lin_options.lin_method = lmRegular;
    }
    else if (select_regular2->is_selected())
    { lin_options.lin_method = lmRegular2;
    }
    else if (select_stack->is_selected())
    { lin_options.lin_method = lmStack;
    }
    else if (select_expansion->is_selected())
    { lin_options.lin_method = lmAlternative;
    }
    lin_options.final_cluster = clusterfinal->get_status();
    lin_options.no_intermediate_cluster = clusterintermediate->get_status();
    lin_options.newstate = newstate->get_status();
    lin_options.binary = binary->get_status();
    lin_options.statenames = statenames->get_status();
    lin_options.norewrite = norewrite->get_status();
    lin_options.nofreevars = nofreevars->get_status();
    lin_options.nosumelm = nosumelm->get_status();
    lin_options.nodeltaelimination = nodeltaelm->get_status();
    
    lin_options.opt_check_only = !(all_phases->is_selected());
    
    if (lin_options.opt_check_only)
    { if (parse_phase->is_selected())
      { lin_options.opt_end_phase = phParse;
      }
      else if (typecheck_phase->is_selected())
      { lin_options.opt_end_phase = phTypeCheck;
      }
      else if (alpha_phase->is_selected())
      { lin_options.opt_end_phase = phAlphaRed;
      }
      else if (data_phase->is_selected())
      { lin_options.opt_end_phase = phDataImpl;
      }
    }
    else
    { lin_options.opt_end_phase=phNone;
    }

        
    // lin_options.opt_end_phase = opt_end_phase;
    lin_options.opt_noalpha = noalpha->get_status();
    lin_options.infilename = infilename;
    lin_options.outfilename=outfilenamefield->get_text();


    cerr << "Configuration sent2\n";
    /* Send the controller the signal that we're ready to rumble 
     * (no further configuration necessary) */
    tc.send_accept_configuration();

    /* Wait for start message */
    
    cerr << "Ready to accept configuration " << infilename << "\n";
    tc.await_message(sip::send_signal_start);


    return 0;
  }
  // else squadt is not active, so parse the commandline
  return parse_command_line(argc,argv,lin_options);
}
#endif


// Main 

int main(int argc, char *argv[])
{
  bool terminate=0;
  t_lin_options lin_options;
#ifdef ENABLE_SQUADT_CONNECTIVITY
  sip::tool::communicator tc;
  terminate=get_squadt_parameters(argc,argv,lin_options,tc);
#else
  terminate=parse_command_line(argc,argv,lin_options);
#endif

  if (terminate) exit(0);

  //initialise ATerm library
  ATerm stack_bottom;
  ATinit(argc,argv,&stack_bottom);
  //enable constructor functions
  gsEnableConstructorFunctions();

  //linearise infilename with options lin_options
  ATermAppl result =linearise_file(lin_options);

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
  return 0;
}

ATermAppl linearise_file(t_lin_options &lin_options)
{
  ATermAppl result = NULL;
  t_phase end_phase = lin_options.opt_check_only?phTypeCheck:lin_options.opt_end_phase;
  //parse specification
  if (lin_options.infilename == "") {
    //parse specification from stdin
    gsVerboseMsg("parsing input from stdin...\n");
    result = parse_specification(cin);
  } else {
    //parse specification from infilename
    ifstream instream(lin_options.infilename.c_str(), ifstream::in|ifstream::binary);
    if (!instream.is_open()) {
      gsErrorMsg("cannot open input file '%s'\n", lin_options.infilename.c_str());
      return NULL;
    }
    gsVerboseMsg("parsing input file '%s'...\n", lin_options.infilename.c_str());
    result = parse_specification(instream);
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
  result = gsTypeCheck(result);
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
  result = gsImplementData(result);
  if (result == NULL) 
  {
    gsErrorMsg("data implementation failed\n");
    return NULL;
  }
  if (end_phase == phDataImpl) {
    return result;
  }
  //linearise the result
  if (gsVerbose) {
    fprintf(stderr, "linearising processes using the ");
    PrintLinMethod(stderr, lin_options.lin_method);
    fprintf(stderr, " method...\n");
  }
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

void AltIllegalOptWarning(char opt)
{
  gsWarningMsg(
    "option -%c is not supported by linearisation method -3, ignored\n", opt);
}

void PrintLinMethod(FILE *stream, t_lin_method lin_method)
{
  if (lin_method == lmStack) {
    fprintf(stream, "stack");
  } else if (lin_method == lmRegular) {
    fprintf(stream, "regular");
  } else if (lin_method == lmRegular2) {
    fprintf(stream, "regular2");
  } else if (lin_method == lmAlternative) {
    fprintf(stream, "alternative");
  }
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
