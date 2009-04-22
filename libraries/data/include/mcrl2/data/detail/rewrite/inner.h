// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/rewrite/inner.h

#ifndef __REWR_INNER_H
#define __REWR_INNER_H

#include "mcrl2/data/detail/rewrite.h"

//#define _INNER_STORE_TREES

namespace mcrl2 {
  namespace data {
    namespace detail {

class RewriterInnermost: public Rewriter
{
	public:
		RewriterInnermost(ATermAppl DataSpec);
		~RewriterInnermost();

		RewriteStrategy getStrategy();

		ATermAppl rewrite(ATermAppl Term);

		ATerm toRewriteFormat(ATermAppl Term);
		ATermAppl fromRewriteFormat(ATerm Term);
		ATerm rewriteInternal(ATerm Term);

		bool addRewriteRule(ATermAppl Rule);
		bool removeRewriteRule(ATermAppl Rule);

	private:
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

#ifdef _INNER_STORE_TREES
		int write_tree(FILE *f, ATermAppl tree, int *num_states);
		void tree2dot(ATermAppl tree, char *name, char *filename);
		ATermAppl create_tree(ATermList rules, int opid, int *max_vars);
#endif

		ATermList tree_matcher_aux(ATerm t, ATermAppl *tree, ATermAppl *vars, ATerm *vals, int *len);
		ATerm tree_matcher(ATermList t, ATermAppl tree);

		ATerm OpId2Int(ATermAppl Term, bool add_opids);
		ATerm toInner(ATermAppl Term, bool add_opids);
		ATermAppl fromInner(ATerm Term);
};
    }
  }
}

#endif
