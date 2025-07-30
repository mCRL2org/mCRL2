// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/fixpoint_symbol.h
/// \brief The class fixpoint_symbol.

#ifndef MCRL2_PBES_FIXPOINT_SYMBOL_H
#define MCRL2_PBES_FIXPOINT_SYMBOL_H

#include "mcrl2/data/data_specification.h"

namespace mcrl2::pbes_system
{

//--- start generated class fixpoint_symbol ---//
/// \\brief A fixpoint symbol
class fixpoint_symbol: public atermpp::aterm
{
  public:
    /// \\brief Default constructor X3.
    fixpoint_symbol()
      : atermpp::aterm(core::detail::default_values::FixPoint)
    {}

    /// \\brief Constructor Z9.
    /// \\param term A term
    explicit fixpoint_symbol(const atermpp::aterm& term)
      : atermpp::aterm(term)
    {
      assert(core::detail::check_rule_FixPoint(*this));
    }

    /// Move semantics
    fixpoint_symbol(const fixpoint_symbol&) noexcept = default;
    fixpoint_symbol(fixpoint_symbol&&) noexcept = default;
    fixpoint_symbol& operator=(const fixpoint_symbol&) noexcept = default;
    fixpoint_symbol& operator=(fixpoint_symbol&&) noexcept = default;
//--- start user section fixpoint_symbol ---//
    /// \brief Returns the mu symbol.
    /// \return The mu symbol.
    static fixpoint_symbol mu()
    {
      return fixpoint_symbol(atermpp::aterm(core::detail::function_symbol_Mu()));
    }

    /// \brief Returns the nu symbol.
    /// \return The nu symbol.
    static fixpoint_symbol nu()
    {
      return fixpoint_symbol(atermpp::aterm(core::detail::function_symbol_Nu()));
    }

    /// \brief Returns true if the symbol is mu.
    /// \return True if the symbol is mu.
    bool is_mu() const
    {
      return function() == core::detail::function_symbols::Mu;
    }

    /// \brief Returns true if the symbol is nu.
    /// \return True if the symbol is nu.
    bool is_nu() const
    {
      return function() == core::detail::function_symbols::Nu;
    }
//--- end user section fixpoint_symbol ---//
};

/// \\brief list of fixpoint_symbols
using fixpoint_symbol_list = atermpp::term_list<fixpoint_symbol>;

/// \\brief vector of fixpoint_symbols
using fixpoint_symbol_vector = std::vector<fixpoint_symbol>;

// prototype declaration
std::string pp(const fixpoint_symbol& x, bool precedence_aware = true);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const fixpoint_symbol& x)
{
  return out << pbes_system::pp(x);
}

/// \\brief swap overload
inline void swap(fixpoint_symbol& t1, fixpoint_symbol& t2) noexcept
{
  t1.swap(t2);
}
//--- end generated class fixpoint_symbol ---//

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_FIXPOINT_SYMBOL_H
