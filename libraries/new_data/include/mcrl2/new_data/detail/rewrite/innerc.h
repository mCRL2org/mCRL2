// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/detail/rewrite/innerc.h

#ifndef __REWR_INNERC_H
#define __REWR_INNERC_H

#include "mcrl2/new_data/detail/rewrite.h"

#ifdef MCRL2_INNERC_AVAILABLE

#include "mcrl2/new_data/data_specification.h"

//#define _INNERC_STORE_TREES

namespace mcrl2 {
  namespace new_data {
    namespace detail {

class RewriterCompilingInnermost: public Rewriter
{
	public:
		RewriterCompilingInnermost(ATermAppl DataSpec);
		~RewriterCompilingInnermost();

		RewriteStrategy getStrategy();

		ATermAppl rewrite(ATermAppl Term);

		ATerm toRewriteFormat(ATermAppl Term);
		ATermAppl fromRewriteFormat(ATerm Term);
		ATerm rewriteInternal(ATerm Term);
		ATermList rewriteInternalList(ATermList Terms);

		void setSubstitutionInternal(ATermAppl Var, ATerm Expr);
		ATerm getSubstitutionInternal(ATermAppl Var);
		void clearSubstitution(ATermAppl Var);
		void clearSubstitutions();

	private:
		int num_opids;

		int true_num;

		ATermTable term2int;
		ATermAppl *int2term;
		ATermList *innerc_eqns;

		char *file_c;
		char *file_o;
		char *file_so;

		void (*so_rewr_init)();
		ATermAppl (*so_rewr)(ATermAppl);
		void (*so_set_subst)(ATermAppl, ATerm);
		ATerm (*so_get_subst)(ATermAppl);
		void (*so_clear_subst)(ATermAppl);
		void (*so_clear_substs)();

#ifdef _INNERC_STORE_TREES
		int write_tree(FILE *f, ATermAppl tree, int *num_states);
		void tree2dot(ATermAppl tree, char *name, char *filename);
		ATermAppl create_tree(ATermList rules, int opid, int arity);
#endif

		void calcTerm(FILE *f, ATerm t, int startarg);
		void implement_tree_aux(FILE *f, ATermAppl tree, int cur_arg, int parent, int level, int cnt, int d, int arity);
		void implement_tree(FILE *f, ATermAppl tree, int arity, int d, int opid);
		void CompileRewriteSystem(ATermAppl DataSpec);

		ATerm OpId2Int(ATermAppl Term, bool add_opids);
		ATerm toInner(ATermAppl Term, bool add_opids);
		ATermAppl fromInner(ATerm Term);
};

#endif

    }
  }
}

#endif
