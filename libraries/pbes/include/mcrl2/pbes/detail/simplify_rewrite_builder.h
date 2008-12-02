// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/simplify_rewrite_builder.h
/// \brief Simplifying rewriter for pbes expressions.

#ifndef MCRL2_PBES_DETAIL_SIMPLIFY_REWRITE_BUILDER_H
#define MCRL2_PBES_DETAIL_SIMPLIFY_REWRITE_BUILDER_H

#include <set>
#include <utility>
#include "mcrl2/core/optimized_boolean_operators.h"
#include "mcrl2/pbes/pbes_expression_builder.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {
  
  struct no_substitution
  {
  };

  // Simplifying PBES rewriter.
  template <typename Term, typename DataRewriter, typename SubstitutionFunction = no_substitution>
  struct simplify_rewrite_builder: public pbes_expression_builder<Term, SubstitutionFunction>
  {
    typedef pbes_expression_builder<Term, SubstitutionFunction>                super;
    typedef SubstitutionFunction                                               argument_type;
    typedef typename super::term_type                                          term_type;
    typedef typename core::term_traits<term_type>::data_term_type              data_term_type;             
    typedef typename core::term_traits<term_type>::data_term_sequence_type     data_term_sequence_type;    
    typedef typename core::term_traits<term_type>::variable_sequence_type variable_sequence_type;
    typedef typename core::term_traits<term_type>::propositional_variable_type propositional_variable_type;
    typedef core::term_traits<Term> tr;

    DataRewriter& m_data_rewriter;

    // Is called in the case rewriting is done with a substitution function.
    template <typename T>
    data_term_type rewrite(data_term_type d, T& sigma)
    {
//data_term_type result = m_data_rewriter(d, sigma);
//std::cout << m_data_rewriter.type() << "<d>" << core::pp(d) << " => " << core::term_traits<data_term_type>::pp(result) << std::endl;
      return m_data_rewriter(d, sigma);
    }
    
    // Is called in the case rewriting is done without a substitution function.
    data_term_type rewrite(data_term_type d, no_substitution&)
    {
      return m_data_rewriter(d);
    }

    /// Constructor.
    ///
    simplify_rewrite_builder(DataRewriter& rewr)
      : m_data_rewriter(rewr)
    { }

    /// Visit data expression node.
    ///
    term_type visit_data_expression(const term_type& x, const data_term_type& d, SubstitutionFunction& sigma)
    {
      return tr::dataterm2term(rewrite(d, sigma));
    }

    /// Visit true node.
    ///
    term_type visit_true(const term_type& x, SubstitutionFunction& sigma)
    {
      return tr::true_();
    }

    /// Visit false node.
    ///
    term_type visit_false(const term_type& x, SubstitutionFunction& sigma)
    {
      return tr::false_();
    }

    /// Visit not node.
    ///
    term_type visit_not(const term_type& x, const term_type& n, SubstitutionFunction& sigma)
    {
      if (tr::is_true(n))
      {
        return tr::false_();
      }
      if (tr::is_false(n))
      {
        return tr::true_();
      }
      return term_type(); // continue recursion
    }

    /// Visit and node.
    ///
    term_type visit_and(const term_type& x, const term_type& left, const term_type& right, SubstitutionFunction& sigma)
    {
      if (tr::is_true(left))
      {
        return super::visit(right, sigma);
      }
      if (tr::is_true(right))
      {
        return super::visit(left, sigma);
      }
      if (tr::is_false(left))
      {
        return tr::false_();
      }
      if (tr::is_false(right))
      {
        return tr::false_();
      }
      if (left == right)
      {
        return super::visit(left, sigma);
      }
      return term_type(); // continue recursion
    }

    /// Visit or node.
    ///
    term_type visit_or(const term_type& x, const term_type& left, const term_type& right, SubstitutionFunction& sigma)
    {
      if (tr::is_true(left))
      {
        return tr::true_();
      }
      if (tr::is_true(right))
      {
        return tr::true_();
      }
      if (tr::is_false(left))
      {
        return super::visit(right, sigma);
      }
      if (tr::is_false(right))
      {
        return super::visit(left, sigma);
      }
      if (left == right)
      {
        return super::visit(left, sigma);
      }
      return term_type(); // continue recursion
    }

    /// Visit imp node.
    ///
    term_type visit_imp(const term_type& x, const term_type& left, const term_type& right, SubstitutionFunction& sigma)
    {
      if (tr::is_true(left))
      {
        return super::visit(right, sigma);
      }
      if (tr::is_false(left))
      {
        return tr::true_();
      }
      if (tr::is_true(right))
      {
        return tr::true_();
      }
      if (left == right)
      {
        return tr::true_();
      }
      if (tr::is_false(right))
      {
        return super::visit(tr::not_(left), sigma);
      }
      return term_type(); // continue recursion
    }

    /// Visit forall node.
    ///
    term_type visit_forall(const term_type& x, const variable_sequence_type& variables, const term_type& phi, SubstitutionFunction& sigma)
    {
      return core::optimized_forall(variables, visit(phi, sigma));
    }

    /// Visit exists node.
    ///
    term_type visit_exists(const term_type& x, const variable_sequence_type& variables, const term_type& phi, SubstitutionFunction& sigma)
    {
      return core::optimized_exists(variables, visit(phi, sigma));
    }

    /// Visit propositional variable node.
    ///
    term_type visit_propositional_variable(const term_type& x, const propositional_variable_type& v, SubstitutionFunction& sigma)
    {
      std::vector<data_term_type> d;
      data_term_sequence_type e = tr::param(v);
      for (typename data_term_sequence_type::const_iterator i = e.begin(); i != e.end(); ++i)
      {
        d.push_back(rewrite(*i, sigma));
      }
      term_type result = tr::prop_var(tr::name(v), d.begin(), d.end());
      return result;
    }
   
    /// Applies this builder to the term x.
    ///
    term_type operator()(const term_type& x)
    {
      SubstitutionFunction tmp;
      return visit(x, tmp);
    }

    /// Applies this builder to the term x, with substitution sigma.
    ///
    term_type operator()(const term_type& x, SubstitutionFunction& sigma)
    {
      return visit(x, sigma);
    }
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_SIMPLIFY_REWRITE_BUILDER_H
