// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesrewr.h
/// \brief Applies a rewriter to a pbes.

#ifndef MCRL2_PBES_PBESREWR_H
#define MCRL2_PBES_PBESREWR_H

#include <algorithm>
#include "mcrl2/pbes/pbes.h"

namespace mcrl2 {

namespace pbes_system {

  /// \cond INTERNAL_DOCS
  template <typename PbesRewriter>
  struct rewrite_equation
  {
    PbesRewriter& pbesr;

    rewrite_equation(PbesRewriter& r)
      : pbesr(r)
    {}

    pbes_equation operator()(const pbes_equation& eq)
    {
      return pbes_equation(eq.symbol(), eq.variable(), pbesr(eq.formula()));
    }
  };
  /// \endcond

  /// \brief Applies a rewriter to a pbes.
  /// \param p A pbes
  /// \param pbesr A pbes rewriter
  /// \return The pbes obtained from \p p by applying the rewriter on all pbes expressions in \p p
  template <typename Container, typename PbesRewriter>
  void pbesrewr(pbes<Container>& p, PbesRewriter pbesr)
  {
    std::transform(p.equations().begin(), p.equations().end(), p.equations().begin(), rewrite_equation<PbesRewriter>(pbesr));
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESREWR_H
