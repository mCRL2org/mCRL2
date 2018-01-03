// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/untyped_data_parameter.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_UNTYPED_DATA_PARAMETER_H
#define MCRL2_DATA_UNTYPED_DATA_PARAMETER_H

#include "mcrl2/data/data_expression.h"

namespace mcrl2 {

namespace data {

//--- start generated class untyped_data_parameter ---//
/// \brief An untyped parameter
class untyped_data_parameter: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    untyped_data_parameter()
      : atermpp::aterm_appl(core::detail::default_values::UntypedDataParameter)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit untyped_data_parameter(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_UntypedDataParameter(*this));
    }

    /// \brief Constructor.
    untyped_data_parameter(const core::identifier_string& name, const data_expression_list& arguments)
      : atermpp::aterm_appl(core::detail::function_symbol_UntypedDataParameter(), name, arguments)
    {}

    /// \brief Constructor.
    untyped_data_parameter(const std::string& name, const data_expression_list& arguments)
      : atermpp::aterm_appl(core::detail::function_symbol_UntypedDataParameter(), core::identifier_string(name), arguments)
    {}

    /// Move semantics
    untyped_data_parameter(const untyped_data_parameter&) noexcept = default;
    untyped_data_parameter(untyped_data_parameter&&) noexcept = default;
    untyped_data_parameter& operator=(const untyped_data_parameter&) noexcept = default;
    untyped_data_parameter& operator=(untyped_data_parameter&&) noexcept = default;

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }

    const data_expression_list& arguments() const
    {
      return atermpp::down_cast<data_expression_list>((*this)[1]);
    }
};

/// \brief list of untyped_data_parameters
typedef atermpp::term_list<untyped_data_parameter> untyped_data_parameter_list;

/// \brief vector of untyped_data_parameters
typedef std::vector<untyped_data_parameter>    untyped_data_parameter_vector;

/// \brief Test for a untyped_data_parameter expression
/// \param x A term
/// \return True if \a x is a untyped_data_parameter expression
inline
bool is_untyped_data_parameter(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::UntypedDataParameter;
}

// prototype declaration
std::string pp(const untyped_data_parameter& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \param x Object x
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const untyped_data_parameter& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(untyped_data_parameter& t1, untyped_data_parameter& t2)
{
  t1.swap(t2);
}
//--- end generated class untyped_data_parameter ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_UNTYPED_DATA_PARAMETER_H
