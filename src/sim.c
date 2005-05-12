/* $Id: sim.c,v 1.1 2005/05/03 15:44:47 muck Exp $ */

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

static void PrintState(ATermList state)
{
	for (; !ATisEmpty(state); state=ATgetNext(state))
	{
		if ( gsIsDataVarId(ATAgetFirst(state)) )
		{
			ATprintf("_");
		} else {
			gsPrintPart(stdout,ATgetFirst(state),0,0);
		}
		if ( !ATisEmpty(ATgetNext(state)) )
		{
			ATprintf(", ");
		}
	}
}

int main(int argc, char **argv)
{
	FILE *SpecStream;
	ATerm stackbot;
	ATermAppl Spec;
	ATermList state, states, l;
	#define sopts "d"
	struct option lopts[] = {
		{ "dummy",	no_argument,	NULL,	'd' },
		{ 0, 0, 0, 0 }
	};
	int opt, i, r;
	bool usedummy;

	ATinit(argc,argv,&stackbot);

	usedummy = false;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case 'd':
				usedummy = true;
				break;
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

	state = gsNextStateInit(Spec,!usedummy);

	ATprintf("initial state: [ ");
	PrintState(state);
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
			ATprintf("%i: ",i);
			gsPrintPart(stdout,ATgetFirst(ATLgetFirst(l)),0,0);
			ATprintf("  ->  [ ");
			PrintState(ATLgetFirst(ATgetNext(ATLgetFirst(l))));
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
		ATprintf("current state: [ ");
		PrintState(state);
		ATprintf(" ]\n\n");
	}
}
