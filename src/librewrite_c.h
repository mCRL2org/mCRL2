#ifndef __LIBGSREWRITE_H
#define __LIBGSREWRITE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>

typedef enum { GS_REWR_INNER
	     , GS_REWR_INNER2
	     , GS_REWR_INNER3
	     , GS_REWR_INNERC
	     , GS_REWR_INNERC2
	     , GS_REWR_JITTY
	     , GS_REWR_JITTYC
	     , GS_REWR_INVALID
	     } RewriteStrategy;

void gsRewriteInit(ATermAppl DataEqnSpec, RewriteStrategy Strategy);
void gsRewriteFinalise();

bool gsRewriteIsInitialised();

void gsRewriteAddEqn(ATermAppl Eqn);
void gsRewriteRemoveEqn(ATermAppl Eqn);

ATermAppl gsRewriteTerm(ATermAppl Term);
ATermList gsRewriteTerms(ATermList Terms);

ATerm gsToRewriteFormat(ATermAppl Term);
ATermAppl gsFromRewriteFormat(ATerm Term);
ATerm gsRewriteInternal(ATerm Term);
ATermList gsRewriteInternals(ATermList Terms);

void PrintRewriteStrategy(FILE *stream, RewriteStrategy strat);
RewriteStrategy RewriteStrategyFromString(const char *s);

void RWsetVariable(ATerm v, ATerm t);
void RWclearVariable(ATerm v);
void RWclearAllVariables();
ATerm RWapplySubstitution(ATerm v);

#ifdef __cplusplus
}
#endif

#endif
