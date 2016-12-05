// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/untyped_identifier.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_UNTYPED_IDENTIFIER_H
#define MCRL2_DATA_UNTYPED_IDENTIFIER_H

#include "mcrl2/data/data_expression.h"

namespace mcrl2 {

namespace data {

//--- start generated class untyped_identifier ---//
/// \brief An untyped identifier
class untyped_identifier: public data_expression
{
  public:
    /// \brief Default constructor.
    untyped_identifier()
      : data_expression(core::detail::default_values::UntypedIdentifier)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit untyped_identifier(const atermpp::aterm& term)
      : data_expression(term)
    {
      assert(core::detail::check_term_UntypedIdentifier(*this));
    }

    /// \brief Constructor.
    untyped_identifier(const core::identifier_string& name)
      : data_expression(atermpp::aterm_appl(core::detail::function_symbol_UntypedIdentifier(), name))
    {}

    /// \brief Constructor.
    untyped_identifier(const std::string& name)
      : data_expression(atermpp::aterm_appl(core::detail::function_symbol_UntypedIdentifier(), core::identifier_string(name)))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }
};

// prototype declaration
std::string pp(const untyped_identifier& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const untyped_identifier& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(untyped_identifier& t1, untyped_identifier& t2)
{
  t1.swap(t2);
}
//--- end generated class untyped_identifier ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_UNTYPED_IDENTIFIER_H
