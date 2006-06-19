#define NAME "lpe2pbes"
#define VERSION "0.2"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include <assert.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint.h"
#include "libprint_c.h"
#include "libparse.h"
#include "typecheck.h"
#include "dataimpl.h"
#include "lpe/specification.h"

#include "mcrl2_revision.h"

using namespace std;

//Type definitions
//t_tool_options represents the options of the tool 
typedef struct {
  bool pretty;
  string formfilename;
  string infilename;
  string outfilename;
} t_tool_options;

//Functions used by the main program
static t_tool_options parse_command_line(int argc, char **argv);
//Post: The command line options are parsed.
//      The program has aborted with a suitable error code, if:
//      - errors were encountered
//      - non-standard behaviour was requested (help or version)
//Ret:  the parsed command line options

static void print_help(char *Name);
static void print_version(void);
static void print_more_info(char *Name);

//Main program

int main(int argc, char **argv)
{
  //parse command line
  t_tool_options tool_options = parse_command_line(argc, argv);
  string infilename = tool_options.infilename;
  string outfilename = tool_options.outfilename;
  string formfilename = tool_options.formfilename;
  bool opt_pretty = tool_options.pretty;

  //initialise ATerm library
  ATerm stackbot;
  ATinit(argc,argv,&stackbot);
  gsEnableConstructorFunctions();

  //open infilename
  lpe::specification lpe_spec = lpe::specification();
  if (infilename == "") {
    if (!lpe_spec.load("-")) {
      gsErrorMsg("cannot open LPE from stdin\n");
      return 1;
    }
  } else {
    if (!lpe_spec.load(infilename)) {
      gsErrorMsg("cannot open LPE from '%s'\n", infilename.c_str());
      return 1;
    }
  }

  //check if lpe_spec is an LPE
  //XXX need soundness check

  //parse formula from formfilename
  gsVerboseMsg("parsing formula from '%s'...\n", formfilename.c_str());
  ifstream formstream(formfilename.c_str(), ifstream::in|ifstream::binary);
  if (!formstream.is_open()) {
    gsErrorMsg("cannot open formula file '%s'\n", formfilename.c_str());
    return 1;
  }
  ATermAppl result = parse_state_formula(formstream);
  formstream.close();
  if (result == NULL) {
    gsErrorMsg("parsing failed\n");
    return 1;
  }

  //type check formula
  gsVerboseMsg("type checking...\n");
  result = type_check_state_formula(result, lpe_spec);
  if (result == NULL) {
    gsErrorMsg("type checking failed\n");
    return 1;
  }

  //implement standard data types and type constructors on the result
  gsVerboseMsg("implementing standard data types and type constructors...\n");
  result = implement_data_state_formula(result, lpe_spec);
  if (result == NULL) {
    gsErrorMsg("data implementation failed\n");
    return 1;
  }

  //store the result
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
  bool opt_pretty = false;
  string formfilename = "";
  #define SHORT_OPTIONS "f:ehqvd"
  #define VERSION_OPTION CHAR_MAX + 1
  struct option long_options[] = {
    { "formula",   required_argument,  NULL,  'f' },
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
  tool_options.pretty       = opt_pretty;
  tool_options.formfilename = formfilename;
  tool_options.infilename   = infilename;
  tool_options.outfilename  = outfilename;
  return tool_options;
}

static void print_help(char *Name)
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
    "  -e, --external        return the result in the external format\n"
    "  -h, --help            display this help message and terminate\n"
    "      --version         display version information and terminate\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         display concise intermediate messages\n"
    "  -d, --debug           display detailed intermediate messages\n",
    Name
  );
}

void print_version(void)
{
  fprintf(stderr,"%s %s (revision %d)\n", NAME, VERSION, REVISION);
}

void print_more_info(char *Name)
{
  fprintf(stderr, "Use %s --help for options\n", Name);
}
