/* $Id: rewr.c,v 1.2 2005/03/09 15:46:00 muck Exp $ */

#define NAME "gsinstantiate"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsparse.h"
#include "libgsnextstate.h"

int main(int argc, char **argv)
{
	FILE *SpecStream,*aut;
	ATerm stackbot;
	ATermAppl Spec;
	ATermList state, l, curr, next;
	ATermTable states;
	unsigned int num_states, curr_num, trans;
	#define sopts ""
	struct option lopts[] = {
		{ 0, 0, 0, 0 }
	};
	int opt;

	ATinit(argc,argv,&stackbot);

	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
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

	states = ATtableCreate(10000,50);
	num_states = 0;
	trans = 0;
	
	fprintf(aut,"des (0,0,0)                   \n");

	state = gsNextStateInit(Spec);

	ATtablePut(states,(ATerm) state,(ATerm) ATmakeInt(num_states++));

	curr = ATmakeList1((ATerm) state);
	while ( !ATisEmpty(curr) )
	{
		next = ATmakeList0();
		for (; !ATisEmpty(curr); curr=ATgetNext(curr))
		{
			state = ATLgetFirst(curr);
			curr_num = ATgetInt((ATermInt) ATtableGet(states,(ATerm) state));
			l = gsNextState(state);
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				ATerm s;
				ATermList e = ATLgetFirst(ATgetNext(ATLgetFirst(l)));
				int i;
		
				if ( (s = ATtableGet(states,(ATerm) e)) == NULL )
				{
					i = num_states;
					s = (ATerm) ATmakeInt(num_states++);
					ATtablePut(states,(ATerm) e,s);
					next = ATinsert(next,(ATerm) e);
				} else {
					i = ATgetInt((ATermInt) s);
				}

				fprintf(aut,"(%i,\"",curr_num);
				gsPrintPart(aut,ATgetFirst(ATLgetFirst(l)),0,0);
				fprintf(aut,"\",%i)\n",i);
				trans++;
			}
		}
		curr = ATreverse(next);
	}

	rewind(aut);
	fprintf(aut,"des (0,%i,%i)",trans,num_states);
	fclose(aut);
}
