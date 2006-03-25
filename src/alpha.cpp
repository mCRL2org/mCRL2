#define NAME      "alpha"
#define LVERSION  "0.1"
#define AUTHOR    "Yaroslav S. Usenko and Muck van Weerdenburg"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <aterm2.h>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_types.h"
#include "libprint_c.h"
#include "libparse.h"
#include "typecheck.h"
#include "libalpha.h"

using namespace std;

void PrintUsage(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
    "Apply alphabet axioms to processes in INFILE and save the result to OUTFILE.\n"
    "If OUTFILE is not present, stdout is used. If INFILE is not present, stdin is\n"
    "used. To use stdin and save the output into a file, use '-' for INFILE.\n"
    "\n"
    "  -a, --read-aterm      INFILE is an ATerm\n"
    "  -w, --write-aterm     OUTFILE should be an ATerm\n"
    "  -h, --help            display this help\n"
    "      --version         display version information\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         turn on the display of short intermediate messages\n"
    "  -d, --debug           turn on the display of detailed intermediate messages\n",
    Name);
}

void PrintMoreInfo(FILE *Stream, char *Name) {
  fprintf(Stream, "Try \'%s --help\' for more information.\n", Name);
}

void PrintVersion(FILE *Stream) {
  fprintf(Stream, "%s %s\nWritten by %s.\n",
    NAME, LVERSION, AUTHOR);
}


int main(int argc, char **argv)
{
        FILE *OutStream;
	ATerm stackbot;
	ATermAppl Spec = NULL;
        #define sopts      "aw:hqvd"
        #define VersionOption     CHAR_MAX + 1
	struct option lopts[] = {
	  { "read-aterm" , no_argument,	      NULL, 'a' },
	  { "write-aterm", no_argument,	      NULL, 'w' },
	  { "help"       , no_argument,       NULL, 'h'},
	  { "version"    , no_argument,       NULL, VersionOption},
	  { "quiet"      , no_argument,       NULL, 'q'},
	  { "verbose"    , no_argument,       NULL, 'v'},
	  { "debug"      , no_argument,       NULL, 'd'},
	  { 0, 0, 0, 0 }
	};
	int opt,read_aterm,write_aterm;

	ATinit(argc,argv,&stackbot);
	gsEnableConstructorFunctions();

	read_aterm = 0;
	write_aterm = 0;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	  {
	    switch ( opt )
	      {
	      case 'a':
		read_aterm = 1;
		break;
	      case 'w':
		write_aterm = 1;
		break;
	      case 'h':
		PrintUsage(stderr, argv[0]);
		return 0;
	      case VersionOption:
		PrintVersion(stderr);
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
		break;
	      }
	  }

	OutStream = stdout;
	if ( optind+1 < argc )
	{
		if ( (OutStream = fopen(argv[optind+1],"wb")) == NULL )
		{
			perror(NAME);
			return 1;
		}
	}

	if ( read_aterm )
	{
		FILE *SpecStream = stdin;
		if ( optind < argc && strcmp(argv[optind],"-") )
		{
			if ( (SpecStream = fopen(argv[optind],"rb")) == NULL )
			{
				perror(NAME);
				return 1;
			}
		}

		Spec = (ATermAppl) ATreadFromFile(SpecStream);

		fclose(SpecStream);
	} else {
		if ( optind < argc && strcmp(argv[optind],"-") )
		{
			ifstream is(argv[optind]);
			if ( !is.is_open() )
			{
				fprintf(stderr,"error: cannot open input file '%s'\n",argv[optind]);
				return 1;
			}
			Spec = parse_specification(is);
			is.close();
		} else {
			Spec = parse_specification(cin);
		}
		Spec = gsTypeCheck(Spec);
	}
	if ( Spec == NULL )
	{
		return 1;
	}

	Spec = gsAlpha(Spec);

	if ( write_aterm )
	{
		ATwriteToTextFile((ATerm) Spec,OutStream);
	} else {
		PrintPart_C(OutStream, (ATerm) Spec, ppDefault);
	}
	if ( OutStream != stdout )
	{
		fclose(OutStream);
	}
}
