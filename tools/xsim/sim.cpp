// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sim.cpp

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

#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "libstruct.h"
#include "libnextstate.h"
#include "librewrite.h"

using namespace std;
using namespace ::mcrl2::utilities;

static void PrintState(ATerm state, NextState *ns)
{
	for (int i=0; i<ns->getStateLength(); i++)
	{
		if ( i > 0 )
		{
			gsMessage(", ");
		}

		ATermAppl a = ns->getStateArgument(state,i);
		if ( gsIsDataVarId(a) )
		{
			gsMessage("_");
		} else {
			gsMessage("%P",a);
		}
	}
}

char help_gsMessage[] = "During the simulation the following commands are accepted:\n"
		      "   N - take transition N to the corresponding state (where N is a number)\n"
		      "   h - print this help gsMessage\n"
		      "   q - quit\n";

void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... INFILE\n"
    "Simulate the LPS in INFILE via a text-based interface.\n"
    "\n"
    "%s"
    "\n"
    "The following command line options are available.\n"
    "  -h, --help               display this help gsMessage\n"
    "  -q, --quiet              do not display any unrequested information\n"
    "  -v, --verbose            display consise intermediate gsMessages\n"
    "  -d, --debug              display detailed intermediate gsMessages\n"
    "  -y, --dummy              replace free variables in the LPS with dummy values\n"
    "  -RNAME, --rewriter=NAME  use rewriter NAME (default 'inner')\n",
    Name,
    help_gsMessage
  );
}

int main(int argc, char **argv)
{
	FILE *SpecStream;
	ATerm stackbot, state = NULL;
	ATermAppl Spec = NULL;
	ATermList states = NULL;
	#define sopts "hqvdyR:"
	struct option lopts[] = {
		{ "help",	no_argument,	NULL,	'h' },
		{ "quiet",	no_argument,	NULL,	'q' },
		{ "verbose",    no_argument,	NULL,	'v' },
		{ "debug",	no_argument,	NULL,	'd' },
		{ "dummy",	no_argument,	NULL,	'y' },
		{ "rewriter",	no_argument,	NULL,	'R' },
		{ 0, 0, 0, 0 }
	};

	ATinit(argc,argv,&stackbot);
	ATprotectAppl(&state);
	ATprotectAppl(&Spec);
	ATprotectList(&states);

	bool quiet = false;
	bool verbose = false;
	bool debug = false;
	bool usedummy = false;
	RewriteStrategy strat = GS_REWR_INNER;
	int opt;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case 'h':
				print_help(stderr, argv[0]);
				return 0;
			case 'q':
				quiet = true;
				break;
			case 'v':
				verbose = true;
				break;
			case 'd':
				debug = true;
				break;
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

	if ( quiet && verbose )
	{
		gsErrorMsg("options -q/--quiet and -v/--verbose cannot be used together\n");
		return false;
	}
	if ( quiet && debug )
	{
		gsErrorMsg("options -q/--quiet and -d/--debug cannot be used together\n");
		return false;
	}
	if ( quiet )
	{
		gsSetQuietMsg();
	}
	if ( verbose )
	{
		gsSetVerboseMsg();
	}
	if ( debug )
	{
		gsSetDebugMsg();
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
                gsErrorMsg("could not read LPS from '%s'\n", SpecFileName);
                fclose(SpecStream);
		return 1;
	}
        assert(Spec != NULL);
        gsEnableConstructorFunctions();
        if (!gsIsSpecV1(Spec)) {
          gsErrorMsg("'%s' does not contain an LPS\n", SpecFileName);
          fclose(SpecStream);
          return false;
        }
        assert(gsIsSpecV1(Spec));

	NextState *nstate = createNextState(Spec,!usedummy,GS_STATE_VECTOR,strat);
	state = nstate->getInitialState();

	gsMessage("initial state: [ ");
	PrintState(state,nstate);
	gsMessage(" ]\n\n");

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
			gsMessage("%i: %P  ->  [ ",i,Transition);
			PrintState(NewState,nstate);
			gsMessage(" ]\n\n");

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
				cout << help_gsMessage;
			} else if ( isdigit(s[0]) ) {
				unsigned int idx;
				sscanf(s.c_str(),"%i",&idx);
				if ( idx < (unsigned int) ATgetLength(states) )
				{
					gsMessage("\ntransition: %P\n\n",ATAgetFirst(ATLelementAt(states,idx)));
					state = ATgetFirst(ATgetNext(ATLelementAt(states,idx)));
					gsMessage("current state: [ ");
					PrintState(state,nstate);
					gsMessage(" ]\n\n");
					break;
				} else {
					cout << "invalid transition index";
					if ( ATgetLength(states) > 0 )
					{
						cout << " " << idx << " (maximum is " << ATgetLength(states)-1 << ")";
					}
					cout << endl;
				}
			} else {
				cout << "unknown command (try 'h' for help)" << endl;
			}
		}
	}

	delete nsgen;
	delete nstate;
}
