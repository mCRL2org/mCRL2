#define NAME "tbf2lpe"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include "gsfunc.h"
#include "libgsparse.h"
#include "lpetrans.h"

void print_help(FILE *f)
{
  fprintf(f,
    "Usage: %s OPTIONS [SPECFILE [OUTFILE]]\n", NAME);
  fprintf(f,
    "Read mCRL LPE from SPECFILE, convert it to a mCRL2 LPE and save the result to\n"
    "OUTFILE. If OUTFILE is not present, stdout is used. If SPECFILE is not present,\n"
    "stdin is used. To use stdin and save the output to a file, use '-' for SPECFILE.\n"
  "\n"
  "The OPTIONS that can be used are:\n"
  "-h, --help               display this help message\n"
  "-q, --quiet              do not print any unrequested\n"
  "                         information\n"
      );
}

int main(int argc, char **argv)
{
	FILE *SpecStream, *OutStream;
	ATerm bot;
	#define sopts "hq"
	struct option lopts[] = {
		{ "help",		no_argument,	NULL,	'h' },
		{ "quiet",		no_argument,	NULL,	'q' },
 		{ 0, 0, 0, 0 }
	};
	int opt;
	bool quiet;
	ATerm mu_spec,spec;

	ATinit(argc,argv,&bot);
	gsEnableConstructorFunctions();

	quiet = false;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case 'h':
				print_help(stderr);
				return 0;
			case 'q':
				quiet = true;
				break;
			default:
				break;
		}
	}

	SpecStream = stdin;
	if ( optind < argc && strcmp(argv[optind],"-") )
	{
		if ( (SpecStream = fopen(argv[optind],"r")) == NULL )
		{
			if ( !quiet )
			{
				perror(NAME);
			}
			return 1;
		}
	}

	OutStream = stdout;
	if ( optind+1 < argc )
	{
		if ( (OutStream = fopen(argv[optind+1],"w")) == NULL )
		{
			if ( !quiet )
			{
				perror(NAME);
			}
			return 1;
		}
	}

	mu_spec = ATreadFromFile(SpecStream);
	if ( mu_spec == NULL )
	{
		return 1;
	}
	spec = (ATerm) translate((ATermAppl) mu_spec);
	ATwriteToBinaryFile(spec,OutStream);

  	return 0;
}
