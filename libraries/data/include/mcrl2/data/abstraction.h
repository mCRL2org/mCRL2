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
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"

namespace mcrl2
{

namespace data
{

//--- start generated class abstraction ---//
/// \brief An abstraction expression.
class abstraction: public data_expression
{
  public:
    /// \brief Default constructor.
    abstraction()
      : data_expression(core::detail::constructBinder())
    {}

    /// \brief Constructor.
    /// \param term A term
    abstraction(const atermpp::aterm_appl& term)
      : data_expression(term)
    {
      assert(core::detail::check_term_Binder(m_term));
    }

    /// \brief Constructor.
    abstraction(const binder_type& binding_operator, const variable_list& variables, const data_expression& body)
      : data_expression(core::detail::gsMakeBinder(binding_operator, variables, body))
    {}

    /// \brief Constructor.
    template <typename Container>
    abstraction(const binder_type& binding_operator, const Container& variables, const data_expression& body, typename atermpp::detail::enable_if_container<Container, variable>::type* = 0)
      : data_expression(core::detail::gsMakeBinder(binding_operator, atermpp::convert<variable_list>(variables), body))
    {}

    const binder_type &binding_operator() const
    {
      return atermpp::aterm_cast<const binder_type>(atermpp::arg1(*this));
    }

    const variable_list &variables() const
    {
      return atermpp::aterm_cast<const variable_list>(atermpp::list_arg2(*this));
    }

    const data_expression &body() const
    {
      return atermpp::aterm_cast<const data_expression>(atermpp::arg3(*this));
    }
};
//--- end generated class abstraction ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ABSTRACTION_H

