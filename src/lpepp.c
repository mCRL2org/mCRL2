#define  NAME      "lpepp"
#define  LVERSION  "0.4"
#define  AUTHOR    "Aad Mathijssen"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <stdbool.h>
#include <assert.h>

#include <aterm2.h>
#include "lpepp.h"
#include "gsfunc.h"
#include "gslowlevel.h"
#include "libprint_types.h"
#include "libprint_c.h"
#include "libgsrewrite.h"

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

static bool PrintSpecificationStream(FILE *SpecStream, FILE *OutStream,
  t_pp_format pp_format);
/*Pre: SpecStream is a stream from which can be read, and which contains a
       specification that adheres to the internal format
       OutStream is the name of a valid stream to which can be written
  Post:the specification in SpecStream is printed in the pp_format format
       and saved to OutStream
  Ret: true, if everything went ok.
       false, otherwise; appropriate error messages have been shown.
*/ 

static void PrintPPFormat(FILE *stream, t_pp_format pp_format);
/*Pre: stream points to a stream to which can be written
 *Ret: a string representation of pp_format is written to stream
 */

//implementation

int main(int argc, char* argv[]) {
  int  Result          = 0;
  //declarations for parsing the specification
  char *SpecFileName   = NULL;
  char *OutputFileName = NULL;
  t_pp_format opt_pp_format = ppAdvanced;
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
        if (strcmp(optarg, "internal") == 0) {
          opt_pp_format = ppInternal;
        } else if (strcmp(optarg, "basic") == 0) {
          opt_pp_format = ppBasic;
        } else if (strcmp(optarg, "advanced") == 0) {
          opt_pp_format = ppAdvanced;
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
      if (strcmp(argv[optind],"-") != 0)
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
  if (!PrintSpecificationFileName(SpecFileName, OutputFileName, opt_pp_format))
  {
    Result = 1;  
  }       
  free(SpecFileName);
  free(OutputFileName);
  gsDebugMsg("all objects are freed; return %d.\n", Result);
  return Result;
}

bool PrintSpecificationFileName(char *SpecFileName, char *OutputFileName,
  t_pp_format pp_format)
{
  bool Result           = true;
  FILE *SpecStream      = NULL;
  FILE *OutputStream    = NULL;
  //open specification file for reading
  if (SpecFileName == NULL ) {
    SpecStream = stdin;
    gsDebugMsg("input from stdin.\n");
  } else {
    SpecStream = fopen(SpecFileName, "r");
  }
  if (SpecStream == NULL) {
    gsErrorMsg(
      "could not open specification file '%s' for reading (error %d)\n",
      SpecFileName, errno);
    Result = false;
  } else {
    if ( SpecStream != stdin )
      gsDebugMsg("specification file %s is opened for reading.\n", SpecFileName);
    //open output file for writing or set to stdout
    if (OutputFileName == NULL) {
      OutputStream = stdout;
      gsDebugMsg("output to stdout.\n");
    } else {  
      OutputStream = fopen(OutputFileName, "w");
      if (!OutputStream) {
        gsErrorMsg("could not open output file '%s' for writing (error %d)\n", 
          OutputFileName, errno);
        Result = false;
      } else {
        gsDebugMsg("output file %s is opened for writing.\n", OutputFileName);
      }
    }
    if (Result &&
          !PrintSpecificationStream(SpecStream, OutputStream, pp_format))
    {
      Result = false;
    }
  }
  if ((SpecStream != NULL) && (SpecStream != stdin)) {
    fclose(SpecStream);
  }
  if ((OutputStream != NULL) && (OutputStream != stdout)) {
    fclose(OutputStream);
  }
  gsDebugMsg("all files are closed; return %s\n", Result?"true":"false");
  return Result;
}

bool PrintSpecificationStream(FILE *SpecStream, FILE *OutputStream,
  t_pp_format pp_format)
{
  assert(SpecStream != NULL);
  assert(OutputStream != NULL);
  bool Result;
  //read specification from SpecStream
  ATermAppl Spec = (ATermAppl) ATreadFromFile(SpecStream);
  if (Spec == NULL) {
    gsErrorMsg("error: could not read specification from stream\n");
    Result = false;
  } else {
    //print specification to OutputStream
    if (OutputStream != stdout) {
      if (gsVerbose) {
        fprintf(stderr, "printing specification to file in the ");
        PrintPPFormat(stderr, pp_format);
        fprintf(stderr, " format\n");
      }
    }
    gsEnableConstructorFunctions();
    gsRewriteInit(ATAgetArgument(Spec,3),GS_REWR_INNER3);
    PrintPart_C(OutputStream, (ATerm) Spec, pp_format);
    gsRewriteFinalise();
    Result = true;
  }
  gsDebugMsg("all files are closed; return %s\n", Result?"true":"false");
  return Result;
}

void PrintUsage(char *Name) {
  fprintf(stderr, 
    "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
    "Print the mCRL2 LPE in INFILE to OUTFILE in a human readable format. If OUTFILE\n"
    "is not present, stdout is used. If INFILE is not present or -, stdin is used.\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -f, --format=FORMAT   the LPE is printed using the supplied FORMAT:\n"
    "                          'internal' for the internal format\n"
    "                          'basic' for the basic structure (used for debugging)\n"
    "                          'advanced' for an mCRL2 specification (default)\n"
    "  -h, --help            display this help\n"
    "      --version         display version information\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         turn on the display of short intermediate messages\n"
    "  -d, --debug           turn on the display of detailed intermediate messages\n",
    Name
  );
}

void PrintMoreInfo(char *Name) {
  fprintf(stderr, "Try \'%s --help\' for more information.\n", Name);
}

void PrintVersion(void) {
  fprintf(stderr, "%s %s\nWritten by %s.\n", 
    NAME, LVERSION, AUTHOR);  
}

void PrintPPFormat(FILE *stream, t_pp_format pp_format)
{
  if (pp_format == ppInternal) {
    fprintf(stream, "internal");
  } else if (pp_format == ppBasic) {
    fprintf(stream, "basic");
  } else if (pp_format == ppAdvanced) {
    fprintf(stream, "advanced");
  }
}

#ifdef __cplusplus
}
#endif
