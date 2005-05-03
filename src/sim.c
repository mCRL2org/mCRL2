/* $Id: rewr.c,v 1.2 2005/03/09 15:46:00 muck Exp $ */

#define NAME "sim"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsparse.h"
#include "libgsnextstate.h"

int main(int argc, char **argv)
{
	FILE *SpecStream;
	ATerm stackbot;
	ATermAppl Spec;
	ATermList state, states, l;
	#define sopts ""
	struct option lopts[] = {
		{ 0, 0, 0, 0 }
	};
	int opt, i, r;

	ATinit(argc,argv,&stackbot);

	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			default:
				break;
		}
	}

	if ( argc-optind < 1 )
	{
		fprintf(stderr,"Usage: %s <lpe file>\n",NAME);
		return 1;
	}

	if ( (SpecStream = fopen(argv[optind],"r")) == NULL )
	{
		perror(NAME);
		return 1;
	}
	gsEnableConstructorFunctions();
	Spec = (ATermAppl) ATreadFromFile(SpecStream);

	state = gsNextStateInit(Spec);

//	ATprintf("initial state: %t\n\n",state);
	ATprintf("initial state: [ ");
	gsPrintParts(stdout,state,0,0,"",", ");
	ATprintf(" ]\n\n");

	while ( 1 )
	{
		states = gsNextState(state);
		if ( ATisEmpty(states) )
		{
			printf("deadlock\n\n");
		}
		for (l=states,i=0; !ATisEmpty(l); l=ATgetNext(l), i++)
		{
//			ATprintf("%i: %t  ->  %t\n\n",i,ATgetFirst(ATLgetFirst(l)),ATgetFirst(ATgetNext(ATLgetFirst(l))));
			ATprintf("%i: ",i);
			gsPrintPart(stdout,ATgetFirst(ATLgetFirst(l)),0,0);
			ATprintf("  ->  [ ");
			gsPrintParts(stdout,ATgetFirst(ATgetNext(ATLgetFirst(l))),0,0,"",", ");
			ATprintf(" ]\n\n");
		}
harm:
		printf("? "); fflush(stdout);
		r = 0;
		while ( r == 0 )
		{
			if ( (r = scanf("%i",&i)) == EOF )
			{
				r = i = -1;
			} else {
				fgetc(stdin);
			}
		}
		if ( i < 0 )
			break;
		if ( i >= ATgetLength(states) )
		{
			goto harm;
		}
		ATprintf("\ntransition: ");
		gsPrintPart(stdout,ATgetFirst(ATLelementAt(states,i)),0,0);
		ATprintf("\n\n");
		state = ATLgetFirst(ATgetNext(ATLelementAt(states,i)));
//	ATprintf("current state: %t\n\n",state);
		ATprintf("current state: [ ");
		gsPrintParts(stdout,state,0,0,"",", ");
		ATprintf(" ]\n\n");
	}
}
