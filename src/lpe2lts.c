#ifdef __cplusplus
extern "C" {
#endif

#define NAME "lpe2lts"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include "svc/svc.h" //XXX
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libprint_c.h"
#include "libgsnextstate.h"
#include "libgsrewrite.h"

#define OF_UNKNOWN	0
#define OF_AUT		1
#define OF_SVC		2

static unsigned long num_states;
static unsigned long trans;
static unsigned long level;
static unsigned long max_states;
static bool trace;
static bool monitor;
static unsigned long current_state;
static ATermIndexedSet states;
static ATermTable backpointers;
static bool deadlockstate;
static ATerm *orig_state;
static int outformat;
static bool outinfo;
static FILE *aut;
static SVCfile svcf, *svc;
static SVCparameterIndex svcparam;

static void gsinst_callback(ATermAppl transition, ATerm state)
{
	ATbool new_state;
	unsigned long i;

	deadlockstate = false;

	i = ATindexedSetPut(states, state, &new_state);
	if ( new_state )
	{
		if ( (max_states == 0) || (num_states < max_states) )
		{
			num_states++;
			if ( trace )
			{
				ATtablePut(backpointers, state, *orig_state);
			}
		}
	}

	if ( i < num_states )
	{
		switch ( outformat )
		{
			case OF_AUT:
				gsfprintf(aut,"(%lu,\"%P\",%lu)\n",current_state,transition,i);
fflush(aut);
				break;
			case OF_SVC:
				if ( outinfo )
				{
					SVCbool b;
					SVCputTransition(svc,
						SVCnewState(svc,(ATerm) gsMakeStateVector(ATindexedSetGetElem(states,current_state)),&b),
						SVCnewLabel(svc,(ATerm) transition,&b),
						SVCnewState(svc,(ATerm) gsMakeStateVector(ATindexedSetGetElem(states,i)),&b),
						svcparam);
				} else {
					SVCbool b;
					SVCputTransition(svc,
						SVCnewState(svc,(ATerm) ATmakeInt(current_state),&b),
						SVCnewLabel(svc,(ATerm) transition,&b),
						SVCnewState(svc,(ATerm) ATmakeInt(i),&b),
						svcparam);
				}
				break;
			default:
				break;
		}
		trans++;
	}
}


static void print_help_suggestion(FILE *f, char *Name)
{
	fprintf(f,"Try '%s --help' for more information.\n",Name);
}

static void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... INFILE [OUTFILE]\n"
    "Generate state space of the LPE in INFILE and save the result to OUTFILE (in the\n"
    "aut format). If OUTFILE is not supplied, the state space is not stored.\n"
    "\n"
    "The format of OUTFILE is determined by its extension (unless it is specified\n"
    "by an option). If the extension is unknown, the aut format will be used.\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -h, --help            display this help message\n"
    "  -q, --quiet           do not print any unrequested information\n"
    "  -v, --verbose         display extra information about the state space\n"
    "                        generation\n"
    "  -f, --freevar         do not replace free variables in the LPE with dummy\n"
    "                        values\n"
    "  -y, --dummy           replace free variables in the LPE with dummy values\n"
    "                        (default)\n"
    "  -c, --vector          store state in a vector (fastest, default)\n"
    "  -r, --tree            store state in a tree (for memory efficiency)\n"
    "  -l, --max=NUM         explore at most NUM states\n"
    "      --deadlock        synonym for --deadlock-detect\n"
    "  -d, --deadlock-detect detect deadlocks (i.e. for every deadlock a message is\n"
    "                        printed)\n"
    "  -e, --deadlock-trace  write trace to each deadlock state to a file\n"
    "  -m, --monitor         print detailed status of generation\n"
    "  -R, --rewriter=NAME   use rewriter NAME (default 'inner3')\n"
    "      --aut             force OUTFILE to be in the aut format (implies\n"
    "                        --no-info, see below)\n"
    "      --svc             force OUTFILE to be in the svc format\n"
    "      --no-info         do not add state information to OUTFILE\n",
    Name);
}

