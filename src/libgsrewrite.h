/* $Id: libgsrewrite.h,v 1.2 2005/03/09 15:46:00 muck Exp $ */

#include "aterm2.h"

#define GS_REWR_INNER	0

void gsRewriteInit(ATermAppl Spec, int strat);

ATermAppl gsRewriteTerm(ATermAppl Term);
ATermList gsRewriteTerms(ATermList Terms);
