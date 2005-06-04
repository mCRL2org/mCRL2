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

int main(int argc, char **argv)
{
	FILE *SpecStream,*aut;
	ATerm stackbot;
	ATermAppl Spec;
	ATermList state, l, curr, currdc, next, nextdc;
	ATermTable states;
	unsigned int num_states, curr_num, trans;
	#define sopts "d"
	struct option lopts[] = {
		{ "dummy", 	no_argument,	NULL,	'd' },
		{ 0, 0, 0, 0 }
	};
	int opt;
	bool usedummies;

	ATinit(argc,argv,&stackbot);

	usedummies = false;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case 'd':
				usedummies = true;
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

	states = ATtableCreate(10000,50);
	num_states = 0;
	trans = 0;
	
	fprintf(aut,"des (0,0,0)                   \n");

	state = gsNextStateInit(Spec,!usedummies);

	ATtablePut(states,(ATerm) SetDCs(state),(ATerm) ATmakeInt(num_states++));

	curr = ATmakeList1((ATerm) state);
	currdc = ATmakeList1((ATerm) SetDCs(state));
	while ( !ATisEmpty(curr) )
	{
		next = ATmakeList0();
		nextdc = ATmakeList0();
		for (; !ATisEmpty(curr); curr=ATgetNext(curr),currdc=ATgetNext(currdc))
		{
			state = ATLgetFirst(curr);
			curr_num = ATgetInt((ATermInt) ATtableGet(states,ATgetFirst(currdc)));
			l = gsNextState(ATLgetFirst(curr));
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				ATerm s;
				ATermList e = ATLgetFirst(ATgetNext(ATLgetFirst(l)));
				ATermList edc = SetDCs(e);
				int i;
		
				if ( (s = ATtableGet(states,(ATerm) edc)) == NULL )
				{
					i = num_states;
					s = (ATerm) ATmakeInt(num_states++);
					ATtablePut(states,(ATerm) edc,s);
					next = ATinsert(next,(ATerm) e);
					nextdc = ATinsert(nextdc,(ATerm) edc);
				} else {
					i = ATgetInt((ATermInt) s);
				}

				fprintf(aut,"(%i,\"",curr_num);
				gsPrintPart(aut,ATAgetFirst(ATLgetFirst(l)),false,0);
				fprintf(aut,"\",%i)\n",i);
				trans++;
			}
		}
		curr = ATreverse(next);
		currdc = ATreverse(nextdc);
	}

	rewind(aut);
	fprintf(aut,"des (0,%i,%i)",trans,num_states);
	fclose(aut);
}

#ifdef __cplusplus
}
#endif
