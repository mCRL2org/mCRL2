/* $Id: libgsrewrite.h,v 1.3 2005/04/08 09:49:58 muck Exp $ */
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
	     } RewriteStrategy;

void gsRewriteInit(ATermAppl DataEqnSpec, RewriteStrategy Strategy);
void gsRewriteFinalise();

void gsRewriteAddEqn(ATermAppl Eqn);
void gsRewriteRemoveEqn(ATermAppl Eqn);

ATermAppl gsRewriteTerm(ATermAppl Term);
ATermList gsRewriteTerms(ATermList Terms);

ATerm gsToRewriteFormat(ATermAppl Term);
ATermAppl gsFromRewriteFormat(ATerm Term);
ATerm gsRewriteInternal(ATerm Term);
ATermList gsRewriteInternals(ATermList Terms);

#ifdef __cplusplus
}
#endif
#endif
