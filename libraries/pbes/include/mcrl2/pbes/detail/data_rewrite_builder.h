// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/data_rewrite_builder.h
/// \brief Rewriter for pbes expressions that rewrites only data expressions.

#ifndef MCRL2_PBES_DETAIL_DATA_REWRITE_BUILDER_H
#define MCRL2_PBES_DETAIL_DATA_REWRITE_BUILDER_H

#include <set>
#include <utility>
#include "mcrl2/core/optimized_boolean_operators.h"
#include "mcrl2/pbes/pbes_expression_builder.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  // Simplifying PBES rewriter.
  template <typename Term, typename DataRewriter, typename SubstitutionFunction = no_substitution>
  struct data_rewrite_builder: public pbes_expression_builder<Term, SubstitutionFunction>
  {
    typedef pbes_expression_builder<Term, SubstitutionFunction>                super;
    typedef SubstitutionFunction                                               argument_type;
    typedef typename super::term_type                                          term_type;
    typedef typename core::term_traits<term_type>::data_term_type              data_term_type;
    typedef typename core::term_traits<term_type>::data_term_sequence_type     data_term_sequence_type;
    typedef typename core::term_traits<term_type>::variable_sequence_type variable_sequence_type;
    typedef typename core::term_traits<term_type>::propositional_variable_type propositional_variable_type;
    typedef core::term_traits<Term> tr;

    const DataRewriter& m_data_rewriter;

    /// \brief Is called in the case rewriting is done with a substitution function.
    /// \param d A data term
    /// \param sigma A substitution function
    /// \return The rewrite result
    template <typename T>
    data_term_type rewrite(data_term_type d, T& sigma)
    {
      return m_data_rewriter(d, sigma);
    }

    /// \brief Is called in the case rewriting is done without a substitution function.
    /// \param d A data term
    /// \return The rewrite result
    data_term_type rewrite(data_term_type d, no_substitution&)
    {
      return m_data_rewriter(d);
    }

    /// \brief Constructor.
    /// \param rewr A data rewriter
    data_rewrite_builder(const DataRewriter& rewr)
      : m_data_rewriter(rewr)
    { }

    /// \brief Visit data_expression node
    /// Visit data expression node.
    /// \param x A term
    /// \param d A data term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_data_expression(const term_type& x, const data_term_type& d, SubstitutionFunction& sigma)
    {
      return tr::dataterm2term(rewrite(d, sigma));
    }

    /// \brief Visit propositional_variable node
    /// Visit propositional variable node.
    /// \param x A term
    /// \param v A propositional variable
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_propositional_variable(const term_type& x, const propositional_variable_type& v, SubstitutionFunction& sigma)
    {
      atermpp::vector<data_term_type> d;
      data_term_sequence_type e = tr::param(v);
      for (typename data_term_sequence_type::const_iterator i = e.begin(); i != e.end(); ++i)
      {
        d.push_back(rewrite(*i, sigma));
      }
      term_type result = tr::prop_var(tr::name(v), d.begin(), d.end());
      return result;
    }

    /// \brief Applies this builder to the term x.
    /// \param x A term
    /// \return The rewrite result
    term_type operator()(const term_type& x)
    {
      SubstitutionFunction tmp;
      return visit(x, tmp);
    }

    /// \brief Applies this builder to the term x, with substitution sigma.
    /// \param x A term
    /// \param sigma A substitution function
    /// \return The rewrite result
    term_type operator()(const term_type& x, SubstitutionFunction sigma)
    {
      return visit(x, sigma);
    }
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_DATA_REWRITE_BUILDER_H
