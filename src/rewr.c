/* $Id: rewr.c,v 1.2 2005/03/09 15:46:00 muck Exp $ */

#define NAME "rewr"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsparse.h"
#include "libgsrewrite.h"

static bool gsIsLPESpec(ATermAppl Spec)
{
	return gsIsLPE(ATAgetArgument(Spec,5));
}

static ATermAppl rewrite_nolpe(ATermAppl Spec)
{
	Spec = ATsetArgument(Spec,(ATerm) gsRewriteTerm(ATAgetArgument(Spec,5)),5);
	Spec = ATsetArgument(Spec,(ATerm) gsRewriteTerm(ATAgetArgument(Spec,6)),6);

	return Spec;
}

static ATermAppl rewrite_lpe(ATermAppl Spec)
{
	return rewrite_nolpe(Spec);
}

int main(int argc, char **argv)
{
	FILE *SpecStream;
	ATerm stackbot;
	ATermAppl Spec;
	#define sopts "ai"
	struct option lopts[] = {
		{ "read-aterm",		no_argument,	NULL,	'a' },
		{ "inner",		no_argument,	NULL,	'i' },
		{ 0, 0, 0, 0 }
	};
	int opt,read_aterm,strat;

	ATinit(argc,argv,&stackbot);

	read_aterm = 0;
	strat = GS_REWR_INNER;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case 'a':
				read_aterm = 1;
				break;
			case 'i':
				strat = GS_REWR_INNER;
				break;
			default:
				break;
		}
	}

	if ( argc-optind < 1 )
	{
/*		fprintf(stderr,"%s: no input file supplied\n",NAME);
		return 1;*/
		SpecStream = stdin;
	} else {
		if ( (SpecStream = fopen(argv[optind],"r")) == NULL )
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
		Spec = gsParseSpecification(SpecStream);
		fclose(SpecStream);
	}

	gsRewriteInit(Spec,strat);

	if ( gsIsLPESpec(Spec) )
	{
		Spec = rewrite_lpe(Spec);
	} else {
		Spec = rewrite_nolpe(Spec);
	}

	gsPrintSpecification(stdout,Spec);
}
