// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/rewriter.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_REWRITER_H
#define MCRL2_DATA_REWRITER_H

#include "mcrl2/data/rewrite.h"

/// The namespace of the mCRL2 tool set (will be renamed to mcrl2).
namespace lps {

///////////////////////////////////////////////////////////////////////////////
// rewriter
/// \brief rewriter.
class rewriter
{
  private:
    Rewriter* m_rewriter;

  public:
    enum strategy
    {
      innermost                  = GS_REWR_INNER   ,  /** \brief Innermost */
	    innermost_compiling        = GS_REWR_INNERC  ,  /** \brief Compiling innermost */
	    jitty                      = GS_REWR_JITTY   ,  /** \brief JITty */
	    jitty_compiling            = GS_REWR_JITTYC  ,  /** \brief Compiling JITty */
	    innermost_prover           = GS_REWR_INNER_P ,  /** \brief Innermost + Prover */
	    innermost_compiling_prover = GS_REWR_INNERC_P,  /** \brief Compiling innermost + Prover*/
	    jitty_prover               = GS_REWR_JITTY_P ,  /** \brief JITty + Prover */
	    jitty_compiling_prover     = GS_REWR_JITTYC_P   /** \brief Compiling JITty + Prover*/
    };

    struct substitution
    {
      friend class rewriter;
      
      ATermAppl m_variable;
      ATerm m_value;

      protected:
        substitution(ATermAppl variable, ATerm value)
          : m_variable(variable),
            m_value(value)
        { }
    };
    
    /// Constructs a rewriter from data specification d.
    ///
    rewriter(data_specification d, strategy s = jitty)
    {
      m_rewriter = createRewriter(d, static_cast<RewriteStrategy>(s));
    }

    ~rewriter()
    {
      delete m_rewriter;
    }

		/// \brief Rewrites a data expression.
		/// \param d The term to be rewritten.
		/// \return The normal form of d.
		// Question: is this function guaranteed to terminate?
		///
		data_expression operator()(const data_expression& d) const
		{
		  ATerm t = m_rewriter->toRewriteFormat(d);
		  return m_rewriter->rewrite((ATermAppl) t);
		}

		/// \brief Rewrites a data expression, and on the fly applies the substitutions
		/// in the sequence [first, last[.
		/// \param d The term to be rewritten.
		/// \return The normal form of d.
		// Question: is this function guaranteed to terminate?
		///
		template <typename Iter>
		data_expression operator()(const data_expression& d, Iter first, Iter last) const
		{
		  ATerm t = m_rewriter->toRewriteFormat(d);
		  // TODO: Copying the substitutions can be avoided by making the rewriter more generic.
		  for (Iter i = first; i != last; ++i)
		  {
		    m_rewriter->setSubstitution(i->m_variable, i->m_value);
		  }
		  data_expression result = m_rewriter->rewrite((ATermAppl) t);
		  m_rewriter->clearSubstitutions();
		  return result;
		}

    substitution make_substitution(const data_variable& variable, const data_expression& value)
    {
      return substitution(variable, m_rewriter->toRewriteFormat(value));
    }

    /// Adds the equation eq to the rewriter rules. Returns true if the operation succeeded.
    ///
    bool add_rule(const data_equation& eq)
    {
      return m_rewriter->addRewriteRule(eq);
    }

    /// Removes the equation eq from the rewriter rules.
    ///
    void remove_rule(const data_equation& eq)
    {
      m_rewriter->removeRewriteRule(eq);
    }
};

} // namespace lps

#endif // MCRL2_DATA_REWRITER_H
