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
#include "libgsparse.h"
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

void gsinst_callback(ATermAppl transition, ATerm state)
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
				gsfprintf(aut,"(%lu,\"%T\",%lu)\n",current_state,transition,i);
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


void print_help_suggestion(FILE *f, char *Name)
{
	fprintf(f,"Try '%s --help' for more information.\n",Name);
}

void print_help(FILE *f, char *Name)
{
	fprintf(f,"Usage: %s OPTIONS LPEFILE [OUTFILE]\n",Name);
	fprintf(f,"Generate state space of LPEFILE and save the result to\n"
	          "OUTFILE (in the aut format). If OUTFILE is not supplied, the\n"
		  "state space is not stored.\n"
		  "\n"
		  "The format of OUTFILE is determined by its extension (unless\n"
		  "the format is explicitly specified with an option). If no\n"
		  "known extension is used, the aut format will be used.\n"
	          "\n"
	          "The OPTIONS that can be used are:\n"
	          "-h, --help               Display this help message\n"
	          "-q, --quiet              Do not print any unrequested\n"
		  "                         information\n"
		  "-f, --freevar            Do not replace free variables in\n"
		  "                         the LPE with dummy values\n"
		  "-y, --dummy              Replace free variables in the LPE\n"
		  "                         with dummy values (default)\n"
		  "-c, --vector             Store state in a vector (fastest,\n"
		  "                         default)\n"
		  "-r, --tree               Store state in a tree (for memory\n"
		  "                         efficiency)\n"
	          "-l, --max num            Explore at most num states\n"
	          "    --deadlock           Synonym for --deadlock-detect\n"
	          "-d, --deadlock-detect    Detect deadlocks (i.e. for every\n"
		  "                         deadlock a message is printed)\n"
	          "-e, --deadlock-trace     Write trace to each deadlock state\n"
		  "                         to a file\n"
	          "-m, --monitor            Print status of generation\n"
	          "-R, --rewriter name      Use rewriter 'name' (default inner3)\n"
		  "    --aut                Force OUTFILE to be in the aut format\n"
		  "                         (No state information)\n"
		  "    --svc                Force OUTFILE to be in the svc format\n"
		  "    --no-info            Do not add state information to\n"
		  "                         OUTFILE\n"
	       );
}

int main(int argc, char **argv)
{
	FILE *SpecStream;
	ATerm stackbot;
	ATermAppl Spec;
	#define sopts "hqfycrldemR"
	struct option lopts[] = {
		{ "help", 		no_argument,		NULL,	'h' },
		{ "quiet", 		no_argument,		NULL,	'q' },
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
	bool usedummies,trace_deadlock,explore,quiet;
	char *rw_arg;

	ATinit(argc,argv,&stackbot);

	quiet = false;
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
			case 'f':
				usedummies = false;
				break;
			case 'y':
				usedummies = true;
				break;
			case 'v':
				stateformat = GS_STATE_VECTOR;
				break;
			case 'r':
				stateformat = GS_STATE_TREE;
				break;
			case 'l':
				if ( optarg == NULL )
				{
					// XXX argument hack
					// argument doesn't seem to work for short options
					if ( optind >= argc )
					{
						if ( !quiet )
						{
							fprintf(stderr,"Option -l/--max needs an argument.\n");
							print_help_suggestion(stderr,argv[0]);
						}
						return 1;
					}

					if ( (argv[optind][0] >= '0') && (argv[optind][0] <= '9') )
					{
						max_states = strtoul(argv[optind],NULL,0);
						optind++;
					}
				} else {
					if ( (optarg[0] >= '0') && (optarg[0] <= '9') )
					{
						max_states = strtoul(optarg,NULL,0);
					}
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
				if ( optarg == NULL )
				{
					if ( optind >= argc )
					{
						if ( !quiet )
						{
							fprintf(stderr,"Option -R/--rewriter needs an argument.\n");
							print_help_suggestion(stderr,argv[0]);
						}
						return 1;
					}

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

	if ( argc-optind < 1 )
	{
		if ( !quiet )
		{
			print_help_suggestion(stderr,argv[0]);
		}
		return 1;
	}

	if ( (SpecStream = fopen(argv[optind],"r")) == NULL )
	{
		if ( !quiet )
		{
			perror(NAME);
		}
		return 1;
	}
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
				outinfo = false;
				if ( (aut = fopen(argv[optind+1],"w")) == NULL )
				{
					if ( !quiet )
					{
						perror(NAME);
					}
					return 1;
				}
				break;
			case OF_SVC:
				{
					SVCbool b;

					svc = &svcf;
					b = outinfo?SVCfalse:SVCtrue;
					SVCopen(svc,argv[optind+1],SVCwrite,&b);
					SVCsetCreator(svc,NAME);
					SVCsetType(svc,outinfo?"mCRL2+info":"mCRL2");
					svcparam = SVCnewParameter(svc,(ATerm) ATmakeList0(),&b);
				}
			default:
				break;
		}
	} else {
		outformat = OF_UNKNOWN;
	}

	gsEnableConstructorFunctions();
	Spec = (ATermAppl) ATreadFromFile(SpecStream);
	if ( Spec == NULL )
	{
		return 1;
	}

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
					ATprintf("a\n");
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
				printf("deadlock-detect: Deadlock found.\n");
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
							gsprintf("%T\n",ATAgetFirst(ATLgetFirst(l)));
							break;
						}
					}
					s = ATgetFirst(tr);
				}
				fflush(stdout);
			}
		}

		current_state++;
		if ( monitor && ( (current_state%1000) == 0 ) )
		{
			printf("monitor: Currently at level %lu with %lu state%s and %lu transition%s explored and %lu state%s seen.\n",level,current_state,(current_state==1)?"":"s",trans,(trans==1)?"":"s",num_states,(num_states==1)?"":"s");
		}
		if ( current_state == nextlevelat )
		{
			if ( monitor )
			{
				printf("monitor: Level %lu done. (%lu state%s, %lu transition%s)\n",level,current_state-prevcurrent,((current_state-prevcurrent)==1)?"":"s",trans-prevtrans,((trans-prevtrans)==1)?"":"s");
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

	if ( !err && !quiet )
	{
		printf("Done with state space generation (%lu level%s, %lu state%s and %lu transition%s).\n",level-1,(level==2)?"":"s",num_states,(num_states==1)?"":"s",trans,(trans==1)?"":"s");
	}
}

#ifdef __cplusplus
}
#endif
