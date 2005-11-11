#ifndef __REWR_JITTY_H
#define __REWR_JITTY_H

#include <aterm2.h>
#include "librewrite.h"

class RewriterJitty: public Rewriter
{
	public:
		RewriterJitty(ATermAppl DataEqnSpec);
		~RewriterJitty();

		ATermAppl rewrite(ATermAppl Term);

		ATerm toRewriteFormat(ATermAppl Term);
		ATermAppl fromRewriteFormat(ATerm Term);
		ATerm rewriteInternal(ATerm Term);

	private:
		int num_opids;
		int max_vars;

		ATermAppl jitty_true;

		ATermTable term2int;
		ATermAppl *int2term;
		ATermList *jitty_eqns;

		ATermAppl rewrite_aux(ATermAppl Term);

		ATerm OpId2Int(ATermAppl Term, bool add_opids);
		ATermAppl toInner(ATermAppl Term, bool add_opids);
		ATermAppl fromInner(ATermAppl Term);
};

#endif
