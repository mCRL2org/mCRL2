#define  NAME      "pbespp"
#define  VERSION   "0.4.5"
#define  AUTHOR    "Aad Mathijssen"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <stdbool.h>
#include <assert.h>

#include <aterm2.h>
#include "libstruct.h"
#include "liblowlevel.h"
#include "libprint_types.h"
#include "libprint_c.h"
#include "librewrite_c.h"

//local declarations

int main(int argc, char *argv[]);
//main function where:
//  argc represents the number of arguments
//  argv represents the arguments

static void PrintUsage(char *Name);
//print usage information

static void PrintMoreInfo(char *Name);
//print --help suggestion

static void PrintVersion(void);
//print version information

static bool PrintSpecificationFileName(char *SpecFileName, char *OutFileName,
  t_pp_format pp_format);
/*Pre: SpecFileName is the name of a file from which can be read, and which
       contains a specification that adheres to the internal format
       which can be read
       OutFileName is the name of a valid file to which can be written, or NULL
  Post:the specification in SpecFileName is printed in the pp_format format
       and saved to OutFileName
       If OutFileName is NULL, stdout is used.
  Ret: true, if everything went ok.
       false, otherwise; appropriate error messages have been shown.
*/ 

static void PrintPPFormat(FILE *stream, t_pp_format pp_format);
/*Pre: stream points to a stream to which can be written
 *Ret: a string representation of pp_format is written to stream
 */

//implementation

int main(int argc, char* argv[]) {
  //declarations for parsing the specification
  char *SpecFileName   = NULL;
  char *OutputFileName = NULL;
  t_pp_format opt_pp_format = ppDefault;
  //declarations for getopt  
  #define ShortOptions      "f:hqvd"
  #define VersionOption     CHAR_MAX + 1
  struct option LongOptions[] = { 
    {"format"    , required_argument, NULL, 'f'},
    {"help"      , no_argument,       NULL, 'h'},
    {"version"   , no_argument,       NULL, VersionOption},
    {"quiet"     , no_argument,       NULL, 'q'},
    {"verbose"   , no_argument,       NULL, 'v'},
    {"debug"     , no_argument,       NULL, 'd'},
    {0, 0, 0, 0}
  };
  int Option;
  //parse options
  Option = getopt_long(argc, argv, ShortOptions, LongOptions, NULL);
  while (Option != -1) {
    switch (Option) {
      case 'f':
        if (strcmp(optarg, "default") == 0) {
          opt_pp_format = ppDefault;
        } else if (strcmp(optarg, "debug") == 0) {
          opt_pp_format = ppDebug;
        } else if (strcmp(optarg, "internal") == 0) {
          opt_pp_format = ppInternal;
        } else {
          gsErrorMsg("option -f has illegal argument '%s'\n", optarg);
          return 1;
        }
        break;
      case 'h':
        PrintUsage(argv[0]);
        return 0; 
      case VersionOption: 
        PrintVersion(); 
        return 0;
      case 'q':
        gsSetQuietMsg();
        break;
      case 'v': 
        gsSetVerboseMsg();
        break;
      case 'd': 
        gsSetDebugMsg();
        break;
      default:
      	PrintMoreInfo(argv[0]);
      	return 1;
    }
    Option = getopt_long(argc, argv, ShortOptions, LongOptions, NULL);
  }
  int NoArgc; //non-option argument count
  NoArgc = argc - optind;
  if (NoArgc > 2) {
    fprintf(stderr, "%s: too many arguments\n", NAME);
   	PrintMoreInfo(argv[0]);
   	return 1;
  } else {
    //NoArgc >= 0 && NoArgc <= 2
    if (NoArgc > 0) {
      SpecFileName = strdup(argv[optind]);
    }
    if (NoArgc == 2) {
      OutputFileName = strdup(argv[optind + 1]);
    }
  }
  //initialise ATerm library
  ATerm StackBottom;
  ATinit(0, NULL, &StackBottom);
  //print specification  
  bool Result =
    PrintSpecificationFileName(SpecFileName, OutputFileName, opt_pp_format);
  free(SpecFileName);
  free(OutputFileName);
  return Result?0:1;
}

