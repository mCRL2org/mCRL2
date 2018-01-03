// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/rename_expression.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_RENAME_EXPRESSION_H
#define MCRL2_PROCESS_RENAME_EXPRESSION_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/identifier_string.h"

namespace mcrl2
{

namespace process
{

//--- start generated class rename_expression ---//
/// \brief A rename expression
class rename_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    rename_expression()
      : atermpp::aterm_appl(core::detail::default_values::RenameExpr)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit rename_expression(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_RenameExpr(*this));
    }

    /// \brief Constructor.
    rename_expression(core::identifier_string& source, core::identifier_string& target)
      : atermpp::aterm_appl(core::detail::function_symbol_RenameExpr(), source, target)
    {}

    /// \brief Constructor.
    rename_expression(const std::string& source, const std::string& target)
      : atermpp::aterm_appl(core::detail::function_symbol_RenameExpr(), core::identifier_string(source), core::identifier_string(target))
    {}

    /// Move semantics
    rename_expression(const rename_expression&) noexcept = default;
    rename_expression(rename_expression&&) noexcept = default;
    rename_expression& operator=(const rename_expression&) noexcept = default;
    rename_expression& operator=(rename_expression&&) noexcept = default;

    const core::identifier_string& source() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }

    const core::identifier_string& target() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[1]);
    }
};

/// \brief list of rename_expressions
typedef atermpp::term_list<rename_expression> rename_expression_list;

/// \brief vector of rename_expressions
typedef std::vector<rename_expression>    rename_expression_vector;

/// \brief Test for a rename_expression expression
/// \param x A term
/// \return True if \a x is a rename_expression expression
inline
bool is_rename_expression(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::RenameExpr;
}

// prototype declaration
std::string pp(const rename_expression& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const rename_expression& x)
{
  return out << process::pp(x);
}

/// \brief swap overload
inline void swap(rename_expression& t1, rename_expression& t2)
{
  t1.swap(t2);
}
//--- end generated class rename_expression ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_RENAME_EXPRESSION_H
