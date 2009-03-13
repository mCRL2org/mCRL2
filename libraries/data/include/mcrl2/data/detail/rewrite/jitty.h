// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/rewrite/jitty.h

#ifndef __REWR_JITTY_H
#define __REWR_JITTY_H

#include "mcrl2/data/rewrite.h"

class RewriterJitty: public Rewriter
{
	public:
		RewriterJitty(mcrl2::data::data_specification DataSpec);
		~RewriterJitty();

		RewriteStrategy getStrategy();

		ATermAppl rewrite(ATermAppl Term);

		ATerm toRewriteFormat(ATermAppl Term);
		ATermAppl fromRewriteFormat(ATerm Term);
		ATerm rewriteInternal(ATerm Term);

		bool addRewriteRule(ATermAppl Rule);
		bool removeRewriteRule(ATermAppl Rule);

	private:
		unsigned int num_opids;
		unsigned int max_vars;
		bool need_rebuild;

		ATermAppl jitty_true;

		ATermTable term2int;
		ATermAppl *int2term;
		ATermTable jitty_eqns;
		ATermList *jitty_strat;

		ATermAppl rewrite_aux(ATermAppl Term);

		ATerm OpId2Int(ATermAppl Term, bool add_opids);
		ATermAppl toInner(ATermAppl Term, bool add_opids);
		ATermAppl fromInner(ATermAppl Term);
};

#endif