int main(int argc, char **argv)
{
	FILE *SpecStream;
	ATerm stackbot;
	ATermAppl Spec;
	#define sopts "hqvfycrl:demR:"
	struct option lopts[] = {
		{ "help", 		no_argument,		NULL,	'h' },
		{ "quiet", 		no_argument,		NULL,	'q' },
		{ "verbose", 		no_argument,		NULL,	'v' },
		{ "freevar", 		no_argument,		NULL,	'f' },
		{ "dummy", 		no_argument,		NULL,	'y' },
		{ "vector", 		no_argument,		NULL,	'c' },
		{ "tree", 		no_argument,		NULL,	'r' },
		{ "max", 		required_argument,	NULL,	'l' },
		{ "deadlock", 		no_argument,		NULL,	'd' },
		{ "deadlock-detect", 	no_argument,		NULL,	'd' },
		{ "deadlock-trace", 	no_argument,		NULL,	'e' },
		{ "monitor", 		no_argument,		NULL,	'm' },
		{ "rewriter", 		required_argument,	NULL,	'R' },
		{ "aut", 		no_argument,		NULL,	0 },
		{ "svc", 		no_argument,		NULL,	1 },
		{ "no-info", 		no_argument,		NULL,	2 },
		{ 0, 0, 0, 0 }
	};
	int opt, stateformat;
	RewriteStrategy strat;
	bool usedummies,trace_deadlock,explore,quiet,verbose;

	ATinit(argc,argv,&stackbot);

	quiet = false;
	verbose = false;
	strat = GS_REWR_INNER3;
	usedummies = true;
	stateformat = GS_STATE_VECTOR;
	outformat = OF_UNKNOWN;
	outinfo = true;
	max_states = 0;
	trace = false;
	trace_deadlock = false;
	monitor = false;
	explore = false;
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
			case 'f':
				usedummies = false;
				break;
			case 'y':
				usedummies = true;
				break;
			case 'c':
				stateformat = GS_STATE_VECTOR;
				break;
			case 'r':
				stateformat = GS_STATE_TREE;
				break;
			case 'l':
				if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
				{
					max_states = strtoul(optarg,NULL,0);
				}
				break;
			case 'd':
				explore = true;
				break;
			case 'e':
				trace = true;
				trace_deadlock = true;
				break;
			case 'm':
				monitor = true;
				break;
			case 'R':
				strat = RewriteStrategyFromString(optarg);
				if ( strat == GS_REWR_INVALID )
				{
					gsErrorMsg("invalid rewrite strategy '%s'\n",optarg);
					return 1;
				}
				break;
			case 0:
				outformat = OF_AUT;
				break;
			case 1:
				outformat = OF_SVC;
				break;
			case 2:
				outinfo = false;
				break;
			default:
				break;
		}
	}
	if ( quiet && verbose )
	{
		gsErrorMsg("options -q/--quiet and -v/--verbose cannot be used together\n");
		return 1;
	}
	if ( quiet )
		gsSetQuietMsg();
	if ( verbose )
		gsSetVerboseMsg();

	if ( argc-optind < 1 )
	{
		print_help_suggestion(stderr,argv[0]);
		return 1;
	}

	if ( (SpecStream = fopen(argv[optind],"rb")) == NULL )
	{
		gsErrorMsg("cannot open '%s' for reading\n",argv[optind]);
		return 1;
	}
	gsEnableConstructorFunctions();
	Spec = (ATermAppl) ATreadFromFile(SpecStream);
	if ( Spec == NULL )
	{
		gsErrorMsg("input is not a valid LPE\n");
		return 1;
	}
	gsVerboseMsg("reading LPE from '%s'.\n",argv[optind]);

	if ( argc-optind > 1 )
	{
		if ( outformat == OF_UNKNOWN )
		{
			char *s;

			s = strrchr(argv[optind+1],'.');
			if ( s == NULL )
			{
				outformat = OF_AUT;
			} else {
				s++;
				if ( !strcmp(s,"svc") )
				{
					outformat = OF_SVC;
				} else {
					outformat = OF_AUT;
				}
			}
		}

		switch ( outformat )
		{
			case OF_AUT:
				gsVerboseMsg("writing state space in AUT format to '%s'.\n",argv[optind+1]);
				outinfo = false;
				if ( (aut = fopen(argv[optind+1],"wb")) == NULL )
				{
					gsErrorMsg("cannot open '%s' for writing\n",argv[optind+1]);
					return 1;
				}
				break;
			case OF_SVC:
				gsVerboseMsg("writing state space in SVC format to '%s'.\n",argv[optind+1]);
				{
					SVCbool b;

					svc = &svcf;
					b = outinfo?SVCfalse:SVCtrue;
					SVCopen(svc,argv[optind+1],SVCwrite,&b); // XXX check result
					SVCsetCreator(svc,NAME);
					SVCsetType(svc,outinfo?"mCRL2+info":"mCRL2");
					svcparam = SVCnewParameter(svc,(ATerm) ATmakeList0(),&b);
				}
			default:
				gsVerboseMsg("not saving state space.\n");
				break;
		}
	} else {
		outformat = OF_UNKNOWN;
		gsVerboseMsg("not saving state space.\n");
	}

	gsVerboseMsg("initialising...\n");
	states = ATindexedSetCreate(10000,50);
	num_states = 0;
	trans = 0;
	if ( trace )
	{
		backpointers = ATtableCreate(10000,50);
	} else {
		backpointers = NULL;
	}

	ATerm state = gsNextStateInit(Spec,!usedummies,stateformat,strat);
	switch ( outformat )
	{
		case OF_AUT:
			fprintf(aut,"des (0,0,0)                   \n");
			break;
		case OF_SVC:
			{
				SVCbool b;
				if ( outinfo )
				{
					SVCsetInitialState(svc,SVCnewState(svc,(ATerm) gsMakeStateVector(state),&b));
				} else {
					SVCsetInitialState(svc,SVCnewState(svc,(ATerm) ATmakeInt(0),&b));
				}
			}
			break;
		default:
			break;
	}

	ATbool new_state;
	current_state = ATindexedSetPut(states,state,&new_state);
	num_states++;

	level = 1;
	unsigned long nextlevelat = 1;
	unsigned long prevtrans = 0;
	unsigned long prevcurrent = 0;
	bool err = false;
	orig_state = &state;
	gsVerboseMsg("generating state space...\n");
	while ( current_state < num_states )
	{
		state = ATindexedSetGetElem(states,current_state);
		deadlockstate = true;
		gsNextState(state, gsinst_callback); // XXX state may contain Nils instead of free vars
		if ( NextStateError )
		{
			err = true;
			break;
		}
		if ( deadlockstate )
		{
			if ( explore )
			{
				fprintf(stderr,"deadlock-detect: deadlock found.\n");
				fflush(stdout);
			}
			if ( trace_deadlock )
			{
				ATerm s = state;
				ATerm ns;
				ATermList tr = ATmakeList0();

				while ( (ns = ATtableGet(backpointers, s)) != NULL )
				{
					tr = ATinsert(tr, s);
					s = ns;
				}

				for (; !ATisEmpty(tr); tr=ATgetNext(tr))
				{
					ATermList l = gsNextState(s, NULL);
					for (; !ATisEmpty(l); l=ATgetNext(l))
					{
						if ( ATisEqual(ATgetFirst(ATgetNext(ATLgetFirst(l))),ATgetFirst(tr)) )
						{
							gsprintf("%P\n",ATAgetFirst(ATLgetFirst(l)));
							break;
						}
					}
					s = ATgetFirst(tr);
				}
				fflush(stdout);
			}
		}

		current_state++;
		if ( (monitor || gsVerbose) && ((current_state%1000) == 0) )
		{
			fprintf(stderr,"monitor: currently at level %lu with %lu state%s and %lu transition%s explored and %lu state%s seen.\n",level,current_state,(current_state==1)?"":"s",trans,(trans==1)?"":"s",num_states,(num_states==1)?"":"s");
		}
		if ( current_state == nextlevelat )
		{
			if ( monitor )
			{
				fprintf(stderr,"monitor: level %lu done. (%lu state%s, %lu transition%s)\n",level,current_state-prevcurrent,((current_state-prevcurrent)==1)?"":"s",trans-prevtrans,((trans-prevtrans)==1)?"":"s");
				fflush(stdout);
			}
			level++;
			nextlevelat = num_states;
			prevcurrent = current_state;
			prevtrans = trans;
		}
/*		if ( current_state == nextlevelat )
		{
			if ( monitor )
			{
				printf("monitor: Level %lu done. Currently %lu states visited and %lu states and %lu transitions explored.\n",level,num_states,current_state,trans);
				fflush(stdout);
			}
			level++;
			nextlevelat = num_states;
		}*/
	}

	switch ( outformat )
	{
		case OF_AUT:
			rewind(aut);
			fprintf(aut,"des (0,%lu,%lu)",trans,num_states);
			fclose(aut);
			break;
		case OF_SVC:
			SVCclose(svc);
			break;
		default:
			break;
	}

	if ( !err && (monitor || gsVerbose))
	{
		fprintf(stderr,"done with state space generation (%lu level%s, %lu state%s and %lu transition%s).\n",level-1,(level==2)?"":"s",num_states,(num_states==1)?"":"s",trans,(trans==1)?"":"s");
	}
}

#ifdef __cplusplus
}
#endif
