// Author(s): Wieger Wesselink
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
#include "mcrl2/core/struct.h"

namespace mcrl2 {

namespace pbes_system {

using atermpp::aterm;
using atermpp::aterm_appl;

/// \brief Pbes fixpoint symbol (mu or nu).
///
// <FixPoint>     ::= Mu
//                  | Nu
class fixpoint_symbol: public aterm_appl
{
  public:
    /// Constructor.
    ///
    fixpoint_symbol()
      : aterm_appl(core::detail::constructFixPoint())
    {}

    /// Constructor.
    ///
    fixpoint_symbol(aterm_appl t)
      : aterm_appl(t)
    {
      assert(core::detail::check_rule_FixPoint(m_term));
    }
    
    /// Assignment operator.
    ///
    fixpoint_symbol& operator=(aterm t)
    {
      m_term = t;
      return *this;
    }

    /// Returns the mu symbol.
    ///
    static fixpoint_symbol mu()
    {
      return fixpoint_symbol(core::detail::gsMakeMu());
    }

    /// Returns the nu symbol.
    ///
    static fixpoint_symbol nu()
    {
      return fixpoint_symbol(core::detail::gsMakeNu());
    }

    /// Returns true if the symbol is mu.
    ///
    bool is_mu() const
    {
      return core::detail::gsIsMu(*this);
    }

    /// Returns true if the symbol is nu.
    ///
    bool is_nu() const
    {
      return core::detail::gsIsNu(*this);
    }
};

} // namespace pbes_system

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
using mcrl2::pbes_system::fixpoint_symbol;

template<>
struct aterm_traits<fixpoint_symbol>
{
  typedef ATermAppl aterm_type;
  static void protect(fixpoint_symbol t)   { t.protect(); }
  static void unprotect(fixpoint_symbol t) { t.unprotect(); }
  static void mark(fixpoint_symbol t)      { t.mark(); }
  static ATerm term(fixpoint_symbol t)     { return t.term(); }
  static ATerm* ptr(fixpoint_symbol& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_PBES_FIXPOINT_SYMBOL_H
