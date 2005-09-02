/* $Id: libgsrewrite.c,v 1.5 2005/04/08 12:33:51 muck Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

#define NAME "libgsrewrite"

#include <stdio.h>
#include <stdlib.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsrewrite.h"
#include "gssubstitute.h"

#include "gsrewr_inner.h"
#include "gsrewr_inner2.h"
#include "gsrewr_inner3.h"
#include "gsrewr_innerc.h"
#include "gsrewr_jitty.h"

//#define RWR_C2
#ifdef RWR_C2
#include "gsrewr_innerc2.h"
#endif

static int strategy;

ATermList opid_eqns;
ATermList dataappl_eqns;

void gsRewriteInit(ATermAppl Eqns, int strat)
{
	ATermList eqns;

	eqns = ATLgetArgument(Eqns,0);
	opid_eqns = ATmakeList0();
	ATprotectList(&opid_eqns);
	dataappl_eqns = ATmakeList0();
	ATprotectList(&dataappl_eqns);

	for (; !ATisEmpty(eqns); eqns=ATgetNext(eqns))
	{
		if ( gsIsOpId(ATAgetArgument(ATAgetFirst(eqns),2)) )
		{
			opid_eqns = ATinsert(opid_eqns, ATgetFirst(eqns));
		} else {
			dataappl_eqns = ATinsert(dataappl_eqns, ATgetFirst(eqns));
		}
	}

// order should not matter
//	opid_eqns = ATreverse(opid_eqns);
//	dataappl_eqns = ATreverse(dataappl_eqns);

	strategy = strat;

	switch ( strategy )
	{
		case GS_REWR_INNER:
			rewrite_init_inner();
			break;
		case GS_REWR_INNER2:
			rewrite_init_inner2();
			break;
		case GS_REWR_INNERC2:
#ifdef RWR_C2
			RWrewrite_init_innerc2();
			break;
#endif
		case GS_REWR_INNERC:
			RWrewrite_init_innerc();
			break;
		case GS_REWR_JITTY:
			rewrite_init_jitty();
			break;
		case GS_REWR_INNER3:
		default:
			rewrite_init_inner3();
			break;
	}
}

void gsRewriteFinalise()
{
	ATunprotectList(&opid_eqns);
	ATunprotectList(&dataappl_eqns);
	RWclearAllVariables();
	switch ( strategy )
	{
		case GS_REWR_INNER3:
			rewrite_finalise_inner3();
			break;
	}
}

void gsRewriteAddEqn(ATermAppl Eqn)
{
	if ( gsIsOpId(ATAgetArgument(Eqn,2)) )
	{
// order should not matter
//		opid_eqns = ATappend(opid_eqns, (ATerm) Eqn);
		opid_eqns = ATinsert(opid_eqns, (ATerm) Eqn);
	} else {
// order should not matter
//		dataappl_eqns = ATappend(dataappl_eqns, (ATerm) Eqn);
		dataappl_eqns = ATinsert(dataappl_eqns, (ATerm) Eqn);
	}

	switch ( strategy )
	{
		case GS_REWR_INNER:
			rewrite_add_inner(Eqn);
			break;
		case GS_REWR_INNER2:
			rewrite_add_inner2(Eqn);
			break;
		case GS_REWR_INNERC2:
#ifdef RWR_C2
			RWrewrite_add_innerc2(Eqn);
			break;
#endif
		case GS_REWR_INNERC:
			RWrewrite_add_innerc(Eqn);
			break;
		case GS_REWR_JITTY:
			rewrite_add_jitty(Eqn);
			break;
		case GS_REWR_INNER3:
		default:
			rewrite_add_inner3(Eqn);
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
// order should not matter
//		opid_eqns = ATreverse(l);
	} else {
		l = ATmakeList0();
		for (; !ATisEmpty(dataappl_eqns); dataappl_eqns=ATgetNext(dataappl_eqns));
		{
			if ( !ATisEqual(Eqn,ATgetFirst(dataappl_eqns)) )
			{
				l = ATinsert(l, ATgetFirst(dataappl_eqns));
			}
		}
// order should not matter
//		dataappl_eqns = ATreverse(l);
	}

	switch ( strategy )
	{
		case GS_REWR_INNER:
			rewrite_remove_inner(Eqn);
			break;
		case GS_REWR_INNER2:
			rewrite_remove_inner2(Eqn);
			break;
		case GS_REWR_INNERC2:
#ifdef RWR_C2
			RWrewrite_remove_innerc2(Eqn);
			break;
#endif
		case GS_REWR_INNERC:
			RWrewrite_remove_innerc(Eqn);
			break;
		case GS_REWR_JITTY:
			rewrite_remove_jitty(Eqn);
			break;
		case GS_REWR_INNER3:
		default:
			rewrite_remove_inner3(Eqn);
			break;
	}
}

ATermAppl gsRewriteTerm(ATermAppl Term)
{
	int b;

	switch ( strategy )
	{
		case GS_REWR_INNER:
			return (ATermAppl) rewrite_inner((ATerm) Term,&b);
		case GS_REWR_INNER2:
			return (ATermAppl) rewrite_inner2((ATerm) Term,&b);
		case GS_REWR_INNERC2:
#ifdef RWR_C2
			return from_rewrite_format_innerc2((ATerm) RWrewrite_innerc2((ATermAppl) to_rewrite_format_innerc2(Term)));
#endif
		case GS_REWR_INNERC:
			return from_rewrite_format_innerc((ATerm) RWrewrite_innerc((ATermAppl) to_rewrite_format_innerc(Term)));
		case GS_REWR_JITTY:
			return from_rewrite_format_jitty(rewrite_jitty(to_rewrite_format_jitty(Term)));
		case GS_REWR_INNER3:
		default:
			return from_rewrite_format_inner3(rewrite_inner3(to_rewrite_format_inner3(Term)));
	}
}

ATermList gsRewriteTerms(ATermList Terms)
{
	ATermList l = ATmakeList0();
	for (; !ATisEmpty(Terms); Terms=ATgetNext(Terms))
	{
		l = ATinsert(l,(ATerm) gsRewriteTerm(ATAgetFirst(Terms)));
	}
	return ATreverse(l);
}

ATerm gsToRewriteFormat(ATermAppl Term)
{
//   ATfprintf(stderr,"gsToRewriteFormat: %d\n",strategy);
	switch ( strategy )
	{
		case GS_REWR_INNER:
		case GS_REWR_INNER2:
			return (ATerm) Term;
		case GS_REWR_JITTY:
			return to_rewrite_format_jitty(Term);
		case GS_REWR_INNERC2:
#ifdef RWR_C2
			return to_rewrite_format_innerc2(Term);
#endif
		case GS_REWR_INNERC:
			return to_rewrite_format_innerc(Term);
		case GS_REWR_INNER3:
		default:
			return to_rewrite_format_inner3(Term);
	}
}

ATermAppl gsFromRewriteFormat(ATerm Term)
{
	switch ( strategy )
	{
		case GS_REWR_INNER:
		case GS_REWR_INNER2:
			return (ATermAppl) Term;
		case GS_REWR_JITTY:
			return from_rewrite_format_jitty(Term);
		case GS_REWR_INNERC2:
#ifdef RWR_C2
			return from_rewrite_format_innerc2(Term);
#endif
		case GS_REWR_INNERC:
			return from_rewrite_format_innerc(Term);
		case GS_REWR_INNER3:
		default:
			return from_rewrite_format_inner3(Term);
	}
}

ATerm gsRewriteInternal(ATerm Term)
{
	int b;
//        ATfprintf(stderr,"REWRITE %t\n",Term);
	switch ( strategy )
	{
		case GS_REWR_INNER:
			return rewrite_inner(Term,&b);
		case GS_REWR_INNER2:
			return rewrite_inner2(Term,&b);
		case GS_REWR_INNERC2:
#ifdef RWR_C2
			return (ATerm) RWrewrite_innerc2((ATermAppl)Term);
#endif
		case GS_REWR_INNERC:
			return (ATerm) RWrewrite_innerc((ATermAppl) Term);
		case GS_REWR_JITTY:
			return rewrite_jitty(Term);
		case GS_REWR_INNER3:
		default:
			return rewrite_inner3(Term);
	}
}

ATermList gsRewriteInternals(ATermList Terms)
{
	ATermList l = ATmakeList0();
	for (; !ATisEmpty(Terms); Terms=ATgetNext(Terms))
	{
		l = ATinsert(l,gsRewriteInternal(ATgetFirst(Terms)));
	}
	return ATreverse(l);
}

#ifdef __cplusplus
}
#endif
