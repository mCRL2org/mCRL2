#define NAME "lpe2pbes"
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
#include "lpe/specification.h"
#include "lpe/mucalculus.h"
#include "lpe/pbes_translate.h"
#include "lpe/pbes.h"
#include "libpbes.h"

using namespace std;
using namespace lpe;

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
//      tool_options.infilename contains an LPE ("" indicates stdin)
//      tool_options.end_phase indicates at which phase conversion stops
//Ret:  if end_phase == PH_NONE, the PBES generated from the state formula and
//      the LPE
//      if end_phase != PH_NONE, the state formula after phase end_phase
//      NULL, if something went wrong

static void print_help(char *name);
static void print_version(void);
static void print_more_info(char *name);

//Main program
//------------

int main(int argc, char **argv)
{
  //parse command line
  t_tool_options tool_options = parse_command_line(argc, argv);

  //initialise ATerm library
  ATerm stackbot;
  ATinit(argc,argv,&stackbot);
  gsEnableConstructorFunctions();

  //process state formula
  ATermAppl result = create_pbes(tool_options);
  if (result == NULL) {
    return 1;
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
      return 1;
    }
    PrintPart_CXX(outstream, (ATerm) result, opt_pretty?ppDefault:ppInternal);
    outstream.close();
  }
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
  #define SHORT_OPTIONS "f:p:ehqvd"
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
  lpe::specification lpe_spec = lpe::specification();
  if (infilename == "") {
    if (!lpe_spec.load("-")) {
      gsErrorMsg("cannot open LPE from stdin\n");
      return NULL;
    }
  } else {
    if (!lpe_spec.load(infilename)) {
      gsErrorMsg("cannot open LPE from '%s'\n", infilename.c_str());
      return NULL;
    }
  }

  //check if lpe_spec is an LPE
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
  result = type_check_state_frm(result, lpe_spec);
  if (result == NULL) {
    gsErrorMsg("type checking failed\n");
    return NULL;
  }
  if (end_phase == PH_TYPE_CHECK) {
    return result;
  }

  //implement standard data types and type constructors on the result
  gsVerboseMsg("implementing standard data types and type constructors...\n");
  result = implement_data_state_frm(result, lpe_spec);
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

  //generate PBES from state formula and LPE
  gsVerboseMsg("generating PBES from state formula and LPE...\n");
  pbes p = lpe::pbes_translate(state_formula(result), lpe_spec, tool_options.untimed);
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
    "Convert the state formula in FILE and the LPE in INFILE to a parameterised\n"
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
    "  -u, --untimed         apply special conversion for untimed LPE's\n"
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
  fprintf(stderr,"%s %s (revision %d)\n", NAME, VERSION, REVISION);
}

void print_more_info(char *name)
{
  fprintf(stderr, "Use %s --help for options\n", name);
}
