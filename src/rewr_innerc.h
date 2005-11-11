#ifndef __REWR_INNERC_H
#define __REWR_INNERC_H

#include <aterm2.h>
#include "librewrite.h"

class RewriterCompilingInnermost: public Rewriter
{
	public:
		RewriterCompilingInnermost(ATermAppl DataEqnSpec);
		~RewriterCompilingInnermost();

		ATermAppl rewrite(ATermAppl Term);

		ATerm toRewriteFormat(ATermAppl Term);
		ATermAppl fromRewriteFormat(ATerm Term);
		ATerm rewriteInternal(ATerm Term);
		ATermList rewriteInternalList(ATermList Term);

		void setSubstitution(ATermAppl Var, ATerm Expr);
		ATerm getSubstitution(ATermAppl Var);
		void clearSubstitution(ATermAppl Var);
		void clearSubstitutions();

	private:
		int num_opids;

		int true_num;

		ATermTable term2int;
		ATermAppl *int2term;
		ATermList *innerc_eqns;
//		ATermAppl *inner_trees;

		void (*so_rewr_init)();
		ATermAppl (*so_rewr)(ATermAppl);
		void (*so_set_subst)(ATermAppl, ATerm);
		ATerm (*so_get_subst)(ATermAppl);
		void (*so_clear_subst)(ATermAppl);
		void (*so_clear_substs)();

		void calcTerm(FILE *f, ATerm t, int startarg);
		void implement_tree_aux(FILE *f, ATermAppl tree, int cur_arg, int parent, int level, int cnt, int d, int arity);
		void implement_tree(FILE *f, ATermAppl tree, int arity, int d, int opid);
		void CompileRewriteSystem(ATermAppl DataEqnSpec);
/*		ATermList build_args(ATermList args, int buildargs, ATermAppl *vars, ATerm *vals, int len);
		ATerm build(ATerm Term, int buildargs, ATermAppl *vars, ATerm *vals, int len);
		ATerm rewrite_func(ATermInt op, ATermList args);
		ATerm rewrite_aux(ATerm Term);
		ATermList rewrite_listelts(ATermList l);

		ATermList tree_matcher_aux(ATerm t, ATermAppl *tree, ATermAppl *vars, ATerm *vals, int *len);
		ATerm tree_matcher(ATermList t, ATermAppl tree);*/

		ATerm OpId2Int(ATermAppl Term, bool add_opids);
		ATerm toInner(ATermAppl Term, bool add_opids);
		ATermAppl fromInner(ATerm Term);
};

#endif
