#define NAME "lin"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include "gsfunc.h"
#include "gslowlevel.h"
#include "libprint_types.h"
#include "libprint_c.h"
#include "gslexer.h"
#include "typecheck.h"
#include "dataimpl.h"
#include "lin_alt.h"

void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... INFILE [OUTFILE]\n"
    "Linearises the mCRL2 specification in INFILE and writes the result to OUTFILE.\n"
    "If OUTFILE is not specified, INFILE with its extension replaced by 'lpe' is\n"
    "assumed. If OUTFILE is -, stdout used.\n"
    "The default is to generate a LPE.\n"
    "\n"
    "      --help            display this help message\n"
    "  -c, --nocluster       do not cluster the LPE\n"
    "  -l, --linear          do not write a LPE but a linear specification\n"
    "  -2, --linear2         same as -l, but substitute processes if possible\n"
    "  -r, --reuse-cycles    improves result of -2 but can take longer to calculate\n"
    "  -s, --state-space     generate state-space. Useful if the system is very\n"
    "                        complex in general, but because of the initial state\n"
    "                        has a small state-space.\n"
    "  -a, --read-aterm      INFILE is an ATerm\n"
    "  -h, --human           write the result in human readable format\n",
    Name);
}

int main(int argc, char **argv)
{
	FILE *SpecStream,*OutFile;
	ATerm stackbot;
	ATermAppl Spec;
	#define sopts "cl2rsah"
	#define HelpOption 1
	struct option lopts[] = {
		{ "help",		no_argument,	NULL,	HelpOption },
		{ "nocluster",		no_argument,	NULL,	'c' },
		{ "linear",		no_argument,	NULL,	'l' },
		{ "linear2",		no_argument,	NULL,	'2' },
		{ "reuse-cycles",	no_argument,	NULL,	'r' },
		{ "state-space",	no_argument,	NULL,	's' },
		{ "read-aterm",		no_argument,	NULL,	'a' },
		{ "human",		no_argument,	NULL,	'h' },
		{ 0, 0, 0, 0 }
	};
	int opt;
	bool cluster,linear,linear2,reuse,sspace,read_aterm,human;

	ATinit(argc,argv,&stackbot);

	cluster = true;
	linear = false;
	linear2 = false;
	reuse = false;
	sspace = false;
	read_aterm = false;
	human = false;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case HelpOption:
				print_help(stderr, argv[0]);
				return 0;
			case 'c':
				cluster = false;
				break;
			case 'l':
				linear = true;
				break;
			case '2':
				linear2 = true;
				break;
			case 'r':
				reuse = true;
				break;
			case 's':
				sspace = true;
				break;
			case 'a':
				read_aterm = true;
				break;
			case 'h':
				human = true;
				break;
			default:
				break;
		}
	}

	if ( argc-optind < 1 )
	{
		print_help(stderr, argv[0]);
		return 0;
	}

	if ( (SpecStream = fopen(argv[optind],"rb")) == NULL )
	{
		perror(NAME);
		return 1;
	}
	gsEnableConstructorFunctions();
	if ( read_aterm )
	{
		Spec = (ATermAppl) ATreadFromFile(SpecStream);
	} else {
                //parse specification
                gsVerboseMsg("parsing...\n");
                Spec = gsParse(SpecStream);
		fclose(SpecStream);
                if (Spec == NULL) {
                  gsErrorMsg("parsing failed\n");
                  return 1;
                }
                //type check specification
                gsVerboseMsg("type checking...\n");
                Spec = gsTypeCheck(Spec);
                if (Spec == NULL) {
                  gsErrorMsg("type checking failed\n");
                  return 1;
                }
                //implement standard data types and type constructors
                gsVerboseMsg("implementing standard data types and type constructors...\n");
                Spec = gsImplementData(Spec);
                if (Spec == NULL) {
                  gsErrorMsg("data implementation failed\n");
                  return 1;
                }
	}

	if ( argc-optind > 1 )
	{
		if ( !strcmp("-",argv[optind+1]) )
		{
			OutFile = stdout;
		} else {
			if ( (OutFile = fopen(argv[optind+1],"wb")) == NULL )
			{
				perror(NAME);
				return 1;
			}
		}
	} else {
		char s[100];
		bool b;

		strncpy(s,argv[optind],95);
		s[95] = 0;
		b = false;
		for (int i=strlen(s); i>=0; i--)
		{
			if ( s[i] == '.' )
			{
				s[++i] = 'l';
				s[++i] = 'p';
				s[++i] = 'e';
				s[++i] = 0;
				b = true;
				break;
			}
		}
		if ( !b )
		{
			int i = strlen(s)-1;
			s[++i] = '.';
			s[++i] = 'l';
			s[++i] = 'p';
			s[++i] = 'e';
			s[++i] = 0;
		}

		if ( (OutFile = fopen(s,"wb")) == NULL )
		{
			perror(NAME);
			return 1;
		}
	}

	if ( sspace )
	{
		Spec = linearise_alt_statespace(Spec,!(linear||linear2));
	} else {
		if ( linear )
		{
			Spec = linearise_alt_nolpe(Spec);
		} else if ( linear2 )
		{
			Spec = linearise_alt_nolpe_subst(Spec,reuse);
		} else {
                        t_lin_options lin_options;
			Spec = linearise_alt(Spec,lin_options);
		}
	}

	if ( human )
	{
		PrintPart_C(OutFile, (ATerm) Spec, ppAdvanced);
	} else {
		ATwriteToTextFile((ATerm) Spec,OutFile);
	}
}
