#ifndef __REWR_PROVER_H
#define __REWR_PROVER_H

#include <aterm2.h>
#include <prover/bdd_prover.h>
#include "librewrite.h"

class RewriterProver: public Rewriter
{
	public:
		RewriterProver(ATermAppl DataEqnSpec, RewriteStrategy strat);
		~RewriterProver();

		RewriteStrategy getStrategy();

		ATermAppl rewrite(ATermAppl Term);

		ATerm toRewriteFormat(ATermAppl Term);
		ATermAppl fromRewriteFormat(ATerm Term);
		ATerm rewriteInternal(ATerm Term);

		bool addRewriteRule(ATermAppl Rule);
		bool removeRewriteRule(ATermAppl Rule);
		
		void setSubstitution(ATermAppl Var, ATerm Expr);
		ATerm getSubstitution(ATermAppl Var);
		void clearSubstitution(ATermAppl Var);
		void clearSubstitutions();

	public:
		BDD_Prover *prover_obj;
		Rewriter *rewr_obj;
};

#endif
