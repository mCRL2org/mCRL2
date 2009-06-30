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
#include <sstream>
#include "boost/shared_ptr.hpp"
#include "mcrl2/core/substitution_function.h"
#include "mcrl2/data/expression_traits.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/data_expression_with_variables.h"
#include "mcrl2/data/detail/rewrite_conversion_helper.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/find.h"
#include "mcrl2/exception.h"

namespace mcrl2 {

namespace data {

  /// \brief Rewriter class for the mCRL2 Library. It only works for terms of type data_expression
  /// and data_expression_with_variables.
  template < typename Term >
  class basic_rewriter
  {
    protected:
      /// \brief The wrapped Rewriter.
      boost::shared_ptr<detail::Rewriter> m_rewriter;

    public:

      /// \brief The term type of the rewriter.
      typedef Term term_type;

      /// \brief The type for expressions manipulated by the rewriter.
      typedef Term expression_type;

      /// \brief The strategy of the rewriter.
      enum strategy
      {
        innermost                  = detail::GS_REWR_INNER   ,  /** \brief Innermost */
#ifdef MCRL2_INNERC_AVAILABLE
        innermost_compiling        = detail::GS_REWR_INNERC  ,  /** \brief Compiling innermost */
#endif
        jitty                      = detail::GS_REWR_JITTY   ,  /** \brief JITty */
#ifdef MCRL2_JITTYC_AVAILABLE
        jitty_compiling            = detail::GS_REWR_JITTYC  ,  /** \brief Compiling JITty */
#endif
        innermost_prover           = detail::GS_REWR_INNER_P ,  /** \brief Innermost + Prover */
#ifdef MCRL2_INNERC_AVAILABLE
        innermost_compiling_prover = detail::GS_REWR_INNERC_P,  /** \brief Compiling innermost + Prover*/
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
        jitty_prover               = detail::GS_REWR_JITTY_P ,  /** \brief JITty + Prover */
        jitty_compiling_prover     = detail::GS_REWR_JITTYC_P   /** \brief Compiling JITty + Prover*/
#else
        jitty_prover               = detail::GS_REWR_JITTY_P    /** \brief JITty + Prover */
#endif
      };

    protected:

      /// \brief Constructor.
      /// \param r A rewriter
      basic_rewriter(boost::shared_ptr<detail::Rewriter> const& r) :
          m_rewriter(r)
      {}

      /// \brief Copy Constructor
      basic_rewriter(basic_rewriter const& other) :
          m_rewriter(other.m_rewriter)
      {}

      /// \brief Constructor.
      basic_rewriter(strategy s = jitty) :
          m_rewriter(detail::createRewriter(detail::data_specification_to_aterm_data_spec(data_specification()), static_cast< detail::RewriteStrategy >(s)))
      {}

    public:

      /// \brief Adds an equation to the rewrite rules.
      /// \param eq The equation that is added.
      /// \return Returns true if the operation succeeded.
      bool add_rule(const data_equation& eq)
      {
        return m_rewriter->addRewriteRule(eq);
      }

      /// \brief Removes an equation from the rewrite rules.
      /// \param eq The equation that is removed.
      void remove_rule(const data_equation& eq)
      {
        m_rewriter->removeRewriteRule(eq);
      }
  };

  /// \brief Rewriter class for the mCRL2 Library. It only works for terms of type data_expression
  /// and data_expression_with_variables.
  template <>
  class basic_rewriter< data_expression > : public basic_rewriter< atermpp::aterm >
  {
    template < typename CompatibleExpression >
    friend class basic_rewriter;
    friend class enumerator;

    public:
      /// \brief The term type of the rewriter.
      typedef data_expression                                     expression_type;
      /// \brief The variable type of the rewriter.
      typedef core::term_traits< expression_type >::variable_type variable_type;

    protected:

      /// \brief for data implementation/reconstruction
      mutable boost::shared_ptr< detail::rewrite_conversion_helper >   m_conversion_helper;

    protected:

      /// \brief Copy constructor for conversion between derived types
      template < typename CompatibleExpression >
      basic_rewriter(basic_rewriter< CompatibleExpression > const& other) :
                       basic_rewriter< atermpp::aterm >(other),
                       m_conversion_helper(other.m_conversion_helper)
      {
      }

      /// \brief Performs data implementation before rewriting (should become obsolete)
      /// \param[in] expression an expression.
      template < typename Expression >
      data_expression implement(Expression const& expression) const
      {
        return m_conversion_helper->implement(expression);
      }

      /// \brief Performs data reconstruction after rewriting (should become obsolete)
      data_expression reconstruct(atermpp::aterm_appl expression) const
      {
        return m_conversion_helper->lazy_reconstruct(expression);
      }

