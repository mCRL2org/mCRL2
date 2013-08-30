// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H
#define MCRL2_PBES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H

#include "mcrl2/pbes/detail/enumerate_quantifiers_builder.h"
#include "mcrl2/pbes/pbes_expression_with_variables.h"

namespace mcrl2 {

namespace pbes_system {

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

    /// If true, data expressions are not rewritten.
    bool m_skip_data;

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
    /// \param enumerate_infinite_sorts Determines if quantifier variables of infinite sorts are enumerated
    /// \param skip_data If false, data expressions are also rewritten. If true,
    ///        only PBES expressions are rewritten, and the data expressions are not.
    enumerate_quantifiers_rewriter(const DataRewriter& r, const DataEnumerator& e, bool enumerate_infinite_sorts = true, bool skip_data = false)
      : m_rewriter(r), m_enumerator(e), m_enumerate_infinite_sorts(enumerate_infinite_sorts), m_skip_data(skip_data)
    {}

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    term_type operator()(const term_type& x) const
    {
      typedef data::mutable_map_substitution<std::map< variable_type, data_term_type> > substitution_function;
      typedef core::term_traits<term_type> tr;

      substitution_function sigma;
      detail::enumerate_quantifiers_builder<Term, DataRewriter, DataEnumerator, substitution_function> r(m_rewriter, m_enumerator, m_enumerate_infinite_sorts, m_skip_data);
      term_type result = r(x, sigma);

#ifdef MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
      mCRL2log(log::debug) << "<enumerate-quantifiers>" << pbes_system::pp(x) << " -> " << pbes_system::pp(result) << std::endl;
#endif
      return result;
    }

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \param sigma A substitution function
    /// \return The rewrite result.
    template <typename SubstitutionFunction>
    term_type operator()(const term_type& x, SubstitutionFunction sigma) const
    {
      typedef data::mutable_substitution_composer<SubstitutionFunction> substitution_function;
      typedef core::term_traits<term_type> tr;

      detail::enumerate_quantifiers_builder<Term, DataRewriter, DataEnumerator, substitution_function> r(m_rewriter, m_enumerator, m_enumerate_infinite_sorts, m_skip_data);
      term_type result = r(x, substitution_function(sigma));

#ifdef MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
      mCRL2log(log::debug) << "<enumerate-quantifiers>" << pbes_system::pp(x) << " -> " << pbes_system::pp(result) << data::print_substitution(sigma) << std::endl;
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

    /// If true, data expressions are not rewritten.
    bool m_skip_data;

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
    /// \param skip_data If false, data expressions are also rewritten. If true,
    ///        only PBES expressions are rewritten, and the data expressions are not.
    enumerate_quantifiers_rewriter(const DataRewriter& r, const DataEnumerator& e, bool enumerate_infinite_sorts = true, bool skip_data = false)
      : m_rewriter(r, e, enumerate_infinite_sorts, skip_data)
    {}

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    term_type operator()(const term_type& x) const
    {
      return core::static_down_cast<const pbes_expression&>(m_rewriter(pbes_expression_with_variables(x, data::variable_list())));
    }

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \param sigma A substitution function
    /// \return The rewrite result.
    template <typename SubstitutionFunction>
    term_type operator()(const term_type& x, SubstitutionFunction sigma) const
    {
      pbes_expression_with_variables x1(x, data::variable_list());

      m_rewriter(x1, sigma);

      pbes_expression_with_variables result = m_rewriter(x1, sigma);
      return result;
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H
