#ifndef __REWR_INNER_H
#define __REWR_INNER_H

#include <aterm2.h>
#include "librewrite.h"

class RewriterInnermost: public Rewriter
{
	public:
		RewriterInnermost(ATermAppl DataEqnSpec);
		~RewriterInnermost();

		RewriteStrategy getStrategy();

		ATermAppl rewrite(ATermAppl Term);

		ATerm toRewriteFormat(ATermAppl Term);
		ATermAppl fromRewriteFormat(ATerm Term);
		ATerm rewriteInternal(ATerm Term);

		bool addRewriteRule(ATermAppl Rule);
		bool removeRewriteRule(ATermAppl Rule);

	public:
		int num_opids;
		int max_vars;

		ATermInt trueint;

		ATermTable term2int;
		ATermAppl *int2term;
		ATermList *inner_eqns;
		ATermAppl *inner_trees;

		bool need_rebuild;

		ATermList build_args(ATermList args, int buildargs, ATermAppl *vars, ATerm *vals, int len);
		ATerm build(ATerm Term, int buildargs, ATermAppl *vars, ATerm *vals, int len);
		ATerm rewrite_func(ATermInt op, ATermList args);
		ATerm rewrite_aux(ATerm Term);
		ATermList rewrite_listelts(ATermList l);

		ATermList tree_matcher_aux(ATerm t, ATermAppl *tree, ATermAppl *vars, ATerm *vals, int *len);
		ATerm tree_matcher(ATermList t, ATermAppl tree);

		ATerm OpId2Int(ATermAppl Term, bool add_opids);
		ATerm toInner(ATermAppl Term, bool add_opids);
		ATermAppl fromInner(ATerm Term);
};

#endif
