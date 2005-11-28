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
		ATermList rewriteInternalList(ATermList Terms);

		void setSubstitution(ATermAppl Var, ATerm Expr);
		ATerm getSubstitution(ATermAppl Var);
		void clearSubstitution(ATermAppl Var);
		void clearSubstitutions();

#ifndef NO_DYNLOAD
	private:
		int num_opids;

		int true_num;

		ATermTable term2int;
		ATermAppl *int2term;
		ATermList *innerc_eqns;

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

		ATerm OpId2Int(ATermAppl Term, bool add_opids);
		ATerm toInner(ATermAppl Term, bool add_opids);
		ATermAppl fromInner(ATerm Term);
#endif
};

#endif
