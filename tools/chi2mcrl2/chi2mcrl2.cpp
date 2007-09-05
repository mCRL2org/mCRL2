// Author(s): Frank Stappers
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file chi2mcrl2.cpp
/// \brief Add your file description here.

#include <cassert>
#include <stdbool.h>
#include <getopt.h>
#include <climits>		//Required by CHAR_MAX
#include <cstdio>
#include <fstream>
#include "aterm2.h"
#include <string>
#include <iostream> 	//Required by cin
#include "chilexer.h"
#include <print/messaging.h>
#include "translate.h"

//#include <cstdlib>
//#include <cstring>
//#include "lin_types.h"
//#include "lin_std.h"
//#include "mcrl2/utilities/aterm_ext.h"
//#include "libstruct.h"
//#include "typecheck.h"
//#include "libalpha.h"
//#include "mcrl2/dataimpl.h"

#define NAME "chi2mcrl2"
#define VERSION "0.0"
#define INFILEEXT ".chi"
#define OUTFILEEXT ".mcrl2"

using namespace ::mcrl2::utilities;
using namespace std;

//t_options represents the options of the translator 
struct t_options {
  string infilename;
  string outfilename;
};

//Functions used by the main program
static ATermAppl translate_file(t_options &options);
static void PrintMoreInfo(char *Name);
static void PrintVersion(void);
static void PrintHelp(char *Name);


static bool parse_command_line(int argc, char *argv[],t_options &options)
{ 
  //declarations for getopt
  /* empty */

  #define ShortOptions   "hqvd"
  #define VersionOption  CHAR_MAX + 1
  struct option LongOptions[] = 
  {
    { "help",        no_argument,       NULL, 'h' },
    { "version",     no_argument,       NULL, VersionOption },
    { "quiet",       no_argument,       NULL, 'q' },
    { "verbose",     no_argument,       NULL, 'v' },
    { "debug",       no_argument,       NULL, 'd' },
    // getop termination string 
	{ 0, 0, 0, 0 }
  };
  int Option;
  //parse options
  Option = getopt_long(argc, argv, ShortOptions, LongOptions, NULL);
  while (Option != -1) {
    switch (Option){
      case 'h': /* help */
        PrintHelp(argv[0]);
        return false;
      case VersionOption: /* version */
        PrintVersion();
        return false;
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
        return false;
    } 
    Option = getopt_long(argc, argv, ShortOptions, LongOptions, NULL);
  }
  //check for dangerous and illegal option combinations
  /* empty */

  //check for wrong number of arguments
  int noargc; //non-option argument count
  noargc = argc - optind;
  if (noargc > 2) 
  {
    fprintf(stderr, "%s: too many arguments\n", NAME);
    PrintMoreInfo(argv[0]);
    return false;
  } else {
    //noargc >= 0 && noargc <= 2
    if (noargc > 0) 
	{
      options.infilename = argv[optind];
    }
    if (noargc == 2) 
	{
      options.outfilename = argv[optind + 1];
    }
  }

  return true;  // main continues
}

ATermAppl translate_file(t_options &options)
{
  ATermAppl result = NULL;
  
  //parse specification
  if (options.infilename == "")
  {
    //parse specification from stdin
    printf("Parsing input from stdin...\n");
    result = parse_stream(cin);
  } else {
    //parse specification from infilename
    ifstream instream(options.infilename.c_str(), ifstream::in|ifstream::binary);
    if (!instream.is_open()) 
	{
      gsErrorMsg("cannot open input file '%s'\n", options.infilename.c_str());
      printf("Cannot open input file '%s'\n", options.infilename.c_str());
      return NULL;
    }
    //gsVerboseMsg("parsing input file '%s'...\n", options.infilename.c_str());
    printf("Parsing input file '%s'...\n", options.infilename.c_str());
	result = parse_stream(instream);
    instream.close();
  }
  
  
  if (result == NULL) 
  {
    gsErrorMsg("parsing failed\n");
    return NULL;
  }

  return result; 
}

void PrintMoreInfo(char *Name)
{
  fprintf(stderr, "Use %s --help for options\n", Name);
}

void PrintVersion(void)
{
  fprintf(stderr,"%s %s (revision %s)\n", NAME, VERSION, REVISION);
}

void PrintHelp(char *Name)
{
  fprintf(stderr,
    "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
	"Translates the Chi specifiation in INFILE and writes the resulting mCRL2 \n"
	"OUTFILE. if OUTFILE is not present, stdout is used. If INFILE is not present\n"
    "stdin is used.\n"
    "\n"
    "  -h, --help            display this help and terminate\n"
    "      --version         display version information and terminate\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         display concise intermediate messages\n"
    "  -d, --debug           display detailed intermediate messages\n",
    Name);
}

// Main 

int main(int argc, char *argv[])
{
  //initialise ATerm library
  ATerm         stack_bottom;
  t_options     options;
 
  CAsttransform asttransform;
  
  ATinit(argc,argv,&stack_bottom);

  //enable constructor functions
  //gsEnableConstructorFunctions();

    if (parse_command_line(argc,argv,options)) {
      //linearise infilename with options lin_options
      ATermAppl result = translate_file(options);

      if (result == NULL) {
        return 1;
      }
	 
 	  gsDebugMsg("Transforming AST to mcrl2 specification\n");
	  asttransform.translator(result);
	  
      //store the result
      if (options.outfilename == "") {
        gsVerboseMsg("saving result to stdout...\n");
        ATwriteToBinaryFile((ATerm) result, stdout);
        fprintf(stdout, "\n");
      } else { //outfilename != NULL
        //open output filename
        FILE *outstream = fopen(options.outfilename.c_str(), "wb");
        if (outstream == NULL) {
          gsErrorMsg("cannot open output file '%s'\n", options.outfilename.c_str());
          return 1;
        }
        gsVerboseMsg("saving result to '%s'...\n", options.outfilename.c_str());
        ATwriteToBinaryFile((ATerm) result, outstream);
        fclose(outstream);
      }
    }

  return 0;
}

