/* $Id: gsrewr.c,v 1.2 2005/04/08 12:33:51 muck Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

#define NAME "gsalpha"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsparse.h"
#include "libgsalpha.h"

extern ATermAppl gsParse(FILE *);
extern ATermAppl gsTypeCheck(ATermAppl);

void print_help(FILE *f)
{
	fprintf(f,"Usage: %s OPTIONS [SPECFILE [OUTFILE]]\n",NAME);
	fprintf(f,"Apply alphabet axioms to processes in SPECFILE and save\n"
		  "the result to OUTFILE. If OUTFILE is not present, stdout\n"
		  "is used. If SPECFILE is not present, stdin is used. To\n"
		  "use stdin and save the output into a file, use '-' for\n"
		  "SPECFILE.\n"
	          "\n"
	          "The OPTIONS that can be used are:\n"
	          "-h, --help               display this help message\n"
	          "-a, --read-aterm         SPECFILE is an ATerm\n"
	          "-w, --write-aterm        OUTFILE should be an ATerm\n"
	       );
}

int main(int argc, char **argv)
{
	FILE *SpecStream, *OutStream;
	ATerm stackbot;
	ATermAppl Spec;
	#define sopts "haw"
	struct option lopts[] = {
		{ "help",		no_argument,	NULL,	'h' },
		{ "read-aterm",		no_argument,	NULL,	'a' },
		{ "write-aterm",	no_argument,	NULL,	'w' },
		{ 0, 0, 0, 0 }
	};
	int opt,read_aterm,write_aterm;

	ATinit(argc,argv,&stackbot);

	read_aterm = 0;
	write_aterm = 0;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case 'h':
				print_help(stderr);
				return 0;
			case 'a':
				read_aterm = 1;
				break;
			case 'w':
				write_aterm = 1;
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
			perror(NAME);
			return 1;
		}
	}

	OutStream = stdout;
	if ( optind+1 < argc )
	{
		if ( (OutStream = fopen(argv[optind+1],"w")) == NULL )
		{
			perror(NAME);
			return 1;
		}
	}

	if ( read_aterm )
	{
		gsEnableConstructorFunctions();
		Spec = (ATermAppl) ATreadFromFile(SpecStream);
	} else {
		gsEnableConstructorFunctions();
		Spec = gsParse(SpecStream);
		Spec = gsTypeCheck(Spec);
		//Spec = gsParseSpecification(SpecStream);
		fclose(SpecStream);
	}
	if ( Spec == NULL )
	{
		return 1;
	}

	Spec = Alpha(Spec);

	if ( write_aterm )
	{
		ATwriteToTextFile((ATerm) Spec,OutStream);
	} else {
		gsPrintSpecification(OutStream,Spec);
	}
	if ( OutStream != stdout )
	{
		fclose(OutStream);
	}
}

#ifdef __cplusplus
}
#endif
