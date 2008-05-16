// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewr_prover.h

#ifndef __REWR_PROVER_H
#define __REWR_PROVER_H

#include <aterm2.h>
#include <mcrl2/data/data_specification.h>
#include <mcrl2/data/prover/bdd_prover.h>
#include "mcrl2/data/rewrite.h"

class RewriterProver: public Rewriter
{
	public:
		RewriterProver(mcrl2::data::data_specification DataSpec, RewriteStrategy strat);
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
