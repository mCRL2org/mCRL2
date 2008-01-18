// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sim.cpp

#define NAME "sim"
#define AUTHOR "Muck van Weerdenburg"

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

#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/core/struct.h"
#include "mcrl2/lps/nextstate.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/utilities/version_info.h"

#include "simulator.h"

using namespace std;
using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;

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

char help_gsMessage[] = "During the simulation the following commands are accepted (short/long):\n"
		      "   N                - take transition N to the corresponding state\n"
		      "                      (where N is a number)\n"
		      "   u/undo           - go to previous state in trace\n"
		      "   r/redo           - go to next state in trace\n"
		      "   i/initial        - go to initial state (preserving trace)\n"
		      "   g/goto N         - go to position N in trace\n"
		      "   t/trace          - print trace (current state is indicated with '>')\n"
		      "   l/load FILENAME  - load trace from file FILENAME\n"
		      "   s/save FILENAME  - save trace to file FILENAME\n"
		      "   h/help           - print this help gsMessage\n"
		      "   q/quit           - quit\n";

void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... INFILE\n"
    "Simulate the LPS in INFILE via a text-based interface.\n"
    "\n"
    "%s"
    "\n"
    "Options:\n"
    "  -y, --dummy              replace free variables in the LPS with dummy values\n"
    "  -RNAME, --rewriter=NAME  use rewriter NAME (default 'jitty')\n"
    "  -h, --help               display this help and terminate\n"
    "      --version            display version information and terminate\n"
    "  -q, --quiet              do not display warning messages\n"
    "  -v, --verbose            display consise intermediate messages\n"
    "  -d, --debug              display detailed intermediate messages\n"
    "\n"
    "Report bugs at <http://www.mcrl2.org/issuetracker>.\n"
    , Name
    , help_gsMessage
  );
}

