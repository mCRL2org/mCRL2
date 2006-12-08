#define NAME "sim"

#include <iostream>
#include <string>
#include <cctype>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include <assert.h>

#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libnextstate.h"
#include "librewrite.h"

using namespace std;

static void PrintState(ATerm state, NextState *ns)
{
	for (int i=0; i<ns->getStateLength(); i++)
	{
		if ( i > 0 )
		{
			gsprintf(", ");
		}

		ATermAppl a = ns->getStateArgument(state,i);
		if ( gsIsDataVarId(a) )
		{
			gsprintf("_");
		} else {
			gsprintf("%P",a);
		}
	}
}

char help_message[] = "During the simulation the following commands are accepted:\n"
		      "   N - take transition N to the corresponding state (where N is a number)\n"
		      "   h - print this help message\n"
		      "   q - quit\n";

void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... INFILE\n"
    "Simulate the LPE in INFILE via a text-based interface.\n"
    "\n"
    "%s"
    "\n"
    "The following command line options are available. Mandatory arguments to long\n"
    "options are mandatory for short options too.\n"
    "  -h, --help            display this help message\n"
    "  -y, --dummy           replace free variables in the LPE with dummy values\n"
    "  -R, --rewriter=NAME   use rewriter NAME (default 'inner')\n",
    Name,
    help_message
  );
}

int main(int argc, char **argv)
{
	FILE *SpecStream;
	ATerm stackbot, state;
	ATermAppl Spec;
	ATermList states;
	#define sopts "hyR:"
	struct option lopts[] = {
		{ "help",	no_argument,	NULL,	'h' },
		{ "dummy",	no_argument,	NULL,	'y' },
		{ "rewriter",	no_argument,	NULL,	'R' },
		{ 0, 0, 0, 0 }
	};
	int opt;
	RewriteStrategy strat;
	bool usedummy;

	ATinit(argc,argv,&stackbot);

	usedummy = false;
	strat = GS_REWR_INNER;
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

	NextState *nstate = createNextState(Spec,!usedummy,GS_STATE_VECTOR,strat);
	state = nstate->getInitialState();

	gsprintf("initial state: [ ");
	PrintState(state,nstate);
	gsprintf(" ]\n\n");

	NextStateGenerator *nsgen = NULL;
	bool notdone = true;
	while ( notdone )
	{
		nsgen = nstate->getNextStates(state,nsgen);

		ATermAppl Transition;
		ATerm NewState;
		states = ATmakeList0();
		int i = 0;
		while ( nsgen->next(&Transition,&NewState) )
		{
			gsprintf("%i: %P  ->  [ ",i,Transition);
			PrintState(NewState,nstate);
			gsprintf(" ]\n\n");

			states = ATinsert(states,(ATerm) ATmakeList2((ATerm) Transition,NewState));
			i++;
		}
		states = ATreverse(states);

		if ( ATisEmpty(states) )
		{
			printf("deadlock\n\n");
		}

		while ( true )
		{
			string s;

			(cout << "? ").flush();
			cin >> s;
			
			if ( cin.eof() || (s == "q") || (s == "quit") )
			{
				if ( cin.eof() )
					cout << endl;	
				notdone = false;
				break;
			} else if ( (s == "h") || (s == "help") ) {
				cout << help_message;
			} else if ( isdigit(s[0]) ) {
				unsigned int idx;
				sscanf(s.c_str(),"%i",&idx);
				if ( idx < (unsigned int) ATgetLength(states) )
				{
					gsprintf("\ntransition: %P\n\n",ATAgetFirst(ATLelementAt(states,idx)));
					state = ATgetFirst(ATgetNext(ATLelementAt(states,idx)));
					gsprintf("current state: [ ");
					PrintState(state,nstate);
					gsprintf(" ]\n\n");
					break;
				} else {
					cout << "invalid transition index " << idx << " (maximum is " << ATgetLength(states)-1 << ")" << endl;
				}
			} else {
				cout << "unknown command (try 'h' for help)" << endl;
			}
		}
	}

	delete nsgen;
	delete nstate;
}
