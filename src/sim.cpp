#define NAME "sim"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include <assert.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libprint_c.h"
#include "libgsnextstate.h"
#include "libgsrewrite.h"

static void gsPrintState(ATerm state)
{
	for (int i=0; i<gsGetStateLength(); i++)
	{
		if ( i > 0 )
		{
			gsprintf(", ");
		}

		ATermAppl a = gsGetStateArgument(state,i);
		if ( gsIsDataVarId(a) )
		{
			gsprintf("_");
		} else {
			gsprintf("%P",a);
		}
	}
}

void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... INFILE\n"
    "Simulates the LPE in INFILE.\n"
    "To end simulation, enter '-1' or press CTRL-d.\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -h, --help            display this help message\n"
    "  -y, --dummy           replace free variables in the LPE with dummy values\n"
    "  -R, --rewriter=NAME   use rewriter NAME (default 'inner3')\n",
    Name
  );
}

int main(int argc, char **argv)
{
	FILE *SpecStream;
	ATerm stackbot, state;
	ATermAppl Spec;
	ATermList states, l;
	#define sopts "hyR:"
	struct option lopts[] = {
		{ "help",	no_argument,	NULL,	'h' },
		{ "dummy",	no_argument,	NULL,	'y' },
		{ "rewriter",	no_argument,	NULL,	'R' },
		{ 0, 0, 0, 0 }
	};
	int opt, i, r;
	RewriteStrategy strat;
	bool usedummy;

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
				strat = RewriteStrategyFromString(optarg);
				if ( strat == GS_REWR_INVALID )
				{
					gsErrorMsg("invalid rewrite strategy '%s'\n",optarg);
					return 1;
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

        char *SpecFileName = argv[optind];
	if ( (SpecStream = fopen(SpecFileName, "rb")) == NULL )
	{
                gsErrorMsg("could not open input file '%s' for reading: ",
                  argv[optind]);
		perror(NULL);
		return 1;
	}
	Spec = (ATermAppl) ATreadFromFile(SpecStream);
	if ( Spec == NULL )
	{
                gsErrorMsg("could not read LPE from '%s'\n", SpecFileName);
                fclose(SpecStream);
		return 1;
	}
        assert(Spec != NULL);
        gsEnableConstructorFunctions();
        if (!gsIsSpecV1(Spec)) {
          gsErrorMsg("'%s' does not contain an LPE\n", SpecFileName);
          fclose(SpecStream);
          return false;
        }
        assert(gsIsSpecV1(Spec));

	state = gsNextStateInit(Spec,!usedummy,GS_STATE_VECTOR,strat);

	gsprintf("initial state: [ ");
	gsPrintState(state);
	gsprintf(" ]\n\n");

	while ( 1 )
	{
		states = gsNextState(state,NULL);
		if ( ATisEmpty(states) )
		{
			printf("deadlock\n\n");
		}
		for (l=states,i=0; !ATisEmpty(l); l=ATgetNext(l), i++)
		{
			gsprintf("%i: %P  ->  [ ",i,ATAgetFirst(ATLgetFirst(l)));
			gsPrintState(ATgetFirst(ATgetNext(ATLgetFirst(l))));
			gsprintf(" ]\n\n");
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
		gsprintf("\ntransition: %P\n\n",ATAgetFirst(ATLelementAt(states,i)));
		state = ATgetFirst(ATgetNext(ATLelementAt(states,i)));
		gsprintf("current state: [ ");
		gsPrintState(state);
		gsprintf(" ]\n\n");
	}
}