    public:

      /// \brief Constructor.
      /// \param r A rewriter
      basic_rewriter(basic_rewriter const& other) :
          basic_rewriter< atermpp::aterm >(other),
          m_conversion_helper(other.m_conversion_helper)
      {
      }

      /// \brief Constructor.
      /// \param d A data specification
      /// \param s A rewriter strategy.
      basic_rewriter(data_specification const& d, strategy s = jitty) :
          basic_rewriter< atermpp::aterm >(s),
          m_conversion_helper(new detail::rewrite_conversion_helper(d, *m_rewriter))
      {
      }

      /// \brief Adds an equation to the rewrite rules.
      /// \param equation The equation that is added.
      /// \return Returns true if the operation succeeded.
      bool add_rule(const data_equation& equation)
      {
        return m_rewriter->addRewriteRule(const_cast< basic_rewriter const* >(this)->implement(equation));
      }
  };

  /// \brief Rewriter that operates on data expressions.
  //
  /// \attention As long as normalisation of sorts remains necessary, the data
  /// specification object used for construction *must* exist during the
  /// lifetime of the rewriter object.
  class rewriter: public basic_rewriter<data_expression>
  {
    public:
      /// \brief Constructor.
      /// \param d A data specification
      /// \param s A rewriter strategy.
      rewriter(data_specification const& d = rewriter::default_specification(), strategy s = jitty) :
         basic_rewriter<data_expression>(d, s)
      { }

      /// \brief Constructor.
      /// \param r a rewriter.
      rewriter(rewriter const& r) :
         basic_rewriter<data_expression>(r)
      { }

      /// \brief Default specification used if no specification is specified at construction
      /// \param d A data specification
      static data_specification& default_specification()
      {
        static data_specification specification;

        return specification;
      }

      /// \brief Rewrites a data expression.
      /// \param d A data expression
      /// \return The normal form of d.
      data_expression operator()(const data_expression& d) const
      {
        return reconstruct(m_rewriter->rewrite(implement(d)));
      }

      /// \brief Rewrites the data expression d, and on the fly applies a substitution function
      /// to data variables.
      /// \param d A data expression
      /// \param sigma A substitution function
      /// \return The normal form of the term.
      template <typename SubstitutionFunction>
      data_expression operator()(const data_expression& d, SubstitutionFunction sigma) const
      {
        return reconstruct(m_rewriter->rewrite(implement(replace_free_variables(d, sigma))));
      }
  };

  /// \brief Rewriter that operates on data expressions.
  class rewriter_with_variables: public basic_rewriter<data_expression>
  {
      /// \brief The term type of the rewriter.
      typedef data_expression term_type;

      /// \brief The variable type of the rewriter.
      typedef core::term_traits< data_expression_with_variables >::variable_type variable_type;

    public:

      /// \brief Constructor.
      /// \param d A data specification
      /// \param s A rewriter strategy.
      rewriter_with_variables(data_specification const& d = rewriter::default_specification(), strategy s = jitty) :
          basic_rewriter<data_expression>(d, s)
      { }

      /// \brief Constructor. The Rewriter object that is used internally will be shared with \p r.
      /// \param r A data rewriter
      rewriter_with_variables(basic_rewriter< data_expression > const& r) :
          basic_rewriter<data_expression>(r)
      {}

      /// \brief Rewrites a data expression.
      /// \param d The term to be rewritten.
      /// \return The normal form of d.
      data_expression_with_variables operator()(const data_expression_with_variables& d) const
      {
        data_expression t = reconstruct(m_rewriter->rewrite(implement(d)));
        return data_expression_with_variables(t, find_free_variables(t));
      }

      /// \brief Rewrites the data expression d, and on the fly applies a substitution function
      /// to data variables.
      /// \param d A term.
      /// \param sigma A substitution function
      /// \return The normal form of the term.
      template <typename SubstitutionFunction>
      data_expression_with_variables operator()(const data_expression_with_variables& d, SubstitutionFunction sigma) const
      {
        data_expression t = this->operator()(replace_free_variables(d, sigma));
        return data_expression_with_variables(t, find_free_variables(t));
      }
  };


  /// \brief Function object for converting a data rewriter into a rewriter that
  /// applies a substitution.
  template <typename DataRewriter, typename Substitution>
  struct rewriter_adapter
  {
    const DataRewriter& R_;
    const Substitution& sigma_;
    
    rewriter_adapter(const DataRewriter& R, const Substitution& sigma)
      : R_(R), sigma_(sigma)
    {}
    
    data::data_expression operator()(const data::data_expression& t) const
    {
      return R_(t, sigma_);
    }
  }; 

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REWRITER_H
