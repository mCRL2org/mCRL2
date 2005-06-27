/* $Id: gsinstantiate.c,v 1.1 2005/05/03 15:44:47 muck Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

#define NAME "gsinstantiate"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsparse.h"
#include "libgsnextstate.h"

static ATermList SetDCs(ATermList l)
{
	ATermList m;

	m = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( gsIsDataVarId(ATAgetFirst(l)) )
		{
			m = ATinsert(m,(ATerm) gsMakeNil());
		} else {
			m = ATinsert(m,ATgetFirst(l));
		}
	}
	m = ATreverse(m);

	return m;
}

void print_help(FILE *f)
{
	fprintf(f,"Usage: %s OPTIONS LPEFILE OUTFILE\n",NAME);
	fprintf(f,"Generate state space of LPEFILE and save the result to\n"
	          "OUTFILE (in the aut format).\n"
	          "\n"
	          "The OPTIONS that can be used are:\n"
	          "-h, --help               Display this help message\n"
		  "-y, --dummy              Replace free variables in states\n"
		  "                         with dummy values.\n"
	          "-l, --max num            Explore at most num states\n"
	          "    --deadlock           Synonym for --deadlock-detect\n"
	          "-d, --deadlock-detect    Detect deadlocks (i.e. for every\n"
		  "                         deadlock a message is printed)\n"
	          "-e, --deadlock-trace     Write trace to each deadlock state\n"
		  "                         to a file\n"
	          "-m, --monitor            Print status of generation\n"
	       );
}

int main(int argc, char **argv)
{
	FILE *SpecStream,*aut;
	ATerm stackbot;
	ATermAppl Spec;
	ATerm state;
	ATermList l;
	ATermIndexedSet states;
	ATermTable backpointers;
	unsigned int num_states, trans;
	#define sopts "hyldem"
	struct option lopts[] = {
		{ "help", 		no_argument,		NULL,	'h' },
		{ "dummy", 		no_argument,		NULL,	'y' },
		{ "max", 		required_argument,	NULL,	'l' },
		{ "deadlock", 		no_argument,		NULL,	'd' },
		{ "deadlock-detect", 	no_argument,		NULL,	'd' },
		{ "deadlock-trace", 	no_argument,		NULL,	'e' },
		{ "monitor", 		no_argument,		NULL,	'm' },
		{ 0, 0, 0, 0 }
	};
	int opt,max_states;
	bool usedummies,trace,trace_deadlock,monitor,explore;

	ATinit(argc,argv,&stackbot);

	usedummies = false;
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
				print_help(stderr);
				return 0;
			case 'y':
				usedummies = true;
				break;
			case 'l':
				if ( optarg == NULL )
				{
					// XXX argument hack
					// argument doesn't seem to work for short options
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
			default:
				break;
		}
	}

	if ( argc-optind < 2 )
	{
		fprintf(stderr,"Usage: %s <lpe file> <output aut file>\n",NAME);
		return 1;
	}

	if ( (SpecStream = fopen(argv[optind],"r")) == NULL )
	{
		perror(NAME);
		return 1;
	}
	if ( (aut = fopen(argv[optind+1],"w")) == NULL )
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

	states = ATindexedSetCreate(10000,50);
	num_states = 0;
	trans = 0;
	if ( trace )
	{
		backpointers = ATtableCreate(10000,50);
	} else {
		backpointers = NULL;
	}
	
	fprintf(aut,"des (0,0,0)                   \n");

	state = (ATerm) gsNextStateInit(Spec,!usedummies);

	ATbool new_state;
	int current_state;
	current_state = ATindexedSetPut(states,(ATerm) SetDCs((ATermList) state),&new_state);
	num_states++;

	int level = 1;
	int nextlevelat = 1;
	bool err = false;
	while ( current_state < num_states )
	{
		state = ATindexedSetGetElem(states,current_state);
		l = gsNextState((ATermList) state); // XXX state may contain Nils instead of free vars
		if ( NextStateError )
		{
			err = true;
			break;
		}
		if ( ATisEmpty(l) )
		{
			if ( explore )
			{
				printf("explore: Deadlock found.\n");
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
					ATermList l = gsNextState((ATermList) s);
					for (; !ATisEmpty(l); l=ATgetNext(l))
					{
						if ( ATisEqual(ATgetFirst(ATgetNext(ATLgetFirst(l))),ATgetFirst(tr)) )
						{
							gsPrintPart(stdout,ATAgetFirst(ATLgetFirst(l)),false,0);
							printf("\n");
							break;
						}
					}
					s = ATgetFirst(tr);
				}
				fflush(stdout);
			}
		}
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			ATerm e = ATgetFirst(ATgetNext(ATLgetFirst(l)));
			ATerm edc = (ATerm) SetDCs((ATermList) e);
			int i;
	
			if ( (i = ATindexedSetGetIndex(states, edc)) < 0 )
			{
				if ( (max_states == 0) || (num_states < max_states) )
				{
					i = ATindexedSetPut(states, edc,&new_state);
					num_states++;
					if ( trace )
					{
						ATtablePut(backpointers, edc, state);
					}
				} else {
					i = -1;
				}
			}

			if ( i >= 0 )
			{
				fprintf(aut,"(%i,\"",current_state);
				gsPrintPart(aut,ATAgetFirst(ATLgetFirst(l)),false,0);
				fprintf(aut,"\",%i)\n",i);
				trans++;
			}
		}

		current_state++;
		if ( current_state == nextlevelat )
		{
			if ( monitor )
			{
				printf("monitor: Level %i done. Currently %i states visited and %i states and %i transitions explored.\n",level,num_states,current_state,trans);
				fflush(stdout);
			}
			level++;
			nextlevelat = num_states;
		}
	}

	rewind(aut);
	fprintf(aut,"des (0,%i,%i)",trans,num_states);
	fclose(aut);

	if ( !err )
	{
		printf("Done with state space generation (%i levels, %i states and %i transitions).\n",level-1,num_states,trans);
	}
}

#ifdef __cplusplus
}
#endif