bool PrintSpecificationFileName(char *SpecFileName, char *OutputFileName,
  t_pp_format pp_format)
{
  FILE *SpecStream      = NULL;
  FILE *OutputStream    = NULL;
  //open SpecFileName for reading
  if (SpecFileName == NULL ) {
    SpecStream = stdin;
    gsDebugMsg("input from stdin.\n");
  } else {
    SpecStream = fopen(SpecFileName, "rb");
  }
  if (SpecStream == NULL) {
    gsErrorMsg(
      "could not open input file '%s' for reading: ", SpecFileName);
    perror(NULL);
    return false;
  }
  assert(SpecStream != NULL);
  //read specification from SpecStream
  if ( SpecStream != stdin )
    gsDebugMsg("input file '%s' is opened for reading.\n", SpecFileName);
  ATermAppl Spec = (ATermAppl) ATreadFromFile(SpecStream);
  if (Spec == NULL) {
    if (SpecStream == stdin) {
      gsErrorMsg("could not read PBES from stdin\n");
    } else {
      gsErrorMsg("could not read PBES from '%s'\n", SpecFileName);
      fclose(SpecStream);
    }
    return false;
  }
  assert(Spec != NULL);
  gsEnableConstructorFunctions();
  if (!gsIsPBES(Spec)) {
    if (SpecStream == stdin) {
      gsErrorMsg("stdin does not contain a PBES\n");
    } else {
      gsErrorMsg("'%s' does not contain a PBES\n", SpecFileName);
      fclose(SpecStream);
    }
    return false;
  }
  assert(gsIsPBES(Spec));
  //open output file for writing or set to stdout
  if (OutputFileName == NULL) {
    OutputStream = stdout;
    gsDebugMsg("output to stdout.\n");
  } else {  
    OutputStream = fopen(OutputFileName, "wb");
    if (OutputStream == NULL) {
      gsErrorMsg("could not open output file '%s' for writing ",
        OutputFileName);
      perror(NULL);
      if (SpecStream != stdin) {
        fclose(SpecStream);
      }
      return false;
    }
    gsDebugMsg("output file '%s' is opened for writing.\n", OutputFileName);
  }
  assert(OutputStream != NULL);
  //print Spec to OutputStream
  if (gsVerbose) {
    fprintf(stderr, "printing PBES from ");
    if (SpecStream == stdin) {
      fprintf(stderr, "stdin");
    } else {
      fprintf(stderr, "'%s'", SpecFileName);
    }
    fprintf(stderr, " to ");
    if (OutputStream == stdout) {
      fprintf(stderr, "stdout");
    } else {
      fprintf(stderr, "'%s'", OutputFileName);
    }
    fprintf(stderr, " in the ");
    PrintPPFormat(stderr, pp_format);
    fprintf(stderr, " format\n");
  }
  //initialise rewriter, if needed
  if (pp_format == ppDefault) {
    gsRewriteInit(ATAgetArgument(ATAgetArgument(Spec,0),3),GS_REWR_INNER);
  }
  //pretty print Spec to OutputStream
  PrintPart_C(OutputStream, (ATerm) Spec, pp_format);
  //finalise rewriter, if needed
  if (pp_format == ppDefault) {
    gsRewriteFinalise();
  }
  if (SpecStream != stdin) {
    fclose(SpecStream);
  }
  if (OutputStream != stdout) {
    fclose(OutputStream);
  }
  return true;
}

void PrintUsage(char *Name) {
  fprintf(stderr, 
    "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
    "Print the PBES in INFILE to OUTFILE in a human readable format. If OUTFILE\n"
    "is not present, stdout is used. If INFILE is not present, stdin is used.\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -f, --format=FORMAT   print the PBES in the specificied FORMAT:\n"
    "                        - 'default' for an mCRL2 specification (default)\n"
    "                        - 'debug' is like 'default' with the following\n"
    "                          exceptions:\n"
    "                          + data expressions are printed in prefix notation\n"
    "                            using identifiers from the internal format\n"
    "                          + each data equation is put in a separate data\n"
    "                            equation section\n"
    "                        - 'internal' for a textual ATerm representation of the\n"
    "                          internal format\n"
    "  -h, --help            display this help and terminate\n"
    "      --version         display version information and terminate\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         display concise intermediate messages\n"
    "  -d, --debug           display detailed intermediate messages\n",
    Name
  );
}

void PrintMoreInfo(char *Name) {
  fprintf(stderr, "Try \'%s --help\' for more information.\n", Name);
}

void PrintVersion(void) {
  fprintf(stderr,"%s %s (revision %s)\n", NAME, VERSION, REVISION);
}

void PrintPPFormat(FILE *stream, t_pp_format pp_format)
{
  if (pp_format == ppDefault) {
    fprintf(stream, "default");
  } else if (pp_format == ppDebug) {
    fprintf(stream, "debug");
  } else if (pp_format == ppInternal) {
    fprintf(stream, "internal");
  }
}
