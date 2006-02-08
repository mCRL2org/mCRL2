#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aterm2.h>
#include "librewrite.h"

static Rewriter *rewr_obj;
static bool rewr_inited = false;

extern "C" void gsRewriteInit(ATermAppl Eqns, RewriteStrategy strat)
{
	rewr_obj = createRewriter(Eqns,strat);
	rewr_inited = true;
}

extern "C" void gsRewriteFinalise()
{
	delete rewr_obj;
	rewr_inited = false;
}

extern "C" bool gsRewriteIsInitialised()
{
	return rewr_inited;
}

extern "C" void gsRewriteAddEqn(ATermAppl Eqn)
{
	rewr_obj->addRewriteRule(Eqn);
}

extern "C" void gsRewriteRemoveEqn(ATermAppl Eqn)
{
	rewr_obj->removeRewriteRule(Eqn);
}

extern "C" ATermAppl gsRewriteTerm(ATermAppl Term)
{
	return rewr_obj->rewrite(Term);
}

extern "C" ATermList gsRewriteTerms(ATermList Terms)
{
	ATermList l = ATmakeList0();
	for (; !ATisEmpty(Terms); Terms=ATgetNext(Terms))
	{
		l = ATinsert(l,(ATerm) rewr_obj->rewrite((ATermAppl) ATgetFirst(Terms)));
	}
	return ATreverse(l);
}

extern "C" ATerm gsToRewriteFormat(ATermAppl Term)
{
	return rewr_obj->toRewriteFormat(Term);
}

extern "C" ATermAppl gsFromRewriteFormat(ATerm Term)
{
	return rewr_obj->fromRewriteFormat(Term);
}

extern "C" ATerm gsRewriteInternal(ATerm Term)
{
	return rewr_obj->rewriteInternal(Term);
}

extern "C" ATermList gsRewriteInternals(ATermList Terms)
{
	ATermList l = ATmakeList0();
	for (; !ATisEmpty(Terms); Terms=ATgetNext(Terms))
	{
		l = ATinsert(l,rewr_obj->rewriteInternal(ATgetFirst(Terms)));
	}
	return ATreverse(l);
}

extern "C" void PrintRewriteStrategy(FILE *stream, RewriteStrategy strat)
{
  if (strat == GS_REWR_INNER) {
    fprintf(stream, "inner");
  } else if (strat == GS_REWR_INNER2) {
    fprintf(stream, "inner2");
  } else if (strat == GS_REWR_INNER3) {
    fprintf(stream, "inner3");
  } else if (strat == GS_REWR_INNERC) {
    fprintf(stream, "innerC");
  } else if (strat == GS_REWR_INNERC2) {
    fprintf(stream, "innerC2");
  } else if (strat == GS_REWR_JITTY) {
    fprintf(stream, "jitty");
  } else if (strat == GS_REWR_JITTYC) {
    fprintf(stream, "jittyc");
  } else {
    fprintf(stream, "invalid");
  }
}

extern "C" RewriteStrategy RewriteStrategyFromString(const char *s)
{
	if ( !strcmp(s,"inner") )
	{
		return GS_REWR_INNER;
	} else if ( !strcmp(s,"inner2") )
	{
		return GS_REWR_INNER2;
	} else if ( !strcmp(s,"inner3") )
	{
		return GS_REWR_INNER3;
	} else if ( !strcmp(s,"innerc") )
	{
		return GS_REWR_INNERC;
	} else if ( !strcmp(s,"innerc2") )
	{
		return GS_REWR_INNERC2;
	} else if ( !strcmp(s,"jitty") )
	{
		return GS_REWR_JITTY;
	} else if ( !strcmp(s,"jittyc") )
	{
		return GS_REWR_JITTYC;
	} else {
		return GS_REWR_INVALID;
	}
}

extern "C" void RWsetVariable(ATerm v, ATerm t)
{
	rewr_obj->setSubstitution((ATermAppl) v, t);
}

extern "C" void RWclearVariable(ATerm v)
{
	rewr_obj->clearSubstitution((ATermAppl) v);
}

extern "C" void RWclearAllVariables()
{
	rewr_obj->clearSubstitutions();
}

extern "C" ATerm RWapplySubstitution(ATerm v)
{
	return rewr_obj->getSubstitution((ATermAppl) v);
}
