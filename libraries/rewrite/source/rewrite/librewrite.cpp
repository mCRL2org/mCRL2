// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file librewrite.cpp

#include <stdlib.h>
#include <aterm2.h>
#include <assert.h>
#include <libstruct.h>
#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/data/data_specification.h"
#include "librewrite.h"
#include "rewr_inner.h"
#include "rewr_jitty.h"
#include "rewr_innerc.h"
#include "rewr_jittyc.h"
#include "rewr_prover.h"

#ifdef __cplusplus
using namespace ::mcrl2::utilities;
#endif

using namespace lps;

Rewriter::Rewriter()
{
	substs = NULL;
	substs_size = 0;
}

Rewriter::~Rewriter()
{
	if ( substs_size > 0 )
	{
		ATunprotectArray(substs);
	}
	free(substs);
}

ATermList Rewriter::rewriteList(ATermList Terms)
{
	ATermList l = ATmakeList0();
	for (; !ATisEmpty(Terms); Terms=ATgetNext(Terms))
	{
		l = ATinsert(l,(ATerm) rewrite((ATermAppl) ATgetFirst(Terms)));
	}
	return ATreverse(l);
}

ATerm Rewriter::toRewriteFormat(ATermAppl Term)
{
	return (ATerm) Term;
}

ATermAppl Rewriter::fromRewriteFormat(ATerm Term)
{
	return (ATermAppl) Term;
}

ATerm Rewriter::rewriteInternal(ATerm Term)
{
	return (ATerm) rewrite((ATermAppl) Term);
}

ATermList Rewriter::rewriteInternalList(ATermList Terms)
{
	ATermList l = ATmakeList0();
	for (; !ATisEmpty(Terms); Terms=ATgetNext(Terms))
	{
		l = ATinsert(l,rewriteInternal(ATgetFirst(Terms)));
	}
	return ATreverse(l);
}

bool Rewriter::addRewriteRule(ATermAppl /*Rule*/)
{
	return false;
}

bool Rewriter::removeRewriteRule(ATermAppl /*Rule*/)
{
	return false;
}

void Rewriter::setSubstitution(ATermAppl Var, ATerm Expr)
{
	long n = ATgetAFun(ATgetArgument(Var,0));
	
	if ( n >= substs_size )
	{
		long newsize;
		
		if ( n >= 2*substs_size )
		{
			if ( n < 1024 )
			{
				newsize = 1024;
			} else {
				newsize = n+1;
			}
		} else {
			newsize = 2*substs_size;
		}
		
		if ( substs_size > 0 )
		{
			ATunprotectArray(substs);
		}
		substs = (ATerm *) realloc(substs,newsize*sizeof(ATerm));
		
		if ( substs == NULL )
		{
			gsErrorMsg("Failed to increase the size of a substitution array to %d\n",newsize);
			exit(1);
		}
		
		for (long i=substs_size; i<newsize; i++)
		{
			substs[i]=NULL;
		}

		ATprotectArray(substs,newsize);
		substs_size = newsize;
	}

	substs[n] = Expr;
}

ATerm Rewriter::getSubstitution(ATermAppl Var)
{
	return lookupSubstitution(Var);
}

void Rewriter::clearSubstitution(ATermAppl Var)
{
	long n = ATgetAFun(ATgetArgument(Var,0));

	if ( n < substs_size )
	{
		substs[n] = NULL;
	}
}

void Rewriter::clearSubstitutions()
{
	for (long i=0; i<substs_size; i++)
	{
		substs[i] = NULL;
	}
}

ATerm Rewriter::lookupSubstitution(ATermAppl Var)
{
	long n = ATgetAFun(ATgetArgument(Var,0));
	
	if ( n >= substs_size )
	{
		return (ATerm) Var;
	}
	
	ATerm r = substs[n];
	
	if ( r == NULL )
	{
		return (ATerm) Var;
	}
	
	return r;
}


Rewriter *createRewriter(data_specification DataSpec, RewriteStrategy Strategy)
{
	switch ( Strategy )
	{
		case GS_REWR_INNER:
			return new RewriterInnermost(DataSpec);
		case GS_REWR_JITTY:
			return new RewriterJitty(DataSpec);
		case GS_REWR_INNERC:
			return new RewriterCompilingInnermost(DataSpec);
		case GS_REWR_JITTYC:
			return new RewriterCompilingJitty(DataSpec);
		case GS_REWR_INNER_P:
			return new RewriterProver(DataSpec,GS_REWR_INNER);
		case GS_REWR_JITTY_P:
			return new RewriterProver(DataSpec,GS_REWR_JITTY);
		case GS_REWR_INNERC_P:
			return new RewriterProver(DataSpec,GS_REWR_INNERC);
		case GS_REWR_JITTYC_P:
			return new RewriterProver(DataSpec,GS_REWR_JITTYC);
		default:
			return NULL;
	}
}

