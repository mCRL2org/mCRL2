#define NAME "tbf2lpe"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include <assert.h>
#include "libstruct.h"
#include "liblowlevel.h"
#include "libprint_c.h"
#include "lpetrans.h"

static void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
    "Read mCRL LPE from INFILE, convert it to a mCRL2 LPE and save the result to\n"
    "OUTFILE. If OUTFILE is not present, stdout is used. If INFILE is not present,\n"
    "stdin is used. To use stdin and save the output to a file, use '-' for INFILE.\n"
    "\n"
    "The following conversions on the data specification will be applied:\n"
    "- constructors T, F: -> Bool are replaced by true, false\n"
    "- mappings and, or: Bool # Bool -> Bool are replaced by &&, ||\n"
    "- mapping eq: S # S -> Bool is replaced by ==, for all sorts S\n"
    "\n"
    "  -h, --help            display this help message\n"
    "  -q, --quiet           do not print any unrequested information\n"
    "  -v, --verbose         display extra information about the conversion process\n"
    "  -n, --no-conv-map     do not apply the conversion of mappings and, or and eq\n"
    "      --no-conv-cons    do not apply the conversion of constructors T and F\n",
    Name);
}

int main(int argc, char **argv)
{
  FILE *InStream, *OutStream;
  ATerm bot;
  #define sopts "hqvn"
  struct option lopts[] = {
    { "help",    no_argument,  NULL,  'h' },
    { "quiet",    no_argument,  NULL,  'q' },
    { "verbose",    no_argument,  NULL,  'v' },
    { "no-conv-map",        no_argument,  NULL,  'n' },
    { "no-conv-cons",  no_argument,  NULL,  0x1 },
     { 0, 0, 0, 0 }
  };
  int opt;
  bool opt_quiet,opt_verbose,convert_funcs,convert_bools;
  ATermAppl mu_spec,spec;

  ATinit(argc,argv,&bot);
  gsEnableConstructorFunctions();

  opt_quiet = false;
  opt_verbose = false;
  convert_funcs = true;
  convert_bools = true;
  while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
  {
    switch ( opt )
    {
      case 'h':
        print_help(stderr, argv[0]);
        return 0;
      case 'q':
        opt_quiet = true;
        break;
      case 'v':
        opt_verbose = true;
        break;
      case 'n':
        convert_funcs = false;
        break;
      case 0x1:
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
  if ( opt_quiet )
    gsSetQuietMsg();
  if ( opt_verbose )
    gsSetVerboseMsg();



  InStream = stdin;
  char *InFileName = NULL;
  if ( optind < argc && strcmp(argv[optind],"-") )
  {
    InFileName = argv[optind];
    gsVerboseMsg("reading mCRL LPE from '%s'...\n", InFileName);
    if ( (InStream = fopen(InFileName, "rb")) == NULL )
    {
      gsErrorMsg("cannot open input file '%s' for reading: ",
        InFileName);
      perror(NULL);
      return 1;
    }
  } else {
    gsVerboseMsg("reading mCRL LPE from stdin...\n");
  }

  mu_spec = (ATermAppl) ATreadFromFile(InStream);

  if (InStream != stdin) {
    fclose(InStream);
  }

  if ( mu_spec == NULL )
  {
    if (InStream == stdin) {
      gsErrorMsg("could not read mCRL LPE from stdin\n");
    } else {
      gsErrorMsg("could not read mCRL LPE from '%s'\n", InFileName);
    }
    return 1;
  }
  assert(mu_spec != NULL);

  if (!is_mCRL_spec(mu_spec)) {
    if (InStream == stdin) {
      gsErrorMsg("stdin does not contain a mCRL LPE\n");
    } else {
      gsErrorMsg("'%s' does not contain a mCRL LPE\n", InFileName);
    }
    return false;
  }
  assert(is_mCRL_spec(mu_spec));


  spec = translate(mu_spec,convert_bools,convert_funcs);


  OutStream = stdout;
  char *OutFileName = NULL;
  if ( optind+1 < argc )
  {
    OutFileName = argv[optind+1];
    gsVerboseMsg("writing mCRL2 LPE to '%s'...\n", OutFileName);
    if ( (OutStream = fopen(OutFileName, "wb")) == NULL )
    {
      gsErrorMsg("cannot open output file '%s' for writing: ",
        OutFileName);
      perror(NULL);
      return 1;
    }
  } else {
    gsVerboseMsg("writing mCRL2 LPE to stdout...\n");
  }

  ATwriteToBinaryFile((ATerm) spec,OutStream);

  if (OutStream != stdout) {
    fclose(OutStream);
  }

  return 0;
}
