#define  NAME      "lpepp"
#define  LVERSION  "0.3"
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
#include "libprint_c.h"

//local declarations

int main(int argc, char *argv[]);
//main function where:
//  argc represents the number of arguments
//  argv represents the arguments

void PrintUsage(FILE* Stream, char *Name);
//print usage information to stream

void PrintMoreInfo(FILE* Stream, char *Name);
//print --help suggestion to stream

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
  //declarations for getopt  
  #define ShortOptions      "hqvd"
  #define VersionOption     CHAR_MAX + 1
  struct option LongOptions[] = { 
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
      case 'h':
        PrintUsage(stdout, argv[0]);
        return 0; 
      case VersionOption: 
        PrintVersion(stdout); 
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
      	PrintMoreInfo(stderr, argv[0]);
      	return 1;
    }
    Option = getopt_long(argc, argv, ShortOptions, LongOptions, NULL);
  }
  int NoArgc; //non-option argument count
  NoArgc = argc - optind;
  if (NoArgc > 2) {
    fprintf(stderr, "%s: too many arguments\n", NAME);
   	PrintMoreInfo(stderr, argv[0]);
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
  if (!PrintSpecificationFileName(SpecFileName, OutputFileName))
  {
    Result = 1;  
  }       
  free(SpecFileName);
  free(OutputFileName);
  gsDebugMsg("all objects are freed; return %d.\n", Result);
  return Result;
}

bool PrintSpecificationFileName(char *SpecFileName, char *OutputFileName)
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
    if (Result && !PrintSpecificationStream(SpecStream, OutputStream))
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

bool PrintSpecificationStream(FILE *SpecStream, FILE *OutputStream)
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
    if (OutputStream != stdout) gsVerboseMsg(
      "printing specification to file in a human readable format\n");
    gsEnableConstructorFunctions();
    PrintPart_C(OutputStream, (ATerm) Spec);
    Result = true;
  }
  gsDebugMsg("all files are closed; return %s\n", Result?"true":"false");
  return Result;
}

void PrintUsage(FILE *Stream, char *Name) {
  fprintf(Stream, 
    "Usage: %s OPTIONS [LPEFILE [OUTFILE]]\n"
    "Print the mCRL2 LPE in LPEFILE to OUTFILE in a human readable format. If OUTFILE\n"
    "is not present, stdout is used. If LPEFILE is not present or -, stdin is used.\n"
    "\n"
    "The OPTIONS that can be used are:\n"
    "  -h, --help             display this help\n"
    "      --version          display version information\n"
    "  -q, --quiet            do not display warning messages\n"
    "  -v, --verbose          turn on the display of short intermediate messages\n"
    "  -d, --debug            turn on the display of detailed intermediate messages\n",
    Name
  );
}

void PrintMoreInfo(FILE *Stream, char *Name) {
  fprintf(Stream, "Try \'%s --help\' for more information.\n", Name);
}

void PrintVersion(FILE *Stream) {
  fprintf(Stream, "%s %s\nWritten by %s.\n", 
    NAME, LVERSION, AUTHOR);  
}

#ifdef __cplusplus
}
#endif
