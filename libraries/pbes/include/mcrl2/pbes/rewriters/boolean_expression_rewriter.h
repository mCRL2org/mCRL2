// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/boolean_expression_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_BOOLEAN_EXPRESSION_REWRITER_H
#define MCRL2_PBES_REWRITERS_BOOLEAN_EXPRESSION_REWRITER_H

#include "mcrl2/bes/detail/boolean_simplify_builder.h"

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
    term_type operator()(const term_type& x) const
    {
      bes::detail::boolean_simplify_builder<Term> r;
      return r(x);
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_BOOLEAN_EXPRESSION_REWRITER_H
