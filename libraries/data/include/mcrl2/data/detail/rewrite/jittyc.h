// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite/jittyc.h

#ifndef __REWR_JITTYC_H
#define __REWR_JITTYC_H

#include "mcrl2/data/detail/rewrite.h"

#ifdef MCRL2_JITTYC_AVAILABLE

//#define _JITTYC_STORE_TREES
#include <utility>
#include <string>

namespace mcrl2 {
  namespace data {
    namespace detail {

typedef unsigned int* nfs_array;

class RewriterCompilingJitty: public Rewriter
{
	public:
		RewriterCompilingJitty(ATermAppl DataSpec);
		~RewriterCompilingJitty();

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
		using Rewriter::clearSubstitutions;
		
                bool addRewriteRule(ATermAppl Rule);
                bool removeRewriteRule(ATermAppl Rule);

	private:
                ATermTable tmp_eqns, subst_store;
                bool need_rebuild;
                bool made_files;

		int num_opids;

		int true_num;

		ATermTable term2int;
		ATermAppl *int2term;
		ATermList *jittyc_eqns;

		ATermTable int2ar_idx;
		unsigned int ar_size;
		ATermAppl *ar;
		ATermAppl build_ar_expr(ATerm expr, ATermAppl var);
		ATermAppl build_ar_expr_aux(ATermList eqn, unsigned int arg, unsigned int arity);
		ATermAppl build_ar_expr(ATermList eqns, unsigned int arg, unsigned int arity);
		bool always_rewrite_argument(ATermInt opid, unsigned int arity, unsigned int arg);
		bool calc_ar(ATermAppl expr);
		void fill_always_rewrite_array();

		std::string file_c;
		std::string file_o;
		std::string file_so;

		void *so_handle;
		void (*so_rewr_init)();
		void (*so_rewr_cleanup)();
		ATermAppl (*so_rewr)(ATermAppl);
		void (*so_set_subst)(ATermAppl, ATerm);
		ATerm (*so_get_subst)(ATermAppl);
		void (*so_clear_subst)(ATermAppl);
		void (*so_clear_substs)();

#ifdef _JITTYC_STORE_TREES
		int write_tree(FILE *f, ATermAppl tree, int *num_states);
		void tree2dot(ATermAppl tree, char *name, char *filename);
		ATermAppl create_tree(ATermList rules, int opid, int arity);
		ATermList create_strategy(ATermList rules, int opid, unsigned int arity, nfs_array nfs);
#endif

		void add_base_nfs(nfs_array a, ATermInt opid, unsigned int arity);
		void extend_nfs(nfs_array a, ATermInt opid, unsigned int arity);
		bool opid_is_nf(ATermInt opid, unsigned int num_args);
		void calc_nfs_list(nfs_array a, unsigned int arity, ATermList args, int startarg, ATermList nnfvars);
		bool calc_nfs(ATerm t, int startarg, ATermList nnfvars);
		std::string calc_inner_terms(nfs_array nfs, unsigned int arity,ATermList args, int startarg, ATermList nnfvars, nfs_array rewr);
		std::pair<bool,std::string> calc_inner_term(ATerm t, int startarg, ATermList nnfvars, bool rewr = true);
		void calcTerm(FILE *f, ATerm t, int startarg, ATermList nnfvars, bool rewr = true);
		void implement_tree_aux(FILE *f, ATermAppl tree, int cur_arg, int parent, int level, int cnt, int d, int arity, bool *used, ATermList nnfvars);
		void implement_tree(FILE *f, ATermAppl tree, int arity, int d, int opid, bool *used);
		void implement_strategy(FILE *f, ATermList strat, int arity, int d, int opid, unsigned int nf_args);
		void CompileRewriteSystem(ATermAppl DataSpec);
		void BuildRewriteSystem();

		ATerm OpId2Int(ATermAppl Term, bool add_opids);
		ATerm toInner(ATermAppl Term, bool add_opids);
		ATermAppl fromInner(ATerm Term);
};

#endif
    }
  }
}

#endif
