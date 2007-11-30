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
    
    /// Constructs a rewriter from data specification d.
    ///
    rewriter(data_specification d, strategy s = innermost)
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

    /// Adds the equation eq to the rewriter rules. Returns true if the operation succeeded.
    ///
    bool add_rule(const data_equation& eq)
    {
      m_rewriter->addRewriteRule(eq);
    }

    /// Removes the equation eq from the rewriter rules. Returns true if the operation succeeded.
    ///
    bool remove_rule(const data_equation& eq)
    {
      m_rewriter->removeRewriteRule(eq);
    }

    /// Adds the substitution [var := value] to the rewriter. During rewrite all
    /// added substitutions will be applied.
    ///
		void add_substitution(const data_variable& var, const data_expression& value)
		{
		  m_rewriter->setSubstitution(var, m_rewriter->toRewriteFormat(value));
		}

    /// Removes the current substitution to the variable var from the rewriter.
		void remove_substitution(const data_variable& var)
		{
		  m_rewriter->clearSubstitution(var);
		}

    /// Returns the current substituted value to the variable var, or data_expression()
    /// if no such value is available.
		data_expression substitution(const data_variable& var)
		{
		  ATerm t = m_rewriter->getSubstitution(var);
		  data_expression result = t == NULL ? data_expression() : data_expression(m_rewriter->fromRewriteFormat(t));
		  if (result == var) // workaround for a bug
		  {
		    result = data_expression();
		  }
		  return result;
		}

    /// Removes all substitutions.
    ///
    void clear_substitutions()
    {
      m_rewriter->clearSubstitutions();
    }
};

} // namespace lps

#endif // MCRL2_DATA_REWRITER_H
