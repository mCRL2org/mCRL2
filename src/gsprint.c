#define  NAME      "gsparse"
#define  LVERSION  "0.1.0"
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

#ifdef __cplusplus
}
#endif

#include "aterm2.h"
#include "gsprint.h"
#include "gsfunc.h"
#include "gslowlevel.h"
#include "libgsparse.h"

//local declarations

int main(int argc, char *argv[]);
//main function where:
//  argc represents the number of arguments
//  argv represents the arguments

void PrintUsage(FILE* Stream);
//print usage information to stream

void PrintVersion(FILE* Stream);
//print version information to stream

bool PrintSpecificationFileName(char *SpecFileName, char *OutFileName);
/*Pre: SpecFileName is the name of a file from which can be read, and which
       contains a specification that adheres to the internal format
       which can be read
       OutFileName is the name of a valid file to which can be written, or NULL
  Post:the specification in SpecFileName is printed and saved to OutFileName
       If OutFileName is NULL, stdout is used.
  Ret: true, if everything went ok.
       false, otherwise; appropriate error messages have been shown.
*/ 

bool PrintSpecificationStream(FILE *SpecStream, FILE *OutStream);
/*Pre: SpecStream is a stream from which can be read, and which contains a
       specification that adheres to the internal format
       OutStream is the name of a valid stream to which can be written
  Post:the specification in SpecStream is printed and saved to OutStream
  Ret: true, if everything went ok.
       false, otherwise; appropriate error messages have been shown.
*/ 

//implementation

int main(int argc, char* argv[]) {
  int  Result          = 0;
  //declarations for parsing the specification
  char *SpecFileName   = NULL;
  char *OutputFileName = NULL;
  bool MoreInfo        = false;
  //declarations for getopt  
  #define ShortOptions      "qvd"
  #define HelpOption        CHAR_MAX + 1
  #define VersionOption     HelpOption + 1
  struct option LongOptions[] = { 
    {"help"      , no_argument,       NULL, HelpOption},
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
      case HelpOption: 
        PrintUsage(stdout);
        ThrowV(0); 
        break;
      case VersionOption: 
        PrintVersion(stdout); 
        ThrowV(0);
        break;
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
      	MoreInfo = true;
      	ThrowV(1);
        break;
    }
    Option = getopt_long(argc, argv, ShortOptions, LongOptions, NULL);
  }
  int NoArgc; //non-option argument count
  NoArgc = argc - optind;
  if (NoArgc <= 0) {
    MoreInfo = true;
    fprintf(stderr, "%s: too few arguments\n", NAME);
    ThrowV(1);
  } else if (NoArgc > 2) {
    MoreInfo = true;
    fprintf(stderr, "%s: too many arguments\n", NAME);
    ThrowV(1);
  } else {
    //NoArgc > 0 && NoArgc <= 2
    SpecFileName = strdup(argv[optind]);
    if (NoArgc == 2) {
      OutputFileName = strdup(argv[optind + 1]);
    }
  }
  //initialise ATerm library
  ATerm StackBottom;
  ATinit(0, NULL, &StackBottom);
  //print specification  
  if (!PrintSpecificationFileName(SpecFileName, OutputFileName))
  {
    ThrowV(1);  
  }       
finally:
  if (MoreInfo) {
    fprintf(stderr, "Try \'%s --help\' for more information.\n", NAME);
  }
  free(SpecFileName);
  free(OutputFileName);
  gsDebugMsg("all objects are freed; return %d.\n", Result);
  return Result;
}

bool PrintSpecificationFileName(char *SpecFileName, char *OutputFileName)
{
  assert(SpecFileName != NULL);
  bool Result           = true;
  FILE *SpecStream      = NULL;
  FILE *OutputStream    = NULL;
  //open specification file for reading
  SpecStream = fopen(SpecFileName, "r");
  if (SpecStream == NULL) {
    ThrowVM(false,
      "could not open specification file '%s' for reading (error %d)\n",
      SpecFileName, errno);
  }
  gsDebugMsg("specification file %s is opened for reading.\n", SpecFileName);
  //open output file for writing or set to stdout
  if (OutputFileName == NULL) {
    OutputStream = stdout;
    gsDebugMsg("output to stdout.\n");
  } else {  
    OutputStream = fopen(OutputFileName, "w");
    if (!OutputStream) {
      ThrowVM(false, "could not open output file '%s' for writing (error %d)\n", 
        OutputFileName, errno);
    }
    gsDebugMsg("output file %s is opened for writing.\n", OutputFileName);
  }
  if (!PrintSpecificationStream(SpecStream, OutputStream))
  {
    ThrowV(false);
  }
finally:
  if (SpecStream != NULL) {
    fclose(SpecStream);
  }
  if (OutputStream != NULL && OutputStream != stdout) {
    fclose(OutputStream);
  }
  gsDebugMsg("all files are closed; return %s\n", Result?"true":"false");
  return Result;
}

bool PrintSpecificationStream(FILE *SpecStream, FILE *OutputStream)
{
  assert(SpecStream != NULL);
  assert(OutputStream != NULL);
  bool Result;
  //read specification from SpecStream
  ATermAppl Spec = (ATermAppl) ATreadFromFile(SpecStream);
  if (Spec == NULL) {
    ThrowVM(false, "error: could not read specification from stream\n");
  }
  //print specification to OutputStream
  if (OutputStream != stdout) gsVerboseMsg(
    "printing specification to file in a human readable format\n");
  gsPrintSpecification(OutputStream, Spec);
  Result = true;
finally:
  gsDebugMsg("all files are closed; return %s\n", Result?"true":"false");
  return Result;
}

void PrintUsage(FILE *Stream) {
  fprintf(Stream, 
    "Usage: %s OPTIONS SPECFILE [OUTFILE]\n"
    "Print the internal GenSpect specification in SPECFILE to OUTFILE in a human\n"
    "readable format. If OUTFILE is not present, stdout is used.\n"
    "\n"
    "The OPTIONS that can be used are:\n"
    "    --help               display this help\n"
    "    --version            display version information\n"
    "-q, --quiet              do not display warning messages\n"
    "-v, --verbose            turn on the display of short intermediate messages\n"
    "-d, --debug              turn on the display of detailed intermediate messages\n",
    NAME
  );
}

void PrintVersion(FILE *Stream) {
  fprintf(Stream, "%s %s\nWritten by %s.\n", 
    NAME, LVERSION, AUTHOR);  
}
