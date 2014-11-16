// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/abstraction.h
/// \brief The class abstraction.

#ifndef MCRL2_DATA_ABSTRACTION_H
#define MCRL2_DATA_ABSTRACTION_H

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/binder_type.h"
#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/detail/soundness_checks.h"

namespace mcrl2
{

namespace data
{

/// \brief An abstraction expression.
class abstraction: public data_expression
{
  public:
    /// \brief Default constructor.
    abstraction()
      : data_expression(core::detail::default_values::Binder)
    {}

    /// \brief Constructor.
    /// \param term A term
    abstraction(const atermpp::aterm& term)
      : data_expression(term)
    {
      assert(core::detail::check_term_Binder(*this));
    }

    /// \brief Constructor.
    abstraction(const binder_type& binding_operator, const variable_list& variables, const data_expression& body)
      : data_expression(atermpp::aterm_appl(core::detail::function_symbol_Binder(), binding_operator, variables, body))
    {}

    /// \brief Constructor.
    template <typename Container>
    abstraction(const binder_type& binding_operator, const Container& variables, const data_expression& body, typename atermpp::enable_if_container<Container, variable>::type* = 0)
      : data_expression(atermpp::aterm_appl(core::detail::function_symbol_Binder(), binding_operator, variable_list(variables.begin(), variables.end()), body))
    {}

    const binder_type& binding_operator() const
    {
      return atermpp::down_cast<const binder_type>((*this)[0]);
    }

    const variable_list& variables() const
    {
      return atermpp::down_cast<const variable_list>((*this)[1]);
    }

    const data_expression& body() const
    {
      return atermpp::down_cast<const data_expression>((*this)[2]);
    }
};

//--- start generated class abstraction ---//
// prototype declaration
std::string pp(const abstraction& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const abstraction& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(abstraction& t1, abstraction& t2)
{
  t1.swap(t2);
}
//--- end generated class abstraction ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ABSTRACTION_H

