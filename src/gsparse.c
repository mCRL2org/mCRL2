#define  NAME      "gsparse"
#define  LVERSION  "0.1.2"
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

#ifdef __cplusplus
}
#endif

#include "aterm2.h"
#include "gsparse.h"
#include "gsfunc.h"
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

bool ParseSpecificationFileName(
  char *SpecFileName,
  char *OutFileName,
  int VbLevel,
  bool NoSave);
/*Pre: SpecFileName is the name of a valid GenSpect specification file from
       which can be read
       OutFileName is the name of a valid file to which can be written, or NULL
       0 <= VbLevel <= 3
  Post:the specification in SpecFileName is parsed and saved to OutFileName
       If OutFileName is NULL, stdout is used.
       The display of messages is controlled by the value of VbLevel.
       If NoSave, the parsed formula is not saved.
  Ret: true, if everything went ok.
       false, otherwise; appropriate error messages have been shown.
*/ 

bool ParseSpecificationStream(
  FILE *SpecStream,
  FILE *OutStream,
  int VbLevel,
  bool NoSave);
/*Pre: SpecStream is a valid GenSpect specification stream from which can be
       read
       OutStream is the name of a valid stream to which can be written
       0 <= VbLevel <= 3
  Post:the specification in SpecStream is parsed and saved to OutStream
       The display of messages is controlled by the value of VbLevel.
       If NoSave, the parsed formula is not saved.
  Ret: true, if everything went ok.
       false, otherwise; appropriate error messages have been shown.
*/ 

//implementation

int main(int argc, char* argv[]) {
  int   Result                 = 0;
  //declarations for parsing the specification
  char  *SpecFileName      = NULL;
  char  *OutputFileName    = NULL;
  int VbLevel              = 1;
  bool  NoSave            = false;
  bool  MoreInfo          = false;
  //declarations for getopt  
  #define ShortOptions      "qvdn"
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
        ThrowV(0); 
        break;
      case VersionOption: 
        PrintVersion(stdout); 
        ThrowV(0);
        break;
      case TestOption: 
        gsTest();
        ThrowV(0);
        break;
      case 'q': 
        VbLevel = 0;
        break;
      case 'v': 
        VbLevel = 2;
        break;
      case 'd': 
        VbLevel = 3;
        break;
      case 'n': 
        NoSave = true;
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
    ThrowVM1(1,"%s: too few arguments\n", NAME);
  } else if (NoArgc > 2) {
    MoreInfo = true;
    ThrowVM1(1,"%s: too many arguments\n", NAME);
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
  if (!ParseSpecificationFileName(SpecFileName, OutputFileName, VbLevel,
    NoSave))
  {
    ThrowV(1);  
  }       
finally:
  if (MoreInfo) {
    fprintf(stderr, "Try \'%s --help\' for more information.\n", NAME);
  }
  free(SpecFileName);
  free(OutputFileName);
  if (VbLevel == 3) {
    printf("(main): all objects are freed; return %d.\n", Result);
  }
  return Result;
}

bool ParseSpecificationFileName(char *SpecFileName, char *OutputFileName,
  int VbLevel, bool NoSave)
{
  bool Result           = true;
  FILE *SpecStream      = NULL;
  FILE *OutputStream    = NULL;
  if (SpecFileName == NULL) {
    ThrowVM0(false, "error: formula file may not be NULL\n");
  }
  //open specification file for reading
  SpecStream = fopen(SpecFileName, "r");
  if (SpecStream == NULL) {
    ThrowVM2(false, "error: could not open formula file '%s' for reading (error %d)\n", 
      SpecFileName, errno);
  }
  if (VbLevel == 3) {  
    printf("formula file %s is opened for reading.\n", SpecFileName);
  }
  //open output file for writing or set to stdout
  if (OutputFileName == NULL) {
    OutputStream = stdout;
    if (VbLevel == 3) {
      printf("output to stdout.\n");
    }
  } else {  
    OutputStream = fopen(OutputFileName, "w");
    if (!OutputStream) {
      ThrowVM2(false, "error: could not open output file '%s' for writing (error %d)\n", 
        OutputFileName, errno);
    }
    if (VbLevel == 3) {
      printf("output file %s is opened for writing.\n", OutputFileName);
    }
  }
  if (!ParseSpecificationStream(SpecStream, OutputStream, VbLevel, NoSave))
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
  if (VbLevel == 3) {
    printf("(parseSpecificationFileName): all files are closed; return %s\n",
      Result?"true":"false");
  }
  return Result;
}

bool ParseSpecificationStream(FILE *SpecStream, FILE *OutputStream,
  int VbLevel, bool NoSave)
{
  bool Result;
  ATermAppl Spec = NULL;
  //check preconditions
  if (SpecStream == NULL) {
    ThrowVM0(false, "error: formula stream may not be empty\n");
  }
  if (OutputStream == NULL) {
    ThrowVM0(false, "error: output stream may not be empty\n");
  }
  //parse specification save it to Spec
  Spec = gsParseSpecification(SpecStream, VbLevel);
  if (Spec != NULL) {
    if (NoSave) {
      if (VbLevel > 1) {
        printf("do not save specification\n");
      }
    } else {
      if (VbLevel > 1 && OutputStream != stdout) {
        printf("saving specification to file\n");
      }
      ATwriteToTextFile((ATerm) Spec, OutputStream);
      fprintf(OutputStream, "\n");
    }
    Result = true;
  } else {
    Result = false;
  }
finally:
  if (VbLevel == 3) {
    printf("(ParseSpecificationStream): all files are closed; return %s\n",
      Result?"true":"false");
  }
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
    "-n, --no-save            do not save the parsed formula\n",
    NAME
  );
}

void PrintVersion(FILE *Stream) {
  fprintf(Stream, "%s %s\nWritten by %s.\n", 
    NAME, LVERSION, AUTHOR);  
}
