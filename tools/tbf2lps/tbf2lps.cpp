// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tbf2lps.cpp

#define NAME "tbf2lps"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include <assert.h>
#include <limits.h>
#include "mcrl2/core/struct.h"
#include "lpstrans.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/version_info.h"

using namespace ::mcrl2::utilities;

static void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
    "Read mCRL LPS from INFILE, convert it to a mCRL2 LPS and save the result to\n"
    "OUTFILE. If OUTFILE is not present, stdout is used. If INFILE is not present,\n"
    "stdin is used. To use stdin and save the output to a file, use '-' for INFILE.\n"
    "\n"
    "The following conversions on the data specification will be applied:\n"
    "- constructors T, F: -> Bool are replaced by true, false\n"
    "- mappings and, or: Bool # Bool -> Bool are replaced by &&, ||\n"
    "- mapping eq: S # S -> Bool is replaced by ==, for all sorts S\n"
    "\n"
    "  -n, --no-conv-map     do not apply the conversion of mappings and, or and eq\n"
    "      --no-conv-cons    do not apply the conversion of constructors T and F\n"
    "  -h, --help            display this help and terminate\n"
    "      --version         display version information and terminate\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         display concise intermediate messages\n"
    "  -d, --debug           display detailed intermediate messages\n",
    Name);
}

int main(int argc, char **argv)
{
  FILE *InStream, *OutStream;
  ATerm bot;
  #define sopts "hqvdn"
  #define version_option CHAR_MAX + 1
  #define no_conv_cons_option CHAR_MAX + 2
  struct option lopts[] = {
    { "help",          no_argument,  NULL,  'h' },
    { "version",       no_argument,  NULL,  version_option },
    { "quiet",         no_argument,  NULL,  'q' },
    { "verbose",       no_argument,  NULL,  'v' },
    { "debug",         no_argument,  NULL,  'd' },
    { "no-conv-map",   no_argument,  NULL,  'n' },
    { "no-conv-cons",  no_argument,  NULL,  no_conv_cons_option },
     { 0, 0, 0, 0 }
  };

  ATinit(argc,argv,&bot);
  gsEnableConstructorFunctions();

  bool opt_quiet = false;
  bool opt_verbose = false;
  bool opt_debug = false;
  bool convert_funcs = true;
  bool convert_bools = true;
  int opt;
  while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
  {
    switch ( opt )
    {
      case 'h':
        print_help(stderr, argv[0]);
        return 0;
      case version_option:
        print_version_information(NAME);
        return 0;
      case 'q':
        opt_quiet = true;
        break;
      case 'v':
        opt_verbose = true;
        break;
      case 'd':
        opt_debug = true;
        break;
      case 'n':
        convert_funcs = false;
        break;
      case no_conv_cons_option:
        convert_bools = false;
        break;
      default:
        break;
    }
  }
  if ( opt_quiet && opt_verbose )
  {
    gsErrorMsg("options -q/--quiet and -v/--verbose cannot be used together\n");
    return 1;
  }
  if ( opt_quiet && opt_debug )
  {
    gsErrorMsg("options -q/--quiet and -d/--debug cannot be used together\n");
    return 1;
  }
  if ( opt_quiet )
    gsSetQuietMsg();
  if ( opt_verbose )
    gsSetVerboseMsg();
  if ( opt_debug )
    gsSetDebugMsg();



  InStream = stdin;
  char *InFileName = NULL;
  if ( optind < argc && strcmp(argv[optind],"-") )
  {
    InFileName = argv[optind];
    gsVerboseMsg("reading mCRL LPS from '%s'...\n", InFileName);
    if ( (InStream = fopen(InFileName, "rb")) == NULL )
    {
      gsErrorMsg("cannot open input file '%s' for reading: ",
        InFileName);
      perror(NULL);
      return 1;
    }
  } else {
    gsVerboseMsg("reading mCRL LPS from stdin...\n");
  }

  ATermAppl mu_spec = (ATermAppl) ATreadFromFile(InStream);
  ATprotectAppl(&mu_spec);

  if (InStream != stdin) {
    fclose(InStream);
  }

  if ( mu_spec == NULL )
  {
    if (InStream == stdin) {
      gsErrorMsg("could not read mCRL LPS from stdin\n");
    } else {
      gsErrorMsg("could not read mCRL LPS from '%s'\n", InFileName);
    }
    return 1;
  }
  assert(mu_spec != NULL);

  if (!is_mCRL_spec(mu_spec)) {
    if (InStream == stdin) {
      gsErrorMsg("stdin does not contain a mCRL LPS\n");
    } else {
      gsErrorMsg("'%s' does not contain a mCRL LPS\n", InFileName);
    }
    return false;
  }
  assert(is_mCRL_spec(mu_spec));


  ATermAppl spec = translate(mu_spec,convert_bools,convert_funcs);
  ATprotectAppl(&spec);


  OutStream = stdout;
  char *OutFileName = NULL;
  if ( optind+1 < argc )
  {
    OutFileName = argv[optind+1];
    gsVerboseMsg("writing mCRL2 LPS to '%s'...\n", OutFileName);
    if ( (OutStream = fopen(OutFileName, "wb")) == NULL )
    {
      gsErrorMsg("cannot open output file '%s' for writing: ",
        OutFileName);
      perror(NULL);
      return 1;
    }
  } else {
    gsVerboseMsg("writing mCRL2 LPS to stdout...\n");
  }

  ATwriteToSAFFile((ATerm) spec,OutStream);

  if (OutStream != stdout) {
    fclose(OutStream);
  }

  return 0;
}
