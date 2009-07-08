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
#include "mcrl2/data/find.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/expression_traits.h"
#include "mcrl2/data/mutable_substitution_adapter.h"
#include "mcrl2/pbes/pbes_expression_with_variables.h"
#include "mcrl2/pbes/detail/boolean_simplify_builder.h"
#include "mcrl2/pbes/detail/simplify_rewrite_builder.h"
#include "mcrl2/pbes/detail/enumerate_quantifiers_builder.h"
#include "mcrl2/data/detail/bdd_prover.h"
#include "mcrl2/pbes/detail/pfnf_visitor.h"

namespace mcrl2 {

namespace pbes_system {

  /// \brief A rewriter that simplifies boolean expressions.
  template <typename Term>
  class boolean_expression_rewriter
  {
    public:
      /// \brief The term type
      typedef typename core::term_traits<Term>::term_type term_type;

      /// \brief The variable type
      typedef typename core::term_traits<Term>::variable_type variable_type;

      /// \brief Rewrites a boolean expression.
      /// \param x A term
      /// \return The rewrite result.
      term_type operator()(const term_type& x)
      {
        bes::detail::boolean_simplify_builder<Term> r;
        return r(x);
      }
  };

  /// \brief A rewriter that simplifies expressions.
  template <typename Term, typename DataRewriter>
  class simplifying_rewriter
  {
    protected:

      /// \brief The data rewriter
      DataRewriter m_rewriter;

    public:
      /// \brief The term type
      typedef typename core::term_traits<Term>::term_type term_type;

      /// \brief The variable type
      typedef typename core::term_traits<Term>::variable_type variable_type;

      /// \brief Constructor
      /// \param rewriter A data rewriter
      simplifying_rewriter(const DataRewriter& rewriter)
        : m_rewriter(rewriter)
      {}

      /// \brief Rewrites a pbes expression.
      /// \param x A term
      /// \return The rewrite result.
      term_type operator()(const term_type& x)
      {
        detail::simplify_rewrite_builder<Term, DataRewriter> r(m_rewriter);
        return r(x);
      }

      /// \brief Rewrites a pbes expression.
      /// \param x A term
      /// \param sigma A substitution function
      /// \return The rewrite result.
      template <typename SubstitutionFunction>
      term_type operator()(const term_type& x, SubstitutionFunction sigma)
      {
        detail::simplify_rewrite_builder<Term, DataRewriter, SubstitutionFunction> r(m_rewriter);
        return r(x, sigma);
      }
  };

  /// \brief A rewriter that brings PBES expressions into PFNF normal form.
  template <typename Term>
  class pfnf_rewriter
  {
    public:
      /// \brief The term type
      typedef typename core::term_traits<Term>::term_type term_type;

      /// \brief The variable type
      typedef typename core::term_traits<Term>::variable_type variable_type;

      /// \brief Rewrites a pbes expression.
      /// \param x A term
      /// \return The rewrite result.
      term_type operator()(const term_type& x)
      {
        pbes_system::detail::pfnf_visitor<term_type> visitor;
        visitor.visit(x);
        return visitor.evaluate();
      }

      /// \brief Rewrites a pbes expression.
      /// \param x A term
      /// \param sigma A substitution function
      /// \return The rewrite result.
      template <typename SubstitutionFunction>
      term_type operator()(const term_type& x, SubstitutionFunction sigma)
      {
        return sigma(this->operator()(x));
      }
  };

  /// \brief A rewriter that simplifies expressions and eliminates quantifiers using enumeration.
  template <typename Term, typename DataRewriter, typename DataEnumerator>
  class enumerate_quantifiers_rewriter
  {
    protected:
      /// \brief A data rewriter
      DataRewriter m_rewriter;

      /// \brief A data enumerator
      DataEnumerator m_enumerator;

      /// \brief If true, quantifier variables of infinite sort are enumerated.
      bool m_enumerate_infinite_sorts;

