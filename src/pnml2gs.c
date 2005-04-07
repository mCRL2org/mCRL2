/* $Id: pnml2gs.c,v 1.0 2005/03/09 15:46:00 ysu Exp $ */

#define NAME "pnml2gs"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "aterm2.h"
#include "libgsparse.h"
#include "gslowlevel.h"
#include "gsfunc.h"

static ATermAppl do_pnml2gs(ATermAppl Spec){
  return Spec;	
}

int main(int argc, char **argv)
{
	FILE *SpecStream;
	ATerm stackbot;
	ATermAppl Spec;
	#define sopts "ai"
	struct option lopts[] = {
		{ "read-aterm",		no_argument,	NULL,	'a' },
		{ 0, 0, 0, 0 }
	};
	int opt,read_aterm;

	ATinit(argc,argv,&stackbot);

	read_aterm = 0;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case 'a':
				read_aterm = 1;
				break;
			default:
				break;
		}
	}

	if ( argc-optind < 1 )
	{
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
		Spec = (ATermAppl) ATreadFromFile(SpecStream);
	} else {
		Spec = gsParseSpecification(SpecStream);
		fclose(SpecStream);
	}

	Spec=do_pnml2gs(Spec);
	gsPrintSpecification(stdout,Spec);
}

