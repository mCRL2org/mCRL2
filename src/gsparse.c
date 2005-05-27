#define  NAME      "gsparse"
#define  LVERSION  "0.1.37"
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
#include "gsparse.h"
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

void PrintMoreInfo(FILE* Stream);
//print --help suggestion to stream

bool ParseSpecificationFileName(
  char *SpecFileName,
  char *OutFileName,
  bool Human,
  bool NoSave);
/*Pre: SpecFileName is the name of a valid GenSpect specification file from
       which can be read
       OutFileName is the name of a valid file to which can be written, or NULL
  Post:the specification in SpecFileName is parsed and saved to OutFileName
       If OutFileName is NULL, stdout is used.
       If Human, the parsed formula is saved in a human readable format
       If NoSave, the parsed formula is not saved.
  Ret: true, if everything went ok.
       false, otherwise; appropriate error messages have been shown.
*/ 

bool ParseSpecificationStream(
  FILE *SpecStream,
  FILE *OutStream,
  bool Human,
  bool NoSave);
/*Pre: SpecStream is a valid GenSpect specification stream from which can be
       read
       OutStream is the name of a valid stream to which can be written
  Post:the specification in SpecStream is parsed and saved to OutStream
       If Human, the parsed formula is saved in a human readable format
       If NoSave, the parsed formula is not saved.
  Ret: true, if everything went ok.
       false, otherwise; appropriate error messages have been shown.
*/ 

//implementation

int main(int argc, char* argv[]) {
  int  Result          = 0;
  //declarations for parsing the specification
  char *SpecFileName   = NULL;
  char *OutputFileName = NULL;
  bool Human           = false;
  bool NoSave          = false;
  //declarations for getopt  
  #define ShortOptions      "hqvdn"
  #define HelpOption        CHAR_MAX + 1
  #define VersionOption     HelpOption + 1
  #define TestOption        VersionOption + 1
  struct option LongOptions[] = { 
    {"help"      , no_argument,       NULL, HelpOption},
    {"version"   , no_argument,       NULL, VersionOption},
    {"test"      , no_argument,       NULL, TestOption},
    {"quiet"     , no_argument,       NULL, 'q'},
    {"verbose"   , no_argument,       NULL, 'v'},
    {"debug"     , no_argument,       NULL, 'd'},
    {"human"     , no_argument,       NULL, 'h'},
    {"no-save"   , no_argument,       NULL, 'n'},
    {0, 0, 0, 0}
  };
  int Option;
  //parse options
  Option = getopt_long(argc, argv, ShortOptions, LongOptions, NULL);
  while (Option != -1) {
    switch (Option) {
      case HelpOption: 
        PrintUsage(stdout);
        return 0; 
      case VersionOption: 
        PrintVersion(stdout); 
        return 0;
      case TestOption: 
        gsTest();
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
      case 'h':
        Human = true;
        break;
      case 'n': 
        NoSave = true;
        break;
      default:
      	PrintMoreInfo(stderr);
      	return 1;
    }
    Option = getopt_long(argc, argv, ShortOptions, LongOptions, NULL);
  }
  int NoArgc; //non-option argument count
  NoArgc = argc - optind;
  if (NoArgc <= 0) {
    fprintf(stderr, "%s: too few arguments\n", NAME);
   	PrintMoreInfo(stderr);
   	return 1;
  } else if (NoArgc > 2) {
    fprintf(stderr, "%s: too many arguments\n", NAME);
   	PrintMoreInfo(stderr);
   	return 1;
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
  //parse specification  
  if (!ParseSpecificationFileName(SpecFileName, OutputFileName, Human, NoSave))
  {
    Result = 1;  
  }       
  free(SpecFileName);
  free(OutputFileName);
  gsDebugMsg("all objects are freed; return %d.\n", Result);
  return Result;
}

bool ParseSpecificationFileName(char *SpecFileName, char *OutputFileName,
  bool Human, bool NoSave)
{
  assert(SpecFileName != NULL);
  bool Result           = true;
  FILE *SpecStream      = NULL;
  FILE *OutputStream    = NULL;
  //open specification file for reading
  SpecStream = fopen(SpecFileName, "r");
  if (SpecStream == NULL) {
    gsErrorMsg("could not open specification file '%s' for reading (error %d)\n",
      SpecFileName, errno);
    Result = false;
  } else {
    gsDebugMsg("formula file %s is opened for reading.\n", SpecFileName);
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
    if ( Result && !ParseSpecificationStream(SpecStream, OutputStream, Human, NoSave))
    {
      Result = false;
    }
  }
  if (SpecStream != NULL) {
    fclose(SpecStream);
  }
  if (OutputStream != NULL && OutputStream != stdout) {
    fclose(OutputStream);
  }
  gsDebugMsg("all files are closed; return %s\n", Result?"true":"false");
  return Result;
}

bool ParseSpecificationStream(FILE *SpecStream, FILE *OutputStream,
  bool Human, bool NoSave)
{
  assert(SpecStream != NULL);
  assert(OutputStream != NULL);
  bool Result;
  //parse specification save it to Spec
  ATermAppl Spec = gsParseSpecification(SpecStream);
  if (Spec != NULL) {
    if (NoSave) {
      gsVerboseMsg("do not save specification\n");
    } else {
      if (Human) {
        //save specification in a human readable format
        if (OutputStream != stdout) gsVerboseMsg(
          "saving specification to file in a human readable format\n");
        gsPrintSpecification(OutputStream, Spec);
      } else {
        //save specification as an ATerm
        if (OutputStream != stdout) gsVerboseMsg(
          "saving specification to file\n");
        ATwriteToTextFile((ATerm) Spec, OutputStream);
        fprintf(OutputStream, "\n");
      }
    }
    Result = true;
  } else {
    Result = false;
  }
  gsDebugMsg("all files are closed; return %s\n", Result?"true":"false");
  return Result;
}

void PrintUsage(FILE *Stream) {
  fprintf(Stream, 
    "Usage: %s OPTIONS SPECFILE [OUTFILE]\n"
    "Translate the GenSpect specification in SPECFILE to the ATerm format and\n"
    "save it to OUTFILE. If OUTFILE is not present, stdout is used.\n"
    "\n"
    "The OPTIONS that can be used are:\n"
    "    --help               display this help\n"
    "    --version            display version information\n"
    "    --test               execute test function (will be removed)\n"
    "-q, --quiet              do not display warning messages\n"
    "-v, --verbose            turn on the display of short intermediate messages\n"
    "-d, --debug              turn on the display of detailed intermediate messages\n"
    "-h, --human              save the parsed formula in a human readable format\n"
    "-n, --no-save            do not save the parsed formula\n",
    NAME
  );
}

void PrintVersion(FILE *Stream) {
  fprintf(Stream, "%s %s\nWritten by %s.\n", 
    NAME, LVERSION, AUTHOR);  
}

void PrintMoreInfo(FILE *Stream) {
  fprintf(Stream, "Try \'%s --help\' for more information.\n", NAME);
}

#ifdef __cplusplus
}
#endif
