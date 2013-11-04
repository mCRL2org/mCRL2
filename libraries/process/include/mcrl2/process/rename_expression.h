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
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"

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
      : atermpp::aterm_appl(core::detail::constructRenameExpr())
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
      : atermpp::aterm_appl(core::detail::gsMakeRenameExpr(source, target))
    {}

    /// \brief Constructor.
    rename_expression(const std::string& source, const std::string& target)
      : atermpp::aterm_appl(core::detail::gsMakeRenameExpr(core::identifier_string(source), core::identifier_string(target)))
    {}

    const core::identifier_string& source() const
    {
      return atermpp::aterm_cast<const core::identifier_string>((*this)[0]);
    }

    const core::identifier_string& target() const
    {
      return atermpp::aterm_cast<const core::identifier_string>((*this)[1]);
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
  return core::detail::gsIsRenameExpr(x);
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
