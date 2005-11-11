#ifndef __LIBREWRITE_H
#define __LIBREWRITE_H

#include <aterm2.h>
#include "libgsrewrite.h"

class Rewriter
{
	public:
		Rewriter();
		virtual ~Rewriter();

		virtual ATermAppl rewrite(ATermAppl Term) = 0;
		virtual ATermList rewriteList(ATermList Term);

		virtual ATerm toRewriteFormat(ATermAppl Term);
		virtual ATermAppl fromRewriteFormat(ATerm Term);
		virtual ATerm rewriteInternal(ATerm Term);
		virtual ATermList rewriteInternalList(ATermList Term);

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

Rewriter *createRewriter(ATermAppl DataEqnSpec, RewriteStrategy Strategy);

#endif
