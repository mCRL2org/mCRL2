// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriter.h
/// \brief Rewriters for pbes expressions.

#ifndef MCRL2_PBES_REWRITER_H
#define MCRL2_PBES_REWRITER_H

#include <map>
#include <set>
#include <utility>
#include <vector>
#include "mcrl2/core/print.h"
#include "mcrl2/core/data_implementation.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/term_traits.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/pbes/gauss.h"
#include "mcrl2/pbes/pbes_expression_with_variables.h"
#include "mcrl2/pbes/detail/simplify_rewrite_builder.h"
#include "mcrl2/pbes/detail/enumerate_quantifiers_builder.h"

namespace mcrl2 {

namespace pbes_system {

  /// \brief A rewriter that simplifies expressions.
  template <typename Term, typename DataRewriter>
  class simplifying_rewriter
  {
    protected:
      DataRewriter m_rewriter;

    public:
      typedef typename core::term_traits<Term>::term_type term_type;
      typedef typename core::term_traits<Term>::variable_type variable_type;

      simplifying_rewriter(const DataRewriter& rewriter)
        : m_rewriter(rewriter)
      {}

      /// \brief Rewrites a pbes expression.
      /// \param[in] x The pbes expression that is rewritten to normal form.
      /// \return The rewrite result.
      term_type operator()(const term_type& x)
      {
        detail::simplify_rewrite_builder<Term, DataRewriter> r(m_rewriter);
        return r(x);
      }

      /// \brief Rewrites a pbes expression.
      /// \param[in] x The pbes expression that is rewritten to normal form.
      /// \param[in] sigma A substitution function that is applied to data variables during rewriting.
      /// \return The rewrite result.
      template <typename SubstitutionFunction>
      term_type operator()(const term_type& x, SubstitutionFunction sigma)
      {
        detail::simplify_rewrite_builder<Term, DataRewriter, SubstitutionFunction> r(m_rewriter);
        return r(x, sigma);
      }
  };

  /// \brief A rewriter that simplifies expressions and eliminates quantifiers using enumeration.
  template <typename Term, typename DataRewriter, typename DataEnumerator>
  class enumerate_quantifiers_rewriter
  {
    protected:
      DataRewriter m_rewriter;
      DataEnumerator m_enumerator;

    public:
      typedef typename core::term_traits<Term>::term_type term_type;
      typedef typename core::term_traits<Term>::data_term_type data_term_type;
      typedef typename core::term_traits<Term>::variable_type variable_type;

      enumerate_quantifiers_rewriter(const DataRewriter& r, const DataEnumerator& e)
        : m_rewriter(r), m_enumerator(e)
      {}

      /// \brief Rewrites a pbes expression.
      /// \param[in] x The pbes expression that is rewritten to normal form.
      /// \return The rewrite result.
      term_type operator()(const term_type& x)
      {
        typedef data::rewriter_map<std::map<variable_type, data_term_type> > substitution_map;
        substitution_map sigma;
        detail::enumerate_quantifiers_builder<Term, DataRewriter, DataEnumerator, substitution_map> r(m_rewriter, m_enumerator);
        term_type result = r(x, sigma);
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
std::cerr << core::pp(x) << " -> " << core::pp(result) << std::endl;
#endif
        return result;
      }

      /// \brief Rewrites a pbes expression.
      /// \param[in] x The pbes expression that is rewritten to normal form.
      /// \param[in] sigma A substitution function that is applied to data variables during rewriting.
      /// \return The rewrite result.
      template <typename SubstitutionFunction>
      term_type operator()(const term_type& x, SubstitutionFunction& sigma)
      {
        detail::enumerate_quantifiers_builder<Term, DataRewriter, DataEnumerator, SubstitutionFunction> r(m_rewriter, m_enumerator);
        term_type result = r(x, sigma);
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
std::cerr << core::pp(x) << " -> " << core::pp(result) << std::endl;
#endif
        return result;
      }
  };

  /// \brief A specialization for pbes_expression. It uses pbes_expression_with_variables internally.
  template <typename DataRewriter, typename DataEnumerator>
  class enumerate_quantifiers_rewriter<pbes_expression, DataRewriter, DataEnumerator>
  {
    protected:
      enumerate_quantifiers_rewriter<pbes_expression_with_variables, DataRewriter, DataEnumerator> m_rewriter;

    public:
      typedef pbes_expression term_type;
      typedef typename core::term_traits<term_type>::data_term_type data_term_type;
      typedef typename core::term_traits<term_type>::variable_type variable_type;

      enumerate_quantifiers_rewriter(const DataRewriter& r, const DataEnumerator& e)
        : m_rewriter(r, e)
      {}

      /// \brief Rewrites a pbes expression.
      /// \param[in] x The pbes expression that is rewritten to normal form.
      /// \return The rewrite result.
      term_type operator()(const term_type& x)
      {
        return m_rewriter(pbes_expression_with_variables(x, data::data_variable_list()));
      }

      /// \brief Rewrites a pbes expression.
      /// \param[in] x The pbes expression that is rewritten to normal form.
      /// \param[in] sigma A substitution function that is applied to data variables during rewriting.
      /// \return The rewrite result.
      template <typename SubstitutionFunction>
      term_type operator()(const term_type& x, SubstitutionFunction& sigma)
      {
        return m_rewriter(pbes_expression_with_variables(x, data::data_variable_list()), sigma);
      }
  };

  /// \brief The simplifying pbes rewriter used in pbes2bool.
  class simplify_rewriter_jfg
  {
    data::rewriter datar;

    public:
      simplify_rewriter_jfg(const data::data_specification& data)
        : datar(data)
      { }

      /// \brief Rewrites a pbes expression.
      pbes_expression operator()(pbes_expression p)
      {
        return pbes_expression_rewrite_and_simplify(p, datar.get_rewriter());
      }
  };

  /// \brief The substituting pbes rewriter used in pbes2bool.
  class substitute_rewriter_jfg
  {
    data::rewriter& datar_;
    const data::data_specification& data_spec;

    public:
      substitute_rewriter_jfg(data::rewriter& datar, const data::data_specification& data)
        : datar_(datar), data_spec(data)
      { }

      /// \brief Rewrites a pbes expression.
      pbes_expression operator()(pbes_expression p)
      {
        return pbes_expression_substitute_and_rewrite(p, data_spec, datar_.get_rewriter(), false);
      }
  };

  /// \brief A pbes rewriter that uses a bdd based prover internally.
  class pbessolve_rewriter
  {
    data::rewriter datar_;
    const data::data_specification& data_spec;
    int n;
    data_variable_list fv;
    boost::shared_ptr<BDD_Prover> prover;

    public:
      pbessolve_rewriter(const data::rewriter& datar, const data::data_specification& data, RewriteStrategy rewrite_strategy, SMT_Solver_Type solver_type)
        : datar_(datar),
          data_spec(data),
          n(0),
          prover(new BDD_Prover(data_spec, rewrite_strategy, 0, false, solver_type, false))
      { }

      /// \brief Rewrites a pbes expression.
      pbes_expression operator()(pbes_expression p)
      {
        return pbes_expression_simplify(p, &n, &fv, prover.get());
      }
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITER_H
