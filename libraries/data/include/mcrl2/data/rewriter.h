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
#include "mcrl2/data/detail/data_expression_with_variables.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/implement_data_types.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/core/aterm_ext.h" // for gsMakeSubst_Appl
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
      basic_rewriter(data_specification const& d = data_specification(), strategy s = jitty) :
          m_rewriter(detail::createRewriter(detail::data_specification_to_aterm_data_spec(d), static_cast< detail::RewriteStrategy >(s)))
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
      /// \brief The variable type of the rewriter.
      typedef core::term_traits< data_expression >::variable_type variable_type;

    protected:
      /// \brief for data implementation
      mutable atermpp::aterm_list                                      m_substitution_context;

      /// \brief for data reconstruction 
      mutable mutable_substitution< sort_expression, sort_expression > m_reconstruction_context;

      /// \brief for data implementation/reconstruction
      mutable atermpp::aterm_appl                                      m_specification;

    protected:

      /// \brief Copy constructor for conversion between derived types
      template < typename CompatibleExpression >
      basic_rewriter(basic_rewriter< CompatibleExpression > const& other) :
                       basic_rewriter< atermpp::aterm >(other),
                       m_specification(other.m_specification),
                       m_substitution_context(other.m_substitution_context),
                       m_reconstruction_context(other.m_substitution_context)
      {
        m_specification.protect();
        m_substitution_context.protect();
      }

      /// \brief Performs data implementation before rewriting (should become obsolete)
      /// \param[in] specification a data specification.
      ATermAppl implement(data_specification const& specification)
      {
        atermpp::aterm_appl result(detail::data_specification_to_aterm_data_spec(specification));

        std::set< alias > known_aliases(convert< std::set< alias > >(specification.aliases()));

        // Convert to data specification again to get the additional aliases that have been introduced (legacy)
        data_specification  specification_with_more_aliases(result);

        for (data_specification::aliases_const_range r(specification_with_more_aliases.aliases()); !r.empty(); r.advance_begin(1))
        {
          if (r.front().reference().is_container_sort() || r.front().reference().is_structured_sort())
          {
            m_substitution_context = core::gsAddSubstToSubsts(core::gsMakeSubst_Appl(r.front().reference(), r.front().name()), m_substitution_context);

            // only for sorts that have been introduced for conversion to
            // ATerm; newly introduced sort are treated as an implementation
            // detail that should not leak to the outside world.
            if (known_aliases.find(r.front()) == known_aliases.end())
            {
              m_reconstruction_context[r.front().name()] = r.front().reference();
            }
          }
          else
          {
            m_substitution_context = core::gsAddSubstToSubsts(core::gsMakeSubst_Appl(r.front().name(), r.front().reference()), m_substitution_context);
          }
        }

        m_specification = result;

        // add rewrite rules (needed only for lambda expressions)
        for (data_specification::equations_const_range r = specification.equations(); !r.empty(); r.advance_begin(1))
        {
          if (!add_rule(r.front()))
          {
            throw mcrl2::runtime_error("Could not add rewrite rule!");
          }
        }

        return result;
      }

      /// \brief Performs data implementation before rewriting (should become obsolete)
      /// \param[in] expression an expression.
      template < typename ExpressionOrEquation >
      ATermAppl implement(ExpressionOrEquation const& expression) const
      {
        ATermList substitution_context = m_substitution_context;
        ATermList data_equations       = ATmakeList0();

        core::detail::t_data_decls declarations = core::detail::get_data_decls(m_specification);

        ATermAppl implemented = detail::impl_exprs_appl(expression,
                        &substitution_context, &declarations, &data_equations);

        if (!ATisEmpty(data_equations)) {
          using namespace atermpp;

          atermpp::term_list< data::data_equation > new_equations(data_equations);

          std::set< sort_expression > known_sorts(
                atermpp::term_list_iterator< sort_expression >(atermpp::list_arg1(atermpp::arg1(m_specification))),
                atermpp::term_list_iterator< sort_expression >());

          // add equations for standard functions for new sorts
          for (atermpp::term_list_iterator< sort_expression > i(declarations.sorts);
                           i != atermpp::term_list_iterator< sort_expression >(); ++i) {

             if (known_sorts.find(*i) == known_sorts.end())
             {
               data_equation_vector equations(standard_generate_equations_code(*i));

               new_equations = atermpp::term_list< data_equation >
                                      (equations.begin(), equations.end()) + new_equations;
             }
          }

          // update reconstruction context
          for (atermpp::term_list_iterator< atermpp::aterm_appl > i(substitution_context); i != atermpp::term_list_iterator< atermpp::aterm_appl >(); ++i)
          {
            m_reconstruction_context[sort_expression((*i)(1))] = sort_expression((*i)(0));
          }

          // add rewrite rules
          for (atermpp::term_list< data::data_equation >::const_iterator r = new_equations.begin();
                                                                        r != new_equations.end(); ++r) {
            if (!m_rewriter->addRewriteRule(detail::impl_exprs_appl(*r, &substitution_context, &declarations, 0))) {
              throw mcrl2::runtime_error("Could not add rewrite rule! (" + pp(*r) + ")");
            }
          }

          m_specification = core::detail::set_data_decls(m_specification, declarations);
        }

        m_substitution_context = substitution_context;

        return implemented;
      }

      /// \brief Performs data reconstruction after rewriting (should become obsolete)
      data_expression reconstruct(ATermAppl expression) const
      {
        return data_expression(atermpp::replace(expression, m_reconstruction_context));
      }

    public:

      /// \brief Constructor.
      /// \param r A rewriter
      basic_rewriter(basic_rewriter const& r) :
          basic_rewriter< atermpp::aterm >(r),
          m_substitution_context(r.m_substitution_context),
          m_reconstruction_context(r.m_reconstruction_context),
          m_specification(r.m_specification)
      {
        m_specification.protect();
        m_substitution_context.protect();
      }

      /// \brief Constructor.
      /// \param d A data specification
      /// \param s A rewriter strategy.
      basic_rewriter(data_specification const& d = data_specification(), strategy s = jitty) :
          basic_rewriter< atermpp::aterm >(data_specification(), s),
          m_specification(implement(d))
      {
        m_specification.protect();
        m_substitution_context.protect();
      }

      /// \brief Adds an equation to the rewrite rules.
      /// \param equation The equation that is added.
      /// \return Returns true if the operation succeeded.
      bool add_rule(const data_equation& equation)
      {
        return m_rewriter->addRewriteRule(const_cast< basic_rewriter const* >(this)->implement(equation));
      }

      /// \brief Returns a reference to the Rewriter object that is used for the implementation.
      /// \return A reference to the wrapped Rewriter object.
      /// \deprecated
      detail::Rewriter const& get_rewriter() const
      {
        return *m_rewriter;
      }

      /// \brief Returns a reference to the Rewriter object that is used for the implementation.
      /// \return A reference to the wrapped Rewriter object.
      /// \deprecated
      detail::Rewriter& get_rewriter()
      {
        return *m_rewriter;
      }

      virtual ~basic_rewriter() {
        m_specification.unprotect();
        m_substitution_context.unprotect();
      }
  };

  /// \brief Rewriter that operates on data expressions.
  class rewriter: public basic_rewriter<data_expression>
  {
    public:
      /// \brief Constructor.
      /// \param d A data specification
      /// \param s A rewriter strategy.
      rewriter(data_specification const& d = data_specification(), strategy s = jitty) :
         basic_rewriter<data_expression>(d, s)
      { }

      /// \brief Constructor.
      /// \param r a rewriter.
      rewriter(rewriter const& r) :
         basic_rewriter<data_expression>(r)
      { }

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
        return reconstruct(m_rewriter->rewrite(implement(replace_variables(d, sigma))));
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
      rewriter_with_variables(data_specification const& d = data_specification(), strategy s = jitty) :
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
        std::set<variable> v = find_all_variables(t);
        return data_expression_with_variables(t, variable_list(v.begin(), v.end()));
      }

      /// \brief Rewrites the data expression d, and on the fly applies a substitution function
      /// to data variables.
      /// \param d A term.
      /// \param sigma A substitution function
      /// \return The normal form of the term.
      template <typename SubstitutionFunction>
      data_expression_with_variables operator()(const data_expression_with_variables& d, SubstitutionFunction sigma) const
      {
        data_expression t = this->operator()(replace_variables(d, sigma));
        std::set<variable> v = find_all_variables(t);
        return data_expression_with_variables(t, variable_list(v.begin(), v.end()));
      }
  };

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REWRITER_H
