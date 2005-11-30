#ifndef __LIBREWRITE_H
#define __LIBREWRITE_H

#include <aterm2.h>
#include "librewrite_c.h"

class Rewriter
{
	public:
		Rewriter();
		virtual ~Rewriter();

		virtual RewriteStrategy getStrategy() = 0;

		virtual ATermAppl rewrite(ATermAppl Term) = 0;
		virtual ATermList rewriteList(ATermList Terms);

		virtual ATerm toRewriteFormat(ATermAppl Term);
		virtual ATermAppl fromRewriteFormat(ATerm Term);
		virtual ATerm rewriteInternal(ATerm Term);
		virtual ATermList rewriteInternalList(ATermList Terms);

		virtual bool addRewriteRule(ATermAppl Rule);
		virtual bool removeRewriteRule(ATermAppl Rule);

		virtual void setSubstitution(ATermAppl Var, ATerm Expr);
		virtual ATerm getSubstitution(ATermAppl Var);
		virtual void clearSubstitution(ATermAppl Var);
		virtual void clearSubstitutions();

	protected:
		ATerm lookupSubstitution(ATermAppl Var);

	private:
		ATerm *substs;
		long substs_size;
};

Rewriter *createRewriter(ATermAppl DataEqnSpec, RewriteStrategy Strategy = GS_REWR_INNER);

#endif
