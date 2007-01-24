#ifndef __REWR_JITTY_H
#define __REWR_JITTY_H

#include "librewrite.h"

class RewriterJitty: public Rewriter
{
	public:
		RewriterJitty(lpe::data_specification DataSpec);
		~RewriterJitty();

		RewriteStrategy getStrategy();

		ATermAppl rewrite(ATermAppl Term);

		ATerm toRewriteFormat(ATermAppl Term);
		ATermAppl fromRewriteFormat(ATerm Term);
		ATerm rewriteInternal(ATerm Term);

	private:
		unsigned int num_opids;
		unsigned int max_vars;

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
