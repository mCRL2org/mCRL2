// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/pfnf_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_PFNF_REWRITER_H
#define MCRL2_PBES_REWRITERS_PFNF_REWRITER_H

#include "mcrl2/pbes/detail/pfnf_traverser.h"



namespace mcrl2::pbes_system {

/// \brief A rewriter that brings PBES expressions into PFNF normal form.
class pfnf_rewriter
{
  public:
    /// \brief The term type
    using term_type = pbes_expression;

    /// \brief The variable type
    using variable_type = data::variable;

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    pbes_expression operator()(const pbes_expression& x) const
    {
      pbes_system::detail::pfnf_traverser visitor;
      visitor.apply(x);
      return visitor.evaluate();
    }

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \param sigma A substitution function
    /// \return The rewrite result.
    template <typename SubstitutionFunction>
    pbes_expression operator()(const pbes_expression& x, SubstitutionFunction sigma) const
    {
      return sigma(this->operator()(x));
    }
};

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_REWRITERS_PFNF_REWRITER_H
