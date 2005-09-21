/* $Id: sim.c,v 1.1 2005/05/03 15:44:47 muck Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

#define NAME "sim"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsparse.h"
#include "libgsnextstate.h"
#include "libgsrewrite.h"

static void gsPrintState(ATerm state)
{
	for (int i=0; i<gsGetStateLength(); i++)
	{
		if ( i > 0 )
		{
			ATprintf(", ");
		}

		ATermAppl a = gsGetStateArgument(state,i);
		if ( gsIsDataVarId(a) )
		{
			ATprintf("_");
		} else {
			gsprintf("%T",a);
		}
	}
}

void print_help(FILE *f, char *Name)
{
	fprintf(f,"Usage: %s OPTIONS LPEFILE\n",Name);
	fprintf(f,"Simulates the LPE in LPEFILE.\n"
		  "(Enter '-1' or use CTRL-d to end simulation.)\n"
		  "\n"
	          "The OPTIONS that can be used are:\n"
	          "-h, --help               Display this help message\n"
	          "-y, --dummy              Replace free variables in the LPE\n"
		  "                         with dummy values\n"
	          "-R, --rewriter name      Use rewriter 'name' (default inner3)\n"
      );
}

int main(int argc, char **argv)
{
	FILE *SpecStream;
	ATerm stackbot, state;
	ATermAppl Spec;
	ATermList states, l;
	#define sopts "hycrR"
	struct option lopts[] = {
		{ "help",	no_argument,	NULL,	'h' },
		{ "dummy",	no_argument,	NULL,	'y' },
		{ "rewriter",	no_argument,	NULL,	'R' },
		{ 0, 0, 0, 0 }
	};
	int opt, i, r;
	RewriteStrategy strat;
	bool usedummy;
	char *rw_arg;

	ATinit(argc,argv,&stackbot);

	usedummy = false;
	strat = GS_REWR_INNER3;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case 'h':
				print_help(stderr, argv[0]);
				return 0;
			case 'y':
				usedummy = true;
				break;
			case 'R':
				if ( optarg == NULL )
				{
					rw_arg = argv[optind++];
				} else {
					rw_arg = optarg;
				}
				if ( !strcmp(rw_arg,"inner") )
				{
					strat = GS_REWR_INNER;
				} else if ( !strcmp(rw_arg,"inner2") )
				{
					strat = GS_REWR_INNER2;
				} else if ( !strcmp(rw_arg,"inner3") )
				{
					strat = GS_REWR_INNER3;
				} else if ( !strcmp(rw_arg,"innerc") )
				{
					strat = GS_REWR_INNERC;
				} else if ( !strcmp(rw_arg,"innerc2") )
				{
					strat = GS_REWR_INNERC2;
				} else if ( !strcmp(rw_arg,"jitty") )
				{
					strat = GS_REWR_JITTY;
				} else {
					fprintf(stderr,"warning: unknown rewriter '%s', using default\n",rw_arg);
					strat = GS_REWR_INNER3;
				}
				break;
			default:
				break;
		}
	}

	if ( argc-optind < 1 )
	{
		print_help(stderr, argv[0]);
		return 1;
	}

	if ( (SpecStream = fopen(argv[optind],"r")) == NULL )
	{
		perror(NAME);
		return 1;
	}
	gsEnableConstructorFunctions();
	Spec = (ATermAppl) ATreadFromFile(SpecStream);
	if ( Spec == NULL )
	{
		return 1;
	}

	state = gsNextStateInit(Spec,!usedummy,GS_STATE_VECTOR,strat);

	ATprintf("initial state: [ ");
	gsPrintState(state);
	ATprintf(" ]\n\n");

	while ( 1 )
	{
		states = gsNextState(state,NULL);
		if ( ATisEmpty(states) )
		{
			printf("deadlock\n\n");
		}
		for (l=states,i=0; !ATisEmpty(l); l=ATgetNext(l), i++)
		{
			gsprintf("%i: %T  ->  [ ",i,ATAgetFirst(ATLgetFirst(l)));
			gsPrintState(ATgetFirst(ATgetNext(ATLgetFirst(l))));
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
		gsprintf("\ntransition: %T\n\n",ATAgetFirst(ATLelementAt(states,i)));
		state = ATgetFirst(ATgetNext(ATLelementAt(states,i)));
		ATprintf("current state: [ ");
		gsPrintState(state);
		ATprintf(" ]\n\n");
	}
}

#ifdef __cplusplus
}
#endif