    public:
      /// \brief The term type
      typedef typename core::term_traits<Term>::term_type term_type;

      /// \brief The data term type
      typedef typename core::term_traits<Term>::data_term_type data_term_type;

      /// \brief The variable type
      typedef typename core::term_traits<Term>::variable_type variable_type;

      /// \brief Constructor
      /// \param r A data rewriter
      /// \param e A data enumerator
      /// \param enumerate_infinite_sorts Determines if quantifier variables of infinte sorts are enumerated
      enumerate_quantifiers_rewriter(const DataRewriter& r, const DataEnumerator& e, bool enumerate_infinite_sorts = true)
        : m_rewriter(r), m_enumerator(e), m_enumerate_infinite_sorts(enumerate_infinite_sorts)
      {}

      /// \brief Rewrites a pbes expression.
      /// \param x A term
      /// \return The rewrite result.
      term_type operator()(const term_type& x)
      {
        typedef data::mutable_map_substitution< atermpp::map< variable_type, data_term_type > > substitution_function;
        substitution_function sigma;
        detail::enumerate_quantifiers_builder<Term, DataRewriter, DataEnumerator, substitution_function> r(m_rewriter, m_enumerator, m_enumerate_infinite_sorts);
        term_type result = r(x, sigma);
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
std::cerr << "<enumerate-quantifiers>" << core::pp(x) << " -> " << core::pp(result) << std::endl;
#endif
        return result;
      }

      /// \brief Rewrites a pbes expression.
      /// \param x A term
      /// \param sigma A substitution function
      /// \return The rewrite result.
      template <typename SubstitutionFunction>
      term_type operator()(const term_type& x, SubstitutionFunction sigma)
      {
        typedef data::mutable_substitution_adapter<SubstitutionFunction> substitution_function;
        detail::enumerate_quantifiers_builder<Term, DataRewriter, DataEnumerator, substitution_function> r(m_rewriter, m_enumerator, m_enumerate_infinite_sorts);
        term_type result = r(x, substitution_function(sigma));
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
std::cerr << "<enumerate-quantifiers>" << core::pp(x) << " -> " << core::pp(result) << data::to_string(sigma) << std::endl;
#endif
        return result;
      }
  };

  /// \brief A specialization for pbes_expression. It uses pbes_expression_with_variables internally.
  template <typename DataRewriter, typename DataEnumerator>
  class enumerate_quantifiers_rewriter<pbes_expression, DataRewriter, DataEnumerator>
  {
    protected:
      /// \brief Rewriter with term type pbes_expression_with_variables
      enumerate_quantifiers_rewriter<pbes_expression_with_variables, DataRewriter, DataEnumerator> m_rewriter;

    public:
      /// \brief The term type
      typedef pbes_expression term_type;

      /// \brief The data term type
      typedef typename core::term_traits<term_type>::data_term_type data_term_type;

      /// \brief The variable type
      typedef typename core::term_traits<term_type>::variable_type variable_type;

      /// \brief Constructor
      /// \param r A data rewriter
      /// \param e A data enumerator
      /// \param enumerate_infinite_sorts If true, quantifier variables of infinite sort are enumerated.
      enumerate_quantifiers_rewriter(const DataRewriter& r, const DataEnumerator& e, bool enumerate_infinite_sorts = true)
        : m_rewriter(r, e, enumerate_infinite_sorts)
      {}

      /// \brief Rewrites a pbes expression.
      /// \param x A term
      /// \return The rewrite result.
      term_type operator()(const term_type& x)
      {
        return m_rewriter(pbes_expression_with_variables(x, data::variable_list()));
      }

      /// \brief Rewrites a pbes expression.
      /// \param x A term
      /// \param sigma A substitution function
      /// \return The rewrite result.
      template <typename SubstitutionFunction>
      term_type operator()(const term_type& x, SubstitutionFunction sigma)
      {
        return m_rewriter(pbes_expression_with_variables(x, data::variable_list()), sigma);
      }
  };

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITER_H
