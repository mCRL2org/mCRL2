// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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

namespace mcrl2 {

namespace pbes_system {

/// \brief A rewriter that brings PBES expressions into PFNF normal form.
class pfnf_rewriter
{
  public:
    /// \brief The term type
    typedef pbes_expression term_type;

    /// \brief The variable type
    typedef data::variable variable_type;

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    pbes_expression operator()(const pbes_expression& x) const
    {
      pbes_system::detail::pfnf_traverser visitor;
      visitor(x);
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

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_PFNF_REWRITER_H
