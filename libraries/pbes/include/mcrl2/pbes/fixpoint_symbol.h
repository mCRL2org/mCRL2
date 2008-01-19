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

namespace lps {

using atermpp::aterm;
using atermpp::aterm_appl;
using namespace mcrl2::core::detail;

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
      : aterm_appl(mcrl2::core::detail::constructFixPoint())
    {}

    /// Constructor.
    ///
    fixpoint_symbol(aterm_appl t)
      : aterm_appl(t)
    {
      assert(mcrl2::core::detail::check_rule_FixPoint(m_term));
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
      return fixpoint_symbol(gsMakeMu());
    }

    /// Returns the nu symbol.
    ///
    static fixpoint_symbol nu()
    {
      return fixpoint_symbol(gsMakeNu());
    }

    /// Returns true if the symbol is mu.
    ///
    bool is_mu() const
    {
      return gsIsMu(*this);
    }

    /// Returns true if the symbol is nu.
    ///
    bool is_nu() const
    {
      return gsIsNu(*this);
    }
};

} // namespace lps

/// \cond INTERNAL_DOCS
namespace atermpp
{
using lps::fixpoint_symbol;

template<>
struct aterm_traits<fixpoint_symbol>
{
  typedef ATermAppl aterm_type;
  static void protect(lps::fixpoint_symbol t)   { t.protect(); }
  static void unprotect(lps::fixpoint_symbol t) { t.unprotect(); }
  static void mark(lps::fixpoint_symbol t)      { t.mark(); }
  static ATerm term(lps::fixpoint_symbol t)     { return t.term(); }
  static ATerm* ptr(lps::fixpoint_symbol& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_PBES_FIXPOINT_SYMBOL_H
