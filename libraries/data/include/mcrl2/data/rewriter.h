// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/rewriter.h
/// \brief The class rewriter.

#ifndef MCRL2_DATA_REWRITER_H
#define MCRL2_DATA_REWRITER_H

#include <functional>
#include <algorithm>
#include <boost/shared_ptr.hpp>
#include "mcrl2/data/term_traits.h"
#include "mcrl2/data/data_expression_with_variables.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/data/parser.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/find.h"

namespace mcrl2 {

namespace data {

  /// Rewriter class for the mCRL2 Library. It only works for terms of type data_expression
  /// and data_expression_with_variables.
  template <typename Term>
  class basic_rewriter
  {
    friend class enumerator;

    protected:
      boost::shared_ptr<Rewriter> m_rewriter;

    public:
      typedef typename core::term_traits<Term>::variable_type variable_type;
      typedef Term term_type;

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

      /// Constructor.
      ///
      basic_rewriter(data_specification d, strategy s = jitty)
        : m_rewriter(createRewriter(d, static_cast<RewriteStrategy>(s)))
      { }

		  /// \brief Rewrites a data expression.
		  /// \param d The term to be rewritten.
		  /// \return The normal form of d.
		  // Question: is this function guaranteed to terminate?
		  ///
		  term_type operator()(const term_type& d) const
		  {
		    return m_rewriter.get()->rewrite(d);
		  }

		  /// \brief Rewrites the data expression d, and on the fly applies a substitution function
		  /// to data variables.
		  /// \return The normal form of d.
		  ///
		  template <typename SubstitutionFunction>
		  term_type operator()(const term_type& d, SubstitutionFunction sigma) const
		  {
		    return this->operator()(replace_data_variables(d, sigma));
		  }

      /// Adds an equation to the rewrite rules.
      /// \param[in] eq The equation that is added.
      /// \return Returns true if the operation succeeded.
      ///
      bool add_rule(const data_equation& eq)
      {
        return m_rewriter.get()->addRewriteRule(eq);
      }

      /// Removes an equation from the rewrite rules.
      /// \param[in] eq The equation that is removed.
      ///
      void remove_rule(const data_equation& eq)
      {
        m_rewriter.get()->removeRewriteRule(eq);
      }

      /// Returns a pointer to the Rewriter class that is used for the implementation.
      /// \deprecated
      Rewriter* get_rewriter()
      {
        return m_rewriter.get();
      }
  };

  typedef basic_rewriter<data_expression> rewriter;

  typedef basic_rewriter<data_expression_with_variables> rewriter_with_variables;

  /// Function object that turns a map of substitutions to variables into a substitution function.
  template <typename SubstitutionMap>
  class rewriter_map: public SubstitutionMap
  {
    public:
      typedef typename SubstitutionMap::mapped_type term_type;
      typedef typename SubstitutionMap::key_type variable_type;

      /// Constructor.
      ///
      rewriter_map()
      {}

      /// Constructor.
      ///
      rewriter_map(const rewriter_map<SubstitutionMap>& m)
        : SubstitutionMap(m)
      {}

      /// Constructor.
      ///
      template <typename Iter>
      rewriter_map(Iter start, Iter end)
        : SubstitutionMap(start, end)
      {}

      /// Function application.
      ///
      term_type operator()(const variable_type& v) const
      {
        typename SubstitutionMap::const_iterator i = this->find(v);
        return i == this->end() ? core::term_traits<term_type>::variable2term(v) : i->second;
      }
  };

  /// Creates a rewriter that contains rewrite rules for the standard data types like
  /// Pos, Nat and Int.
  /// \param[in] strategy The rewriter strategy
  /// \return The created rewriter
  inline
  rewriter default_data_rewriter(rewriter::strategy strategy = rewriter::jitty)
  {
    return rewriter(default_data_specification(), strategy);
  }

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REWRITER_H
