// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/data_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_DATA_REWRITER_H
#define MCRL2_PBES_REWRITERS_DATA_REWRITER_H

#include "mcrl2/pbes/detail/data_rewrite_builder.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief A rewriter that applies a data rewriter to data expressions in a term.
template <typename Term, typename DataRewriter>
class data_rewriter
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
    data_rewriter(const DataRewriter& rewriter)
      : m_rewriter(rewriter)
    {}

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    term_type operator()(const term_type& x) const
    {
      detail::data_rewrite_builder<Term, DataRewriter> r(m_rewriter);
      return r(x);
    }

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \param sigma A substitution function
    /// \return The rewrite result.
    template <typename SubstitutionFunction>
    term_type operator()(const term_type& x, SubstitutionFunction sigma) const
    {
      detail::data_rewrite_builder<Term, DataRewriter, SubstitutionFunction> r(m_rewriter);
      return r(x, sigma);
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_DATA_REWRITER_H
