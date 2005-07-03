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

#include "gsrewr_inner.h"
#include "gsrewr_inner2.h"
#include "gsrewr_inner3.h"
#include "gsrewr_innerc.h"
#include "gsrewr_jitty.h"

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

	opid_eqns = ATreverse(opid_eqns);
	dataappl_eqns = ATreverse(dataappl_eqns);

	strategy = strat;

	switch ( strategy )
	{
		case GS_REWR_INNER:
			rewrite_init_inner();
			break;
		case GS_REWR_INNER2:
			rewrite_init_inner2();
			break;
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
		case GS_REWR_INNER:
			rewrite_add_inner(Eqn);
			break;
		case GS_REWR_INNER2:
			rewrite_add_inner2(Eqn);
			break;
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
		case GS_REWR_INNER:
			rewrite_remove_inner(Eqn);
			break;
		case GS_REWR_INNER2:
			rewrite_remove_inner2(Eqn);
			break;
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

ATerm gsRewriteTermGen(ATerm Term, int *b)
{
	switch ( strategy )
	{
		case GS_REWR_INNER:
			return rewrite_inner(Term,b);
		case GS_REWR_INNER2:
			return rewrite_inner2(Term,b);
		case GS_REWR_INNERC:
			return (ATerm)RWrewrite_innerc((ATermAppl)Term);
		case GS_REWR_JITTY:
			return rewrite_jitty(Term,b);
		case GS_REWR_INNER3:
		default:
			return rewrite_inner3(Term,b);
	}
}

ATerm gsRewriteTermsGen(ATerm Term, int *b)
{                       
        switch ( strategy )
        {               
                case GS_REWR_INNER:
                        return rewrite_inner(Term,b);
                case GS_REWR_INNER2:
                        return rewrite_inner2(Term,b);
                case GS_REWR_INNERC:
                        return (ATerm)RWrewritelist_innerc((ATermAppl)Term);
                case GS_REWR_JITTY:
                        return rewrite_jitty(Term,b);
                case GS_REWR_INNER3:
                default:
                        return rewrite_inner3(Term,b);
        }
}


static ATerm s(ATerm t, ATermTable Substs)
{
	ATermList ss,l;

	if ( Substs != NULL )
	{
		ss = ATmakeList0();
		l = ATtableKeys(Substs);
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			ss = ATinsert(ss,(ATerm) gsMakeSubst(ATgetFirst(l),ATtableGet(Substs,ATgetFirst(l))));
		}
	
		return gsSubstValues(ss,t,true);
	} else {
		return t;
	}
}

ATerm gsRewriteTermGenSubsts(ATerm Term, ATermTable Substs, int *b)
{
  ATfprintf(stderr,"gsRewriteTermGenSubsts: %t\n",Term);
	switch ( strategy )
	{
		case GS_REWR_INNER:
			Term = s(Term,Substs);
			return rewrite_inner(Term,b);
		case GS_REWR_INNER2:
			Term = s(Term,Substs);
			return rewrite_inner2(Term,b);
		case GS_REWR_INNERC:
//			Term = s(Term,Substs);
//			return rewrite_innerc(Term,b);
			return (ATerm)RWrewrite_innerc((ATermAppl)Term);
		case GS_REWR_JITTY:
			Term = s(Term,Substs);
			return rewrite_jitty(Term,b);
		case GS_REWR_INNER3:
		default:
			return rewrite_substs_inner3(Term,Substs,b);
	}
}

ATerm gsRewriteTermsGenSubsts(ATerm Term, ATermTable Substs, int *b)
{
  ATfprintf(stderr,"gsRewriteTermsGenSubsts: %t\n",Term);
        switch ( strategy )
        {
                case GS_REWR_INNER:
                        Term = s(Term,Substs);
                        return rewrite_inner(Term,b);
                case GS_REWR_INNER2:
                        Term = s(Term,Substs);
                        return rewrite_inner2(Term,b);
                case GS_REWR_INNERC:
//                      Term = s(Term,Substs);
//                      return rewrite_innerc(Term,b);
                        return (ATerm)RWrewritelist_innerc((ATermAppl)Term);
                case GS_REWR_JITTY:
                        Term = s(Term,Substs);
                        return rewrite_jitty(Term,b);
                case GS_REWR_INNER3:
                default:
                        return rewrite_substs_inner3(Term,Substs,b);
        }
}


ATermAppl gsRewriteTerm(ATermAppl Term)
{
	int b;

	return (ATermAppl) gsRewriteTermGen((ATerm) Term, &b);
}

ATermAppl gsRewriteTermWithSubsts(ATermAppl Term, ATermTable Substs)
{
	int b;

	return (ATermAppl) gsRewriteTermGenSubsts((ATerm) Term, Substs, &b);
}

ATermList gsRewriteTerms(ATermList Terms)
{
	int b;

	return (ATermList) gsRewriteTermsGen((ATerm) Terms, &b);
}

ATermList gsRewriteTermsWithSubsts(ATermList Terms, ATermTable Substs)
{
	int b;

	return (ATermList) gsRewriteTermsGenSubsts((ATerm) Terms, Substs, &b);
}

ATerm gsToRewriteFormat(ATermAppl Term)
{
   ATfprintf(stderr,"gsToRewriteFormat: %d\n",strategy);
	switch ( strategy )
	{
		case GS_REWR_INNER:
		case GS_REWR_INNER2:
		case GS_REWR_JITTY:
			return (ATerm) Term;
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
		case GS_REWR_JITTY:
			return (ATermAppl) Term;
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
        ATfprintf(stderr,"REWRITE %t\n",Term);
	switch ( strategy )
	{
		case GS_REWR_INNER:
			return rewrite_inner(Term,&b);
		case GS_REWR_INNER2:
			return rewrite_inner2(Term,&b);
		case GS_REWR_INNERC:
			return (ATerm)RWrewrite_innerc((ATermAppl)Term);
		case GS_REWR_JITTY:
			return rewrite_jitty(Term,&b);
		case GS_REWR_INNER3:
		default:
			return rewrite_internal_inner3(Term,&b);
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

ATerm gsRewriteInternalWithSubsts(ATerm Term, ATermTable Substs)
{
	int b;

	switch ( strategy )
	{
		case GS_REWR_INNER:
			Term = s(Term,Substs);
			return rewrite_inner(Term,&b);
		case GS_REWR_INNER2:
			Term = s(Term,Substs);
			return rewrite_inner2(Term,&b);
		case GS_REWR_INNERC:
			return (ATerm)RWrewrite_innerc((ATermAppl)Term);
		case GS_REWR_JITTY:
			Term = s(Term,Substs);
			return rewrite_jitty(Term,&b);
		case GS_REWR_INNER3:
		default:
			return rewrite_internal_substs_inner3(Term,Substs,&b);
	}
}

#ifdef __cplusplus
}
#endif
