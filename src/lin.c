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
	fprintf(f,"Usage: %s OPTIONS SPECFILE\n",NAME);
	fprintf(f,"Linearises SPECFILE and writes the result to stdout.\n"
		  "The default is to generate a LPE.\n"
	          "\n"
	          "The OPTIONS that can be used are:\n"
	          "    --help               display this help message\n"
	          "-c, --nocluster          Do not cluster the LPE\n"
	          "-h, --human              Do not write a LPE but a linear\n"
		  "                         specification\n"
	          "-2, --human2             Same as -h, but substitute\n"
		  "                         processes if possible\n"
	          "-r, --reuse-cycles       Improves result of -2 but can take\n"
		  "                         longer to calculate\n"
	          "-a, --read-aterm         SPECFILE is an ATerm\n"
	          "-w, --write-aterm        Write the result as an ATerm\n"
	       );
}

int main(int argc, char **argv)
{
	FILE *SpecStream;
	ATerm stackbot;
	ATermAppl Spec;
	#define sopts "ch2raw"
	#define HelpOption 1
	struct option lopts[] = {
		{ "help",		no_argument,	NULL,	HelpOption },
		{ "nocluster",		no_argument,	NULL,	'c' },
		{ "human",		no_argument,	NULL,	'h' },
		{ "human2",		no_argument,	NULL,	'2' },
		{ "reuse-cycles",	no_argument,	NULL,	'r' },
		{ "read-aterm",		no_argument,	NULL,	'a' },
		{ "write-aterm",	no_argument,	NULL,	'w' },
		{ 0, 0, 0, 0 }
	};
	int opt,cluster,human,reuse,read_aterm,write_aterm;

	ATinit(argc,argv,&stackbot);

	cluster = 1;
	human = 0;
	reuse = 0;
	read_aterm = 0;
	write_aterm = 0;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case HelpOption:
				print_help(stderr);
				return 0;
			case 'c':
				cluster = 0;
				break;
			case 'h':
				human = 1;
				break;
			case '2':
				human = 2;
				break;
			case 'r':
				reuse = 1;
				break;
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

	switch ( human )
	{
		case 1:
			Spec = gsLinearise2_nolpe(Spec);
			break;
		case 2:
			Spec = gsLinearise2_nolpe_subst(Spec,reuse);
			break;
		case 0:
			Spec = gsLinearise2(Spec,cluster);
			break;
	}

	if ( write_aterm )
	{
		ATwriteToTextFile((ATerm) Spec,stdout);
	} else {
		gsPrintSpecification(stdout,Spec);
	}
}
