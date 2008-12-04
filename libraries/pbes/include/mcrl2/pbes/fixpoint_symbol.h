// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/fixpoint_symbol.h
/// \brief The class fixpoint_symbol.

#ifndef MCRL2_PBES_FIXPOINT_SYMBOL_H
#define MCRL2_PBES_FIXPOINT_SYMBOL_H

#include <cassert>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/detail/struct.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Pbes fixpoint symbol (mu or nu).
///
// <FixPoint>     ::= Mu
//                  | Nu
class fixpoint_symbol: public atermpp::aterm_appl
{
  public:
    /// \brief Constructor.
    fixpoint_symbol()
      : atermpp::aterm_appl(core::detail::constructFixPoint())
    {}

    /// \brief Constructor.
    fixpoint_symbol(atermpp::aterm_appl t)
      : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_rule_FixPoint(m_term));
    }

    /// \brief Assignment operator.
    fixpoint_symbol& operator=(atermpp::aterm t)
    {
      m_term = t;
      return *this;
    }

    /// \brief Returns the mu symbol.
    static fixpoint_symbol mu()
    {
      return fixpoint_symbol(core::detail::gsMakeMu());
    }

    /// \brief Returns the nu symbol.
    static fixpoint_symbol nu()
    {
      return fixpoint_symbol(core::detail::gsMakeNu());
    }

    /// \brief Returns true if the symbol is mu.
    bool is_mu() const
    {
      return core::detail::gsIsMu(*this);
    }

    /// \brief Returns true if the symbol is nu.
    bool is_nu() const
    {
      return core::detail::gsIsNu(*this);
    }
};

} // namespace pbes_system

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::pbes_system::fixpoint_symbol)
/// \endcond

#endif // MCRL2_PBES_FIXPOINT_SYMBOL_H