int main(int argc, char **argv)
{
    MCRL2_ATERM_INIT(argc, argv)

	FILE *SpecStream;
	#define sopts "hqvdyR:"
        #define version_option 0x1
	struct option lopts[] = {
		{ "help",	no_argument,	NULL,	'h' },
                { "version",    no_argument,    NULL,   version_option },
		{ "quiet",	no_argument,	NULL,	'q' },
		{ "verbose",    no_argument,	NULL,	'v' },
		{ "debug",	no_argument,	NULL,	'd' },
		{ "dummy",	no_argument,	NULL,	'y' },
		{ "rewriter",	no_argument,	NULL,	'R' },
		{ 0, 0, 0, 0 }
	};


	bool quiet = false;
	bool verbose = false;
	bool debug = false;
	bool usedummy = false;
	RewriteStrategy strat = GS_REWR_JITTY;
	int opt;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case 'h':
				print_help(stdout, argv[0]);
				return 0;
                        case version_option:
                                print_version_information(NAME, AUTHOR);
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
	ATermAppl Spec = (ATermAppl) ATreadFromFile(SpecStream);
	ATprotectAppl(&Spec);
	if ( Spec == NULL )
	{
                gsErrorMsg("could not read LPS from '%s'\n", SpecFileName);
                fclose(SpecStream);
		return 1;
	}
        assert(Spec != NULL);
        if (!gsIsSpecV1(Spec)) {
          gsErrorMsg("'%s' does not contain an LPS\n", SpecFileName);
          fclose(SpecStream);
          return false;
        }
        assert(gsIsSpecV1(Spec));

        StandardSimulator simulator;
        simulator.rewr_strat = strat;
        simulator.use_dummies = usedummy;
        simulator.LoadSpec(Spec);

	gsMessage("initial state: [ ");
	PrintState(simulator.GetState(),simulator.GetNextState());
	gsMessage(" ]\n\n");

	bool notdone = true;
	while ( notdone )
	{
                ATermList next_states = simulator.GetNextStates();
		int i = 0;
		for (ATermList l=next_states; !ATisEmpty(l); l=ATgetNext(l) )
		{
			ATermAppl Transition = ATAgetFirst(ATLgetFirst(l));
			ATerm NewState = ATgetFirst(ATgetNext(ATLgetFirst(l)));
			gsMessage("%i: %P  ->  [ ",i,Transition);
			PrintState(NewState,simulator.GetNextState());
			gsMessage(" ]\n\n");
			i++;
		}
		if ( simulator.ErrorOccurred() )
		{
			gsMessage("an error occurred while calculating the transitions from this state; this likely means that not all possible transitions are shown\n\n");
		}

		if ( ATisEmpty(next_states) )
		{
			printf("deadlock\n\n");
		}

		while ( true )
		{
			string s;

			(cout << "? ").flush();
			getline(cin, s);
			if ( (s.length() > 0) && (s[s.length()-1] == '\r') )
			{ // remove CR
				s.resize(s.length()-1);
			}
			
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
				if ( idx < (unsigned int) ATgetLength(next_states) )
				{
					gsMessage("\ntransition: %P\n\n",ATAgetFirst(ATLelementAt(next_states,idx)));
					gsMessage("current state: [ ");
					PrintState(simulator.GetState(),simulator.GetNextState());
					gsMessage(" ]\n\n");
					simulator.ChooseTransition(idx);
					break;
				} else {
					cout << "invalid transition index";
					if ( ATgetLength(next_states) > 0 )
					{
						cout << " " << idx << " (maximum is " << ATgetLength(next_states)-1 << ")";
					}
					cout << endl;
				}
			} else if ( (s == "i") || (s == "initial") )
			{
				simulator.SetTracePos(0);
				gsMessage("\ninitial state: [ ");
				PrintState(simulator.GetState(),simulator.GetNextState());
				gsMessage(" ]\n\n");
				break;
			} else if ( (s == "u") || (s == "undo") )
			{
				if ( simulator.GetTracePos() > 0 )
				{
					simulator.Undo();
					gsMessage("\ncurrent state: [ ");
					PrintState(simulator.GetState(),simulator.GetNextState());
					gsMessage(" ]\n\n");
					break;
				} else {
					cout << "already at start of trace" << endl;
				}
			} else if ( (s == "r") || (s == "redo") )
			{
				ATermAppl trans = simulator.GetNextTransitionFromTrace();
				if ( trans != NULL )
				{
					simulator.Redo();
					gsMessage("\ntransition: %P\n\n",trans);
					gsMessage("current state: [ ");
					PrintState(simulator.GetState(),simulator.GetNextState());
					gsMessage(" ]\n\n");
					break;
				} else {
					cout << "already at end of trace" << endl;
				}
			} else if ( (s.substr(0,2) == "g ") || (s.substr(0,5) == "goto ") )
			{
				if ( s.substr(0,2) == "g " )
				{
					s = s.substr(2);
				} else {
					s = s.substr(5);
				}
				int idx;
				sscanf(s.c_str(),"%i",&idx);
				if ( idx >= 0 && idx < simulator.GetTraceLength() )
				{
					simulator.SetTracePos(idx);
					gsMessage("\ncurrent state: [ ");
					PrintState(simulator.GetState(),simulator.GetNextState());
					gsMessage(" ]\n\n");
					break;
				} else {
					cout << "invalid trace position " << idx << " (maximum is " << (simulator.GetTraceLength()-1) << ")" << endl;
				}
			} else if ( (s == "t") || (s == "trace") )
			{
				gsMessage("\ncurrent trace:\n\n");
				ATermList trace = simulator.GetTrace();
				int pos = simulator.GetTracePos();
				for (int i=0; !ATisEmpty(trace); trace=ATgetNext(trace), ++i)
				{
					ATermAppl Transition = ATAgetFirst(ATLgetFirst(trace));
					ATerm NewState = ATgetFirst(ATgetNext(ATLgetFirst(trace)));
					gsMessage("%s %i: ",(i==pos)?">":" ",i);
					if ( i == 0 )
					{
						gsMessage("    ");
					} else {
						gsMessage("%P  ->",Transition);
					}
					gsMessage("  [ ",(i==pos)?">":" ",i,Transition);
					PrintState(NewState,simulator.GetNextState());
					gsMessage(" ]\n\n");
				}
			} else if ( (s.substr(0,2) == "s ") || (s.substr(0,5) == "save ") )
			{
				string filename;
				if ( s.substr(0,2) == "s " )
				{
					filename = s.substr(2);
				} else {
					filename = s.substr(5);
				}
				try
				{
					simulator.SaveTrace(filename);
					cout << "trace saved" << endl;
				} catch ( string err )
				{
					cout << "error saving trace: " << err << endl;
				}
			} else if ( (s.substr(0,2) == "l ") || (s.substr(0,5) == "load ") )
			{
				string filename;
				if ( s.substr(0,2) == "l " )
				{
					filename = s.substr(2);
				} else {
					filename = s.substr(5);
				}
				try
				{
					simulator.LoadTrace(filename);
					cout << "trace loaded" << endl;
					gsMessage("\ninitial state: [ ");
					PrintState(simulator.GetState(),simulator.GetNextState());
					gsMessage(" ]\n\n");
					break;
				} catch ( string err )
				{
					cout << "error loading trace: " << err << endl;
				}
			} else {
				cout << "unknown command (try 'h' for help)" << endl;
			}
		}
	}

	//delete nsgen;
	//delete nstate;
}
