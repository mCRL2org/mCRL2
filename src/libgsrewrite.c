/* $Id: libgsrewrite.c,v 1.3 2005/03/22 13:26:16 muck Exp $ */

#define NAME "rewr"

#include <stdio.h>
#include <stdlib.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsrewrite.h"

#include "gsrewr_inner.h"

static int strategy;

ATermList opid_eqns;
ATermList dataappl_eqns;

void gsRewriteInit(ATermAppl Spec, int strat)
{
	ATermList eqns;

	ATprotectList(&opid_eqns);
	ATprotectList(&dataappl_eqns);

	eqns = ATLgetArgument(ATAgetArgument(Spec,3),0);
	opid_eqns = ATmakeList0();
	dataappl_eqns = ATmakeList0();

	for (; !ATisEmpty(eqns); eqns=ATgetNext(eqns))
	{
		if ( gsIsOpId(ATAgetArgument(ATAgetFirst(eqns),2)) )
		{
			opid_eqns = ATinsert(opid_eqns, ATgetFirst(eqns));
		} else {
			dataappl_eqns = ATinsert(dataappl_eqns, ATgetFirst(eqns));
		}
	}

	opid_eqns = ATreverse(opid_eqns);
	dataappl_eqns = ATreverse(dataappl_eqns);

	strategy = strat;

	switch ( strategy )
	{
		case GS_REWR_INNER:
		default:
			rewrite_init_inner();
	}
}

ATerm gsRewriteTermGen(ATerm Term, int *b)
{
	switch ( strategy )
	{
		case GS_REWR_INNER:
		default:
			return rewrite_inner(Term,b,ATmakeList0());
	}
}

ATermAppl gsRewriteTerm(ATermAppl Term)
{
	int b;

	return (ATermAppl) gsRewriteTermGen((ATerm) Term, &b);
}

ATermList gsRewriteTerms(ATermList Terms)
{
	int b;

	return (ATermList) gsRewriteTermGen((ATerm) Terms, &b);
}