//Prototype
static bool checkVars(ATermAppl Expr, ATermList Vars, ATermList *UsedVars = NULL);

static bool checkVars(ATermList Exprs, ATermList Vars, ATermList *UsedVars = NULL)
{
        assert(ATgetType(Exprs) == AT_LIST);
        bool result = true;

        for( ; !ATisEmpty(Exprs) && result ; Exprs = ATgetNext(Exprs))
        {
                result = result && checkVars((ATermAppl) ATAgetFirst(Exprs),Vars,UsedVars);
        }

        return result;
}

static bool checkVars(ATermAppl Expr, ATermList Vars, ATermList *UsedVars)
{
	assert(ATgetType(Expr) == AT_APPL);

	if ( gsIsNil(Expr) || gsIsOpId(Expr) )
	{
		return true;
	} else if ( gsIsDataAppl(Expr) )
	{
		return checkVars((ATermAppl) ATgetArgument(Expr,0),Vars,UsedVars) && checkVars((ATermList) ATLgetArgument(Expr,1),Vars,UsedVars);
	} else { // gsIsDataVarId(Expr)
		assert(gsIsDataVarId(Expr));

		if ( (UsedVars != NULL) && (ATindexOf(*UsedVars,(ATerm) Expr,0) < 0) )
		{
			*UsedVars = ATinsert(*UsedVars,(ATerm) Expr);
		}

		return (ATindexOf(Vars,(ATerm) Expr,0) >= 0);
	}
}

//Prototype
static bool checkPattern(ATermAppl p);

static bool checkPattern(ATermList l)
{
        bool result = true;
        for( ; !ATisEmpty(l) && result; l = ATgetNext(l) )
        {
                result = result && checkPattern(ATAgetFirst(l));
        }
        return result;
}

static bool checkPattern(ATermAppl p)
{
	if ( gsIsDataVarId(p) || gsIsOpId(p) )
	{
		return true;
	} else { // gsIsDataAppl(p)
		return !gsIsDataVarId(ATAgetArgument(p,0)) &&
		       checkPattern(ATAgetArgument(p,0))   &&
		       checkPattern(ATLgetArgument(p,1));
	}
}

bool isValidRewriteRule(ATermAppl DataEqn)
{
	assert(gsIsDataEqn(DataEqn));

	ATermList vars = ATLgetArgument(DataEqn,0);
	ATermList lhs_vars = ATmakeList0();
	if ( !checkVars(ATAgetArgument(DataEqn,2),vars,&lhs_vars) )
	{
		return false;
	}

	if ( !checkVars(ATAgetArgument(DataEqn,1),lhs_vars) )
	{
		return false;
	}

	return checkVars(ATAgetArgument(DataEqn,3),lhs_vars) && checkPattern(ATAgetArgument(DataEqn,2));
}

void PrintRewriteStrategy(FILE *stream, RewriteStrategy strat)
{
  if (strat == GS_REWR_INNER) {
    fprintf(stream, "inner");
  } else if (strat == GS_REWR_INNERC) {
    fprintf(stream, "innerC");
  } else if (strat == GS_REWR_JITTY) {
    fprintf(stream, "jitty");
  } else if (strat == GS_REWR_JITTYC) {
    fprintf(stream, "jittyc");
  } else if (strat == GS_REWR_INNER_P) {
    fprintf(stream, "innerp");
  } else if (strat == GS_REWR_INNERC_P) {
    fprintf(stream, "innercp");
  } else if (strat == GS_REWR_JITTY_P) {
    fprintf(stream, "jittyp");
  } else if (strat == GS_REWR_JITTYC_P) {
    fprintf(stream, "jittycp");
  } else {
    fprintf(stream, "invalid");
  }
}

RewriteStrategy RewriteStrategyFromString(const char *s)
{
	if ( !strcmp(s,"inner") )
	{
		return GS_REWR_INNER;
	} else if ( !strcmp(s,"innerc") )
	{
		return GS_REWR_INNERC;
	} else if ( !strcmp(s,"jitty") )
	{
		return GS_REWR_JITTY;
	} else if ( !strcmp(s,"jittyc") )
	{
		return GS_REWR_JITTYC;
	} else if ( !strcmp(s,"innerp") )
	{
		return GS_REWR_INNER_P;
	} else if ( !strcmp(s,"innercp") )
	{
		return GS_REWR_INNERC_P;
	} else if ( !strcmp(s,"jittyp") )
	{
		return GS_REWR_JITTY_P;
	} else if ( !strcmp(s,"jittycp") )
	{
		return GS_REWR_JITTYC_P;
	} else {
		return GS_REWR_INVALID;
	}
}
