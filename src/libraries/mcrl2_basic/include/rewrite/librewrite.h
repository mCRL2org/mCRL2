#ifndef __LIBREWRITE_H
#define __LIBREWRITE_H

#include <aterm2.h>
#include "librewrite_c.h"

/**
 * \brief Rewriter interface class.
 *
 * This is the interface class for the rewriters. To create a specific
 * rewriter, use createRewriter.
 *
 * Simple use of the rewriter would be as follow (with t a term in the mCRL2
 * internal format):
 *
 * \code
 *   Rewriter *r = createRewriter(equations);
 *   t = r->rewrite(t);
 *   delete r;
 * \endcode
 *
 * Most rewriters use their own format to store data terms (for reasons of
 * performance). To make optimal use of this, one can convert terms to this
 * format and use the specialised rewrite function on such terms. This is
 * especially useful when rewriting the same term for many different
 * instantiations of variables occurring in that term.
 *
 * Instead of first substituting specific values for variables before rewriting
 * a term, one can tell the rewriter to do this substitution during rewriting.
 * Typical usage would be as follows (with t an mCRL2 data term, var an mCRL2
 * data variable and values a list of terms in the internal rewriter format):
 *
 * \code
 *   Rewriter *r = createRewriter(equations);
 *   ATerm u = r->toRewriteFormat(t);
 *   for (iterator i = values.begin(); i != values.end(); i++)
 *   {
 *     r->setSubstitution(var,*i);
 *     ATerm v = t->rewriteInternal(u);
 *     // v is the normal form in internal rewriter format of t[var:=*i]
 *     ...
 *   }
 *   delete r;
 * \endcode
 **/
class Rewriter
{
	public:
		/**
		 * \brief Constructor. Do not use directly; use createRewriter()
		 *        function instead.
		 * \sa createRewriter()
		 **/
		Rewriter();
		/** \brief Destructor. */
		virtual ~Rewriter();

		/**
		 * \brief Get rewriter strategy that is used.
		 * \return Used rewriter strategy.
		 **/
		virtual RewriteStrategy getStrategy() = 0;

		/**
		 * \brief Rewrite an mCRL2 data term.
		 * \param Term The term to be rewritten. This term should be
		 *             in the internal mCRL2 format.
		 * \return The normal form of Term.
		 **/
		virtual ATermAppl rewrite(ATermAppl Term) = 0;
		/**
		 * \brief Rewrite a list of mCRL2 data terms.
		 * \param Terms The list of terms to be rewritten. These terms
		 *              should be in the internal mCRL2 format.
		 * \return The list Terms where each element is replaced by its
		 *         normal form.
		 **/
		virtual ATermList rewriteList(ATermList Terms);

		/**
		 * \brief Convert an mCRL2 data term to a term in the internal
		 *        rewriter format.
		 * \param Term A mCRL2 data term.
		 * \return The term Term in the internal rewriter format.
		 **/
		virtual ATerm toRewriteFormat(ATermAppl Term);
		/**
		 * \brief Convert a term in the internal rewriter format to a
		 *        mCRL2 data term.
		 * \param Term A term in the internal rewriter format.
		 * \return The term Term as an mCRL2 data term.
		 **/
		virtual ATermAppl fromRewriteFormat(ATerm Term);
		/**
		 * \brief Rewrite a term in the internal rewriter format.
		 * \param Term The term to be rewritten. This term should be
		 *             in the internal rewriter format.
		 * \return The normal form of Term.
		 **/
		virtual ATerm rewriteInternal(ATerm Term);
		/**
		 * \brief Rewrite a list of terms in the internal rewriter
		 *        format.
		 * \param Terms The list of terms to be rewritten. These terms
		 *              should be in the internal rewriter format.
		 * \return The list Terms where each element is replaced by its
		 *         normal form.
		 **/
		virtual ATermList rewriteInternalList(ATermList Terms);

		/**
		 * \brief Add a rewrite rule to this rewriter.
		 * \param Rule A mCRL2 rewrite rule (DataEqn).
		 * \return Whether or not the rule was succesfully added. Note
		 *         that some rewriters do not support adding of rewrite
		 *         rules altogether and will always return false.
		 **/
		virtual bool addRewriteRule(ATermAppl Rule);
		/**
		 * \brief Remove a rewrite rule from this rewriter (if present).
		 * \param Rule A mCRL2 rewrite rule (DataEqn).
		 * \return Whether or not the rule was succesfully removed. Note
		 *         that some rewriters do not support removing of
		 *         rewrite rules altogether and will always return
		 *         false.
		 **/
		virtual bool removeRewriteRule(ATermAppl Rule);

		/**
		 * \brief Link a variable to a value for on-the-fly
		 *        substitution. (Replacing any previous linked value.)
		 * \param Var  A mCRL2 data variable.
		 * \param Expr A term in the internal rewriter format.
		 **/
		virtual void setSubstitution(ATermAppl Var, ATerm Expr);
		/**
		 * \brief Get the value linked to a variable for on-the-fly
		 *        substitution.
		 * \param Var A mCRL2 data variable.
		 * \return The value linked to Var. If no value is linked to
		 *         Var, then NULL is returned.
		 **/
		virtual ATerm getSubstitution(ATermAppl Var);
		/**
		 * \brief Remove the value linked to a variable for on-the-fly
		 *        substitution. (I.e. make sure that no value is
		 *        substituted for this variable during rewriting.)
		 * \param Var A mCRL2 data variable.
		 **/
		virtual void clearSubstitution(ATermAppl Var);
		/**
		 * \brief Remove all values linked to a variable for on-the-fly
		 *        substitution. (I.e. make sure that no substitution is
		 *        done during rewriting.)
		 **/
		virtual void clearSubstitutions();

	protected:
		ATerm lookupSubstitution(ATermAppl Var);

	private:
		ATerm *substs;
		long substs_size;
};

/**
 * \brief Create a rewriter.
 * \param DataEqnSpec The data equation specification of an LPE.
 * \param Strategy The rewrite strategy to be used by the rewriter.
 * \return A (pointer to a) rewriter that uses the data equations in DataEqnSpec
 *         and strategy Strategy to rewrite.
 **/
Rewriter *createRewriter(ATermAppl DataEqnSpec, RewriteStrategy Strategy = GS_REWR_INNER);

/**
 * \brief Check whether or not an mCRL2 data equation is a valid rewrite rule.
 * \param DataEqn The mCRL2 data equation to be checked.
 * \return Whether or not DataEqn is a valid rewrite rule.
 **/
bool isValidRewriteRule(ATermAppl DataEqn);

#endif
