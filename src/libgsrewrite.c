/* $Id: libgsrewrite.c,v 1.5 2005/04/08 12:33:51 muck Exp $ */

#define NAME "libgsrewrite"

#include <stdio.h>
#include <stdlib.h>
#include "aterm2.h"
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsrewrite.h"

#include "gsrewr_inner.h"
#include "gsrewr_inner2.h"
#include "gsrewr_inner3.h"

static int strategy;

ATermList opid_eqns;
ATermList dataappl_eqns;

void gsRewriteInit(ATermAppl Eqns, int strat)
{
	ATermList eqns;

	ATprotectList(&opid_eqns);
	ATprotectList(&dataappl_eqns);

	eqns = ATLgetArgument(Eqns,0);
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
		case GS_REWR_INNER2:
			rewrite_init_inner2();
			break;
		case GS_REWR_INNER3:
			rewrite_init_inner3();
			break;
		case GS_REWR_INNER:
		default:
			rewrite_init_inner();
			break;
	}
}

void gsRewriteAddEqn(ATermAppl Eqn)
{
	if ( gsIsOpId(ATAgetArgument(Eqn,2)) )
	{
		opid_eqns = ATappend(opid_eqns, (ATerm) Eqn);
	} else {
		dataappl_eqns = ATappend(dataappl_eqns, (ATerm) Eqn);
	}

	switch ( strategy )
	{
		case GS_REWR_INNER2:
			rewrite_add_inner2(Eqn);
			break;
		case GS_REWR_INNER3:
			rewrite_add_inner3(Eqn);
			break;
		case GS_REWR_INNER:
		default:
			rewrite_add_inner(Eqn);
			break;
	}
}

void gsRewriteRemoveEqn(ATermAppl Eqn)
{
	ATermList l;

	if ( gsIsOpId(ATAgetArgument(Eqn,2)) )
	{
		l = ATmakeList0();
		for (; !ATisEmpty(opid_eqns); opid_eqns=ATgetNext(opid_eqns));
		{
			if ( !ATisEqual(Eqn,ATgetFirst(opid_eqns)) )
			{
				l = ATinsert(l, ATgetFirst(opid_eqns));
			}
		}
		opid_eqns = ATreverse(l);
	} else {
		l = ATmakeList0();
		for (; !ATisEmpty(dataappl_eqns); dataappl_eqns=ATgetNext(dataappl_eqns));
		{
			if ( !ATisEqual(Eqn,ATgetFirst(dataappl_eqns)) )
			{
				l = ATinsert(l, ATgetFirst(dataappl_eqns));
			}
		}
		dataappl_eqns = ATreverse(l);
	}

	switch ( strategy )
	{
		case GS_REWR_INNER2:
			rewrite_remove_inner2(Eqn);
			break;
		case GS_REWR_INNER3:
			rewrite_remove_inner3(Eqn);
			break;
		case GS_REWR_INNER:
		default:
			rewrite_remove_inner(Eqn);
			break;
	}
}

ATerm gsRewriteTermGen(ATerm Term, int *b)
{
	switch ( strategy )
	{
		case GS_REWR_INNER2:
			return rewrite_inner2(Term,b);
		case GS_REWR_INNER3:
			return rewrite_inner3(Term,b);
		case GS_REWR_INNER:
		default:
			return rewrite_inner(Term,b);
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
