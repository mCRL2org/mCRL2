#include <stdlib.h>
#include <aterm2.h>
#include <assert.h>
#include "libprint_c.h"
#include "libstruct.h"
#include "librewrite.h"
#include "rewr_inner.h"
#include "rewr_jitty.h"
#include "rewr_innerc.h"
#include "rewr_jittyc.h"

Rewriter::Rewriter()
{
	substs = NULL;
	substs_size = 0;
}

Rewriter::~Rewriter()
{
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

	substs[n] = NULL;
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


Rewriter *createRewriter(ATermAppl DataEqnSpec, RewriteStrategy Strategy)
{
	switch ( Strategy )
	{
		case GS_REWR_INNER:
		case GS_REWR_INNER2:
		case GS_REWR_INNER3:
			return new RewriterInnermost(DataEqnSpec);
		case GS_REWR_JITTY:
			return new RewriterJitty(DataEqnSpec);
		case GS_REWR_INNERC:
		case GS_REWR_INNERC2:
			return new RewriterCompilingInnermost(DataEqnSpec);
		case GS_REWR_JITTYC:
			return new RewriterCompilingJitty(DataEqnSpec);
		default:
			return NULL;
	}
}

static bool checkVars(ATermAppl Expr, ATermList Vars, ATermList *UsedVars = NULL)
{
	assert(ATgetType(Expr) == AT_APPL);

	if ( gsIsNil(Expr) || gsIsOpId(Expr) )
	{
		return true;
	} else if ( gsIsDataAppl(Expr) )
	{
		return checkVars((ATermAppl) ATgetArgument(Expr,0),Vars,UsedVars) && checkVars((ATermAppl) ATgetArgument(Expr,1),Vars,UsedVars);
	} else { // gsIsDataVarId(Expr)
		assert(gsIsDataVarId(Expr));

		if ( (UsedVars != NULL) && (ATindexOf(*UsedVars,(ATerm) Expr,0) < 0) )
		{
			*UsedVars = ATinsert(*UsedVars,(ATerm) Expr);
		}

		return (ATindexOf(Vars,(ATerm) Expr,0) >= 0);
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

	return checkVars(ATAgetArgument(DataEqn,3),lhs_vars);
}
