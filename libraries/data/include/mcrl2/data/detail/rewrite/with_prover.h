// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/rewrite/with_prover.h
/// \brief Rewriting combined with semantic simplification using a prover

#ifndef __REWR_PROVER_H
#define __REWR_PROVER_H

#include <aterm2.h>
#include <mcrl2/data/data_specification.h>
#include <mcrl2/data/bdd_prover.h>
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

		void setSubstitution(ATermAppl Var, ATermAppl Expr);
		void setSubstitutionList(ATermList Substs);
		void setSubstitutionInternal(ATermAppl Var, ATerm Expr);
		void setSubstitutionInternalList(ATermList Substs);
		ATermAppl getSubstitution(ATermAppl Var);
		ATerm getSubstitutionInternal(ATermAppl Var);
		void clearSubstitution(ATermAppl Var);
		void clearSubstitutions();
		void clearSubstitutions(ATermList Vars);

	public:
		BDD_Prover *prover_obj;
		Rewriter *rewr_obj;
};

#endif
