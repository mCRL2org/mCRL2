// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/communication_expression.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_COMMUNICATION_EXPRESSION_H
#define MCRL2_PROCESS_COMMUNICATION_EXPRESSION_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/process/action_name_multiset.h"

namespace mcrl2
{

namespace process
{

//--- start generated class communication_expression ---//
/// \brief A communication expression
class communication_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    communication_expression()
      : atermpp::aterm_appl(core::detail::default_values::CommExpr)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit communication_expression(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_CommExpr(*this));
    }

    /// \brief Constructor.
    communication_expression(const action_name_multiset& action_name, const core::identifier_string& name)
      : atermpp::aterm_appl(core::detail::function_symbol_CommExpr(), action_name, name)
    {}

    /// \brief Constructor.
    communication_expression(const action_name_multiset& action_name, const std::string& name)
      : atermpp::aterm_appl(core::detail::function_symbol_CommExpr(), action_name, core::identifier_string(name))
    {}

    const action_name_multiset& action_name() const
    {
      return atermpp::down_cast<action_name_multiset>((*this)[0]);
    }

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[1]);
    }
};

/// \brief list of communication_expressions
typedef atermpp::term_list<communication_expression> communication_expression_list;

/// \brief vector of communication_expressions
typedef std::vector<communication_expression>    communication_expression_vector;

/// \brief Test for a communication_expression expression
/// \param x A term
/// \return True if \a x is a communication_expression expression
inline
bool is_communication_expression(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::CommExpr;
}

// prototype declaration
std::string pp(const communication_expression& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const communication_expression& x)
{
  return out << process::pp(x);
}

/// \brief swap overload
inline void swap(communication_expression& t1, communication_expression& t2)
{
  t1.swap(t2);
}
//--- end generated class communication_expression ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_COMMUNICATION_EXPRESSION_H
