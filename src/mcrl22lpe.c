#ifdef __cplusplus
extern "C" {
#endif

#define NAME "mcrl22lpe"
#define VERSION "0.1.0"
#define INFILEEXT ".mcrl2"
#define OUTFILEEXT ".lpe"

#include <assert.h>
#include <stdbool.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <aterm2.h>
#include <string.h>
#include "lin_types.h"
#include "lin_std.h"
#include "lin_alt.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libprint_c.h"
#include "gslexer.h"
#include "gstypecheck.h"
#include "gsdataimpl.h"

//Type definitions

typedef enum { phNone, phParse, phTypeCheck, phDataImpl } t_phase;
//t_phase represents the phases at which the program should be able to stop

//Functions used by the main program
static ATermAppl linearise_file(char *infilename, t_lin_options lin_options,
  t_phase end_phase);
static void calc_infilename(char *infilename, char *specname);
static void calc_outfilename(char *outfilename, char *infilename);
static void AltIllegalOptWarning(char opt);
static void PrintMoreInfo(char *Name);
static void PrintVersion(void);
static void PrintHelp(char *Name);
static void PrintLinMethod(FILE *stream, t_lin_method lin_method);

//Main program

int main(int argc, char *argv[])
{ 
  //declarations for getopt
  bool lm_chosen = false;
  t_lin_method opt_lin_method = lmRegular;
  t_cluster_method opt_cluster_method = cmDefault;
  bool opt_newstate = false;
  bool opt_binary = false;
  bool opt_statenames = false;
  bool opt_norewrite = false;
  bool opt_nofreevars = false;
  bool opt_check_only = false;
  t_phase opt_end_phase = phNone;
  bool opt_stdout = false;
  #define ShortOptions   "0123cnwbaofep:hqvd"
  #define StdOutOption   CHAR_MAX + 1
  #define VersionOption  StdOutOption + 1
  struct option LongOptions[] = {
    { "stack",       no_argument,       NULL, '0' },
    { "regular",     no_argument,       NULL, '1' },
    { "regular2",    no_argument,       NULL, '2' },
    { "alternative", no_argument,       NULL, '3' },
    { "cluster",     no_argument,       NULL, 'c' },
    { "no-cluster",  no_argument,       NULL, 'n' },
    { "newstate",    no_argument,       NULL, 'w' },
    { "binary",      no_argument,       NULL, 'b' },
    { "statenames",  no_argument,       NULL, 'a' },
    { "no-rewrite",  no_argument,       NULL, 'o' },
    { "no-freevars", no_argument,       NULL, 'f' },
    { "check-only",  no_argument,       NULL, 'e' },
    { "end-phase",   required_argument, NULL, 'p' },
    { "stdout",      no_argument,       NULL, StdOutOption },
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
        if (opt_cluster_method == cmNone) {
          gsErrorMsg("only one clustering method is allowed\n");
          return 1;
        }
        opt_cluster_method = cmFull;
        break;
      case 'n': /* nocluster */
        if (opt_cluster_method == cmFull) {
          gsErrorMsg("only one clustering method is allowed\n");
          return 1;
        }
        opt_cluster_method = cmNone;
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
      case 'p': /* end-phase */
        if (strcmp(optarg, "pa") == 0) {
          opt_end_phase = phParse;
        } else if (strcmp(optarg, "tc") == 0) {
          opt_end_phase = phTypeCheck;
        } else if (strcmp(optarg, "di") == 0) {
          opt_end_phase = phDataImpl;
        } else {
          gsErrorMsg("option -p has illegal argument '%s'\n", optarg);
          return 1;
        }
        break;
      case StdOutOption:  /* stdout */
        opt_stdout = true;
        break;
      case 'h': /* help */
        PrintHelp(argv[0]);
        return 0;
      case VersionOption: /* version */
        PrintVersion();
        return 0;
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
  if (opt_lin_method == lmAlternative) {
    if (opt_cluster_method == cmFull) AltIllegalOptWarning('c');
    if (opt_cluster_method == cmNone) AltIllegalOptWarning('n');
    if (opt_newstate)                 AltIllegalOptWarning('w');
    if (opt_binary)                   AltIllegalOptWarning('b');
    if (opt_statenames)               AltIllegalOptWarning('a');
    if (opt_norewrite)                AltIllegalOptWarning('n');
    if (opt_nofreevars)               AltIllegalOptWarning('f');
  }
  //check for wrong number of arguments
  int noargc; //non-option argument count
  noargc = argc - optind;
  if (noargc <= 0) {
    fprintf(stderr, "%s: too few arguments\n", NAME);
    PrintMoreInfo(argv[0]);
    return 1;
  } else if (noargc > 1) {
    fprintf(stderr, "%s: too many arguments\n", NAME);
    PrintMoreInfo(argv[0]);
    return 1;
  }
  assert(noargc == 1);

  //initialise ATerm library
  ATerm stack_bottom;
  ATinit(argc,argv,&stack_bottom);
  //enable constructor functions
  gsEnableConstructorFunctions();

  //determine input filename
  char infilename[strlen(argv[optind]) + strlen(INFILEEXT) + 1];
  calc_infilename(infilename, argv[optind]);

  //set linearisation parameters
  t_lin_options lin_options;
  lin_options.lin_method = opt_lin_method;
  lin_options.cluster_method = opt_cluster_method;
  lin_options.newstate = opt_newstate;
  lin_options.binary = opt_binary;
  lin_options.statenames = opt_statenames;
  lin_options.norewrite = opt_norewrite;
  lin_options.nofreevars = opt_nofreevars;

  //linearise infilename with options lin_options
  ATermAppl result =
    linearise_file(infilename, lin_options,
      opt_check_only?phTypeCheck:opt_end_phase);
  if (result == NULL) {
    return 1;
  }
  if (opt_check_only) {
    fprintf(stdout,
      "The file %s contains a well-formed mCRL2 specification.\n",
      infilename);
    return 0;
  } else {
    //store the result
    if (opt_stdout) {
      gsVerboseMsg("saving result to stdout...\n");
      ATwriteToTextFile((ATerm) result, stdout);
      fprintf(stdout, "\n");
    } else { //!opt_stdout
      //determine output filename
      char outfilename[strlen(infilename) + strlen(OUTFILEEXT) + 1];
      calc_outfilename(outfilename, infilename);
      //open output filename
      FILE *outstream = fopen(outfilename, "w");
      if (outstream == NULL) {
        gsErrorMsg("cannot open output file '%s'\n", outfilename);
        return 1;
      }
      gsVerboseMsg("saving result to '%s'...\n", outfilename);
      ATwriteToTextFile((ATerm) result, outstream);
      fclose(outstream);
    }
  }
  return 0;
}

ATermAppl linearise_file(char *infilename, t_lin_options lin_options,
  t_phase end_phase)
{
  assert(infilename != NULL);
  //open input filename
  FILE *instream = fopen(infilename, "r");
  if (instream == NULL) {
    gsErrorMsg("cannot open input file '%s'\n", infilename);
    return NULL;
  }
  //parse specification from instream
  gsVerboseMsg("parsing input file '%s'...\n", infilename);
  ATermAppl result = gsParse(instream);
  fclose(instream);
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

void calc_infilename(char *infilename, char *specname)
{
  //Pre : infilename is able to store specname appended with INFILEEXT
  //Post: infilename represents specname, in which INFILEEXT is appended
  //      if it didn't already end with it
  strcpy(infilename, specname);
  char *file_ext = strrchr(infilename, '.');
  if (file_ext == NULL) {
    //'.' does not occur in infilename, append INFILEEXT 
    strcat(infilename, INFILEEXT);
  } else { //file_ext != NULL
    if (strcmp(file_ext, INFILEEXT) != 0) {
      //file_ext is not equal to INFILEEXT, append INFILEEXT
      strcat(infilename, INFILEEXT);
    }
  }
}

void calc_outfilename(char *outfilename, char *infilename)
{
  //Pre: infilename ends with INFILEEXT
  strcpy(outfilename, infilename);
  //replace suffix INFILEEXT by OUTFILEEXT
  outfilename[strlen(outfilename) - strlen(INFILEEXT)] = '\0';
  strcat(outfilename, OUTFILEEXT);
  //strip path
  char *no_path = strrchr(outfilename, '/');
  if (no_path != NULL) {
    strcpy(outfilename, no_path + 1);
  }
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
  fprintf(stderr,"%s version %s\n", NAME, VERSION);
}

void PrintHelp(char *Name)
{
  fprintf(stderr,
    "Usage: %s OPTIONS SPECFILE\n"
    "Linearises the mCRL2 specification in SPECFILE and writes the resulting LPE to\n"
    "a file. If SPECFILE does not have the extension 'mcrl2', SPECFILE.mcrl2 is used.\n"
    "In the name of the output file, the extension 'mcrl2' is replaced by 'lpe'.\n"
    "\n"
    "The OPTIONS that can be used are:\n"
    "  -0, --stack            the LPE is generated using stack datatypes;\n"
    "                         useful when -1 and -2 do not work\n"
    "  -1, --regular          if the specification is regular, the LPE is generated\n"
    "                         in regular form (default)\n"
    "  -2, --regular2         a variant of regular that uses more data variables;\n"
    "                         sometimes successful when -1 leads to non-termination\n"
    "  -3, --alternative      more general method of linearisation that can handle\n"
    "                         a wider range of specifications; currently it is unable\n"
    "                         to handle time and does not accept the -c to -f options\n"
    "  -c, --cluster          all actions in the LPE are clustered\n"
    "  -n, --no-cluster       no actions are clustered, not even in intermediate LPEs\n"
    "  -w, --newstate         state variables are encoded using enumerated types\n"
    "                         (requires -1 or -2); without -w numbers are used\n"
    "  -b, --binary           when clustering use binary case functions instead of\n"
    "                         n-ary; in the presence of -w, state variables are\n"
    "                         encoded by a vector of boolean variables\n"
    "  -a, --statenames       the names of state variables are derived from the\n"
    "                         specification\n"
    "  -o, --no-rewrite       do not rewrite data terms while linearising;\n"
    "                         useful when the rewrite system does not terminate\n"
    "  -f, --no-freevars      instantiate don't care values with arbitrary constants,\n"
    "                         instead of modelling them by free variables\n"
    "  -e  --check-only       check syntax and static semantics; do not linearise\n"
    "  -p  --end-phase=PHASE  stop linearisation after phase PHASE and output the\n"
    "                         result; PHASE can be 'pa' (parse), 'tc' (type check)\n"
    "                         or 'di' (data implementation)\n"
    "      --stdout           the generated LPE is written to stdout\n"
    "  -h, --help             display this help\n"    
    "      --version          display version information\n"
    "  -q, --quiet            do not display warning messages\n"
    "  -v, --verbose          turn on the display of short intermediate messages\n"
    "  -d, --debug            turn on the display of detailed intermediate messages\n",
    Name);
}

#ifdef __cplusplus
}
#endif
