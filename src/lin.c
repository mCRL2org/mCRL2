/* $Id: lin.c,v 1.8 2005/05/05 16:59:02 muck Exp $ */

#define NAME "lin"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "aterm2.h"
#include "gsfunc.h"
#include "libgsparse.h"
#include "gslinearise2.h"

void print_help(FILE *f)
{
	fprintf(f,"Usage: %s OPTIONS SPECFILE [OUTFILE]\n",NAME);
	fprintf(f,"Linearises SPECFILE and writes the result to OUTFILE. If\n"
		  "OUTFILE is not specified, SPECFILE with its extension\n"
		  "replaced by 'lpe' is assumed. If OUTFILE is -, stdout is\n"
		  "used.\n"
		  "The default is to generate a LPE.\n"
	          "\n"
	          "The OPTIONS that can be used are:\n"
	          "    --help               display this help message\n"
	          "-c, --nocluster          Do not cluster the LPE\n"
	          "-l, --linear             Do not write a LPE but a linear\n"
		  "                         specification\n"
	          "-2, --linear2            Same as -l, but substitute\n"
		  "                         processes if possible\n"
	          "-r, --reuse-cycles       Improves result of -2 but can take\n"
		  "                         longer to calculate\n"
	          "-a, --read-aterm         SPECFILE is an ATerm\n"
	          "-h, --human              Write the result in human readable\n"
		  "                         format\n"
	       );
}

int main(int argc, char **argv)
{
	FILE *SpecStream,*OutFile;
	ATerm stackbot;
	ATermAppl Spec;
	#define sopts "cl2rah"
	#define HelpOption 1
	struct option lopts[] = {
		{ "help",		no_argument,	NULL,	HelpOption },
		{ "nocluster",		no_argument,	NULL,	'c' },
		{ "linear",		no_argument,	NULL,	'l' },
		{ "linear2",		no_argument,	NULL,	'2' },
		{ "reuse-cycles",	no_argument,	NULL,	'r' },
		{ "read-aterm",		no_argument,	NULL,	'a' },
		{ "human",		no_argument,	NULL,	'h' },
		{ 0, 0, 0, 0 }
	};
	int opt;
	bool cluster,linear,linear2,reuse,read_aterm,human;

	ATinit(argc,argv,&stackbot);

	cluster = true;
	linear = false;
	linear2 = false;
	reuse = false;
	read_aterm = false;
	human = false;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case HelpOption:
				print_help(stderr);
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
		print_help(stderr);
		return 0;
	}

	if ( (SpecStream = fopen(argv[optind],"r")) == NULL )
	{
		perror(NAME);
		return 1;
	}
	if ( read_aterm )
	{
		gsEnableConstructorFunctions();
		Spec = (ATermAppl) ATreadFromFile(SpecStream);
	} else {
		Spec = gsParseSpecification(SpecStream);
		fclose(SpecStream);
	}

	if ( argc-optind > 1 )
	{
		if ( !strcmp("-",argv[optind+1]) )
		{
			OutFile = stdout;
		} else {
			if ( (OutFile = fopen(argv[optind+1],"w")) == NULL )
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

		if ( (OutFile = fopen(s,"w")) == NULL )
		{
			perror(NAME);
			return 1;
		}
	}

	if ( linear )
	{
		Spec = gsLinearise2_nolpe(Spec);
	} else if ( linear2 )
	{
		Spec = gsLinearise2_nolpe_subst(Spec,reuse);
	} else {
		Spec = gsLinearise2(Spec,cluster);
	}

	if ( human )
	{
		gsPrintSpecification(OutFile,Spec);
	} else {
		ATwriteToTextFile((ATerm) Spec,OutFile);
	}
}
